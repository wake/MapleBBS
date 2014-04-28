MapleBBS
========

Source code of MapleBBS-3.10-20050817-PACK.tgz

Evnironment for MapleBBS under 64bit OS (ex: CentOS)
----------------------------------------------------

1. Install gcc 3 compat

```yum install compat-gcc-34 -y```

2. Install glibc-devel

Error message : `gnu/stubs-32.h: No such file or directory`

```yum install glibc-devel.i686```

3.  Install libgcc.i686

Error message : `/usr/bin/ld: cannot find -lgcc_s_32`

```yum install libgcc.i686 -y```