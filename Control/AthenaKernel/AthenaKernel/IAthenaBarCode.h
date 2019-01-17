///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENABARCODE_H
#define ATHENAKERNEL_IATHENABARCODE_H
///////////////////////////////////////////////////////////////////////////////
///
///              IAthenaBarCode interface class
///
/// \author  Yushu Yao
///
/// \brief barcode for all INav4Mom classes
///
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <GaudiKernel/StatusCode.h>
#include <stdint.h>
//#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
// Universal Object Identifier for all Navigable4Momentum Objects
//
// Class Structure:
// IAthenaBarCode   INavigable  I4Momentum
//        |           /           /
//        V        /           /
//      INav4M
//        |
//        V
//    IParticle
//        |
//        V
//    ParticleImpl
//
///////////////////////////////////////////////////////////////////////////////

//Work Around dependency issue of Navigation
//If don't see IUserDataSvc.h yet, do the definitions here.
#include <stdint.h> // for uint64_t
typedef uint64_t AthenaBarCode_t;

typedef AthenaBarCode_t AthenaBarCodeVersion_t;

class IAthenaBarCode {

public:

  static const AthenaBarCode_t UNDEFINEDBARCODE = (AthenaBarCode_t)(-1);
  static const AthenaBarCode_t UNDEFINEDVERSION = UNDEFINEDBARCODE;

public:

  //Constructors

  IAthenaBarCode() {  }
  virtual ~IAthenaBarCode() = default;

  //All the default implementations are for the classes that derives from
  //INav4M but not ParticleImpl

public:
  //Public setter and getters
  virtual AthenaBarCode_t getAthenaBarCode() const {
    return UNDEFINEDBARCODE;
  }

  virtual void setAthenaBarCode(AthenaBarCode_t /*id*/) {
    //    throw std::runtime_error("IAthenaBarCode::setAthenaBarCode():: can't set AthenaBarCode in the dummy implementation");
  }

public:
  //Comparing & Versioning Control
  virtual bool hasSameAthenaBarCode(const IAthenaBarCode &/*obj*/) const {
    return false;
  }

  virtual bool hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &/*obj*/) const {
    return false;
  }

  virtual AthenaBarCodeVersion_t getVersion() const {
    return UNDEFINEDVERSION;
  }

  virtual void newVersion() {
    //    throw std::runtime_error("IAthenaBarCode::newVersion():: can't newVersion() in the dummy implementation");
  }

  virtual void setVersion(AthenaBarCodeVersion_t /*newversion*/) {
    //    throw std::runtime_error("IAthenaBarCode::setVersion():: can't setVersion() in the dummy implementation");
  }

};

#endif //  NAVIGATION_ATHENABARCODE_H
