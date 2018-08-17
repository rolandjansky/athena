/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALGS_ALVEC_H
#define TRKALGS_ALVEC_H

// PBdR (17Apr2007)

#include <exception>
#include <map>
#include <vector>

class StatusCode;

namespace Trk {

class AlSymMatBase;
class AlSymMat;
class AlMat;
class AlSpaMat;

class AlVec {
 public:
  AlVec(int N);
  AlVec();
  AlVec(const AlVec& v);

  ~AlVec();

  inline double& operator[](int i);
  inline const double& operator[](int i) const;

  AlVec& operator=(const AlVec& v);
  AlVec& operator=(const double& v);
  AlVec  operator+(const AlVec&) const;
  AlVec& operator+=(const AlVec&);
  AlVec  operator-(const AlVec&) const;
  AlVec& operator-=(const AlVec&);
  double operator*(const AlVec&) const;
  AlVec  operator*(const AlMat&) const;
  AlVec  operator*(const AlSymMatBase&) const;
  AlVec  operator*(const double&) const;
  AlVec& operator*=(const double&);

  double norm() const;

  void reSize(int);
  void RemoveModule(int);
  void RemoveAlignPar(int, int);
  int  RemoveElements(std::vector<int>);
  void SetPathBin(const std::string&);
  void SetPathTxt(const std::string&);
  StatusCode Write(const std::string&, bool, double, std::map<int,unsigned long long>, float);
  StatusCode WritePartial(const std::string&, bool, double, std::map<int,unsigned long long>, float);
  StatusCode WritePartial(const std::string&, bool, double, std::map<int,std::string>, float);
  StatusCode InitializeOutputVector(const std::string&, bool, double, float, std::ofstream&);

  StatusCode CheckVecVersion(const std::string, bool&);
  StatusCode Read(const std::string&, double&, std::map<int,unsigned long long>&, float&);
  StatusCode ReadPartial(const std::string&, double&, std::map<int,unsigned long long>&, float&);
  StatusCode ReadProjected(const std::string&, double&, std::map<int,unsigned long long>&, float&);
  StatusCode ReadScalaPack(const std::string&);
  StatusCode WriteEigenvalueVec(const std::string &, bool);
  inline int size() const;
  inline double* ptrData() const;

 protected:
  int m_size;
  double* m_ptr_data;
  std::string m_pathbin;
  std::string m_pathtxt;

  void copy(const AlVec&);

  //friend class AlSymMatBase;
  //friend class AlSymMat;
  //friend class AlSpaMat;
  //friend class AlMat;
};

// inline methods:

inline int AlVec::size() const {
  return m_size;
}

inline double* AlVec::ptrData() const {
  return m_ptr_data;
}

inline double& AlVec::operator[](int i) {
  if( i < 0 ) {
    throw std::out_of_range( "AlVec: Index < zero! " );
  }

  if( i >= m_size ) {
    throw std::out_of_range( "AlVec: Index too large! ");
  }

  return *(m_ptr_data+i);
}

inline const double& AlVec::operator[](int i) const {
  if( i < 0 ) {
    throw std::out_of_range( "AlVec: Index < zero! " );
  }

  if( i >= m_size ) {
    throw std::out_of_range( "AlVec: Index too large! " );
  }

  return *(m_ptr_data+i);
}

} // end namespace Trk

#endif // TRKALGS_ALVEC_H
