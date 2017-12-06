/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_CALDBSVC_H
#define ITRT_CALDBSVC_H
/** @file ITRT_CalDbSvc.h
 * @brief abstract interface to TRT calibration constants
 * @author Peter Hansen <phansen@nbi.dk>, Wouter hulsbergen
 */

#include <string>
#include <iostream>
//#include "AthenaKernel/IOVSvcDefs.h" // For IOVSVC_CALLBACK_ARGS macro definition
#include "GaudiKernel/IService.h"
#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "TRT_ConditionsData/RtRelationMultChanContainer.h"
#include "TRT_ConditionsData/StrawT0MultChanContainer.h"


class Identifier;
namespace TRTCond {
  class RtRelation;
}

/** @class ITRT_CalDbSvc
 *  abstract interface to TRT calibration constants
 */
class ITRT_CalDbSvc: virtual public IService
{
 public:
  typedef TRTCond::RtRelationMultChanContainer RtRelationContainer ;
  typedef TRTCond::StrawT0MultChanContainer StrawT0Container ;
  static const InterfaceID& interfaceID();  

  virtual float getT0( const Identifier& ,  int level = TRTCond::ExpandedIdentifier::STRAW ) const = 0;
  virtual const TRTCond::RtRelation* getRtRelation( const Identifier& , int level = TRTCond::ExpandedIdentifier::STRAW ) const  = 0;
  virtual const TRTCond::RtRelation* getErrors( const Identifier& , int level = TRTCond::ExpandedIdentifier::STRAW ) const  = 0;
  virtual const TRTCond::RtRelation* getSlopes( const Identifier& , int level = TRTCond::ExpandedIdentifier::STRAW ) const  = 0;
  virtual double driftRadius(const double&, float&, const Identifier&, bool&) const = 0;
  virtual double driftError(const double&, const Identifier&, bool&) const = 0;
  virtual double driftSlope(const double&, const Identifier&, bool&) const = 0;
  virtual TRTCond::ExpandedIdentifier trtcondid( const Identifier&, int level = TRTCond::ExpandedIdentifier::STRAW ) const =0;
  virtual void setT0( const TRTCond::ExpandedIdentifier&, float, float)= 0 ;
  virtual void setRtRelation( const TRTCond::ExpandedIdentifier&, const TRTCond::RtRelation*) =0 ;
  virtual void setRtErrors( const TRTCond::ExpandedIdentifier&, const TRTCond::RtRelation*) =0 ;
  virtual void setRtSlopes( const TRTCond::ExpandedIdentifier&, const TRTCond::RtRelation*) =0 ;
  virtual StatusCode writeTextFile_Format0(std::ostream&) const =0;
  virtual StatusCode writeTextFile_Format1(std::ostream&) const =0;
  virtual StatusCode writeTextFile_Format2(std::ostream&) const =0;
  virtual StatusCode writeTextFile_Format3(std::ostream&) const =0;
  virtual StatusCode readTextFile(const std::string&, int&) =0;
  virtual StatusCode readTextFile_Format0(std::istream&) =0 ;
  virtual StatusCode readTextFile_Format1(std::istream&) =0 ;
  virtual StatusCode readTextFile_Format2(std::istream&) =0 ;
  virtual StatusCode readTextFile_Format3(std::istream&) =0 ;
  virtual StatusCode streamOutCalibObjects () const =0;
  virtual StatusCode registerCalibObjects  (std::string , unsigned int, unsigned int, unsigned int, unsigned int) const =0;
  virtual RtRelationContainer* getRtContainer() const = 0;
  virtual RtRelationContainer* getErrContainer() const = 0;
  virtual RtRelationContainer* getSlopeContainer() const = 0;
  virtual StrawT0Container* getT0Container() const = 0 ;
  virtual void setRtContainer(RtRelationContainer*)= 0 ;
  virtual void setErrContainer(RtRelationContainer*)= 0 ;
  virtual void setSlopeContainer(RtRelationContainer*)= 0 ;
  virtual void setT0Container(StrawT0Container*) = 0 ;
  virtual void useCachedPtr(const bool&) = 0;
};
inline const InterfaceID& ITRT_CalDbSvc::interfaceID() {
  static const InterfaceID IID("ITRT_CalDbSvc",1,0);
  return IID;
}
#endif //  ITRT_CALDBSVC_H
