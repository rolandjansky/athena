/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// Name: L1CaloPprPhos4ShapeMaker.h
// Package: TrigT1CaloCalibUtils
// 
// Author: Taylor Childers (childers@mail.cern.ch)
// 
// ***************************************************************************
#ifndef L1CALOPPRPHOS4SHAPEMAKER_H
#define L1CALOPPRPHOS4SHAPEMAKER_H

#include <vector>
#include <string>
#include <map>

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"

// Get the Event information
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Trigger Menu tools
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfHLTData/HLTStreamTag.h"

// L1Calo specific includes
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloEvent/RODHeader.h"

// Calo specific includes
// #include "TrigT1CaloCalibTools/L1CaloTTIdTools.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
// #include "TrigT1CaloCalibTools/L1CaloCells2TriggerTowers.h"
// #include "TrigT1CaloCalibToolInterfaces/IL1CaloCells2TriggerTowers.h"

// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// Local includes
#include "TrigT1CaloCalibTools/L1CaloPprPhos4ShapeCollection.h"

#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

class TH2F;

// The AthenaExample class inherits publically from Algorithm. 
class L1CaloPprPhos4ShapeMaker : public AthAlgorithm
{
public:
   // These are the standard Athena public member functions.
   L1CaloPprPhos4ShapeMaker(const std::string& name, ISvcLocator* pSvcLocator);

   virtual StatusCode initialize() override;
   virtual StatusCode execute() override;
   virtual StatusCode finalize ATLAS_NOT_THREAD_SAFE() override;
       
private:
   // Unless you provide a class with a default constructor, copy constructor and copy asignment operator
   // the compiler will provide them for you. We do not want that to happen, g++ will probably make a mess.
   // By declaring these private, they cannot be called and g++ will not fudge anything.
   L1CaloPprPhos4ShapeMaker();
   L1CaloPprPhos4ShapeMaker(const L1CaloPprPhos4ShapeMaker& rhs);
   void operator=(const L1CaloPprPhos4ShapeMaker& rhs);
   
   L1CaloCoolChannelId GetCoolId(const double eta,const double phi,const unsigned int layer);
   
   bool IsCoolIdValid(const L1CaloCoolChannelId coolId);
   
   L1CaloPprPhos4ShapeCollection* m_signalShapes;
   
   // These data members are our Athena tools.
   ToolHandle<LVL1::TrigT1CaloLWHistogramTool> m_histTool;
//    ToolHandle<TrigDec::TrigDecisionTool> m_trigger;
   
   StatusCode loadAthenaObjects();
   
   // for each cool id I read in the FullDelayData as set before
   // the run.
   bool m_currentDatabaseMapsFilled;
   std::string m_fullDataDelayName;
   std::map<unsigned int,unsigned int> m_currentFullDelayDataMap;
   std::string m_pedValueName;
   std::map<unsigned int,unsigned int> m_currentPedValueMap;
   std::string m_l1aFadcSliceName;
   unsigned int m_l1aFadcSlice;
   std::string m_errorCodeName;
   std::string m_timingRegimeName;
   std::string m_timingRegime;
   std::string m_ttcConfigurationName;
   std::string m_ttcConfiguration;
   
   // Data Colletions
   const TriggerTowerCollection* m_triggerTowers;
   typedef TriggerTowerCollection::const_iterator Itr_tt;
   StatusCode GetTriggerTowers();
   const DataVector<LVL1::RODHeader>* m_rodHeader;
   StatusCode GetRODHeader();
   const EventInfo* m_evtInfo;
   StatusCode GetEventInfo();
   unsigned int m_runNumber;
   const CondAttrListCollection* m_attrList;
   std::string m_pprChanCalibFolderName;
   std::string m_readoutConfigFolderName;
   std::string m_ppmDeadChannelsFolderName;
   std::string m_l1caloDerivedRunParsFolderName;
   std::string m_l1caloRunParametersFolderName;
   StatusCode GetDatabaseHandle(std::string folderName);
   
   void SetTimingRegime(std::string regime);
   
   /////////////////////////////////////////////////
   // Calo access tools

//    L1CaloTTIdTools* m_l1CaloTTIdTools;
   ToolHandle<LVL1::IL1CaloTTIdTools> m_l1CaloTTIdTools;
   const CaloIdManager* m_caloMgr;
   CaloTriggerTowerService* m_ttSvc;
   const TTOnlineID* m_ttOnlineIdHelper;  
   const CaloLVL1_ID* m_lvl1Helper;
//    L1CaloCells2TriggerTowers* m_cells2tt;
   
   // Personal Histograms - add your own here.
   // These are some simple examples.
   std::map<unsigned int,TProfile*>* m_signalProfiles;
   
   bool m_firstEvent;
   
   // properties
   std::string m_outputDirectory;
   unsigned int m_minSignalHeight;
   
   std::vector<unsigned int> m_selectedCoolIds;
   std::map<unsigned int,TH2F*> m_histosForSelectedCoolIds;
   static const unsigned short int m_maxSelectedCoolIds = 50;
   
   // Control plot style
   void SetAtlasStyle(void);
   
   bool m_isTile;
   bool m_isLAr;
   
};
#endif


