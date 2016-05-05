/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigL2PattRecoStrategyB
// -------------------------------
// ATLAS Collaboration
//
// 16.01.2011 Package created
//
// Author: Andrea Coccaro
// e-mail: Andrea.Coccaro AT cern.ch
//
// implements SiTrack algorithm
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>
//#include "GaudiKernel/ToolFactory.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrigInDetRecoTools/ITrigL2PattRecoStrategy.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

//#include "SiSpacePoint/SCT_SpacePoint.h"
//#include "SiSpacePoint/PixelSpacePoint.h"

//#include "InDetIdentifier/PixelID.h" 
//#include "InDetIdentifier/SCT_ID.h" 

#include "IRegionSelector/IRegSelSvc.h"
#include "TrigInDetRecoTools/TrigL2PattRecoStrategyB.h"

#include "AthenaKernel/Timeout.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

/** static members **/
StrBTrk*   TrigL2PattRecoStrategyB::m_trks=0;
StrBTrack* TrigL2PattRecoStrategyB::m_full_trks=0;


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigL2PattRecoStrategyB::TrigL2PattRecoStrategyB(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_regionSelector("RegSelSvc", n),
  m_numberingTool("TrigL2LayerNumberTool")
{
  declareInterface< ITrigL2PattRecoStrategy >( this );

  /** Layer properties. */
  declareProperty("Map_LayerOneMultiplicity",         m_mapLayerOneMultiplicity    = 2);
  declareProperty("Map_LayerTwoMultiplicity",         m_mapLayerTwoMultiplicity    = 2);
  declareProperty("Map_ForcedInefficientLayers",      m_mapForcedInefficientLayers);
  declareProperty("Map_ForcedTurnedOffLayers",        m_mapForcedTurnedOffLayers);
  declareProperty("Map_MaxLayerFailureFraction",      m_mapMaxLayerFailureFraction = 0.001);
  declareProperty("Map_MaxTotalFailureFraction",      m_mapMaxTotalFailureFraction = 0.00001);

  /** New cut properties*/
  declareProperty("Reco_NewCuts",                     m_newCuts                    = false);
  declareProperty("Reco_NewCutsRZ",                   m_newCutsRZ                  = false);
  declareProperty("Reco_PtMinSeedDiff",               m_recoPtMinSeedDiff          = 0.0);
  declareProperty("Reco_D0MaxSeedDiff",               m_recoD0MaxSeedDiff          = 2.0);
  declareProperty("Reco_PtMinTripletDiff",            m_recoPtMinTripletDiff       = 0.0);
  declareProperty("Reco_D0MaxTripletDiff",            m_recoD0MaxTripletDiff       = 1.0);
  declareProperty("Reco_PtMin",                       m_recoPtMin                  = 1000.0);
  declareProperty("Reco_D0Max",                       m_recoD0Max                  = 3.0);
  declareProperty("Reco_TimingFactor",                m_recoTimingFactor           = 3.0);
  declareProperty("Reco_AlignmentScaling",            m_recoAlignmentScaling       = 1.0);
  declareProperty("Reco_OutlierRemoval",              m_recoOutlierRemoval         = false);
  declareProperty("Reco_OutlierRemovalCut",           m_recoOutlierRemovalCut      = 0.9);
  declareProperty("Reco_OutlierRemovalLayers",        m_recoOutlierRemovalLayers   = 4);
  declareProperty("Reco_OutlierRemovalD0",            m_recoOutlierRemovalD0       = 0.005);
  declareProperty("Reco_OutlierRemovalPt",            m_recoOutlierRemovalPt       = 40);
  declareProperty("Reco_MaxSeeds",                    m_recoMaxSeeds               = 30000);
  declareProperty("Reco_MaxTracks",                   m_recoMaxTracks              = 3000);
  declareProperty("Reco_Z0Bin",                       m_recoZ0Bin                  = 20);
  declareProperty("Reco_EtaBin",                      m_recoEtaBin                 = 0.1);

  /** Preliminary vertexing properties. */				   
  declareProperty("PreVertexing_Enable",              m_preVertexingEnable         = false);
  declareProperty("PreVertexing_VtxName",             m_preVertexingVtxName        = "TrueTrigVertexColl");
  declareProperty("PreVertexing_ZCut",                m_preVertexingZCut           = 5);
									   
  /** Seeding properties. */
  declareProperty("Seeding_PreCutRZ",                 m_seedingPreCutRZ            = 10000);
  declareProperty("Seeding_PreCutPhi",                m_seedingPreCutPhi           = 3.15);
  declareProperty("Seeding_PreCutRPhi",               m_seedingPreCutRPhi          = 0.001);
  declareProperty("Seeding_CutRZ",                    m_seedingCutRZ               = 1.5);

  /** Vertexing properties. */
  declareProperty("Vertexing_Enable",                 m_vertexingEnable            = false);
  declareProperty("Vertexing_CutRPhi",                m_vertexingCutRPhi           = 0.00025);
  //declareProperty("Vertexing_CutRZ",                  m_vertexingCutRZ             = 1000);
  m_vertexingCutRZ = 1000;
  declareProperty("Vertexing_MaxVertices",            m_vertexingMaxVertices       = 3);
  declareProperty("Vertexing_WindowSize",             m_vertexingWindowSize        = 2.0);
  declareProperty("Vertexing_WindowSegments",         m_vertexingWindowSegments    = 4);

  /** Extension properties. */
  declareProperty("Extension_PreCutRZ",               m_extensionPreCutRZ              = 10000);
  declareProperty("Extension_PreCutPhi",              m_extensionPreCutPhi             = 3.15);
  declareProperty("Extension_PreCutRPhi",             m_extensionPreCutRPhi            = 0.0006);
  declareProperty("Extension_PreCutRPhiDiff",         m_extensionPreCutRPhiDiff        = 0.0006);
  declareProperty("Extension_CutRZPixel",             m_extensionCutRZPixel            = 0.8);
  declareProperty("Extension_CutRZ",                  m_extensionCutRZ                 = 4.5);
  declareProperty("Extension_CutRPhi",                m_extensionCutRPhi               = 0.0035);
  declareProperty("Extension_CutSpacePoints",         m_extensionCutSpacePoints        = 4);
  declareProperty("Extension_CutSpacePointsMiss1",    m_extensionCutSpacePointsMiss1   = 4);
  declareProperty("Extension_CutSpacePointsMiss2",    m_extensionCutSpacePointsMiss2   = 4);
  declareProperty("Extension_SolveTriplets",          m_extensionSolveTriplets         = true);
  declareProperty("Extension_SolveTripletsDRCut",     m_extensionSolveTripletsDRCut    = 9.0);
  declareProperty("Extension_SolveTripletsPhiScale",  m_extensionSolveTripletsPhiScale = 0.0006);
  declareProperty("Extension_SolveTripletsEtaScale",  m_extensionSolveTripletsEtaScale = 0.0022);
  declareProperty("Extension_SolveTracks",            m_extensionSolveTracks           = true);
  declareProperty("Extension_SolveV0Tracks",          m_extensionSolveV0Tracks         = true);
  declareProperty("Extension_SolveTracksDRCut",       m_extensionSolveTracksDRCut      = 9.0);
  declareProperty("Extension_SolveTracksPhiScale",    m_extensionSolveTracksPhiScale   = 0.0006);
  declareProperty("Extension_SolveTracksEtaScale",    m_extensionSolveTracksEtaScale   = 0.0022);
  declareProperty("Extension_SolveShared",            m_extensionSolveShared           = true);
  declareProperty("Extension_MaxMissingLayers",       m_extensionMaxMissingLayers      = 20);
  declareProperty("Extension_Refit",                  m_extensionRefit                 = true);
  declareProperty("Extension_SpacePoints",            m_extensionSpacePoints           = 5);
  declareProperty("Extension_SpacePointsMiss1",       m_extensionSpacePointsMiss1      = 5);
  declareProperty("Extension_SpacePointsMiss2",       m_extensionSpacePointsMiss2      = 5);
  declareProperty("Extension_TRTExtension",           m_extensionTRTExtension          = true);

  /** Debug properties. */
  declareProperty("Debug_Tuning",                     m_debugTuning                    = false);
  declareProperty("Debug_TuningGoodSeeds",            m_debugTuningGoodSeeds           = false);
  declareProperty("Debug_TuningBadSeeds",             m_debugTuningBadSeeds            = false);
  declareProperty("Debug_TuningGoodTriplets",         m_debugTuningGoodTriplets        = false);
  declareProperty("Debug_TuningBadTriplets",          m_debugTuningBadTriplets         = false);
 
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode TrigL2PattRecoStrategyB::initialize() {

  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  sc = m_numberingTool.retrieve();
  if (sc.isFailure()) {
    athenaLog << MSG::FATAL << "Tool " << m_numberingTool << " not found " << endreq;
    return sc;
  }
  m_laySctB  = m_numberingTool->offsetBarrelSCT();
  m_layPixEC = m_numberingTool->offsetEndcapPixels();
  m_laySctEC = m_numberingTool->offsetEndcapSCT();
  m_layMax   = m_numberingTool->maxSiliconLayerNum();
  StrBTrack::setMaxLay(m_layMax);

  athenaLog << MSG::INFO << "set m_layMax to " << m_layMax << endreq;
  athenaLog << MSG::INFO << "set m_laySctB to " << m_laySctB << endreq;

  sc = m_regionSelector.retrieve();
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL << "Unable to retrieve RegionSelector tool " << m_regionSelector.type() << endreq;
    return sc;
  }

  //Set vertexing RZ cut to max z-width of fullscan RoI
  TrigRoiDescriptor fs_roi(true);
  m_vertexingCutRZ = fs_roi.zedPlus() + 30;//z-width should be symmetric


  /************************************/
  /** FILL LAYERS *********************/
  /************************************/

  /** Define geometry. */
  m_geoR = new float[19]; m_geoRmin = new float[19]; m_geoRmax = new float[19];
  m_geoZ = new float[19]; m_geoZmin = new float[19]; m_geoZmax = new float[19];

  m_geoR[0] =50.5;   m_geoZmin[0] =-405; m_geoZmax[0]=405;
  m_geoR[1] =88.5;   m_geoZmin[1] =-405; m_geoZmax[1]=405;
  m_geoR[2] =122.5;  m_geoZmin[2] =-405; m_geoZmax[2]=405;
  m_geoR[3] =299;    m_geoZmin[3] =-755; m_geoZmax[3]=755;
  m_geoR[4] =371;    m_geoZmin[4] =-755; m_geoZmax[4]=755;
  m_geoR[5] =443;    m_geoZmin[5] =-755; m_geoZmax[5]=755;
  m_geoR[6] =514;    m_geoZmin[6] =-755; m_geoZmax[6]=755;

  m_geoZ[7] =496;    m_geoRmin[7] =88;   m_geoRmax[7] =185;
  m_geoZ[8] =581;    m_geoRmin[8] =88;   m_geoRmax[8] =185;
  m_geoZ[9] =651;    m_geoRmin[9] =88;   m_geoRmax[9] =185;
  m_geoZ[10]=861.5;  m_geoRmin[10]=330;  m_geoRmax[10]=565;
  m_geoZ[11]=940;    m_geoRmin[11]=270;  m_geoRmax[11]=565;
  m_geoZ[12]=1098.5; m_geoRmin[12]=270;  m_geoRmax[12]=565;
  m_geoZ[13]=1306;   m_geoRmin[13]=270;  m_geoRmax[13]=565;
  m_geoZ[14]=1408;   m_geoRmin[14]=270;  m_geoRmax[14]=565;
  m_geoZ[15]=1778;   m_geoRmin[15]=270;  m_geoRmax[15]=565;
  m_geoZ[16]=2122.5; m_geoRmin[16]=330;  m_geoRmax[16]=565;
  m_geoZ[17]=2511.5; m_geoRmin[17]=400;  m_geoRmax[17]=565;
  m_geoZ[18]=2742.5; m_geoRmin[18]=430;  m_geoRmax[18]=565;


  /** Fill layer groups. */
  std::list<int> gr1,gr2,gr3,gr4,gr5,gr6,gr7,gr8,gr9; // layer groups as a function of eta

  if(m_layMax==19) {
    
    /* Group definitions
       gr1:         eta<1        0  1  2  3  4  5   6
       gr2:      1<=eta<1.5      0  1  2  3  4  5   6   10  11  12  13
       gr3:    1.5<=eta<1.7      0  1  2  3  4  5   7   10  12  12  13  14
       gr4:    1.7<=eta<1.8      0  1  2  3  4  7   10  11  12  13  14  15
       gr5:    1.8<=eta<1.9      0  1  2  3  7  8   11  12  13  14  15  16
       gr6:    1.9<=eta<2.1      0  1  2  3  7  8   9   11  12  13  14  15  16  17
       gr7:    2.1<=eta<2.2      0  1  2  7  8  9   11  12  13  14  15  16  17
       gr8:    2.2<=eta<2.4      0  1  2  7  8  9   13  14  15  16  17  18
       gr9:         eta>=2.4     0  9  15  16  17  18
    */
    
    gr1.push_back(0); gr1.push_back(1); gr1.push_back(2); gr1.push_back(3); gr1.push_back(4); gr1.push_back(5); gr1.push_back(6);
    
    gr2.push_back(0); gr2.push_back(1); gr2.push_back(2); gr2.push_back(3); gr2.push_back(4); gr2.push_back(5); gr2.push_back(6);
    gr2.push_back(10); gr2.push_back(11); gr2.push_back(12); gr2.push_back(13);
    
    gr3.push_back(0); gr3.push_back(7); gr3.push_back(1); gr3.push_back(2);
    gr3.push_back(3); gr3.push_back(4); gr3.push_back(5); gr3.push_back(10); gr3.push_back(11); gr3.push_back(12); gr3.push_back(13); gr3.push_back(14);
    
    gr4.push_back(0); gr4.push_back(7); gr4.push_back(1); gr4.push_back(2);
    gr4.push_back(3); gr4.push_back(4); gr4.push_back(10); gr4.push_back(11); gr4.push_back(12); gr4.push_back(13); gr4.push_back(14); gr4.push_back(15);
    
    gr5.push_back(0); gr5.push_back(7); gr5.push_back(1); gr5.push_back(8); gr5.push_back(2);
    gr5.push_back(3); gr5.push_back(11); gr5.push_back(12); gr5.push_back(13); gr5.push_back(14); gr5.push_back(15); gr5.push_back(16);
    
    gr6.push_back(0); gr6.push_back(7); gr6.push_back(1); gr6.push_back(8);  gr6.push_back(2); gr6.push_back(9);
    gr6.push_back(3); gr6.push_back(11); gr6.push_back(12); gr6.push_back(13); gr6.push_back(14); gr6.push_back(15); gr6.push_back(16); gr6.push_back(17);
    
    gr7.push_back(0); gr7.push_back(7); gr7.push_back(1); gr7.push_back(8);  gr7.push_back(2); gr7.push_back(9);
    gr7.push_back(11); gr7.push_back(12); gr7.push_back(13); gr7.push_back(14); gr7.push_back(15); gr7.push_back(16); gr7.push_back(17);
    
    gr8.push_back(0); gr8.push_back(7); gr8.push_back(1); gr8.push_back(8);  gr8.push_back(2); gr8.push_back(9);
    gr8.push_back(13); gr8.push_back(14); gr8.push_back(15); gr8.push_back(16); gr8.push_back(17); gr8.push_back(18);
    
    gr9.push_back(0); gr9.push_back(9);
    gr9.push_back(15); gr9.push_back(16); gr9.push_back(17); gr9.push_back(18);

  } else {

    /* Group definitions
       gr1:         eta<1        0  1  2   3   4   5   6   7
       gr2:      1<=eta<1.5      0  1  2   3   4   5   6   7   11  12  13  14
       gr3:    1.5<=eta<1.7      0  1  2   3   4   5   6   8   11  12  13  14  15
       gr4:    1.7<=eta<1.8      0  1  2   3   4   5   8   11  12  13  14  15  16
       gr5:    1.8<=eta<1.9      0  1  2   3   4   8   9   12  13  14  15  16  17
       gr6:    1.9<=eta<2.1      0  1  2   3   4   8   9   10  12  13  14  15  16  17  18
       gr7:    2.1<=eta<2.2      0  1  2   3   8   9   10  12  13  14  15  16  17  18
       gr8:    2.2<=eta<2.4      0  1  2   3   8   9   10  14  15  16  17  18  19
       gr9:         eta>=2.4     0  1  10  16  17  18  19
    */
    
    gr1.push_back(0); gr1.push_back(1); gr1.push_back(2); gr1.push_back(3); gr1.push_back(4); gr1.push_back(5); gr1.push_back(6); gr1.push_back(7);
    
    gr2.push_back(0); gr2.push_back(1); gr2.push_back(2); gr2.push_back(3); gr2.push_back(4); gr2.push_back(5); gr2.push_back(6); gr2.push_back(7);
    gr2.push_back(11); gr2.push_back(12); gr2.push_back(13); gr2.push_back(14);
    
    gr3.push_back(0); gr3.push_back(1); gr3.push_back(8); gr3.push_back(2); gr3.push_back(3);
    gr3.push_back(4); gr3.push_back(5); gr3.push_back(6); gr3.push_back(11); gr3.push_back(12); gr3.push_back(13); gr3.push_back(14); gr3.push_back(15);
    
    gr4.push_back(0); gr4.push_back(1); gr4.push_back(8); gr4.push_back(2); gr4.push_back(3);
    gr4.push_back(4); gr4.push_back(5); gr4.push_back(11); gr4.push_back(12); gr4.push_back(13); gr4.push_back(14); gr4.push_back(15); gr4.push_back(16);
    
    gr5.push_back(0); gr5.push_back(1); gr5.push_back(8); gr5.push_back(2); gr5.push_back(9); gr5.push_back(3);
    gr5.push_back(4); gr5.push_back(12); gr5.push_back(13); gr5.push_back(14); gr5.push_back(15); gr5.push_back(16); gr5.push_back(17);
    
    gr6.push_back(0); gr6.push_back(1); gr6.push_back(8); gr6.push_back(2); gr6.push_back(9);  gr6.push_back(3); gr6.push_back(10);
    gr6.push_back(4); gr6.push_back(12); gr6.push_back(13); gr6.push_back(14); gr6.push_back(15); gr6.push_back(16); gr6.push_back(17); gr6.push_back(18);
    
    gr7.push_back(0); gr7.push_back(1); gr7.push_back(8); gr7.push_back(2); gr7.push_back(9);  gr7.push_back(3); gr7.push_back(10);
    gr7.push_back(12); gr7.push_back(13); gr7.push_back(14); gr7.push_back(15); gr7.push_back(16); gr7.push_back(17); gr7.push_back(18);
    
    gr8.push_back(0); gr8.push_back(1); gr8.push_back(8); gr8.push_back(2); gr8.push_back(9);  gr8.push_back(3); gr8.push_back(10);
    gr8.push_back(14); gr8.push_back(15); gr8.push_back(16); gr8.push_back(17); gr8.push_back(18); gr8.push_back(19);
    
    gr9.push_back(0); gr9.push_back(1); gr9.push_back(10);
    gr9.push_back(16); gr9.push_back(17); gr9.push_back(18); gr9.push_back(19);

  }


  /** Vector of groups (from 0 -> 8). */
  m_layerGroups.push_back(gr1);
  m_layerGroups.push_back(gr2);
  m_layerGroups.push_back(gr3);
  m_layerGroups.push_back(gr4);
  m_layerGroups.push_back(gr5);
  m_layerGroups.push_back(gr6);
  m_layerGroups.push_back(gr7);
  m_layerGroups.push_back(gr8);
  m_layerGroups.push_back(gr9);
 
  /* Save content of Failure/Remove lists from vector to set */
  std::vector<int>::iterator it;
  for(it=m_mapForcedInefficientLayers.begin();it!=m_mapForcedInefficientLayers.end();it++){
    m_mapSetLayersFailure.insert(*it);
  }
  for(it=m_mapForcedTurnedOffLayers.begin();it!=m_mapForcedTurnedOffLayers.end();it++){
    m_mapSetLayersRemove.insert(*it);
  }
 
  /* Initialize sp triplet container array to zero */
  m_spTriplet=0;

 
  /************************************/
  /** SETUP PT-DEPENDENT CUTS *********/
  /************************************/

  m_dynamicalSeedingPreCutRZ         = m_seedingPreCutRZ;
  m_dynamicalSeedingPreCutPhi        = m_seedingPreCutPhi; 
  m_dynamicalSeedingPreCutRPhi       = m_seedingPreCutRPhi;
  m_dynamicalSeedingCutRZ            = m_seedingCutRZ;     
  m_dynamicalExtensionPreCutRZ       = m_extensionPreCutRZ;         
  m_dynamicalExtensionPreCutPhi      = m_extensionPreCutPhi;     
  m_dynamicalExtensionPreCutRPhi     = m_extensionPreCutRPhi;   
  m_dynamicalExtensionPreCutRPhiDiff = m_extensionPreCutRPhiDiff;
  m_dynamicalExtensionCutRZPixel     = m_extensionCutRZPixel;   
  m_dynamicalExtensionCutRZ          = m_extensionCutRZ;             
  m_dynamicalExtensionCutRPhi        = m_extensionCutRPhi;       

  double extensionPtMin = m_recoPtMin + m_recoPtMinTripletDiff;

  if(extensionPtMin<2000.0 && extensionPtMin>0.1) {
    m_dynamicalExtensionPreCutRPhi     = m_extensionPreCutRPhi     * (2000.0/extensionPtMin);
    m_dynamicalExtensionPreCutRPhiDiff = m_extensionPreCutRPhiDiff * (2000.0/extensionPtMin);
    m_dynamicalExtensionCutRPhi        = m_extensionCutRPhi        * (2000.0/extensionPtMin);
  }

  if(extensionPtMin<1000.0 && extensionPtMin>0.1) {
    m_dynamicalExtensionCutRZ          = m_extensionCutRZ          * (1000.0/extensionPtMin);
  }


  /************************************/
  /** SETUP TRACK DATA ****************/
  /************************************/

  /** Create pre-dimensioned arrays for data structures. */
  m_trks_num = 0;
  if(m_trks==0) m_trks = new StrBTrk[m_recoMaxSeeds];

  m_full_trks_num = 0;
  if(m_full_trks==0) m_full_trks = new StrBTrack[m_recoMaxTracks];


  /************************************/
  /** SETUP VERTEXING *****************/
  /************************************/

  /** Create fine vertexing histogram. */
  if(m_vertexingEnable) {
    m_vtxMaxBin = (int)ceil(2*m_vertexingCutRZ/(m_vertexingWindowSize/m_vertexingWindowSegments));
    m_vtxHisto = new int[m_vtxMaxBin];
    if(!m_vtxHisto) {
      athenaLog << MSG::FATAL << "could not create Z vertexing histogram" << endreq;
      return StatusCode::FAILURE;
    }
  }


  /************************************/
  /** SETUP TUNING ********************/
  /************************************/

  /** Relax cuts. **/
  if(m_debugTuning) {
    m_dynamicalSeedingPreCutRZ          = 10000;
    m_dynamicalSeedingPreCutPhi        *= 3;
    m_dynamicalSeedingPreCutRPhi        = 10000;
    m_dynamicalSeedingCutRZ             = 10000;
    m_dynamicalExtensionPreCutRZ        = 10000;
    m_dynamicalExtensionPreCutPhi      *= 3;
    m_dynamicalExtensionPreCutRPhi      = 10000;
    m_dynamicalExtensionPreCutRPhiDiff  = 10000;
    m_dynamicalExtensionCutRZPixel      = 10000;
    m_dynamicalExtensionCutRZ           = 10000;
    m_dynamicalExtensionCutRPhi         = 10000;
  }

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    athenaLog << MSG::INFO << "Unable to locate Service TrigTimerSvc" << endreq;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("StrB_PattReco");
  }


  /************************************/
  /** PROPERTY REVIEW *****************/
  /************************************/

  if (outputLevel <= MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "declareProperty review:" << endreq;
    
    /** Layer properties. */
    athenaLog << MSG::DEBUG << "Map_LayerOneMultiplicity = "        << m_mapLayerOneMultiplicity        << endreq;
    athenaLog << MSG::DEBUG << "Map_LayerTwoMultiplicity = "        << m_mapLayerTwoMultiplicity        << endreq;
    athenaLog << MSG::DEBUG << "Map_ForcedInefficientLayers = "     << m_mapForcedInefficientLayers     << endreq;
    athenaLog << MSG::DEBUG << "Map_ForcedTurnedOffLayers = "       << m_mapForcedTurnedOffLayers       << endreq;
    athenaLog << MSG::DEBUG << "Map_MaxLayerFailureFraction = "     << m_mapMaxLayerFailureFraction     << endreq;
    athenaLog << MSG::DEBUG << "Map_MaxTotalFailureFraction = "     << m_mapMaxTotalFailureFraction     << endreq;
    
    /** New cut properties*/
    athenaLog << MSG::DEBUG << "Reco_NewCuts = "                    << m_newCuts                        << endreq;
    athenaLog << MSG::DEBUG << "Reco_NewCutsRZ = "                  << m_newCutsRZ                      << endreq;
    athenaLog << MSG::DEBUG << "Reco_PtMinSeedDiff = "              << m_recoPtMinSeedDiff              << endreq;
    athenaLog << MSG::DEBUG << "Reco_D0MaxSeedDiff = "              << m_recoD0MaxSeedDiff              << endreq;
    athenaLog << MSG::DEBUG << "Reco_PtMinTripletDiff = "           << m_recoPtMinTripletDiff           << endreq;
    athenaLog << MSG::DEBUG << "Reco_D0MaxTripletDiff = "           << m_recoD0MaxTripletDiff           << endreq;
    athenaLog << MSG::DEBUG << "Reco_PtMin = "                      << m_recoPtMin                      << endreq;
    athenaLog << MSG::DEBUG << "Reco_D0Max = "                      << m_recoD0Max                      << endreq;
    athenaLog << MSG::DEBUG << "Reco_TimingFactor = "               << m_recoTimingFactor               << endreq;
    athenaLog << MSG::DEBUG << "Reco_AlignmentScaling = "           << m_recoAlignmentScaling           << endreq;
    athenaLog << MSG::DEBUG << "Reco_OutlierRemoval = "             << m_recoOutlierRemoval             << endreq;
    athenaLog << MSG::DEBUG << "Reco_OutlierRemovalCut = "          << m_recoOutlierRemovalCut          << endreq;
    athenaLog << MSG::DEBUG << "Reco_OutlierRemovalLayers = "       << m_recoOutlierRemovalLayers       << endreq;
    athenaLog << MSG::DEBUG << "Reco_OutlierRemovalD0 = "           << m_recoOutlierRemovalD0           << endreq;
    athenaLog << MSG::DEBUG << "Reco_OutlierRemovalPt = "           << m_recoOutlierRemovalPt           << endreq;
    athenaLog << MSG::DEBUG << "Reco_MaxSeeds = "                   << m_recoMaxSeeds                   << endreq;
    athenaLog << MSG::DEBUG << "Reco_MaxTracks = "                  << m_recoMaxTracks                  << endreq;
    athenaLog << MSG::DEBUG << "Reco_Z0Bin = "                      << m_recoZ0Bin                      << endreq;
    athenaLog << MSG::DEBUG << "Reco_EtaBin = "                     << m_recoEtaBin                     << endreq;
    
    /** Preliminary vertexing properties. */				   
    athenaLog << MSG::DEBUG << "PreVertexing_Enable = "             << m_preVertexingEnable             << endreq;
    athenaLog << MSG::DEBUG << "PreVertexing_VtxName = "            << m_preVertexingVtxName            << endreq;
    athenaLog << MSG::DEBUG << "PreVertexing_ZCut = "               << m_preVertexingZCut               << endreq;
    
    /** Seeding properties. */
    athenaLog << MSG::DEBUG << "Seeding_PreCutRZ = "                << m_seedingPreCutRZ                << endreq;
    athenaLog << MSG::DEBUG << "Seeding_PreCutPhi = "               << m_seedingPreCutPhi               << endreq;
    athenaLog << MSG::DEBUG << "Seeding_PreCutRPhi = "              << m_seedingPreCutRPhi              << endreq;
    athenaLog << MSG::DEBUG << "Seeding_CutRZ = "                   << m_seedingCutRZ                   << endreq;
    
    /** Vertexing properties. */
    athenaLog << MSG::DEBUG << "Vertexing_Enable = "                << m_vertexingEnable                << endreq;
    athenaLog << MSG::DEBUG << "Vertexing_CutRZ = "                 << m_vertexingCutRZ                 << endreq;
    athenaLog << MSG::DEBUG << "Vertexing_CutRPhi = "               << m_vertexingCutRPhi               << endreq;
    athenaLog << MSG::DEBUG << "Vertexing_MaxVertices = "           << m_vertexingMaxVertices           << endreq;
    athenaLog << MSG::DEBUG << "Vertexing_WindowSize = "            << m_vertexingWindowSize            << endreq;
    athenaLog << MSG::DEBUG << "Vertexing_WindowSegments = "        << m_vertexingWindowSegments        << endreq;
    
    /** Extension properties. */
    athenaLog << MSG::DEBUG << "Extension_PreCutRZ = "              << m_extensionPreCutRZ              << endreq;
    athenaLog << MSG::DEBUG << "Extension_PreCutPhi = "             << m_extensionPreCutPhi             << endreq;
    athenaLog << MSG::DEBUG << "Extension_PreCutRPhi = "            << m_extensionPreCutRPhi            << endreq;
    athenaLog << MSG::DEBUG << "Extension_PreCutRPhiDiff = "        << m_extensionPreCutRPhiDiff        << endreq;
    athenaLog << MSG::DEBUG << "Extension_CutRZPixel = "            << m_extensionCutRZPixel            << endreq;
    athenaLog << MSG::DEBUG << "Extension_CutRZ = "                 << m_extensionCutRZ                 << endreq;
    athenaLog << MSG::DEBUG << "Extension_CutRPhi = "               << m_extensionCutRPhi               << endreq;
    athenaLog << MSG::DEBUG << "Extension_CutSpacePoints = "        << m_extensionCutSpacePoints        << endreq;
    athenaLog << MSG::DEBUG << "Extension_CutSpacePointsMiss1 = "   << m_extensionCutSpacePointsMiss1   << endreq;
    athenaLog << MSG::DEBUG << "Extension_CutSpacePointsMiss2 = "   << m_extensionCutSpacePointsMiss2   << endreq;
    athenaLog << MSG::DEBUG << "Extension_SolveTriplets = "         << m_extensionSolveTriplets         << endreq;
    athenaLog << MSG::DEBUG << "Extension_SolveTripletsDRCut = "    << m_extensionSolveTripletsDRCut    << endreq;
    athenaLog << MSG::DEBUG << "Extension_SolveTripletsPhiScale = " << m_extensionSolveTripletsPhiScale << endreq;
    athenaLog << MSG::DEBUG << "Extension_SolveTripletsEtaScale = " << m_extensionSolveTripletsEtaScale << endreq;
    athenaLog << MSG::DEBUG << "Extension_SolveTracks = "           << m_extensionSolveTracks           << endreq;
    athenaLog << MSG::DEBUG << "Extension_SolveV0Tracks = "         << m_extensionSolveV0Tracks         << endreq;
    athenaLog << MSG::DEBUG << "Extension_SolveTracksDRCut = "      << m_extensionSolveTracksDRCut      << endreq;
    athenaLog << MSG::DEBUG << "Extension_SolveTracksPhiScale = "   << m_extensionSolveTracksPhiScale   << endreq;
    athenaLog << MSG::DEBUG << "Extension_SolveTracksEtaScale = "   << m_extensionSolveTracksEtaScale   << endreq;
    athenaLog << MSG::DEBUG << "Extension_SolveShared = "           << m_extensionSolveShared           << endreq;
    athenaLog << MSG::DEBUG << "Extension_MaxMissingLayers = "      << m_extensionMaxMissingLayers      << endreq;
    athenaLog << MSG::DEBUG << "Extension_Refit = "                 << m_extensionRefit                 << endreq;
    athenaLog << MSG::DEBUG << "Extension_SpacePoints = "           << m_extensionSpacePoints           << endreq;
    athenaLog << MSG::DEBUG << "Extension_SpacePointsMiss1 = "      << m_extensionSpacePointsMiss1      << endreq;
    athenaLog << MSG::DEBUG << "Extension_SpacePointsMiss2 = "      << m_extensionSpacePointsMiss2      << endreq;
    athenaLog << MSG::DEBUG << "Extension_TRTExtension = "          << m_extensionTRTExtension          << endreq;
    
    /** Debug properties. */
    athenaLog << MSG::DEBUG << "Debug_Tuning = "                    << m_debugTuning                    << endreq;
    athenaLog << MSG::DEBUG << "Debug_TuningGoodSeeds = "           << m_debugTuningGoodSeeds           << endreq;
    athenaLog << MSG::DEBUG << "Debug_TuningBadSeeds = "            << m_debugTuningBadSeeds            << endreq;
    athenaLog << MSG::DEBUG << "Debug_TuningGoodTriplets = "        << m_debugTuningGoodTriplets        << endreq;
    athenaLog << MSG::DEBUG << "Debug_TuningBadTriplets = "         << m_debugTuningBadTriplets         << endreq;
  }


  /************************************/
  /** FINAL MESSAGE *******************/
  /************************************/

  /** Initialization report message. */
  athenaLog << MSG::INFO << "TrigL2PattRecoStrategyB initialized" << endreq;

  return sc;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode TrigL2PattRecoStrategyB::finalize() {

  /** Delete pre-dimensioned arrays for track data. */
  if(m_trks!=0) {delete[] m_trks; m_trks=0;}
  if(m_full_trks!=0) {delete[] m_full_trks; m_full_trks=0;}
  m_trks_num = 0;
  m_full_trks_num = 0;

  /** Delete and clear processing structures. */
  if(m_vertexingEnable) delete[] m_vtxHisto;
  
  /** Delete geometry vectors. */
  delete [] m_geoR; delete [] m_geoRmin; delete [] m_geoRmax;
  delete [] m_geoZ; delete [] m_geoZmin; delete [] m_geoZmax;

  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigL2PattRecoStrategyB::~TrigL2PattRecoStrategyB() {

}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigL2PattRecoStrategyB::findTracks(const std::vector<const TrigSiSpacePoint*>& vsp, const IRoiDescriptor& roi, TrigInDetTrackCollection*  trackColl) {

  /** Cleanup and setup. */
  executeCleanup();

  HLT::ErrorCode rc=HLT::OK;

  m_zVertices.clear();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::DEBUG) 
    athenaLog << MSG::DEBUG << "TrigL2PattRecoStrategyB called in RoI-based mode" << endreq;

  m_roiEta = roi.eta(); 
  m_roiPhi = roi.phi(); 

  m_etaHalfWidth = 0.5*fabs(roi.etaPlus()-roi.etaMinus());
  m_phiHalfWidth = 0.5*fabs(roi.phiPlus()-roi.phiMinus());

  roiPhiMin = roi.phiMinus();  
  roiPhiMax = roi.phiPlus();
  roiEtaMin = roi.etaMinus();
  roiEtaMax = roi.etaPlus();

  executeSetup();

  if(m_timers) m_timer[0]->start();


  /************************************************************************************************************/
  /************************************************************************************************************/
  /** SEEDING BLOCK *******************************************************************************************/
  /************************************************************************************************************/
  /************************************************************************************************************/


  /************************************/
  /** EXECUTE SEED ROADS **************/
  /************************************/

  /** Perform seed road making. */
  executeSeedRoads(roiEtaMin, roiEtaMax, roiPhiMin, roiPhiMax);

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "SEED ROAD MAKING: found " << m_roads.size() << " roads" << endreq;
    StrBRoadIterator road, roadEnd=m_roads.end();
    for(road=m_roads.begin(); road!=roadEnd; road++) {
      athenaLog << MSG::DEBUG << "SEED ROAD MAKING: road " << (*road)->lay1 << " --> " << (*road)->lay2 << " --> ";
      std::set<int>::iterator it, itEnd=(*road)->lay3.end();
      for(it=(*road)->lay3.begin(); it!=itEnd; it++) {
	athenaLog << MSG::DEBUG << (*it) << " ";
      }
      athenaLog << MSG::DEBUG << endreq;
    }
  }

  /************************************/
  /** EXECUTE SEED DATA SORTING *******/
  /************************************/
  
  /** Perform sorting. */
  executeSeedDataSorting(vsp);
  m_nSP = vsp.size();

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    for(int i=0; i<m_layMax; i++){
      athenaLog << MSG::DEBUG << "SEED DATA SORTING: found " << m_spSeed[i].size() << " space points on layer " << i << endreq;
    }
  }

  /************************************/
  /** EXECUTE SEED MAKING *************/
  /************************************/

  /** Perform track seeding. */
  executeSeedMaking();

   /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "SEED MAKING: found " << m_trks_num_active << " seeds" << endreq;
  }
  
  
  /************************************/
  /** EXECUTE SEED VERTEXING **********/
  /************************************/

  /** Perform seed vertexing. */
  if(m_vertexingEnable) {
    executeSeedFilter_Vertex();
  }

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    if(m_vertexingEnable) {
      athenaLog << MSG::DEBUG << "SEED FILTER VERTEX: found " << m_recZv.size() << " vertices" << endreq;
    }
  }


  /************************************************************************************************************/
  /************************************************************************************************************/
  /** TRIPLET BLOCK *******************************************************************************************/
  /************************************************************************************************************/
  /************************************************************************************************************/
  
    
  /************************************/
  /** EXECUTE TRIPLET DATA SORTING ****/
  /************************************/
  
  /** Perform sorting. */
  executeTripletDataSorting(vsp);

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    for(int i=0; i<m_layMax; i++) {
      athenaLog << MSG::DEBUG << "TRIPLET DATA SORTING: found ";
      for(unsigned int loopz0=0; loopz0<m_spTripletZ0Steps; loopz0++) {
        for(unsigned int loopeta=0; loopeta<m_spTripletEtaSteps; loopeta++)
          athenaLog << MSG::DEBUG << m_spTriplet[i*m_spTripletZ0Steps*m_spTripletEtaSteps+loopz0*m_spTripletEtaSteps+loopeta].size() << " ";
        athenaLog << MSG::DEBUG << endreq << "                            ";
      }
      athenaLog << " space points on layer " << i << endreq;
    }
  }


  /************************************/
  /** EXECUTE TRIPLET MAKING **********/
  /************************************/

  /** Perform triplet making. */
  executeTripletMaking();

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "TRIPLET MAKING: found " << m_trks_num_active << " patterns" << endreq;
  }


  /************************************/
  /** EXECUTE TRIPLET AMBIGUITY *******/
  /************************************/

  /** Perform triplet ambiguity. */
  executeTripletAmbiguity();

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "TRIPLET AMBIGUITY: retained " << m_full_trks_num << " patterns" << endreq;
  }
  

  /************************************************************************************************************/
  /************************************************************************************************************/
  /** TRACK BLOCK *********************************************************************************************/
  /************************************************************************************************************/
  /************************************************************************************************************/
  

  /************************************/
  /** EXECUTE TRACK AMBIGUITY *********/
  /************************************/

  /** Perform track ambiguity. */
  executeTrackAmbiguity();

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "TRACK AMBIGUITY: retained " << m_full_trks_num_active << " patterns" << endreq;
  }


  /************************************/
  /** EXECUTE TRACK MAKING ************/
  /************************************/

  /** Perform track ambiguity. */
  executeTrackMaking(trackColl);

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "TRACK MAKING: created " << trackColl->size() << " tracks" << endreq;
  }

  /** Perform cleanup. */
  executeCleanup();

  if(m_timers) {
    m_timer[0]->stop();
    m_pattRecoTiming=m_timer[0]->elapsed();
  }

  return rc;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigL2PattRecoStrategyB::findTracks(const std::vector<const TrigSiSpacePoint*>& vsp, const IRoiDescriptor& roi, const TrigVertexCollection* vtx, TrigInDetTrackCollection*  trackColl) {

  /** Cleanup. */
  executeCleanup();

  HLT::ErrorCode rc=HLT::OK;

  m_zVertices.clear();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::DEBUG) 
    athenaLog << MSG::DEBUG << "TrigL2PattRecoStrategyB called in RoI-based mode" << endreq;

  m_roiEta = roi.eta(); 
  m_roiPhi = roi.phi(); 

  m_etaHalfWidth = 0.5*fabs(roi.etaPlus()-roi.etaMinus());
  m_phiHalfWidth = 0.5*fabs(roi.phiPlus()-roi.phiMinus());

  roiPhiMin = roi.phiMinus();  
  roiPhiMax = roi.phiPlus();
  roiEtaMin = roi.etaMinus();
  roiEtaMax = roi.etaPlus();

  executeSetup();

  if(m_timers) m_timer[0]->start();


  /************************************************************************************************************/
  /************************************************************************************************************/
  /** SEEDING BLOCK *******************************************************************************************/
  /************************************************************************************************************/
  /************************************************************************************************************/


  /************************************/
  /** EXECUTE SEED ROADS **************/
  /************************************/

  /** Perform seed road making. */
  executeSeedRoads(roiEtaMin, roiEtaMax, roiPhiMin, roiPhiMax);

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "SEED ROAD MAKING: found " << m_roads.size() << " roads" << endreq;
    StrBRoadIterator road, roadEnd=m_roads.end();
    for(road=m_roads.begin(); road!=roadEnd; road++) {
      athenaLog << MSG::DEBUG << "SEED ROAD MAKING: road " << (*road)->lay1 << " --> " << (*road)->lay2 << " --> ";
      std::set<int>::iterator it, itEnd=(*road)->lay3.end();
      for(it=(*road)->lay3.begin(); it!=itEnd; it++) {
	athenaLog << MSG::DEBUG << (*it) << " ";
      }
      athenaLog << MSG::DEBUG << endreq;
    }
  }

  /************************************/
  /** EXECUTE SEED DATA SORTING *******/
  /************************************/
  
  /** Perform sorting. */
  executeSeedDataSorting(vsp);
  m_nSP = vsp.size();

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    for(int i=0; i<m_layMax; i++){
      athenaLog << MSG::DEBUG << "SEED DATA SORTING: found " << m_spSeed[i].size() << " space points on layer " << i << endreq;
    }
  }

  /************************************/
  /** EXECUTE SEED MAKING *************/
  /************************************/

  /** Perform track seeding. */
  if(vtx==NULL)
    executeSeedMaking();
  else {
    std::vector<float> vtxpos;
    TrigVertexCollection::const_iterator v, vEnd=vtx->end();
    for(v=vtx->begin(); v!=vEnd; v++) vtxpos.push_back((*v)->z());
    executeSeedMakingVertex(vtxpos);
  }

   /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "SEED MAKING: found " << m_trks_num_active << " seeds" << endreq;
  }
  
  
  /************************************/
  /** EXECUTE SEED VERTEXING **********/
  /************************************/

  /** Perform seed vertexing. */
  if(m_vertexingEnable) {
    executeSeedFilter_Vertex();
  }

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    if(m_vertexingEnable) {
      athenaLog << MSG::DEBUG << "SEED FILTER VERTEX: found " << m_recZv.size() << " vertices" << endreq;
    }
  }


  /************************************************************************************************************/
  /************************************************************************************************************/
  /** TRIPLET BLOCK *******************************************************************************************/
  /************************************************************************************************************/
  /************************************************************************************************************/
  
    
  /************************************/
  /** EXECUTE TRIPLET DATA SORTING ****/
  /************************************/
  
  /** Perform sorting. */
  executeTripletDataSorting(vsp);

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    for(int i=0; i<m_layMax; i++) {
      athenaLog << MSG::DEBUG << "TRIPLET DATA SORTING: found ";
      for(unsigned int loopz0=0; loopz0<m_spTripletZ0Steps; loopz0++) {
        for(unsigned int loopeta=0; loopeta<m_spTripletEtaSteps; loopeta++)
          athenaLog << MSG::DEBUG << m_spTriplet[i*m_spTripletZ0Steps*m_spTripletEtaSteps+loopz0*m_spTripletEtaSteps+loopeta].size() << " ";
        athenaLog << MSG::DEBUG << endreq << "                            ";
      }
      athenaLog << " space points on layer " << i << endreq;
    }
  }


  /************************************/
  /** EXECUTE TRIPLET MAKING **********/
  /************************************/

  /** Perform triplet making. */
  executeTripletMaking();

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "TRIPLET MAKING: found " << m_trks_num_active << " patterns" << endreq;
  }


  /************************************/
  /** EXECUTE TRIPLET AMBIGUITY *******/
  /************************************/

  /** Perform triplet ambiguity. */
  executeTripletAmbiguity();

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "TRIPLET AMBIGUITY: retained " << m_full_trks_num << " patterns" << endreq;
  }
  

  /************************************************************************************************************/
  /************************************************************************************************************/
  /** TRACK BLOCK *********************************************************************************************/
  /************************************************************************************************************/
  /************************************************************************************************************/
  

  /************************************/
  /** EXECUTE TRACK AMBIGUITY *********/
  /************************************/

  /** Perform track ambiguity. */
  executeTrackAmbiguity();

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "TRACK AMBIGUITY: retained " << m_full_trks_num_active << " patterns" << endreq;
  }


  /************************************/
  /** EXECUTE TRACK MAKING ************/
  /************************************/

  /** Perform track ambiguity. */
  executeTrackMaking(trackColl);

  /** Debug info. */
  if(outputLevel<=MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "TRACK MAKING: created " << trackColl->size() << " tracks" << endreq;
  }

  /** Perform cleanup. */
  executeCleanup();

  if(m_timers) {
    m_timer[0]->stop();
    m_pattRecoTiming=m_timer[0]->elapsed();
  }

  return rc;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigL2PattRecoStrategyB::findTracks(const std::vector<const TrigSiSpacePoint*>& vsp, const TrigVertexCollection* vtx, TrigInDetTrackCollection* trackColl) {
  findTracks(vsp,TrigRoiDescriptor(true),vtx,trackColl);
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigL2PattRecoStrategyB::findTracks(const std::vector<const TrigSiSpacePoint*>& vsp, TrigInDetTrackCollection* trackColl) {
  findTracks(vsp,TrigRoiDescriptor(true),trackColl);
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::executeSeedRoads(float etaMin, float etaMax, float /*phiMin*/, float /*phiMax*/) {


  /*************************************/
  /** CREATE ROADS *********************/
  /*************************************/
 
  int bcount1, bcount2;
  int ecount1, ecount2;
 
  std::vector<int> bad,good;
  std::vector<float> perc;
 
  for(int i=0;i<m_layMax;i++){
    bad.push_back(0);
    good.push_back(0);
    perc.push_back(0.0);
  }
 
  // NEWCODE
  int nMissingBarrel=0, nMissingEndcap=0;
  for(int i=0; i<m_layMax; i++) {
    if(m_mapSetLayersRemove.find(i)!=m_mapSetLayersRemove.end()) { 
      perc[i]=1.0;
    }
    if(perc[i]==1.0) {
      if(i<=6) nMissingBarrel++;
      else nMissingEndcap++;
    }
  }
  m_dynamic_extensionCutSpacePoints = m_extensionCutSpacePoints;
  m_dynamic_extensionSpacePoints    = m_extensionSpacePoints;
  if(nMissingBarrel>0||nMissingEndcap>0) {
    if(m_extensionCutSpacePointsMiss1 < m_dynamic_extensionCutSpacePoints) m_dynamic_extensionCutSpacePoints = m_extensionCutSpacePointsMiss1;
    if(m_extensionSpacePointsMiss1    < m_dynamic_extensionSpacePoints)    m_dynamic_extensionSpacePoints    = m_extensionSpacePointsMiss1;
  }
  if(nMissingBarrel>1||nMissingEndcap>1) {
    if(m_extensionCutSpacePointsMiss2 < m_dynamic_extensionCutSpacePoints) m_dynamic_extensionCutSpacePoints = m_extensionCutSpacePointsMiss2;
    if(m_extensionSpacePointsMiss2    < m_dynamic_extensionSpacePoints)    m_dynamic_extensionSpacePoints    = m_extensionSpacePointsMiss2;
  }
  // NEWCODE  

 
  /** Layers to be used in barrel and endcap. */
  float absetaMin=0, absetaMax=2.5;
  if(etaMin>0)            {absetaMin = etaMin;  absetaMax = etaMax;}
  if(etaMin<=0&&etaMax>0) {absetaMin = 0;       absetaMax = (etaMax>-etaMin) ? etaMax : -etaMin;}
  if(etaMax<=0)           {absetaMin = -etaMax; absetaMax = -etaMin;}
 
  int imin=1,imax=9;
  if (absetaMin<1.)                     imin=1;
  if (absetaMin>=1.  && absetaMin<1.5)  imin=2;
  if (absetaMin>=1.5 && absetaMin<1.7)  imin=3;
  if (absetaMin>=1.7 && absetaMin<1.8)  imin=4;
  if (absetaMin>=1.8 && absetaMin<1.9)  imin=5;
  if (absetaMin>=1.9 && absetaMin<2.1)  imin=6;
  if (absetaMin>=2.1 && absetaMin<2.2)  imin=7;
  if (absetaMin>=2.2 && absetaMin<2.4)  imin=8;
  if (absetaMin>=2.4)                   imin=9;
  if (absetaMax<1.)                     imax=1;
  if (absetaMax>=1.  && absetaMax<1.5)  imax=2;
  if (absetaMax>=1.5 && absetaMax<1.7)  imax=3;
  if (absetaMax>=1.7 && absetaMax<1.8)  imax=4;
  if (absetaMax>=1.8 && absetaMax<1.9)  imax=5;
  if (absetaMax>=1.9 && absetaMax<2.1)  imax=6;
  if (absetaMax>=2.1 && absetaMax<2.2)  imax=7;
  if (absetaMax>=2.2 && absetaMax<2.4)  imax=8;
  if (absetaMax>=2.4)                   imax=9;
 
  for(int i=imin-1; i<imax;i++) {
   
    // Layers     
    std::list<int> l = m_layerGroups[i];
    float perctot1,perctot2;

    /** Start from first layer. */
    std::list<int>::iterator lay1, lay2, lay3;
    for(bcount1=0, ecount1=0, perctot1=1.0, lay1=l.begin(); (lay1!=l.end())&&(bcount1<m_mapLayerOneMultiplicity&&ecount1<m_mapLayerOneMultiplicity); lay1++) {
      if(perc[*lay1]==1.0) continue;
      if(perc[*lay1]<m_mapMaxLayerFailureFraction) {if(*lay1<m_layPixEC) bcount1++; else ecount1++;}
      else {
        perctot1 *= perc[*lay1];
        if(perctot1 < m_mapMaxTotalFailureFraction) {if(*lay1<m_layPixEC) bcount1++; else ecount1++;};
      }         

      // Second layer
      for(bcount2=0, ecount2=0, perctot2=1.0,lay2=lay1,lay2++; (lay2!=l.end())&&(bcount2<m_mapLayerTwoMultiplicity&&ecount2<m_mapLayerTwoMultiplicity); lay2++) {
        if(*lay2<*lay1) continue;
        if(perc[*lay2]==1.0) continue;
        if(perc[*lay2]<m_mapMaxLayerFailureFraction) {if(*lay2<m_layPixEC) bcount2++; else ecount2++;}
        else {
          perctot2 *= perc[*lay2];
          if(perctot2 < m_mapMaxTotalFailureFraction) {if(*lay2<m_layPixEC) bcount2++; else ecount2++;};
        }
   
        StrBRoad* road = new StrBRoad;
        road->lay1 = *lay1;
        road->lay2 = *lay2;
       
        // Third layer
        for(lay3=lay2, lay3++; lay3!=l.end(); lay3++) {
          if(*lay3<*lay2) continue;
          if(perc[*lay3]==1.0) continue;
         
          road->lay3.insert(*lay3);
        }

        m_roads.push_back(road);
        StrBRoadIterator r, rEnd=m_roads.end();
        std::set<int>::iterator it, itEnd=road->lay3.end();
        for(r=m_roads.begin(); r!=rEnd-1; r++){
          if((*r)->lay1==*lay1 && (*r)->lay2==*lay2){
            for(it=road->lay3.begin(); it!=itEnd; it++){
              (*r)->lay3.insert(*it);
            }
            delete (m_roads.back());
            m_roads.pop_back();
          }
        }
      }
    }
  }

  // Sort roads
  std::multimap<int,StrBRoad*> sortRoads;
  StrBRoadIterator r, rEnd=m_roads.end();
  for(r=m_roads.begin(); r!=rEnd; r++) {

    int layerSum=0;
   
    if((*r)->lay1<m_layPixEC) layerSum+=(*r)->lay1;
    else layerSum+=((*r)->lay1-m_layPixEC);
   
    if((*r)->lay2<m_layPixEC) layerSum+=(*r)->lay2;
    else if((*r)->lay1<m_layPixEC) layerSum+=((*r)->lay2-6);
    else layerSum+=(*r)->lay2;
   
    sortRoads.insert(std::make_pair(layerSum, (*r)));

  }

  m_roads.clear();

  std::map<int,StrBRoad*>::iterator sr, srEnd=sortRoads.end();
  for(sr=sortRoads.begin(); sr!=srEnd; sr++)
    m_roads.push_back(sr->second);

  sortRoads.clear();
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::executeSeedDataSorting(const std::vector<const TrigSiSpacePoint*>& spacePointData) {

  /** Loop on space points. */
  std::vector<const TrigSiSpacePoint*>::const_iterator sp, spEnd=spacePointData.end();
  for(sp=spacePointData.begin(); sp!=spEnd; sp++) {
    long layer = (*sp)->layer();
    m_spSeed[layer].push_back(*sp);
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigL2PattRecoStrategyB::executeSeedMaking() {

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  float seedMakingLimit=m_recoTimingFactor*2000.0*(sqrt(m_nSP/500.0))*m_mapLayerOneMultiplicity*m_mapLayerTwoMultiplicity;
  if(seedMakingLimit>m_recoMaxSeeds) seedMakingLimit=m_recoMaxSeeds;
  bool limit = false;
  int seedMakingCount = 0;
  m_trks_num = 0;

  /** Pointers to array positions. */
  StrBTrk* pSeed=m_trks;
  const TrigSiSpacePoint *s1, *s2;

  /** Perform combinations. */
  float phi1, r1, z1;
  float dphi;
  long  lay1, lay2;
  float cutd02 = (m_recoD0Max+m_recoD0MaxSeedDiff)*(m_recoD0Max+m_recoD0MaxSeedDiff);
  float ptMin = m_recoPtMin+m_recoPtMinSeedDiff; if(ptMin<1.0) ptMin=1.0;
  float cuteta = 0.0325 + 490.0/ptMin; if(cuteta<m_etaHalfWidth)cuteta=m_etaHalfWidth; if(cuteta>10)cuteta=10;
  float mintheta = 2*atan(exp(-(m_roiEta-cuteta)));
  float maxtheta = 2*atan(exp(-(m_roiEta+cuteta)));
  float mindzdr = cos(mintheta)/sin(mintheta);
  float maxdzdr = cos(maxtheta)/sin(maxtheta);
  
  StrBRoadIterator r, rEnd=m_roads.end();
  std::vector<const TrigSiSpacePoint*>::iterator sp1, sp1End, sp2, sp2End;
  
  // Loop on Roads
  for(r=m_roads.begin(); r!=rEnd && !limit; r++){
    
    lay1 = (*r)->lay1;
    lay2 = (*r)->lay2;
    
    if(m_spSeed[lay1].size()!=0 && m_spSeed[lay2].size()!=0) {

      // Loop on SP container of layer1
      sp1End=m_spSeed[lay1].end();
      for(sp1=m_spSeed[lay1].begin(); sp1!=sp1End && !limit; sp1++) {

	// Check if event timeout was reached
	if (Athena::Timeout::instance().reached()) {
	  if(outputLevel<=MSG::DEBUG)
	    athenaLog << MSG::DEBUG << "Timeout reached. Aborting sequence." << endreq;
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}

	// Get sp1 info
	s1 = (*sp1);
	phi1 = s1->phi();
	r1   = s1->r();
	z1   = s1->z();
	
	// Loop on layer2
	sp2End=m_spSeed[lay2].end();
	for(sp2=m_spSeed[lay2].begin(); sp2!=sp2End && !limit; sp2++) {
	  
	  // Get sp2 info
	  s2 = (*sp2);

          // Cut on d0
          //a = s2->y() - s1->y();
          //b = s1->x() - s2->x();
          //c = b*s1->y() + a*s1->x();
          //d02 = c*c;
          //cut = cutd02*(a*a+b*b);
          //if(d02>cut) continue;
          if(((s1->x()-s2->x())*s1->y()+(s2->y()-s1->y())*s1->x())*((s1->x()-s2->x())*s1->y()+(s2->y()-s1->y())*s1->x()) >
             cutd02*((s2->y()-s1->y())*(s2->y()-s1->y())+(s1->x()-s2->x())*(s1->x()-s2->x()))) continue;

          // Cut on phi/r
          //cut = m_dynamicalSeedingPreCutRPhi*dr;
          //dphi = s2->phi() - phi1;
          //if(dphi>M_PI) dphi=dphi-2*M_PI; else if(dphi<-M_PI) dphi=dphi+2*M_PI;
          //if(dphi>cut || dphi<-cut) continue;
          dphi = s2->phi()-phi1;
          if(dphi>M_PI) dphi=dphi-2*M_PI; else if(dphi<-M_PI) dphi=dphi+2*M_PI;
          if(dphi>m_dynamicalSeedingPreCutRPhi*(s2->r()-r1) || dphi<-m_dynamicalSeedingPreCutRPhi*(s2->r()-r1)) continue;
          
          // Cut on z
          //cut = m_vertexingCutRZ*dr;
          //z0  = z1*dr - dz*r1;
          //if(z0>cut || z0<-cut) continue;
          if((z1*(s2->r()-r1)-(s2->z()-z1)*r1)>m_vertexingCutRZ*(s2->r()-r1) || (z1*(s2->r()-r1)-(s2->z()-z1)*r1)<-m_vertexingCutRZ*(s2->r()-r1)) continue;

	  // Cut on eta
	  // dr = (s2->r()-r1);
	  // dz = (s2->z()-z1);
	  // if(dz<mindzr*dr || dz>maxdzr*dr) continue;
	  if((s2->z()-z1)<mindzdr*(s2->r()-r1) || (s2->z()-z1)>maxdzdr*(s2->r()-r1)) continue;

	  // Cut on dr
	  if(s2->r()<=r1) continue;

	  // Create and save StrBSeed
	  pSeed->deleted   = false;
	  pSeed->seed_sp1  = s1;
	  pSeed->seed_sp2  = s2;
	  pSeed->z0        = (z1*(s2->r()-r1)-(s2->z()-z1)*r1)/(s2->r()-r1);

	  // Increase pointers
	  pSeed++;

	  // Increase counters
	  m_trks_num++;

	  seedMakingCount++;
	  if(seedMakingCount>=seedMakingLimit) limit=true;

	}
      }
    }
  }

  m_trks_num_active = m_trks_num; 
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigL2PattRecoStrategyB::executeSeedMakingVertex(std::vector<float>& preZv) {

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  float seedMakingLimit=m_recoTimingFactor*2000.0*(sqrt(m_nSP/500.0))*m_mapLayerOneMultiplicity*m_mapLayerTwoMultiplicity;
  if(seedMakingLimit>m_recoMaxSeeds) seedMakingLimit=m_recoMaxSeeds;
  bool limit = false;
  int seedMakingCount = 0;
  m_trks_num = 0;

  /** Pointers to array positions. */
  StrBTrk* pSeed=m_trks;
  const TrigSiSpacePoint *s1, *s2;

  /** Vertex iterators. */
  std::vector<float>::iterator zv, zvEnd;
  bool matchZV;

  /** Perform combinations. */
  float phi1, r1, z1;
  float dphi;
  long  lay1, lay2;
  float cutd02 = (m_recoD0Max+m_recoD0MaxSeedDiff)*(m_recoD0Max+m_recoD0MaxSeedDiff);
  float ptMin = m_recoPtMin+m_recoPtMinSeedDiff; if(ptMin<1.0) ptMin=1.0;
  float cuteta = 0.0325 + 490.0/ptMin; if(cuteta<m_etaHalfWidth)cuteta=m_etaHalfWidth; if(cuteta>10)cuteta=10;
  float mintheta = 2*atan(exp(-(m_roiEta-cuteta)));
  float maxtheta = 2*atan(exp(-(m_roiEta+cuteta)));
  float mindzdr = cos(mintheta)/sin(mintheta);
  float maxdzdr = cos(maxtheta)/sin(maxtheta);
  
  StrBRoadIterator r, rEnd=m_roads.end();
  std::vector<const TrigSiSpacePoint*>::iterator sp1, sp1End, sp2, sp2End;
  
  // Loop on Roads
  for(r=m_roads.begin(); r!=rEnd && !limit; r++){
    
    lay1 = (*r)->lay1;
    lay2 = (*r)->lay2;
    
    if(m_spSeed[lay1].size()!=0 && m_spSeed[lay2].size()!=0) {

      // Loop on SP container of layer1
      sp1End=m_spSeed[lay1].end();
      for(sp1=m_spSeed[lay1].begin(); sp1!=sp1End && !limit; sp1++) {

	// Check if event timeout was reached
	if (Athena::Timeout::instance().reached()) {
	  if(outputLevel<=MSG::DEBUG)
	    athenaLog << MSG::DEBUG << "Timeout reached. Aborting sequence." << endreq;
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}

	// Get sp1 info
	s1 = (*sp1);
	phi1 = s1->phi();
	r1   = s1->r();
	z1   = s1->z();
	
	// Loop on layer2
	sp2End=m_spSeed[lay2].end();
	for(sp2=m_spSeed[lay2].begin(); sp2!=sp2End && !limit; sp2++) {
	  
	  // Get sp2 info
	  s2 = (*sp2);

	  // Cut on eta
	  // dr = (s2->r()-r1);
	  // dz = (s2->z()-z1);
	  // if(dz<mindzr*dr || dz>maxdzr*dr) continue;
	  if((s2->z()-z1)<mindzdr*(s2->r()-r1) || (s2->z()-z1)>maxdzdr*(s2->r()-r1)) continue;

	  // Loop on vtx
	  matchZV = false;
	  zvEnd=preZv.end();
	  for(zv=preZv.begin(); zv!=zvEnd; zv++) {
	    // Cut on zv
	    if((z1*(s2->r()-r1)-(s2->z()-z1)*r1)<((*zv)+m_preVertexingZCut)*(s2->r()-r1) && (z1*(s2->r()-r1)-(s2->z()-z1)*r1)>((*zv)-m_preVertexingZCut)*(s2->r()-r1)) {matchZV=true; break;}
	  }
	  if(!matchZV) continue;

	  // Cut on d0
	  //a = s2->y() - s1->y();
	  //b = s1->x() - s2->x();
	  //c = b*s1->y() + a*s1->x();
	  //d02 = c*c;
	  //cut = cutd02*(a*a+b*b);
	  //if(d02>cut) continue;
	  if(((s1->x()-s2->x())*s1->y()+(s2->y()-s1->y())*s1->x())*((s1->x()-s2->x())*s1->y()+(s2->y()-s1->y())*s1->x()) >
	     cutd02*((s2->y()-s1->y())*(s2->y()-s1->y())+(s1->x()-s2->x())*(s1->x()-s2->x()))) continue;

	  // Cut on dr
	  if(s2->r()<=r1) continue;

	  // Cut on phi/r
	  //cut = m_dynamicalSeedingPreCutRPhi*dr;
	  //dphi = s2->phi() - phi1;
	  //if(dphi>M_PI) dphi=dphi-2*M_PI; else if(dphi<-M_PI) dphi=dphi+2*M_PI;
	  //if(dphi>cut || dphi<-cut) continue;
	  dphi = s2->phi()-phi1;
	  if(dphi>M_PI) dphi=dphi-2*M_PI; else if(dphi<-M_PI) dphi=dphi+2*M_PI;
	  if(dphi>m_dynamicalSeedingPreCutRPhi*(s2->r()-r1) || dphi<-m_dynamicalSeedingPreCutRPhi*(s2->r()-r1)) continue;
	  
	  // Create and save StrBSeed
	  pSeed->deleted   = false;
	  pSeed->seed_sp1  = s1;
	  pSeed->seed_sp2  = s2;
	  pSeed->z0        = (z1*(s2->r()-r1)-(s2->z()-z1)*r1)/(s2->r()-r1);

	  // Increase pointers
	  pSeed++;

	  // Increase counters
	  m_trks_num++;

	  seedMakingCount++;
	  if(seedMakingCount>=seedMakingLimit) limit=true;

	}
      }
    }
  }

  m_trks_num_active = m_trks_num; 
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::executeSeedFilter_Vertex() {

  /** Clear vertexing histo. */
  for(int i=0; i<m_vtxMaxBin; i++) m_vtxHisto[i] = 0;

  /** Fill histogram. */
  float scale=m_vertexingWindowSegments/m_vertexingWindowSize;
  for(unsigned int seed=0; seed<m_trks_num; seed++) {
    int index = (int)floor((m_trks[seed].z0+m_vertexingCutRZ)*scale);
    m_vtxHisto[index]++;
  }

  /** Find maxima. */
  for(int nVtx=0; nVtx<m_vertexingMaxVertices; nVtx++) {
    int i, val=0, maxVal=0, maxPos=m_vertexingWindowSegments/4;
    for(i=0; i<m_vertexingWindowSegments; i++) val+=m_vtxHisto[i];
    for(maxVal=val, i=m_vertexingWindowSegments; i<m_vtxMaxBin; i++) {
      val-=m_vtxHisto[i-m_vertexingWindowSegments];
      val+=m_vtxHisto[i];
      if(val>maxVal) {maxVal=val; maxPos=i-(m_vertexingWindowSegments/4);}
    }
    for(i=-(m_vertexingWindowSegments/2); i<(m_vertexingWindowSegments/2); i++) if((maxPos+i)>=0 && (maxPos+i)<m_vtxMaxBin) m_vtxHisto[maxPos+i]=0;
    float recVtx = -m_vertexingCutRZ + maxPos*(m_vertexingWindowSize/m_vertexingWindowSegments);
    
    /** Save vertex. */
    m_recZv.push_back(recVtx);
  }

  /** Filter tracks around primary vertex. */
  m_trks_num_active = 0;
  for(unsigned int seed=0; seed<m_trks_num; seed++) {
    
    // Evaluate minimum distance from primary vertex
    float z0V = 1000.0;
    std::vector<float>::iterator v, vEnd=m_recZv.end();
    for(v=m_recZv.begin(); v!=vEnd; v++)
      if(m_trks[seed].z0-(*v)<z0V && m_trks[seed].z0-(*v)>-z0V) z0V=fabs(m_trks[seed].z0-(*v));

    //Scale with sin theta
    float dr = m_trks[seed].seed_sp2->r()-m_trks[seed].seed_sp1->r();
    float dz = m_trks[seed].seed_sp2->z()-m_trks[seed].seed_sp1->z();
    float denom = dr*dr+dz*dz;
    float sintheta2=1;
    if(denom!=0) sintheta2=dr*dr/denom;

    // Apply RZ cut around primary vertex
    if((z0V*z0V*sintheta2)>(m_dynamicalSeedingCutRZ*m_dynamicalSeedingCutRZ))
      m_trks[seed].deleted=true;
    else
      m_trks_num_active++;
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::executeTripletDataSorting(const std::vector<const TrigSiSpacePoint*>& spacePointData) {

  /** Define z0 bins. */
  const float cutz0 = m_vertexingCutRZ;
  const float z0Min = -cutz0;
  const float z0Bin = 2*cutz0/m_spTripletZ0Steps;

  /** Define eta bins. */
  float ptMin = m_recoPtMin+m_recoPtMinSeedDiff; if(ptMin<1.0) ptMin=1.0;
  float cuteta = 0.0325 + 490.0/ptMin; if(cuteta<m_etaHalfWidth)cuteta=m_etaHalfWidth; if(cuteta>10)cuteta=10;
  const float etaMin = m_roiEta-cuteta;
  const float etaBin = 2*cuteta/m_spTripletEtaSteps;
  std::vector<float> dzdrLo, dzdrHi;
  dzdrLo.reserve(m_spTripletEtaSteps+1);  
  dzdrHi.reserve(m_spTripletEtaSteps+1);  
  for(unsigned int loop=0; loop<m_spTripletEtaSteps; loop++) {
    float etaLo = etaMin+etaBin*loop - 0.5*etaBin;
    float etaHi = etaMin+etaBin*(loop+1) + 0.5*etaBin;
    float mintheta = 2*atan(exp(-(etaLo)));
    float maxtheta = 2*atan(exp(-(etaHi)));
    float mindzdr = cos(mintheta)/sin(mintheta);
    float maxdzdr = cos(maxtheta)/sin(maxtheta);
    dzdrLo.push_back(mindzdr);
    dzdrHi.push_back(maxdzdr);
  }

  /** Loop on space points. */
  std::vector<const TrigSiSpacePoint*>::const_iterator sp, spEnd=spacePointData.end();
  for(sp=spacePointData.begin(); sp!=spEnd; sp++) {
    const long layer = (*sp)->layer();
    const float z = (*sp)->z(), r = (*sp)->r();
    if (r == 0) continue;
    for(unsigned int loopz0=0; loopz0<m_spTripletZ0Steps; loopz0++) {
      const float z0Lo = z0Min+z0Bin*loopz0 - 0.5*z0Bin;
      //const float z0Hi = z0Lo + 2*z0Bin;
      const float dzdrH = (z - z0Lo) / r;
      const float dzdrL = dzdrH - 2*z0Bin / r;   // = (z - z0Hi) / r; dzdrL is always lower than dzdrH
      std::vector<float>::iterator pos = std::lower_bound(dzdrHi.begin(), dzdrHi.end(), dzdrL);
      const int firstEta = std::distance(dzdrHi.begin(), pos);
      //  int lastEta = -1;
      for (unsigned loopeta = firstEta; loopeta < m_spTripletEtaSteps and dzdrLo[loopeta] <= dzdrH; loopeta++) {
        m_spTriplet[layer*m_spTripletZ0Steps*m_spTripletEtaSteps+loopz0*m_spTripletEtaSteps+loopeta].push_back(*sp);
	// lastEta = loopeta;
      }
    }
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigL2PattRecoStrategyB::executeTripletMaking() {

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
	 
  float tripletMakingLimit=m_recoTimingFactor*2000.0*(sqrt(m_nSP/500.0))*m_mapLayerOneMultiplicity*m_mapLayerTwoMultiplicity;
  bool limit = false;
  int tripletMakingCount = 0;
  
  std::vector<const TrigSiSpacePoint*>::iterator sp3, sp3End; // 3rd layer spacepoints
  const TrigSiSpacePoint *s1, *s2;
  float x1, x2, y1, y2, z1, z2, r1, r2;
  float x12, y12, z12, r12;
  float r12inv; // z12inv; // extrapolation scale along R or Z
  float dphi;
  float cut_rz, cut_rphi=m_dynamicalExtensionCutRPhi*m_dynamicalExtensionCutRPhi;
  float ptMin = m_recoPtMin+m_recoPtMinTripletDiff; if(ptMin<1.0) ptMin=1.0;
  float cut_eta = (5./5.) * (0.0325 + 490/ptMin);
  float cut_phi = (5./5.) * (0.003  + 970/ptMin);
  if(cut_eta<m_etaHalfWidth) cut_eta = m_etaHalfWidth;
  if(cut_phi<m_phiHalfWidth) cut_phi = m_phiHalfWidth;
  float cut_d0  = m_recoD0Max + m_recoD0MaxTripletDiff;
  float cut_pt  = m_recoPtMin + m_recoPtMinTripletDiff;
  float r12_squared, costheta_squared;
  float x12_scaled, y12_scaled;//, z12_scaled, r12_scaled;
  float xm1, ym1, cx1, cy1;
  float xm2, ym2, cx2, cy2;
  float tr, xc, yc, rc, phic;
  float cotantheta;
  float n, rr, rz, r, z, ang;
  float iphi, ir, iz, ezez;
  float d0, pt, phi, eta;
  float diff_eta, diff_phi;
  float phi12, eta12, theta12, theta, tanHalfTheta;


  // Define z0 bins
  float cutz0 = m_vertexingCutRZ;
  float z0Min = -cutz0;
  float z0Bin = 2*cutz0/m_spTripletZ0Steps;
  std::vector<float> z0Lo, z0Hi;
  for(unsigned int loop=0; loop<m_spTripletZ0Steps; loop++) {
    float minz0 = z0Min+z0Bin*loop;
    float maxz0 = z0Min+z0Bin*(loop+1);
    z0Lo.push_back(minz0);
    z0Hi.push_back(maxz0);
  }

  // Define eta bins
  float cuteta = 0.0325 + 490.0/ptMin; if(cuteta<m_etaHalfWidth)cuteta=m_etaHalfWidth; if(cuteta>10)cuteta=10;
  float etaMin = m_roiEta-cuteta;
  float etaBin = 2*cuteta/m_spTripletEtaSteps;
  std::vector<float> dzdrLo, dzdrHi;
  for(unsigned int loop=0; loop<m_spTripletEtaSteps; loop++) {
    float etaLo = etaMin+etaBin*loop;
    float etaHi = etaMin+etaBin*(loop+1);
    float mintheta = 2*atan(exp(-(etaLo)));
    float maxtheta = 2*atan(exp(-(etaHi)));
    float mindzdr = cos(mintheta)/sin(mintheta);
    float maxdzdr = cos(maxtheta)/sin(maxtheta);
    dzdrLo.push_back(mindzdr);
    dzdrHi.push_back(maxdzdr);
  }


  // Loop on patterns
  m_trks_num_active=0;
  for(unsigned int trk=0; trk<m_trks_num && !limit; trk++) {

    // Check if deleted and clear number of triplets
    if(m_trks[trk].deleted) continue;
    m_trks[trk].triplet_num=0;
    m_trks[trk].triplet_sp3.clear();
    m_trks[trk].triplet_eta.clear();
    m_trks[trk].triplet_phi.clear();
    m_trks[trk].triplet_chi2.clear();
    m_trks[trk].triplet_pt.clear();
    m_trks[trk].triplet_d0.clear();

    // Check if event timeout was reached
    if (Athena::Timeout::instance().reached()) {
      if(outputLevel<=MSG::DEBUG)
    	athenaLog << MSG::DEBUG << "Timeout reached. Aborting sequence." << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
    }
      
    // Get sp1 and sp2 info
    s1=m_trks[trk].seed_sp1;
    s2=m_trks[trk].seed_sp2;
    x1 = s1->x();
    x2 = s2->x();
    y1 = s1->y();
    y2 = s2->y();
    z1 = s1->z();
    z2 = s2->z();
    r1 = s1->r();
    r2 = s2->r();
    x12 = x2 - x1;
    y12 = y2 - y1;
    z12 = z2 - z1;
    r12 = r2 - r1;

    // Protect against division by zero
    if(r12>-0.00001 && r12<0.00001) {
      float sign=1; if(r12<0) sign=-1;
      r12=sign*0.00001;
    }

    // Protect against division by zero
    if(z12>-0.00001 && z12<0.00001) {
      float sign=1; if(z12<0) sign=-1;
      z12=sign*0.00001;
    }

    r12inv = 1.0 / r12;
    //  z12inv = 1.0 / z12;
    costheta_squared = r12*r12/(z12*z12+r12*r12);
    r12_squared = r12*r12;
    x12_scaled = x12*r12inv;
    y12_scaled = y12*r12inv;
    //    z12_scaled = z12*r12inv;
    // r12_scaled = r12*z12inv;
    // rphi fit variables
    xm1 = (s2->x()+s1->x())*0.5;
    ym1 = (s2->y()+s1->y())*0.5;
    cx1 = s2->y()-s1->y(); if(cx1==0) cx1=0.0001;
    cy1 = s1->x()-s2->x(); if(cy1==0) cy1=0.0001;
    // rz fit variables
    cotantheta = z12/r12;
    
    // Get z0 index info
    unsigned int z0index=0;
    float z0=m_trks[trk].z0;;
    for(unsigned int loop=0; loop<m_spTripletZ0Steps; loop++) {
      if(z0>=z0Lo[loop] && z0<z0Hi[loop]) z0index=loop;
    }

    // Get eta index info
    unsigned int etaindex=0;
    float dzdr=cotantheta;
    for(unsigned int loop=0; loop<m_spTripletEtaSteps; loop++) {
      if(dzdr>=dzdrLo[loop] && dzdr<dzdrHi[loop]) etaindex=loop;
    }

    // Loop on z0 indexes
    for(unsigned int z0i=z0index; z0i<=z0index; z0i++) {
      
      // Loop on eta indexes
      for(unsigned int etai=etaindex; etai<=etaindex; etai++) {
	
	// Loop on barrel layers
	int lay = s2->layer()+1;
	for( ; lay<m_layPixEC && !limit; lay++) { // USING GEO INFO
	  
	  // Check layer matching
	  //if(fabs((z1 + z12_scaled*(m_geoR[lay]-r1)))<m_geoZmin[lay] || fabs((z1 + z12_scaled*(m_geoR[lay]-r1)))>m_geoZmax[lay]) continue;
	  if(lay<m_laySctB) cut_rz=m_dynamicalExtensionCutRZPixel*m_dynamicalExtensionCutRZPixel; // USING GEO INFO
	  else cut_rz=m_dynamicalExtensionCutRZ*m_dynamicalExtensionCutRZ;
	  
	  // Loop on space points of each layer
	  std::vector<const TrigSiSpacePoint*>& spv = m_spTriplet[lay*m_spTripletZ0Steps*m_spTripletEtaSteps+z0i*m_spTripletEtaSteps+etai];
	  sp3End=spv.end();
	  for(sp3=spv.begin(); sp3!=sp3End && !limit; sp3++) {
	    
	    // Cut on rz
	    //dz = (z1-(*sp3)->z())*r12+((*sp3)->r()-r1)*z12;
	    //value = dz*dz*costheta_squared;
	    //cut = cut_rz*r12*r12;
	    if(((z1-(*sp3)->z())*r12+((*sp3)->r()-r1)*z12)*((z1-(*sp3)->z())*r12+((*sp3)->r()-r1)*z12)*costheta_squared > (cut_rz*r12_squared)) continue;
	    
	    // Cut on rphi
	    if(((x1 + x12_scaled*((*sp3)->r()-r1) - (*sp3)->x())*(x1 + x12_scaled*((*sp3)->r()-r1) - (*sp3)->x()) +
		(y1 + y12_scaled*((*sp3)->r()-r1) - (*sp3)->y())*(y1 + y12_scaled*((*sp3)->r()-r1) - (*sp3)->y())) > 
	       (cut_rphi*((*sp3)->r()-r2)*((*sp3)->r()-r2)*((*sp3)->r()-r2))) continue;
	    
	    // Fit in rphi
	    xm2 = (s2->x()+(*sp3)->x())*0.5;
	    ym2 = (s2->y()+(*sp3)->y())*0.5;
	    cx2 = (s2->y()-(*sp3)->y()); if(cx2==0) cx2=0.0001;
	    cy2 = ((*sp3)->x()-s2->x()); if(cy2==0) cy2=0.0001;
	    // Cut on too straight tracks
	    if(cy2==0) continue; 
	    tr = (cx1-cy1*cx2/cy2); 
	    if(tr==0) continue;
	    // Evaluate circle radius
	    tr = (xm2-xm1+(ym1-ym2)*cx2/cy2)/tr;
	    xc = xm1 + cx1*tr;
	    yc = ym1 + cy1*tr;
	    rc = sqrt((xc-s1->x())*(xc-s1->x())+(yc-s1->y())*(yc-s1->y()));
	    // Cut on d0
	    d0 = fabs(sqrt(xc*xc+yc*yc)-rc);
	    if(d0>cut_d0) continue;
	    // Cut on pT
	    pt = rc*1.042*9.0/15.0;
	    if(pt<cut_pt) continue;
	    // Evaluate phi
	    phi  = atan2(-xc, yc);
	    dphi = phi-s1->phi();
	    if(dphi<-M_PI) dphi+=2*M_PI;
	    if(dphi>M_PI)  dphi-=2*M_PI;
	    if(dphi<-M_PI*0.5) phi+=M_PI;
	    if(dphi>M_PI*0.5)  phi-=M_PI;
	    if(phi<-M_PI) phi+=2*M_PI;
	    if(phi>M_PI)  phi-=2*M_PI;
	    
	    // Fit in rz
	    phic = atan2(xc, -yc);
	    n=rr=rz=r=z=0;
	    // sp1
	    ezez =(s1->dz()*s1->dz())+(cotantheta*cotantheta)*(s1->dr()*s1->dr());
	    iphi = atan2(xc-s1->x(), -yc+s1->y());
	    ir = iphi-phic; if(ir>M_PI) ir=ir-2*M_PI; if(ir<-M_PI) ir=ir+2*M_PI;
	    iz = s1->z();
	    n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
	    // sp2
	    ezez =(s2->dz()*s2->dz())+(cotantheta*cotantheta)*(s2->dr()*s2->dr());
	    iphi = atan2(xc-s2->x(), -yc+s2->y());
	    ir = iphi-phic; if(ir>M_PI) ir=ir-2*M_PI; if(ir<-M_PI) ir=ir+2*M_PI;
	    iz = s2->z();
	    n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
	    // sp3
	    ezez =((*sp3)->dz()*(*sp3)->dz())+(cotantheta*cotantheta)*((*sp3)->dr()*(*sp3)->dr());
	    iphi = atan2(xc-(*sp3)->x(), -yc+(*sp3)->y());
	    ir = iphi-phic; if(ir>M_PI) ir=ir-2*M_PI; if(ir<-M_PI) ir=ir+2*M_PI;
	    iz = (*sp3)->z();
	    n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
	    // Evaluate eta
	    if((n*rr-r*r)==0) continue;
	    ang = (n*rz-r*z)/(n*rr-r*r);
	    theta = atan2(rc, ang); if(theta<-M_PI) theta+=2*M_PI; if(theta>M_PI) theta-=2*M_PI; if(theta<0) theta=M_PI+theta;
	    if((theta>(M_PI*0.5) && z12>0) || (theta<(M_PI*0.5) && z12<0)) theta=M_PI-theta;
	    eta = -log(tan(theta*0.5));
	    
	    // Cut on eta-phi
	    diff_eta = eta - m_roiEta;
	    diff_phi = phi - m_roiPhi;
	    if(diff_phi<-M_PI) diff_phi+=2*M_PI;
	    if(diff_phi>M_PI)  diff_phi-=2*M_PI;
	    if(diff_eta>cut_eta || diff_eta<-cut_eta || diff_phi>cut_phi || diff_phi<-cut_phi) continue;
	    
	    // Save triplet
	    if(m_trks[trk].triplet_num<100) {
	      m_trks[trk].triplet_sp3.push_back(*sp3);
	      m_trks[trk].triplet_eta.push_back(eta);
	      m_trks[trk].triplet_phi.push_back(phi);
	      m_trks[trk].triplet_d0.push_back(d0);
	      m_trks[trk].triplet_pt.push_back(pt);
	      m_trks[trk].triplet_num++;
	    }
	    tripletMakingCount++;
	    if(tripletMakingCount>tripletMakingLimit) {m_trks_num=trk+1; limit=true;}
	    
	  }
	}
	
	// Loop on endcap layers
	for( ; lay<m_layMax && !limit; lay++) { // USING GEO INFO
	  
	  // Check layer matching
	  //if(fabs((r1 + r12_scaled*(m_geoZ[lay]-z1)))<m_geoRmin[lay] || fabs((r1 + r12_scaled*(m_geoZ[lay]-z1)))>m_geoRmax[lay]) {rejLayMatch++; continue;}
	  if(lay<m_laySctEC) cut_rz=m_dynamicalExtensionCutRZPixel*m_dynamicalExtensionCutRZPixel; // USING GEO INFO
	  else cut_rz=m_dynamicalExtensionCutRZ*m_dynamicalExtensionCutRZ;
	  
	  // Loop on space points of each layer
	  std::vector<const TrigSiSpacePoint*>& spv = m_spTriplet[lay*m_spTripletZ0Steps*m_spTripletEtaSteps+z0i*m_spTripletEtaSteps+etai];
	  sp3End=spv.end();
	  for(sp3=spv.begin(); sp3!=sp3End && !limit; sp3++) {
	    
	    // Cut on rz
	    if(((z1-(*sp3)->z())*r12+((*sp3)->r()-r1)*z12)*((z1-(*sp3)->z())*r12+((*sp3)->r()-r1)*z12)*costheta_squared > (cut_rz*r12_squared)) continue;
	    
	    // Cut on rphi
	    if(((x1 + x12_scaled*((*sp3)->r()-r1) - (*sp3)->x())*(x1 + x12_scaled*((*sp3)->r()-r1) - (*sp3)->x()) +
		(y1 + y12_scaled*((*sp3)->r()-r1) - (*sp3)->y())*(y1 + y12_scaled*((*sp3)->r()-r1) - (*sp3)->y())) > 
	       (cut_rphi*((*sp3)->r()-r2)*((*sp3)->r()-r2)*((*sp3)->r()-r2))) continue;
	    
	    // Fit in rphi
	    xm2 = (s2->x()+(*sp3)->x())*0.5;
	    ym2 = (s2->y()+(*sp3)->y())*0.5;
	    cx2 = (s2->y()-(*sp3)->y()); if(cx2==0) cx2=0.0001;
	    cy2 = ((*sp3)->x()-s2->x()); if(cy2==0) cy2=0.0001;
	    // Cut on too straight tracks
	    if(cy2==0) continue; 
	    tr = (cx1-cy1*cx2/cy2); 
	    if(tr==0) continue;
	    // Evaluate circle radius
	    tr = (xm2-xm1+(ym1-ym2)*cx2/cy2)/tr;
	    xc = xm1 + cx1*tr;
	    yc = ym1 + cy1*tr;
	    rc = sqrt((xc-s1->x())*(xc-s1->x())+(yc-s1->y())*(yc-s1->y()));
	    // Cut on d0
	    d0 = fabs(sqrt(xc*xc+yc*yc)-rc);
	    if(d0>cut_d0) continue;
	    // Cut on pT
	    pt = rc*1.042*9.0/15.0;
	    if(pt<cut_pt) continue;
	    // Evaluate phi
	    phi = atan2(-xc, yc);
	    dphi = phi-s1->phi();
	    if(dphi<-M_PI) dphi+=2*M_PI;
	    if(dphi>M_PI)  dphi-=2*M_PI;
	    if(dphi<-M_PI*0.5) phi+=M_PI;
	    if(dphi>M_PI*0.5)  phi-=M_PI;
	    if(phi<-M_PI) phi+=2*M_PI;
	    if(phi>M_PI)  phi-=2*M_PI;
	    
	    // Fit in rz
	    phic = atan2(xc, -yc);
	    n=rr=rz=r=z=0;
	    // sp1
	    ezez =(s1->dz()*s1->dz())+(cotantheta*cotantheta)*(s1->dr()*s1->dr());
	    iphi = atan2(xc-s1->x(), -yc+s1->y());
	    ir = iphi-phic; if(ir>M_PI) ir=ir-2*M_PI; if(ir<-M_PI) ir=ir+2*M_PI;
	    iz = s1->z();
	    n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
	    // sp2
	    ezez =(s2->dz()*s2->dz())+(cotantheta*cotantheta)*(s2->dr()*s2->dr());
	    iphi = atan2(xc-s2->x(), -yc+s2->y());
	    ir = iphi-phic; if(ir>M_PI) ir=ir-2*M_PI; if(ir<-M_PI) ir=ir+2*M_PI;
	    iz = s2->z();
	    n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
	    // sp3
	    ezez =((*sp3)->dz()*(*sp3)->dz())+(cotantheta*cotantheta)*((*sp3)->dr()*(*sp3)->dr());
	    iphi = atan2(xc-(*sp3)->x(), -yc+(*sp3)->y());
	    ir = iphi-phic; if(ir>M_PI) ir=ir-2*M_PI; if(ir<-M_PI) ir=ir+2*M_PI;
	    iz = (*sp3)->z();
	    n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
	    // Evaluate eta
	    if((n*rr-r*r)==0) continue;
	    ang = (n*rz-r*z)/(n*rr-r*r);
	    theta = atan2(rc, ang); if(theta<-M_PI) theta+=2*M_PI; if(theta>M_PI) theta-=2*M_PI; if(theta<0) theta=M_PI+theta;
	    if((theta>(M_PI*0.5) && z12>0) || (theta<(M_PI*0.5) && z12<0)) theta=M_PI-theta;
	    eta = -log(tan(theta*0.5));
	    
	    // Cut on eta-phi
	    diff_eta = eta - m_roiEta;
	    diff_phi = phi - m_roiPhi;
	    if(diff_phi<-M_PI) diff_phi+=2*M_PI;
	    if(diff_phi>M_PI)  diff_phi-=2*M_PI;
	    if(diff_eta>cut_eta || diff_eta<-cut_eta || diff_phi>cut_phi || diff_phi<-cut_phi) continue;
	    
	    // Save triplet
	    if(m_trks[trk].triplet_num<100) {
	      m_trks[trk].triplet_sp3.push_back(*sp3);
	      m_trks[trk].triplet_eta.push_back(eta);
	      m_trks[trk].triplet_phi.push_back(phi);
	      m_trks[trk].triplet_d0.push_back(d0);
	      m_trks[trk].triplet_pt.push_back(pt);
	      m_trks[trk].triplet_num++;
	    }
	    tripletMakingCount++;
	    if(tripletMakingCount>tripletMakingLimit) {m_trks_num=trk+1; limit=true;}
	    
	  }
	}
      }
      
      // Delete trk if too few triplets are found
      if((int)m_trks[trk].triplet_num<(m_dynamic_extensionCutSpacePoints-2))
	m_trks[trk].deleted = true;
      else {
	// Increase counter
	m_trks_num_active++;
	// Evaluate chi2 of triplets
	phi12 = atan2(s2->y()-s1->y(), s2->x()-s1->x());
	if(phi12>M_PI) phi12=phi12-2*M_PI; else if(phi12<-M_PI) phi12=phi12+2*M_PI;
	theta12 = atan2(s2->r()-s1->r(), s2->z()-s1->z());
	if(theta12<-M_PI) theta12+=2*M_PI; if(theta12>M_PI) theta12-=2*M_PI; if(theta12<0) theta12=M_PI+theta12;
	tanHalfTheta=tan(0.5*theta12);
	if(tanHalfTheta!=0)
	  eta12 = -log(tanHalfTheta);
	else {
	  if(s2->z()-s1->z()>0) eta12=4.0;
	  else eta12=-4.0;
	}
	for(unsigned int trip=0; trip<m_trks[trk].triplet_num; trip++) {
	  diff_eta = m_trks[trk].triplet_eta[trip]-eta12;
	  diff_phi = m_trks[trk].triplet_phi[trip]-phi12;
	  if(diff_phi<-M_PI) diff_phi+=2*M_PI;
	  if(diff_phi>M_PI)  diff_phi-=2*M_PI;
	  m_trks[trk].triplet_chi2.push_back(diff_eta*diff_eta+diff_phi*diff_phi);
	}
      }    
    }  
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::executeTripletAmbiguity() {
 
  // Loop on trks
  m_full_trks_num = 0;
  for(unsigned int trk=0; trk<m_trks_num && ((int)m_full_trks_num)<m_recoMaxTracks; trk++) {

    // Check if deleted
    if(m_trks[trk].deleted) continue;

    // Create ambiguities
    std::vector<StrBTrack> ambiguities(m_trks[trk].triplet_num);
    for(unsigned int triplet=0; triplet<m_trks[trk].triplet_num; triplet++) {
      ambiguities[triplet].sp[m_trks[trk].seed_sp1->layer()]=m_trks[trk].seed_sp1;
      ambiguities[triplet].sp[m_trks[trk].seed_sp2->layer()]=m_trks[trk].seed_sp2;
      ambiguities[triplet].sp[m_trks[trk].triplet_sp3[triplet]->layer()]=m_trks[trk].triplet_sp3[triplet];
      ambiguities[triplet].eta=m_trks[trk].triplet_eta[triplet];
      ambiguities[triplet].phi=m_trks[trk].triplet_phi[triplet];
      ambiguities[triplet].chi2=m_trks[trk].triplet_chi2[triplet];
    }

    // Fill ambiguity list
    typedef std::vector<StrBTrack>::iterator StrBTrackScalarIterator;
    std::multimap< float, std::pair<StrBTrackScalarIterator,StrBTrackScalarIterator> > ambiguitiesMap;
    StrBTrackScalarIterator t1, t2, tEnd=ambiguities.end();
    for(t1=ambiguities.begin(); t1!=tEnd; t1++) {
      for(t2=t1, t2++; t2!=tEnd; t2++) {
	if(t1->deleted || t2->deleted) continue;
	float distance = tripletAmbiguityDistance(&(*t1), &(*t2));
	if(distance<m_extensionSolveTripletsDRCut) ambiguitiesMap.insert(std::make_pair(distance, std::make_pair(t1,t2)));
      }
    }
    if(ambiguitiesMap.size()==0) {
      m_trks[trk].deleted=true;
      continue;
    }
    
    // Merge ambiguities
    std::multimap< float, std::pair<StrBTrackScalarIterator,StrBTrackScalarIterator> >::iterator ambiguity, ambiguityEnd=ambiguitiesMap.end();
    for(ambiguity=ambiguitiesMap.begin(); ambiguity!=ambiguityEnd; ambiguity++) {
      tripletMergeAmbiguity(&(*(ambiguity->second.first)), &(*(ambiguity->second.second)));
    }

    // Find best ambiguity
    StrBTrackScalarIterator amb=ambiguities.begin(), ambEnd=ambiguities.end(), ambBest=ambiguities.begin();
    for(amb++; amb!=ambEnd; amb++) {
      if((*amb)<(*ambBest)) ambBest = amb;
    }

    // Delete pattern if too few space points in best ambiguity
    ambBest->evaluateSum();
    if(ambBest->sumSp < m_dynamic_extensionCutSpacePoints) {
      m_trks[trk].deleted=true;
    }
    // Otherwise save best ambiguity
    else {
      m_full_trks[m_full_trks_num] = (*ambBest);
      m_full_trks[m_full_trks_num].resetDistance();
      m_full_trks_num++;
    }
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::executeTrackAmbiguity() {
	
  // Fill ambiguity list
  std::multimap< float, std::pair<StrBTrack*,StrBTrack*> > ambiguities;
  float deta, dphi;

  // Loop on trks
  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
    // Check if deleted
    if(m_full_trks[trk1].deleted) continue;
    for(unsigned int trk2=trk1+1; trk2<m_full_trks_num; trk2++) {
      // Check if deleted
      if(m_full_trks[trk2].deleted) continue;
      // Check eta range
      deta = m_full_trks[trk2].eta-m_full_trks[trk1].eta;
      if(deta<-0.1) continue;
      if(deta>0.1) continue;
      // Check phi range
      dphi = m_full_trks[trk2].phi-m_full_trks[trk1].phi; if(dphi<-M_PI) dphi+=2*M_PI; if(dphi>M_PI) dphi-=2*M_PI;
      if(dphi<-0.1) continue;
      if(dphi>0.1) continue;
      // Evaluate ambiguity
      float fraction = trackAmbiguityFraction(&(m_full_trks[trk1]), &(m_full_trks[trk2]));
      if((m_extensionSolveShared&&(fraction>0.3)) || (fraction>0.5))
	ambiguities.insert(std::make_pair(fraction, std::make_pair(&(m_full_trks[trk1]),&(m_full_trks[trk2]))));
    }
  }
      
  // Remove ambiguities
  std::multimap< float, std::pair<StrBTrack*,StrBTrack*> >::reverse_iterator amb, ambEnd=ambiguities.rend();
  for(amb=ambiguities.rbegin(); amb!=ambEnd; amb++) {
    if(amb->second.first->deleted || amb->second.second->deleted) continue;
    float fraction = trackAmbiguityFraction(amb->second.first, amb->second.second);
    if(fraction>=0.75 && trackAmbiguityDistance(amb->second.first, amb->second.second)<m_extensionSolveTracksDRCut)
      trackMergeAmbiguity(amb->second.first, amb->second.second);
    else
      trackRemoveAmbiguity(amb->second.first, amb->second.second);
  }
  ambiguities.clear();
  
  // Loop on trks
  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
    // Check if deleted
    if(m_full_trks[trk1].deleted) continue;
    for(unsigned int trk2=trk1+1; trk2<m_full_trks_num; trk2++) {
      // Check if deleted
      if(m_full_trks[trk2].deleted) continue;
      // Evaluate ambiguity
      float fraction = trackAmbiguityFraction(&(m_full_trks[trk1]), &(m_full_trks[trk2]));
      if((m_extensionSolveShared&&(fraction>0.2)) || (fraction>0.5))
	ambiguities.insert(std::make_pair(fraction, std::make_pair(&(m_full_trks[trk1]),&(m_full_trks[trk2]))));
    }
  }
      
  // Remove ambiguities
  ambEnd=ambiguities.rend();
  for(amb=ambiguities.rbegin(); amb!=ambEnd; amb++) {
    if(amb->second.first->deleted || amb->second.second->deleted) continue;
    float fraction = trackAmbiguityFraction(amb->second.first, amb->second.second);
    if(fraction>=0.75 && trackAmbiguityDistance(amb->second.first, amb->second.second)<m_extensionSolveTracksDRCut)
      trackMergeAmbiguity(amb->second.first, amb->second.second);
    else
      trackRemoveAmbiguity(amb->second.first, amb->second.second);
  }
  ambiguities.clear();
  
  // Remove shared ambiguities
  if(m_extensionSolveShared) {
    for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
      if(m_full_trks[trk1].deleted) continue;
      for(unsigned int trk2=trk1+1; trk2<m_full_trks_num; trk2++) {
	if(m_full_trks[trk2].deleted) continue;
	// Evaluate shared ambiguities
	trackAmbiguityShared(&(m_full_trks[trk1]), &(m_full_trks[trk2]));
      }
    }
  }

  m_full_trks_num_active=0;
  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
    if(m_full_trks[trk1].deleted) continue;
    m_full_trks[trk1].evaluateSum();
    if(m_extensionSolveShared && (m_full_trks[trk1].evaluateSharedFraction()>0.2)) {
      m_full_trks[trk1].deleted=true;
    }
    else m_full_trks_num_active++;
  }
}
	

//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::executeTrackMaking(TrigInDetTrackCollection*& trackColl) {

  // Check if the track has too few space points or has been deleted
  for(unsigned int trk=0; trk<m_full_trks_num; trk++) {
    if(m_full_trks[trk].deleted) continue;
    m_full_trks[trk].evaluateSum();
    if((m_full_trks[trk].sumSp<4) || (m_full_trks[trk].sumSpAll<m_dynamic_extensionSpacePoints) || m_full_trks[trk].deleted) {
      m_full_trks[trk].deleted = true;
    }
  }

  // Create TrigInDetTracks
  for(unsigned int trk=0; trk<m_full_trks_num; trk++) {
    if(m_full_trks[trk].deleted) continue;

    // Create space point vector
    std::vector<const TrigSiSpacePoint*>* trackSpacePoints = new std::vector<const TrigSiSpacePoint*>;

    //std::cout << "SITRACK TRACK ----------------------" << std::endl;

    // Pixel barrel
    for(int i=0; i<m_laySctB; i++) {
      if(m_full_trks[trk].sp[i]!=0) {
	trackSpacePoints->push_back(m_full_trks[trk].sp[i]);
	//std::cout << "SP" << i << " r/z/phi " << m_full_trks[trk].sp[i]->r() << " " << m_full_trks[trk].sp[i]->z() << " " << m_full_trks[trk].sp[i]->phi() << " " << std::endl;
      }
      if(m_full_trks[trk].sp2[i]!=0) {
	trackSpacePoints->push_back(m_full_trks[trk].sp2[i]);
	//std::cout << "SP" << i << " r/z/phi " << m_full_trks[trk].sp2[i]->r() << " " << m_full_trks[trk].sp2[i]->z() << " " << m_full_trks[trk].sp2[i]->phi() << " " << std::endl;
      }
    }
    // Pixel endcap
    for(int i=m_layPixEC; i<m_laySctEC; i++) {
      if(m_full_trks[trk].sp[i]!=0) {
	trackSpacePoints->push_back(m_full_trks[trk].sp[i]);
	//std::cout << "SP" << i << " r/z/phi " << m_full_trks[trk].sp[i]->r() << " " << m_full_trks[trk].sp[i]->z() << " " << m_full_trks[trk].sp[i]->phi() << " " << std::endl;
      }
      if(m_full_trks[trk].sp2[i]!=0) {
	trackSpacePoints->push_back(m_full_trks[trk].sp2[i]);
	//std::cout << "SP" << i << " r/z/phi " << m_full_trks[trk].sp2[i]->r() << " " << m_full_trks[trk].sp2[i]->z() << " " << m_full_trks[trk].sp2[i]->phi() << " " << std::endl;
      }
    }
    // SCT barrel
    for(int i=m_laySctB; i<m_layPixEC; i++) {
      if(m_full_trks[trk].sp[i]!=0) {
	trackSpacePoints->push_back(m_full_trks[trk].sp[i]);
	//std::cout << "SP" << i << " r/z/phi " << m_full_trks[trk].sp[i]->r() << " " << m_full_trks[trk].sp[i]->z() << " " << m_full_trks[trk].sp[i]->phi() << " " << std::endl;
      }
      if(m_full_trks[trk].sp2[i]!=0) {
	trackSpacePoints->push_back(m_full_trks[trk].sp2[i]);
	//std::cout << "SP" << i << " r/z/phi " << m_full_trks[trk].sp2[i]->r() << " " << m_full_trks[trk].sp2[i]->z() << " " << m_full_trks[trk].sp2[i]->phi() << " " << std::endl;
      }
    }
    // SCT endcap
    for(int i=m_laySctEC; i<m_layMax; i++) {
      if(m_full_trks[trk].sp[i]!=0) {
	trackSpacePoints->push_back(m_full_trks[trk].sp[i]);
	//std::cout << "SP" << i << " r/z/phi " << m_full_trks[trk].sp[i]->r() << " " << m_full_trks[trk].sp[i]->z() << " " << m_full_trks[trk].sp[i]->phi() << " " << std::endl;
      }
      if(m_full_trks[trk].sp2[i]!=0) {
	trackSpacePoints->push_back(m_full_trks[trk].sp2[i]);
	//std::cout << "SP" << i << " r/z/phi " << m_full_trks[trk].sp2[i]->r() << " " << m_full_trks[trk].sp2[i]->z() << " " << m_full_trks[trk].sp2[i]->phi() << " " << std::endl;
      }
    }

    // Create parameters
    const TrigInDetTrackFitPar* fittedTrack;
    fittedTrack = new TrigInDetTrackFitPar(0, m_full_trks[trk].phi, 0, m_full_trks[trk].eta, 0,
					   0, 0, 0, 0, 0);
    
    // Create TrigInDetTrack
    TrigInDetTrack* track = new TrigInDetTrack(trackSpacePoints, fittedTrack, 0, 0);
    track->algorithmId(TrigInDetTrack::STRATEGY_B_ID);
    
    // Preliminary fit track
    bool goodFit = fit(track);
    if(!goodFit){
      delete track;
      continue;
    }

    // Add track to collection
    trackColl->push_back(track);
    
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::executeSetup() {

  /** Inizialize SP layers */
  // Seeding
  std::vector<const TrigSiSpacePoint*> spLayer;
  for(int i=0; i<m_layMax; i++) {
    m_spSeed.push_back(spLayer);
  }

  // Triplet
  if(m_spTriplet==0) {
    // Evaluate z0 steps
    float cutz0 = m_vertexingCutRZ;
    float z0Bin = m_recoZ0Bin;
    m_spTripletZ0Steps = (unsigned int)ceil(2*cutz0/z0Bin);
    // Evaluate eta steps
    float ptMin = m_recoPtMin+m_recoPtMinSeedDiff; if(ptMin<1.0) ptMin=1.0;
    float cuteta = 0.0325 + 490.0/ptMin; if(cuteta<m_etaHalfWidth)cuteta=m_etaHalfWidth; if(cuteta>10)cuteta=10;
    float etaBin = m_recoEtaBin;
    m_spTripletEtaSteps = (unsigned int)ceil(2*cuteta/etaBin);
    // Create array    
    m_spTriplet = new std::vector<const TrigSiSpacePoint*>[m_layMax*m_spTripletZ0Steps*m_spTripletEtaSteps];
  }

}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::executeCleanup() {

  if (m_spTriplet) {
    for (auto triplet : *m_spTriplet) {
      if (triplet) {
        delete triplet;
        triplet = nullptr;
      }
    }
    m_spTriplet->clear();
    delete [] m_spTriplet;
    m_spTriplet = nullptr;
  }

  std::vector< std::vector<const TrigSiSpacePoint*> >::iterator it, itEnd=m_spSeed.end();
  for(it=m_spSeed.begin(); it!=itEnd; it++){
    it->clear();
  } 
  m_spSeed.clear();

  /** Clear roads */
  StrBRoadIterator road, roadEnd=m_roads.end();
  for(road=m_roads.begin(); road!=roadEnd; road++) delete (*road);
  m_roads.clear();
  
  /** Clear tracks */
  m_trks_num = 0;
  m_full_trks_num = 0;

  /** Clear reconstructed vertexes. */
  m_recZv.clear();
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigL2PattRecoStrategyB::tripletAmbiguityDistance(StrBTriplet* t1, StrBTriplet* t2) {

  // Scaling factors
  float scale;
  float dphi, deta;
 
  // Phi distance
  dphi = t1->phi - t2->phi;
  if(dphi>M_PI)  dphi = dphi - 2*M_PI;
  if(dphi<-M_PI) dphi = dphi + 2*M_PI;
  scale = m_extensionSolveTripletsPhiScale;
  dphi /= scale;

  // Eta distance
  deta = t1->eta - t2->eta;
  scale = m_extensionSolveTripletsEtaScale;
  deta /= scale;

  // Return squared distance
  float dr = dphi*dphi + deta*deta;
  return dr;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigL2PattRecoStrategyB::tripletAmbiguityDistance(StrBTrack* t1, StrBTrack* t2) {

  // Scaling factors
  float scale;
  float dphi, deta;
 
  // Phi distance
  dphi = t1->phi - t2->phi;
  if(dphi>M_PI)  dphi = dphi - 2*M_PI;
  if(dphi<-M_PI) dphi = dphi + 2*M_PI;
  scale = m_extensionSolveTripletsPhiScale;
  dphi /= scale;

  // Eta distance
  deta = t1->eta - t2->eta;
  scale = m_extensionSolveTripletsEtaScale;
  deta /= scale;

  // Return squared distance
  float dr = dphi*dphi + deta*deta;
  return dr;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::tripletMergeAmbiguity(StrBTrack* t1, StrBTrack* t2) {

  // Skip deleted
  if(t1->deleted || t2->deleted) return;

  // Check order and set flags
  if((*t1)<(*t2)) {
    // Insert second track space points in first track
    for(int i=0; i<m_layMax; i++) {
      // Add first layer SP
      if(t1->sp[i]==0 && t2->sp[i]!=0) {
        t1->sp[i]=t2->sp[i];
        t1->tr[i]=t2->tr[i];
      }
      // Add second layer SP
      if(t1->sp[i]!=0 && t1->sp2[i]==0 && t2->sp[i]!=0) {
        // Barrel
        if(i<m_layPixEC) {
          float dr = t2->sp[i]->r()-t1->sp[i]->r();
          if(dr>0.5)  {t1->sp2[i]=t2->sp[i]; t1->tr2[i]=t2->tr[i];}
          if(dr<-0.5) {t1->sp2[i]=t1->sp[i]; t1->sp[i]=t2->sp[i]; t1->tr2[i]=t1->tr[i]; t1->tr[i]=t2->tr[i];}
        }
        // Endcap
        else {
          float dz = t2->sp[i]->z()-t1->sp[i]->z();
          if(t2->sp[i]->z()<0) dz=-dz;
          if(dz>0.5)  {t1->sp2[i]=t2->sp[i]; t1->tr2[i]=t2->tr[i];}
          if(dz<-0.5) {t1->sp2[i]=t1->sp[i]; t1->sp[i]=t2->sp[i]; t1->tr2[i]=t1->tr[i]; t1->tr[i]=t2->tr[i];}
        }
      }
    }
    t1->chi2+=t2->chi2;
    t2->deleted=true;
  }
  else {
    // Insert first track space points in second track
    for(int i=0; i<m_layMax; i++) {
      // Add first layer SP
      if(t2->sp[i]==0 && t1->sp[i]!=0) {
        t2->sp[i]=t1->sp[i];
        t2->tr[i]=t1->tr[i];
      }
      // Add second layer SP
      if(t2->sp[i]!=0 && t2->sp2[i]==0 && t1->sp[i]!=0) {
        // Barrel
        if(i<m_layPixEC) {
          float dr = t1->sp[i]->r()-t2->sp[i]->r();
          if(dr>0.5)  {t2->sp2[i]=t1->sp[i]; t2->tr2[i]=t1->tr[i];}
          if(dr<-0.5) {t2->sp2[i]=t2->sp[i]; t2->sp[i]=t1->sp[i]; t2->tr2[i]=t2->tr[i]; t2->tr[i]=t1->tr[i];}
        }
        // Endcap
        else {
          float dz = t1->sp[i]->z()-t2->sp[i]->z();
          if(t1->sp[i]->z()<0) dz=-dz;
          if(dz>0.5)  {t2->sp2[i]=t1->sp[i]; t2->tr2[i]=t1->tr[i];}
          if(dz<-0.5) {t2->sp2[i]=t2->sp[i]; t2->sp[i]=t1->sp[i]; t2->tr2[i]=t2->tr[i]; t2->tr[i]=t1->tr[i];}
        }
      }
    }
    t2->chi2 += t1->chi2;
    t1->deleted=true;
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigL2PattRecoStrategyB::trackAmbiguityFraction(StrBTrack* t1, StrBTrack* t2) {

  // Count identical spacepoints
  int total=0, common=0;
  bool first=true, commonFirst=false;
  for(int i=0; i<m_layMax; i++) {
    if(t1->sp[i]!=0 && t2->sp[i]!=0) total++;
    if(t1->sp[i]!=0 && t1->sp[i]==t2->sp[i]) {
      if(first) {commonFirst=true; first=false;}
      common++;
    }
    if(t1->sp2[i]!=0 && t1->sp2[i]==t2->sp2[i]) {
      common++;
    }
    if((t1->sp[i]!=0||t2->sp[i]!=0) && (t1->sp[i]!=t2->sp[i])) first=false;
  }

  // Evaluate fraction
  float ambiguity=0.0;
  if(total!=0) {
    ambiguity=((float)common)/total;
    if(m_extensionSolveV0Tracks && commonFirst) ambiguity += 1.0;
  }

  return ambiguity;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigL2PattRecoStrategyB::trackAmbiguityDistance(StrBTrack* t1, StrBTrack* t2) {

  // Scaling factors
  double scale;
  double dphi, deta;

  // Phi distance
  dphi = t1->phi - t2->phi;
  if(dphi>M_PI)  dphi = dphi - 2*M_PI;
  if(dphi<-M_PI) dphi = dphi + 2*M_PI;
  scale = m_extensionSolveTracksPhiScale;
  dphi /= scale;

  // Eta distance
  deta = t1->eta - t2->eta;
  scale = m_extensionSolveTracksEtaScale;
  deta /= scale;

  // Return squared distance
  double dr = dphi*dphi + deta*deta;
  return dr;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::trackAmbiguityShared(StrBTrack* t1, StrBTrack* t2) {
 
  // Count identical spacepoints
  if((*t1)<(*t2)) {
    for(int i=0; i<m_layMax; i++) {
      if(t1->sp[i]!=0  && t1->sp[i]==t2->sp[i])   t2->spShared[i]=1;
      if(t1->sp[i]!=0  && t1->sp[i]==t2->sp2[i])  t2->spShared[i]=1;
      if(t1->sp2[i]!=0 && t1->sp2[i]==t2->sp[i])  t2->spShared[i]=1;
      if(t1->sp2[i]!=0 && t1->sp2[i]==t2->sp2[i]) t2->spShared[i]=1;
    }
  }
  else {
    for(int i=0; i<m_layMax; i++) {
      if(t2->sp[i]!=0  && t2->sp[i]==t1->sp[i])   t1->spShared[i]=1;
      if(t2->sp[i]!=0  && t2->sp[i]==t1->sp2[i])  t1->spShared[i]=1;
      if(t2->sp2[i]!=0 && t2->sp2[i]==t1->sp[i])  t1->spShared[i]=1;
      if(t2->sp2[i]!=0 && t2->sp2[i]==t1->sp2[i]) t1->spShared[i]=1;
    }
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::trackMergeAmbiguity(StrBTrack* t1, StrBTrack* t2) {
 
  // Skip deleted
  if(t1->deleted || t2->deleted) return;

  // Check order and set flags
  if((*t1)<(*t2)) {
    // Insert second track space points in first track
    for(int i=0; i<m_layMax; i++) {
      // Add first layer SP
      if(t1->sp[i]==0 && t2->sp[i]!=0) {
        t1->sp[i]=t2->sp[i];
        t1->tr[i]=t2->tr[i];
      }
      // Add second layer SP
      if(t1->sp[i]!=0 && t1->sp2[i]==0 && t2->sp[i]!=0) {
        // Barrel
        if(i<m_layPixEC) {
          float dr = t2->sp[i]->r()-t1->sp[i]->r();
          if(dr>0.5)  {t1->sp2[i]=t2->sp[i]; t1->tr2[i]=t2->tr[i];}
          if(dr<-0.5) {t1->sp2[i]=t1->sp[i]; t1->sp[i]=t2->sp[i]; t1->tr2[i]=t1->tr[i]; t1->tr[i]=t2->tr[i];}
        }
        // Endcap
        else {
          float dz = t2->sp[i]->z()-t1->sp[i]->z();
          if(t2->sp[i]->z()<0) dz=-dz;
          if(dz>0.5)  {t1->sp2[i]=t2->sp[i]; t1->tr2[i]=t2->tr[i];}
          if(dz<-0.5) {t1->sp2[i]=t1->sp[i]; t1->sp[i]=t2->sp[i]; t1->tr2[i]=t1->tr[i]; t1->tr[i]=t2->tr[i];}
        }
      }
    }
    //t1->chi2+=t2->chi2;
    t2->deleted=true;
  }
  else {
    // Insert first track space points in second track
    for(int i=0; i<m_layMax; i++) {
      // Add first layer SP
      if(t2->sp[i]==0 && t1->sp[i]!=0) {
        t2->sp[i]=t1->sp[i];
        t2->tr[i]=t1->tr[i];
      }
      // Add second layer SP
      if(t2->sp[i]!=0 && t2->sp2[i]==0 && t1->sp[i]!=0) {
        // Barrel
        if(i<m_layPixEC) {
          float dr = t1->sp[i]->r()-t2->sp[i]->r();
          if(dr>0.5)  {t2->sp2[i]=t1->sp[i]; t2->tr2[i]=t1->tr[i];}
          if(dr<-0.5) {t2->sp2[i]=t2->sp[i]; t2->sp[i]=t1->sp[i]; t2->tr2[i]=t2->tr[i]; t2->tr[i]=t1->tr[i];}
        }
        // Endcap
        else {
          float dz = t1->sp[i]->z()-t2->sp[i]->z();
          if(t1->sp[i]->z()<0) dz=-dz;
          if(dz>0.5)  {t2->sp2[i]=t1->sp[i]; t2->tr2[i]=t1->tr[i];}
          if(dz<-0.5) {t2->sp2[i]=t2->sp[i]; t2->sp[i]=t1->sp[i]; t2->tr2[i]=t2->tr[i]; t2->tr[i]=t1->tr[i];}
        }
      }
    }
    //t2->chi2 += t1->chi2;
    t1->deleted=true;
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::trackRemoveAmbiguity(StrBTrack* t1, StrBTrack* t2) {
   
  // Skip deleted
  if(t1->deleted || t2->deleted) return;

  // Check order and set flags
  if((*t1)<(*t2)) {
    t2->deleted=true;
  }
  else {
    t1->deleted=true;
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigL2PattRecoStrategyB::fit(TrigInDetTrack* t) {

  // Temporary variables
  float xc, yc, rc, phic, q, pt;
  bool goodFit = true;

  /** PERFORM FIT. */
  if(t->siSpacePoints()->size()>2) {
    // Perform preliminaty RPhi fit
    circlefitRPhi(t, xc, yc, rc, phic, q, pt);
    // Perform extended RZ fit
    fitRZ(t, xc, yc, rc, phic, q, pt);
  }
  if(t->siSpacePoints()->size()>3) {
    // Perform extended RPhi fit
    goodFit = fitRPhi(t, xc, yc);
  }

  return goodFit;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::circlefitRPhi(TrigInDetTrack* t, float &xc, float &yc, float &rc, float &phic, float &q, float &pt) {
 
  /** Fitting variables. **/
  float phi=0, d0=0;
  float x[3], y[3];// z[3], r[3];
  float tr, cx1, cy1, cx2, cy2, xm1, ym1, xm2, ym2;
  float phi0, dph0p;

  /** Get space point indexes. **/
  float rsp, rval[3]={10000.0, 10000.0, 10000.0};
  int ind, rind[3]={0, 1, 2};
 
  std::vector<const TrigSiSpacePoint*>::iterator sp, spEnd=t->siSpacePoints()->end();
  for(sp=t->siSpacePoints()->begin(), ind=0; sp!=spEnd; sp++, ind++) {
    // Save indices for preliminary RPhi fit
    rsp = (*sp)->r();
    if(rsp<rval[0])                          {rval[0]=rsp; rind[0]=ind;}
    if(rsp>122.5 && (rsp-122.5)<rval[1])     {rval[1]=rsp-122.5; rind[1]=ind;}
    if(rsp>350.0 && (rsp-350.0)<rval[2])     {rval[2]=rsp-350.5; rind[2]=ind;}
  }
  if(rind[0]==rind[1] || rind[1]==rind[2] ||
     ((*(t->siSpacePoints()))[rind[1]]->r()-(*(t->siSpacePoints()))[rind[0]]->r())<1.0 ||
     ((*(t->siSpacePoints()))[rind[2]]->r()-(*(t->siSpacePoints()))[rind[1]]->r())<1.0) {rind[0]=0; rind[1]=1; rind[2]=t->siSpacePoints()->size()-1;}

  /** Get space point coordinates. */
  for(int i=0; i<3; i++) {
    x[i] = (*(t->siSpacePoints()))[rind[i]]->x();
    y[i] = (*(t->siSpacePoints()))[rind[i]]->y();
    //z[i] = (*(t->siSpacePoints()))[rind[i]]->z();
    // r[i] = (*(t->siSpacePoints()))[rind[i]]->r();
  }

  /** Evaluate useful quantities. */
  xm1 = (x[1]+x[0])/2;
  ym1 = (y[1]+y[0])/2;
  cx1 = y[1] - y[0];
  cy1 = x[0] - x[1];
       
  xm2 = (x[1]+x[2])/2;
  ym2 = (y[1]+y[2])/2;
  cx2 = y[1] - y[2];
  cy2 = x[2] - x[1];
       
  /** Check if coordinates need switching. */
  bool switched=false;
  double tmp;
  if(cy1==0 || cy2==0) {
    switched=true;
    tmp=xm1; xm1=ym1; ym1=tmp;
    tmp=xm2; xm2=ym2; ym2=tmp;
    tmp=cx1; cx1=-cy1; cy1=-tmp;
    tmp=cx2; cx2=-cy2; cy2=-tmp;
  }

  /** Check if track is too straight. */
  bool straight=false;
  if(cx1/cy1 == cx2/cy2) {
    straight=true;
  }
  else {
    // Evaluate circle radius
    tr = (xm2-xm1+(ym1-ym2)*cx2/cy2)/(cx1-cy1*cx2/cy2);
    xc = xm1 + cx1*tr;
    yc = ym1 + cy1*tr;
    if(switched) {tmp=xc; xc=yc; yc=tmp;}
    rc = sqrt((xc-x[0])*(xc-x[0])+(yc-y[0])*(yc-y[0]));
    pt = rc*1.042*9.0/15.0;
    if(pt>=1000000.0) straight=true;
  }

  /** Treat straight tracks. */
  if(straight) {
    phi = atan2(y[1]-y[0], x[1]-x[0]);
    if(phi<-M_PI) phi+=2*M_PI;
    if(phi>M_PI)  phi-=2*M_PI;
    pt = 1000000.0;
    xc = yc = rc = 0;
    float phiV = atan2(y[0], x[0]);
    d0 = sqrt(x[0]*x[0]+y[0]*y[0])*sin(phiV-phi);
  }
  /** Treat curved tracks. */
  else {
    phic = atan2(xc, -yc);
    d0 = sqrt(xc*xc+yc*yc)-rc;
    /** Evaluate charge. */
    phi0 = atan2(xc-x[0],-yc+y[0]);
    dph0p = phi0-phic;
    if (dph0p >  M_PI) dph0p = dph0p - 2*M_PI;
    if (dph0p < -M_PI) dph0p = dph0p + 2*M_PI;
    if (dph0p > 0) {
      q = -1;
    } else {
      q = 1;
    }
    pt = pt*q;
     
    /** Evaluate phi0. */
    phi = atan2(-xc, yc) + (1+q)/2*M_PI;
    if(phi<-M_PI) phi+=2*M_PI;
    if(phi>M_PI)  phi-=2*M_PI;
   
    /** Evaluate d0 sign. */
    float absd0  = fabs(d0);
    float phiV = atan2(y[0], x[0]);
    if(phiV<-M_PI) phiV+=2*M_PI;
    if(phiV> M_PI) phiV-=2*M_PI;
    float newsign = sin(phiV-phi);
    if (newsign<0){
      d0 = -absd0;
    } else {
      d0 =  absd0;
    }   
  }
   
  /** Save track parameters. */
  TrigInDetTrackFitPar* par = const_cast<TrigInDetTrackFitPar*>(t->param());
  par->a0(d0);
  par->phi0(phi);
  par->pT(pt);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::fitRZ(TrigInDetTrack* t, float &xc, float &yc, float &rc, float &phic, float &q, float &pt) {
       
  /** Fill sum variables. */
  // Sum variable
  float ang=0, z0=0, cotantheta=0;
  float n=0, rr=0, rz=0, r=0, z=0;
  float iphi, ir, iz, ezez;

  // Preliminary track parameters
  TrigInDetTrackFitPar* par = const_cast<TrigInDetTrackFitPar*>(t->param());
  float z1=(*(t->siSpacePoints()))[0]->z(), r1=(*(t->siSpacePoints()))[0]->r(), z2, r2;
  z2=z1; r2=r1;
  for(unsigned int nSP=1; nSP<t->siSpacePoints()->size(); nSP++) {
    z2=(*(t->siSpacePoints()))[nSP]->z(); r2=(*(t->siSpacePoints()))[1]->r();
    if((r2-r1)!=0 && (z2-z1)!=0) break;
  }

  int trackType=2; // 0=nonsense 1=straight 2=normal
  if((r2-r1)==0 && (z2-z1)==0) trackType=0;
  else if(pt>=1000000.0) trackType=1;

  if(trackType==0) { // NONSENSE TRACK
    par->eta(0);
    par->z0(0);
  }
  else if(trackType==1) { // STRAIGHT TRACK
    z0=z1-((z2-z1)/(r2-r1))*r1;
    par->z0(z0);
    float theta = atan2(r2-r1, z2-z1);
    if(theta<-M_PI) theta+=2*M_PI; if(theta>M_PI) theta-=2*M_PI; if(theta<0) theta=M_PI+theta;
    float tanHalfTheta=tan(0.5*theta);
    float eta=0;   
    if(tanHalfTheta!=0)
      eta = -log(tanHalfTheta);
    else {
      if((z2-z1)>0) eta=4.0;
      else eta=-4.0;
    }
    par->eta(eta);
  }
  else { // NORMAL TRACK

    // Loop on SPs
    cotantheta = (z2-z1)/(r2-r1);
    std::vector<const TrigSiSpacePoint*>::iterator sp, spEnd=t->siSpacePoints()->end();
    for(sp=t->siSpacePoints()->begin(); sp!=spEnd; sp++) {
      // Evaluate error on Z
      ezez = ((*sp)->dz()*(*sp)->dz()) + (cotantheta*cotantheta)*((*sp)->dr()*(*sp)->dr());
      // Evaluate sum variables
      iphi = atan2(xc-(*sp)->x(), -yc+(*sp)->y());
      ir = iphi-phic; if(ir>M_PI) ir=ir-2*M_PI; if(ir<-M_PI) ir=ir+2*M_PI;
      iz = (*sp)->z();
      n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
    }

    /** Evaluate line parameters. */
    float invdelta, z0;
    invdelta = 1/((double)n*rr-(double)r*r);
    z0       = invdelta*((double)rr*z-(double)r*rz);
    ang      = invdelta*((double)n*rz-(double)r*z);
   
    /** Evaluate track parameters. */
    float theta = atan2(-q*rc, ang);
    if(theta<-M_PI) theta+=2*M_PI; if(theta>M_PI) theta-=2*M_PI; if(theta<0) theta=M_PI+theta;
    float tanHalfTheta=tan(0.5*theta);
    float eta=0;   
    if(tanHalfTheta!=0)
      eta = -log(tanHalfTheta);
    else {
      if(-q*rc*ang>0) eta=4.0;
      else eta=-4.0;
    }

    par->eta(eta);
    par->z0(z0);
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigL2PattRecoStrategyB::fitRPhi(TrigInDetTrack* t, float &xc, float &yc) {

  /** Fill sum variables. */
  float infinite=666666666666.666;
  float Cxx, Cxy, Cyy, Cxrr, Cyrr, Crrrr, q1, q2;
  float Sw=0, x=0, y=0, xx=0, xy=0, yy=0, xrr=0, yrr=0, rr=0, rrrr=0;
  float ix, iy, ir, iphi, iphic, idr, idphi, ee;
  float phiref, phi, phi1, phi2, phidiff1, phidiff2, rho, d0;
  float k, delta;

  // Preliminary track parameters
  TrigInDetTrackFitPar* par = const_cast<TrigInDetTrackFitPar*>(t->param());
  phiref = phi = phi1 = phi2 = par->phi0();

  // Loop on SPs
  for(unsigned int i=0; i<t->siSpacePoints()->size(); i++) {
    // Get coordinates
    ix     = (*t->siSpacePoints())[i]->x();
    iy     = (*t->siSpacePoints())[i]->y();
    ir     = (*t->siSpacePoints())[i]->r();
    iphi   = (*t->siSpacePoints())[i]->phi();
    iphic  = atan2(iy-yc, ix-xc);
    idr    = (*t->siSpacePoints())[i]->dr();
    idphi  = (*t->siSpacePoints())[i]->dphi();

    // Evaluate error
    ee = (ir*ir*idphi*idphi*cos(M_PI/2+iphi-iphic)*cos(M_PI/2+iphi-iphic)) +
      (idr*idr*sin(M_PI/2+iphi-iphic)*sin(M_PI/2+iphi-iphic));
    // Evaluate sum variables
    Sw+=1/ee; x+=ix/ee; y+=iy/ee; xx+=(ix*ix)/ee; xy+=(ix*iy)/ee; yy+=(iy*iy)/ee; xrr+=(ix*ir*ir)/ee; yrr+=(iy*ir*ir)/ee; rr+=(ir*ir)/ee; rrrr+=(ir*ir*ir*ir)/ee;
  }
  // Divide by weight sum
  x/=Sw; y/=Sw; xx/=Sw; xy/=Sw; yy/=Sw; xrr/=Sw; yrr/=Sw; rr/=Sw; rrrr/=Sw;

  /** Evaluate circle parameters. */
  Cxx=xx-x*x; Cxy=xy-x*y; Cyy=yy-y*y; Cxrr=xrr-x*rr; Cyrr=yrr-y*rr; Crrrr=rrrr-rr*rr;
  q1=(Crrrr*Cxy)-(Cxrr*Cyrr); q2=Crrrr*(Cxx-Cyy)-(Cxrr*Cxrr)+(Cyrr*Cyrr);
 
  float signx = cos(phiref)/fabs(cos(phiref)); float signy = sin(phiref)/fabs(sin(phiref));

  if(q2==0) phi=0.5*atan2(signy*infinite, signx);
  else phi=0.5*atan2(signy*2*q1/q2, signx);
  if(phi>M_PI) phi-=2*M_PI; else if(phi<-M_PI) phi+=2*M_PI;
 
  if(signx<0 && signy>=0) {
    phi1 = M_PI/2 - phi;
    phi2 = M_PI   - phi;
  }
  else if(signx>=0 && signy<0) {
    phi1 = 3*M_PI/2-phi;
    phi2 = -phi;
  }
  else if(signx>=0 && signy>=0) {
    phi1 = M_PI/2+phi;
    phi2 = phi;
  }
  else if(signx<0 && signy<0) {
    phi1 = 3*M_PI/2+phi;
    phi2 = M_PI+phi;
  }

  if(phi1>M_PI) phi1-=2*M_PI; else if(phi1<-M_PI) phi1+=2*M_PI;
  if(phi2>M_PI) phi2-=2*M_PI; else if(phi2<-M_PI) phi2+=2*M_PI;
  phidiff1 = phi1-phiref; if(phidiff1>M_PI) phidiff1-=2*M_PI; else if(phidiff1<-M_PI) phidiff1+=2*M_PI; phidiff1=fabs(phidiff1);
  phidiff2 = phi2-phiref; if(phidiff2>M_PI) phidiff2-=2*M_PI; else if(phidiff2<-M_PI) phidiff2+=2*M_PI; phidiff2=fabs(phidiff2);
  if(phidiff1>0.5 && phidiff2>0.5) return false;
  if(phidiff1<phidiff2) phi=phi1; else phi=phi2;
 
  if(Crrrr==0) k=infinite;
  else k =(sin(phi)*Cxrr-cos(phi)*Cyrr)/Crrrr;
  delta  = -k*rr + sin(phi)*x - cos(phi)*y;
  rho    = 2*k/sqrt(1-4*delta*k);
  d0     = -2*delta/(1+sqrt(1-4*delta*k));

  /** Evaluate track parameters. */
  par->phi0(phi);
  par->a0(d0);
  par->pT(1.042*(1/rho)*9.0/15.0);

  return true;
}       


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigL2PattRecoStrategyB::shiftRPhi(TrigInDetTrack* t) {

  /** SHIFT FINAL TRACK D0 VALUE. */
  // Get original d0 value
  TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>(t->param());
  float oldd0 = param->a0();
  float phi0 = param->phi0();

  // Evaluate new d0 value
  float newd0 = oldd0 - m_xc*sin(phi0) + m_yc*cos(phi0);
  param->a0(newd0);
}
