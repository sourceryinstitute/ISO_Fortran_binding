set( unit_tests
  CFI_establish
  CFI_deallocate
  CFI_is_contiguous
  CFI_address
  CFI_setpointer
  CFI_section
  CFI_select_part
)
foreach(unit_test ${unit_tests})
  add_executable( "test-${unit_test}" "test-${unit_test}.c" )
  target_link_libraries( test-${unit_test} ISO_Fortran_binding )
endforeach()
