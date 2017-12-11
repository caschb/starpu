/* GCC-StarPU
   Copyright (C) 2011 Inria

   GCC-StarPU is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   GCC-StarPU is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC-StarPU.  If not, see <http://www.gnu.org/licenses/>.  */

/* (instructions compile (cflags "-Wno-unused-variable")) */

static int global[123]              /* (error "cannot be used") */
  __attribute__ ((heap_allocated, used));

extern int external[123]            /* (error "cannot be used") */
  __attribute__ ((heap_allocated));

void
foo (size_t size)
{
  float scalar /* (error "must have an array type") */
    __attribute__ ((heap_allocated));
  float *ptr   /* (error "must have an array type") */
    __attribute__ ((heap_allocated));
  float incomp[]  /* (error "incomplete array type") */
    __attribute__ ((heap_allocated));
  float incomp2[size][3][]  /* (error "incomplete element type") */
    __attribute__ ((heap_allocated));
}
