/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_STRAWSTATUSSUMMARYSVC_H
#define ITRT_STRAWSTATUSSUMMARYSVC_H
/** @file ITRT_StrawStatusSummarySvc.h
 * @brief abstract interface to TRT straw status constants
 * @author Esben Klinkby <klinkby@nbi.dk>
 */

#include <string>
#include "GaudiKernel/IService.h"
#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include "TRT_ConditionsData/TRTStrawStatusData.h"
#include "TRT_ConditionsData/StrawStatusContainer.h"
#include "AthenaKernel/IOVSvcDefs.h" // For IOVSVC_CALLBACK_ARGS macro definition

class Identifier;
namespace TRTCOND {
  class ExpandedIdentifier;
}

/** @class ITRT_StrawStatusSummarySvc
 abstract interface to TRT straw status constants
*/
class ITRT_StrawStatusSummarySvc: virtual public IService
{
 public:

  typedef TRTCond::StrawStatusMultChanContainer StrawStatusContainer ;
  static const InterfaceID& interfaceID();  
  virtual int getStatus(Identifier ) =0;
  virtual int getStatusPermanent(Identifier) =0;
  virtual int getStatusHT(Identifier) =0;
  virtual void setStatus( const TRTCond::ExpandedIdentifier&, unsigned int) =0;
  virtual void  set_LT_occ(Identifier, double) =0;
  virtual double get_LT_occ(Identifier) =0;
  virtual void set_HT_occ(Identifier, double) =0;
  virtual double get_HT_occ(Identifier) =0;
  //virtual void set_status(Identifier, bool) =0;
  virtual void set_status_temp(Identifier, bool) =0;
  virtual void set_status_permanent(Identifier, bool) =0;
  virtual bool get_status(Identifier) =0;
  virtual bool get_statusHT(Identifier)=0;
  virtual void set_pid_status(Identifier , bool) =0;
  virtual bool get_pid_status(Identifier) =0;
  virtual void set_track_status(Identifier, bool) =0;
  virtual bool get_track_status(Identifier) =0;
  virtual StatusCode readFromTextFile(const std::string&) =0;
  virtual StatusCode writeToTextFile(const std::string& ) const =0;
  virtual StatusCode streamOutStrawStatObjects () const =0;
  virtual StatusCode registerObjects(std::string, int, int, int, int) const =0;
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS) =0;
  virtual StrawStatusContainer* getStrawStatusContainer() const =0;
  virtual StrawStatusContainer* getStrawStatusPermanentContainer() const =0;
  virtual StrawStatusContainer* getStrawStatusHTContainer() const =0;


  virtual StatusCode ComputeAliveStraws() =0;



   virtual int  *getStwTotal()		= 0;	 
   virtual int **getStwLocal() 		= 0;
   virtual int **getStwWheel()          = 0;
};



////////////////////////////////////////////////////////////////////////////////////////////
//  inline methods
////////////////////////////////////////////////////////////////////////////////////////////


inline const InterfaceID& ITRT_StrawStatusSummarySvc::interfaceID() {
  static const InterfaceID IID("ITRT_StrawStatusSummarySvc",1,0);
  return IID;
}

#endif //  ITRT_STRAWSTATUSSUMMARYSVC_H
