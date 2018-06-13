/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"

#include "TrkAlgebraUtils/AlMat.h"
#include "TrkAlgebraUtils/AlVec.h"
#include "TrkAlgebraUtils/AlSymMat.h"
#include "TrkAlgebraUtils/AlSpaMat.h"

#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <exception>

namespace Trk {

AlMat::AlMat() {
  m_ncol = 0; m_nrow = 0; m_nele = 0;
  m_ptr_data = 0;  // set pointer to null
  m_transpose = false;
  m_pathbin="./";
  m_pathtxt="./";
}

AlMat::AlMat(int N, int M) {
  m_nrow = N;
  m_ncol = M;
  m_nele = N*M;
  m_ptr_data = new double[m_nele];
  m_transpose = false;
  m_pathbin="./";
  m_pathtxt="./";

  double*  p = m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--p) = 0.;

}

AlMat::AlMat(const AlMat& m) {
  m_nrow = m.nrow(); m_ncol = m.ncol(); m_nele = m_nrow*m_ncol;
  m_ptr_data = new double[m_nele];
  m_transpose = false;
  m_pathbin = m.m_pathbin;
  m_pathtxt = m.m_pathtxt;
  copy(m);
}

AlMat::~AlMat()
{if( m_ptr_data != nullptr ) delete [] m_ptr_data;}

void AlMat::copy(const AlMat&  m) {
  int  nr = nrow();
  int  nc = ncol();
  if( nr != m.nrow() || nc != m.ncol() ) {
    throw std::range_error( "AlMat::copy: sizes do not match!" );
  }

  for( int i=0; i<nr; i++ ) {
    for( int j=0; j<nc; j++ ) {
      elemr(i, j) = m.elemc(i, j);
    }
  }
}

void AlMat::copy(const AlSymMatBase&  m) {
  long int  n = m.size();
  if( nrow() != n || ncol() != n ) {
    throw std::range_error( "AlMat::copy: sizes do not match!" );
  }

  for( int i=0; i<n; i++ ) {
    for( int j=0; j<n; j++ ) {
      *(m_ptr_data+i*m_ncol+j) = m.elemc(i, j);
    }
  }
}

double& AlMat::elemr(long int i,long int j) {
#ifdef _DEBUG
  if( i<0 )     { throw std::out_of_range( "AlMat::elemr: Index 1 < zero! " ); };
  if( i>=size ) { throw std::out_of_range( "AlMat::elemr: Index 1 too large!" ); };
  if( j<0 )     { throw std::out_of_range( "AlMat::elemr: Index 2 < zero! " ); };
  if( j>=size ) { throw std::out_of_range( "AlMat::elemr: Index 2 too large!" );  };
#endif

  if( m_transpose )   return  *(m_ptr_data+j*m_ncol+i);
  return  *(m_ptr_data+i*m_ncol+j);
}

double AlMat::elemc(long int i,long int j) const {
#ifdef _DEBUG
  if( i<0 )     { throw std::out_of_range( "AlMat::elemr: Index 1 < zero! " ); };
  if( i>=size ) { throw std::out_of_range( "AlMat::elemr: Index 1 too large!" ); };
  if( j<0 )     { throw std::out_of_range( "AlMat::elemr: Index 2 < zero! " ); };
  if( j>=size ) { throw std::out_of_range( "AlMat::elemr: Index 2 too large!" );  };
#endif

  if( m_transpose )   return  *(m_ptr_data+j*m_ncol+i);
  return  *(m_ptr_data+i*m_ncol+j);
}

long int AlMat::elem(long int i,long int j) const {
#ifdef _DEBUG
  if( i<0 )     { throw std::out_of_range( "AlMat::elemr: Index 1 < zero! " ); };
  if( i>=size ) { throw std::out_of_range( "AlMat::elemr: Index 1 too large!" ); };
  if( j<0 )     { throw std::out_of_range( "AlMat::elemr: Index 2 < zero! " ); };
  if( j>=size ) { throw std::out_of_range( "AlMat::elemr: Index 2 too large!" );  };
#endif

  if( m_transpose )   return  (j*m_ncol+i);
  return  (i*m_ncol+j);
}


AlMat&  AlMat::operator=(const double& d) {

  double*  p = m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--p) = d;

  return *this;
}

AlMat& AlMat::operator=(const AlMat& m) {
  if (this==&m) return *this;

  if(( m_nrow!=0 || m_ncol!=0) && (m_nrow != m.nrow() || m_ncol != m.ncol() ))
  { 
    throw std::range_error( "AlMat=AlMat Assignment: size do not match!" );
  }

  if ( m_ptr_data != m.m_ptr_data ) {
    reSize(m.nrow(), m.ncol());
    copy(m);
  };
  return (*this);
}

AlMat&  AlMat::operator=(const AlSymMat& m) {
  if( ( m_nrow!=0 || m_ncol!=0) && (m_nrow != m.size() || m_ncol != m.size() ))
  {
    throw std::range_error( "AlMat=AlSymMatBase Assignment: size do not match!" ); 
  }

  if ( m_ptr_data != m.ptrData() ) {
    reSize(m.size(), m.size());
    copy(m);
  }
  return (*this);
}

AlMat AlMat::operator+(const AlMat& m) const {
  if( m_nrow != m.m_nrow || m_ncol != m.m_ncol )
  { 
    throw std::range_error( "AlMat: operator+: size do not match!" );
  }

  AlMat b( m_nrow, m_ncol );

  double*  p = m_ptr_data + m_nele;
  double*  q = m.m_ptr_data + m_nele;
  double*  r = b.m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--r) = (*(--p))+(*(--q));

  return b;
}

AlMat&  AlMat::operator+=(const AlMat& m) {

  if( m_nrow != m.m_nrow || m_ncol != m.m_ncol )
  {  
    throw std::range_error( "AlMat: operator+=: size do not match!" ); 
  }

  double*  p = m_ptr_data + m_nele;
  double*  q = m.m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--p) += *(--q);

  return *this;
}

AlMat   AlMat::operator-(const AlMat& m) const {
  if( m_nrow != m.m_nrow || m_ncol != m.m_ncol )
  {  
    throw std::range_error( "AlMat: operator-: size do not match!" ); 
  }

  AlMat b( m_nrow, m_ncol );

  double*  p = m_ptr_data + m_nele;
  double*  q = m.m_ptr_data + m_nele;
  double*  r = b.m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--r) = (*(--p))-(*(--q));

  return b;
}

AlMat&  AlMat::operator-=(const AlMat& m) {
  if( m_nrow != m.m_nrow || m_ncol != m.m_ncol )
  { 
    throw std::range_error( "AlMat: operator-=: size do not match!"); 
  }

  double*  p = m_ptr_data + m_nele;
  double*  q = m.m_ptr_data + m_nele;
  while (p > m_ptr_data) *(--p) -= *(--q);

  return *this;
}

AlMat AlMat::operator*(const AlMat& m) const {
  if( ncol() != m.nrow() ) 
  {  
    throw std::range_error( "AlMat: operator*: size do not match!" );
  }

  int k(nrow());
  int l(m.ncol());
  int f(ncol());

  AlMat b( k, l);
  for( int i=0; i<k; i++ ) {
    for( int j=0; j<l; j++ ) {
      for( int n=0; n<f; n++) b.elemr(i, j) += elemc(i, n)*m.elemc(n, j);
    }
  }
  return b;
}

AlMat AlMat::operator*(const AlSymMatBase& m) const {
  if( ncol() != m.size()) 
  { 
    throw std::range_error( "AlMat: operator*: size do not match!" ); 
  }

  int k(nrow());
  int l(m.size());

  AlMat b( k, l);
  for( int i=0; i<k; i++ ) {
    for( int j=0; j<l; j++ ) {
      for( int n=0; n<l; n++) b.elemr(i, j) += elemc(i, n)*m.elemc(n, j);
    }
  }
  return b;
}


AlVec AlMat::operator*(const AlVec& v) const {
  if( ncol() != v.size() )
  {  
    throw std::range_error( "AlMat: operator*: size do not match! " );
  }

  int k(nrow());
  int l(ncol());
  double* p;
  double* q;

  AlVec b(k);
  for( int i=0; i<k; i++ ) {
    p = b.ptrData()+i;
    q = m_ptr_data+i*l;
    for( int j=0; j<l; j++ )  *p += (*(q+j))*(*(v.ptrData()+j));
  }

  return b;
}

AlMat&   AlMat::operator*=(const double& d) {
  double*  p = m_ptr_data+m_nele;
  while (p > m_ptr_data)
    *(--p) *= d;

  return *this;
}

// transposition
AlMat AlMat::T() const {
  AlMat b(m_ncol,m_nrow);
  for( int i=0; i<b.m_nrow; i++ ) {
    for( int j=0; j<b.m_ncol; j++ ) {
      b[i][j]= *(m_ptr_data+j*m_ncol+i);
    }
  }
  return b;
}

// transposition
AlMat& AlMat::Transpose() {

  m_transpose = true;

  return *this;
}

// normal position
AlMat& AlMat::Normal() {

  m_transpose = false;

  return *this;
}


//invert sym matrix declared as non-symetric for convenience
void AlMat::invertS(int& ierr, double Norm = 1.) {
  if(m_nrow!=m_ncol) {
    throw std::range_error( "AlMat invertS: non-square matrix!" );
  }

  AlSymMat b(m_nrow);
  for( int i=0; i<b.size(); i++ ) {
    for( int j=0; j<=i; j++ ) {
      b.elemr(i, j) = elemc(i, j);
    }
  }

  b*=Norm;
  ierr = b.invert();
  b*=Norm;

  if (ierr==0) (*this).copy(b);

}

// reSize
void AlMat::reSize(int Nnew, int Mnew) {
  if ( Nnew != m_nrow || Mnew != m_ncol ) {

    double*  p = m_ptr_data;
    m_nele = Nnew*Mnew;
    m_ptr_data = new double[m_nele];
    int m_nrow_old = m_nrow;
    int m_ncol_old = m_ncol;
    m_transpose = false;

    m_nrow = Nnew;
    m_ncol = Mnew;
    int k = m_nrow <= m_nrow_old ? m_nrow : m_nrow_old;
    int l = m_ncol <= m_ncol_old ? m_ncol : m_ncol_old;

    for( int i=0; i<k; i++ ) {
      for( int j=0; j<l; j++ ) {
        *(m_ptr_data+i*m_ncol+j) = *(p+i*m_ncol_old+j);
      }
    }

    delete [] p;
  }
}


StatusCode AlMat::ReadScalaPack(const std::string &filename){
  std::ifstream inmat(filename.c_str(), std::ios::binary);
  if(inmat.fail())
    return StatusCode::FAILURE;

  int32_t mNrow = m_nrow;
  inmat.read((char*)&mNrow, sizeof (mNrow));
  int32_t mNcol = m_ncol;
  inmat.read((char*)&mNcol, sizeof (mNcol));

  m_nrow=abs(mNrow);
  m_ncol=abs(mNcol);
  m_nele=m_ncol*m_nrow;
  m_transpose = false;

  // printf("ALMat::nrow: %d \n",m_nrow);
  // printf("ALMat::ncol: %d \n",m_ncol);
  // printf("ALMat::nele: %d \n",m_nele);

  double melem=0;
  for(int i=0; i<m_nrow; i++) {
    for(int j=0; j<m_ncol; j++) {
      inmat.read((char*)&melem, sizeof (melem));
      *(m_ptr_data+i*m_ncol+j) = melem;
      // printf("(%d,%d) = %.16lf \n",i,j,melem);
    }
  }

  inmat.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
void AlMat::SetPathBin(const std::string &path)
{
  m_pathbin.assign(path);
}

//______________________________________________________________________________
void AlMat::SetPathTxt(const std::string &path)
{
  m_pathtxt.assign(path);
}

//______________________________________________________________________________
StatusCode AlMat::Write(const std::string &filename, bool binary, unsigned int precision)
{
  std::ofstream outmat;

  if(binary) {
    outmat.open((m_pathbin+filename).c_str(), std::ios::binary);
    if(outmat.fail())
      return StatusCode::FAILURE;

    int32_t mNrow = m_nrow;
    outmat.write((char*)&mNrow, sizeof (mNrow));
    int32_t mNcol = m_ncol;
    outmat.write((char*)&mNcol, sizeof (mNcol));
  }
  else {
    outmat.open((m_pathtxt+filename).c_str());
    if(outmat.fail())
      return StatusCode::FAILURE;
    outmat.setf(std::ios::fixed);
    if(precision>0)
      outmat.setf(std::ios::showpoint);
    outmat.precision(precision);
  }

  double melem=0;

  for( int i=0; i<m_nrow; i++) {
    for( int j=0; j<m_ncol; j++) {
      melem =  *(m_ptr_data+i*m_ncol+j);
      if(binary)
        outmat.write((char*)&(melem), sizeof (melem));
      else
        outmat << " " << std::setw(12) << melem;
    }
    if(!binary)
      outmat << std::endl;
  }

  outmat.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
const std::string AlMat::Print(const int NColsPerSet)
{
  std::ostringstream textmatrix;
  int FirstCol = 0;
  int LastCol = NColsPerSet;

  int NSets = (ncol()/NColsPerSet);
  if (ncol()%NColsPerSet>0) NSets++;
  textmatrix << std::endl;
  textmatrix << " ++++++++++++++ AlMat ++ "
        << nrow()
        << " x "
        << ncol()
        << " +++++++++++++++++++++++++++++"<< std::endl;
  for (int set=0; set < NSets; set++) {
    FirstCol = set*NColsPerSet;
    LastCol = (set+1)*NColsPerSet;
    if (LastCol>ncol()) LastCol = ncol();
    textmatrix << " |row\\col| ";
    for (int col=FirstCol; col < LastCol; col++) {
      textmatrix << std::setw(6) << col << " | ";
    }
    textmatrix << std::endl;
    textmatrix << " |-------|";
    for (int col=FirstCol; col < LastCol; col++) {
      textmatrix << "--------|";
    }
    textmatrix << std::endl;
    for (int row=0; row<nrow(); row++) {
      textmatrix << " |" << std::setw(6) << row << " | ";
      for (int col=FirstCol; col<LastCol; col++) {
    double melem =  *(m_ptr_data+row*ncol()+col);
    textmatrix << std::setprecision(5) << std:: setw(6) << melem << " | ";
      }
      textmatrix << std::endl;
    }
    if (set != (NSets-1)) textmatrix << std::endl;
  }
  textmatrix << " +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<< std::endl;
  return textmatrix.str();
}

} // end namespace Trk

