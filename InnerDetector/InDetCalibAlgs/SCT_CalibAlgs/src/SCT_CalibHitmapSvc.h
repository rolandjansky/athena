/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibHitmapSvc.h
 * Header file for the SCT_CalibHitmapSvc class
 * @author Shaun Roe
**/

#ifndef SCT_CalibHitmapSvc_h
#define SCT_CalibHitmapSvc_h

//STL includes
#include <string>

//Athena includes
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

//Inner detector includes
#include "InDetIdentifier/SCT_ID.h"

//local includes
#include "SCT_CalibAlgs/ISCT_CalibHistoSvc.h"


//fwd declarations
template <class TYPE> class SvcFactory;
class StatusCode;
class ISvcLocator;

class SCT_CalibHitmapSvc:virtual public ISCT_CalibHistoSvc, public AthService{
  friend class SvcFactory<SCT_CalibHitmapSvc>;
  
public:
  //@name Service methods, reimplemented
  //@{
  SCT_CalibHitmapSvc(const std::string &name, ISvcLocator * svc); 
  virtual ~SCT_CalibHitmapSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  //interfaceID() implementation is in the baseclass
  virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
  //@}
  //@name ISCT_CalibHistoSvc methods, reimplemented
  //@{
  virtual bool book();
  virtual bool read(const std::string & fileName);
  virtual bool fill(const bool fromData=false);
  virtual bool fillFromData();
  //  virtual bool binHistograms(const int eventsPerWindow);
  //@}
private:
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<StoreGateSvc> m_evtStore;
 
  const SCT_ID* m_pSCTHelper;
  SCT_ID::const_id_iterator m_waferItrBegin;
  SCT_ID::const_id_iterator m_waferItrEnd;
  typedef std::vector<int> VecInt;
  VecInt *m_sct_waferHash;
  VecInt *m_sct_firstStrip;
  VecInt *m_sct_rdoGroupSize;
  


};
#endif
