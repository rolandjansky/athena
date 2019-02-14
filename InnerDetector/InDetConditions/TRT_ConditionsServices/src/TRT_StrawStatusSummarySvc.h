/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_STRAWSTATUSSUMMARYSVC_H
#define TRT_STRAWSTATUSSUMMARYSVC_H
/** @file TRT_StrawStatusSummarySvc.h
 * @brief interface to TRT straw status constants
 * @author Esben Klinkby <klinkby@nbi.dk>
 */

#include <vector>
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/DataHandle.h"
//#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h" 
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"


class TRT_ID;
namespace InDetDD{ class TRT_DetectorManager; }

class IToolSvc;
/** @class TRT_StrawStatusSummarySvc
 interface to TRT straw status constants
*/
class TRT_StrawStatusSummarySvc: public AthService,
  virtual public ITRT_StrawStatusSummarySvc,
  virtual public ITRT_ConditionsSvc
{
 public:
  
  /// constructor
  TRT_StrawStatusSummarySvc( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// destructor
  virtual ~TRT_StrawStatusSummarySvc();
  
  /// tool initialize
  virtual StatusCode initialize();

  /// tool finalize
  virtual StatusCode finalize();

  /// Evaluation for TRT_ConditionsSummarySvc
  InDet::TRT_CondFlag condSummaryStatus( const Identifier& id ) {
    if (!(get_status(id)==1)) return InDet::TRT_COND_GOOD; 
    else return InDet::TRT_COND_BAD;
  };

  InDet::TRT_CondFlag condSummaryStatusHT( const Identifier& id ) {
    if (!(get_statusHT(id)==1)) return InDet::TRT_COND_GOOD; 
    else return InDet::TRT_COND_BAD;
  };


  /*
    // EK: Consider to put something like this
    if (get_LT_occ(Identifier)<0.1 ) return InDet::TRT_COND_GOOD;
    else if (get_LT_occ(Identifier)>=0.1 ) return InDet::TRT_COND_BAD;
    };
  */

  /// access to interfaceID
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) ;


 /// access to the status
  int getStatus(Identifier offlineId);// const;
  int getStatusPermanent(Identifier offlineId);// const;
  int getStatusHT(Identifier offlineId);// const;

  //  void setStatus( const TRTCond::ExpandedIdentifier& id, char status ) ;
  void setStatus( const TRTCond::ExpandedIdentifier& id, unsigned int status ) ;

  void  set_LT_occ(Identifier offlineID, double lt_occ) ;
  double get_LT_occ(Identifier offlineID);

  void set_HT_occ(Identifier offlineID, double ht_occ);
  double get_HT_occ(Identifier offlineID);

  //special bits
  //void set_status(Identifier offlineID, bool set);
  void set_status_temp(Identifier offlineID, bool set);
  void set_status_permanent(Identifier offlineID, bool set);
  bool get_status(Identifier offlineID);
  bool get_statusHT(Identifier offlineID);
  void set_pid_status(Identifier offlineID, bool set);
  bool get_pid_status(Identifier offlineID);
  void set_track_status(Identifier offlineID, bool set);
  bool get_track_status(Identifier offlineID);

  StatusCode readFromTextFile(const std::string& filename) ;
  StatusCode writeToTextFile(const std::string& filename) ;

  StatusCode streamOutStrawStatObjects () const ;
  StatusCode registerObjects(std::string tag, int run1, int event1, int run2, int event2) const;
  

  const StrawStatusContainer* getStrawStatusContainer() ;// const ;
  const StrawStatusContainer* getStrawStatusPermanentContainer() ; //const ;
  const StrawStatusContainer* getStrawStatusHTContainer() ; //const ;


  void ComputeAliveStraws() ;
  int  *getStwTotal()		;	 
  int **getStwLocal() 		;
  int **getStwWheel()   	;
  int **getStwLocalStraw()  	; 



 private:
  ServiceHandle<StoreGateSvc> m_detStore;
  std::vector<double> m_ltvaluevector;
  std::vector<double> m_htvaluevector;
  ServiceHandle<IToolSvc> m_toolsvc; 
  std::string m_par_strawstatuscontainerkey;
  std::string m_par_strawstatuspermanentcontainerkey;
  std::string m_par_strawstatusHTcontainerkey;

  std::string m_par_stattextfile;           //input text file
  std::string m_par_stattextfilepermanent;  //input text file: permanent
  std::string m_par_stattextfileHT;         //input text file HT

  std::string m_par_statstream;            //output stream
  const TRT_ID* m_trtid;
  const InDetDD::TRT_DetectorManager* m_trtDetMgr; // TRT detector manager (to get ID helper)
  

  ServiceHandle<ICondSvc> m_condSvc;
  //  ReadHandle  keys
  SG::ReadCondHandleKey<StrawStatusContainer> m_statReadKey{this,"StatReadKeyName","in","StrawStatus in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_permReadKey{this,"PermReadKeyName","in","StrawStatusPermanent in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_statHTReadKey{this,"StatHTReadKeyName","in","StrawStatusHT in-key"};

  // Caches
  mutable std::vector<const StrawStatusContainer*> m_strawstatuscontainer;
  mutable std::vector<const StrawStatusContainer*> m_strawstatuspermanentcontainer;
  mutable std::vector<const StrawStatusContainer*> m_strawstatusHTcontainer;
  mutable std::mutex m_cacheMutex;
  mutable std::vector<EventContext::ContextEvt_t> m_evtstat;
  mutable std::vector<EventContext::ContextEvt_t> m_evtperm;
  mutable std::vector<EventContext::ContextEvt_t> m_evtstatHT;
  // Used in simulation (GEANT4) jobs
  bool m_isGEANT4;
  const DataHandle<StrawStatusContainer> m_strawstatusG4;
  const DataHandle<StrawStatusContainer> m_strawstatuspermanentG4;
  const DataHandle<StrawStatusContainer> m_strawstatusHTG4;

  bool m_setup;



      int  *m_stw_total;
      int **m_stw_local;
      int **m_stw_wheel;

  void resetArrays();

	// COPIED FROM TRT LOCAL OCCUPANCY TOOL NOT OPTIMAL!!!
  int findArrayTotalIndex(const int det, const int lay);
  int findArrayLocalWheelIndex(const int det, const int lay);
  int findArrayLocalStrawIndex(const int det, const int lay, const int strawlay);

};



////////////////////////////////////////////////////////////////////////////////////////////
//  inline methods
////////////////////////////////////////////////////////////////////////////////////////////



inline void TRT_StrawStatusSummarySvc::setStatus( const TRTCond::ExpandedIdentifier& id, unsigned int status ) 
{
  const_cast<StrawStatusContainer*>(getStrawStatusContainer())->set( id, status) ;
}


inline const TRT_StrawStatusSummarySvc::StrawStatusContainer* TRT_StrawStatusSummarySvc::getStrawStatusContainer() {
  if(m_isGEANT4) {
    return m_strawstatusG4.cptr();
  }
  const EventContext& event_context=Gaudi::Hive::currentContext();
  EventContext::ContextID_t slot=event_context.slot();
  EventContext::ContextEvt_t event_id=event_context.evt();
  std::lock_guard<std::mutex> lock(m_cacheMutex);
  if(slot>=m_evtstat.size()) {
     m_evtstat.resize(slot+1);
     m_strawstatuscontainer.resize(slot+1);
  }

  if(m_evtstat[slot]!=event_id) {
    SG::ReadCondHandle<StrawStatusContainer> rst(m_statReadKey,event_context);
    m_strawstatuscontainer[slot]=(*rst);
    m_evtstat[slot]=event_id;
  }

  return m_strawstatuscontainer[slot];

}
inline const TRT_StrawStatusSummarySvc::StrawStatusContainer* TRT_StrawStatusSummarySvc::getStrawStatusPermanentContainer() {

  if(m_isGEANT4) {
    return m_strawstatuspermanentG4.cptr();
  }
  const EventContext& event_context=Gaudi::Hive::currentContext();
  EventContext::ContextID_t slot=event_context.slot();
  EventContext::ContextEvt_t event_id=event_context.evt();
  std::lock_guard<std::mutex> lock(m_cacheMutex);
  if(slot>=m_evtperm.size()) {
     m_evtperm.resize(slot+1);
     m_strawstatuspermanentcontainer.resize(slot+1);
  }

  if(m_evtperm[slot]!=event_id) {
    SG::ReadCondHandle<StrawStatusContainer> rp(m_permReadKey,event_context);
    m_strawstatuspermanentcontainer[slot]=(*rp);
    m_evtperm[slot]=event_id;
  }

  return m_strawstatuspermanentcontainer[slot];

}
inline const TRT_StrawStatusSummarySvc::StrawStatusContainer* TRT_StrawStatusSummarySvc::getStrawStatusHTContainer() {

  if(m_isGEANT4) {
    return m_strawstatusHTG4.cptr();
  }
  const EventContext& event_context=Gaudi::Hive::currentContext();
  EventContext::ContextID_t slot=event_context.slot();
  EventContext::ContextEvt_t event_id=event_context.evt();
  std::lock_guard<std::mutex> lock(m_cacheMutex);
  if(slot>=m_evtstatHT.size()) {
    m_evtstatHT.resize(slot+1);
    m_strawstatusHTcontainer.resize(slot+1);
  }

  if(m_evtstatHT[slot]!=event_id) {
    SG::ReadCondHandle<StrawStatusContainer> rht(m_statHTReadKey,event_context);
    m_strawstatusHTcontainer[slot]=(*rht);
    m_evtstatHT[slot]=event_id;
  }

  return m_strawstatusHTcontainer[slot];
}




inline StatusCode TRT_StrawStatusSummarySvc::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == ITRT_StrawStatusSummarySvc::interfaceID() )  {
    *ppvIf = dynamic_cast<ITRT_StrawStatusSummarySvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  } else if ( ITRT_ConditionsSvc::interfaceID() == riid ) {
    *ppvIf = dynamic_cast<ITRT_ConditionsSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthService::queryInterface( riid, ppvIf );

}

#endif 
