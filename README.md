<a name="top"> </a>

[This document is formatted with GitHub-Flavored Markdown.              ]:#
[For better viewing, including hyperlinks, read it online at            ]:#
[https://github.com/sourceryinstitute/OpenCoarrays/blob/master/README.md]:#
<div align="center">

[![Sourcery Institute][sourcery-institute logo]][Sourcery, Inc.]


ISO_Fortran_binding
===================

[About this library](#about-this-library) | [Build, test, and instal](#build-test-and-install) 

</div>

About this library
------------------
ISO_Fortran_binding supports C/Fortran interoperability by providing the [Fortran 2018 Draft International Standard]'s `ISO_Fortran_binding.h` C header file, corresponding function definitions, and CMake scripts for building, testing, and installing static and dynamic libraries. C programs may use the provided function to manipulate interoperable Fortran data objects by exploiting metadata describing the type, rank, extents, contiguity, and other information about the object.  

* `CFI_allocate`, `CFI_deallocate`. Allocate or deallocate Fortran allocatable variables. 🐏 
* `CFI_establish`. Establish the metadata necessary to access Fortran variables inside a C descriptor `struct`.
* `CFI_address`. If a C descriptor describes a scalar variable, find its address. If it describes an array, find the address of the element with column-major (Fortran ordering) indexing. :house:
* `CFI_is_contiguous`. Determine whether a Fortran array is contiguous.
* `CFI_select_part`. Update a C descriptor to select an element or contiguous elements of a Fortran derived type. The variable whose part is selected could be an array section of derived type. 🚣‍ 
* `CFI_section`. Update a C descriptor to select a section of an array. The array can be of interoperable derived types. The array can also be composed of elements of a derived type, i.e., a C descriptor updated by `CFI_select_part`. The array can also be a section.
* `CFI_setpointer`: Update a C descriptor to point at a Fortran variable. 🏹 

[Fortran 2018 Draft International Standard]: http://isotc.iso.org/livelink/livelink?func=ll&objId=19442438&objAction=Open&viewType=1


Build, test, and install
------------------------ 
```
cd ISO_Fortran_binding 
mkdir build
cd build
CC=<insert-C-compiler-name> FC=<insert-Fortran-compiler-name> cmake .. -DCMAKE_INSTALL_PREFIX=<insert-install-location-here>
make
ctest
make install
```

[sourcery-institute logo]: http://www.sourceryinstitute.org/uploads/4/9/9/6/49967347/sourcery-logo-rgb-hi-rez-1.png
