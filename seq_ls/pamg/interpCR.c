/*BHEADER**********************************************************************
 * (c) 1998   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision$
*********************************************************************EHEADER*/

#include "headers.h"

/*--------------------------------------------------------------------------
 * hypre_AMGBuildInterpCR
 *--------------------------------------------------------------------------*/

int
hypre_AMGBuildCRInterp( hypre_CSRMatrix  *A,
                   int                 *CF_marker,
		   int			n_coarse,
		   int			num_relax_steps,
		   int			relax_type,
		   double	        relax_weight,
                   hypre_CSRMatrix     **P_ptr )
{
   
   double          *A_data;
   int             *A_i;
   int             *A_j;

   hypre_CSRMatrix *P; 
   hypre_Vector	   *zero_vector;
   hypre_Vector	   *x_vector;
   hypre_Vector	   *tmp_vector;
   double          *x_data;

   double          *P_data;
   int             *P_i;
   int             *P_j;

   int              P_size;
   
   int             *P_marker;

   int              n_fine;

   int             *coarse_to_fine;
   int              coarse_counter;
   
   int              i,ic,i1,i2;
   int              j,jj,jj1;
   int              kk,k1;
   int              extended_nghbr;
   
   double           summ, sump;
   
   /*-----------------------------------------------------------------------
    *  Access the CSR vectors for A and S. Also get size of fine grid.
    *-----------------------------------------------------------------------*/

   A_data = hypre_CSRMatrixData(A);
   A_i    = hypre_CSRMatrixI(A);
   A_j    = hypre_CSRMatrixJ(A);

   n_fine = hypre_CSRMatrixNumRows(A);

   /*-----------------------------------------------------------------------
    *  First Pass: Determine size of P and fill in fine_to_coarse mapping.
    *-----------------------------------------------------------------------*/

   /*-----------------------------------------------------------------------
    *  Intialize counters and allocate mapping vector.
    *-----------------------------------------------------------------------*/

   extended_nghbr = 0;
   if (num_relax_steps > 1) extended_nghbr = 1;
   coarse_counter = 0;

   coarse_to_fine = hypre_CTAlloc(int, n_coarse);

   /*-----------------------------------------------------------------------
    *  Loop over fine grid.
    *-----------------------------------------------------------------------*/
    
   P_i    = hypre_CTAlloc(int, n_fine+1);
   P_marker = hypre_CTAlloc(int, n_fine);

   for (i = 0; i < n_fine; i++)
   {
      
      /*--------------------------------------------------------------------
       *  If i is a c-point, interpolation is the identity. Also set up
       *  mapping vector.
       *--------------------------------------------------------------------*/

      if (CF_marker[i] > 0)
      {
         coarse_to_fine[coarse_counter] = i;
         coarse_counter++;
      
	 i2 = i+2;
	 P_marker[i] = i2;
	 P_i[i+1]++;          
         for (jj = A_i[i]+1; jj < A_i[i+1]; jj++)
         {
            i1 = A_j[jj]; 
	    if (CF_marker[i1] < 0)
	    {
	       if (P_marker[i1] != i2) 
	       {
		  P_i[i1+1]++;          
	          P_marker[i1] = i2;
	       }
	       if (extended_nghbr)
	       {
                  for (kk = A_i[i1]+1; kk < A_i[i1+1]; kk++)
                  {
	             k1 = A_j[kk];
	             if (CF_marker[k1] < 0)
	             {
	                if (P_marker[k1] != i2) 
                        {
			   P_i[k1+1]++;	
                           P_marker[k1] = i2;
                        }
                     }
                  }
               }
            }
         }
      }
   }
   for (i = 1; i < n_fine; i++)
      P_i[i+1] += P_i[i];

   /*-----------------------------------------------------------------------
    *  Allocate  arrays.
    *-----------------------------------------------------------------------*/

   n_coarse = coarse_counter;

   P_size = P_i[n_fine];

   P_j    = hypre_CTAlloc(int, P_size);
   P_data = hypre_CTAlloc(double, P_size);
   zero_vector = hypre_VectorCreate(n_fine);
   x_vector = hypre_VectorCreate(n_fine);
   tmp_vector = hypre_VectorCreate(n_fine);
   hypre_VectorInitialize(zero_vector);
   hypre_VectorInitialize(x_vector);
   hypre_VectorInitialize(tmp_vector);
   x_data = hypre_VectorData(x_vector);

   /*-----------------------------------------------------------------------
    *  Second Pass: Define interpolation and fill in P_data, P_i, and P_j.
    *-----------------------------------------------------------------------*/

   /*-----------------------------------------------------------------------
    *  Intialize some stuff.
    *-----------------------------------------------------------------------*/

   for (ic = 0; ic < n_coarse; ic++)
   {
      i = coarse_to_fine[ic];
      i2 = i+2;
      P_marker[i] = 0;
      for (jj = A_i[i]+1; jj < A_i[i+1]; jj++)
      {
         i1 = A_j[jj]; 
         if (CF_marker[i1] < 0) 
	 {
	    P_marker[i1] = i2;
	    if (extended_nghbr)
	    {
               for (kk = A_i[i1]+1; kk < A_i[i1+1]; kk++)
               {
	          k1 = A_j[kk];
                  if (CF_marker[k1] < 0) 
			P_marker[k1] = i2;
                  else
			P_marker[k1] = 0;
               }
            }
         }
      }
      hypre_VectorSetConstantValues(x_vector, 0.0);
      x_data[i] = 1.0;
      for (jj = 0; jj < num_relax_steps; jj++) 
         hypre_AMGRelax(A, zero_vector, P_marker, relax_type, i2,
			   relax_weight, x_vector, tmp_vector);
      for (jj = 0; jj < n_fine; jj++)
      {
	 if (P_marker[jj] == i2)
	 {
	    P_j[P_i[jj]] = ic;
	    P_data[P_i[jj]] = x_data[jj];
	    P_i[jj]++;
	 }
      }
      P_data[P_i[i]] = 1.0;
      P_j[P_i[i]] = ic;
      P_i[i]++;
   }
   for (i = n_fine-1; i > -1; i--)
      P_i[i+1] = P_i[i];
   P_i[0] = 0;
   
   /*--------------------------------------------------------------------
    *  global compatible relaxation
    *--------------------------------------------------------------------*/

   hypre_VectorSetConstantValues(x_vector, 1.0);
   for (jj = 0; jj < num_relax_steps; jj++) 
       hypre_AMGRelax(A, zero_vector, CF_marker, relax_type, -1,
		      relax_weight, x_vector, tmp_vector);
  
   /*-----------------------------------------------------------------------
    *  perform normalization
    *-----------------------------------------------------------------------*/
    
   for (i = 0; i  < n_fine  ; i ++)
   {
      if (CF_marker[i] < 0)
      {
	 sump = 0.0;
	 summ = 0.0;
	 for (j = P_i[i]; j < P_i[i+1]; j++)
	    if (P_data[j] > 0)
	       sump += P_data[j];
	    else
	       summ += P_data[j];
	 if (sump != 0) sump = x_data[i]/sump;
	 if (summ != 0) summ = x_data[i]/summ;
	 for (j = P_i[i]; j < P_i[i+1]; j++)
	    if (P_data[j] > 0)
               P_data[j] = P_data[j]*sump;
	    else
               P_data[j] = P_data[j]*summ;
      }
   }   
      
   P = hypre_CSRMatrixCreate(n_fine, n_coarse, P_size);
   hypre_CSRMatrixData(P) = P_data; 
   hypre_CSRMatrixI(P) = P_i; 
   hypre_CSRMatrixJ(P) = P_j; 

   *P_ptr = P; 

   /*-----------------------------------------------------------------------
    *  Free mapping vector and marker array.
    *-----------------------------------------------------------------------*/

   hypre_TFree(P_marker);   
   hypre_TFree(coarse_to_fine);   
   hypre_VectorDestroy(tmp_vector);   
   hypre_VectorDestroy(x_vector);   
   hypre_VectorDestroy(zero_vector);   

   return(0);
}            
          
