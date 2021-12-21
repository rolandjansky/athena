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
    AlSpaMat&  operator=(const double&) ;
    AlSpaMat&  operator=(const AlSpaMat&);
    AlSpaMat&  operator=(const AlSymMat&);
    AlSpaMat&  operator=(const AlMat&);
    AlSpaMat   operator+(const AlSpaMat&) const;
    AlSpaMat&  operator+=(const AlSpaMat&);
    AlSpaMat   operator-(const AlSpaMat&) const;
    AlSpaMat&  operator-=(const AlSpaMat&);
    AlMat      operator*(const AlSymMatBase&) const;
    AlVec      operator*(const AlVec&) const;
    AlMat      operator*(const AlMat&) const;
    AlSpaMat   operator*(const double&) const;
    AlSpaMat&  operator*=(const double&);

  // ADVANCED:
    int SolveWithEigen(AlVec& RHS);
    virtual  void RemoveModule( int) override final;
    virtual  void RemoveAlignPar(int, int) override final;
    void RemoveDoF( int, int nelem=1);
    virtual  int  RemoveCollsRows(std::vector<int>) override final;
    virtual  void reSize(long int) override final;
    virtual  void SetPathBin(const std::string&) override final;
    virtual  void SetPathTxt(const std::string&) override final;
    virtual  StatusCode Write(const std::string&, bool, bool, double, float) override final;
    virtual  StatusCode CheckMatVersion(const std::string, bool&) override final;
    virtual  StatusCode Read(const std::string&, int&, bool&, float&) override final;
    virtual  StatusCode ReadProjected(const std::string&, int&, bool&, float&) override final;
    virtual  int invert() override final;
    virtual  int diagonalize(char jobz, AlVec& w, AlMat& z) override final;
    virtual  double& elemr(long int, long int) override final;
    virtual  double  elemc(long int, long int) const override final;
    inline   long int nele();
    virtual  double determinant() override final;

    inline void elem(const indices&, long int&, long int&) const;
    inline std::string pathBin() const;
    inline std::string pathTxt() const;

    virtual TMatrixDSparse* makeTMatrix() override final;
  protected:

    indices elem(long int, long int) const;

    void copy(const AlSpaMat&  m);
    void copy(const AlSymMat&  m);
    void copy(const AlMat&  m);

    std::string m_pathbin;
    std::string m_pathtxt;

    int* m_ptr_row = nullptr;
    int* m_ptr_col = nullptr;
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
