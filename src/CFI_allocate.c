/*
   OpenCoarrays is distributed under the OSI-approved BSD 3-clause License:
   OpenCoarrays -- ISO_Fortran_binding standard-compliant interoperability with
   C.
   Copyright (c) 2018, Sourcery, Inc.
   Copyright (c) 2018, Sourcery Institute
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
   3. Neither the names of the copyright holders nor the names of their
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
 */

#include "ISO_Fortran_binding.h"
#include <stdio.h>
#include <stdlib.h>

int CFI_allocate (CFI_cdesc_t *dv, const CFI_index_t lower_bounds[],
                  const CFI_index_t upper_bounds[], size_t elem_len)
{
  /* C Descriptor must not be NULL. */
  if (dv == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_allocate: C Descriptor is "
                       "NULL. (Error No. %d).\n",
               CFI_INVALID_DESCRIPTOR);
      return CFI_INVALID_DESCRIPTOR;
    }

  /* Base address of C Descriptor must be NULL. */
  if (dv->base_addr != NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_allocate: Base address of C "
                       "Descriptor must be NULL. (Error No. %d).\n",
               CFI_ERROR_BASE_ADDR_NOT_NULL);
      return CFI_ERROR_BASE_ADDR_NOT_NULL;
    }

  /* The C Descriptor must be for an allocatable or pointer object. */
  if (dv->attribute == CFI_attribute_other)
    {
      fprintf (stderr,
               "ISO_Fortran_binding.c: CFI_allocate: The object of the C "
               "Descriptor must be a pointer or allocatable variable. "
               "(Error No. %d).\n",
               CFI_INVALID_ATTRIBUTE);
      return CFI_INVALID_ATTRIBUTE;
    }

  /* If the type is a character, the descriptor's element length is replaced
   * by the elem_len argument. */
  if (dv->type == CFI_type_char || dv->type == CFI_type_ucs4_char ||
      dv->type == CFI_type_signed_char)
    {
      dv->elem_len = elem_len;
    }

  /* Dimension information and calculating the array length. */
  size_t arr_len = 1;
  /* If rank is greater than 0, lower_bounds and upper_bounds are used. They're
   * ignored otherwhise. */
  if (dv->rank > 0)
    {
      if (lower_bounds == NULL || upper_bounds == NULL)
        {
          fprintf (stderr, "ISO_Fortran_binding.c: CFI_allocate: If 0 < rank "
                           "(= %d) upper_bounds[] and lower_bounds[], must not "
                           "be NULL. (Error No. %d).\n",
                   dv->rank, CFI_INVALID_EXTENT);
          return CFI_INVALID_EXTENT;
        }
      for (int i = 0; i < dv->rank; i++)
        {
          dv->dim[i].lower_bound = lower_bounds[i];
          dv->dim[i].extent      = upper_bounds[i] - dv->dim[i].lower_bound + 1;
          dv->dim[i].sm          = dv->elem_len;
          arr_len *= dv->dim[i].extent;
        }
    }

  dv->base_addr = calloc (arr_len, dv->elem_len);
  if (dv->base_addr == NULL)
    {
      printf ("ISO_Fortran_binding.c: CFI_allocate: Failure in memory "
              "allocation. (Error no. %d).\n",
              CFI_ERROR_MEM_ALLOCATION);
      return CFI_ERROR_MEM_ALLOCATION;
    }

  return CFI_SUCCESS;
}
