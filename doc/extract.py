import sys, struct, os

# Extract .spk file using python

def hexprint(by):
    for b in by:
        sys.stdout.write("%02X " % ord(b))

if len(sys.argv) < 2:
    print("Usage: extract.py <file>")
    exit(1)
f = open(sys.argv[1], "rb")
sys.stdout.write("Header: ")
hdr = f.read(5)
hexprint(hdr)
print("")
if hdr[0] != 'S' or hdr[1] != 'P' or hdr[2] != 'K' or ord(hdr[3]) != 255 or ord(hdr[4]) != 0:
    print("Not an .spk file")
    exit(1)
while True:
    p = f.read(255)
    if len(p) == 0:
        break
    p = p[:p.find(chr(0))] # Remove null bytes
    print("Path: %s" % p)

    sys.stdout.write("Type: ")
    b = f.read(1)
    hexprint(b)
    print("= %s" % struct.unpack("=B", b))
    ty = struct.unpack("=B", b)[0]

    sys.stdout.write("Mode: ")
    b = f.read(2)
    hexprint(b)
    print("= 0o%o" % struct.unpack("=H", b))
    mode = struct.unpack("=H", b)[0]

    sys.stdout.write("UID: ")
    b = f.read(2)
    hexprint(b)
    print("= %s" % struct.unpack("=H", b))
    uid = struct.unpack("=H", b)[0]

    sys.stdout.write("GID: ")
    b = f.read(2)
    hexprint(b)
    print("= %s" % struct.unpack("=H", b))
    gid = struct.unpack("=H", b)[0]

    sys.stdout.write("Length: ")
    b = f.read(4)
    hexprint(b)
    print("= %s" % struct.unpack("=I", b))
    length = struct.unpack("=I", b)[0]

    if ty == 1: # File
        of = open(p, "wb")
        of.write(f.read(length))
        of.close()
    elif ty == 2: # Directory
        os.mkdir(p)
    else:
        print("Unknown type " + str(ty))
        exit(1)
    if gid != 65535:
        os.chown(p, uid, gid)
    if mode != 65535:
        os.chmod(p, mode)
