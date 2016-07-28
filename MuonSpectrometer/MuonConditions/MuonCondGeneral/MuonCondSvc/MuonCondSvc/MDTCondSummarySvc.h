/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_MDTCONDSUMMARYSVC_H
#define MUONCONDSVC_MDTCONDSUMMARYSVC_H
//STL includes
#include <string>
#include <set>

#include <vector>

//Gaudi Includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

//local includes
#include "MuonCondSvc/MuonHierarchy.h"

#include "MuonCondInterface/IMDTConditionsSvc.h"
#include "MuonCondInterface/IMDT_DCSConditionsSvc.h"
#include "MuonCondInterface/IMDT_DCSConditionsRun2Svc.h"
//Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"





//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class MdtIdHelper;
class StatusCode;

class Identifier;
class IMDTConditionsSvc;
class IMDT_DCSConditionsSvc;
class IMDT_DCSConditionsRun2Svc;


class MDTCondSummarySvc: virtual public IMDTConditionsSvc, public AthService{
  friend class SvcFactory<MDTCondSummarySvc>;
public:

  MDTCondSummarySvc( const std::string & name, ISvcLocator* svc);
  virtual ~MDTCondSummarySvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();


  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));
  StatusCode update_MDT(IOVSVC_CALLBACK_ARGS_P(I,keys));
 

  virtual bool isGoodChannel(const Identifier & Id) const;
  virtual bool isGoodChamber(const Identifier & Id) const;
  virtual bool isGoodMultiLayer(const Identifier & Id) const;  
  virtual bool isGood(const Identifier & Id) const;
 
  virtual const std::vector<std::string>&  deadStations() const;
  virtual const std::vector<std::string>& deadMultiLayers() const;
  virtual const std::vector<std::string>& deadTubes()  const;
  
  virtual const std::vector<Identifier> &deadStationsId() const;
  virtual const std::vector<Identifier>& deadMultiLayersId() const;
  virtual const std::vector<Identifier>& deadLayersId() const;
  virtual const std::vector<Identifier>& deadTubesId() const;
  virtual const std::vector<Identifier>& List_Chambers_with_deadTube() const;
 
private:

 ServiceHandleArray<IMDTConditionsSvc> m_reportingServices; //!< list of services to be used
  
 std::vector<Identifier> m_emptyId;
 std::vector<std::string> m_empty;
 const MdtIdHelper * m_pHelper;

 ServiceHandle<StoreGateSvc> m_detStore;


 bool m_noReports; 

 //compare method for the binary search
 static bool Compare(const Identifier &a, const Identifier &b) {return (a>b);}
 
};



inline const InterfaceID & MDTCondSummarySvc::interfaceID(){
  return IMDTConditionsSvc::interfaceID(); 
}

#endif
