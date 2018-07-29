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

int CFI_establish (CFI_cdesc_t *dv, void *base_addr, CFI_attribute_t attribute,
                   CFI_type_t type, size_t elem_len, CFI_rank_t rank,
                   const CFI_index_t extents[])
{
  /* C descriptor must not be NULL. */
  if (dv == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_establish: C descriptor is "
                       "NULL. (Error No. %d).\n",
               CFI_INVALID_DESCRIPTOR);
      return CFI_INVALID_DESCRIPTOR;
    }

  /* Rank must be between 0 and CFI_MAX_RANK. */
  if (rank < 0 || rank > CFI_MAX_RANK)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_establish: Rank must be "
                       "between 0 and %d, 0 < rank (0 !< %d). (Error No. "
                       "%d).\n",
               CFI_MAX_RANK, rank, CFI_INVALID_RANK);
      return CFI_INVALID_RANK;
    }

  /* C Descriptor must not be an allocated allocatable. */
  if (dv->attribute == CFI_attribute_allocatable && dv->base_addr != NULL)
    {
      fprintf (stderr,
               "ISO_Fortran_binding.c: CFI_establish: If the C Descriptor "
               "represents an allocatable variable (dv->attribute = %d), its "
               "base address must be NULL (dv->base_addr = NULL). (Error No. "
               "%d).\n",
               CFI_attribute_allocatable, CFI_INVALID_DESCRIPTOR);
      return CFI_INVALID_DESCRIPTOR;
    }

  /* If base address is not NULL, the established C Descriptor is for a
   * nonallocatable entity. */
  if (attribute == CFI_attribute_allocatable && base_addr != NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_establish: If base address "
                       "is not NULL (base_addr != NULL), the established C "
                       "Descriptor is for a nonallocatable entity (attribute "
                       "!= %d). (Error No. %d).\n",
               CFI_attribute_allocatable, CFI_INVALID_ATTRIBUTE);
      return CFI_INVALID_ATTRIBUTE;
    }

  dv->base_addr = base_addr;

  /* elem_len is only used if the item is not a type with a kind parameter. */
  if (type == CFI_type_char || type == CFI_type_ucs4_char ||
      type == CFI_type_signed_char || type == CFI_type_struct ||
      type == CFI_type_other)
    {
      dv->elem_len = elem_len;
    }
  else
    {
      /* base_type describes the intrinsic type with kind parameter. */
      size_t base_type = type & CFI_type_mask;
      /* base_type_size is the size in bytes of the variable as given by its
       * kind parameter. */
      size_t base_type_size = (type - base_type) >> CFI_type_kind_shift;
      /* Kind types 10 have a size of 64 bytes. */
      if (base_type_size == 10)
        {
          base_type_size = 64;
        }
      /* Complex numbers are twice the size of their real counterparts. */
      if (base_type == CFI_type_Complex)
        {
          base_type_size *= 2;
        }
      dv->elem_len = base_type_size;
    }

  dv->version   = CFI_VERSION;
  dv->rank      = rank;
  dv->attribute = attribute;
  dv->type      = type;

  /* Extents must not be NULL if rank is greater than zero and base_addr is not
   * NULL */
  if (rank > 0 && base_addr != NULL)
    {
      if (extents == NULL)
        {
          fprintf (stderr, "ISO_Fortran_binding.c: CFI_establish: Extents must "
                           "not be NULL (extents != NULL) if rank (= %d) > 0 "
                           "and base address is not NULL (base_addr != NULL). "
                           "(Error No. %d).\n",
                   rank, CFI_INVALID_EXTENT);
          return CFI_INVALID_EXTENT;
        }
      for (int i = 0; i < rank; i++)
        {
          /* If the C Descriptor is for a pointer then the lower bounds of every
           * dimension are set to zero. */
          if (attribute == CFI_attribute_pointer)
            {
              dv->dim[i].lower_bound = 0;
            }
          else
            {
              dv->dim[i].lower_bound = 1;
            }
          dv->dim[i].extent = extents[i];
          dv->dim[i].sm     = dv->elem_len;
        }
    }

  return CFI_SUCCESS;
}
