# Haplous
The Haplous parser, seperated from libheb12 if needed.  
Adds benchmarks, and fixes a memory leak (missing free() in test).  

# Building
```
wget http://api.heb12.com/translations/haplous/kjv.txt
cc *.c; ./a.out
```
