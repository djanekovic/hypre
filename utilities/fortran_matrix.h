/*BHEADER**********************************************************************
 * Copyright (c) 2006   The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * Written by the HYPRE team <hypre-users@llnl.gov>, UCRL-CODE-222953.
 * All rights reserved.
 *
 * This file is part of HYPRE (see http://www.llnl.gov/CASC/hypre/).
 * Please see the COPYRIGHT_and_LICENSE file for the copyright notice, 
 * disclaimer and the GNU Lesser General Public License.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (as published by the Free
 * Software Foundation) version 2.1 dated February 1999.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the terms and conditions of the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * $Revision$
 ***********************************************************************EHEADER*/

#ifndef FORTRAN_STYLE_MATRIX
#define FORTRAN_STYLE_MATRIX

typedef struct
{
  long	globalHeight;
  long	height;
  long	width;
  double* value;
  int		ownsValues;
} utilities_FortranMatrix;

#ifdef __cplusplus
extern "C" {
#endif

utilities_FortranMatrix* 
utilities_FortranMatrixCreate(void);
void 
utilities_FortranMatrixAllocateData( long h, long w, 
				     utilities_FortranMatrix* mtx );
void 
utilities_FortranMatrixWrap( double*, long gh, long h, long w, 
			     utilities_FortranMatrix* mtx );
void 
utilities_FortranMatrixDestroy( utilities_FortranMatrix* mtx );

long
utilities_FortranMatrixGlobalHeight( utilities_FortranMatrix* mtx );
long
utilities_FortranMatrixHeight( utilities_FortranMatrix* mtx );
long
utilities_FortranMatrixWidth( utilities_FortranMatrix* mtx );
double*
utilities_FortranMatrixValues( utilities_FortranMatrix* mtx );

void 
utilities_FortranMatrixClear( utilities_FortranMatrix* mtx );
void 
utilities_FortranMatrixClearL( utilities_FortranMatrix* mtx );
void 
utilities_FortranMatrixSetToIdentity( utilities_FortranMatrix* mtx );

void 
utilities_FortranMatrixTransposeSquare( utilities_FortranMatrix* mtx );
void 
utilities_FortranMatrixSymmetrize( utilities_FortranMatrix* mtx );

void 
utilities_FortranMatrixCopy( utilities_FortranMatrix* src, int t, 
			     utilities_FortranMatrix* dest );
void 
utilities_FortranMatrixIndexCopy( int* index, 
				  utilities_FortranMatrix* src, int t, 
				  utilities_FortranMatrix* dest );

void 
utilities_FortranMatrixSetDiagonal( utilities_FortranMatrix* mtx, 
				    utilities_FortranMatrix* d );
void 
utilities_FortranMatrixGetDiagonal( utilities_FortranMatrix* mtx, 
				    utilities_FortranMatrix* d );
void 
utilities_FortranMatrixAdd( double a, 
			    utilities_FortranMatrix* mtxA, 
			    utilities_FortranMatrix* mtxB, 
			    utilities_FortranMatrix* mtxC );
void 
utilities_FortranMatrixDMultiply( utilities_FortranMatrix* d, 
				  utilities_FortranMatrix* mtx );
void 
utilities_FortranMatrixMultiplyD( utilities_FortranMatrix* mtx, 
				  utilities_FortranMatrix* d );
void 
utilities_FortranMatrixMultiply( utilities_FortranMatrix* mtxA, int tA, 
				 utilities_FortranMatrix* mtxB, int tB,
				 utilities_FortranMatrix* mtxC );
double 
utilities_FortranMatrixFNorm( utilities_FortranMatrix* mtx );

double 
utilities_FortranMatrixValue( utilities_FortranMatrix* mtx, 
			      long i, long j );
double* 
utilities_FortranMatrixValuePtr( utilities_FortranMatrix* mtx, 
				 long i, long j );
double 
utilities_FortranMatrixMaxValue( utilities_FortranMatrix* mtx );

void 
utilities_FortranMatrixSelectBlock( utilities_FortranMatrix* mtx,
				    long iFrom, long iTo, 
				    long jFrom, long jTo,
				    utilities_FortranMatrix* block );
void 
utilities_FortranMatrixUpperInv( utilities_FortranMatrix* u );

int
utilities_FortranMatrixPrint( utilities_FortranMatrix* mtx, char fileName[] );

#ifdef __cplusplus
}
#endif

#endif /* FORTRAN_STYLE_MATRIX */

