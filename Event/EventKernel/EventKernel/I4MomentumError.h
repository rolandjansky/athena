// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTKERNEL_I4MOMENTUMERROR_H
#define EVENTKERNEL_I4MOMENTUMERROR_H 1

class ErrorMatrixPxPyPzE;
class ErrorMatrixEEtaPhiM;
class ErrorMatrixPtEtaPhiM;
class ErrorMatrixPtCotThPhiM;

/** 
   @class I4Momentum

   I4MomentumError is an abstract base class providing errors (uncertainties) 
   for 4-momentum classes.
   Concrete base classes and some implementation are provided in package 
   Event/FourMom

    
   @author Teddy Todorov teddy.todorov@cern.ch
 */


class I4MomentumError {
public:

  virtual ~I4MomentumError();

  /** Access to PxPyPzE error matrix, can return 0 if errors not defined */
  virtual const ErrorMatrixPxPyPzE* pxPyPzEMatrix() const = 0;
  /** Access to EEtaPhiM error matrix, can return 0 if errors not defined */
  virtual const ErrorMatrixEEtaPhiM* eEtaPhiMMatrix() const = 0;
  /** Access to PtEtaPhiM error matrix, can return 0 if errors not defined */
  virtual const ErrorMatrixPtEtaPhiM* ptEtaPhiMMatrix() const = 0;
  /** Access to PtCotThPhiM error matrix, can return 0 if errors not defined */
  virtual const ErrorMatrixPtCotThPhiM* ptCotThPhiMMatrix() const = 0;

  /** The following methods correspond 1-to-1 to the I4Momentum methods,
      and provide the errors on the corresponding quantities.
      If errors are not defined, the returned values are zero.
   */
  virtual double pxError() const = 0;
  virtual double pyError() const = 0;
  virtual double pzError() const = 0;
  virtual double mError() const = 0;
  virtual double m2Error() const = 0;
  virtual double pError() const = 0;
  virtual double p2Error() const = 0;
  virtual double etaError() const = 0;
  virtual double rapidityError() const = 0;
  virtual double phiError() const = 0;
  virtual double eError() const = 0;
  virtual double etError() const = 0;   
  virtual double ptError() const = 0;
  virtual double iPtError() const = 0;
  virtual double cosPhiError() const = 0;
  virtual double sinPhiError() const = 0;
  virtual double cosThError() const = 0;
  virtual double sinThError() const = 0;
  virtual double cotThError() const = 0;
  virtual double tanThError() const = 0;

};

#endif
