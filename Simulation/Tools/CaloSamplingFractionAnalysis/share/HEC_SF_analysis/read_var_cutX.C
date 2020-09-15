/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>

using namespace std;

////////////////////////////////////////////////////////////////////////
//
//  Read average values and apply given cuts (if necessary)
//
void read_var_cutX  ( 
     Char_t  *file, 
     Float_t X1, Float_t X2,
     Int_t   &N, 
     Float_t *&X, Float_t *&EX,
     Float_t *&Y, Float_t *&EY )
{
//
   Int_t   *nevts;
   Float_t *x, *ave,*err;
//
//
   Int_t   i;
   Int_t   n, nx;
   Float_t xmin,xmax, dx;
//
   ifstream fin;
//   
/* ------------------------------------------------------------------ */   
//
//  Open file
//
   fin.open(file);
   if ( !fin.good() ) {
      cerr << " *** read_var_cutX:  No file - " << file;
      cerr << " !!! *** " << endl;
      exit (-2);
   }
//
//
   fin >> n;
   if (n < 1 || !fin.good()) {
      cerr << " *** read_var_cutX:  Bad n = " << n << " in file - " << file;
      cerr << " !!! *** " << endl;
      exit (-2);
   }
//
   fin >> xmin >> xmax >> dx;
   if ( !fin.good() ) {
      cerr << " *** read_var_cutX:  Bad 2-nd line in file - " << file;
      cerr << " !!! *** " << endl;
      exit (-2);
   }
//
//  Read in data
//
   nevts = new Int_t   [n];
   x     = new Float_t [n];   
   ave   = new Float_t [n];   
   err   = new Float_t [n];   
//
   N = 0;
//
   for (i=0; i<n; i++) {
      fin >> x[i] >> nevts[i] >> ave[i] >> err[i];
      if ( !fin.good() ) {
   	 cerr << " *** read_var_cutX:  Bad reading for i=" << i;
	 cerr << " in file - " << file << " !!! *** " << endl;
   	 exit (-2);
      }
//
      if (nevts[i] == 0) continue;
      if (X1<X2 && ( x[i]<=X1 || x[i]>=X2 )) continue;
//
      N ++;
   }
//
   fin.close();
//
//
   if (N < 1) {
      cerr << " *** read_var_cutX:  Bad N = " << N << " for file - " << file;
      cerr << " !!! *** " << endl;
      exit (-2);
   }
//
//  Store non-zero data after cuts
//
    X = new Float_t [N]; 
   EX = new Float_t [N]; 
    Y = new Float_t [N];   
   EY = new Float_t [N];   
//
   nx = N;
   N  = 0;
//
   for (i=0; i<n; i++) {
      if (nevts[i] == 0) continue;
      if (X1<X2 && ( x[i]<=X1 || x[i]>=X2 )) continue;
//
       X[N] = x[i];
      EX[N] = 0.5 * dx;
       Y[N] = ave[i];
      EY[N] = err[i];    
//
      N ++;        
   }
//
   if (N != nx) {
      cerr << " *** read_var_cutX:  Bad N != nx : " << N << " " << nx;
      cerr << " !!! *** " << endl;
      exit (-2);
   }
//
//
   delete [] nevts;
   delete [] x;
   delete [] ave;
   delete [] err;
//      
}
////////////////////////////////////////////////////////////////////////
