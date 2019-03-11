/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_CALDBTOOL_H
#define ITRT_CALDBTOOL_H
/** @file ITRT_CalDbTool.h
 * @brief abstract interface to TRT calibration constants
 * @author Peter Hansen <phansen@nbi.dk>, Wouter hulsbergen
 */

#include <string>
#include <iostream>
#include "GaudiKernel/IAlgTool.h"
#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "TRT_ConditionsData/RtRelationMultChanContainer.h"
#include "TRT_ConditionsData/StrawT0MultChanContainer.h"


class Identifier;
namespace TRTCond {
  class RtRelation;
}

/** @class ITRT_CalDbTool
 *  abstract interface to TRT calibration constants
 */
class ITRT_CalDbTool: virtual public IAlgTool
{
 public:
  typedef TRTCond::RtRelationMultChanContainer RtRelationContainer ;
  typedef TRTCond::StrawT0MultChanContainer StrawT0Container ;

  DeclareInterfaceID(ITRT_CalDbTool, 1, 0);

  virtual float getT0( const Identifier& ,  int level = TRTCond::ExpandedIdentifier::STRAW ) const = 0;
  virtual const TRTCond::RtRelation* getRtRelation( const Identifier& , int level = TRTCond::ExpandedIdentifier::STRAW ) const  = 0;
  virtual const TRTCond::RtRelation* getErrors( const Identifier& , int level = TRTCond::ExpandedIdentifier::STRAW ) const  = 0;
  virtual const TRTCond::RtRelation* getSlopes( const Identifier& , int level = TRTCond::ExpandedIdentifier::STRAW ) const  = 0;
  virtual double driftRadius(const double&, float&, const Identifier&, bool&) const = 0;
  virtual double driftError(const double&, const Identifier&, bool&) const = 0;
  virtual double driftSlope(const double&, const Identifier&, bool&) const = 0;
  virtual TRTCond::ExpandedIdentifier trtcondid( const Identifier&, int level = TRTCond::ExpandedIdentifier::STRAW ) const =0;
  virtual const RtRelationContainer* getRtContainer() const = 0;
  virtual const RtRelationContainer* getErrContainer() const = 0;
  virtual const RtRelationContainer* getSlopeContainer() const = 0;
  virtual const StrawT0Container* getT0Container() const = 0 ;

};

#endif //  ITRT_CALDBTOOL_H
