# The Filesystem-First Mindset

**Problem Statement:** How can an operating system organize billions of objects?

## Why does a comptuer need file system?

A filesystem is the method used by an OS to store, organize, retrive and manage data on storage devices. Filesystem gives a layer of context to raw bytes. 

Memory Disappears: 
---

Running programs loaded into RAM disappears, when DRAM capacitor loses its charge. 

Every linux process owns a virtual address space; examples: stack, heap, mmap(), shared libraries, program code. Wehn the process exits: kernel destroys process and releases memory. Persistent storage is needed to make data survive. 

Linux optimizes disk performance using page cache. Even after saving, the data may temporarily live only in RAM until kernel flushes dirty pages. `sync` or `fsync()` force disk sync.

Storage Persists:
---

Storage devices are persistent, even without power. Raw storage without file system is just blocks of bytes. A file system provides filenames, hierarchy, ownership, maintains meta-data, location, etc.  

`mkfs.ext4 /dev/sdb1` formats a partition with ext4 filesystem. This creates:
- Superblock: main information record (type, size, number of blocks, inodes, filesystem status, location of important structures)
- block groups: ext4 divides disk into smallar sections.
- inode tables: stores permissions, owner, file size, timestamps, location of the file's data blocks
- bitmaps: tracks free and used spaces
- journal: prevents corruption, linux checks the journal and recovers if interruptions to on-going task occur


Organization:
---

ext4 Organization

```text
+----------------+
|   Boot Block   |
+----------------+
|   Superblock   |
+----------------+
| Group Desc.    |
+----------------+
| Block Bitmap   |
+----------------+
| Inode Bitmap   |
+----------------+
|  Inode Table   |
+----------------+
|  Data Blocks   |
+----------------+
```

`sudo dumpe2fs /dev/sda1` to view super block if booted from sd card (usually).

`lsblk -f` to figure out file system. In our case `mmcblk0p2` uses ext4 so `sudo dumpe2fs /dev/mmcblk0p2`

Files:
---

|symbol|type|example|
|--------|---- |-------|
| - | Regular File | Text, program, image|
|d | directory| folder|
|b | block device| Hard drive, SSD|
|c | character device| terminal, keyboard|
|l | symbolic link | shortcut |
|p | pipe | communication channel|
|s | socket| network/process communication|

Exercise:

```
-rw-r--r--  1 user user  1200 Jul 30  file.txt
drwxr-xr-x  2 user user  4096 Jul 30  Documents
brw-rw----  1 root disk  8,0  Jul 30  sda
crw-rw-rw-  1 root tty   4,64 Jul 30  tty0
lrwxrwxrwx  1 user user     7 Jul 30  link -> file.txt
```
Solution:

```
-rw-r--r--  1 user user  1200 Jul 30  file.txt
It is regular file, owner can read and write, group can read, others can read. user created it, user is the group, 1200 is the size, Jul 30 is the date of last modification, file.txt is the name.
```
- To create it: `touch file.txt`
- To add content: `echo "Hello" > file.txt`
- To change permissions: `chmod 644 file.txt`
- To verify: `ls -l file.txt`
---
Note:

Permissions are represented by three groups: owner (u), group (g), others (o)
|Permission|Symbol|Numeric value| Purpose|
------|-----|-------|---------
read|r|4|view file contents/ list directory contents
write|w|2| modify file contents/ create-delete files in a directory
execute|x|1| run a file/ enter a directory

_numeric permission is calculated by adding values._

Basic chmod:

- `chmod 755 script.sh` makes the file `-rwxr-xr-x`
- `chmod +x script.sh` add execute permission
- `chmod o-w file.txt` removes write permission from others
- `chmod g+w file.txt` gives group write permissions 

Naming Object:
---

```mermaid
flowchart LR
    A[Filename]
    --> B[Directory Entry]
    --> C[Inode Number]
    --> D[Inode]
    --> E[Data Blocks]
```
A filename is stored in directory it belongs to, which map filename to inode number. 

Everything managed by filesystem in an object. Every filesystem object has:
- data (if applicable)
- metadata 
- an inode (in ext4)

An inode uniqely identifies a filesystem object. It stores metadata such as:
- permissions
- owner
- group
- timestamp
- file size
- pointers to data blocks

_Note:_
- To find inode of a file use `ls -i filename`.
- To view metadata use `stat filename`
- To dump the raw inode: use `sudo debugfs /dev/mmcblk0p2` then use `stat <inode number>`

Identity:
---

Identity begins with the Inode. 

Example:
```text
Inode: 141960   Type: regular    Mode:  0644   Flags: 0x80000
Generation: 333427436    Version: 0x00000000:00000002
User:  1000   Group:  1000   Project:     0   Size: 397
File ACL: 0
Links: 1   Blockcount: 8
Fragment:  Address: 0    Number: 0    Size: 0
 ctime: 0x6a512f86:d87e87f0 -- Fri Jul 10 12:44:38 2026
 atime: 0x6a512f86:d5a21e28 -- Fri Jul 10 12:44:38 2026
 mtime: 0x6a512f86:d5a21e28 -- Fri Jul 10 12:44:38 2026
crtime: 0x6a512f86:d5a21e28 -- Fri Jul 10 12:44:38 2026
Size of extra inode fields: 32
Inode checksum: 0xb67583ca
EXTENTS:
(0):3952274
```
How to read Inode:

Field|Meaning
------|-------
Inode| Inode Number
Type | Gives type of file regular, directory, etc.
Mode | File permission, ugo numeric file permissions
Flags| 0x80000 corresponds to EXT4_EXTENT_FL, meaning file uses extent-based allocation.
Generation| used by NFS to detect stale file handles
Version | version counter / inode change
Ownership | UID/GID, 1000 corresponds to first normal user 
Size | gives file size
Links | no. of directory pointer to this inode
Blockcount | ext4 Blockcount is measured in 512-byte sectors; despite file is 397B, it occupies 4096 bytes typically.
Times| {(atime, last access), (mtime, last modification), (ctime, last inode metadata change), (crtime, file creation time (ext4))}
Extra inode fields | modern ext4 stores additional metadata beyond the original inode structure
Checksum | used by ext4 to detect corruption
Extents | Example: (0):3952274; logical file block is 0, maps to physical disk block 3952274

```
Extent-based allocation: 
File is stored as a small number of contiguous blocks, called extents. Extent is represented as: starting block address, length. Example: File needs 100 blocks; instead of storing 100 separate block pointers, file system stores start block and length. 
```

There is stable identity, becuase renaming, changing locations, etc doesn't chage the inode. What changes when renamed is filename that points to inode, and deleting means the directory doesn't maintain key-value for that filename and inode. Multiple filenames can point one single object.
