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

int CFI_is_contiguous (const CFI_cdesc_t *dv)
{
  /* C descriptor must not be NULL. */
  if (dv == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_is_contiguous: C descriptor "
                       "is NULL. (Error No. %d).\n",
               CFI_INVALID_DESCRIPTOR);
      return CFI_INVALID_DESCRIPTOR;
    }

  /* Base address must not be NULL. */
  if (dv->base_addr == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_is_contiguous: Base address "
                       "of C Descriptor is already NULL. (Error No. %d).\n",
               CFI_ERROR_BASE_ADDR_NULL);
      return CFI_ERROR_BASE_ADDR_NULL;
    }

  /* Must be an array. */
  if (dv->rank == 0)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_is_contiguous: C Descriptor "
                       "must describe an array (0 < dv->rank = %d). (Error No. "
                       "%d).\n",
               dv->rank, CFI_INVALID_RANK);
      return CFI_INVALID_RANK;
    }

  /* If an array is not contiguous the memory stride is different to the element
   * length. */
  for (int i = 0; i < dv->rank; i++)
    {
      if (dv->dim[i].sm != dv->elem_len)
        return CFI_FAILURE;
    }

  /* Allocatable arrays are always contiguous. */
  if (dv->attribute == CFI_attribute_allocatable)
    {
      return CFI_SUCCESS;
    }
  else
    {
      return CFI_FAILURE;
    }
}
