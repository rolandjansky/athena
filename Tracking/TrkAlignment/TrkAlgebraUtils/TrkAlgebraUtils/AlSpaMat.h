/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALGS_ALSPAMAT_H
#define TRKALGS_ALSPAMAT_H

#include "TrkAlgebraUtils/AlSymMatBase.h"

#include <exception>

class StatusCode;
template<class Element> class TMatrixTSparse;
typedef class TMatrixTSparse<double> TMatrixDSparse;

namespace Trk {

class AlVec;
class AlSymMat;
class AlSymMatBase;
/**
@class AlSpaMat
contains the implementation for handling sparse matrices
*/

////////////////////////////////////////////
class AlSpaMat : public AlSymMatBase {
////////////////////////////////////////////

  public:
    AlSpaMat();
    AlSpaMat(long int N);
    AlSpaMat(const AlSpaMat& m);
    AlSpaMat(const AlSymMat& m);
    ~AlSpaMat();

    // OPERATORS:
    virtual  AlSpaMat&  operator=(const double&);
    virtual  AlSpaMat&  operator=(const AlSpaMat&);
    virtual  AlSpaMat&  operator=(const AlSymMat&);
    virtual  AlSpaMat&  operator=(const AlMat&);
    virtual  AlSpaMat   operator+(const AlSpaMat&) const;
    virtual  AlSpaMat&  operator+=(const AlSpaMat&);
    virtual  AlSpaMat   operator-(const AlSpaMat&) const;
    virtual  AlSpaMat&  operator-=(const AlSpaMat&);
    virtual  AlMat      operator*(const AlSymMatBase&) const;
    virtual  AlVec      operator*(const AlVec&) const;
    virtual  AlMat      operator*(const AlMat&) const;
    virtual  AlSpaMat   operator*(const double&) const;
    virtual  AlSpaMat&  operator*=(const double&);

  // ADVANCED:
    int SolveWithEigen(AlVec& RHS);
    virtual  void RemoveModule( int);
    virtual  void RemoveAlignPar(int, int);
    virtual  void RemoveDoF( int, int nelem=1);
    virtual  int  RemoveCollsRows(std::vector<int>);
    virtual  void reSize(long int);
    virtual  void SetPathBin(const std::string&);
    virtual  void SetPathTxt(const std::string&);
    virtual  StatusCode Write(const std::string&, bool, bool, double, float);
    virtual  StatusCode CheckMatVersion(const std::string, bool&);
    virtual  StatusCode Read(const std::string&, int&, bool&, float&);
    virtual  StatusCode ReadProjected(const std::string&, int&, bool&, float&);
    virtual  int invert();
    virtual  int diagonalize(char jobz, AlVec& w, AlMat& z);
    virtual  double& elemr(long int, long int);
    virtual  double  elemc(long int, long int) const;
    inline   long int nele();
    virtual  double determinant();

    inline void elem(const indices&, long int&, long int&) const;
    inline std::string pathBin() const;
    inline std::string pathTxt() const;

    virtual TMatrixDSparse* makeTMatrix();
  protected:

    virtual  indices elem(long int, long int) const;

    virtual void copy(const AlSpaMat&  m);
    virtual void copy(const AlSymMat&  m);
    virtual void copy(const AlMat&  m);

    std::string m_pathbin;
    std::string m_pathtxt;

    int* m_ptr_row;
    int* m_ptr_col;

};

// inline methods:

inline void AlSpaMat::elem(const indices& key, long int& i, long int& j) const {
  i = key.first;
  j = key.second;
  if(j>i) throw std::out_of_range( "AlSpaMat::elem: i<j ! " );
  return;
}

inline long int AlSpaMat::nele() {
  // first check for intrinsic consistency:
  if( int(m_ptr_map.size()) != m_nele ) throw std::range_error( "AlSpaMat::m_nele has been corrupted!" );
  m_nele = m_ptr_map.size();
  return m_nele;
}

inline std::string AlSpaMat::pathBin() const {
  return m_pathbin;
}

inline std::string AlSpaMat::pathTxt() const {
  return m_pathtxt;
}



} // end namespace Trk

#endif // TRKALGS_ALSPAMAT_H
