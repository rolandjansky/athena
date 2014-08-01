/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyView_H
#define MboyView_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ToolHandle.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

#include "MboyView/MboyViewEventSelectionTool.h"
#include "MboyView/MboyViewGenEventSelectionTool.h"
#include "MboyView/MboyViewEventInfoTool.h"
#include "MboyView/MboyViewMuonRecDigiStoreTDSTool.h"
#include "MboyView/MboyViewIDRecDigiTool.h"
#include "MboyView/MboyViewTrackRecordCollectionTool.h"
#include "MboyView/MboyViewCaloTool.h"
#include "MboyView/MboyViewCombinedMuonContainerTool.h"
#include "MboyView/MboyViewTrackParticleContainerTool.h"
#include "MboyView/MboyViewTrkTrackCollectionTool.h"
#include "MboyView/MboyViewTrkSegmentCollectionTool.h"
#include "MboyView/MboyViewMcEventCollectionTool.h"
#include "MboyView/MboyViewEgammaContainerTool.h"
#include "MboyView/MboyViewVertexContainerTool.h"
#include "MboyView/MboyViewDumpCombinedMuonContainerTool.h"
//apolici
#include "MboyView/MboyViewJetContainerTool.h"
#include "MboyView/MboyViewMETContainerTool.h"
#include "MboyView/MboyViewL1RoIContainerTool.h"
//apolici

  /**
   @class MboyView

   This class is the top algorithm managing production of Persint files

  @author samusog@cern.ch
  
  */

class MboyView:public AthAlgorithm {
public:
    MboyView(const std::string& name, ISvcLocator* pSvcLocator);

public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////

   int m_SwitchOff ; //!<  Switch Off Option

   std::string m_NameOfMboyViewOutputFile ; //!< Name of the MboyView output file
   std::string m_NameOfMboyViewOutputFileCombinedDump  ; //
 
   /**Evt Selection */
   int m_EvtSelectionType;
   int m_EvtSelectionThreshold;
   ToolHandle< MboyViewEventSelectionTool > p_MboyViewEventSelectionTool;
   std::string m_MuonRecDigiStoreTDSLocationForEventSelection;
   std::string m_TrkTrackCollectionLocationForEventSelection;
   
   /**GenEvent Selection */
   int m_GenEvtSelectionType;
   float m_GenEvtSelectionEtaMin;
   float m_GenEvtSelectionEtaMax;
   ToolHandle< MboyViewGenEventSelectionTool > p_MboyViewGenEventSelectionTool;
   std::vector<std::string> m_TrackRecordCollectionLocationForGenEventSelectionList;

   /**EventInfo Viewer */
   ToolHandle< MboyViewEventInfoTool > p_MboyViewEventInfoTool;
      
   /**Digits Viewer */
   int m_ViewDigits ;
   ToolHandle< MboyViewMuonRecDigiStoreTDSTool > p_MboyViewMuonRecDigiStoreTDSTool;
   std::string m_MuonRecDigiStoreTDSLocation;
   int m_ViewAthIds;
   int m_ATCOSM;
   int m_PrintRSLine;

   /**ID Digits Viewer */
   int m_ViewIDDigits ;
   ToolHandle< MboyViewIDRecDigiTool > p_MboyViewIDRecDigiTool;
   int m_ViewTRTRDO;
   std::string m_TRTRDOContainerName;
   int m_TRTToTMin ;
   int m_TRTToTMax ;
   int m_ViewSCTRDO;
   std::string m_SCTRDOContainerName;
   int m_ViewPixelRDO;
   std::string m_PixelRDOContainerName;

   /**TrackRecordCollection Viewer */
   int m_ViewTrackRecordCollection ;
   ToolHandle< MboyViewTrackRecordCollectionTool > p_MboyViewTrackRecordCollectionTool;
   std::vector<std::string> m_TrackRecordCollectionLocationList;

   /**Calo Viewer */
   int m_ViewCalo ;
   ToolHandle< MboyViewCaloTool > p_MboyViewCaloTool;
   float m_CaloMinimumE;

   /**CombinedMuonContainer Viewer */
   int m_ViewCombinedMuonContainer ;
   ToolHandle< MboyViewCombinedMuonContainerTool > p_MboyViewCombinedMuonContainerTool;
   std::vector<std::string> m_CombinedMuonContainerLocationList;

   /**TrackParticleContainer Viewer */
   int m_ViewTrackParticleContainer ;
   ToolHandle< MboyViewTrackParticleContainerTool > p_MboyViewTrackParticleContainerTool;
   std::vector<std::string> m_TrackParticleContainerLocationList;

   /**TrkTrackCollection Viewer */
   int m_ViewTrkTrackCollection ;
   ToolHandle< MboyViewTrkTrackCollectionTool > p_MboyViewTrkTrackCollectionTool;
   std::vector<std::string> m_TrkTrackCollectionLocationList;

   /**TrkSegmentCollection Viewer */
   int m_ViewTrkSegmentCollection ;
   ToolHandle< MboyViewTrkSegmentCollectionTool > p_MboyViewTrkSegmentCollectionTool;
   std::vector<std::string> m_TrkSegmentCollectionLocationList;

   /**McEventCollection Viewer */
   int m_ViewMcEventCollection ;
   ToolHandle< MboyViewMcEventCollectionTool > p_MboyViewMcEventCollectionTool;
   std::vector<std::string> m_McEventCollectionLocationList;
   float m_PtMinID;
  
   /**EgammaContainer Viewer */
   int m_ViewEgammaContainer ;
   ToolHandle< MboyViewEgammaContainerTool > p_MboyViewEgammaContainerTool;
   std::vector<std::string> m_EgammaContainerLocationList;
   std::string m_EMTrackMatchContainerName;
   float m_etEgammaCut;
   float m_etaEgammaCut;

//apolici
   /**JetContainer Viewer */
   int m_ViewJetContainer;
   ToolHandle< MboyViewJetContainerTool > p_MboyViewJetContainerTool;
   std::vector<std::string> m_JetContainerLocationList;
   float m_etJetCut;
   float m_etaJetCut;

   /**METContainer Viewer */
   int m_ViewMETContainer;
   ToolHandle< MboyViewMETContainerTool > p_MboyViewMETContainerTool;
   std::vector<std::string> m_METContainerLocationList;
   float m_etMETCut;


   /**L1RoIContainer Viewer */
   int m_ViewL1RoIContainer;
   ToolHandle< MboyViewL1RoIContainerTool > p_MboyViewL1RoIContainerTool;
   std::vector<std::string> m_L1RoIContainerLocationList;
//apolici

   /**VertexContainer Viewer */
   int m_ViewVertexContainer ;
   ToolHandle< MboyViewVertexContainerTool > p_MboyViewVertexContainerTool;
   std::vector<std::string> m_VertexContainerLocationList;

   /**Dump  CombinedMuonContainer Viewer */
   int m_ViewCombinedDump ;
   ToolHandle< MboyViewDumpCombinedMuonContainerTool > p_MboyViewDumpCombinedMuonContainerTool;
   std::vector<std::string> m_CombinedDumpLocationList;

   std::ofstream  m_OutMboyView ; //!< The file to dump the View
   std::vector<std::ofstream*> m_OutMboyViewCombinedDumpList;

};

#endif

