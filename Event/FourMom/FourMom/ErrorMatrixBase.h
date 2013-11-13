/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ErrorMatrixBase_H
#define ErrorMatrixBase_H

#include "CLHEP/Matrix/SymMatrix.h"

template <class Type>
class ErrorMatrixBase {
public:

  ErrorMatrixBase() : m_matrix() {}
  ErrorMatrixBase(int dim) : m_matrix(dim) {}
  ErrorMatrixBase(const CLHEP::HepSymMatrix& mat) : m_matrix(mat) {}

  inline int num_row() const {return m_matrix.num_row();}
  inline int num_col() const {return m_matrix.num_col();}

  /// The element access operator M(i,j) starts counting indeces from zero
  const double & operator()(int row, int col) const; 
  double & operator()(int row, int col);

  /// The single argument versions return diagonal elements
  const double & operator()(int diagElement) const; 
  double & operator()(int diagElement);

  const CLHEP::HepSymMatrix& hsm() const {return m_matrix;}

   class ErrorMatrixBase_row {
   public:
      inline ErrorMatrixBase_row(ErrorMatrixBase&,int);
      inline double & operator[](int);
   private:
      ErrorMatrixBase& m_a;
      int m_r;
   };
   class ErrorMatrixBase_row_const {
   public:
      inline ErrorMatrixBase_row_const(const ErrorMatrixBase&,int);
      inline const double & operator[](int) const;
   private:
      const ErrorMatrixBase& m_a;
      int m_r;
   };
   // helper class to implement m[i][j]

   inline ErrorMatrixBase_row operator[] (int);
   inline ErrorMatrixBase_row_const operator[] (int) const;

private:

  CLHEP::HepSymMatrix m_matrix;

protected:

  const CLHEP::HepSymMatrix& impl() const {return m_matrix;}
  CLHEP::HepSymMatrix& impl() {return m_matrix;}

};

template <class Type>
inline const double & ErrorMatrixBase<Type>::operator()(int row, int col) const 
{
  return m_matrix(row+1,col+1);
}

template <class Type>
inline const double & ErrorMatrixBase<Type>::operator()(int diag) const 
{
  return m_matrix(diag+1,diag+1);
}

template <class Type>
inline double & ErrorMatrixBase<Type>::operator()(int row, int col) 
{
  return m_matrix(row+1,col+1);
}

template <class Type>
inline double & ErrorMatrixBase<Type>::operator()(int diag) 
{
  return m_matrix(diag+1,diag+1);
}

template <class Type>
inline typename ErrorMatrixBase<Type>::ErrorMatrixBase_row ErrorMatrixBase<Type>::operator[] (int r)
{
  return ErrorMatrixBase_row(*this,r);
}

template <class Type>
inline typename ErrorMatrixBase<Type>::ErrorMatrixBase_row_const ErrorMatrixBase<Type>::operator[] (int r) const
{
  return ErrorMatrixBase_row_const(*this,r);
}

template <class Type>
inline double &ErrorMatrixBase<Type>::ErrorMatrixBase_row::operator[](int c)
{
  return m_a.impl()(m_r,c);
}

template <class Type>
inline const double &
ErrorMatrixBase<Type>::ErrorMatrixBase_row_const::operator[](int c) const 
{
  return m_a.impl()(m_r,c);
}

template <class Type>
inline ErrorMatrixBase<Type>::ErrorMatrixBase_row::ErrorMatrixBase_row(ErrorMatrixBase &a,
									  int r) 
   : m_a(a), m_r(r)
{}

template <class Type>
inline ErrorMatrixBase<Type>::ErrorMatrixBase_row_const::ErrorMatrixBase_row_const
(const ErrorMatrixBase&a,int r) 
   : m_a(a), m_r(r)
{}


#endif
