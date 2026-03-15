import os
import sys

if len(sys.argv) != 3:
    print("Usage: python makefsdata.py <input_dir> <output_file>")
    sys.exit(1)

input_dir = sys.argv[1]
output_file = sys.argv[2]

def byte_to_c_array(byte_data):
    # Convert bytes to comma-separated C values
    return ','.join(str(b) for b in byte_data)

files = []

for root, _, filenames in os.walk(input_dir):
    for name in filenames:
        path = os.path.join(root, name)
        key = os.path.relpath(path, input_dir).replace("\\", "/")
        with open(path, "rb") as f:   # <-- binary mode
            content = f.read()
        files.append((key, content))

with open(output_file, "w", encoding="utf-8") as f:
    f.write("#include <stdint.h>\n\n")
    f.write("typedef struct {\n")
    f.write("    const char *name;\n")
    f.write("    const uint8_t *content;\n")
    f.write("    unsigned int length;\n")
    f.write("} fs_file_t;\n\n")
    f.write(f"const fs_file_t fs_files[{len(files)}] = {{\n")
    for name, content in files:
        f.write(f'    {{"{name}", (const uint8_t[]){{{byte_to_c_array(content)}}}, {len(content)}}},\n')
    f.write("};\n")