/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Original Author: Anthony Morley  (15Jan2007)
// MKU (15Jan2007)
// PBdR (17Apr2007) interface ~identcal to AlSymMat

#include "GaudiKernel/StatusCode.h"

#include "TrkAlgebraUtils/AlSymMat.h"
#include "TrkAlgebraUtils/AlSpaMat.h"
#include "TrkAlgebraUtils/AlVec.h"
#include "TrkAlgebraUtils/AlMat.h"

#include <algorithm>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <float.h>
#include <stdint.h>
#include <stdexcept>

#include <TMatrixDSparse.h>


#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCholesky>


namespace Trk {

//______________________________________________________________________________
AlSpaMat::AlSpaMat()
{
  m_matrix_type = 2;
  m_size = 0;
  m_nele = 0;
  m_ptr_row = nullptr;
  m_ptr_col = nullptr;    // set pointer to null
}

//______________________________________________________________________________
AlSpaMat::AlSpaMat(long int N)
{
  m_matrix_type = 2;
  m_size = N;
  m_nele = 0;
  m_ptr_row = nullptr;
  m_ptr_col = nullptr;    // set pointer to null
}

//______________________________________________________________________________
AlSpaMat::AlSpaMat(const AlSpaMat& m)
 : AlSymMatBase(m)
{
  m_matrix_type = 2;
  m_size = m.size();
  m_ptr_row = nullptr;
  m_ptr_col = nullptr;    // set pointer to null
  copy(m);
}

//______________________________________________________________________________
AlSpaMat::AlSpaMat(const AlSymMat& m)
{
  m_matrix_type = 2;
  m_size = m.size();
  m_ptr_row = nullptr;
  m_ptr_col = nullptr;    // set pointer to null
  copy(m);
}

//______________________________________________________________________________
AlSpaMat::~AlSpaMat()
{
  m_ptr_map.clear();
  if( m_ptr_row != nullptr )  delete [] m_ptr_row;
  if( m_ptr_col != nullptr )  delete [] m_ptr_col;
}

//______________________________________________________________________________
void AlSpaMat::copy(const AlSpaMat& m)
{
  if( size() != m.size()) {
    throw std::range_error( "AlSpaMat::copy: size do not match!" );
  }
  m_ptr_map.clear();
  m_nele=m.m_nele;
  m_ptr_map = m.m_ptr_map;
  return;
}

//______________________________________________________________________________
void AlSpaMat::copy(const AlSymMat& m)
{
  if( size() != m.size()) {
    throw std::range_error( "AlSpaMat::copy: size do not match!" );
  }
  m_ptr_map.clear();
  m_nele=0;

  // Convert Storage System
  for (int i = 0; i<m_size; i++)
    for (int j = 0; j<=i; j++)
      if  (m.elemc(i,j) != 0.) {
        m_ptr_map.insert(std::make_pair(std::make_pair(i,j), m.elemc(i,j)));
        m_nele++;
      }

  return;
}

//______________________________________________________________________________
void AlSpaMat::copy(const AlMat& m)
{
  if( size() != m.nrow() || size() != m.ncol() ) {
    throw std::range_error( "AlSpaMat::copy: size do not match!" );
  }

  // copy just the lower triangle:
  m_ptr_map.clear();
  m_nele=0;

  //Convert Storage System
  for (int i = 0; i<m_size; i++)
    for (int j = 0; j<=i; j++)
      if  (m.elemc(i,j) != 0.) {
        m_ptr_map.insert(std::make_pair(std::make_pair(i,j), m.elemc(i,j)));
        m_nele++;
      }

  return;
}

//______________________________________________________________________________
double& AlSpaMat::elemr(long int i,long int j)
{
#ifdef _DEBUG
  if( i<0 ) {
    throw std::out_of_range( "AlSpaMat::elemr: Index 1 < zero! " );
  }
  if( i>=size() ) {
    throw std::out_of_range( "AlSpaMat::elemr: Index 1 too large! " );
  }
  if( j<0 ) {
    throw std::out_of_range( "AlSpaMat::elemr: Index 2 < zero! " );
  }
  if( j>=size() ) {
    throw std::out_of_range("AlSpaMat::elemr: Index 2 too large! " );;
  }
#endif
  // try fast referencing:
  mapiterator pos;
  indices key = j < i ? std::make_pair(i,j) : std::make_pair(j,i);
  pos=m_ptr_map.find(key);
  if( pos!=m_ptr_map.end() )    // already exists
    return pos->second;
  else {                      // does not yet exist
    // create it with the null value:
    m_nele++;
    return (m_ptr_map.insert(std::make_pair(key, 0.))).first->second;
  }
  // simpler implementation of the above:
  /*
    m_ptr_map[key]=0;
    m_nele = m_ptr_map.size();
    return m_ptr_map[key];
  */
}

//______________________________________________________________________________
double  AlSpaMat::elemc(long int i,long int j) const
{
#ifdef _DEBUG
  if( i<0 ) {
    throw std::out_of_range( "AlSpaMat::elemc: Index 1 < zero! " );
  }
  if( i>=size() ) {
    throw std::out_of_range( "AlSpaMat::elemc: Index 1 too large! " );
  }
  if( j<0 ) {
    throw std::out_of_range( "AlSpaMat::elemc: Index 2 < zero! " );
  }
  if( j>=size() ) {
    throw std::out_of_range( "AlSpaMat::elemc: Index 2 too large! " );
  }
#endif
  // try fast referencing:
  const_mapiterator pos;
  indices key = j < i ? std::make_pair(i,j) : std::make_pair(j,i);
  pos=m_ptr_map.find(key);

  if( pos!=m_ptr_map.end() )    // already exists
    return pos->second;

  // does not yet exist
  // return zero:
  return 0.;
}

//______________________________________________________________________________
indices  AlSpaMat::elem(long int i,long int j) const
{
  // ATTENTION! the key value is returned:
#ifdef _DEBUG
  if( i<0 ) {
    throw std::out_of_range( "AlSpaMat::elem: Index 1 < zero! " );
  }
  if( i>=size() ) {
    throw std::out_of_range( "AlSpaMat::elem: Index 1 too large! " );
  }
  if( j<0 ) {
    throw std::out_of_range( "AlSpaMat::elem: Index 2 < zero! " );
  }
  if( j>=size() ) {
    throw std::out_of_range( "AlSpaMat::elem: Index 2 too large! " );
  }
#endif

  indices key = j < i ? std::make_pair(i,j) : std::make_pair(j,i);
  return key;
}

//______________________________________________________________________________
AlSpaMat&  AlSpaMat::operator=(const AlSpaMat& m)
{
  
 if ( this != &m ) {
    m_size=m.size();
    copy(m);
  }
  return *this;
}

//______________________________________________________________________________
AlSpaMat&  AlSpaMat::operator=(const AlSymMat& m)
{
  m_size=m.size();
  copy(m);
  return *this;
}

//______________________________________________________________________________
AlSpaMat&  AlSpaMat::operator=(const AlMat& m)
{
  if( m.nrow() != m.ncol() ) {
    throw std::range_error( "AlSpaMat::=operator: allowed for square matrices only!" );
  }

  m_size=m.nrow();
  copy(m);
  return *this;
}

//______________________________________________________________________________
AlSpaMat&  AlSpaMat::operator=(const double& d)
{
  mapiterator pos;
  for (pos = m_ptr_map.begin(); pos!=m_ptr_map.end(); pos++)
    pos->second = d;

  return *this;
}

//______________________________________________________________________________
AlSpaMat AlSpaMat::operator+(const AlSpaMat& m) const
{
  if( size() != m.size()) {
    throw std::range_error(  "AlSpaMat::operator+: size do not match!" );
  }

  AlSpaMat b(m);
  const_mapiterator pos;
  for (pos = m_ptr_map.begin(); pos!=m_ptr_map.end(); pos++)
    b.m_ptr_map[pos->first] += pos->second;
  b.m_nele = b.m_ptr_map.size();

  return b;
}

//______________________________________________________________________________
AlSpaMat&  AlSpaMat::operator+=(const AlSpaMat& m)
{
  if( size() != m.size()) {
    throw std::range_error( "AlSpaMat::operator+=: size do not match!" );
  }

  const_mapiterator pos;
  for (pos = m.m_ptr_map.begin(); pos!=m.m_ptr_map.end(); pos++)
    (*this).m_ptr_map[pos->first] += pos->second;
  m_nele = m_ptr_map.size();

  return *this;
}

//______________________________________________________________________________
AlSpaMat AlSpaMat::operator-(const AlSpaMat& m) const
{
  if( size() != m.size()) {
    throw std::range_error(  "AlSpaMat::operator-: size do not match!" );
  }

  AlSpaMat b(m);
  const_mapiterator pos;
  for (pos = m_ptr_map.begin(); pos!=m_ptr_map.end(); pos++)
    b.m_ptr_map[pos->first] -= pos->second;
  b.m_nele = b.m_ptr_map.size();

  return b;
}

//______________________________________________________________________________
AlSpaMat&  AlSpaMat::operator-=(const AlSpaMat& m)
{
  if( size() != m.size()) {
    throw std::range_error(  "AlSpaMat::operator-=: size do not match!" );
  }

  const_mapiterator pos;
  for (pos = m.m_ptr_map.begin(); pos!=m.m_ptr_map.end(); pos++)
    (*this).m_ptr_map[pos->first] -= pos->second;
  m_nele = m_ptr_map.size();

  return *this;
}

//______________________________________________________________________________
AlMat AlSpaMat::operator*(const AlSymMatBase& m) const
{
  if( size() != m.size() ) {
    throw std::range_error(  "AlSpaMat::operator*: size do not match!" );
  }

  long int  isiz(size());
  AlMat b( isiz, isiz);

  for( long int i=0; i<isiz; i++ )
    for( long int j=0; j<isiz; j++ )
      for( int k=0; k<isiz; k++)
        b.elemr(i,j) += elemc(i,k)*m.elemc(k,j);

  return b;
}

//______________________________________________________________________________
AlMat  AlSpaMat::operator*(const AlMat& m) const {
  if( size() != m.nrow() ) {
    throw std::range_error(  "AlSpaMat::operator*: size do not match!" );
  }

  long int  isiz(size());
  long int  icol(m.ncol());
  AlMat b( isiz, icol);

  for( long int i=0; i<isiz; i++ )
    for( long int j=0; j<icol; j++ )
      for( long int k=0; k<isiz; k++)
        b.elemr(i,j) += elemc(i,k)*m.elemc(k,j);

  return b;
}

//______________________________________________________________________________
AlVec AlSpaMat::operator*(const AlVec& v) const
{
  if( size() != v.size() ) {
    throw std::range_error(  "AlSpaMat::operator*: size do not match! " );
  }

  long int  isiz(size());
  AlVec b(isiz);

  for( long int i=0; i<isiz; i++ )
    for( long int j=0; j<isiz; j++ )
      b[i] += elemc(i,j)*v[j];

  return b;
}

//______________________________________________________________________________
AlSpaMat&  AlSpaMat::operator*=(const double& d)
{
  mapiterator pos;
  for (pos = m_ptr_map.begin(); pos!=m_ptr_map.end(); pos++)
    pos->second *= d;

  return *this;
}

//______________________________________________________________________________
AlSpaMat  AlSpaMat::operator*(const double& d) const
{
  AlSpaMat a(size());
  const_mapiterator pos;
  for (pos = m_ptr_map.begin(); pos!=m_ptr_map.end(); pos++)
    a.m_ptr_map.insert(std::make_pair(pos->first, (pos->second)*d));

  return a;
}

//______________________________________________________________________________
double AlSpaMat::determinant()
{
  throw std::invalid_argument( "AlSpaMat::determinant: not implemented!" );
}

int AlSpaMat::SolveWithEigen(AlVec& RHS){
 
  if(RHS.size() != size() ){
    throw std::range_error(  "AlSpaMat::SolveWithEigen vector size is incorrect" );
  }
  
  Eigen::VectorXd eigenBigVector( RHS.size() );
  for(int i(0); i< RHS.size(); ++i ){
    eigenBigVector[i] = RHS[i];
  }

  Eigen::SparseMatrix<double> eigenBigMatrix( m_size, m_size );
  
  typedef Eigen::Triplet<double> Triplet;
  std::vector<Triplet> tripletList;
  tripletList.reserve(m_nele);
  long int      i, j;
  long int counter(0);
  mapiterator pos;
  for (pos = m_ptr_map.begin(); pos!=m_ptr_map.end(); pos++){
    elem(pos->first, i, j);
    tripletList.push_back(Triplet(i,j,pos->second));
    if(i!=j) tripletList.push_back(Triplet(j,i,pos->second));
    counter++;
  }
  eigenBigMatrix.setFromTriplets(tripletList.begin(), tripletList.end());

  // Eigen::CholmodSupernodalLLT  is much much quicker (x50) so it would be great to move to that in the future
  // requires an external package SuiteSparse
  // BiCGSTAB was the fastest iterative solver in Eigen from a quick test
  // SimplicialLDLT was the fastest direct solver in Eigen (x2 slower than BiCGSTAB ) 

  // Eigen::BiCGSTAB<Eigen::SparseMatrix<double> > solver;
  Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > solver;

  solver.compute(eigenBigMatrix);
  if(solver.info()!=Eigen::Success) {
    // decomposition failed
    throw std::domain_error("AlSpaMat::SolveWithEigen: failed to compute: -- is the input matrix singular?" );
    return 1;
  }

  Eigen::VectorXd x = solver.solve( eigenBigVector );
  if(solver.info()!=Eigen::Success) {
  // solving failed
    throw std::domain_error("AlSpaMat::SolveWithEigen: Failed to solve: -- is your matrix singular? ");
    return 2;
  }
  
  //Copy results into vector
  for(int i(0); i< RHS.size(); ++i ){
    RHS[i] = x[i];
  }

  //Check accuracy
  Eigen::VectorXd residual = eigenBigMatrix * x - eigenBigVector;
  double sumresidual = 0;
  for( int i=0; i<residual.size(); ++i){
    sumresidual += fabs(residual[i]);
  }
  sumresidual /= (double) residual.size();
  
  if( sumresidual > 1e-3 ){
    throw std::overflow_error( "AlSpaMat::SolveWithEigen: your solution is no good! ");
    return 3;
  }

  return 0;


}



//______________________________________________________________________________
//jlove int AlSpaMat::diagonalize(char jobz, AlVec& w, AlMat& z) {
int AlSpaMat::diagonalize(char, AlVec&, AlMat&)
{
  throw std::invalid_argument( "AlSpaMat::diagonalize: not implemented!" );
}

//______________________________________________________________________________
int AlSpaMat::invert()
{
  throw std::invalid_argument( "AlSpaMat::invert: not implemented!" );
}

//______________________________________________________________________________
void AlSpaMat::RemoveDoF(int index, int nelem)
{
  // index here is the first alignment parameter to remove, nelem number of consecutive ones

  m_size-=nelem;
  long int n=index+nelem-1;
  mapiterator pos;
  mapiterator pos_obs=m_ptr_map.end();
  for (pos = m_ptr_map.begin(); pos!=m_ptr_map.end(); pos++) {

    if( pos_obs!=m_ptr_map.end() )
      m_ptr_map.erase(pos_obs);

    pos_obs = m_ptr_map.end();

    long int k, l;
    elem(pos->first, k, l);

    if( (k>=index && k<=n) || (l>=index && l<=n) )
      pos_obs = pos;
    else {
      if( k > n )
        k -= nelem;
      if( l > n )
        l -= nelem;
      if( k >= index || l >= index )  {
        elemr(k, l) = pos->second;
        pos_obs = pos;
      }
    }
  }

  if( pos_obs!=m_ptr_map.end() )
    m_ptr_map.erase(pos_obs);
  m_nele = m_ptr_map.size();
}

//______________________________________________________________________________
int AlSpaMat::RemoveCollsRows(std::vector<int> indices)
{
  int n = indices.size();
  if (n==0) {
    return m_size;
  }
  if (n>m_size) {
    throw std::invalid_argument( "AlSpaMat::RemoveCollsRows: Vector of indices larger than matrix size." );
    return m_size;
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
    if (indices[i] > m_size-1) {
      throw std::out_of_range( "AlSpaMat::RemoveCollsRows: Index goes beyond matrix." );
    }
    RemoveDoF(indices[i]);
  }

  return m_size;
}

//______________________________________________________________________________
void AlSpaMat::RemoveAlignPar(int index, int control)
{
  // Dynamic shiftRow
  int shiftRow = 1;
  int counterRow = 0;

  // Dynamic shiftCol
  int shiftCol;
  int counterCol = 0;

  index = index-control;

  mapiterator pos;
  indices key;

  for(int row=index; row<(size()-shiftRow); row++) {
    shiftCol = 0;
    counterCol = 0;

    for(int col=0; col<row+1; col++) {

      if (col==index) {
        shiftCol = 1;
        counterCol=0;
      }

      pos = m_ptr_map.find(std::make_pair(row+shiftRow,col+shiftCol));
      if( pos!=m_ptr_map.end() ) {    // exists
        elemr(row,col) = pos->second;
        m_ptr_map.erase(pos);
      }
      else {
        pos = m_ptr_map.find(std::make_pair(row,col));
        if( pos!=m_ptr_map.end() )
          m_ptr_map.erase(pos);
      }
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

  m_nele = m_ptr_map.size();
}

//______________________________________________________________________________
void AlSpaMat::RemoveModule(int index)
{
  // index here is module number within matrix (size=6*max_index)
  const int shift=6;
  const int ind=shift*index;

  RemoveDoF(ind, shift);
  return;
}

//______________________________________________________________________________
void AlSpaMat::reSize(long int n)
{
  // balanced binary tree!
  if( n!=m_size ) {
    // make a temporary copy:
    AlSpaMat m(*this);
    m_ptr_map.clear();
    m_size = n;
    long int i, j;
    mapiterator pos;
    for (pos = m.m_ptr_map.begin(); pos!=m.m_ptr_map.end(); pos++) {
      m.elem(pos->first, i, j);
      if( i<n && j<n )
        m_ptr_map.insert(*pos);
    }
  }

  m_nele = m_ptr_map.size();
  return;
}

//______________________________________________________________________________
void AlSpaMat::SetPathBin(const std::string &path)
{
  m_pathbin.assign(path);
}

//______________________________________________________________________________
void AlSpaMat::SetPathTxt(const std::string &path)
{
  m_pathtxt.assign(path);
}

//______________________________________________________________________________
//jlove StatusCode AlSpaMat::Write(const std::string &filename, bool binary,
//                           bool square, double scale, float version){
StatusCode AlSpaMat::Write(const std::string &filename, bool binary,
                           bool /*square*/, double, float version)
{
  std::ofstream outmat;

  int32_t msizz = 1000000+size();
  int32_t nelem = m_ptr_map.size();

  if(binary) {
    outmat.open((m_pathbin+filename).c_str(), std::ios::binary);
    if(outmat.fail())
      return StatusCode::FAILURE;
    // add 10^6 to the size to distinguish
    // from dense format
    outmat.write((char*)&msizz, sizeof (msizz));
    outmat.write((char*)&version, sizeof (version));
    outmat.write((char*)&nelem, sizeof (nelem));
  }
  else {
    outmat.open((m_pathtxt+filename).c_str());
    if(outmat.fail())
      return StatusCode::FAILURE;
    outmat.setf(std::ios::fixed);
    outmat.setf(std::ios::showpoint);
    outmat.precision(3);
    outmat << "msizz: " << std::setw(6) << msizz << std::endl;
    outmat << "nelem: " << std::setw(6) << nelem << std::endl;
    outmat << "AlSpaMat version: " << std::setw(6) << version << std::endl;
  }

  double melem=0;
  long int ii, jj;
  int32_t i, j;

  mapiterator pos;
  for (pos = m_ptr_map.begin(); pos!=m_ptr_map.end(); pos++) {
    melem = pos->second;
    elem(pos->first, ii, jj);   i=ii;  j=jj;     // just a type conversion
    if(binary) {
      outmat.write((char*)&(i), sizeof (i));
      outmat.write((char*)&(j), sizeof (j));
      outmat.write((char*)&(melem), sizeof (melem));
    }
    else
      outmat << std::setw(6) << i  << std::setw(6) << j << std::setw(18) << melem << std::endl;
  }
  outmat.close();

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlSpaMat::CheckMatVersion(const std::string filename, bool &StdUnits)
{
  std::ifstream inmat((filename).c_str(), std::ios::binary);
  if(inmat.fail())
    return StatusCode::FAILURE;

  m_ptr_map.clear();

  int32_t msiz=0;
  inmat.read((char*)&msiz, sizeof (msiz));

  float version=0.0;
  inmat.read((char*)&version, sizeof (version));

  if (version>=2.0)
    StdUnits = true;
  else
    StdUnits = false;

  inmat.close();

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlSpaMat::Read(const std::string &filename, int &dofs, bool &triang, float &version)
{
  bool m_StdUnits = true;
  if (StatusCode::SUCCESS != CheckMatVersion(m_pathbin+filename, m_StdUnits))
    return StatusCode::FAILURE;

  std::ifstream inmat((m_pathbin+filename).c_str(), std::ios::binary);
  if(inmat.fail())
    return StatusCode::FAILURE;

  m_ptr_map.clear();

  int32_t msiz=0;
  int32_t nelem;
  inmat.read((char*)&msiz, sizeof (msiz));
  if( msiz>999999 )
    dofs = msiz-1000000;
  else
    dofs = abs(msiz);
  m_size = dofs;

  if (m_StdUnits)
    inmat.read((char*)&version, sizeof (version));

  double melem=0;
  int32_t i, j;

  if(msiz>999999) { // sparse format
    triang=false;
    inmat.read((char*)&nelem, sizeof (nelem));
    m_nele=nelem;
    for(int k=0; k<nelem; k++) {
      inmat.read((char*)&i, sizeof (i));
      inmat.read((char*)&j, sizeof (j));
      inmat.read((char*)&melem, sizeof (melem));
      m_ptr_map.insert(std::make_pair(std::make_pair(i,j), melem));
    }
  }
  else if(msiz>0) { // square format
    triang=false;
    for(int32_t i=0; i<msiz; i++) {
      for(int32_t j=0; j<msiz; j++) {
        inmat.read((char*)&melem, sizeof (melem));
        if( i>=j && melem!=0. )
          m_ptr_map.insert(std::make_pair(std::make_pair(i,j), melem));
      }
    }
  }
  else { // triangular format
    triang=true;
    msiz = (-1)*msiz;
    for( int32_t i=0; i<msiz; i++) {
      for( int32_t j=0; j<=i; j++) {
        inmat.read((char*)&melem, sizeof (melem));
        if( melem!=0. )
          m_ptr_map.insert(std::make_pair(std::make_pair(i,j), melem));
      }
    }
  }

  m_nele = m_ptr_map.size();
  inmat.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlSpaMat::ReadProjected(const std::string &filename, int &dofs,
                                   bool &triang, float &version)
{
  std::ifstream inmat((m_pathbin+filename).c_str(), std::ios::binary);
  if(inmat.fail())
    return StatusCode::FAILURE;

  m_ptr_map.clear();

  int32_t msiz=0;
  int32_t nelem;
  inmat.read((char*)&msiz, sizeof (msiz));
  if( msiz>999999 )
    dofs = msiz-1000000;
  else
    dofs = abs(msiz);
  m_size = dofs;

  inmat.read((char*)&version, sizeof (version));

  double melem=0;
  int32_t i, j;

  if(msiz>999999) { // sparse format
    triang=false;
    inmat.read((char*)&nelem, sizeof (nelem));
    m_nele=nelem;
    for(int k=0; k<nelem; k++) {
      inmat.read((char*)&i, sizeof (i));
      inmat.read((char*)&j, sizeof (j));
      inmat.read((char*)&melem, sizeof (melem));
      m_ptr_map.insert(std::make_pair(std::make_pair(i,j), melem));
    }
  }
  else if(msiz>0) { // square format
    triang=false;
    for(int32_t i=0; i<msiz; i++) {
      for(int32_t j=0; j<msiz; j++) {
        inmat.read((char*)&melem, sizeof (melem));
        if( i>=j && melem!=0. )
          m_ptr_map.insert(std::make_pair(std::make_pair(i,j), melem));
      }
    }
  }
  else { // triangular format
    triang=true;
    msiz = (-1)*msiz;
    for( int32_t i=0; i<msiz; i++) {
      for( int32_t j=0; j<=i; j++) {
        inmat.read((char*)&melem, sizeof (melem));
        if( melem!=0. )
          m_ptr_map.insert(std::make_pair(std::make_pair(i,j), melem));
      }
    }
  }

  m_nele = m_ptr_map.size();
  inmat.close();
  return StatusCode::SUCCESS;
}

TMatrixDSparse* AlSpaMat::makeTMatrix()
{

  long int nonZeroElements = m_ptr_map.size();
  int    *irow = new int[nonZeroElements];
  int    *icol = new int[nonZeroElements];
  double *val  = new double[nonZeroElements];

  long int      i, j;
  long int counter(0);
  const_mapiterator pos = m_ptr_map.begin();
  for(pos=m_ptr_map.begin(); pos!=m_ptr_map.end(); pos++){
    i = pos->first.first;
    j = pos->first.second;
    *(val+counter)=pos->second;
    *(irow+counter)= i;
    *(icol+counter)= j;
     counter++;
  }

  TMatrixDSparse* myTMatrix = new TMatrixDSparse(0,m_size-1,0,m_size-1);
  myTMatrix->SetMatrixArray(nonZeroElements,irow,icol,val);

  delete [] irow;
  delete [] icol;
  delete [] val;
  return myTMatrix;
}



} // end namespace Trk
