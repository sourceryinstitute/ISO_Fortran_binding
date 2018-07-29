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

int CFI_section (CFI_cdesc_t *result, const CFI_cdesc_t *source,
                 const CFI_index_t lower_bounds[],
                 const CFI_index_t upper_bounds[], const CFI_index_t strides[])
{
  /* C Descriptors must not be NULL. */
  if (source == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: Source must not be "
                       "NULL. (Error No. %d).\n",
               CFI_INVALID_DESCRIPTOR);
      return CFI_INVALID_DESCRIPTOR;
    }
  if (result == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: Result must not be "
                       "NULL. (Error No. %d).\n",
               CFI_INVALID_DESCRIPTOR);
      return CFI_INVALID_DESCRIPTOR;
    }

  /* Base address of source must not be NULL. */
  if (source->base_addr == NULL)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: Base address of "
                       "source must not be NULL. (Error No. %d).\n",
               CFI_ERROR_BASE_ADDR_NULL);
      return CFI_ERROR_BASE_ADDR_NULL;
    }

  /* Result must not be an allocatable array. */
  if (result->attribute == CFI_attribute_allocatable)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: Result must not "
                       "describe an allocatable array. (Error No. %d).\n",
               CFI_INVALID_ATTRIBUTE);
      return CFI_INVALID_ATTRIBUTE;
    }

  /* Source must be some form of array (nonallocatable nonpointer array,
   * allocated allocatable array or an associated pointer array). */
  if (source->rank <= 0)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: Source must "
                       "describe an array (0 < source->rank, 0 !< %d). (Error No. "
                       "%d).\n",
               source->rank, CFI_INVALID_RANK);
      return CFI_INVALID_RANK;
    }

  /* Element lengths of source and result must be equal. */
  if (result->elem_len != source->elem_len)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: The element "
                       "lengths of source (source->elem_len = %ld) and result "
                       "(result->elem_len = %ld) must be equal. (Error No. "
                       "%d).\n",
               source->elem_len, result->elem_len, CFI_INVALID_ELEM_LEN);
      return CFI_INVALID_ELEM_LEN;
    }

  /* Types must be equal. */
  if (result->type != source->type)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: Types of source "
                       "(source->type = %d) and result (result->type = %d) "
                       "must be equal. (Error No. %d).\n",
               source->type, result->type, CFI_INVALID_TYPE);
      return CFI_INVALID_TYPE;
    }

  /* Stride of zero in the i'th dimension means rank reduction in that
   * dimension. */
  int zero_count = 0;
  for (int i = 0; i < source->rank; i++)
    {
      if (strides[i] == 0)
        {
          zero_count++;
        }
    }

  /* Rank of result must be equal the the rank of source minus the number of
   * zeros in strides. */
  if (result->rank != source->rank - zero_count)
    {
      fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: Rank of result "
                       "must be equal to the rank of source minus the number "
                       "of zeros in strides (result->rank = source->rank - "
                       "zero_count, %d != %d - %d) (Error No. %d).\n",
               result->rank, source->rank, zero_count, CFI_INVALID_RANK);
      return CFI_INVALID_RANK;
    }

  /* Dimension information. */
  CFI_index_t *lower;
  CFI_index_t *upper;
  CFI_index_t *stride;
  lower  = malloc (source->rank * sizeof (CFI_index_t));
  upper  = malloc (source->rank * sizeof (CFI_index_t));
  stride = malloc (source->rank * sizeof (CFI_index_t));

  /* Lower bounds. */
  if (lower_bounds == NULL)
    {
      for (int i = 0; i < source->rank; i++)
        {
          lower[i] = source->dim[i].lower_bound;
        }
    }
  else
    {
      for (int i = 0; i < source->rank; i++)
        {
          lower[i] = lower_bounds[i];
        }
    }

  /* Upper bounds. */
  if (upper_bounds == NULL)
    {
      if (source->dim[source->rank].extent == -1)
        {
          fprintf (stderr,
                   "ISO_Fortran_binding.c: CFI_section: Source must not "
                   "be an assumed size array if upper_bounds is NULL. (Error "
                   "No. %d).\n",
                   CFI_INVALID_EXTENT);
          return CFI_INVALID_EXTENT;
        }
      for (int i = 0; i < source->rank; i++)
        {
          upper[i] = source->dim[i].lower_bound + source->dim[i].extent - 1;
        }
    }
  else
    {
      for (int i = 0; i < source->rank; i++)
        {
          upper[i] = upper_bounds[i];
        }
    }

  /* Stride */
  if (strides == NULL)
    {
      for (int i = 0; i < source->rank; i++)
        {
          stride[i] = 1;
        }
    }
  else
    {
      for (int i = 0; i < source->rank; i++)
        {
          stride[i] = strides[i];
          /* If stride[i] = then lower[i] and upper[i] must be equal. */
          if (stride[i] == 0 && lower[i] != upper[i])
            {
              fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: If "
                               "strides[%d] = 0, then the lower bounds, "
                               "lower_bounds[%d] = %ld, and upper_bounds[%d] = "
                               "%ld, must be equal. (Error No. %d).\n",
                       i, i, lower_bounds[i], i, upper_bounds[i],
                       CFI_ERROR_OUT_OF_BOUNDS);
              return CFI_ERROR_OUT_OF_BOUNDS;
            }
        }
    }

  /* Check that section upper and lower bounds are within the array bounds. */
  for (int i = 0; i < source->rank; i++)
    {
      if (lower_bounds != NULL &&
          (lower[i] < source->dim[i].lower_bound ||
           lower[i] > source->dim[i].lower_bound + source->dim[i].extent - 1))
        {
          fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: Lower bounds "
                           "must be within the bounds of the fortran array "
                           "(source->dim[%d].lower_bound <= lower_bounds[%d] "
                           "<= source->dim[%d].lower_bound + "
                           "source->dim[%d].extent - 1, %ld <= %ld <= %ld). "
                           "(Error No. %d).\n",
                   i, i, i, i, source->dim[i].lower_bound, lower[i],
                   source->dim[i].lower_bound + source->dim[i].extent - 1,
                   CFI_ERROR_OUT_OF_BOUNDS);
          return CFI_ERROR_OUT_OF_BOUNDS;
        }
      if (upper_bounds != NULL &&
          (upper[i] < source->dim[i].lower_bound ||
           upper[i] > source->dim[i].lower_bound + source->dim[i].extent - 1))
        {
          fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: Upper bounds "
                           "must be within the bounds of the fortran array "
                           "(source->dim[%d].lower_bound <= upper_bounds[%d] "
                           "<= source->dim[%d].lower_bound + "
                           "source->dim[%d].extent - 1, %ld !<= %ld !<= %ld). "
                           "(Error No. %d).\n",
                   i, i, i, i, source->dim[i].lower_bound, upper[i],
                   source->dim[i].lower_bound + source->dim[i].extent - 1,
                   CFI_ERROR_OUT_OF_BOUNDS);
          return CFI_ERROR_OUT_OF_BOUNDS;
        }
      if (upper[i] < lower[i] && stride[i] >= 0)
        {
          fprintf (stderr, "ISO_Fortran_binding.c: CFI_section: If the upper "
                           "bound is smaller than the lower bound for a given "
                           "dimension (upper[%d] < lower[%d], %ld < %ld), then "
                           "the stride for said dimension must be negative "
                           "(stride[%d] < 0, %ld < 0). (Error No. %d)\n",
                   i, i, upper[i], lower[i], i, stride[i], CFI_INVALID_STRIDE);
          return CFI_INVALID_STRIDE;
        }
    }

  /* Update the result to describe the array section. */
  /* Set appropriate memory address. */
  result->base_addr = CFI_address (source, lower);

  /* Set the appropriate dimension information that gives us access to the
   * data. */
  int aux = 0;
  for (int i = 0; i < source->rank; i++)
    {
      if (stride[i] == 0)
        {
          aux++;
          continue;
        }
      int idx                      = i - aux;
      result->dim[idx].lower_bound = lower[i];
      result->dim[idx].extent      = upper[i] - lower[i] + 1;
      result->dim[idx].sm          = stride[i] * source->dim[i].sm;
    }

  free (lower);
  free (upper);
  free (stride);

  return CFI_SUCCESS;
}
