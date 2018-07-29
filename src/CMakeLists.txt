add_library( ISO_Fortran_binding STATIC 
  CFI_address.c
  CFI_allocate.c
  CFI_deallocate.c
  CFI_establish.c
  CFI_is_contiguous.c
  CFI_section.c
  CFI_select_part.c
  CFI_setpointer.c
)
install(TARGETS ISO_Fortran_binding DESTINATION lib)
