/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledSvc.h
 * interface file for service that keeps track of Tdaq enabling/disabling of SCT Rods.
 * @author shaun.roe@cern.ch
**/

#ifndef SCT_TdaqEnabledSvc_h
#define SCT_TdaqEnabledSvc_h

#include <set>
#include <list>
#include <map>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "SCT_Cabling/ISCT_CablingSvc.h" 

class Identifier;
class IdentifierHash;
class StatusCode;
class SCT_ID;

/**
 * @class SCT_ConfigurationConditionsSvc
 * Service that keeps track of Tdaq enabling/disabling of SCT Rods.
**/

class SCT_TdaqEnabledSvc: virtual public ISCT_ConditionsSvc, public AthService {
 friend class SvcFactory<SCT_TdaqEnabledSvc>;
public:
  //@name Service methods
  //@{
  // destructor
  SCT_TdaqEnabledSvc( const std::string & name, ISvcLocator* svc);
   virtual ~SCT_TdaqEnabledSvc(){}
   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
   static const InterfaceID & interfaceID();
  //@}
  
   ///Can the service report about the given component? (TdaqEnabledSvc can answer questions about a module or module side)
   virtual bool canReportAbout(InDetConditions::Hierarchy h);

   ///Is the detector element good?
   virtual bool isGood(const Identifier & elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);

   ///is it good?, using wafer hash
   virtual bool isGood(const IdentifierHash & hashId);

   ///Manually get the data in the structure before proceeding
   virtual StatusCode fillData();

   ///Overload 'fillData' to provide callback to data folder
   virtual StatusCode fillData(int& /*i*/ , std::list<std::string>& /*l*/);

   ///Are the data available?
   virtual bool filled() const;

   ///Can the data be filled during the initialize phase?
   virtual bool canFillDuringInitialize();
  
private:
   static const unsigned int NRODS; //!< This was 90 in run 1; changed to 128 on Oct 22, 2014
   //StringArrayProperty m_badElements; //list of bad (unconfigured) robs
   std::set<unsigned int> m_goodRods;
   std::set<IdentifierHash> m_goodIds;
   bool m_filled;
   std::string m_coolFolderName;
   //
  
   const SCT_ID * m_pHelper;
   bool m_useDatabase;
   ServiceHandle<StoreGateSvc>           m_detStore;                      //!< Handle on the detector store
   ServiceHandle<StoreGateSvc>           m_storeGateSvc;                  //!< Handle on storegate
   ServiceHandle<ISCT_CablingSvc>        m_cablingSvc;                    //!< Handle on SCT cabling service
   const DataHandle<CondAttrListCollection>   m_dbList;                   //!< implies multi channel folder used
   bool m_noneBad;
   bool unfilledRun() const;  //!<Before run 119253, the folder was never filled so it looks like a disabled detector: this is to flag that condition
   ///The folder name changed from run 1 to run 2; this function looks to see which folder has been pre-loaded
   std::string determineFolder(const std::string& option1, const std::string& option2) const;
};

inline const InterfaceID & SCT_TdaqEnabledSvc::interfaceID(){
  static const InterfaceID IID_SCT_TdaqEnabledSvc("SCT_TdaqEnabledSvc",1,0);
  return IID_SCT_TdaqEnabledSvc;
}

#endif
