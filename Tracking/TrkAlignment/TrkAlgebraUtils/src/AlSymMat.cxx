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
  _matrix_type = 1;
  _size = 0;
  _nele = 0;
  ptr_data = NULL;  // set pointer to null
  m_pathbin="./";
  m_pathtxt="./";
}


//______________________________________________________________________________
AlSymMat::AlSymMat(long int N)
{
  _matrix_type = 1;
  _size = N;
  _nele = N*(N+1)/2;
  ptr_data = new double[_nele];
  m_pathbin="./";
  m_pathtxt="./";

  double*  p = ptr_data + _nele;
  while (p > ptr_data) *(--p) = 0.;

}

//______________________________________________________________________________
AlSymMat::AlSymMat(const AlSymMat& m)
  : AlSymMatBase(m)
{
  _matrix_type = 1;
  _size      = m.size();
  _nele      = m._nele;
  m_pathbin = m.m_pathbin;
  m_pathtxt = m.m_pathtxt;
  ptr_data = new double[_nele];
  copy(m);
}


//______________________________________________________________________________
AlSymMat::AlSymMat(const AlSpaMat& m)
{
  _matrix_type = 1;
  _size      = m.size();
  _nele      = _size*(_size+1)/2;
  m_pathbin = m.pathBin();
  m_pathtxt = m.pathTxt();
  ptr_data = new double[_nele];
  copy(m);
}

//______________________________________________________________________________
AlSymMat& AlSymMat::operator=(const AlSpaMat& m)
{
  _matrix_type = 1;
  _size = m.size();
  _nele = _size*(_size+1)/2;
  m_pathbin = m.pathBin();
  m_pathtxt = m.pathTxt();

  ptr_data = new double[_nele];
  copy(m);
  return *this;
}

//______________________________________________________________________________
AlSymMat::~AlSymMat()
{
  if( ptr_data != NULL ) delete [] ptr_data;
  //ptr_map.clear();
}

//______________________________________________________________________________
void AlSymMat::copy(const AlSymMat& m)
{
  // this one implements the fast copy alg.
  if( size() != m.size()) {
    std::cerr << "AlSymMat::copy: size do not match!" << std::endl;
    return;
  }

  double * p = ptr_data + _nele;
  double*  r = m.ptr_data + _nele;
  while (p > ptr_data) *(--p) = *(--r);

  return;
}

//______________________________________________________________________________
void AlSymMat::copy(const AlSpaMat& m)
{
  //
  if( size() != m.size()) {
    std::cerr << "AlSymMat::copy: size do not match!" << std::endl;
    return;
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
    std::cerr << "AlSymMat::copy: sizes do not match!" << std::endl;
    return;
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

  if ( ptr_data != m.ptr_data ) {
    reSize(m.size());
    copy(m);
  }
  return *this;
}

//______________________________________________________________________________
AlSymMat& AlSymMat::operator=(const AlMat& m)
{
  if( m.nrow() != m.ncol() ) {
    std::cerr << "AlSymMat::operator=: a squared matrix is required!" << std::endl;
    return *this;
  }

  reSize(m.nrow());
  copy(m);

  return *this;
}

//______________________________________________________________________________
AlSymMat& AlSymMat::operator=(const double& d)
{
  double * p = ptr_data + _nele;
  while (p > ptr_data) *(--p) = d;

  return *this;
}

//______________________________________________________________________________
AlSymMat AlSymMat::operator+(const AlSymMat& m) const
{
  if( size() != m.size()) {
    std::cerr << "AlSymMat::operator+: size do not match!" << std::endl;
    return *this;
  }

  AlSymMat b(size());
  double * p = ptr_data + _nele;
  double * q = m.ptr_data + _nele;
  double * r = b.ptr_data + _nele;
  while (p > ptr_data) *(--r) = (*(--p))+(*(--q));

  return b;
}

//______________________________________________________________________________
AlSymMat&  AlSymMat::operator+=(const AlSymMat& m)
{
  if( size() != m.size()){
    std::cerr << "AlSymMat::operator+=: size do not match!" << std::endl;
    return *this;
  }

  double * p = ptr_data + _nele;
  double * q = m.ptr_data + _nele;
  while (p > ptr_data) *(--p) += *(--q);

  return *this;
}

//______________________________________________________________________________
AlSymMat AlSymMat::operator-(const AlSymMat& m) const
{
  if( size() != m.size()) {
    std::cerr << "AlSymMat::operator-: size do not match!" << std::endl;
    return *this;
  }

  AlSymMat b(size());
  double * p = ptr_data + _nele;
  double * q = m.ptr_data + _nele;
  double * r = b.ptr_data + _nele;
  while (p > ptr_data) *(--r) = (*(--p))-(*(--q));

  return b;
}

//______________________________________________________________________________
AlSymMat&  AlSymMat::operator-=(const AlSymMat& m)
{
  if( size() != m.size()) {
    std::cerr << "AlSymMat::operator-=: size do not match!" << std::endl;
    return *this;
  }

  double * p = ptr_data + _nele;
  double * q = m.ptr_data + _nele;
  while (p > ptr_data) *(--p) -= *(--q);

  return *this;
}

//______________________________________________________________________________
AlMat AlSymMat::operator*(const AlSymMat& m) const
{
  if( size() != m.size() ) {
    std::cerr << "AlSymMat::operator*: size do not match!" << std::endl;
    AlMat b( size(), m.size());
    return b;
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
        x += (*(ptr_data+ii+k))*(*(m.ptr_data+jj+k));
      if( i<j ) {
        for( k=l; k<n; k++)
          x += (*(ptr_data+(k+1)*k/2+i))*(*(m.ptr_data+jj+k));
      }
      else {
        for( k=l; k<n; k++)
          x += (*(ptr_data+ii+k))*(*(m.ptr_data+(k+1)*k/2+j));
      }
      for( k=n; k<siz; k++)
        x += (*(ptr_data+(k+1)*k/2+i))*(*(m.ptr_data+(k+1)*k/2+j));
      b.elemr(i,j) = x;
    }

  return b;
}

//______________________________________________________________________________
AlMat AlSymMat::operator*(const AlMat& m) const
{
  if( size() != m.nrow() ) {
    std::cerr << "AlSymMat::operator*: size do not match!" << std::endl;
    return m;
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
        x += (*(ptr_data+ii+k))*m.elemc(k,j);
      for( k=i; k<siz; k++)
        x += (*(ptr_data+(k+1)*k/2+i))*m.elemc(k,j);
      b.elemr(i,j) = x;
    }

  return b;
}

//______________________________________________________________________________
AlVec AlSymMat::operator*(const AlVec& v) const
{
  if( size() != v.size() ) {
    std::cerr << "AlSymMat::operator*: size do not match! " << std::endl;
    return v;
  }

  AlVec b(size());

  long int ii;

  for( long int i=0; i<size(); i++ ) {
    ii = (i+1)*i/2;
    for( long int j=0; j<i; j++ )
      b[i] += (*(ptr_data+ii+j))*v[j];
    for( long int j=i; j<size(); j++ )
      b[i] += (*(ptr_data+(j+1)*j/2+i))*v[j];
  }

  return b;
}

//______________________________________________________________________________
AlSymMat&  AlSymMat::operator*=(const double& d)
{
  double * p = ptr_data + _nele;
  while (p > ptr_data) *(--p) *= d;

  return *this;
}

//______________________________________________________________________________
AlSymMat  AlSymMat::operator*(const double& d) const
{
  AlSymMat a(size());
  double * p = ptr_data + _nele;
  double * q = a.ptr_data + _nele;
  while (p > ptr_data) *(--q) = (*(--p))*d;

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
  double * mx = ptr_data;

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

  return 0.;
}

//______________________________________________________________________________
int AlSymMat::diagonalize(char jobz, AlVec &w, AlMat &z)
{
  int   info = 0;
  char  uplo = 'U';
  int   N = size();
  double * work =  new double[3*N];

  double * ap = ptr_data;

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
          //  cout << "(" << row+k << "," << col << ") -> " << *(ptr_data+elem(row+k,col)) << endl;

          if(col<shift*index)
            *(ptr_data+elem(row+k,col))=*(ptr_data+elem(row+k+shift,col));
          else
            *(ptr_data+elem(row+k,col))=*(ptr_data+elem(row+k+shift,col+shift));
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
    std::cerr<<"Vector of indices to remove is empty."<<std::endl;
    return _size;
  }
  if (n>_size) {
    std::cerr<<"Vector of indices larger than matrix size."<<std::endl;
    return _size;
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
    if (index > _size-1) {
      std::cerr<<"Index "<<index<<" goes beyond matrix (size "<<_size<<")."<<std::endl;
      continue;
    }

    for (int j=index; j<_size-1; j++)
      for (int col=0; col<=j; col++)
        if(col<index)
          *(ptr_data+elem(j,col)) = *(ptr_data+elem(j+1,col));
        else
          *(ptr_data+elem(j,col)) = *(ptr_data+elem(j+1,col+1));

    _size--;
  }

  return _size;
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

      *(ptr_data+elem(row,col))=*(ptr_data+elem(row+shiftRow,col+shiftCol));

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
        melem = *(ptr_data+(i+1)*i/2+j);
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
        if(i>=j) melem =  *(ptr_data+(i+1)*i/2+j);
        else     melem =  *(ptr_data+(j+1)*j/2+i);
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
  _size = abs(msiz);

  if (m_StdUnits)
    inmat.read((char*)&version, sizeof (version));

  double melem=0;

  if(msiz>0) { // square format
    triang=false;
    for(int i=0; i<msiz; i++) {
      for(int j=0; j<msiz; j++) {
        inmat.read((char*)&melem, sizeof (melem));
        if( i>=j )
          *(ptr_data+(i+1)*i/2+j) = melem;
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
        *(ptr_data+(i+1)*i/2+j) = melem;
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
  _size = abs(msiz);

  inmat.read((char*)&version, sizeof (version));

  double melem=0;

  if(msiz>0) { // square format
    triang=false;
    for(int i=0; i<msiz; i++) {
      for(int j=0; j<msiz; j++) {
        inmat.read((char*)&melem, sizeof (melem));
        if( i>=j )
          *(ptr_data+(i+1)*i/2+j) = melem;
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
        *(ptr_data+(i+1)*i/2+j) = melem;
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
    double * p = ptr_data;
    long int ii;

    _nele = Nnew*(Nnew+1)/2;
    ptr_data = new double[_nele];
    long int Size_old = size();
    _size = Nnew;
    long int k = size() <= Size_old ? size() : Size_old;
    for( long int i=0; i<k; i++ ) {
      ii = (i+1)*i/2;
      for( long int j=0; j<=i; j++ )
        *(ptr_data+ii+j) = *(p+ii+j);
    }
    delete [] p;
  }
}

//______________________________________________________________________________
double& AlSymMat::elemr(long int i,long int j)
{
#ifdef _DEBUG
  if( i<0 ) {
    std::cerr << "AlSymMat::elemr: Index 1 < zero! " << i << std::endl;
    return *(ptr_data);
  }
  if( i>=size() ) {
    std::cerr << "AlSymMat::elemr: Index 1 too large! " << i << std::endl;
    return *(ptr_data);
  }
  if( j<0 ) {
    std::cerr << "AlSymMat::elemr: Index 2 < zero! " << j << std::endl;
    return *(ptr_data);
  }
  if( j>=size() ) {
    std::cerr << "AlSymMat::elemr: Index 2 too large! " << j << std::endl;
    return *(ptr_data);
  }
#endif
  if( j<=i )
    return  *(ptr_data+(i+1)*i/2+j);

  return  *(ptr_data+(j+1)*j/2+i);
}

//______________________________________________________________________________
double AlSymMat::elemc(long int i,long int j) const
{
#ifdef _DEBUG
  if( i<0 ) {
    std::cerr << "AlSymMat::elemc: Index 1 < zero! " << i << std::endl;
    return *(ptr_data);
  }
  if( i>=size() ) {
    std::cerr << "AlSymMat::elemc: Index 1 too large! " << i << std::endl;
    return *(ptr_data);
  }
  if( j<0 ) {
    std::cerr << "AlSymMat::elemc: Index 2 < zero! " << j << std::endl;
    return *(ptr_data);
  }
  if( j>=size() ) {
    std::cerr << "AlSymMat::elemc: Index 2 too large! " << j << std::endl;
    return *(ptr_data);
  }
#endif
  if( j<=i )
    return  *(ptr_data+(i+1)*i/2+j);

  return  *(ptr_data+(j+1)*j/2+i);
}


TMatrixDSparse* AlSymMat::makeTMatrix(){

  int  si = size();
	
	int nnz(0);

  for( int i=0; i<si; i++ )
    for( int j=0; j<=i; j++ )
			if( *(ptr_data+(i+1)*i/2+j)  != 0 ) 
				++nnz;

	
	int    *irow = new int[nnz];
	int    *icol = new int[nnz];
	double *val  = new double[nnz];
	int n(0);
  
  for (int i=0;i<si;i++) {
    for (int j=0;j<=i;j++) {
      if ( *(ptr_data+(i+1)*i/2+j) != 0.) {
        irow[n] = i;
        icol[n] = j;
        val[n]  = *(ptr_data+(i+1)*i/2+j);
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
