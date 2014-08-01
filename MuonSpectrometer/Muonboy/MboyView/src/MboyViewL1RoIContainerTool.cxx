/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * $Id: MboyViewL1RoIContainerTool.cxx 482567 2012-02-14 08:02:17Z sizun $
 * @author apolici
 */
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewL1RoIContainerTool.h"

static const InterfaceID IID_IMboyViewL1RoIContainerTool("MboyViewL1RoIContainerTool", 1, 0);

const InterfaceID& MboyViewL1RoIContainerTool::interfaceID()
{
  return IID_IMboyViewL1RoIContainerTool;
}

MboyViewL1RoIContainerTool::MboyViewL1RoIContainerTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewL1RoIContainerTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   


}

MboyViewL1RoIContainerTool::~MboyViewL1RoIContainerTool(){}

// Initialize
StatusCode MboyViewL1RoIContainerTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewL1RoIContainerTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewL1RoIContainerTool::ViewL1RoIContainer(
               std::string L1RoIContainerLocation,
               std::ofstream* pOut
){

//Access
  if (!(evtStore()->contains<LVL1_ROI>(L1RoIContainerLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << L1RoIContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  StatusCode sc = StatusCode::SUCCESS;

  sc = evtStore()->retrieve(lvl1Roi, L1RoIContainerLocation);
  if (sc.isFailure()) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( "lvl1Roi not found at " << L1RoIContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }



//Dump

  //muon RoIs
  double l1muonroi_thr = -99.;
  double l1muonroi_eta = 0.;
  double l1muonroi_phi = 0.;
  LVL1_ROI::muons_type::const_iterator muonit = (lvl1Roi->getMuonROIs()).begin();
  LVL1_ROI::muons_type::const_iterator muonit_end = (lvl1Roi->getMuonROIs()).end();
  for ( ; muonit != muonit_end ; ++muonit ) {
      l1muonroi_thr = (*muonit).pt();
      l1muonroi_eta = (*muonit).eta();
      l1muonroi_phi = ((*muonit).phi())*57.2957;
      if(l1muonroi_phi > 0.){
        l1muonroi_phi = l1muonroi_phi;
      }
      if(l1muonroi_phi < 0.){
        l1muonroi_phi = l1muonroi_phi + 360;
      }
      // Write Out
      if(l1muonroi_thr!=-99.) {
        *pOut << "ROI"     << " "
              << "L1Muon" << " "
              << std::setw(12)<<std::setprecision(4) << l1muonroi_eta              << " "
              << std::setw(12)<<std::setprecision(4) << l1muonroi_phi              << " "
              << std::setw(12)<<std::setprecision(4) << l1muonroi_thr/1000.        << " "  //GeV
              << std::endl;
    }
  }
  //emtau ROIS
  double l1emtauroi_thr = -99.;
  double l1emtauroi_eta = 0.;
  double l1emtauroi_phi = 0.;
  LVL1_ROI::emtaus_type::const_iterator tauItr = (lvl1Roi->getEmTauROIs()).begin();
  LVL1_ROI::emtaus_type::const_iterator tauEnd = (lvl1Roi->getEmTauROIs()).end();
  for(; tauItr != tauEnd; ++tauItr) {
      l1emtauroi_thr = (*tauItr).pt();
      l1emtauroi_eta = (*tauItr).eta();
      l1emtauroi_phi = ((*tauItr).phi())*57.2957;
      if(l1emtauroi_phi > 0.){
        l1emtauroi_phi = l1emtauroi_phi;
      }
      if(l1emtauroi_phi < 0.){
        l1emtauroi_phi = l1emtauroi_phi + 360;
      }
      // Write Out
      if(l1emtauroi_thr!=-99.) {
        *pOut << "ROI"     << " "
              << "L1EMTau" << " "
              << std::setw(12)<<std::setprecision(4) << l1emtauroi_eta              << " "
              << std::setw(12)<<std::setprecision(4) << l1emtauroi_phi              << " "
              << std::setw(12)<<std::setprecision(4) << l1emtauroi_thr/1000.        << " " //GeV
              << std::endl;
   }
  }
  //jet ROIS
  double l1jetroi_thr = -99.;
  double l1jetroi_eta = 0.;
  double l1jetroi_phi = 0.;
  LVL1_ROI::jets_type::const_iterator jetItr = (lvl1Roi->getJetROIs()).begin();
  LVL1_ROI::jets_type::const_iterator jetEnd = (lvl1Roi->getJetROIs()).end();
  for(; jetItr != jetEnd; ++jetItr) {
      l1jetroi_thr = (*jetItr).pt();
      l1jetroi_eta = (*jetItr).eta();
      l1jetroi_phi = ((*jetItr).phi())*57.2957;
      if(l1jetroi_phi > 0.){
        l1jetroi_phi = l1jetroi_phi;
      }
      if(l1jetroi_phi < 0.){
        l1jetroi_phi = l1jetroi_phi + 360;
      }
      // Write Out
      if(l1emtauroi_thr!=-99.) {
        *pOut << "ROI"     << " "
              << "L1Jet" << " "
              << std::setw(12)<<std::setprecision(4) << l1jetroi_eta              << " "
              << std::setw(12)<<std::setprecision(4) << l1jetroi_phi              << " "
              << std::setw(12)<<std::setprecision(4) << l1jetroi_thr/1000.        << " " //GeV
              << std::endl;
    }
  }

  return StatusCode::SUCCESS;
  
}

