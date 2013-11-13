/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FourMomentumError_H
#define FourMomentumError_H

#include "EventKernel/I4MomentumError.h"
#include "FourMom/DeepCopyPointer.h"

#include "CLHEP/Matrix/SymMatrix.h"

template < class FourMom>
class FourMomentumError : public I4MomentumError {
private: 

  enum ParamType {PxPyPzE, EEtaPhiM, PtEtaPhiM, PtCotThPhiM};

public:

  FourMomentumError(const ErrorMatrixPxPyPzE&, const FourMom&);
  FourMomentumError(const ErrorMatrixEEtaPhiM&, const FourMom&);
  FourMomentumError(const ErrorMatrixPtEtaPhiM&, const FourMom&);
  FourMomentumError(const ErrorMatrixPtCotThPhiM&, const FourMom&);

  FourMomentumError( const CLHEP::HepSymMatrix&, ParamType, const FourMom&);

  // copy constructor may not copy cached matrices
  FourMomentumError( const FourMomentumError& other);
  FourMomentumError &operator=( const FourMomentumError& other);

  virtual const ErrorMatrixPxPyPzE* pxPyPzEMatrix() const;
  virtual const ErrorMatrixEEtaPhiM* eEtaPhiMMatrix() const;
  virtual const ErrorMatrixPtEtaPhiM* ptEtaPhiMMatrix() const;
  virtual const ErrorMatrixPtCotThPhiM* ptCotThPhiMMatrix() const;
  virtual double pxError() const;
  virtual double pyError() const;
  virtual double pzError() const;
  virtual double mError() const;
  virtual double m2Error() const;
  virtual double pError() const;
  virtual double p2Error() const;
  virtual double etaError() const;
  virtual double rapidityError() const;
  virtual double phiError() const;
  virtual double eError() const;
  virtual double etError() const;   
  virtual double ptError() const;
  virtual double iPtError() const;
  virtual double cosPhiError() const;
  virtual double sinPhiError() const;
  virtual double cosThError() const;
  virtual double sinThError() const;
  virtual double cotThError() const;
  virtual double tanThError() const;

private: 

  const FourMom&                  m_p4;
  mutable DeepCopyPointer<ErrorMatrixPxPyPzE>     m_PxPyPzE;
  mutable DeepCopyPointer<ErrorMatrixEEtaPhiM>    m_EEtaPhiM;
  mutable DeepCopyPointer<ErrorMatrixPtEtaPhiM>   m_PtEtaPhiM;
  mutable DeepCopyPointer<ErrorMatrixPtCotThPhiM> m_PtCotThPhiM;

  ParamType m_originalType;

  ParamType originalType() const {return m_originalType;}
  bool computePxPyPzEMatrix() const;
  bool computeEEtaPhiMMatrix() const;
  bool computePtEtaPhiMMatrix() const;

  double sqr( double x) const {return x*x;}
};

#include "FourMom/FourMomentumError.icc"

#endif
