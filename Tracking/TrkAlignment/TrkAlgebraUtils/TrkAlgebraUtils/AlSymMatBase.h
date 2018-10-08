/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALGS_ALSYMMATBASE_H
#define TRKALGS_ALSYMMATBASE_H

// AlSymMatBase.h - Base implementation of generic matrices for alignment
// modified from SiGlobalChi2AlgebrUtils::AlSymMatBase, modified to make
// pure virtual

#include <exception>
#include <map>
#include <vector>

class StatusCode;
template<class Element> class TMatrixTSparse;
typedef class TMatrixTSparse<double> TMatrixDSparse;

namespace Trk {

class AlVec;
class AlMat;
class AlSpaMat;

// definition of custom types used:
typedef std::pair<long int, long int> indices;
typedef std::map<indices, double> datamap;
typedef datamap::iterator mapiterator;
typedef datamap::const_iterator const_mapiterator;

/**
@class AlSymMatBase
contains the base implementation for handling symmertic matrices
*/

////////////////////////////////////////////
class AlSymMatBase  {
////////////////////////////////////////////

 public:
  virtual ~AlSymMatBase();

  class AlSymMatBase_row {
  public:
    inline AlSymMatBase_row(AlSymMatBase&,long int);
    double & operator[](long int);
  private:
    AlSymMatBase& m_a;
    long int  m_r;
  };

  class AlSymMatBase_row_const {
  public:
    inline AlSymMatBase_row_const(const AlSymMatBase&,long int);
    double operator[](long int) const;
  private:
    const AlSymMatBase& m_a;
    long int  m_r;
  };
  // helper class to implement m[i][j]

  inline AlSymMatBase_row operator[] (long int);
  inline AlSymMatBase_row_const operator[] (long int) const;

  inline long int nrow() const;
  inline long int ncol() const;
  inline long int size() const;

  inline int matrix_type() const;

  // pure virtual:
  virtual  void       reSize(long int Nnew)=0;
  virtual  double&    elemr(long int, long int)=0;
  virtual  double     elemc(long int, long int) const=0;
  virtual  int        invert() = 0;
  virtual  void       RemoveModule(int) = 0;
  virtual  void       RemoveAlignPar(int, int) = 0;
  virtual  int        RemoveCollsRows(std::vector<int>) = 0;
  virtual  void       SetPathBin(const std::string&) = 0;
  virtual  void       SetPathTxt(const std::string&) = 0;
  virtual  StatusCode Write(const std::string&, bool, bool, double, float) = 0;
  virtual  StatusCode CheckMatVersion(const std::string, bool&) = 0;
  virtual  StatusCode Read(const std::string&, int&, bool&, float&) = 0;
  virtual  StatusCode ReadProjected(const std::string&, int&, bool&, float&) = 0;
  virtual  int diagonalize(char jobz, AlVec& w, AlMat& z) = 0;
  virtual  double determinant() = 0;

  inline const datamap * ptrMap() const;

	virtual TMatrixDSparse* makeTMatrix() =0;


 protected:
  AlSymMatBase(); // ensure only create derived classes
  AlSymMatBase(long int N);
  AlSymMatBase(const AlSymMatBase&);
  AlSymMatBase& operator=(const AlSymMatBase&);

  int      m_matrix_type;
  datamap  m_ptr_map;

  long int m_size;
  long int m_nele;

};

// inline operators

inline AlSymMatBase::AlSymMatBase_row AlSymMatBase::operator[] (long int r) {
  AlSymMatBase_row b(*this,r);
  return b;
}

inline AlSymMatBase::AlSymMatBase_row_const AlSymMatBase::operator[] (long int r) const {
  const AlSymMatBase_row_const b(*this,r);
  return b;
}

inline double & AlSymMatBase::AlSymMatBase_row::operator[](long int c) {
  if(m_r<0||m_r>=m_a.nrow() || c<0||c>=m_a.ncol()) {
    throw std::out_of_range( "Range error in AlSymMatBase::operator[][]" );
  } else {
    return m_a.elemr(m_r,c);
  }
}

inline double AlSymMatBase::AlSymMatBase_row_const::operator[](long int c) const {
  if(m_r<0||m_r>=m_a.nrow() || c<0||c>=m_a.ncol()) {
    throw std::out_of_range( "Range error in AlSymMatBase::operator[][]" );
  } else {
    return m_a.elemc(m_r,c);
  }
}

inline AlSymMatBase::AlSymMatBase_row::AlSymMatBase_row(AlSymMatBase &a,long int r)
  : m_a(a), m_r(r)
{}

inline AlSymMatBase::AlSymMatBase_row_const::AlSymMatBase_row_const
(const AlSymMatBase&a,long int r)
  : m_a(a), m_r(r)
{}


inline long int AlSymMatBase::size() const { return m_size; }
inline long int AlSymMatBase::nrow() const { return m_size; }
inline long int AlSymMatBase::ncol() const { return m_size; }
inline int AlSymMatBase::matrix_type() const  { return m_matrix_type; }
inline const datamap * AlSymMatBase::ptrMap() const { return &m_ptr_map; }

} // end namespace Trk

#endif // TRKALGS_ALSYMMATBASE_H
