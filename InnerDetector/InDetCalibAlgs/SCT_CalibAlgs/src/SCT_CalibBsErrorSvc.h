/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_CalibBsErrorSvc.h
* Header file for the SCT_CalibBsErrorSvc class
* @author Shaun Roe
**/

#ifndef SCT_CalibBsErrorSvc_h
#define SCT_CalibBsErrorSvc_h


//STL includes
#include <string>

//Athena includes
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetIdentifier/SCT_ID.h"

#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"  //template parameter


//local includes
#include "SCT_CalibAlgs/ISCT_CalibHistoSvc.h"

//fwd declarations
template <class TYPE> class SvcFactory;
namespace InDetDD{
  class  SCT_DetectorManager;
}
class StatusCode;
class ISvcLocator;



class SCT_CalibBsErrorSvc:virtual public ISCT_CalibHistoSvc, public AthService{
  friend class SvcFactory<SCT_CalibBsErrorSvc>;
public:
  //@name Service methods, reimplemented
   //@{
  SCT_CalibBsErrorSvc(const std::string &name, ISvcLocator * svc); 
  virtual ~SCT_CalibBsErrorSvc(){}
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
   //@}
private:
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<StoreGateSvc> m_evtStore;
  ServiceHandle<ISCT_ByteStreamErrorsSvc> m_bytestreamErrorsSvc;
  const SCT_ID* m_pSCTHelper;


  // SCT specific numbers
  
  SCT_ID::const_id_iterator m_waferItrBegin;
  SCT_ID::const_id_iterator m_waferItrEnd;
  typedef std::vector<int> VecInt;
  VecInt *m_scterr_bec;       
  VecInt *m_scterr_layer;     
  VecInt *m_scterr_eta;       
  VecInt *m_scterr_phi;       
  VecInt *m_scterr_side;
  VecInt *m_scterr_rodid;     
  VecInt *m_scterr_channel;
  VecInt *m_scterr_type;


    //
  void
    fillBsErrorsForWafer(const Identifier & waferId, const int type);

};
#endif
