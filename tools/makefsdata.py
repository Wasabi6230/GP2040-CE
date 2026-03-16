import os
import sys

output = sys.argv[2] if len(sys.argv) > 2 else "src/wifi/fsdata.c"
folder = sys.argv[1] if len(sys.argv) > 1 else "www"

files = []

def escape_bytes(data):
    return ", ".join(str(b) for b in data)

for root, dirs, filenames in os.walk(folder):
    for f in filenames:
        path = os.path.join(root, f)
        rel_path = os.path.relpath(path, folder).replace("\\", "/")
        with open(path, "rb") as file:
            content = file.read()
        files.append((rel_path, content))

with open(output, "w", encoding="utf-8") as f:
    f.write('#include "lwip/apps/fs.h"\n\n')
    f.write('/* Auto-generated fsdata.c */\n\n')

    prev_name = None
    prev_file = None

    for name, data in files:
        name_var = f"file_{name.replace('/', '_').replace('.', '_')}_name"
        file_var = f"file_{name.replace('/', '_').replace('.', '_')}_data"

        # Write file name
        f.write(f'static const unsigned char {name_var}[] = "{name}";\n')

        # Write file content
        f.write(f'static const unsigned char {file_var}[] = {{ {escape_bytes(data)} }};\n')

        # Write fsdata_file struct
        f.write(f'static const struct fsdata_file {file_var}_struct = {{\n')
        f.write(f'    {prev_file},\n' if prev_file else '    NULL,\n')
        f.write(f'    {name_var}, {file_var}, {len(data)},\n')
        f.write('    FS_FILE_FLAGS_HEADER_INCLUDED\n')
        f.write('};\n\n')

        prev_file = f"{file_var}_struct"

    # Set FS_ROOT
    f.write(f'const struct fsdata_file *FS_ROOT = {prev_file};\n')