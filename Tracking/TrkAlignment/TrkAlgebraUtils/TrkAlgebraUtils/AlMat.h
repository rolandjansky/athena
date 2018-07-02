/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALGS_ALMAT_H
#define TRKALGS_ALMAT_H

#include <iostream>
#include <string>
#include <exception>

class StatusCode;

namespace Trk {

class AlVec;
class AlSymMatBase;
class AlSymMat;
class AlSpaMat;

/**
   @class AlMat
   contains the implementation of the methods of class AlMat, for handling general NxM matrices
*/

////////////////////////////////////////////
class AlMat {
  ////////////////////////////////////////////
 public:
  AlMat(int N, int M);
  AlMat();
  AlMat(const AlMat& m);

  virtual ~AlMat();

  class AlMat_row {
  public:
    inline AlMat_row(AlMat&,int);
    double & operator[](int);
  private:
    AlMat& m_a;
    int m_r;
  };

  class AlMat_row_const {
  public:
    inline AlMat_row_const(const AlMat&,int);
    const double & operator[](int) const;
  private:
    const AlMat& m_a;
    int m_r;
  };
  // helper class to implement m[i][j]

  inline AlMat_row operator[] (int);
  inline AlMat_row_const operator[] (int) const;

  inline long int nrow() const;
  inline long int ncol() const;

  const std::string Print(const int NColsPerSet=10);

  AlMat&  operator=(const double&);
  AlMat&  operator=(const AlMat& m);
  AlMat&  operator=(const AlSymMat& m);
  AlMat   operator+(const AlMat& m) const;
  AlMat&  operator+=(const AlMat& m);
  AlMat   operator-(const AlMat& m) const;
  AlMat&  operator-=(const AlMat& m);
  AlMat   operator*(const AlMat& m) const;
  AlMat   operator*(const AlSymMatBase& m) const;
  AlVec   operator*(const AlVec& v) const;
  AlMat&  operator*=(const double& d);

  // transposition
  AlMat   T() const;      // creates a transposed copy. To be avoided!
  AlMat&  Transpose();    //   sets the transpose flag (ultrafast!)
  AlMat&  Normal();       // unsets the transpose flag (ultrafast!)

  //invert sym matrix declared as non-symetric for convenience
  void invertS(int& ierr, double Norm);

  // reSize method
  void reSize (int Nnew, int Mnew);

  StatusCode ReadScalaPack(const std::string&);

  void SetPathBin(const std::string&);
  void SetPathTxt(const std::string&);

  // write method
  // arguments are filename, flag for binary (true) or text file (false), text printout precision
  StatusCode Write(const std::string&, bool, unsigned int precision=6);

  virtual double& elemr(long int, long int);
  virtual double  elemc(long int, long int) const;

  inline double* ptrData() const;

  inline std::string pathBin() const;
  inline std::string pathTxt() const;

 private:
  void copy(const AlMat&  m);
  void copy(const AlSymMatBase&  m);

 protected:
  int m_ncol, m_nrow;
  int m_nele;
  std::string m_pathbin;
  std::string m_pathtxt;
  double* m_ptr_data;
  bool m_transpose;

  virtual long int elem(long int, long int) const;

};

// inline operators

inline AlMat::AlMat_row AlMat::operator[] (int r){
  AlMat_row b(*this,r);
  return b;
}

inline AlMat::AlMat_row_const AlMat::operator[] (int r) const{
  const AlMat_row_const b(*this,r);
  return b;
}

inline double &AlMat::AlMat_row::operator[](int c){
  if(m_r<0||m_r>=m_a.nrow() || c<0||c>=m_a.ncol())
    throw std::out_of_range( "Range error in AlMat::operator[][]" );

  return *(m_a.m_ptr_data+m_r*m_a.ncol()+c);
}

inline const double & AlMat::AlMat_row_const::operator[](int c) const {
  if(m_r<0||m_r>=m_a.nrow() || c<0||c>=m_a.ncol())
    throw std::out_of_range( "Range error in AlMat::operator[][]" );

  return *(m_a.m_ptr_data+m_r*m_a.ncol()+c);
}

inline AlMat::AlMat_row::AlMat_row(AlMat &a,int r)
	     : m_a(a), m_r(r)
{}

inline AlMat::AlMat_row_const::AlMat_row_const
(const AlMat&a,int r)
	     : m_a(a), m_r(r)
{}


inline long int AlMat::nrow() const {
  if( m_transpose ) return m_ncol;
  return m_nrow;
}

inline long int AlMat::ncol() const {
  if( m_transpose ) return m_nrow;
  return m_ncol;
}

inline double* AlMat::ptrData() const {
  return m_ptr_data;
}

inline std::string AlMat::pathBin() const {
  return m_pathbin;
}

inline std::string AlMat::pathTxt() const {
  return m_pathtxt;
}

} // end namespace Trk

#endif // TRKALGS_ALMAT_H
