import sys, struct, os

# Print information about .spk file using python

def hexprint(by):
    for b in by:
        sys.stdout.write("%02X " % ord(b))

if len(sys.argv) < 2:
    print("Usage: info.py <file>")
    exit(1)
f = open(sys.argv[1], "rb")
sys.stdout.write("Header: ")
hdr = f.read(5)
hexprint(hdr)
print("")
if hdr[0] != 'S' or hdr[1] != 'P' or hdr[2] != 'K' or ord(hdr[3]) != 255 or ord(hdr[4]) != 0:
    print("Not an .spk file")
    exit(1)
    
numfiles = 0
numdirs = 0
numlinks = 0
biggestfile = ("", -1)
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

    f.read(2) # Mode
    f.read(2) # UID
    f.read(2) # GID

    sys.stdout.write("Length: ")
    b = f.read(4)
    hexprint(b)
    print("= %s" % struct.unpack("=I", b))
    length = struct.unpack("=I", b)[0]

    if ty == 1: # File
        numfiles += 1
    elif ty == 2: # Directory
        numdirs += 1
    elif ty == 3:
        numlinks += 1
    else:
        print("Unknown type " + str(ty))
        exit(1)
    if length > biggestfile[1]:
        biggestfile = (p, length)
    f.read(length)

print("")
print("%d files, %d directories, %d symlinks" % (numfiles, numdirs, numlinks))
print("Biggest file '%s' with %ld bytes" % biggestfile)    
