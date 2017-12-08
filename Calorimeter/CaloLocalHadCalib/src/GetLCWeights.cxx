/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: GetLCWeights.cxx,v 1.3 2009-03-06 14:43:23 pospelov Exp $
//
// Description: see GetLCWeights.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloLocalHadCalib/GetLCWeights.h"

//---------------
// C++ Headers --
//---------------
#include "CaloLocalHadCalib/GetLCDefs.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloUtils/CaloSamplingHelper.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "AthenaKernel/errorcheck.h"

#include "TFile.h"
#include "TProfile2D.h"
#include "TString.h"
#include <iterator>
#include <cmath>

//###############################################################################
GetLCWeights::GetLCWeights(const std::string& name, 
			   ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_outputFile(0),
    m_clusterCollName("CaloTopoCluster"),
    m_useInversionMethod(true),
    m_NormalizationType("Lin"),
    m_NormalizationTypeNumber(0),
    m_ClassificationType("None"),
    m_ClassificationTypeNumber(0),
    m_calo_dd_man(0),
    m_calo_id(0)
{
 
  std::vector<Gaudi::Histo1DDef> dims(7);
  dims[1] = Gaudi::Histo1DDef("side",-1.5,1.5,1);
  dims[2] = Gaudi::Histo1DDef("|eta|",0.,1.6,16);
  dims[3] = Gaudi::Histo1DDef("phi",-M_PI,M_PI,1);
  dims[4] = Gaudi::Histo1DDef("log10(E_clus (MeV))",log10(200),log10(1e6),14);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-4.6,1.1,14);
  dims[6] = Gaudi::Histo1DDef("weight",-2,3,1);

  dims[0] = Gaudi::Histo1DDef("EMB1",0.5,1.5,1);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("EMB2",1.5,2.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-4.8,0.5,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("EMB3",2.5,3.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-4.9,0.6,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("EME1",4.5,5.5,1);
  dims[2] = Gaudi::Histo1DDef("|eta|",1.2,3.2,20);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-4.5,1.7,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("EME2",5.5,6.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-4.8,1.2,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("EME3",6.5,7.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-4.5,1.0,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("HEC0",7.5,8.5,1);
  dims[2] = Gaudi::Histo1DDef("|eta|",1.4,3.4,20);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-4.8,0.6,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("HEC1",8.5,9.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-4.9,-0.2,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("HEC2",9.5,10.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-4.8,-0.2,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("HEC3",10.5,11.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-4.7,-0.2,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("TileBar0",11.5,12.5,1);
  dims[2] = Gaudi::Histo1DDef("|eta|",0.,1.2,12);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-6,-1,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("TileBar1",12.5,13.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-6.7,-1.2,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("TileBar2",13.5,14.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-7,-1.9,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("TileGap1",14.5,15.5,1);
  dims[2] = Gaudi::Histo1DDef("|eta|",0.8,1.8,10);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-6,-1,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("TileGap2",15.5,16.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-6.5,-1.5,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("TileExt0",17.5,18.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-6.5,-1,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("TileExt1",18.5,19.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-6.6,-1.5,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("TileExt2",19.5,20.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-7.2,-2.4,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("FCal1",20.5,21.5,1);
  dims[2] = Gaudi::Histo1DDef("|eta|",2.8,5.0,22);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-3.5,2,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("FCal2",21.5,22.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-3.5,1.5,14);
  mapinsert(dims);
  dims[0] = Gaudi::Histo1DDef("FCal3",22.5,23.5,1);
  dims[5] = Gaudi::Histo1DDef("log10(rho_cell (MeV/mm^3))",-3.8,1,14);
  mapinsert(dims);

  // Dimensions to use for all samplings
  declareProperty("SamplingDimensions",m_dimensionsmap);

  // Name of output file to save histograms in
  declareProperty("OutputFileName",m_outputFileName);  
  
  // Name of ClusterContainer to use
  declareProperty("ClusterCollectionName",m_clusterCollName); 

  // Names of CalibrationHitContainers to use
  declareProperty("CalibrationHitContainerNames",m_CalibrationHitContainerNames); 
  // Use Inversion Method
  declareProperty("UseInversionMethod",m_useInversionMethod); 
  // Normalization type "Const", "Lin", "Log", "NClus"
  declareProperty("NormalizationType",m_NormalizationType); 
  // Classification type "None" for single pion MC or 
  //                     "ParticleID_EM" for ParticleID based em-type clusters  
  //                     "ParticleID_HAD" for ParticleID based had-type clusters  
  declareProperty("ClassificationType", m_ClassificationType);
}

//###############################################################################

GetLCWeights::~GetLCWeights()
{ }

//###############################################################################

StatusCode GetLCWeights::initialize()
{
  //---- initialize the StoreGateSvc ptr ----------------
  
  // pointer to detector manager:
  m_calo_dd_man = CaloDetDescrManager::instance(); 

  m_calo_id = m_calo_dd_man->getCaloCell_ID();

  m_outputFile = new TFile(m_outputFileName.c_str(),"RECREATE");
  m_outputFile->cd();

  m_weight.resize(CaloSampling::Unknown);
  m_isampmap.resize(CaloSampling::Unknown);
  mapparse();


  if ( m_NormalizationType == "Lin" ) {
    ATH_MSG_INFO( "Using weighting proportional to E_calib" );
    m_NormalizationTypeNumber = GetLCDefs::LIN;
  }
  else if ( m_NormalizationType == "Log" ) {
    ATH_MSG_INFO( "Using weighting proportional to log(E_calib)" );
    m_NormalizationTypeNumber = GetLCDefs::LOG;
  }
  else if ( m_NormalizationType == "NClus" ) {
    ATH_MSG_INFO( "Using weighting proportional to 1/N_Clus_E_calib>0" );
    m_NormalizationTypeNumber = GetLCDefs::NCLUS;
  }
  else {
    ATH_MSG_INFO( "Using constant weighting" );
    m_NormalizationTypeNumber = GetLCDefs::CONST;
  }

  if ( m_ClassificationType == "None" ) {
    ATH_MSG_INFO( "Expecting single particle input" );
    m_ClassificationTypeNumber = GetLCDefs::NONE;
  }
  else if ( m_ClassificationType == "ParticleID_EM" ) {
    ATH_MSG_INFO( "Expecting ParticleID simulation as input -- use EM type clusters only" );
    m_ClassificationTypeNumber = GetLCDefs::PARTICLEID_EM;
  }
  else if ( m_ClassificationType == "ParticleID_HAD" ) {
    ATH_MSG_INFO( "Expecting ParticleID simulation as input -- use HAD type clusters only" );
    m_ClassificationTypeNumber = GetLCDefs::PARTICLEID_HAD;
  }
  else {
    ATH_MSG_WARNING( " unknown classification type " << m_ClassificationType << " given! Using None instead" );
    m_ClassificationTypeNumber = GetLCDefs::NONE;
  }

  for(unsigned int isamp=0;isamp<m_dimensions.size();isamp++) {
    int theSampling(CaloSampling::Unknown);
    for (unsigned int jsamp = 0;jsamp< CaloSampling::Unknown; jsamp++) {
      if ( m_dimensions[isamp][0].title() == CaloSamplingHelper::getSamplingName((CaloSampling::CaloSample)jsamp)) {
	theSampling = jsamp;
	break;
      }
    }
    if ( theSampling == CaloSampling::Unknown ) {
      ATH_MSG_ERROR( "Calorimeter sampling " 
	  << m_dimensions[isamp][0].title() 
          << " is not a valid Calorimeter sampling name and will be ignored! "
          << "Valid names are: ";
      for (unsigned int jsamp = 0;jsamp< CaloSampling::Unknown; jsamp++) {
	msg() << CaloSamplingHelper::getSamplingName((CaloSampling::CaloSample)jsamp);
	if ( jsamp < CaloSampling::Unknown-1) 
	  msg() << ", ";
	else 
	  msg() << ".";
      }
      msg() );
    }
    else {
      m_isampmap[theSampling].resize(4,-1);
      m_isampmap[theSampling][0] = isamp;
      int iside(-1);
      int ieta(-1);
      int iphi(-1);
      int ilogE(-1);
      int ilogrho(-1);
      int iweight(-1);
      
      for(unsigned int idim=1;idim<m_dimensions[isamp].size();idim++) {
	if (      m_dimensions[isamp][idim].title() == "side" ) {
	  iside = idim;
	  m_isampmap[theSampling][1] = iside;
	}
	else if ( m_dimensions[isamp][idim].title() == "|eta|" ) {
	  ieta = idim;
	  m_isampmap[theSampling][2] = ieta;
	}
	else if ( m_dimensions[isamp][idim].title() == "phi" ) {
	  iphi = idim;
	  m_isampmap[theSampling][3] = iphi;
	}
	else if ( m_dimensions[isamp][idim].title() == "log10(E_clus (MeV))" )
	  ilogE = idim;
	else if ( m_dimensions[isamp][idim].title() == "log10(rho_cell (MeV/mm^3))" )
	  ilogrho = idim;
	else if ( m_dimensions[isamp][idim].title() == "weight" )
	  iweight = idim;
      }
      if ( ilogE < 0 || ilogrho < 0 || iweight < 0 ) {
	ATH_MSG_FATAL( " Mandatory dimension log10E, log10rho or weight missing ..." );
	return StatusCode::FAILURE;
      }
      int nside = (iside>=0?m_dimensions[isamp][iside].bins():1);
      int neta = (ieta>=0?m_dimensions[isamp][ieta].bins():1);
      int nphi = (iphi>=0?m_dimensions[isamp][iphi].bins():1);
      m_weight[theSampling].resize(nside*neta*nphi,0);
      for ( int jside=0;jside<nside;jside++) {
	for ( int jeta=0;jeta<neta;jeta++) {
	  for ( int jphi=0;jphi<nphi;jphi++) {
	    TString wname("");
	    if ( m_useInversionMethod ) 
	      wname += "inv_weight";
	    else
	      wname += "weight";
	    wname += "_isamp_";
	    wname += theSampling;
	    wname += "_iside_";
	    wname += jside;
	    wname += "_[";
	    wname += (iside>=0?m_dimensions[isamp][iside].lowEdge():-1);
	    wname += ",";
	    wname += (iside>=0?m_dimensions[isamp][iside].highEdge():-1);
	    wname += ",";
	    wname += nside;
	    wname += "]";
	    wname += "_ieta_";
	    wname += jeta;
	    wname += "_[";
	    wname += (ieta>=0?m_dimensions[isamp][ieta].lowEdge():-1);
	    wname += ",";
	    wname += (ieta>=0?m_dimensions[isamp][ieta].highEdge():-1);
	    wname += ",";
	    wname += neta;
	    wname += "]";
	    wname += "_iphi_";
	    wname += jphi;
	    wname += "_[";
	    wname += (iphi>=0?m_dimensions[isamp][iphi].lowEdge():-1);
	    wname += ",";
	    wname += (iphi>=0?m_dimensions[isamp][iphi].highEdge():-1);
	    wname += ",";
	    wname += nphi;
	    wname += "]";
	    int iW = jphi*neta*nside+jeta*nside+jside;
	    m_weight[theSampling][iW]=
	      new TProfile2D(wname,wname,
			     m_dimensions[isamp][ilogE].bins(),
			     m_dimensions[isamp][ilogE].lowEdge(),
			     m_dimensions[isamp][ilogE].highEdge(),
			     m_dimensions[isamp][ilogrho].bins(),
			     m_dimensions[isamp][ilogrho].lowEdge(),
			     m_dimensions[isamp][ilogrho].highEdge(),
			     m_dimensions[isamp][iweight].lowEdge(),
			     m_dimensions[isamp][iweight].highEdge(),
			     "spread");
	    if ( m_useInversionMethod ) {
	      m_weight[theSampling][iW]->SetYTitle("log10(#rho_{cell}^{true} (MeV/mm^{3}))");
	      m_weight[theSampling][iW]->SetZTitle("E_{reco}/E_{tot}");
	    }
	    else {
	      m_weight[theSampling][iW]->SetYTitle("log10(#rho_{cell} (MeV/mm^{3}))");
	      m_weight[theSampling][iW]->SetZTitle("E_{tot}/E_{reco}");
	    }
	    m_weight[theSampling][iW]->SetXTitle("log10(E_{clus} (MeV))");
	  }
	}
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode GetLCWeights::finalize()
{
  ATH_MSG_INFO( "Writing out histograms" );
  m_outputFile->cd();
  for(unsigned int i=0;i<m_weight.size();i++) {
    for(unsigned int j=0;j<m_weight[i].size();j++) {
      if ( m_weight[i][j] ) 
      m_weight[i][j]->Write();
    }
  }
  m_outputFile->Close();
  
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode GetLCWeights::execute()
{
  const DataHandle<xAOD::CaloClusterContainer> cc ;
  StatusCode sc = evtStore()->retrieve(cc,m_clusterCollName);

  if(sc != StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Could not retrieve ClusterContainer " 
	<< m_clusterCollName << " from StoreGate" );
    return sc;
  }

  const DataHandle<CaloCalibrationHitContainer> cchc;
  std::vector<const CaloCalibrationHitContainer *> v_cchc;
  std::vector<std::string>::iterator iter;
  for (iter=m_CalibrationHitContainerNames.begin();
       iter!=m_CalibrationHitContainerNames.end();iter++) {
    if ( !evtStore()->contains<CaloCalibrationHitContainer>(*iter)) {
      ATH_MSG_ERROR( "SG does not contain calibration hit container " << *iter );
      return StatusCode::FAILURE;
    }
    else {
      sc = evtStore()->retrieve(cchc,*iter);
      if (sc.isFailure() ) {
	ATH_MSG_ERROR( "Cannot retrieve calibration hit container " << *iter );
	return sc;
      } 
      else
	v_cchc.push_back(cchc);
    }
  }

  std::vector<ClusWeight *> cellVector[CaloCell_ID::NSUBCALO];
  
  for(int ic=0;ic<CaloCell_ID::NSUBCALO; ic++) {
    unsigned int maxHashSize(0);
    IdentifierHash myHashMin,myHashMax;
    m_calo_id->calo_cell_hash_range (ic,myHashMin,myHashMax);
    maxHashSize = myHashMax-myHashMin;
    cellVector[ic].resize(maxHashSize,0);
  }

  // loop over all cell members of all clusters and fill cell vector
  // for used cells
    
  // total calib hit energy of all clusters 
  double eCalibTot(0.); 

  xAOD::CaloClusterContainer::const_iterator clusIter = cc->begin();
  xAOD::CaloClusterContainer::const_iterator clusIterEnd = cc->end();
  unsigned int iClus = 0;
  double nClusECalibGt0 = 0.0;
  for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {
    const xAOD::CaloCluster * theCluster = (*clusIter);
    
    double eC=999; 
    if (!theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TOT,eC)) {
      ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_TOT" <<endmsg;
      return StatusCode::FAILURE;      
    }
    if ( m_ClassificationTypeNumber != GetLCDefs::NONE ) {
      double emFrac=-999; 
      if (!theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_FRAC_EM,emFrac)){
	ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_FAC_EM" <<endmsg;
	return StatusCode::FAILURE;
      }
      if (m_ClassificationTypeNumber == GetLCDefs::PARTICLEID_EM && emFrac < 0.5 )
	eC = 0;
      if (m_ClassificationTypeNumber == GetLCDefs::PARTICLEID_HAD && emFrac > 0.5 )
	eC = 0;
    }
    eCalibTot += eC;
    if ( eC > 0 ) {
      nClusECalibGt0++;
    }
    xAOD::CaloCluster::const_cell_iterator cellIter    = theCluster->cell_begin();
    xAOD::CaloCluster::const_cell_iterator cellIterEnd = theCluster->cell_end();
    for(; cellIter != cellIterEnd; cellIter++ ){
      const CaloCell* pCell = (*cellIter);
      Identifier myId = pCell->ID();
      IdentifierHash myHashId;
      int otherSubDet(0);
      myHashId = m_calo_id->subcalo_cell_hash(myId,otherSubDet);
      ClusWeight * myClus = new ClusWeight();
      myClus->iClus = iClus;
      myClus->weight = cellIter.weight();
      myClus->next = 0;
      myClus->eCalibTot = 0;
      ClusWeight * theList = cellVector[otherSubDet][(unsigned int)myHashId];
      if ( theList ) {
	while ( theList->next ) 
	  theList = theList->next;
	theList->next = myClus;
      }
      else {
	cellVector[otherSubDet][(unsigned int)myHashId] = myClus;
      }
    }
  }

  // calculate total calib energy of each cell in each cluster

  std::vector<const CaloCalibrationHitContainer * >::const_iterator it;
  for (it=v_cchc.begin();it!=v_cchc.end();it++) {
    CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
    CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
    for(;chIter!=chIterE;chIter++)  {
      Identifier myId = (*chIter)->cellID();
      int otherSubDet;
      IdentifierHash myHashId = m_calo_id->subcalo_cell_hash(myId,otherSubDet);
      if ( myHashId != CaloCell_ID::NOT_VALID ) {
	ClusWeight * theList = cellVector[otherSubDet][(unsigned int)myHashId];
	while ( theList ) {
	 theList->eCalibTot += (*chIter)->energyTotal();
	 theList = theList->next;
	}
      }
    }
  }
    
  // fill weight histos for all cells in all clusters
  // since pions can be split in several clusters the weights are 
  // calculated by importance of the cluster for the current pion - i.e.
  // the weights get a weight itself proportinal to the calib hit energy 
  // sum of the cluster over the total calib hit energy

  if ( eCalibTot > 0 ) {
    const double inv_eCalibTot = 1. / eCalibTot;
    const double inv_nClusECalibGt0 = 1. / nClusECalibGt0;
    for (unsigned int j=0;j<cc->size();j++) {
      const xAOD::CaloCluster * pClus = cc->at(j);
      double eng = pClus->e();
      double eCalib=-999;  
      if (!pClus->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TOT,eCalib)) {
	ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_TOT" <<endmsg;
	return StatusCode::FAILURE;
      }
      if ( eng > 0 && eCalib > 0 ) {
	if ( m_ClassificationTypeNumber != GetLCDefs::NONE ) {
	  double emFrac=-999;
	  if (!pClus->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_FRAC_EM,emFrac)) {
	    ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_FAC_EM" <<endmsg;
	    return StatusCode::FAILURE;
	  }
	  if (m_ClassificationTypeNumber == GetLCDefs::PARTICLEID_EM && emFrac < 0.5 )
	    continue;
	  if (m_ClassificationTypeNumber == GetLCDefs::PARTICLEID_HAD && emFrac > 0.5 )
	    continue;
	}
	xAOD::CaloCluster::const_cell_iterator cellIter    = pClus->cell_begin();
	xAOD::CaloCluster::const_cell_iterator cellIterEnd = pClus->cell_end();
	for(; cellIter != cellIterEnd; cellIter++ ){
	  const CaloCell* pCell = (*cellIter);
	  const Identifier myId = pCell->ID();
	  const CaloDetDescrElement* myCDDE=pCell->caloDDE();
	  const int caloSample = myCDDE->getSampling();//m_calo_id->calo_sample(myId);
	  if ( m_isampmap[caloSample].size() > 0 && 
	       m_weight[caloSample].size() > 0 ) {
	    int isideCell = 0;
	    int ietaCell = 0;
	    int iphiCell = 0;
	    int nside = 1;
	    int neta = 1;
	    int nphi = 1;
	    if ( m_isampmap[caloSample][1] >= 0 ) {
	      const Gaudi::Histo1DDef & hd = m_dimensions[m_isampmap[caloSample][0]][m_isampmap[caloSample][1]];
	      nside = hd.bins();
	      isideCell = (int)(nside*(((pCell->eta()<0?-1.0:1.0) - hd.lowEdge())
				       /(hd.highEdge()-hd.lowEdge())));
	      if ( isideCell < 0 || isideCell > nside-1 ) {
		ATH_MSG_WARNING( " Side index out of bounds " <<
		  isideCell << " not in [0," << nside-1 << "] for "
		    << "Sampl=" << caloSample ); 
		isideCell = -1;
	      }
	    }
	    if ( m_isampmap[caloSample][2] >= 0 ) {
	      const Gaudi::Histo1DDef & hd = m_dimensions[m_isampmap[caloSample][0]][m_isampmap[caloSample][2]];
	      neta = hd.bins();
	      ietaCell = (int)(neta*((fabs(pCell->eta()) - hd.lowEdge())
				     /(hd.highEdge()-hd.lowEdge())));
	      if ( ietaCell < 0 || ietaCell > neta-1 ) {
		ATH_MSG_WARNING( " Eta index out of bounds " <<
		  ietaCell << " not in [0," << neta-1 << "] for "
		    << "Sampl=" << caloSample ); 
		ietaCell = -1;
	      }
	    }
	    if ( m_isampmap[caloSample][3] >= 0 ) {
	      const Gaudi::Histo1DDef & hd = m_dimensions[m_isampmap[caloSample][0]][m_isampmap[caloSample][3]];
	      nphi = hd.bins();
	      iphiCell = (int)(nphi*((pCell->phi() - hd.lowEdge())
				     /(hd.highEdge()-hd.lowEdge())));
	      if ( iphiCell < 0 || iphiCell > nphi-1 ) {
		ATH_MSG_WARNING( " Phi index out of bounds " <<
		  iphiCell << " not in [0," << nphi-1 << "] for "
		    << "Sampl=" << caloSample ); 
		iphiCell = -1;
	      }
	    }
	    if ( isideCell >=0 && ietaCell >=0 && iphiCell >= 0 ) {
	      if ( myCDDE->volume() > 0 ) {
		IdentifierHash myHashId;
		int otherSubDet(0);
		myHashId = m_calo_id->subcalo_cell_hash(myId,otherSubDet);
		unsigned int iW = iphiCell*neta*nside+ietaCell*nside+isideCell;
		if ( iW >= m_weight[caloSample].size() ) {
		  ATH_MSG_WARNING( " Index out of bounds " <<
		    iW << " > " << m_weight[caloSample].size()-1 << " for "
		      << "Sampl=" << caloSample
		      << ", iphi=" << iphiCell 
		      << ", ieta=" << ietaCell 
		      << ", iside=" << isideCell );
		}
		else {
		  ClusWeight * theList = cellVector[otherSubDet][(unsigned int)myHashId];
		  while ( theList && theList->iClus != j ) 
		    theList = theList->next;
		  
		  if (m_weight[caloSample][iW] && theList && eCalibTot > 0) {
		    double norm = 0.0;
		    if ( m_NormalizationTypeNumber == GetLCDefs::LIN ) {
		      norm = eCalib * inv_eCalibTot;
		    }
		    else if ( m_NormalizationTypeNumber == GetLCDefs::LOG ) {
		      if ( eCalib > 0 ) {
			// cluster has to have at least 1% of the calib hit E
			norm = log10(eCalib * inv_eCalibTot)+2.0;
		      }
		    }
		    else if ( m_NormalizationTypeNumber == GetLCDefs::NCLUS ) {
		      if ( eCalib > 0 ) {
			norm = inv_nClusECalibGt0;
		      }
		    }
		    else {
		      norm = 1.0;
		    }
		    if ( norm > 0 ) {
		      if ( !m_useInversionMethod && pCell->e()>0 ) {
			m_weight[caloSample][iW]->Fill(log10(eng),
						       log10(pCell->e()/myCDDE->volume()),
						       theList->eCalibTot/pCell->e(),norm);

		      }
		      else if (m_useInversionMethod && theList->eCalibTot>0 ) {
			m_weight[caloSample][iW]->Fill(log10(eng),
						       log10(theList->eCalibTot/myCDDE->volume()),
						       pCell->e()/theList->eCalibTot,
						       norm);
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  
  for(unsigned int ic=0;ic<CaloCell_ID::NSUBCALO; ic++) {
    for (unsigned int ii = 0;ii<cellVector[ic].size();ii++ ) {
      ClusWeight * theList = cellVector[ic][ii];
      ClusWeight * prev = 0;
      while ( theList) {
	while ( theList->next ) {
	  prev = theList;
	  theList = theList->next;
	}
	delete theList;
	if ( prev ) 
	  prev->next = 0;
	else 
	  cellVector[ic][ii] = 0;
	theList = cellVector[ic][ii];
	prev = 0;
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

void GetLCWeights::mapinsert(const std::vector<Gaudi::Histo1DDef> & dims) {
  for (unsigned int i=0;i<dims.size();i++) {
    m_dimensionsmap[dims[0].title()+":"+dims[i].title()] = dims[i];
  }
}

void GetLCWeights::mapparse() {

  std::vector<int> theUsedSamplings(CaloSampling::Unknown,-1);

  int nsamp(-1);

  std::map<std::string,Gaudi::Histo1DDef>::iterator miter = m_dimensionsmap.begin();
  std::map<std::string,Gaudi::Histo1DDef>::iterator mend = m_dimensionsmap.end();
  
  for( ; miter != mend; miter++ ) {
    std::string dimname = miter->first.substr(0,miter->first.find(":"));
    int theSampling(CaloSampling::Unknown);
    for (unsigned int jsamp = 0;jsamp< CaloSampling::Unknown; jsamp++) {
      if ( dimname == CaloSamplingHelper::getSamplingName((CaloSampling::CaloSample)jsamp)) {
	theSampling = jsamp;
	break;
      }
    }
    if ( theSampling == CaloSampling::Unknown ) {
      msg(MSG::ERROR) "Calorimeter sampling " << dimname
          << " is not a valid Calorimeter sampling name and will be ignored! "
          << "Valid names are: ";
      for (unsigned int jsamp = 0;jsamp< CaloSampling::Unknown; jsamp++) {
	msg() << CaloSamplingHelper::getSamplingName((CaloSampling::CaloSample)jsamp);
	if ( jsamp < CaloSampling::Unknown-1) 
	  msg() << ", ";
	else 
	  msg() << ".";
      }
      msg() << endmsg;
    }
    else {
      if ( theUsedSamplings[theSampling] == -1 ) {
	nsamp++;
	theUsedSamplings[theSampling] = nsamp;
	m_dimensions.resize(nsamp+1);
	m_dimensions[nsamp].resize(0);
      }
      m_dimensions[theUsedSamplings[theSampling]].push_back(miter->second);
      ATH_MSG_DEBUG(" New Dimension for " << dimname << ": " 
		    << miter->second.title() << ", [" << miter->second.lowEdge()
		    << ", " << miter->second.highEdge() 
		    << ", " << miter->second.bins()
		    << "]");
    }
  }
}

//###############################################################################
