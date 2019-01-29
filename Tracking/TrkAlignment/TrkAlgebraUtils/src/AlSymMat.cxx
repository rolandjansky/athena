/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"

#include "TrkAlgebraUtils/AlSymMat.h"
#include "TrkAlgebraUtils/AlSpaMat.h"
#include "TrkAlgebraUtils/AlVec.h"
#include "TrkAlgebraUtils/AlMat.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <float.h> //for DBL_EPSILON
#include <stdint.h>
#include <stdexcept>

#include <TMatrixDSparse.h>


extern "C" {
  void dsptrf_(char*, const int*, double*, int*, int* );
}

extern "C" {
  void dsptri_(char*, const int*, double*, int*, double*, int* );
}

extern "C" {
  void dspev_(char*, char*, const int*, double*,
              double*, double*, const int*, double*, int* );
}

namespace Trk {

//______________________________________________________________________________
AlSymMat::AlSymMat()
{
  m_matrix_type = 1;
  m_size = 0;
  m_nele = 0;
  m_ptr_data = nullptr;  // set pointer to null
  m_pathbin="./";
  m_pathtxt="./";
}


//______________________________________________________________________________
AlSymMat::AlSymMat(long int N)
{
  m_matrix_type = 1;
  m_size = N;
  m_nele = N*(N+1)/2;
  m_ptr_data = new double[m_nele];
  m_pathbin="./";
  m_pathtxt="./";

  double*  p = m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--p) = 0.;

}

//______________________________________________________________________________
AlSymMat::AlSymMat(const AlSymMat& m)
  : AlSymMatBase(m)
{
  m_matrix_type = 1;
  m_size      = m.size();
  m_nele      = m.m_nele;
  m_pathbin = m.m_pathbin;
  m_pathtxt = m.m_pathtxt;
  m_ptr_data = new double[m_nele];
  copy(m);
}


//______________________________________________________________________________
AlSymMat::AlSymMat(const AlSpaMat& m)
{
  m_matrix_type = 1;
  m_size      = m.size();
  m_nele      = m_size*(m_size+1)/2;
  m_pathbin = m.pathBin();
  m_pathtxt = m.pathTxt();
  m_ptr_data = new double[m_nele];
  copy(m);
}

//______________________________________________________________________________
AlSymMat& AlSymMat::operator=(const AlSpaMat& m)
{
  m_matrix_type = 1;
  m_size = m.size();
  m_nele = m_size*(m_size+1)/2;
  m_pathbin = m.pathBin();
  m_pathtxt = m.pathTxt();

  m_ptr_data = new double[m_nele];
  copy(m);
  return *this;
}

//______________________________________________________________________________
AlSymMat::~AlSymMat()
{
  if( m_ptr_data != nullptr ) delete [] m_ptr_data;
  //ptr_map.clear();
}

//______________________________________________________________________________
void AlSymMat::copy(const AlSymMat& m)
{
  // this one implements the fast copy alg.
  if( size() != m.size()) {
    throw std::range_error( "AlSymMat::copy: size do not match!" );
  }

  double * p = m_ptr_data + m_nele;
  double*  r = m.m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--p) = *(--r);

  return;
}

//______________________________________________________________________________
void AlSymMat::copy(const AlSpaMat& m)
{
  //
  if( size() != m.size()) {
    throw std::range_error( "AlSymMat::copy: size do not match!" );
  }

  (*this) = 0.;
  //Convert Storage System
  long int      i, j;
  const_mapiterator pos;
  for (pos = m.ptrMap()->begin(); pos!=m.ptrMap()->end(); pos++) {
    m.elem(pos->first, i, j);
    elemr(i,j) = pos->second;
  }

  return;
}

//______________________________________________________________________________
void AlSymMat::copy(const AlMat& m)
{
  // copy the lower triangle only!
  int  si = size();
  if( si != m.nrow() || si != m.ncol() ) {
    throw std::range_error("AlSymMat::copy: sizes do not match!" );
  }

  for( int i=0; i<si; i++ )
    for( int j=0; j<=i; j++ )
      elemr(i, j) = m.elemc(i, j);
}

//______________________________________________________________________________
AlSymMat& AlSymMat::operator=(const AlSymMat& m)
{
  if (this==&m)
    return *this;

  if ( m_ptr_data != m.m_ptr_data ) {
    reSize(m.size());
    copy(m);
  }
  return *this;
}

//______________________________________________________________________________
AlSymMat& AlSymMat::operator=(const AlMat& m)
{
  if( m.nrow() != m.ncol() ) {
    throw std::range_error( "AlSymMat::operator=: a squared matrix is required!" );
  }

  reSize(m.nrow());
  copy(m);

  return *this;
}

//______________________________________________________________________________
AlSymMat& AlSymMat::operator=(const double& d)
{
  double * p = m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--p) = d;

  return *this;
}

//______________________________________________________________________________
AlSymMat AlSymMat::operator+(const AlSymMat& m) const
{
  if( size() != m.size()) {
    throw std::range_error( "AlSymMat::operator+: size do not match!" );
  }

  AlSymMat b(size());
  double * p = m_ptr_data + m_nele;
  double * q = m.m_ptr_data + m_nele;
  double * r = b.m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--r) = (*(--p))+(*(--q));

  return b;
}

//______________________________________________________________________________
AlSymMat&  AlSymMat::operator+=(const AlSymMat& m)
{
  if( size() != m.size()){
    throw std::range_error( "AlSymMat::operator+=: size do not match!" );
  }

  double * p = m_ptr_data + m_nele;
  double * q = m.m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--p) += *(--q);

  return *this;
}

//______________________________________________________________________________
AlSymMat AlSymMat::operator-(const AlSymMat& m) const
{
  if( size() != m.size()) {
    throw std::range_error( "AlSymMat::operator-: size do not match!" );
  }

  AlSymMat b(size());
  double * p = m_ptr_data + m_nele;
  double * q = m.m_ptr_data + m_nele;
  double * r = b.m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--r) = (*(--p))-(*(--q));

  return b;
}

//______________________________________________________________________________
AlSymMat&  AlSymMat::operator-=(const AlSymMat& m)
{
  if( size() != m.size()) {
    throw std::range_error( "AlSymMat::operator-=: size do not match!" );
  }

  double * p = m_ptr_data + m_nele;
  double * q = m.m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--p) -= *(--q);

  return *this;
}

//______________________________________________________________________________
AlMat AlSymMat::operator*(const AlSymMat& m) const
{
  if( size() != m.size() ) {
    throw std::range_error( "AlSymMat::operator*: size do not match!" );
  }

  AlMat b( size(), size());
  double x;
  long int k, l, n, ii, jj;
  long int siz(size());

  // optimised version of symmetric matrix product:
  for( long int i=0; i<siz; i++ )
    for( long int j=0; j<siz; j++ ) {
      x = 0.;
      l = i < j ? i : j;
      n = i < j ? j : i;
      ii = (i+1)*i/2;
      jj = (j+1)*j/2;
      for( k=0; k<l; k++)
        x += (*(m_ptr_data+ii+k))*(*(m.m_ptr_data+jj+k));
      if( i<j ) {
        for( k=l; k<n; k++)
          x += (*(m_ptr_data+(k+1)*k/2+i))*(*(m.m_ptr_data+jj+k));
      }
      else {
        for( k=l; k<n; k++)
          x += (*(m_ptr_data+ii+k))*(*(m.m_ptr_data+(k+1)*k/2+j));
      }
      for( k=n; k<siz; k++)
        x += (*(m_ptr_data+(k+1)*k/2+i))*(*(m.m_ptr_data+(k+1)*k/2+j));
      b.elemr(i,j) = x;
    }

  return b;
}

//______________________________________________________________________________
AlMat AlSymMat::operator*(const AlMat& m) const
{
  if( size() != m.nrow() ) {
    throw std::range_error("AlSymMat::operator*: size do not match!" );
  }

  AlMat b( size(), m.ncol());

  double x;
  long int k, ii;
  long int siz(size()), nco(m.ncol());

  // optimised version of matrix product:
  for( long int i=0; i<siz; i++ )
    for( long int j=0; j<nco; j++ ) {
      x = 0.;
      ii = (i+1)*i/2;
      for( k=0; k<i; k++)
        x += (*(m_ptr_data+ii+k))*m.elemc(k,j);
      for( k=i; k<siz; k++)
        x += (*(m_ptr_data+(k+1)*k/2+i))*m.elemc(k,j);
      b.elemr(i,j) = x;
    }

  return b;
}

//______________________________________________________________________________
AlVec AlSymMat::operator*(const AlVec& v) const
{
  if( size() != v.size() ) {
    throw std::range_error( "AlSymMat::operator*: size do not match! " );
  }

  AlVec b(size());

  long int ii;

  for( long int i=0; i<size(); i++ ) {
    ii = (i+1)*i/2;
    for( long int j=0; j<i; j++ )
      b[i] += (*(m_ptr_data+ii+j))*v[j];
    for( long int j=i; j<size(); j++ )
      b[i] += (*(m_ptr_data+(j+1)*j/2+i))*v[j];
  }

  return b;
}

//______________________________________________________________________________
AlSymMat&  AlSymMat::operator*=(const double& d)
{
  double * p = m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--p) *= d;

  return *this;
}

//______________________________________________________________________________
AlSymMat  AlSymMat::operator*(const double& d) const
{
  AlSymMat a(size());
  double * p = m_ptr_data + m_nele;
  double * q = a.m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--q) = (*(--p))*d;

  return a;
}

//______________________________________________________________________________
int AlSymMat::invert()
{
  // add a protection to detect singular matrices:
  //  double det = (*this).determinant();
  //  double detcut = 1.E-20;

  //  if (fabs(det)>detcut) {

  int   ierr = 0;
  char  uplo = 'U';
  int   N = size();
  int    * ipiv =  new int[N];
  double * work =  new double[N];
  double * mx = m_ptr_data;

  dsptrf_(&uplo, &N, mx, ipiv, &ierr);

  dsptri_(&uplo, &N, mx, ipiv, work, &ierr);

  delete [] ipiv;
  delete [] work;

  //  } else {
  //    ierr=999;
  //  }
  return ierr;
}

//______________________________________________________________________________
double AlSymMat::determinant()
{
  double deter = 1.;

  // get determinant using LU decomposition + Crout algorithm
  AlMat A(size(),size());
  A = (*this);

  double AMAX,DUM,SUM;
  //int CODE;
  double TINY = 1.E-20;
  double D;
  int N=size();

  D=1.; //CODE=0;

  for(int I=0;I<N;I++) {
    AMAX=0.;
    for(int J=0;J<N;J++)
      if (fabs(A[I][J])>AMAX)
        AMAX=fabs(A[I][J]);
    if(AMAX<TINY) {
//      CODE = 1; // matrix is singular
      return 0;
    }
  }

  for(int J=0;J<N;J++) {
    for(int I=0;I<N;I++) {
      SUM = A[I][J];
      int KMAX = (I<J)?I:J;
      for(int K=0;K<KMAX;K++)
        SUM = SUM - A[I][K]*A[K][J];
      A[I][J]=SUM;
    }

    // find pivot and exchange if necessary
    int IMAX=J;
    for (int I=J+1;I<N;I++) {
      if(fabs(A[I][J])>fabs(A[IMAX][J])) IMAX=I;
    }

    if(IMAX!=J) {
      for(int K=0;K<N;K++) {
        DUM=A[IMAX][K];
        A[IMAX][K]=A[J][K];
        A[J][K]=DUM;
      }
      D=-D;
    }

    if(J<N && (fabs(A[J][J])>TINY)) {
      DUM = 1./ A[J][J];
      for(int I=J+1;I<N;I++) A[I][J] = A[I][J]*DUM;
    }
  }

  deter=deter*D;
  for(int I=0;I<N;I++)
    deter = deter*A[I][I];

  return deter;
}

//______________________________________________________________________________
int AlSymMat::diagonalize(char jobz, AlVec &w, AlMat &z)
{
  int   info = 0;
  char  uplo = 'U';
  int   N = size();
  double * work =  new double[3*N];

  double * ap = m_ptr_data;

  dspev_(&jobz, &uplo, &N, ap, w.ptrData(), z.ptrData(), &N, work, &info);

  delete [] work;

  return info;
}

//______________________________________________________________________________
// index here is module number within matrix (size=6*max_index)
void AlSymMat::RemoveModule(int index)
{
  const int shift=6;

  if((shift*index)<size() && index>=0) {
    for(int row=(shift*index); row<(size()-shift); row+=shift) {
      for(int k=0; k<shift; k++) {
        for(int col=0; col<row+k+1; col++) {
          //  cout << "(" << row+k << "," << col << ") -> " << *(m_ptr_data+elem(row+k,col)) << endl;

          if(col<shift*index)
            *(m_ptr_data+elem(row+k,col))=*(m_ptr_data+elem(row+k+shift,col));
          else
            *(m_ptr_data+elem(row+k,col))=*(m_ptr_data+elem(row+k+shift,col+shift));
        }
      }
    }
  }
}


//______________________________________________________________________________
int AlSymMat::RemoveCollsRows(std::vector<int> indices)
{
  int n = indices.size();
  if (n==0) {
    return m_size;
  }
  if (n>m_size) {
    throw std::range_error("Vector of indices larger than matrix size.");
  }

  // first sort the list of indices descending
  // maybe not the fastest way to do that but it works
  for (int i=0;i<n-1;i++)
    for (int j=i+1; j<n; j++)
      if (indices[j] > indices[i]) {
        int itmp = indices[i];
        indices[i] = indices[j];
        indices[j] = itmp;
      }

  // remove rows and columns starting from largest indices
  for (int i=0;i<n;i++) {
    int index = indices[i];
    if (index > m_size-1) {
      throw std::out_of_range("AlSymMat::RemoveCollsRows: Index goes beyond matrix.");
    }

    for (int j=index; j<m_size-1; j++)
      for (int col=0; col<=j; col++)
        if(col<index)
          *(m_ptr_data+elem(j,col)) = *(m_ptr_data+elem(j+1,col));
        else
          *(m_ptr_data+elem(j,col)) = *(m_ptr_data+elem(j+1,col+1));

    m_size--;
  }

  return m_size;
}

//______________________________________________________________________________
// index here is alignment parameter to remove
void AlSymMat::RemoveAlignPar(int index, int control)
{
  // Dynamic shiftRow
  int shiftRow = 1;
  int counterRow = 0;

  // Dynamic shiftCol
  int shiftCol;
  int counterCol = 0;

  index = index-control;

  for(int row=index; row<(size()-shiftRow); row++) {

    shiftCol = 0;
    counterCol = 0;

    for(int col=0; col<row+1; col++) {

      if (col==index) {
        shiftCol = 1;
        counterCol=0;
      }

      *(m_ptr_data+elem(row,col))=*(m_ptr_data+elem(row+shiftRow,col+shiftCol));

      counterCol++;
      if (counterCol==5-control) {
        counterCol=0;
        shiftCol++;
      }
    }

    counterRow++;
    if (counterRow==5-control) {
      counterRow=0;
      shiftRow++;
    }
  }
}

//______________________________________________________________________________
void AlSymMat::SetPathBin(const std::string &path)
{
  m_pathbin.assign(path);
}

//______________________________________________________________________________
void AlSymMat::SetPathTxt(const std::string &path)
{
  m_pathtxt.assign(path);
}

//______________________________________________________________________________
StatusCode AlSymMat::Write(const std::string &filename, bool binary,
                           bool square, double, float version)
{
  std::ofstream outmat;

  int32_t msizz = square ? size() : (-1)*size();

  if(binary) {
    outmat.open((m_pathbin+filename).c_str(), std::ios::binary);
    if(outmat.fail())
      return StatusCode::FAILURE;
    // if triangular, change size sign to distinguish
    // from square format
    outmat.write((char*)&msizz, sizeof (msizz));
    outmat.write((char*)&version, sizeof (version));
  }
  else {
    outmat.open((m_pathtxt+filename).c_str());
    if(outmat.fail())
      return StatusCode::FAILURE;
    outmat.setf(std::ios::fixed);
    outmat.setf(std::ios::showpoint);
    outmat.precision(3);
    outmat << "msizz: " << std::setw(6) << msizz << std::endl;
    outmat << "AlSymMat version: " << std::setw(6) << version << std::endl;
  }

  double melem=0;

  if (!square) {
    for( int i=0; i<size(); i++) {
      for( int j=0; j<=i; j++) {
        melem = *(m_ptr_data+(i+1)*i/2+j);
        if(binary)
          outmat.write((char*)&(melem), sizeof (melem));
        else
          outmat << std::setw(14) << melem;
      }
      if(!binary)
        outmat << std::endl;
    }
  }
  else {
    for( int i=0; i<size(); i++) {
      for( int j=0; j<size(); j++) {
        if(i>=j) melem =  *(m_ptr_data+(i+1)*i/2+j);
        else     melem =  *(m_ptr_data+(j+1)*j/2+i);
        if(binary)
          outmat.write((char*)&(melem), sizeof (melem));
        else
          outmat << std::setw(14) << melem;
      }
      if(!binary)
        outmat << std::endl;
    }
  }
  outmat.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlSymMat::CheckMatVersion(const std::string filename, bool &StdUnits)
{
  std::ifstream inmat((filename).c_str(), std::ios::binary);
  if(inmat.fail())
    return StatusCode::FAILURE;

  int32_t msiz=0;
  inmat.read((char*)&msiz, sizeof (msiz));

  float version=0.0;
  inmat.read((char*)&version, sizeof (version));

  if (version>=2.0)
    StdUnits = true;
  else
    StdUnits = false;

  inmat.close();

  // std::cout << "AlSymMat::StdUnits: " << StdUnits << std::endl;

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlSymMat::Read(const std::string &filename, int &dofs,
                          bool &triang, float &version)
{
  bool m_StdUnits = true;
  if (StatusCode::SUCCESS != CheckMatVersion(m_pathbin+filename, m_StdUnits))
    return StatusCode::FAILURE;

  // std::cout << "AlSymMat::StdUnits: " << m_StdUnits << std::endl;

  std::ifstream inmat((m_pathbin+filename).c_str(), std::ios::binary);
  if(inmat.fail())
    return StatusCode::FAILURE;

  int32_t msiz=0;
  inmat.read((char*)&msiz, sizeof (msiz));
  dofs = abs(msiz);
  m_size = abs(msiz);

  if (m_StdUnits)
    inmat.read((char*)&version, sizeof (version));

  double melem=0;

  if(msiz>0) { // square format
    triang=false;
    for(int i=0; i<msiz; i++) {
      for(int j=0; j<msiz; j++) {
        inmat.read((char*)&melem, sizeof (melem));
        if( i>=j )
          *(m_ptr_data+(i+1)*i/2+j) = melem;
      }
    }
  }
  else { // triangular format
    triang=true;
    msiz = (-1)*msiz;
    // std::cout << "msiz="  << msiz << std::endl;
    for( int i=0; i<msiz; i++) {
      for( int j=0; j<=i; j++) {
        inmat.read((char*)&melem, sizeof (melem));
        *(m_ptr_data+(i+1)*i/2+j) = melem;
      }
    }
  }

  inmat.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlSymMat::ReadProjected(const std::string &filename, int &dofs,
                                   bool &triang, float &version)
{
  std::ifstream inmat((m_pathbin+filename).c_str(), std::ios::binary);
  if(inmat.fail())
    return StatusCode::FAILURE;

  int32_t msiz=0;
  inmat.read((char*)&msiz, sizeof (msiz));
  dofs = abs(msiz);
  m_size = abs(msiz);

  inmat.read((char*)&version, sizeof (version));

  double melem=0;

  if(msiz>0) { // square format
    triang=false;
    for(int i=0; i<msiz; i++) {
      for(int j=0; j<msiz; j++) {
        inmat.read((char*)&melem, sizeof (melem));
        if( i>=j )
          *(m_ptr_data+(i+1)*i/2+j) = melem;
      }
    }
  }
  else { // triangular format
    triang=true;
    msiz = (-1)*msiz;
    // std::cout << "msiz="  << msiz << std::endl;
    for( int i=0; i<msiz; i++) {
      for( int j=0; j<=i; j++) {
        inmat.read((char*)&melem, sizeof (melem));
        *(m_ptr_data+(i+1)*i/2+j) = melem;
      }
    }
  }

  inmat.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
void AlSymMat::reSize(long int Nnew)
{
  if ( Nnew != size() ) {
    double * p = m_ptr_data;
    long int ii;

    m_nele = Nnew*(Nnew+1)/2;
    m_ptr_data = new double[m_nele];
    long int Size_old = size();
    m_size = Nnew;
    long int k = size() <= Size_old ? size() : Size_old;
    for( long int i=0; i<k; i++ ) {
      ii = (i+1)*i/2;
      for( long int j=0; j<=i; j++ )
        *(m_ptr_data+ii+j) = *(p+ii+j);
    }
    delete [] p;
  }
}

//______________________________________________________________________________
double& AlSymMat::elemr(long int i,long int j)
{
#ifdef _DEBUG
  if( i<0 ) {
    throw std::underflow_error( "AlSymMat::elemr: Index 1 < zero!" );
  }
  if( i>=size() ) {
    throw std::overflow_error( "AlSymMat::elemr: Index 1 too large!" );
  }
  if( j<0 ) {
    throw std::underflow_error( "AlSymMat::elemr: Index 2 < zero!" );
  }
  if( j>=size() ) {
    throw std::overflow_error( "AlSymMat::elemr: Index 2 too large!" );
  }
#endif
  if( j<=i )
    return  *(m_ptr_data+(i+1)*i/2+j);

  return  *(m_ptr_data+(j+1)*j/2+i);
}

//______________________________________________________________________________
double AlSymMat::elemc(long int i,long int j) const
{
#ifdef _DEBUG
  if( i<0 ) {
    throw std::underflow_error( "AlSymMat::elemc: Index 1 < zero!" );
  }
  if( i>=size() ) {
    throw std::overflow_error( "AlSymMat::elemc: Index 1 too large!" );
  }
  if( j<0 ) {
    throw std::underflow_error( "AlSymMat::elemc: Index 2 < zero!" );
  }
  if( j>=size() ) {
    throw std::overflow_error( "AlSymMat::elemc: Index 2 too large!" );
  }
#endif
  if( j<=i )
    return  *(m_ptr_data+(i+1)*i/2+j);

  return  *(m_ptr_data+(j+1)*j/2+i);
}


TMatrixDSparse* AlSymMat::makeTMatrix(){

  int  si = size();
	
	int nnz(0);

  for( int i=0; i<si; i++ )
    for( int j=0; j<=i; j++ )
			if( *(m_ptr_data+(i+1)*i/2+j)  != 0 ) 
				++nnz;

	
	int    *irow = new int[nnz];
	int    *icol = new int[nnz];
	double *val  = new double[nnz];
	int n(0);
  
  for (int i=0;i<si;i++) {
    for (int j=0;j<=i;j++) {
      if ( *(m_ptr_data+(i+1)*i/2+j) != 0.) {
        irow[n] = i;
        icol[n] = j;
        val[n]  = *(m_ptr_data+(i+1)*i/2+j);
        ++n;
      }
    }
  }
     
 
  TMatrixDSparse* myTMatrix = new TMatrixDSparse(0,si-1,0,si-1);
  myTMatrix->SetMatrixArray(nnz,irow,icol,val);

  delete [] irow;
  delete [] icol;
  delete [] val;


  return myTMatrix;

}


} // end namespace Trk
