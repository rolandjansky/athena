/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALGS_ALSYMMAT_H
#define TRKALGS_ALSYMMAT_H

#include "TrkAlgebraUtils/AlSymMatBase.h"

class StatusCode;
template<class Element> class TMatrixTSparse;
typedef class TMatrixTSparse<double> TMatrixDSparse;

namespace Trk {

class AlVec;
class AlMat;
class AlSpaMat;

/**
@class AlSymMat
contains the implementation for handling symmetric matrices in triangular representation
*/

////////////////////////////////////////////
class AlSymMat : public AlSymMatBase {
////////////////////////////////////////////

 public:
  AlSymMat(long int);
  AlSymMat();
  AlSymMat(const AlSymMat&);
  AlSymMat(const AlSpaMat&);
  AlSymMat& operator=(const AlSpaMat&);

  ~AlSymMat();

  // OPERATORS:
  virtual  AlSymMat&  operator=(const double&);
  virtual  AlSymMat&  operator=(const AlSymMat&);
  //virtual  AlSymMat&  operator=(const AlSpaMat&);
  virtual  AlSymMat&  operator=(const AlMat&);
  virtual  AlSymMat   operator+(const AlSymMat&) const;
  virtual  AlSymMat&  operator+=(const AlSymMat&);
  virtual  AlSymMat   operator-(const AlSymMat&) const;
  virtual  AlSymMat&  operator-=(const AlSymMat&);
  virtual  AlMat      operator*(const AlSymMat&) const;
  virtual  AlVec      operator*(const AlVec&) const;
  virtual  AlMat      operator*(const AlMat&) const;
  virtual  AlSymMat   operator*(const double&) const;
  virtual  AlSymMat&  operator*=(const double&);

  // ADVANCED:
  virtual  void reSize(long int Nnew) override final;
  virtual  double&    elemr(long int, long int) override final;
  virtual  double     elemc(long int, long int) const override final;

  virtual  int invert() override final;
  virtual  void RemoveModule(int) override final;
  virtual  void RemoveAlignPar(int, int) override final;
  virtual  int  RemoveCollsRows(std::vector<int>) override final;
  virtual  void SetPathBin(const std::string&) override final;
  virtual  void SetPathTxt(const std::string&) override final;
  virtual  StatusCode Write(const std::string&, bool, bool, double, float) override final;
  virtual  StatusCode CheckMatVersion(const std::string, bool&) override final;
  virtual  StatusCode Read(const std::string&, int&, bool&, float&) override final;
  virtual  StatusCode ReadProjected(const std::string&, int&, bool&, float&) override final;
  virtual  int diagonalize(char jobz, AlVec& w, AlMat& z) override final;
  virtual  double determinant() override final;

  inline double* ptrData() const;

  inline std::string pathBin() const;
  inline std::string pathTxt() const;

  virtual TMatrixDSparse* makeTMatrix() override final;


 protected:
  inline long int elem(long int, long int) const;

  void copy(const AlSymMat&  m);
  void copy(const AlSpaMat&  m);
  void copy(const AlMat&  m);

  double*     m_ptr_data = nullptr;

  std::string m_pathbin;
  std::string m_pathtxt;

};

// inline operators

inline long int AlSymMat::elem(long int i,long int j) const {
#ifdef _DEBUG
  if( i<0 )     { throw std::out_of_range( "AlSymMat::elem: Index 1 < zero! " ); };
  if( i>=size() ) { throw std::out_of_range( "AlSymMat::elem: Index 1 too large! " ); };
  if( j<0 )     { throw std::out_of_range( "AlSymMat::elem: Index 2 < zero! " );  };
  if( j>=size() ) { throw std::out_of_range( "AlSymMat::elem: Index 2 too large! " ); };
#endif
  if( j<=i ) {
    return  ((i+1)*i/2+j);
  }
  else {
    return  ((j+1)*j/2+i);
  };
}

inline double* AlSymMat::ptrData() const {
  return m_ptr_data;
}

inline std::string AlSymMat::pathBin() const {
  return m_pathbin;
}

inline std::string AlSymMat::pathTxt() const {
  return m_pathtxt;
}

} // end namespace Trk

#endif // TRKALGS_ALSYMMAT_H
