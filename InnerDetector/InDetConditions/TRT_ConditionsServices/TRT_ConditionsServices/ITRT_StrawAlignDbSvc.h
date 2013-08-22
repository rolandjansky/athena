/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_STRAWALIGNDBSVC_H
#define ITRT_STRAWALIGNDBSVC_H
/** @file ITRT_StrawAlignDbSvc.h
 * @brief abstract interface to TRT straw alignment constants
 * @author Peter Hansen <phansen@nbi.dk>
 */

#include <string>
#include "GaudiKernel/IService.h"
#include "AthenaKernel/IOVSvcDefs.h"

class Identifier;
namespace TRTCond {
  class ExpandedIdentifier;
  class RtRelation;
  class StrawDxContainer;
}
class  IAthenaOutputStreamTool ;
namespace InDetDD {
  class TRT_DetectorManager ;
}

/** @class ITRT_StrawAlignDbSvc
  abstract interface to TRT straw alignment constants
*/
class ITRT_StrawAlignDbSvc: virtual public IService
{
 public:
  typedef TRTCond::StrawDxContainer StrawDxContainer ;
  static const InterfaceID& interfaceID() ;
  virtual TRTCond::ExpandedIdentifier trtcondid( const Identifier&, int) const =0;
  virtual void setDx( const TRTCond::ExpandedIdentifier&, float, float, float ) =0;
  virtual StatusCode writeTextFile(const std::string&) const =0;
  virtual StatusCode readTextFile(const std::string&) =0;
  virtual StatusCode streamOutObjects () const =0;
  virtual StatusCode registerObjects (std::string, int, int, int, int) const=0;
  virtual StrawDxContainer* getDxContainer() const =0;
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS) =0;
  virtual float getDx1( const Identifier&) const =0;
  virtual float getDx2( const Identifier&) const =0;
  virtual float getDxErr( const Identifier& ) const =0 ;
  virtual void setDx( const Identifier&, float, float, float) =0;
  
};

////////////////////////////////////////////////////////////////////////////////////////////
//  inline methods
////////////////////////////////////////////////////////////////////////////////////////////

inline const InterfaceID& ITRT_StrawAlignDbSvc::interfaceID() {
  static const InterfaceID id("ITRT_StrawAlignDbSvc",1,0);
  return id;
}

#endif //  ITRT_STRAWALIGNDBTOOL_H
