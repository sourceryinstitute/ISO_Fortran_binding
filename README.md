<a name="top"> </a>

[This document is formatted with GitHub-Flavored Markdown.              ]:#
[For better viewing, including hyperlinks, read it online at            ]:#
[https://github.com/sourceryinstitute/OpenCoarrays/blob/master/README.md]:#
<div align="center">

[![Sourcery Institute][sourcery-institute logo]][Sourcery Institute]


ISO_Fortran_binding
===================

[About](#about-this-library) | [Installing](#build-test-and-install) | [Contributing](#contributing) | [Known Issues](#known-issues)

</div>

About this library
------------------
ISO_Fortran_binding supports C/Fortran interoperability by providing the [Fortran 2018] `ISO_Fortran_binding.h` C header file, corresponding function definitions, and CMake scripts for building, testing, and installing static and dynamic libraries. C programs may use the provided function to manipulate interoperable Fortran data objects by exploiting metadata describing the type, rank, extents, contiguity, and other information about the object.  

* `CFI_allocate`, `CFI_deallocate`. Allocate or deallocate Fortran allocatable variables. 🐏 
* `CFI_establish`. Establish the metadata necessary to access Fortran variables inside a C descriptor `struct`.
* `CFI_address`. If a C descriptor describes a scalar variable, find its address. If it describes an array, find the address of the element with column-major (Fortran ordering) indexing. :house:
* `CFI_is_contiguous`. Determine whether a Fortran array is contiguous.
* `CFI_select_part`. Update a C descriptor to select an element or contiguous elements of a Fortran derived type. The variable whose part is selected could be an array section of derived type. 🚣‍ 
* `CFI_section`. Update a C descriptor to select a section of an array. The array can be of interoperable derived types. The array can also be composed of elements of a derived type, i.e., a C descriptor updated by `CFI_select_part`. The array can also be a section.
* `CFI_setpointer`: Update a C descriptor to point at a Fortran variable. 🏹 

[Fortran 2018]: https://j3-fortran.org/doc/year/18/18-007r1.pdf


Build, test, and install
------------------------ 
```
cd ISO_Fortran_binding 
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<insert-install-path-here>
make
ctest
make install
```

Contributing
------------
* Please submit questions, suggestions, or bug reports via our Issues page.
* Prior to submitting code via pull requests after 
    1. Signing our [Contributor License Agreemnt].
    2. Reviewing the [GitHub Flow] workflow.

Known issues
------------
This repository uses GNU C language extensions that are not supported by the clang compiler.


[About]: #about-this-library
[Installing]: #build-test-and-install
[Contributing]: #contributing
[Known Issues]: #known-issues
[Contributor License Agreemnt]: https://cla-assistant.io/sourceryinstitute/ISO_Fortran_binding
[GitHub Flow]: https://guides.github.com/introduction/flow/

[sourcery-institute logo]: http://www.sourceryinstitute.org/uploads/4/9/9/6/49967347/sourcery-logo-rgb-hi-rez-1.png
[Sourcery Institute]: http://www.sourceryinstitute.org

