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

int CFI_select_part (CFI_cdesc_t *result, const CFI_cdesc_t *source,
                     size_t displacement, size_t elem_len)
{
  /* C Descriptors must not be NULL. */
  if (source == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_select_part: Source must "
                       "not be NULL. (Error No. %d).\n",
               CFI_INVALID_DESCRIPTOR);
      return CFI_INVALID_DESCRIPTOR;
    }
  if (result == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_select_part: Result must "
                       "not be NULL. (Error No. %d).\n",
               CFI_INVALID_DESCRIPTOR);
      return CFI_INVALID_DESCRIPTOR;
    }

  /* Attribute of result will be CFI_attribute_other or CFI_attribute_pointer.
   */
  if (result->attribute == CFI_attribute_allocatable)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_select_part: Result must "
                       "not describe an allocatabale object (result->attribute "
                       "!= %d). (Error No. %d).\n",
               CFI_attribute_allocatable, CFI_INVALID_ATTRIBUTE);
      return CFI_INVALID_ATTRIBUTE;
    }

  /* Base address of source must not be NULL. */
  if (source->base_addr == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_select_part: Base address "
                       "of source must not be NULL. (Error No. %d).\n",
               CFI_ERROR_BASE_ADDR_NULL);
      return CFI_ERROR_BASE_ADDR_NULL;
    }

  /* Source and result must have the same rank. */
  if (source->rank != result->rank)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_select_part: Source and "
                       "result must have the same rank (source->rank = %d, "
                       "result->rank = %d). (Error No. %d).\n",
               source->rank, result->rank, CFI_INVALID_RANK);
      return CFI_INVALID_RANK;
    }

  /* Nonallocatable nonpointer must not be an assumed size array. */
  if (source->rank > 0 && source->dim[source->rank - 1].extent == -1)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_select_part: Source must "
                       "not describe an assumed size array "
                       "(source->dim[%d].extent != -1). (Error No. %d).\n",
               source->rank - 1, CFI_INVALID_DESCRIPTOR);
      return CFI_INVALID_DESCRIPTOR;
    }

  /* Element length. */
  if (result->type == CFI_type_char || result->type == CFI_type_ucs4_char ||
      result->type == CFI_type_signed_char)
    {
      result->elem_len = elem_len;
    }

  /* Ensure displacement is within the bounds of the element length of source.
   */
  if (displacement > source->elem_len - 1)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_select_part: Displacement "
                       "must be within the bounds of source (0 <= displacement "
                       "<= source->elem_len - 1, 0 <= %ld <= %ld). (Error No. "
                       "%d).\n",
               displacement, source->elem_len - 1, CFI_ERROR_OUT_OF_BOUNDS);
      return CFI_ERROR_OUT_OF_BOUNDS;
    }
  /* Ensure displacement and element length of result are less than or equal to
   * the element length of source. */
  if (displacement + result->elem_len > source->elem_len)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_select_part: Displacement "
                       "plus the element length of result must be less than or "
                       "equal to the element length of source (displacement + "
                       "result->elem_len <= source->elem_len, %ld + %ld = %ld "
                       "<= %ld). (Error No. %d).\n",
               displacement, result->elem_len, displacement + result->elem_len,
               source->elem_len, CFI_ERROR_OUT_OF_BOUNDS);
      return CFI_ERROR_OUT_OF_BOUNDS;
    }
  if (result->rank > 0)
    {
      for (int i = 0; i < result->rank; i++)
        {
          result->dim[i].lower_bound = source->dim[i].lower_bound;
          result->dim[i].extent      = source->dim[i].extent;
          result->dim[i].sm =
              source->dim[i].sm +
              displacement * (source->dim[i].sm / source->elem_len - 1);
        }
    }

  result->base_addr = (char *) source->base_addr + displacement;
  return CFI_SUCCESS;
}
