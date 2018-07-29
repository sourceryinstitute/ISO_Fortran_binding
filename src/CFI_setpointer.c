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

int CFI_setpointer (CFI_cdesc_t *result, CFI_cdesc_t *source,
                    const CFI_index_t lower_bounds[])
{
  /* Result must not be NULL. */
  if (result == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_setpointer: Result is NULL. "
                       "(Error No. %d).\n",
               CFI_INVALID_DESCRIPTOR);
      return CFI_INVALID_DESCRIPTOR;
    }

  /* If source is NULL, the result is a C Descriptor that describes a
   * disassociated pointer. */
  if (source == NULL)
    {
      result->base_addr = NULL;
      result->version   = CFI_VERSION;
      result->attribute = CFI_attribute_pointer;
    }
  else
    {
      /* Check that element lengths, ranks and types of source and result are
       * the same. */
      if (result->elem_len != source->elem_len)
        {
          fprintf (stderr, "ISO_Fortran_binding.c: CFI_setpointer: Element "
                           "lengths of result (result->elem_len = %ld) and "
                           "source (source->elem_len = %ld) must be the same. "
                           "(Error No. %d).\n",
                   result->elem_len, source->elem_len, CFI_INVALID_ELEM_LEN);
          return CFI_INVALID_ELEM_LEN;
        }

      if (result->rank != source->rank)
        {
          fprintf (stderr, "ISO_Fortran_binding.c: CFI_setpointer: Ranks of "
                           "result (result->rank = %d) and source "
                           "(source->rank = %d) must be the same. (Error "
                           "No. %d).\n",
                   result->rank, source->rank, CFI_INVALID_RANK);
          return CFI_INVALID_RANK;
        }

      if (result->type != source->type)
        {
          fprintf (stderr, "ISO_Fortran_binding.c: CFI_setpointer: Types of "
                           "result (result->type = %d) and source "
                           "(source->type = %d) must be the same. (Error "
                           "No. %d).\n",
                   result->type, source->type, CFI_INVALID_TYPE);
          return CFI_INVALID_TYPE;
        }

      /* If the source is a disassociated pointer, the result must also describe
       * a disassociated pointer. */
      if (source->base_addr == NULL &&
          source->attribute == CFI_attribute_pointer)
        {
          result->base_addr = NULL;
        }
      else
        {
          result->base_addr = source->base_addr;
        }
      /* Assign components to result. */
      result->version   = source->version;
      result->attribute = source->attribute;

      /* Dimension information. */
      for (int i = 0; i < source->rank; i++)
        {
          if (lower_bounds != NULL)
            {
              result->dim[i].lower_bound = lower_bounds[i];
            }
          else
            {
              result->dim[i].lower_bound = source->dim[i].lower_bound;
            }
          result->dim[i].extent = source->dim[i].extent;
          result->dim[i].sm     = source->dim[i].sm;
        }
    }

  return CFI_SUCCESS;
}
