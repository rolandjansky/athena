/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"

#include "TrkAlgebraUtils/AlVec.h"
#include "TrkAlgebraUtils/AlMat.h"
#include "TrkAlgebraUtils/AlSymMat.h"

#include <iomanip>
#include <fstream>
#include <stdint.h>
#include <math.h>
#include <exception>

namespace Trk {

//______________________________________________________________________________
AlVec::AlVec() {
  m_size = 0;
  m_ptr_data = 0;  // set pointer to null
  m_pathbin="./";
  m_pathtxt="./";
}

//______________________________________________________________________________
AlVec::AlVec(int N) {
  m_size = N;
  m_ptr_data = new double[m_size];
  m_pathbin="./";
  m_pathtxt="./";

  double*  p = m_ptr_data + m_size;
  while (p > m_ptr_data) *(--p) = 0.;
}

//______________________________________________________________________________
AlVec::AlVec(const AlVec& v) {
  m_size   = v.m_size;
  m_pathbin = v.m_pathbin;
  m_pathtxt = v.m_pathtxt;
  m_ptr_data = new double[m_size];

  copy(v);
}

//______________________________________________________________________________
AlVec::~AlVec()
{
  delete [] m_ptr_data;
}

//______________________________________________________________________________
void AlVec::copy(const AlVec& v) {
  if(m_size!=v.m_size) {
    throw std::range_error( "AlVec Assignment: size does not match!" );
  }

  double*  p = m_ptr_data + m_size;
  double*  q = v.m_ptr_data + m_size;
  while (p > m_ptr_data) *(--p) = (*(--q));
}

//______________________________________________________________________________
AlVec&  AlVec::operator=(const double& d) {

  double*  p = m_ptr_data + m_size;
  while (p > m_ptr_data) *(--p) = d;

  return *this;
}

//______________________________________________________________________________
AlVec& AlVec::operator=(const AlVec& v) {
  if (this==&v) return *this;

  if(m_size!=0 && m_size!=v.m_size) {
    throw std::range_error( "AlVec Assignment: size does not match!" );
  }

  if ( m_ptr_data != v.m_ptr_data )  copy(v);

  return *this;
}

//______________________________________________________________________________
AlVec AlVec::operator+(const AlVec& v) const {
  if( m_size != v.m_size ) {
    throw std::range_error(  "operator+: vectors size does not match!" );
  }

  AlVec b(m_size);

  double*  p = m_ptr_data + m_size;
  double*  q = v.m_ptr_data + m_size;
  double*  r = b.m_ptr_data + m_size;
  while (p > m_ptr_data) *(--r) = (*(--p))+(*(--q));

  return b;
}

//______________________________________________________________________________
AlVec& AlVec::operator+=(const AlVec& v) {
  if( m_size != v.m_size ) {
    throw std::range_error(  "operator+=: vectors size does not match!" );
  }

  double*  p = m_ptr_data + m_size;
  double*  q = v.m_ptr_data + m_size;
  while (p > m_ptr_data) *(--p) += (*(--q));

  return *this;
}

//______________________________________________________________________________
AlVec AlVec::operator-(const AlVec& v) const {
  if( m_size != v.m_size ) {
    throw std::range_error(  "operator-: vectors size does not match!" );
  }

  AlVec b(m_size);

  double*  p = m_ptr_data + m_size;
  double*  q = v.m_ptr_data + m_size;
  double*  r = b.m_ptr_data + m_size;
  while (p > m_ptr_data) *(--r) = (*(--p))-(*(--q));

  return b;
}

//______________________________________________________________________________
AlVec& AlVec::operator-=(const AlVec& v) {
  if( m_size != v.m_size ) {
    throw std::range_error(  "operator+=: vectors size does not match!" );
  }

  double*  p = m_ptr_data + m_size;
  double*  q = v.m_ptr_data + m_size;
  while (p > m_ptr_data) *(--p) -= (*(--q));

  return *this;
}

//______________________________________________________________________________
double AlVec::operator*(const AlVec& v) const {
  double  b=0.;
  if( m_size != v.m_size ) {
    throw std::range_error(  "scalar product: vectors size does not match!" );
  }

  double*  p = m_ptr_data + m_size;
  double*  q = v.m_ptr_data + m_size;
  while (p > m_ptr_data) b += *(--p)*(*(--q));

  return b;
}

//______________________________________________________________________________
AlVec AlVec::operator*(const AlMat& m) const {
  if (m_size != m.nrow()) {
    throw std::range_error(  "Left hand vector-matrix multiplication: size does not match!" );
  }

  AlVec b(m.ncol());

  for (int i=0;i<m.ncol();i++) {
    for (int j=0;j<m_size;j++)  *(b.m_ptr_data+i) += *(m_ptr_data+j)*m.elemc(j, i);
  }
  return b;
}

//______________________________________________________________________________
AlVec  AlVec::operator*(const AlSymMatBase& m) const {
  if (m_size != m.size()) {
    throw std::range_error(  "Left hand vector-matrix multiplication: size does not match!" );
  }

  AlVec b(m_size);

  for (int i=0;i<m_size;i++) {
    for (int j=0;j<m_size;j++)  *(b.m_ptr_data+i) += *(m_ptr_data+j)*m.elemc(j, i);
  }
  return b;
}

//______________________________________________________________________________
AlVec& AlVec::operator*=(const double& d) {

  double*  p = m_ptr_data + m_size;
  while (p > m_ptr_data) *(--p) *= d;

  return *this;
}

//______________________________________________________________________________
// norm of the vector
// squared root of squared size
double AlVec::norm() const {

  double result(0.);
  double * p = m_ptr_data + m_size;
  while (p > m_ptr_data) {
    --p;
    result += *p * *p ;
  }

  return sqrt(result);
}

//______________________________________________________________________________
void AlVec::reSize(int Nnew) {
  if ( Nnew>=0 && Nnew != m_size ) {
    double*  p = m_ptr_data;
    int m_size_old = m_size;
    m_ptr_data = new double[Nnew];
    m_size = Nnew;
    int k = m_size <= m_size_old ? m_size : m_size_old;

    p += k;
    double*  q = m_ptr_data + k;
    while (q > m_ptr_data) *(--q) = *(--p);

    delete [] p;
  }
}

//______________________________________________________________________________
void AlVec::RemoveModule(int index) {
  // Static shift
  const int shift=6;
  const int start = shift*index;

  if(start<m_size && index>=0){
    for(int row=start; row<m_size; row++) {
      *(m_ptr_data+row)=*(m_ptr_data+row+shift);
    }
  }
}

//______________________________________________________________________________
void AlVec::RemoveAlignPar(int index, int control)
{
  // Dynamic shift
  int shift = 1;
  int counter = 0;

  index = index-control;
  //  std::cout << "index: " << index << " - control: " << control << std::endl;

  for(int row=index; row<m_size; row++) {
    *(m_ptr_data+row) = *(m_ptr_data+row+shift);
    counter++;
    if (counter==5-control) { counter=0; shift++; }
  }
}

//______________________________________________________________________________
int AlVec::RemoveElements(std::vector<int> indices)
{
  int n = indices.size();
  if (n==0) {
    return m_size;
  }
  if (n>m_size) {
    throw std::range_error( "Vector of indices larger than matrix size." );
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

  // remove elements starting from largest indices
  for (int i=0;i<n;i++) {
    int index = indices[i];
    if (index > m_size-1) {
      throw std::out_of_range( "AlVec::RemoveElements: Index  goes beyond matrix " );
      continue;
    }

    for (int j=index; j<m_size-1; j++)
      *(m_ptr_data+j) = *(m_ptr_data+j+1);

    m_size--;
  }

  return m_size;
}

//______________________________________________________________________________
void AlVec::SetPathBin(const std::string &path)
{
  m_pathbin.assign(path);
}

//______________________________________________________________________________
void AlVec::SetPathTxt(const std::string &path)
{
  m_pathtxt.assign(path);
}

//______________________________________________________________________________
StatusCode AlVec::Write(const std::string &filename, bool binary, double scale,
                        std::map<int,unsigned long long> moduleIndexMap, float version)
{
  std::ofstream outvec;
  int32_t  io_size=m_size;
//  int32_t  io_scale=scale;

  if(binary) {
    outvec.open((m_pathbin+filename).c_str(), std::ios::binary);
    if(outvec.fail())
      return StatusCode::FAILURE;
    outvec.write((char*)&io_size, sizeof (io_size));
    outvec.write((char*)&scale, sizeof (scale));
    outvec.write((char*)&version, sizeof (version));
  }
  else {
    outvec.open((m_pathtxt+filename).c_str());
    if(outvec.fail())
      return StatusCode::FAILURE;
    outvec.setf(std::ios::fixed);
    outvec.setf(std::ios::showpoint);
    outvec.precision(6);
    outvec << "DoF: " << std::setw(6) << m_size << std::endl;
    outvec << "scale: " << std::setw(18) << scale << std::endl;
    outvec << "AlVec version: " << std::setw(6) << version << std::endl;
  }

  int64_t ielem=0;
  double  velem=0;
  std::map<int,unsigned long long>::iterator itcod;

  for( int i=0; i<m_size; i++) {
    itcod=moduleIndexMap.find(i/6);
    if (itcod != moduleIndexMap.end()) {
      ielem = (itcod->second);
    }

    velem = *(m_ptr_data+i);

    if(binary){
      outvec.write((char*)&(ielem), sizeof (ielem));
      outvec.write((char*)&(velem), sizeof (velem));
    }
    else
      outvec << std::setw(20) << ielem << std::setw(18) << velem << std::endl;
  }
  outvec.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlVec::WritePartial(const std::string &filename, bool binary, double scale,
			       std::map<int,unsigned long long> moduleIndexMap, float version)
{
  std::ofstream outvec;

  /*
    int32_t  io_size=m_size;
    //  int32_t  io_scale=scale;
    
    if(binary) {
    outvec.open((m_pathbin+filename).c_str(), std::ios::binary);
    if(outvec.fail())
    return StatusCode::FAILURE;
    outvec.write((char*)&io_size, sizeof (io_size));
    outvec.write((char*)&scale, sizeof (scale));
    outvec.write((char*)&version, sizeof (version));
    
    }
    else {
    outvec.open((m_pathtxt+filename).c_str());
    if(outvec.fail())
    return StatusCode::FAILURE;
    outvec.setf(std::ios::fixed);
    outvec.setf(std::ios::showpoint);
    outvec.precision(6);
    outvec << "DoF: " << std::setw(6) << m_size << std::endl;
    outvec << "scale: " << std::setw(18) << scale << std::endl;
    outvec << "AlVec version: " << std::setw(6) << version << std::endl;
    }
  */
  
  StatusCode sc=InitializeOutputVector(filename,binary,scale,version,outvec);
  if (sc!=StatusCode::SUCCESS) return StatusCode::FAILURE;

  int64_t ielem=0;
  double velem=0;
  std::map<int,unsigned long long>::iterator itcod;

  for( int i=0; i<m_size; i++) {
    itcod=moduleIndexMap.find(i);
    if (itcod != moduleIndexMap.end())
      ielem = (itcod->second);

    velem = *(m_ptr_data+i);

    if(binary){
      outvec.write((char*)&(ielem), sizeof (ielem));
      outvec.write((char*)&(velem), sizeof (velem));
    }
    else
      outvec << std::setw(20) << ielem << std::setw(18) << velem << std::endl;
  }
  outvec.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlVec::WritePartial(const std::string &filename, bool binary, double scale,
			       std::map<int,std::string> moduleNameMap, float version)
{
  std::ofstream outvec;
  
  StatusCode sc=InitializeOutputVector(filename,binary,scale,version,outvec);
  if (sc!=StatusCode::SUCCESS) return StatusCode::FAILURE;
  
  std::string elem="";
  double velem=0;
  std::map<int,std::string>::iterator itcod;
  
  for( int i=0; i<m_size; i++) {
    itcod=moduleNameMap.find(i);
    if (itcod != moduleNameMap.end()) {
      elem = (itcod->second);
    }

    velem = *(m_ptr_data+i);
        
    if(binary){    
      std::cout<<"can't write module name in binary output!"<<std::endl;
      return StatusCode::FAILURE;
    }
    else
      outvec << std::setw(20) << elem << std::setw(18) << velem << std::endl;
  }
  outvec.close();
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode AlVec::InitializeOutputVector(const std::string& filename, bool binary, double scale, 
					 float version, std::ofstream& outvec)
{
  int32_t  io_size=m_size;
  
  if(binary) {
    outvec.open((m_pathbin+filename).c_str(), std::ios::binary);
    if(outvec.fail()) 
      return StatusCode::FAILURE;
    outvec.write((char*)&io_size, sizeof (io_size));
    outvec.write((char*)&scale,   sizeof (scale));
    outvec.write((char*)&version, sizeof (version));
    
  }
  else {
    outvec.open((m_pathtxt+filename).c_str());
    if(outvec.fail()) 
      return StatusCode::FAILURE;
    outvec.setf(std::ios::fixed);
    outvec.setf(std::ios::showpoint);
    outvec.precision(6);
    outvec << "DoF: " << std::setw(6) << m_size << std::endl;
    outvec << "scale: " << std::setw(18) << scale << std::endl;    
    outvec << "AlVec version: " << std::setw(6) << version << std::endl;    
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlVec::ReadPartial(const std::string &filename, double &scale, 
			      std::map<int,unsigned long long> &modmap, float &version)
{
  bool m_StdUnits = true;
  if (StatusCode::SUCCESS != CheckVecVersion(m_pathbin+filename, m_StdUnits)) {
    //std::cout<<"CheckVecVersion failed"<<std::endl;
    return StatusCode::FAILURE;
  }

  std::ifstream invec((m_pathbin+filename).c_str(), std::ios::binary);
  if(invec.fail()) {
    //std::cout<<"ifstream failed"<<std::endl;
    return StatusCode::FAILURE;
  }

  int32_t vsiz=0;
  invec.read((char*)&vsiz, sizeof (vsiz));
  m_size  = vsiz;

//  int32_t io_scale;
  invec.read((char*)&scale, sizeof(scale));
//  scale=io_scale;

  if (m_StdUnits)
    invec.read((char*)&version, sizeof (version));

//  std::cout << "AlVec::StdUnits: " << m_StdUnits << std::endl;
//  std::cout << "AlVec::scale: " << scale << std::endl;
//  std::cout << "AlVec::version: " << version << std::endl;

  int64_t ielem=0;
  double  velem=0.0;
  for( int i=0; i<m_size; i++) {
    invec.read((char*)&ielem, sizeof (ielem));
    modmap[i] = ielem;

    invec.read((char*)&velem, sizeof (velem));
    *(m_ptr_data+i) = velem;

    // std::cout << "AlVec (" << i << "):: " << ielem << ", " << velem << std::endl;

  }

  invec.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlVec::CheckVecVersion(const std::string filename, bool &StdUnits){
  std::ifstream invec((filename).c_str(), std::ios::binary);
  if(invec.fail())
    return StatusCode::FAILURE;

  int32_t vsiz=0;
  invec.read((char*)&vsiz, sizeof (vsiz));

//  int32_t scale=0;
  double scale=0.;
  invec.read((char*)&scale, sizeof (scale));

  float version=0.0;
  invec.read((char*)&version, sizeof (version));

  if (version>=2.0) StdUnits = true;
  else StdUnits = false;

  invec.close();

  // std::cout << "AlVec::StdUnits: " << StdUnits << std::endl;
  // std::cout << "AlVec::scale: " << scale << std::endl;
  // std::cout << "AlVec::vsiz: " << vsiz << std::endl;
  // std::cout << "AlVec::version: " << version << std::endl;

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlVec::Read(const std::string &filename, double &scale,
                       std::map<int,unsigned long long> &modmap, float &version)
{

  bool m_StdUnits = true;
  if (StatusCode::SUCCESS != CheckVecVersion(m_pathbin+filename, m_StdUnits)) {
    //std::cout<<"CheckVecVersion failed"<<std::endl;
    return StatusCode::FAILURE;
  }

  std::ifstream invec((m_pathbin+filename).c_str(), std::ios::binary);
  if(invec.fail())
    return StatusCode::FAILURE;

  int32_t vsiz=0;
  invec.read((char*)&vsiz, sizeof (vsiz));
  m_size  = vsiz;
//  std::cout<<"size="<<m_size<<std::endl;

//  int32_t io_scale;
  invec.read((char*)&scale, sizeof (scale));
//  scale=io_scale;
//  std::cout<<"scale="<<scale<<std::endl;

  if (m_StdUnits)
    invec.read((char*)&version, sizeof (version));

//  std::cout << "AlVec::StdUnits: " << m_StdUnits << std::endl;
//  std::cout << "AlVec::scale: " << scale << std::endl;
//  std::cout << "AlVec::version: " << version << std::endl;

  int64_t ielem=0;
  double  velem=0.0;
  for( int i=0; i<m_size; i++) {
    invec.read((char*)&ielem, sizeof (ielem));
    modmap[i/6] = ielem;

    invec.read((char*)&velem, sizeof (velem));
    *(m_ptr_data+i) = velem;

    // std::cout << "AlVec (" << i << "):: " << ielem << ", " << velem << std::endl;

  }

  invec.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlVec::ReadProjected(const std::string &filename, double &scale,
                                std::map<int,unsigned long long> &modmap, float &version)
{
  std::ifstream invec((m_pathbin+filename).c_str(), std::ios::binary);
  if(invec.fail())
    return StatusCode::FAILURE;

  int32_t vsiz=0;
  invec.read((char*)&vsiz, sizeof (vsiz));
  m_size  = vsiz;

//  int32_t io_scale;
  invec.read((char*)&scale, sizeof (scale));
//  scale=io_scale;

  invec.read((char*)&version, sizeof (version));

  // std::cout << "AlVec::scale: " << scale << std::endl;
  // std::cout << "AlVec::version: " << version << std::endl;

  int64_t ielem=0;
  double  velem=0.0;
  for( int i=0; i<m_size; i++) {
    invec.read((char*)&ielem, sizeof (ielem));
    modmap[i/6] = ielem;

    invec.read((char*)&velem, sizeof (velem));
    *(m_ptr_data+i) = velem;

    // std::cout << "AlVec (" << i << "):: " << ielem << ", " << velem << std::endl;

  }

  invec.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlVec::ReadScalaPack(const std::string &filename){
  std::ifstream eigenvec(filename.c_str(), std::ios::binary);
  if(eigenvec.fail())
    return StatusCode::FAILURE;

  int32_t vsiz=0;
  eigenvec.read((char*)&vsiz, sizeof (vsiz));
  m_size=vsiz;

  double velem=0;
  for( int i=0; i<m_size; i++) {
    eigenvec.read((char*)&velem, sizeof (velem));
    // printf("v[%d] = %.16lf \n",i,velem);
    *(m_ptr_data+i) = velem;
  }

  eigenvec.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlVec::WriteEigenvalueVec(const std::string &filename, bool binary){

  std::ofstream outvec;
  int32_t  io_size=m_size;

  if(binary) {
    outvec.open((m_pathbin+filename).c_str(), std::ios::binary);

    if(outvec.fail())
      return StatusCode::FAILURE;

    outvec.write((char*)&io_size, sizeof (io_size));
  }
  else{
    outvec.open((m_pathtxt+filename).c_str());

    if(outvec.fail())
      return StatusCode::FAILURE;

    outvec.setf(std::ios::fixed);
    outvec.setf(std::ios::showpoint);
    outvec.precision(6);
    outvec << "AlVec::DoF: " << std::setw(6) << m_size << std::endl;
  }

  //jlove int32_t ielem=0;
  double  velem=0;
//  std::map<int,int>::iterator itcod;

  for( int i=0; i<m_size; i++) {

    velem = *(m_ptr_data+i);

    if(binary)
      outvec.write((char*)&(velem), sizeof (velem));
    else
      outvec << std::setw(10) << i << std::setw(18) << velem << std::endl;
  }
  outvec.close();
  return StatusCode::SUCCESS;
}

} // end namespace Trk
