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
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

int main (void)
{

  CFI_rank_t      rank;
  CFI_attribute_t attribute;
  CFI_type_t      type[10] = {CFI_type_Bool,        CFI_type_short,
                         CFI_type_ucs4_char,   CFI_type_double,
                         CFI_type_float128,    CFI_type_float128_Complex,
                         CFI_type_long_double, CFI_type_long_double_Complex,
                         CFI_type_struct,      CFI_type_other};
  size_t elem_len;
  int    ind;
  size_t base_type;
  size_t base_type_size;
  size_t errno;

  /* Test function establish. */
  /* Fresh descriptor, base address is NULL. */
  /* Loop through type. */
  for (int i = 0; i < 10; i++)
    {
      elem_len = 0;
      if (type[i] == CFI_type_char || type[i] == CFI_type_ucs4_char ||
          type[i] == CFI_type_signed_char || type[i] == CFI_type_struct ||
          type[i] == CFI_type_other)
        {
          base_type      = type[i];
          base_type_size = elem_len;
        }
      else
        {
          base_type      = type[i] & CFI_type_mask;
          base_type_size = (type[i] - base_type) >> CFI_type_kind_shift;
        }
      /* Loop through attribute. */
      for (int j = 1; j <= 3; j++)
        {
          attribute = j;
          /* Loop through rank. */
          for (int k = 0; k <= CFI_MAX_RANK; k++)
            {
              errno = 1;
              rank  = k;
              CFI_CDESC_T (rank) test1;
              /* We do this because C sometimes doesn't make the structures with
               * a null base_addr which leads to weird behaviour inside
               * CFI_establish.
               */
              if (test1.base_addr != NULL)
                {
                  test1.base_addr = NULL;
                  free (test1.base_addr);
                }
              ind = CFI_establish ((CFI_cdesc_t *) &test1, NULL, attribute,
                                   type[i], elem_len, rank, NULL);
              if (ind != CFI_SUCCESS)
                {
                  goto next_attribute1;
                }
              if (attribute != test1.attribute)
                {
                  printf ("CFI_establish: failed to assign attribute.\n");
                  return 1;
                }
              if (type[i] != test1.type)
                {
                  printf ("CFI_establish: failed to assign type.\n");
                  return 1;
                }
              if (rank != test1.rank)
                {
                  printf ("CFI_establish: failed to assign rank.\n");
                  return 1;
                }
              elem_len = base_type_size;
              if (base_type_size == 10)
                {
                  elem_len = 64;
                }
              if (base_type == CFI_type_Complex)
                {
                  elem_len *= 2;
                }
              if (elem_len != test1.elem_len)
                {
                  printf ("CFI_establish: failed to assign element length.\n");
                  return 1;
                }
            }
        next_attribute1:;
        }
    }

  /* Fresh descriptor, base address is not NULL */
  CFI_index_t *extents = NULL;
  /* Loop through type. */
  for (int i = 0; i < 10; i++)
    {
      elem_len = 0;
      if (type[i] == CFI_type_char || type[i] == CFI_type_ucs4_char ||
          type[i] == CFI_type_signed_char || type[i] == CFI_type_struct ||
          type[i] == CFI_type_other)
        {
          base_type      = type[i];
          base_type_size = elem_len;
        }
      else
        {
          base_type      = type[i] & CFI_type_mask;
          base_type_size = (type[i] - base_type) >> CFI_type_kind_shift;
        }
      /* Loop through attribute. */
      for (int j = 1; j <= 3; j++)
        {
          attribute = j;
          /* Loop through rank. */
          for (int k = 0; k <= CFI_MAX_RANK; k++)
            {
              errno = 1;
              rank  = k;
              if (extents != NULL)
                {
                  free (extents);
                }
              extents = malloc (rank * sizeof (CFI_index_t));
              for (int r = 0; r < rank; r++)
                {
                  extents[r] = r + 1;
                }
              CFI_CDESC_T (rank) test2;
              /* We do this because C sometimes doesn't make the structures with
               * a null base_addr which leads to weird behaviour inside
               * CFI_establish.
               */
              if (test2.base_addr != NULL)
                {
                  test2.base_addr = NULL;
                  free (test2.base_addr);
                }
              ind = CFI_establish ((CFI_cdesc_t *) &test2, &ind, attribute,
                                   type[i], elem_len, rank, extents);
              if (ind != CFI_SUCCESS)
                {
                  goto next_attribute2;
                }
              if (attribute != test2.attribute)
                {
                  printf ("CFI_establish: failed to assign attribute.\n");
                  return 1;
                }
              if (type[i] != test2.type)
                {
                  printf ("CFI_establish: failed to assign type.\n");
                  return 1;
                }
              if (rank != test2.rank)
                {
                  printf ("CFI_establish: failed to assign rank.\n");
                  return 1;
                }

              elem_len = base_type_size;
              if (base_type_size == 10)
                {
                  elem_len = 64;
                }
              if (base_type == CFI_type_Complex)
                {
                  elem_len *= 2;
                }
              if (elem_len != test2.elem_len)
                {
                  printf ("CFI_establish: failed to assign element length.\n");
                  return 1;
                }

              for (int r = 0; r < rank; r++)
                {
                  if (extents[r] != test2.dim[r].extent)
                    {
                      printf ("CFI_establish: failed to assign dimension "
                              "extents.\n");
                      return 1;
                    }
                }

              if (attribute == CFI_attribute_pointer)
                {
                  for (int r = 0; r < rank; r++)
                    {
                      if (test2.dim[r].lower_bound != 0)
                        {
                          printf ("CFI_establish: failed to assign dimension "
                                  "lower bounds.\n");
                          return 1;
                        }
                    }
                }
            }
        next_attribute2:;
        }
    }

  /* Fresh descriptor, base address is not NULL */
  CFI_index_t *lower = NULL;
  CFI_index_t *upper = NULL;
  /* Loop through type. */
  for (int i = 0; i < 10; i++)
    {
      elem_len = 0;
      if (type[i] == CFI_type_struct)
        {
          base_type      = type[i];
          base_type_size = 69;
        }
      else if (type[i] == CFI_type_other)
        {
          base_type      = type[i];
          base_type_size = 666;
        }
      else if (type[i] == CFI_type_char || type[i] == CFI_type_ucs4_char ||
               type[i] == CFI_type_signed_char)
        {
          base_type      = type[i] & CFI_type_mask;
          base_type_size = 3;
        }
      else
        {
          base_type      = type[i] & CFI_type_mask;
          base_type_size = (type[i] - base_type) >> CFI_type_kind_shift;
        }

      elem_len = base_type_size;
      if (base_type_size == 10)
        {
          elem_len = 64;
        }
      if (base_type == CFI_type_Complex)
        {
          elem_len *= 2;
        }
      /* Loop through attribute. */
      for (int j = 1; j <= 3; j++)
        {
          attribute = j;
          /* Loop through rank. */
          for (int k = 0; k <= CFI_MAX_RANK; k++)
            {
              errno = 1;
              rank  = k;
              if (extents != NULL)
                {
                  free (extents);
                }
              if (lower != NULL)
                {
                  free (lower);
                }
              if (upper != NULL)
                {
                  free (upper);
                }
              extents = malloc (rank * sizeof (CFI_index_t));
              lower   = malloc (rank * sizeof (CFI_index_t));
              upper   = malloc (rank * sizeof (CFI_index_t));
              for (int r = 0; r < rank; r++)
                {
                  extents[r] = 2;
                  lower[r]   = r;
                  upper[r]   = lower[r] + extents[r];
                }
              CFI_CDESC_T (rank) test3;
              /* We do this because C sometimes doesn't make the structures with
               * a null base_addr which leads to weird behaviour inside
               * CFI_establish.
               */
              if (test3.base_addr != NULL)
                {
                  test3.base_addr = NULL;
                  free (test3.base_addr);
                }
              ind = CFI_establish ((CFI_cdesc_t *) &test3, NULL, attribute,
                                   type[i], elem_len, rank, extents);
              ind =
                  CFI_allocate ((CFI_cdesc_t *) &test3, lower, upper, elem_len);
              if (ind != CFI_SUCCESS)
                {
                  goto next_attribute3;
                }
              for (int r = 0; r < rank; r++)
                {
                  if (lower[r] != test3.dim[r].lower_bound)
                    {
                      printf ("CFI_allocate: failed to reassign dimension "
                              "lower bounds.\n");
                      return 1;
                    }
                  if (upper[r] - test3.dim[r].lower_bound + 1 !=
                      test3.dim[r].extent)
                    {
                      printf ("CFI_allocate: failed to reassign dimension "
                              "extents.\n");
                      return 1;
                    }
                  if (test3.dim[r].sm != test3.elem_len)
                    {
                      printf (
                          "CFI_allocate: failed to assign dimension stride.\n");
                      return 1;
                    }
                }
              if (elem_len != test3.elem_len)
                {
                  printf ("CFI_allocate: failed to reassign element length.\n");
                  return 1;
                }
            }
        next_attribute3:;
        }
    }

  rank  = 1;
  errno = 1;
  CFI_CDESC_T (rank) test4;
  base_type      = type[3] & CFI_type_mask;
  base_type_size = (type[3] - base_type) >> CFI_type_kind_shift;
  attribute      = CFI_attribute_allocatable;
  ind = CFI_establish ((CFI_cdesc_t *) &test4, NULL, attribute, type[3],
                       elem_len, rank, NULL);
  ind = CFI_allocate ((CFI_cdesc_t *) &test4, NULL, NULL, base_type_size);
  if (ind != CFI_INVALID_EXTENT)
    {
      printf ("CFI_allocate: failed to detect invalid extents.\n");
      return 1;
    }

  rank  = 1;
  errno = 1;
  CFI_CDESC_T (rank) test5;
  base_type      = type[3] & CFI_type_mask;
  base_type_size = (type[3] - base_type) >> CFI_type_kind_shift;
  attribute      = CFI_attribute_pointer;
  ind = CFI_establish ((CFI_cdesc_t *) &test5, &ind, attribute, type[3],
                       elem_len, rank, extents);
  ind = CFI_allocate ((CFI_cdesc_t *) &test5, NULL, NULL, base_type_size);
  if (ind != CFI_ERROR_BASE_ADDR_NOT_NULL)
    {
      printf ("CFI_allocate: failed to detect base address is not NULL.\n");
      return 1;
    }

  /* Test CFI_deallocate. */
  rank           = 1;
  errno          = 1;
  base_type      = type[3] & CFI_type_mask;
  base_type_size = (type[3] - base_type) >> CFI_type_kind_shift;
  for (int i = 1; i <= 3; i++)
    {
      attribute = i;
      if (extents != NULL)
        {
          free (extents);
        }
      if (lower != NULL)
        {
          free (lower);
        }
      if (upper != NULL)
        {
          free (upper);
        }
      extents = malloc (rank * sizeof (CFI_index_t));
      lower   = malloc (rank * sizeof (CFI_index_t));
      upper   = malloc (rank * sizeof (CFI_index_t));
      CFI_CDESC_T (rank) test6;
      ind = CFI_establish ((CFI_cdesc_t *) &test6, NULL, attribute, type[i],
                           elem_len, rank, extents);
      ind = CFI_allocate ((CFI_cdesc_t *) &test6, lower, upper, base_type_size);
      if (ind == CFI_SUCCESS)
        {
          ind = CFI_deallocate ((CFI_cdesc_t *) &test6);
          if (ind != CFI_INVALID_ATTRIBUTE && test6.base_addr != NULL)
            {
              printf ("CFI_deallocate: failed to deallocate memory.\n");
              return 1;
            }
        }
    }

  /* Test CFI_is_contiguous. */
  int tmp_ind;
  base_type      = type[3] & CFI_type_mask;
  base_type_size = (type[3] - base_type) >> CFI_type_kind_shift;
  for (int i = 1; i <= 3; i++)
    {
      attribute = i;
      for (int j = 0; j <= 4; j++)
        {
          errno = 1;
          rank  = j;
          if (extents != NULL)
            {
              free (extents);
            }
          if (lower != NULL)
            {
              free (lower);
            }
          if (upper != NULL)
            {
              free (upper);
            }
          extents = malloc (rank * sizeof (CFI_index_t));
          lower   = malloc (rank * sizeof (CFI_index_t));
          upper   = malloc (rank * sizeof (CFI_index_t));
          for (int r = 0; r < rank; r++)
            {
              extents[r] = 2;
              lower[r]   = r;
              upper[r]   = lower[r] + extents[r];
            }
          CFI_CDESC_T (rank) test7;
          ind = CFI_establish ((CFI_cdesc_t *) &test7, NULL, attribute, type[3],
                               elem_len, rank, extents);
          tmp_ind = CFI_allocate ((CFI_cdesc_t *) &test7, lower, upper,
                                  base_type_size);
          if (tmp_ind != CFI_SUCCESS)
            {
              goto next_attribute4;
            }
          ind = CFI_is_contiguous ((CFI_cdesc_t *) &test7);
          if (ind != CFI_INVALID_RANK && rank == 0 &&
              tmp_ind != CFI_INVALID_ATTRIBUTE)
            {
              printf ("CFI_is_contiguous: failed to detect incorrect rank.\n");
              return 1;
            }
          else if (ind == CFI_ERROR_BASE_ADDR_NULL && test7.base_addr != NULL &&
                   tmp_ind != CFI_SUCCESS)
            {
              printf ("CFI_is_contiguous: failed to detect base address is not "
                      "NULL.\n");
              return 1;
            }
        }
      next_attribute4:;
    }

  /* Test CFI_address. */
  CFI_index_t *tr_subscripts;
  CFI_dim_t *  tr_dim;
  /* Loop through type. */
  for (int i = 0; i < 10; i++)
    {
      elem_len = 0;
      if (type[i] == CFI_type_struct)
        {
          base_type      = type[i];
          base_type_size = 69;
        }
      else if (type[i] == CFI_type_other)
        {
          base_type      = type[i];
          base_type_size = 666;
        }
      else if (type[i] == CFI_type_char || type[i] == CFI_type_ucs4_char ||
               type[i] == CFI_type_signed_char)
        {
          base_type      = type[i] & CFI_type_mask;
          base_type_size = 3;
        }
      else
        {
          base_type      = type[i] & CFI_type_mask;
          base_type_size = (type[i] - base_type) >> CFI_type_kind_shift;
        }

      elem_len = base_type_size;
      if (base_type_size == 10)
        {
          elem_len = 64;
        }
      if (base_type == CFI_type_Complex)
        {
          elem_len *= 2;
        }
      /* Loop through attribute. */
      for (int j = 1; j <= 3; j++)
        {
          attribute = j;
          /* Loop through rank. */
          for (int k = 1; k <= CFI_MAX_RANK; k++)
            {
              errno = 1;
              rank  = k;
              CFI_CDESC_T (rank) source;
              if (extents != NULL)
                {
                  free (extents);
                }
              if (lower != NULL)
                {
                  free (lower);
                }
              if (upper != NULL)
                {
                  free (upper);
                }
              extents = malloc (rank * sizeof (CFI_index_t));
              lower   = malloc (rank * sizeof (CFI_index_t));
              upper   = malloc (rank * sizeof (CFI_index_t));
              for (int r = 0; r < rank; r++)
                {
                  extents[r] = rank - r + 1;
                  lower[r]   = rank - r - 3;
                  upper[r]   = lower[r] + extents[r] - 1;
                }
              ind = CFI_establish ((CFI_cdesc_t *) &source, NULL,
                                   CFI_attribute_allocatable, type[i], elem_len,
                                   rank, extents);
              ind = CFI_allocate ((CFI_cdesc_t *) &source, lower, upper,
                                  elem_len);
              if (ind == CFI_SUCCESS)
                {
                  CFI_index_t dif_addr;
                  CFI_index_t n_entries = 1;
                  dif_addr              = (CFI_index_t) (
                      (char *) CFI_address ((CFI_cdesc_t *) &source, upper) -
                      (char *) CFI_address ((CFI_cdesc_t *) &source, lower));
                  for (int r = 0; r < rank; r++)
                    {
                      n_entries = n_entries * (upper[r] - lower[r] + 1);
                    }
                  tr_subscripts = malloc (rank * sizeof (CFI_index_t));
                  tr_dim        = malloc (rank * sizeof (CFI_dim_t));
                  for (int i = 0; i < rank; i++)
                    {
                      CFI_index_t idx  = rank - i - 1;
                      tr_subscripts[i] = upper[idx];
                      tr_dim[i]        = source.dim[idx];
                      /* Normalise the subscripts to start counting the address
                       * from 0. */
                      tr_subscripts[i] -= tr_dim[i].lower_bound;
                    }
                  /* We assume column major order as that is how Fortran stores
                   * arrays. We
                   * calculate the memory address of the specified element via
                   * the canonical
                   * array dimension reduction map and multiplying by the memory
                   * stride. */
                  CFI_index_t index     = tr_subscripts[0] * tr_dim[0].sm;
                  CFI_index_t tmp_index = 1;
                  for (int i = 1; i < rank; i++)
                    {
                      tmp_index *= tr_subscripts[i] * tr_dim[i - 1].extent *
                                   tr_dim[i - 1].sm;
                      index += tmp_index;
                    }
                  free (tr_subscripts);
                  free (tr_dim);
                  if (index - dif_addr != 0)
                    {
                      printf ("CFI_address: difference in address is not being "
                              "properly calculated.\n");
                      return 1;
                    }
                }
              else if (ind == CFI_ERROR_MEM_ALLOCATION)
                {
                  goto next_type;
                }
            }
        }
    next_type:;
    }

  return 0;
}
