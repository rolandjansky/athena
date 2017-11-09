/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: GetLCDeadMaterialTree.cxx,v 1.3 2009-05-18 20:31:52 pospelov Exp $
//
// Description: see GetLCDeadMaterialTree.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Gennady Pospelov
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloLocalHadCalib/GetLCDeadMaterialTree.h"

//---------------
// C++ Headers --
//---------------
#include "PathResolver/PathResolver.h"
#include "AthenaKernel/errorcheck.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GeneratorObjects/McEventCollection.h"
#include <CLHEP/Units/SystemOfUnits.h>
#include "CaloConditions/CaloLocalHadCoeff.h"
#include "CaloLocalHadCalib/CaloLocalHadCoeffHelper.h"
#include "CaloLocalHadCalib/CaloHadDMCoeffData.h"
#include "CaloEvent/CaloCompositeKineBase.h"
#include "CaloEvent/CaloRecoStatus.h"
//#include "CaloEvent/CaloClusterMoment.h"
#include "TBEvent/TBEventInfo.h"
#include "CaloLocalHadCalib/GetLCSinglePionsPerf.h"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include <iterator>
#include <cmath>

using CLHEP::MeV;
using CLHEP::TeV;


//###############################################################################
GetLCDeadMaterialTree::GetLCDeadMaterialTree(const std::string& name, 
                           ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_HadDMCoeffInitFile("CaloHadDMCoeff_init_v2.txt"),
    m_outputTree(0),
    m_outputFileName("DeadMaterialTree.root"),
    m_outputFile(0),
    m_clusterCollName("CaloTopoCluster"),
    m_HadDMCoeff(0),
    m_data(0),
    m_doSaveCalibClusInfo(false),
    m_isTestbeam(false),
    m_energyMin(200*MeV),
    //m_energyMax(2*TeV),
    m_calo_dd_man(0),
    m_calo_id(0)
{

  // dead material zone description
  declareProperty("HadDMCoeffInitFile",m_HadDMCoeffInitFile);

  // Name of output file to save histograms in
  declareProperty("OutputFileName",m_outputFileName);

  // Name of ClusterContainer (uncalibrated) to use
  declareProperty("ClusterCollectionName",m_clusterCollName); 

  // Name of ClusterContainer (calibrated) to use
  declareProperty("ClusterCollectionNameCalib",m_clusterCollNameCalib); 

  // to work with testbeam data
  declareProperty("isTestbeam",m_isTestbeam);

  // to save additional info from the collection with calibrated clusters
  declareProperty("doSaveCalibClusInfo", m_doSaveCalibClusInfo);

}



/* ****************************************************************************

***************************************************************************** */
GetLCDeadMaterialTree::~GetLCDeadMaterialTree()
{
  if( m_HadDMCoeff ) delete m_HadDMCoeff;
}



/* ****************************************************************************

***************************************************************************** */
StatusCode GetLCDeadMaterialTree::initialize()
{
  //---- initialize the StoreGateSvc ptr ----------------

  // pointer to detector manager:
  m_calo_dd_man = CaloDetDescrManager::instance(); 
  m_calo_id = m_calo_dd_man->getCaloCell_ID();

  /* ********************************************
  set list of valid moments
  ******************************************** */
  moment_name_vector validNames;
  validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_EMB0"),xAOD::CaloCluster::ENG_CALIB_DEAD_EMB0));
  validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_TILE0"),xAOD::CaloCluster::ENG_CALIB_DEAD_TILE0));
  validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_TILEG3"),xAOD::CaloCluster::ENG_CALIB_DEAD_TILEG3));
  validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_EME0"),xAOD::CaloCluster::ENG_CALIB_DEAD_EME0));
  validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_HEC0"),xAOD::CaloCluster::ENG_CALIB_DEAD_HEC0));
  validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_FCAL"),xAOD::CaloCluster::ENG_CALIB_DEAD_FCAL));
  validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_LEAKAGE"),xAOD::CaloCluster::ENG_CALIB_DEAD_LEAKAGE));
  validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_UNCLASS"),xAOD::CaloCluster::ENG_CALIB_DEAD_UNCLASS));

  /* ********************************************
  initial coefficients
  ******************************************** */
  CaloLocalHadCoeffHelper dmHelper;
  std::string fileName = PathResolver::find_file (m_HadDMCoeffInitFile, "DATAPATH");
  m_HadDMCoeff = dmHelper.InitDataFromFile(fileName.c_str());
  if( !m_HadDMCoeff ) {
    msg(MSG::FATAL)
        << " Error while initializing default dead material coefficients "
        << endmsg;
    return StatusCode::FAILURE;
  }
  // how we have to set correspondance between dead material areas and calibration moments
  m_momentForDMArea.resize( m_HadDMCoeff->getSizeAreaSet());
  for(int i_dm=0; i_dm<m_HadDMCoeff->getSizeAreaSet(); i_dm++){
    moment_name_vector::const_iterator mVNameIter = validNames.begin(); 
    moment_name_vector::const_iterator mVNameIterEnd = validNames.end();
    bool isValid(false);
    for(; mVNameIter!=mVNameIterEnd; mVNameIter++) {
      if ( m_HadDMCoeff->getArea(i_dm)->getTitle() == mVNameIter->first ) {
        m_momentForDMArea[i_dm] = mVNameIter->second;
        isValid = true;
        break;
      }
    }
    if ( !isValid) {
      msg(MSG::FATAL)
          << " Unknown moment name '" << m_HadDMCoeff->getArea(i_dm)->getTitle() << "' in the m_HadDMCoeff!"
          << endmsg;
      return StatusCode::FAILURE;
    }
  }

  /* ********************************************
  output file&tree
  ******************************************** */
  m_data = new CaloHadDMCoeffData(0);

  m_outputFile = new TFile(m_outputFileName.c_str(),"RECREATE");
  m_outputFile->cd();

  m_outputTree = m_data->MakeTree("DeadMaterialTree");

  return StatusCode::SUCCESS;
}



/* ****************************************************************************

***************************************************************************** */
StatusCode GetLCDeadMaterialTree::finalize()
{

  msg(MSG::INFO) << "Writing out tree" << endmsg;
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();

  return StatusCode::SUCCESS;
}



/* ****************************************************************************

***************************************************************************** */
StatusCode GetLCDeadMaterialTree::execute()
{

  //bool useLink = true;

  /* ********************************************
  access to cluster container
  ******************************************** */
  const DataHandle<xAOD::CaloClusterContainer> pClusColl;
  StatusCode sc = evtStore()->retrieve(pClusColl,m_clusterCollName);
  if(sc != StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Could not retrieve ClusterContainer " 
        << m_clusterCollName << " from StoreGate" << endmsg;
    return sc;
  }

  const DataHandle<xAOD::CaloClusterContainer> pClusCollCalib;
  if(m_doSaveCalibClusInfo) {
    sc = evtStore()->retrieve(pClusCollCalib,m_clusterCollNameCalib);
    if(sc != StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "Could not retrieve ClusterContainer " 
          << m_clusterCollNameCalib << " from StoreGate" << endmsg;
      return sc;
    }

    if(pClusColl->size() != pClusCollCalib->size()) {
      msg(MSG::WARNING) << "Different size of calibrated and uncalibrated cluster collection " 
          << pClusColl->size() << " " << pClusCollCalib->size() << endmsg;
      return StatusCode::SUCCESS;
    }
  }

  /* ********************************************
  reading TBEventInfo
  ******************************************** */
  const TBEventInfo* theTBEventInfo = 0;
  if(m_isTestbeam) {
    sc = evtStore()->retrieve(theTBEventInfo,"TBEventInfo");
    if (sc.isFailure()||!theTBEventInfo){
      msg(MSG::ERROR) << "No  theTBEventInfo found"<< endmsg;
      return StatusCode::FAILURE;
    }
  }

  /* ********************************************
  reading primary particle
  ******************************************** */
  const McEventCollection* truthEvent=0;
  sc = evtStore()->retrieve(truthEvent, "TruthEvent");
  if (sc.isFailure()||!truthEvent){
    msg(MSG::ERROR) << "No  McEventCollection found"<< endmsg;
    return StatusCode::FAILURE;
  }
  HepMC::GenEvent::particle_const_iterator pit  = truthEvent->at(0)->particles_begin();
  const HepMC::GenParticle * gen  = *pit;

  double mc_eta = gen->momentum().pseudoRapidity();
  double mc_phi = gen->momentum().phi();
  if(m_isTestbeam) {
    double x0 = gen->production_vertex()->position().x();
    double y0 = gen->production_vertex()->position().y();
    // to calculate ATKAS-like eta, phi coordinates of incident particle
    GetLCSinglePionsPerf::as_in_atlas(mc_eta, mc_phi, x0, y0, (double)theTBEventInfo->getCryoX());
  }

  m_data->clear();

  m_data->m_mc_pdg = gen->pdg_id();
  m_data->m_mc_ener = gen->momentum().e();
  m_data->m_mc_eta = mc_eta;
  m_data->m_mc_phi = mc_phi;

  int nClus = pClusColl->size();
  m_data->m_ncls = nClus;
  m_data->m_cls_ener->resize(nClus, 0.0);
  m_data->m_cls_ener_unw->resize(nClus, 0.0);
  m_data->m_cls_lambda->resize(nClus, 0.0);
  m_data->m_cls_eta->resize(nClus, 0.0);
  m_data->m_cls_phi->resize(nClus, 0.0);
  //m_data->m_cls_emfrac->resize(nClus, 0.0);
  m_data->m_cls_smpener->resize(nClus);
  m_data->m_cls_smpener_unw->resize(nClus);
  //m_data->m_cls_ibin->resize(nClus);
  m_data->m_cls_eprep->resize(nClus);
  m_data->m_cls_dmener->resize(nClus);
  m_data->m_narea = m_HadDMCoeff->getSizeAreaSet();
  for(int i_cls=0; i_cls<nClus; i_cls++){
    //(*m_cls_ibin)[i_cls].resize(m_narea, -1);
    (*m_data->m_cls_smpener)[i_cls].resize(CaloSampling::Unknown, 0.0);
    (*m_data->m_cls_smpener_unw)[i_cls].resize(CaloSampling::Unknown, 0.0);
    (*m_data->m_cls_eprep)[i_cls].resize(m_data->m_narea, 0.0);
    (*m_data->m_cls_dmener)[i_cls].resize(m_data->m_narea, 0.0);
  }
  m_data->m_cls_engcalib->resize(nClus, 0.0);
  m_data->m_cls_recostat->resize(nClus, 0);
  m_data->m_cls_pi0prob->resize(nClus, 0.0);
  m_data->m_cls_isol->resize(nClus, 0.0);
  m_data->m_cls_oocener->resize(nClus, 0.0);
  m_data->m_cls_calib_emfrac->resize(nClus, 0.0);
  m_data->m_cls_engcalibpres->resize(nClus, 0.0);

  xAOD::CaloClusterContainer::const_iterator clusIter = pClusColl->begin();
  xAOD::CaloClusterContainer::const_iterator clusIterEnd = pClusColl->end();
  unsigned int iClus = 0;
  for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {
    const xAOD::CaloCluster * theCluster = (*clusIter);

    (*m_data->m_cls_ener)[iClus] = theCluster->e();
    // cluster energy in samplings
    for(int i_smp=0; i_smp<(int)CaloSampling::Unknown; i_smp++){
      (*m_data->m_cls_smpener)[iClus][i_smp] = theCluster->eSample((CaloSampling::CaloSample)i_smp);
    }

    // calibration energy of clusters
    double mx_calib_tot=0;
    if( !theCluster->retrieveMoment( xAOD::CaloCluster::ENG_CALIB_TOT, mx_calib_tot) ) {
      msg(MSG::ERROR) << "Moment ENG_CALIB_TOT is absent"  << endmsg;
      return StatusCode::FAILURE;
    }
    m_data->m_engClusSumCalib += mx_calib_tot;
    (*m_data->m_cls_engcalib)[iClus] = mx_calib_tot;
    
    double mx_calib_emb0=0, mx_calib_eme0=0, mx_calib_tileg3=0;
    if( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_EMB0, mx_calib_emb0)
	|| !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_EME0, mx_calib_eme0)
	|| !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TILEG3, mx_calib_tileg3)){
      msg(MSG::ERROR) << "One of the moment ENG_CALIB_EMB0, ENG_CALIB_EME0, ENG_CALIB_TILEG3 is absent" << endmsg;
      return StatusCode::FAILURE;
    }else{
       (*m_data->m_cls_engcalibpres)[iClus] = (mx_calib_emb0+mx_calib_eme0+mx_calib_tileg3);
    }

    if(m_doSaveCalibClusInfo) {
      const xAOD::CaloCluster * theClusterCalib = pClusCollCalib->at(iClus);

      // reco status
      CaloRecoStatus recoStatus = theClusterCalib->recoStatus();

      (*m_data->m_cls_recostat)[iClus] = recoStatus.getStatusWord();

      // classification pi0 probability (available on calibrated cluster)
      double pi0Prob = 0;
      if( !theClusterCalib->retrieveMoment( xAOD::CaloCluster::EM_PROBABILITY, pi0Prob) ) {
        //msg(MSG::ERROR) << "Moment ENG_CALIB_TOT is absent"  << endmsg;
        pi0Prob = -1.0;
      } else {
        if ( pi0Prob < 0 ) pi0Prob = 0;
        if ( pi0Prob > 1 ) pi0Prob = 1;
      }
      (*m_data->m_cls_pi0prob)[iClus] = pi0Prob;
    } // m_doSaveCalibClusInfo

    // cluster isolation moment and out of cluster energy
    double mx_isol;
    if ( !theCluster->retrieveMoment(xAOD::CaloCluster::ISOLATION, mx_isol)) {
      msg(MSG::ERROR) << "Moment ISOLATION is absent"  << endmsg;
      return StatusCode::FAILURE;
    }else{
      (*m_data->m_cls_isol)[iClus] = mx_isol;
    }

    double mx_calib_oocL;
    if ( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_OUT_L, mx_calib_oocL)) {
      msg(MSG::ERROR) << "Moment ENG_CALIB_OUT_L is absent"  << endmsg;
      return StatusCode::FAILURE;
    }else{
      (*m_data->m_cls_oocener)[iClus] = mx_calib_oocL;
    }

    double mx_calib_emfrac;
    if ( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_FRAC_EM, mx_calib_emfrac)) {
      msg(MSG::WARNING) << "Moment ENG_CALIB_FRAC_EM is absent"  << endmsg;
      return StatusCode::FAILURE;
    }else{
      (*m_data->m_cls_calib_emfrac)[iClus] = mx_calib_emfrac;
    }


    // calculation of cluster energy and energy in samplings without accounting
    // cells weights
    xAOD::CaloCluster::const_cell_iterator cellIter    = theCluster->cell_begin();
    xAOD::CaloCluster::const_cell_iterator cellIterEnd = theCluster->cell_end();
    for(; cellIter != cellIterEnd; cellIter++ ){
      const CaloCell* pCell = (*cellIter);
      Identifier myId = pCell->ID();
      CaloSampling::CaloSample nsmp = (CaloSampling::CaloSample)m_calo_id->calo_sample(myId);
      (*m_data->m_cls_ener_unw)[iClus] += pCell->e();
      (*m_data->m_cls_smpener_unw)[iClus][(int)nsmp] += pCell->e();
    }

    double clusEner = (*m_data->m_cls_ener_unw)[iClus];
    double clusLambda=0;
    if (!theCluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,clusLambda)) {
      msg(MSG::WARNING) << "Moment CENTER_LAMBDA is absent"  << endmsg;
      return StatusCode::FAILURE;
    }
    (*m_data->m_cls_lambda)[iClus] = clusLambda;
    (*m_data->m_cls_eta)[iClus] = theCluster->eta();
    (*m_data->m_cls_phi)[iClus] = theCluster->phi();
    if(clusEner > m_energyMin ) {
      //clusEner = log10(clusEner);
      //if(clusEner > 6.3) clusEner = 6.2999;
      //clusLambda = log10(clusLambda);
      //if(clusLambda > 4.0) clusLambda =  3.9999;
      //double clusEta=fabs(theCluster->eta());

      for(int i_dma=0; i_dma<m_data->m_narea; i_dma++){
        //(*m_cls_ibin)[iClus][i_dma] = m_HadDMCoeff->getBin(i_dma, (float)clusEmFrac, (float)clusEner, (float)clusLambda, (float)clusEta);
	double dmVal=0;
	if (!theCluster->retrieveMoment( m_momentForDMArea[i_dma],dmVal)) {
	  msg(MSG::WARNING) << "Moment "<< m_momentForDMArea[i_dma] << " is absent"  << endmsg;
	  return StatusCode::FAILURE;
	}
        (*m_data->m_cls_dmener)[iClus][i_dma] = dmVal;

        // now we have to calculate cluster quantities which we will use later for
        // reconstruction of given dead material calibration moment
        double eprep = 0.0;
        double x(0), y(0);
        switch ( m_momentForDMArea[i_dma] ) {
          case xAOD::CaloCluster::ENG_CALIB_DEAD_EMB0:
            x = (*m_data->m_cls_smpener_unw)[iClus][CaloSampling::PreSamplerB];
            if(x > 0.0) {
              eprep = x;
            }
            break;
          case xAOD::CaloCluster::ENG_CALIB_DEAD_TILE0:
            x = (*m_data->m_cls_smpener_unw)[iClus][CaloSampling::EMB3];
            y = (*m_data->m_cls_smpener_unw)[iClus][CaloSampling::TileBar0];
            if(x>0.0 && y>0.0) {
              eprep = sqrt(x*y);
            }
            break;
          case xAOD::CaloCluster::ENG_CALIB_DEAD_TILEG3:
            x = (*m_data->m_cls_smpener_unw)[iClus][CaloSampling::TileGap3];
            if(x > 0.0) {
              eprep = x;
            }
            break;
          case xAOD::CaloCluster::ENG_CALIB_DEAD_EME0:
            x = (*m_data->m_cls_smpener_unw)[iClus][CaloSampling::PreSamplerE];
            if( x > 0.0 ) {
              eprep = x;
            }
            break;
          case xAOD::CaloCluster::ENG_CALIB_DEAD_HEC0:
            x = (*m_data->m_cls_smpener_unw)[iClus][CaloSampling::EME3];
            y = (*m_data->m_cls_smpener_unw)[iClus][CaloSampling::HEC0];
            if(x>0.0 && y>0.0) {
              eprep = sqrt(x*y);
            }
            break;
          case xAOD::CaloCluster::ENG_CALIB_DEAD_FCAL:
            eprep = (*m_data->m_cls_ener_unw)[iClus];
            break;
          case xAOD::CaloCluster::ENG_CALIB_DEAD_LEAKAGE:
            eprep = (*m_data->m_cls_ener_unw)[iClus];
            break;
          case xAOD::CaloCluster::ENG_CALIB_DEAD_UNCLASS:
            eprep = (*m_data->m_cls_ener_unw)[iClus];
            break;
          default:
            msg(MSG::ERROR) << "No such moment registered " << m_momentForDMArea[i_dma] << endmsg;
            return StatusCode::FAILURE;
            break;
        }
        (*m_data->m_cls_eprep)[iClus][i_dma] = eprep;

      } // loop over dm areas

    } // good cluster
  } // loop over clusters

  // now we have to process special case, when we have DM energy, say, in front of the
  // calorimeter but there is no signal in presampler itself. When we have to add this
  // DM energy into 'unclassified' area
  for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){
    double edm_uncorrected = 0.0;
    for(int i_dma=0; i_dma<m_data->m_narea; i_dma++){
      xAOD::CaloCluster::MomentType xm = m_momentForDMArea[i_dma];
      if( (*m_data->m_cls_eprep)[i_cls][i_dma] <= 0.0 &&
         (xm == xAOD::CaloCluster::ENG_CALIB_DEAD_EMB0 ||
          xm == xAOD::CaloCluster::ENG_CALIB_DEAD_TILE0 ||
          xm == xAOD::CaloCluster::ENG_CALIB_DEAD_TILEG3 ||
          xm == xAOD::CaloCluster::ENG_CALIB_DEAD_EME0 ||
          xm == xAOD::CaloCluster::ENG_CALIB_DEAD_HEC0) )
      {
        edm_uncorrected += (*m_data->m_cls_dmener)[i_cls][i_dma];
        (*m_data->m_cls_dmener)[i_cls][i_dma] = 0.0;
      }
    } // i_dma
    // now let's put this energy into unclassified area
    for(int i_dma=0; i_dma<m_data->m_narea; i_dma++){
      if( m_momentForDMArea[i_dma] == xAOD::CaloCluster::ENG_CALIB_DEAD_UNCLASS) {
         (*m_data->m_cls_dmener)[i_cls][i_dma] += edm_uncorrected;
      }
    } // i_dma
  } // i_cls

  m_outputTree->Fill();

  return StatusCode::SUCCESS;
}

