/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibLbSvc.h
 * Header file for the SCT_CalibLbSvc class
 * @author Shaun Roe
**/

#ifndef SCT_CalibLbSvc_h
#define SCT_CalibLbSvc_h

//STL includes
#include <string>

//Athena includes
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "SCT_CalibAlgs/ISCT_CalibEvtInfo.h"

//Inner detector includes
#include "InDetIdentifier/SCT_ID.h"

//local includes
#include "SCT_CalibAlgs/ISCT_CalibHistoSvc.h"

//fwd declarations
template <class TYPE> class SvcFactory;
namespace InDetDD{
  class  SCT_DetectorManager;
}
class StatusCode;
class ISvcLocator;
class IdentifierHash;

class SCT_CalibLbSvc: virtual public ISCT_CalibHistoSvc, public AthService{
  friend class SvcFactory<SCT_CalibLbSvc>;
  
public:
  //@name Service methods, reimplemented
  //@{
  SCT_CalibLbSvc(const std::string &name, ISvcLocator * svc); 
  virtual ~SCT_CalibLbSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  //interfaceID() implementation is in the baseclass
  virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
  //@}
  //@name ISCT_CalibHistoSvc methods, reimplemented
  //@{
  virtual bool book();
  virtual bool fill(const bool fromData=false);
  virtual bool read(const std::string & fileName);
  virtual bool fillFromData();
  //@}
private:
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<StoreGateSvc> m_evtStore;
  ServiceHandle<ISCT_CalibEvtInfo> m_evtInfo;
    
  const SCT_ID* m_pSCTHelper;
  const InDetDD::SCT_DetectorManager* m_pManager;
  SCT_ID::const_id_iterator m_waferItrBegin;
  SCT_ID::const_id_iterator m_waferItrEnd;
  typedef std::vector<int> VecInt;
  VecInt *m_sct_waferHash;
  VecInt *m_sct_firstStrip;
  VecInt *m_sct_rdoGroupSize;
  
  mutable int m_lumiBlock;
  
  //private use in this class
  int                m_LbRange;
  int                m_LbsToMerge;
  
  void
  fillLbForWafer(const IdentifierHash &waferHash, const int theFirstStrip, const int groupSize );
 
};
#endif
