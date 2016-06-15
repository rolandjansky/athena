/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibHvSvc.h
 * Header file for the SCT_CalibHvSvc class
 * @author Shaun Roe
**/

#ifndef SCT_CalibHvSvc_h
#define SCT_CalibHvSvc_h

//STL includes
#include <string>
#include <vector>
#include <queue>
#include <utility> //for 'pair'

//Athena includes
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

//Inner detector includes
#include "InDetIdentifier/SCT_ID.h"

//local includes
#include "SCT_CalibAlgs/ISCT_CalibHistoSvc.h"
#include "SCT_ConditionsServices/ISCT_DCSConditionsSvc.h" //template parameter
#include "SCT_Cabling/ISCT_CablingSvc.h"  //template parameter

//fwd declarations
template <class TYPE> class SvcFactory;
namespace InDetDD{
  class  SCT_DetectorManager;
}
class StatusCode;
class ISvcLocator;
class IdentifierHash;
class EventInfo;

class SCT_CalibHvSvc:virtual public ISCT_CalibHistoSvc, public AthService{
  friend class SvcFactory<SCT_CalibHvSvc>;
  
public:
  //@name Service methods, reimplemented
  //@{
  SCT_CalibHvSvc(const std::string &name, ISvcLocator * svc); 
  virtual ~SCT_CalibHvSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  //interfaceID() implementation is in the baseclass
  virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
  //@}
  //@name ISCT_CalibHistoSvc methods, reimplemented
  //@{
  virtual bool book();
  virtual bool fill(const bool fromData=false);
  virtual bool fillFromData();
  //@}
private:
  enum { n_chipPerSide = 6, n_chipPerModule = 12, n_stripPerChip = 128, n_etaInBarrel = 12,
         n_phiBinsB0 = 32, n_phiBinsB1 = 40, n_phiBinsB2 = 48, n_phiBinsB3 = 56,
         n_phiBinsECShort = 40, n_phiBinsECMiddle = 40, n_phiBinsECOuter = 52, n_elements=8176,
         n_BSErrorType = 15, firstBSErrorType = 0, lastBSErrorType = 14 };
         
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<StoreGateSvc> m_evtStore;
  ServiceHandle<ISCT_DCSConditionsSvc>            m_DCSConditionsSvc;
  ServiceHandle<ISCT_CablingSvc>                  m_cablingSvc;
  
  const SCT_ID* m_pSCTHelper;
  SCT_ID::const_id_iterator m_waferItrBegin;
  SCT_ID::const_id_iterator m_waferItrEnd;
  typedef std::vector<int> VecInt;
  VecInt *m_sct_waferHash;
  VecInt *m_sct_numHitsInWafer;
  mutable int m_lumiBlock;
  
  //private use in this class
  int m_maxq;
  // For HV trips
  std::vector< std::queue<int> > m_phvtripQueue;
  std::queue<int> m_prevLBN;
  VecInt m_phvtripRunningTotalInt;
  VecInt m_phvtripProcessedEventsInt;
  VecInt m_phvtripHasItTripped;
  VecInt m_phvtripHasItTripped_prev;
  //
  int m_phvtripPrevTime;
  int m_phvtripFirstTime;
  double          m_absolutetriplimit;
  double          m_relativetriplimit;
   int m_tq[100];

  std::vector< std::vector< std::pair<int, int> > > m_summarytrips;
  std::vector< std::vector< std::pair<int, int> > > m_summarytripslb;
  const EventInfo* m_evt;
  bool m_outputLowHits;
  int m_lowHitCut;
 ///retrieve a service and report if it failed
 template<class S>
 bool retrievedService(S & service, const std::string & serviceName){
   if (service.retrieve().isFailure() ) return msg(MSG::ERROR)<<"Unable to retrieve "<<serviceName<<endreq, false;
   return true;
 }

};
#endif
