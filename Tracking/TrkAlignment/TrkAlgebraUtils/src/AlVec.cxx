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

namespace Trk {

//______________________________________________________________________________
AlVec::AlVec() {
  _size = 0;
  ptr_data = 0;  // set pointer to null
  m_pathbin="./";
  m_pathtxt="./";
}

//______________________________________________________________________________
AlVec::AlVec(int N) {
  _size = N;
  ptr_data = new double[_size];
  m_pathbin="./";
  m_pathtxt="./";

  double*  p = ptr_data + _size;
  while (p > ptr_data) *(--p) = 0.;
}

//______________________________________________________________________________
AlVec::AlVec(const AlVec& v) {
  _size   = v._size;
  m_pathbin = v.m_pathbin;
  m_pathtxt = v.m_pathtxt;
  ptr_data = new double[_size];

  copy(v);
}

//______________________________________________________________________________
AlVec::~AlVec()
{
  delete [] ptr_data;
}

//______________________________________________________________________________
void AlVec::copy(const AlVec& v) {
  if(_size!=v._size) {
    std::cerr << "AlVec Assignment: size does not match!" << std::endl;
    return;
  }

  double*  p = ptr_data + _size;
  double*  q = v.ptr_data + _size;
  while (p > ptr_data) *(--p) = (*(--q));
}

//______________________________________________________________________________
AlVec&  AlVec::operator=(const double& d) {

  double*  p = ptr_data + _size;
  while (p > ptr_data) *(--p) = d;

  return *this;
}

//______________________________________________________________________________
AlVec& AlVec::operator=(const AlVec& v) {
  if (this==&v) return *this;

  if(_size!=0 && _size!=v._size) {
    std::cerr << "AlVec Assignment: size does not match!" << std::endl;
    return *this;
  }

  if ( ptr_data != v.ptr_data )  copy(v);

  return *this;
}

//______________________________________________________________________________
AlVec AlVec::operator+(const AlVec& v) const {
  if( _size != v._size ) {
    std::cerr << "operator+: vectors size does not match!" << std::endl;
    return *this;
  }

  AlVec b(_size);

  double*  p = ptr_data + _size;
  double*  q = v.ptr_data + _size;
  double*  r = b.ptr_data + _size;
  while (p > ptr_data) *(--r) = (*(--p))+(*(--q));

  return b;
}

//______________________________________________________________________________
AlVec& AlVec::operator+=(const AlVec& v) {
  if( _size != v._size ) {
    std::cerr << "operator+=: vectors size does not match!" << std::endl;
    return *this;
  }

  double*  p = ptr_data + _size;
  double*  q = v.ptr_data + _size;
  while (p > ptr_data) *(--p) += (*(--q));

  return *this;
}

//______________________________________________________________________________
AlVec AlVec::operator-(const AlVec& v) const {
  if( _size != v._size ) {
    std::cerr << "operator-: vectors size does not match!" << std::endl;
    return *this;
  }

  AlVec b(_size);

  double*  p = ptr_data + _size;
  double*  q = v.ptr_data + _size;
  double*  r = b.ptr_data + _size;
  while (p > ptr_data) *(--r) = (*(--p))-(*(--q));

  return b;
}

//______________________________________________________________________________
AlVec& AlVec::operator-=(const AlVec& v) {
  if( _size != v._size ) {
    std::cerr << "operator+=: vectors size does not match!" << std::endl;
    return *this;
  }

  double*  p = ptr_data + _size;
  double*  q = v.ptr_data + _size;
  while (p > ptr_data) *(--p) -= (*(--q));

  return *this;
}

//______________________________________________________________________________
double AlVec::operator*(const AlVec& v) const {
  double  b=0.;
  if( _size != v._size ) {
    std::cerr << "scalar product: vectors size does not match!" << std::endl;
    return b;
  }

  double*  p = ptr_data + _size;
  double*  q = v.ptr_data + _size;
  while (p > ptr_data) b += *(--p)*(*(--q));

  return b;
}

//______________________________________________________________________________
AlVec AlVec::operator*(const AlMat& m) const {
  if (_size != m.nrow()) {
    std::cerr << "Left hand vector-matrix multiplication: size does not match!" << std::endl;
    return *this;
  }

  AlVec b(m.ncol());

  for (int i=0;i<m.ncol();i++) {
    for (int j=0;j<_size;j++)  *(b.ptr_data+i) += *(ptr_data+j)*m.elemc(j, i);
  }
  return b;
}

//______________________________________________________________________________
AlVec  AlVec::operator*(const AlSymMatBase& m) const {
  if (_size != m.size()) {
    std::cerr << "Left hand vector-matrix multiplication: size does not match!" << std::endl;
    return *this;
  }

  AlVec b(_size);

  for (int i=0;i<_size;i++) {
    for (int j=0;j<_size;j++)  *(b.ptr_data+i) += *(ptr_data+j)*m.elemc(j, i);
  }
  return b;
}

//______________________________________________________________________________
AlVec& AlVec::operator*=(const double& d) {

  double*  p = ptr_data + _size;
  while (p > ptr_data) *(--p) *= d;

  return *this;
}

//______________________________________________________________________________
// norm of the vector
// squared root of squared size
double AlVec::norm() const {

  double result(0.);
  double * p = ptr_data + _size;
  while (p > ptr_data) {
    --p;
    result += *p * *p ;
  }

  return sqrt(result);
}

//______________________________________________________________________________
void AlVec::reSize(int Nnew) {
  if ( Nnew>=0 && Nnew != _size ) {
    double*  p = ptr_data;
    int _size_old = _size;
    ptr_data = new double[Nnew];
    _size = Nnew;
    int k = _size <= _size_old ? _size : _size_old;

    p += k;
    double*  q = ptr_data + k;
    while (q > ptr_data) *(--q) = *(--p);

    delete [] p;
  }
}

//______________________________________________________________________________
void AlVec::RemoveModule(int index) {
  // Static shift
  const int shift=6;
  const int start = shift*index;

  if(start<_size && index>=0){
    for(int row=start; row<_size; row++) {
      *(ptr_data+row)=*(ptr_data+row+shift);
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

  for(int row=index; row<_size; row++) {
    *(ptr_data+row) = *(ptr_data+row+shift);
    counter++;
    if (counter==5-control) { counter=0; shift++; }
  }
}

//______________________________________________________________________________
int AlVec::RemoveElements(std::vector<int> indices)
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

  // remove elements starting from largest indices
  for (int i=0;i<n;i++) {
    int index = indices[i];
    if (index > _size-1) {
      std::cerr<<"Index "<<index<<" goes beyond matrix (size "<<_size<<")."<<std::endl;
      continue;
    }

    for (int j=index; j<_size-1; j++)
      *(ptr_data+j) = *(ptr_data+j+1);

    _size--;
  }

  return _size;
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
  int32_t  io_size=_size;
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
    outvec << "DoF: " << std::setw(6) << _size << std::endl;
    outvec << "scale: " << std::setw(18) << scale << std::endl;
    outvec << "AlVec version: " << std::setw(6) << version << std::endl;
  }

  int64_t ielem=0;
  double  velem=0;
  std::map<int,unsigned long long>::iterator itcod;

  for( int i=0; i<_size; i++) {
    itcod=moduleIndexMap.find(i/6);
    if (itcod != moduleIndexMap.end()) {
      ielem = (itcod->second);
    }

    velem = *(ptr_data+i);

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
    int32_t  io_size=_size;
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
    outvec << "DoF: " << std::setw(6) << _size << std::endl;
    outvec << "scale: " << std::setw(18) << scale << std::endl;
    outvec << "AlVec version: " << std::setw(6) << version << std::endl;
    }
  */
  
  StatusCode sc=InitializeOutputVector(filename,binary,scale,version,outvec);
  if (sc!=StatusCode::SUCCESS) return StatusCode::FAILURE;

  int64_t ielem=0;
  double velem=0;
  std::map<int,unsigned long long>::iterator itcod;

  for( int i=0; i<_size; i++) {
    itcod=moduleIndexMap.find(i);
    if (itcod != moduleIndexMap.end())
      ielem = (itcod->second);

    velem = *(ptr_data+i);

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
  
  for( int i=0; i<_size; i++) {
    itcod=moduleNameMap.find(i);
    if (itcod != moduleNameMap.end()) {
      elem = (itcod->second);
    }

    velem = *(ptr_data+i);
        
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
  int32_t  io_size=_size;
  
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
    outvec << "DoF: " << std::setw(6) << _size << std::endl;
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
    std::cout<<"CheckVecVersion failed"<<std::endl;
    return StatusCode::FAILURE;
  }

  std::ifstream invec((m_pathbin+filename).c_str(), std::ios::binary);
  if(invec.fail()) {
    std::cout<<"ifstream failed"<<std::endl;
    return StatusCode::FAILURE;
  }

  int32_t vsiz=0;
  invec.read((char*)&vsiz, sizeof (vsiz));
  _size  = vsiz;

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
  for( int i=0; i<_size; i++) {
    invec.read((char*)&ielem, sizeof (ielem));
    modmap[i] = ielem;

    invec.read((char*)&velem, sizeof (velem));
    *(ptr_data+i) = velem;

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
    std::cout<<"CheckVecVersion failed"<<std::endl;
    return StatusCode::FAILURE;
  }

  std::ifstream invec((m_pathbin+filename).c_str(), std::ios::binary);
  if(invec.fail())
    return StatusCode::FAILURE;

  int32_t vsiz=0;
  invec.read((char*)&vsiz, sizeof (vsiz));
  _size  = vsiz;
//  std::cout<<"size="<<_size<<std::endl;

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
  for( int i=0; i<_size; i++) {
    invec.read((char*)&ielem, sizeof (ielem));
    modmap[i/6] = ielem;

    invec.read((char*)&velem, sizeof (velem));
    *(ptr_data+i) = velem;

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
  _size  = vsiz;

//  int32_t io_scale;
  invec.read((char*)&scale, sizeof (scale));
//  scale=io_scale;

  invec.read((char*)&version, sizeof (version));

  // std::cout << "AlVec::scale: " << scale << std::endl;
  // std::cout << "AlVec::version: " << version << std::endl;

  int64_t ielem=0;
  double  velem=0.0;
  for( int i=0; i<_size; i++) {
    invec.read((char*)&ielem, sizeof (ielem));
    modmap[i/6] = ielem;

    invec.read((char*)&velem, sizeof (velem));
    *(ptr_data+i) = velem;

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
  _size=vsiz;

  double velem=0;
  for( int i=0; i<_size; i++) {
    eigenvec.read((char*)&velem, sizeof (velem));
    // printf("v[%d] = %.16lf \n",i,velem);
    *(ptr_data+i) = velem;
  }

  eigenvec.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode AlVec::WriteEigenvalueVec(const std::string &filename, bool binary){

  std::ofstream outvec;
  int32_t  io_size=_size;

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
    outvec << "AlVec::DoF: " << std::setw(6) << _size << std::endl;
  }

  //jlove int32_t ielem=0;
  double  velem=0;
//  std::map<int,int>::iterator itcod;

  for( int i=0; i<_size; i++) {

    velem = *(ptr_data+i);

    if(binary)
      outvec.write((char*)&(velem), sizeof (velem));
    else
      outvec << std::setw(10) << i << std::setw(18) << velem << std::endl;
  }
  outvec.close();
  return StatusCode::SUCCESS;
}

} // end namespace Trk
