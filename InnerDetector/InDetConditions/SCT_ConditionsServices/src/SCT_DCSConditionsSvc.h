/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_DCSConditionsSvc_h
#define SCT_DCSConditionsSvc_h
/**
 * @file SCT_DCSConditionsSvc.h
 *
 * @brief Header file for the SCT_DCSConditionsSvc class 
 *  in package SCT_ConditionsServices
 *
 * @author A. R-Veronneau 26/02/07, Shaun Roe 4/4/2008
 **/

//
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
//
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_DCSConditionsSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaKernel/IOVSvcDefs.h" 
#include "AthenaKernel/IIOVDbSvc.h" 
//STL
#include <vector>
#include <list>
#include <string>
#include <map>

class IIOVSvc;
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class StatusCode;
class SCT_DCSConditionsData;
class SCT_ID;

/**
 * Class to provide DCS information about modules from the COOL database
 **/
class SCT_DCSConditionsSvc: virtual public ISCT_DCSConditionsSvc,virtual public AthService {
  friend class SvcFactory<SCT_DCSConditionsSvc>;
  
public:
  SCT_DCSConditionsSvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~SCT_DCSConditionsSvc(){ /*do nothing*/}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID& interfaceID();
  
  
  /// @name Methods to be implemented from virtual baseclass methods, when introduced
  //@{
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h);
  //returns the module ID (int), or returns 9999 (not a valid module number) if not able to report
  virtual  Identifier getModuleID(const Identifier & elementId, InDetConditions::Hierarchy h);
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier & elementId,InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash & hashId);
  //Returns HV (0 if there is no information)
  virtual float modHV(const Identifier & elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  //Does the same for hashIds
  virtual float modHV(const IdentifierHash & hashId);
  //Returns temp0 (0 if there is no information)
  virtual float hybridTemperature(const Identifier & elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  //Does the same for hashIds
  virtual float hybridTemperature(const IdentifierHash & hashId);
  //Returns temp0 + correction for Lorentz angle calculation (0 if there is no information)
  virtual float sensorTemperature(const Identifier & elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  //Does the same for hashIds
  virtual float sensorTemperature(const IdentifierHash & hashId);
  virtual StatusCode fillData(int&  i  , std::list<std::string>& keys);
  ///Manually get the data in the structure before proceding
  virtual StatusCode fillData(){return StatusCode::FAILURE;}
  virtual bool filled() const;
  ///Need to access cool on every callback, so can't fill during initialize
  virtual bool canFillDuringInitialize(){return false; }
  //@}
    
private:
  //Declare Storegate container
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<IIOVDbSvc> m_IOVDbSvc; //!< Handle on the IOVDb service
  // list folders to be read as CondAttrListCollection*
  StringArrayProperty m_par_atrcollist;
  bool m_dataFilled;
  int m_status;
  //DataHandle for callback
  const DataHandle<CondAttrListCollection> m_DCSData_HV;
  const DataHandle<CondAttrListCollection> m_DCSData_MT;
  const DataHandle<CondAttrListCollection> m_DCSData_CS;
  //Key for DataHandle
  BooleanProperty m_readAllDBFolders;
  BooleanProperty m_returnHVTemp;
  BooleanProperty m_dropFolder; 
  float m_barrel_correction;
  float m_ecInner_correction;
  float m_ecOuter_correction;
  float m_hvLowLimit;
  float m_hvUpLimit;
  SCT_DCSConditionsData* m_pBadModules;
  std::map<CondAttrListCollection::ChanNum, float >* m_pModulesHV;
  std::map<CondAttrListCollection::ChanNum, float >* m_pModulesTemp0;
  std::map<CondAttrListCollection::ChanNum, float >* m_pModulesTemp1;
  const SCT_ID*  m_pHelper;
  //IdentifierHash  m_hashId;
  //IdentifierHash  channum_hash;
  Identifier  m_moduleId;
  Identifier  m_waferId;
  std::string m_folderPrefix;
  std::string m_chanstatCut;
  bool m_useHV;
  float m_useHVLowLimit;
  float  m_useHVUpLimit;
  std::string m_useHVChanCut;
  std::vector<std::pair<bool, float> > m_cacheHV;
};

// inline const InterfaceID & SCT_DCSConditionsSvc::interfaceID(){
//   static const InterfaceID IID_SCT_DCSConditionsSvc("SCT_DCSConditionsSvc",1,0);
//   return IID_SCT_DCSConditionsSvc;
// }


#endif // SCT_DCSConditionsSvc_h 
