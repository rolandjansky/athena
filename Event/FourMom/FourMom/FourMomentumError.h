/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FourMomentumError_H
#define FourMomentumError_H

#include "EventKernel/I4MomentumError.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CxxUtils/CachedUniquePtr.h"

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

  virtual const ErrorMatrixPxPyPzE* pxPyPzEMatrix() const override;
  virtual const ErrorMatrixEEtaPhiM* eEtaPhiMMatrix() const override;
  virtual const ErrorMatrixPtEtaPhiM* ptEtaPhiMMatrix() const override;
  virtual const ErrorMatrixPtCotThPhiM* ptCotThPhiMMatrix() const override;
  virtual double pxError() const override;
  virtual double pyError() const override;
  virtual double pzError() const override;
  virtual double mError() const override;
  virtual double m2Error() const override;
  virtual double pError() const override;
  virtual double p2Error() const override;
  virtual double etaError() const override;
  virtual double rapidityError() const override;
  virtual double phiError() const override;
  virtual double eError() const override;
  virtual double etError() const override;   
  virtual double ptError() const override;
  virtual double iPtError() const override;
  virtual double cosPhiError() const override;
  virtual double sinPhiError() const override;
  virtual double cosThError() const override;
  virtual double sinThError() const override;
  virtual double cotThError() const override;
  virtual double tanThError() const override;

private: 

  const FourMom&                  m_p4;
  CxxUtils::CachedUniquePtr<ErrorMatrixPxPyPzE>     m_PxPyPzE;
  CxxUtils::CachedUniquePtr<ErrorMatrixEEtaPhiM>    m_EEtaPhiM;
  CxxUtils::CachedUniquePtr<ErrorMatrixPtEtaPhiM>   m_PtEtaPhiM;
  CxxUtils::CachedUniquePtr<ErrorMatrixPtCotThPhiM> m_PtCotThPhiM;

  ParamType m_originalType;

  ParamType originalType() const {return m_originalType;}
  bool computePxPyPzEMatrix() const;
  bool computeEEtaPhiMMatrix() const;
  bool computePtEtaPhiMMatrix() const;

  double sqr( double x) const {return x*x;}
};

#include "FourMom/FourMomentumError.icc"

#endif
