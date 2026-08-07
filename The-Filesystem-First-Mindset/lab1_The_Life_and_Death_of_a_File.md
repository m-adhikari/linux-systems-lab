# The Life and Death of a File

**Objective:** To diagnose what happens when file is created and deleted.

**Steps:** 

- created a workspace with `mkdir testfs` and `touch testfs/hello.txt`; added some text using `echo "hello" > testfs/hello.txt`

- Recorded `hello.txt`'s inode using `ls -i testfs/hello.txt` (Output: `566679 testfs/hello.txt`)

- Finding the the partition where the file is stored using `lsblk -f`

- entering `sudo debugfs /dev/mmcblk0p2` and getting metadata using `stat <566679>`

- Read the metadata; `EXTENTS: (0):2142746` recorded the block. 

- Using `block_dump 2142746`

```text
OUTPUT:

0000  6865 6c6c 6f0a 0000 0000 0000 0000 0000  hello...........
0020  0000 0000 0000 0000 0000 0000 0000 0000  ................
*
```

This is the content stored in the file, we checked directly by extracting from the persistent storage. 

**Observations:**

- Observation 1: modifying the content of the file changes the inode. 
```
Discussion: This may or may not be true. File was modified using vim. A lot of file editors may relocate files. It may have created a temporary file and renamed it to be hello.txt.
```
- Observation 2: moving the file using `mv hello.txt /testfs-2` doesn't change inode. 

- Observation 3: redoing the steps to do a `block_dump` after deletion resulted in no change. We can still infer the content. 

**Conclusion:**
A file is like land. Moving is ownership change; technically, data doesn't move. Modifying may change location due to relocation. Renaming just changes how directory maintains inode-filename key-value. Deletion just deletes directory entry, the data doesn't cease to exist. 





