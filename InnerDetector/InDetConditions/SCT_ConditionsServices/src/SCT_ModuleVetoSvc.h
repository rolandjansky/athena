/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ModuleVetoSvc.h
 * header file for service allowing one to declare modules as bad
 * @author shaun.roe@cern.ch
**/

#ifndef SCT_ModuleVetoSvc_h
#define SCT_ModuleVetoSvc_h
//STL includes
#include <string>
#include <set>

//Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

//local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class SCT_ID;
class StatusCode;
//class Identifier; < not fwd declared, used in a vector so need storage size


/**
 * @class SCT_ModuleVetoSvc
 * Service allowing one to manually declare detector elements as 'bad' in the joboptions file
**/
class SCT_ModuleVetoSvc: virtual public ISCT_ConditionsSvc, public AthService{
  friend class SvcFactory<SCT_ModuleVetoSvc>;
public:
  //@name Service methods
  //@{
  SCT_ModuleVetoSvc( const std::string & name, ISvcLocator* svc);
  virtual ~SCT_ModuleVetoSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();
  //@}
  
  ///Can the service report about the given component? (chip, module...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h);
  
  ///Is the detector element good?
  virtual bool isGood(const Identifier & elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  
  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash & hashId);

  
  ///Manually get the data in the structure before proceding
  virtual StatusCode fillData();
  
  //I'm going to fill this from job options, but these may pecify the database is to be used
  virtual StatusCode fillData(int& /*i*/ , std::list<std::string>& /*l*/);
  
  ///Are the data available?
  virtual bool filled() const;
  
  ///Can the data be filled during the initialize phase?
  virtual bool canFillDuringInitialize();
  
private:
  StringArrayProperty m_badElements; //list of bad detector elements (= module sides)
  std::set<Identifier> m_badIds;
  bool m_filled;
  const SCT_ID * m_pHelper;
  bool m_useDatabase;
  bool m_maskLayers;
  int m_maskSide;
  std::vector<int> m_layersToMask; 
  std::vector<int> m_disksToMask; 
  const DataHandle<AthenaAttributeList>   m_dbList;// implies single channel folder used
  ServiceHandle<StoreGateSvc> m_detStore;
};

inline const InterfaceID & SCT_ModuleVetoSvc::interfaceID(){
  return ISCT_ConditionsSvc::interfaceID(); 
}

#endif
