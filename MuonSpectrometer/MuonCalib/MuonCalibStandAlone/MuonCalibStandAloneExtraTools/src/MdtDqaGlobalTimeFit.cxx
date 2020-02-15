/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 05.09.2008, AUTHOR: MAURO IODICE
// 20.03.2009, MODIFICATIONS: DAN LEVIN  ==> allow endcap
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS MdtDqaGlobalTimeFit      ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

// MuonReadoutGeometry //
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MdtCalibInterfaces/IMdtSegmentFitter.h"

#include "MuonCalibStandAloneExtraTools/HistogramManager.h"

#include "MuonCalibStandAloneExtraTools/PhiEtaUtils.h"

// MuonCalib //
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"

// Rt stuff   ------
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibUtils/GlobalTimeFitter.h"

#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/RtFullInfo.h"
#include "MdtCalibData/MdtCalibrationFactory.h"

#include "MdtCalibFitters/LocalSegmentResolver.h"

//MdtCalibFitters
#include "MdtCalibFitters/DCSLFitter.h"

// MdtDqaGlobalTimeFit      //
#include "MuonCalibStandAloneExtraTools/MdtDqaGlobalTimeFit.h"

//this
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//root
#include "TFile.h"
#include "TH1.h"
#include "TNtuple.h"
#include "TString.h"
#include "TDirectory.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

namespace MuonCalib {

//*****************************************************************************

MdtDqaGlobalTimeFit::MdtDqaGlobalTimeFit(int nbHitsMin, int nbHitsMax, float chi2cut, int BfieldON,  bool debug) :
  m_detMgr(nullptr),
  m_id_tool(nullptr),
  p_reg_sel_svc(nullptr),
  m_histoManager(nullptr),
  m_rtRel(nullptr),
  m_muFitter(nullptr),
  m_GTFitter(nullptr)
{
  m_minNumHits = nbHitsMin;
  m_maxNumHits = nbHitsMax;
  m_chi2_cut = chi2cut;
  m_BfieldON = BfieldON;
  m_debug = debug;
}
  
//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::
  
StatusCode MdtDqaGlobalTimeFit::initialize(const MuonGM::MuonDetectorManager *detMgr, 
					   const MuonCalib::IIdToFixedIdTool *id_tool, RegionSelectionSvc *reg_sel_svc,
					   HistogramManager *histoManager) {

  m_detMgr = detMgr; 
  m_id_tool = id_tool;
  p_reg_sel_svc = reg_sel_svc;
  m_histoManager = histoManager;

  m_muFitter = new DCSLFitter();

  m_rtRel = getDefaultRtRelation(m_BfieldON);
  m_GTFitter = new GlobalTimeFitter(m_muFitter, m_rtRel);

  return StatusCode::SUCCESS;
}  //end MdtDqaGlobalTimeFit::initialize

//*****************************************************************************

  //::::::::::::::::::::::::
  //:: METHOD handleEvent ::
  //::::::::::::::::::::::::

StatusCode MdtDqaGlobalTimeFit::handleEvent( const MuonCalibEvent & /*event*/,
					     int /*eventnumber*/,
					     const std::vector<MuonCalibSegment *> &segments, 
					     unsigned int position) {

  if (segments.size()<=position) return StatusCode::SUCCESS;
  for (unsigned int k=position; k<segments.size(); k++) {    // LOOP OVER SEGMENTS 
    
    MuonCalibSegment segment(*segments[k]);
 
    //---------------//
    //-- Variables --//
    //---------------//

    int nhits = (int) segment.mdtHitsOnTrack();
    bool REFIT_GTFIT = true;    // THIS SHOULD BE PASSED VIA JOBOPTIONS !!!!!!!!!!!!!    
    // station identifiers //
  
    MuonFixedId Mid((segment.mdtHOT()[0])->identify());
     
    //this has to be set in order to get the m_detMgr->getMdtReadoutElement() method working correctly
    //otherwise also for the first multilayer, the second is returned
    Mid.setMdtMultilayer(1);

    //----------//
    //-- Hits --//
    //----------//
    
    if(nhits<m_minNumHits || nhits>m_maxNumHits) continue;

    //
    // Get REGION and STATION of the segment :
    // 
    // and access to MdtDqa histograms 
    //
    // TFile * mdtDqaRoot =  m_histoManager->rootFile();
    ToString ts;
    int stationNameId = Mid.stationName();
    int phi  = Mid.phi();
    int eta  = Mid.eta();
    int absEta = eta;
    if (eta<0) absEta=-eta;
    std::string stationNameStr = Mid.stationNameString();

    //
    // HERE WE LIMIT THE ANALYSIS TO BM(L,S) and EM(L,S) CHAMBERS : 
    //
    if ( !(stationNameStr.substr(0,2)=="BM" || stationNameStr.substr(0,2)=="EM") ) continue;

    std::string region = "Barrel";
    if ( stationNameStr.substr(0,1) == "E" ) region = "Endcap";
    std::string side = "A";
    if (eta<0) side = "C";

    PhiEtaNameConverter phiEtaConverter;
    std::string chamberType = stationNameStr;

    std::string fullStationName = chamberType+"_"+ts(phi)+"_"+ts(eta);

    int sector = phiEtaConverter.phi_8to16(stationNameId,phi);

    TH1F *h1;
    TH2F *h2;

    float toffset(999.);
    if ( REFIT_GTFIT ) toffset =m_GTFitter->GTFit(&segment);
    
    float SegChi2DoF = segment.chi2();
    int DoF = segment.mdtHitsOnTrack() - 2;
    if ( REFIT_GTFIT ) DoF = DoF-1;
    float SegChi2 = SegChi2DoF*((float)DoF);
    

    if (SegChi2DoF<=m_chi2_cut){ // chi2DoF cut
      // get MDT coordinates from first hit 
      // (this can be changed in future by averaging over the hit for example)
 
      // TRY THE FOLLOWING 
      float distRO = (segment.mdtHOT()[0])->distanceToReadout();  // along the tube
      float tube   = (float) ((segment.mdtHOT()[0])->identify()).mdtTube();

      // FILLING Histograms 
      //
      std::string histoType;

      std::string station = "undefined";
      if ( region == "Barrel" )station = "BM";
      if ( region == "Endcap" )station = "EM";

      histoType="TrigTime_"+station+"_eta_"+ts(absEta);
      h1 = (TH1F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h1) {
	h1->Fill(toffset);
      } 

      histoType="TrigTimeVsROdistance_"+station+"_eta_"+ts(absEta);

      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h2) h2->Fill(distRO,toffset);

      histoType="TriggerCoverage_"+station+"_eta_"+ts(absEta);
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h2) h2->Fill(distRO,tube);

      histoType="Segment_chi2";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h2) h2->Fill(SegChi2DoF,absEta);

      histoType="Segment_ProbChi2";
      h1 = (TH1F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h1) h1->Fill(TMath::Prob(SegChi2,DoF));

      histoType="HitsOnSegment";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h2) h2->Fill(segment.mdtHitsOnTrack(),absEta);

      histoType="HitRadius";
      h1 = (TH1F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      for (std::vector<MdtCalibHitBase*>::iterator it =segment.mdtHOTBegin();
	   it!=segment.mdtHOTEnd();++it) {

	float radius=(*it)->driftRadius();

	if ((*it)->signedDistanceToTrack()<0.) radius=-radius;
	if (h1) h1->Fill(radius);
      }
      
      histoType="Residuals";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      for (std::vector<MdtCalibHitBase*>::iterator it =segment.mdtHOTBegin();
	   it!=segment.mdtHOTEnd();++it) {
	float resid=(*it)->radialResidual();
	if (h2) h2->Fill(resid,absEta);
      }

      histoType="ResidualsVsRadius";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      for (std::vector<MdtCalibHitBase*>::iterator it =segment.mdtHOTBegin();
	   it!=segment.mdtHOTEnd();++it) {
	float resid=(*it)->radialResidual();
	float radius=(*it)->driftRadius();
	if ((*it)->signedDistanceToTrack()<0.) radius=-radius;
	if (h2) h2->Fill(radius,resid);
      }

    } // End if over chi2DoF
  } // end LOOP OVER SEGMENTS

  return StatusCode::SUCCESS;
}  //end MdtDqaGlobalTimeFit::handleEvent

//*****************************************************************************

  //::::::::::::::::::::::::::::
  //:: METHOD analyseSegments ::
  //::::::::::::::::::::::::::::
StatusCode MdtDqaGlobalTimeFit::analyseSegments(const std::vector<MuonCalibSegment *> & /*segments*/) {
  return StatusCode::SUCCESS;
}

IRtRelation *MdtDqaGlobalTimeFit::getDefaultRtRelation(int BfieldON ) {
  MuonCalib::CalibFunc::ParVec rtPars;
  rtPars.push_back( -50. );   // t_Start 
  rtPars.push_back( 8. );     // t_binSize

   // B field is OFF :
  if ( BfieldON == 0) {       
    rtPars.push_back(0.2);        // r(-50)
    rtPars.push_back(0.2);          // r(-50+8)
    rtPars.push_back(0.2);          // ...
    rtPars.push_back(0.25);
    rtPars.push_back(0.294403);
    rtPars.push_back(0.311703);
    rtPars.push_back(0.485873);
    rtPars.push_back(0.804235);
    rtPars.push_back(1.19624);
    rtPars.push_back(1.5562);
    rtPars.push_back(1.91466);
    rtPars.push_back(2.3147);
    rtPars.push_back(2.66996);
    rtPars.push_back(3.05436);
    rtPars.push_back(3.40371);
    rtPars.push_back(3.80574);
    rtPars.push_back(4.13506);
    rtPars.push_back(4.45473);
    rtPars.push_back(4.77338);
    rtPars.push_back(5.05294);
    rtPars.push_back(5.33755);
    rtPars.push_back(5.58459);
    rtPars.push_back(5.8718);
    rtPars.push_back(6.09389);
    rtPars.push_back(6.37129);
    rtPars.push_back(6.56824);
    rtPars.push_back(6.73359);
    rtPars.push_back(6.96923);
    rtPars.push_back(7.16944);
    rtPars.push_back(7.38872);
    rtPars.push_back(7.55627);
    rtPars.push_back(7.73885);
    rtPars.push_back(7.86403);
    rtPars.push_back(8.07223);
    rtPars.push_back(8.26818);
    rtPars.push_back(8.39531);
    rtPars.push_back(8.54069);
    rtPars.push_back(8.69377);
    rtPars.push_back(8.86487);
    rtPars.push_back(9.01724);
    rtPars.push_back(9.16516);
    rtPars.push_back(9.24791);
    rtPars.push_back(9.45596);
    rtPars.push_back(9.56237);
    rtPars.push_back(9.69553);
    rtPars.push_back(9.80647);
    rtPars.push_back(9.9432);
    rtPars.push_back(10.039);
    rtPars.push_back(10.207);
    rtPars.push_back(10.2884);
    rtPars.push_back(10.3977);
    rtPars.push_back(10.6045);
    rtPars.push_back(10.6181);
    rtPars.push_back(10.7883);
    rtPars.push_back(10.9057);
    rtPars.push_back(10.9908);
    rtPars.push_back(11.1615);
    rtPars.push_back(11.2359);
    rtPars.push_back(11.3409);
    rtPars.push_back(11.4451);
    rtPars.push_back(11.5412);
    rtPars.push_back(11.6896);
    rtPars.push_back(11.7487);
    rtPars.push_back(11.8853);
    rtPars.push_back(11.9571);
    rtPars.push_back(12.0381);
    rtPars.push_back(12.1739);
    rtPars.push_back(12.2601);
    rtPars.push_back(12.3892);
    rtPars.push_back(12.4406);
    rtPars.push_back(12.5809);
    rtPars.push_back(12.6297);
    rtPars.push_back(12.7495);
    rtPars.push_back(12.8314);
    rtPars.push_back(12.9463);
    rtPars.push_back(13.0487);
    rtPars.push_back(13.1337);
    rtPars.push_back(13.2417);
    rtPars.push_back(13.2706);
    rtPars.push_back(13.3917);
    rtPars.push_back(13.5107);
    rtPars.push_back(13.60);
    rtPars.push_back(13.69);
    rtPars.push_back(13.71);
    rtPars.push_back(13.8088);
    rtPars.push_back(13.8577);
    rtPars.push_back(14.0212);
    rtPars.push_back(14.0671);
    rtPars.push_back(14.1225);
    rtPars.push_back(14.2112);
    rtPars.push_back(14.3113);
    rtPars.push_back(14.3747);
    rtPars.push_back(14.3531);
    rtPars.push_back(14.4799);
    rtPars.push_back(14.458);
    rtPars.push_back(14.516);
    rtPars.push_back(14.8312);
    rtPars.push_back(14.85);
    rtPars.push_back(14.86);
    rtPars.push_back(14.87);
    
  }
  // B field is ON :
  if ( BfieldON == 1) {   
    rtPars.push_back(0.2);
    rtPars.push_back(0.2);
    rtPars.push_back(0.2);
    rtPars.push_back(0.22);
    rtPars.push_back(0.27);
    rtPars.push_back(0.33);
    rtPars.push_back(0.37);
    rtPars.push_back(0.460902);
    rtPars.push_back(0.805728);
    rtPars.push_back(1.10767);
    rtPars.push_back(1.53996);
    rtPars.push_back(1.9101);
    rtPars.push_back(2.36668);
    rtPars.push_back(2.71068);
    rtPars.push_back(3.06689);
    rtPars.push_back(3.41867);
    rtPars.push_back(3.78364);
    rtPars.push_back(4.11741);
    rtPars.push_back(4.41084);
    rtPars.push_back(4.73491);
    rtPars.push_back(5.02635);
    rtPars.push_back(5.24373);
    rtPars.push_back(5.5519);
    rtPars.push_back(5.80729);
    rtPars.push_back(5.98834);
    rtPars.push_back(6.26835);
    rtPars.push_back(6.42993);
    rtPars.push_back(6.62648);
    rtPars.push_back(6.85403);
    rtPars.push_back(6.99663);
    rtPars.push_back(7.21787);
    rtPars.push_back(7.32455);
    rtPars.push_back(7.64958);
    rtPars.push_back(7.73685);
    rtPars.push_back(7.90371);
    rtPars.push_back(8.0533);
    rtPars.push_back(8.18414);
    rtPars.push_back(8.44938);
    rtPars.push_back(8.60877);
    rtPars.push_back(8.65394);
    rtPars.push_back(8.84461);
    rtPars.push_back(8.95214);
    rtPars.push_back(9.14024);
    rtPars.push_back(9.24901);
    rtPars.push_back(9.37823);
    rtPars.push_back(9.5724);
    rtPars.push_back(9.60793);
    rtPars.push_back(9.74432);
    rtPars.push_back(9.89992);
    rtPars.push_back(10.0009);
    rtPars.push_back(10.1321);
    rtPars.push_back(10.187);
    rtPars.push_back(10.3244);
    rtPars.push_back(10.4858);
    rtPars.push_back(10.6038);
    rtPars.push_back(10.7109);
    rtPars.push_back(10.8385);
    rtPars.push_back(10.8563);
    rtPars.push_back(10.977);
    rtPars.push_back(11.0865);
    rtPars.push_back(11.2767);
    rtPars.push_back(11.3279);
    rtPars.push_back(11.4734);
    rtPars.push_back(11.5325);
    rtPars.push_back(11.6352);
    rtPars.push_back(11.793);
    rtPars.push_back(11.8545);
    rtPars.push_back(11.9321);
    rtPars.push_back(12.0367);
    rtPars.push_back(12.091);
    rtPars.push_back(12.1884);
    rtPars.push_back(12.3246);
    rtPars.push_back(12.3793);
    rtPars.push_back(12.5348);
    rtPars.push_back(12.6328);
    rtPars.push_back(12.7038);
    rtPars.push_back(12.7699);
    rtPars.push_back(12.9211);
    rtPars.push_back(12.9866);
    rtPars.push_back(13.0501);
    rtPars.push_back(13.1832);
    rtPars.push_back(13.2441);
    rtPars.push_back(13.2971);
    rtPars.push_back(13.4107);
    rtPars.push_back(13.562);
    rtPars.push_back(13.5098);
    rtPars.push_back(13.6794);
    rtPars.push_back(13.7281);
    rtPars.push_back(13.8953);
    rtPars.push_back(13.9238);
    rtPars.push_back(14.0338);
    rtPars.push_back(14.0898);
    rtPars.push_back(14.1931);
    rtPars.push_back(14.2 );
    rtPars.push_back(14.4053);
    rtPars.push_back(14.41 );
    rtPars.push_back(14.4481);
    rtPars.push_back(14.4606);
    rtPars.push_back(14.49);
    rtPars.push_back(14.5283);
  }    

  IRtRelation *rt = MuonCalib::MdtCalibrationFactory::createRtRelation( "RtRelationLookUp", rtPars );

  return rt;
}  //end MdtDqaGlobalTimeFit::getDefaultRtRelation

} // namespace MuonCalib 
