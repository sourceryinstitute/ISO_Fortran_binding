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

void *CFI_address (const CFI_cdesc_t *dv, const CFI_index_t subscripts[])
{
  /* C Descriptor must not be NULL. */
  if (dv == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_address: C Descriptor is "
                       "NULL. (Error No. %d).\n",
               CFI_INVALID_DESCRIPTOR);
      return NULL;
    }

  /* Base address of C Descriptor must not be NULL. */
  if (dv->base_addr == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_address: base address of C "
                       "Descriptor must not be NULL. (Error No. %d).\n",
               CFI_ERROR_BASE_ADDR_NULL);
      return NULL;
    }

  /* Return base address if C descriptor is a scalar. */
  if (dv->rank == 0)
    {
      return dv->base_addr;
    }
  /* Calculate the appropriate base address if dv is not a scalar. */
  else
    {
      /* Base address is the C address of the element of the object specified by
       * subscripts. */
      void *base_addr;

      /* In order to properly account for Fortran's column major order we need
       * to transpose the subscripts, since columns are stored contiguously as
       * opposed to rows like C. */
      CFI_index_t *tr_subscripts;
      CFI_dim_t *  tr_dim;
      tr_subscripts = malloc (dv->rank * sizeof (CFI_index_t));
      tr_dim        = malloc (dv->rank * sizeof (CFI_dim_t));
      for (int i = 0; i < dv->rank; i++)
        {
          CFI_index_t idx  = dv->rank - i - 1;
          tr_subscripts[i] = subscripts[idx];
          tr_dim[i]        = dv->dim[idx];
          /* Normalise the subscripts to start counting the address from 0. */
          tr_subscripts[i] -= tr_dim[i].lower_bound;
        }

      /* We assume column major order as that is how Fortran stores arrays. We
       * calculate the memory address of the specified element via the canonical
       * array dimension reduction map and multiplying by the memory stride. */
      CFI_index_t index = tr_subscripts[0] * tr_dim[0].sm;
      /* Check that the first subscript is within the bounds of the Fortran
       * array. */
      if (subscripts[0] < dv->dim[0].lower_bound ||
          subscripts[0] > dv->dim[0].lower_bound + dv->dim[0].extent - 1)
        {
          fprintf (stderr, "ISO_Fortran_binding.c: CFI_address: subscripts[0], "
                           "is out of bounds. dim->[0].lower_bound <= "
                           "subscripts[0] <= dv->dim[0].lower_bound + "
                           "dv->dim[0].extent - 1 (%ld <= %ld <= %ld). (Error "
                           "No. %d).\n",
                   dv->dim[0].lower_bound, subscripts[0],
                   dv->dim[0].lower_bound + dv->dim[0].extent - 1,
                   CFI_ERROR_OUT_OF_BOUNDS);
          return NULL;
        }

      /* Start calculating the memory offset. We use the transposed subscripts
       * because we assume the array is coming from Fortran and the address is
       * being queried in column-major order. */
      CFI_index_t tmp_index = 1;
      for (int i = 1; i < dv->rank; i++)
        {
          /* Check that the subsequent subscripts are within the bounds of the
           * Fortran array. */
          if (subscripts[i] < dv->dim[i].lower_bound ||
              subscripts[i] > dv->dim[i].lower_bound + dv->dim[i].extent - 1)
            {
              fprintf (stderr, "ISO_Fortran_binding.c: CFI_address: "
                               "subscripts[%d], is out of bounds. "
                               "dv->dim[%d].lower_bound <= subscripts[%d] <= "
                               "dv->dim[%d].lower_bound + dv->dim[%d].extent - "
                               "1 (%ld <= %ld <= %ld). (Error No. %d).\n",
                       i, i, i, i, i, dv->dim[i].lower_bound, subscripts[i],
                       dv->dim[i].extent + dv->dim[i].lower_bound - 1,
                       CFI_ERROR_OUT_OF_BOUNDS);
              return NULL;
            }

          /* Use the canonical dimension reduction mapping to find the memory
           * address of the relevant subscripts. It is assumed the arrays are
           * stored in column-major order like in Fortran, and the provided
           * subscripts are given as if we were operating on a Fortran array. */
          tmp_index *=
              tr_subscripts[i] * tr_dim[i - 1].extent * tr_dim[i - 1].sm;
          index += tmp_index;
        }
      free (tr_subscripts);
      free (tr_dim);

      /* There's no way in C to do general arithmetic on a void pointer so we
       * cast to a char pointer, do the arithmetic and cast back to a
       * void pointer. */
      base_addr = (char *) dv->base_addr + index;

      return base_addr;
    }
}
