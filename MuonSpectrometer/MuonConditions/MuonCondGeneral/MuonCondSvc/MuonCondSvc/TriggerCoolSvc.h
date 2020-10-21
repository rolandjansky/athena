/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_TRIGGERCOOLSVC_H
#define MUONCONDSVC_TRIGGERCOOLSVC_H


#include <vector>
#include <string>
#include <fstream>

#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"


#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
//Added for attribute list declarations (might not need all of these)
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"


#include "Identifier/Identifier.h"


#include "MuonCondInterface/ITriggerCoolSvc.h"
template <class TYPE> class SvcFactory;
namespace MuonCalib {

  class TriggerCoolSvc : public AthService, virtual public ITriggerCoolSvc
{
  
 friend class SvcFactory<TriggerCoolSvc>; 
 public:
  TriggerCoolSvc(const std::string& name, ISvcLocator* svc);

  virtual ~TriggerCoolSvc();
  
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  
  virtual const InterfaceID& type() const;
  
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  void getString(); 
 
  virtual StatusCode writeToDBPhi(const std::string& phiFolder,const std::string& filename_CM,const std::string& filename_Th0,
				  const int chan,const std::string& info) const;
  virtual StatusCode writeToDBEta(const std::string& etaFolder ,const std::string& filename_CM,const std::string& filename_Th0,
				  const int chan, const std::string& sequence,const std::string& info) const;  
  virtual StatusCode getData(const std::string& etaFolder ,const int chan) const;  

  
 private:
  
  
  
 
  StoreGateSvc* p_detstore;
  
 
  mutable MsgStream m_log;
  
  /**Cool folder name*/
  std::string m_etafolder;
  std::string m_phifolder;

  bool m_debugLevel;
  
    
  
};




}
#endif // MUONCONDSVC_TRIGGERCOOLSVC_H
