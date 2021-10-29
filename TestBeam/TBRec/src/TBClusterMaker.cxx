/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TBClusterMaker.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloGeoHelpers/proxim.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "StoreGate/ReadCondHandle.h"

//#############################################################################

TBClusterMaker::TBClusterMaker(const std::string& type, 
				     const std::string& name,
				     const IInterface* parent)
  
  : AthAlgTool(type, name, parent),
    m_cellCut(-99999.),
    m_seedCut(5.),
    m_deltaR(0.02),
    m_maxIter(4),
    m_CellEnergyInADC(false),
    m_calo_id(0)
{
  // CaloCell Container Name
  declareProperty("caloCellContainerName",m_caloCellContainerName="AllCalo");
  // Names of used calorimeter samplings
  declareProperty("samplingNames",m_samplingNames);
  // Cone cuts for each (!!) sampling used
  declareProperty("coneCuts",m_coneCuts);
  // Cut on a cell energy in sigma noise units
  declareProperty("cellCut",m_cellCut);
  // Cut on a seed cell energy in sigma noise units
  declareProperty("seedCut",m_seedCut);
  // Maximal claster position shift at the current step to stop iterations 
  declareProperty("deltaR",m_deltaR);
  // Maximal number of iterations to find cluster
  declareProperty("maxIterations",m_maxIter);
  // True if cell enrgy is in ADC counts, default = FALSE
  declareProperty("CellEnergyInADC",m_CellEnergyInADC);
  // Flag to fix cluster position ( \f$ \eta,\phi \f$) from JO file
  declareProperty("fixClusterPosition",m_fixClusterPosition=false);
  // Cluster \f$ \eta \f$) from JO file
  declareProperty("etaCluster",m_eta0=99.);
  // Cluster \f$ \phi \f$) from JO file
  declareProperty("phiCluster",m_phi0=99.);

}
////////////////////
// Initialization //
////////////////////

StatusCode TBClusterMaker::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endmsg;

  // Get pointer to CaloCell_ID:
  ATH_CHECK(detStore()->retrieve(m_calo_id, "CaloCell_ID"));
  ATH_CHECK( m_elecNoiseKey.initialize() );

  // setup calorimeter module and sampling lookup tables
  if ((this->setupLookupTables()).isFailure()) {
    log << MSG::FATAL
           << "problems performing setup of module and sampling lookup tables"
           << endmsg;
    return StatusCode::FAILURE;
  }

  // fill map with cone cuts
  if (m_samplingNames.size() <= 0) {
    log << MSG::FATAL << "List of samplings is not supplied" << endmsg;
    return StatusCode::FAILURE;    
  }
  if (m_samplingNames.size() != m_coneCuts.size()) {
    log << MSG::FATAL << "Number of cone cuts = " << m_coneCuts.size() << 
      " does not correspond to the number of samplings = " << 
      m_samplings.size() << endmsg;
    return StatusCode::FAILURE;
  }

  log << MSG::INFO << "Included calorimeter samplings(coneCuts): ";
  std::vector<std::string>::const_iterator sampling = m_samplingNames.begin();
  std::vector<float>::const_iterator cut = m_coneCuts.begin();
  m_samplingConeCuts.resize (CaloSampling::getNumberOfSamplings());
  for (; sampling != m_samplingNames.end(); ++sampling, ++cut) {
     CaloSampling::CaloSample idSamp = m_samplingFromNameLookup[*sampling];
    if (idSamp == CaloSampling::Unknown) {
      log << MSG::FATAL << " Unknown sampling: \042" << *sampling << "\042 "
	  << endmsg;
      return StatusCode::FAILURE;
    }
      log << MSG::INFO << "\042" << *sampling << "\042"
	  << "(" << *cut << ") ";
      m_samplings.push_back(idSamp);
      m_samplingConeCuts[idSamp] = *cut;
      CaloCell_ID::SUBCALO idCalo = m_caloLookup[idSamp];
      std::vector<CaloCell_ID::SUBCALO>::iterator it=m_calos.begin();
      for (;it!=m_calos.end();++it) if (*it == idCalo) break;
      if (it == m_calos.end()) m_calos.push_back(idCalo);
  }
  log << MSG::INFO << endmsg;
  log << MSG::INFO << "Included calorimeters: " << m_calos << endmsg;
  if (m_fixClusterPosition && (m_eta0 == 99. || m_phi0 == 99.)) {
    log << MSG::FATAL << " Cluster position is fixed but (eta, phi) are not " 
	<< "supplied in the JO file" << endmsg;
    return StatusCode::FAILURE;
  }
  // Clear counters
  //m_numSeedCellNotFound=0;
  //m_numCluIterationsConverged=0;
  //m_numCluIterationsNonConverged=0;

  return StatusCode::SUCCESS;
}


StatusCode TBClusterMaker::execute(const EventContext& ctx,
                                   xAOD::CaloClusterContainer* clusCont) const
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endmsg;

  SG::ReadCondHandle<CaloNoise> elecNoise (m_elecNoiseKey, ctx);

  /////////////////
  // Data Access //
  /////////////////

  // CaloCells
  const CaloCellContainer* cellContainer;
  StatusCode sc 
    = evtStore()->retrieve(cellContainer, m_caloCellContainerName);
  if (sc.isFailure()) {
    log << MSG::ERROR
	<< "cannot allocate CaloCellContainer with key <"
	<< m_caloCellContainerName << ">" << endmsg;
    return sc;
  }
  log << MSG::DEBUG << "CaloCellContainer container size = " <<  
    cellContainer->size() << endmsg;

  xAOD::CaloCluster *Cluster=0;
  double cluEta=0,cluPhi=0,cluNorm = 0;
  double cluEta0=0,cluPhi0=0,cluNorm0 = 0;
  bool clusterPositionFound = false;
  if (m_fixClusterPosition) {
    clusterPositionFound = true;
    cluEta0 = m_eta0;
    cluPhi0 = m_phi0;
    //m_numCluIterationsConverged++;
  }
  int nIter = 0;
  for (; nIter<m_maxIter+1; nIter++) {
    // loop over calorimeters
    for (CaloCell_ID::SUBCALO calo : m_calos) {
      // loop over cells of the current calorimeter
      CaloCellContainer::const_iterator itc= cellContainer->beginConstCalo(calo);
      int cindex=cellContainer->indexFirstCellCalo(calo);
      for (; itc!=cellContainer->endConstCalo(calo); ++itc,++cindex) {
	const CaloCell* cell = (*itc);
	double e = cell->energy();
        double noiseRMS = elecNoise->getNoise(cell->ID(), cell->gain());
	if(noiseRMS <= 0.) noiseRMS = 0.0001;
	double eSigma = e/noiseRMS;
        CaloSampling::CaloSample idSample=(CaloSampling::CaloSample) m_calo_id->calo_sample(cell->ID());
	if (m_CellEnergyInADC) eSigma *= m_adcToMeV[idSample];
	if (!clusterPositionFound) {
	  // find cluster position
	  if (nIter == 0) {           // use seed cells
	    if (eSigma < m_seedCut) continue;
	    log << MSG::DEBUG <<"smp,eta,phi,e,eSigma= "
		<<idSample<<" "<<cell->eta()<<" "<<cell->phi()<<" "<<
	      e<<" "<<eSigma<<" "<<endmsg;
	    double phiRef=0.;
	    if (cluNorm0 == 0) {
	      cluPhi0 = e*cell->phi();
	      phiRef=cell->phi();
	    }
	    else cluPhi0 += e*proxim(cell->phi(),phiRef);
	    cluEta0 += e*cell->eta();
	    cluNorm0 += e;
	  }                 // nIter == 0
	  else {
	    double dist = sqrt(pow(cluPhi0-proxim(cell->phi(),cluPhi0),2)
			       +pow(cluEta0-cell->eta(),2));
	    if (dist > m_samplingConeCuts[idSample]) continue;
	    if (eSigma < m_cellCut) continue;
	    //if (e>0.)  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	    if (eSigma > 2.) {
	      cluPhi += e*proxim(cell->phi(),cluPhi0);
	      cluEta += e*cell->eta();
	      cluNorm += e;
	    }
	  }
	} 
	else {
	  // fill the cluster
	  double dist = sqrt(pow(cluPhi0-proxim(cell->phi(),cluPhi0),2)
			     +pow(cluEta0-cell->eta(),2));
	  if (dist > m_samplingConeCuts[idSample]) continue;
	  if (eSigma < m_cellCut) continue;
	  //	  if (!Cluster) Cluster = new CaloCluster(cluEta0,cluPhi0);
	  if (!Cluster) Cluster = CaloClusterStoreHelper:: makeCluster(cellContainer,cluEta0,cluPhi0,xAOD::CaloCluster::SW_7_11);
	    //makeCluster();
	  //Cluster->addCell(cellContainer,cell,1.);
	  Cluster->addCell(cindex,1.);
	  log << MSG::DEBUG <<"smp,e,eSigma= "
	      <<idSample<<" "<<e<<" "<<eSigma<<" "<<endmsg;
	}
      } // end of loop over cells of the current calorimeter
    } // end of loop over calorimeters
    if (clusterPositionFound) break;
    if (nIter==0) {
      if (cluNorm0 == 0.) {
	//m_numSeedCellNotFound++;
	log << MSG::INFO << "No seed cell found" << endmsg;
	return StatusCode::SUCCESS;
      }
      cluPhi0 /= cluNorm0;
      cluEta0 /= cluNorm0;
      log << MSG::DEBUG << "nIter=0: cluEta0,cluPhi0,cluNorm0= " << cluEta0 
	  << " " << cluPhi0 << " " << cluNorm0 << endmsg;
      continue;
    }
    if (cluNorm == 0.) {
      log << MSG::ERROR << "cluNorm = 0.: should never be" << endmsg;
      return StatusCode::SUCCESS;
    }
    cluPhi /= cluNorm;
    cluEta /= cluNorm;
    double dist = sqrt(pow(cluPhi0-proxim(cluPhi,cluPhi0),2)
		       +pow(cluEta0-cluEta,2));
    if (dist < m_deltaR) {
      clusterPositionFound = true;
      //m_numCluIterationsConverged++;
    }
    else if (nIter == m_maxIter-1) {
      clusterPositionFound = true;
      //m_numCluIterationsNonConverged++;
      log << MSG::DEBUG << "Maximal number of iterations reached" << endmsg;
      log << MSG::DEBUG << "cluEta0,cluPhi0,cluNorm0= " << cluEta0 << " " <<
	cluPhi0 << " " << cluNorm0 << endmsg;
      log << MSG::DEBUG << "cluEta,cluPhi,cluNorm= " << cluEta << " " <<
	cluPhi << " " << cluNorm << endmsg;
    }
    cluEta0 = cluEta;
    cluPhi0 = cluPhi;
    cluNorm0 = cluNorm;
    cluEta=0.; cluPhi=0.; cluNorm = 0.;
  } // end of loop over iterations

  if (Cluster) {
    // ** Calclulate Kine **
    CaloClusterKineHelper::calculateKine(Cluster,false,true); //No weight at this point! 
    clusCont->push_back(Cluster);
    log<<MSG::DEBUG<<"Cluster eta0, phi0= "<<Cluster->eta0()<<" "<<
      Cluster->phi0();
    log<<MSG::DEBUG<<" Cluster #cells, E, eta, phi= "<<
      Cluster->clusterSize()<<" "<<Cluster->e()<<" "<<
      Cluster->eta()<<" "<<Cluster->phi()<<endmsg;
    log<<MSG::DEBUG<<"In samplings: #samp E eta(etasize) phi(phisize):"
       <<endmsg;
    //std::vector<CaloSampling::CaloSample>::const_iterator
      //it=m_samplings.begin();
    //for(;it!=m_samplings.end();it++) {
       // CaloSampling::CaloSample smp=*it;
      //      if (!Cluster->is_valid_sampling(smp)) continue;
      //log<<MSG::DEBUG<<"Valid= "<<Cluster->is_valid_sampling(smp)<<endmsg;
      //log<<MSG::DEBUG<<"#"<<smp<<" "<<Cluster->eSample(smp)<<" ";
      //log<<MSG::DEBUG<<"#"<<smp<<" "<<Cluster->eSample(smp)<<" "<<
      //	Cluster->etaSample(smp)<<"("<<Cluster->etasize(smp)<<") "<<
      //	Cluster->phiSample(smp)<<"("<<Cluster->phisize(smp)<<") ";
    //}
    //log<<MSG::DEBUG<<endmsg;
    return StatusCode::SUCCESS;
  } else {
    log << MSG::ERROR << "Cluster not found: should never be here!" << endmsg;
    return StatusCode::SUCCESS;
  }

}

StatusCode TBClusterMaker::finalize(){

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finalize()" << endmsg;

  //log << MSG::INFO << "Total number of found clusters = " <<
  //  m_numCluIterationsConverged + m_numCluIterationsNonConverged << endmsg;
  //log << MSG::INFO << "Numbers of clusters with converged/non-converged " <<
  //  "iteration procedure = " << m_numCluIterationsConverged << "/" <<
  //  m_numCluIterationsNonConverged<<endmsg;
  //log << MSG::INFO << " Seed cells were not found in " << m_numSeedCellNotFound
  //    << " events" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode TBClusterMaker::setupLookupTables() {

  m_samplingFromNameLookup["PreSamplerB"] = CaloSampling::PreSamplerB; // electromagnetic barrel
  m_samplingFromNameLookup["EMB1"]        = CaloSampling::EMB1;       
  m_samplingFromNameLookup["EMB2"]        = CaloSampling::EMB2;       
  m_samplingFromNameLookup["EMB3"]        = CaloSampling::EMB3;       
  m_samplingFromNameLookup["PreSamplerE"] = CaloSampling::PreSamplerE; // electromagnetic endcap
  m_samplingFromNameLookup["EME1"]        = CaloSampling::EME1;       
  m_samplingFromNameLookup["EME2"]        = CaloSampling::EME2;       
  m_samplingFromNameLookup["EME3"]        = CaloSampling::EME3;       
  m_samplingFromNameLookup["HEC0"]        = CaloSampling::HEC0;        // hadronic endcap
  m_samplingFromNameLookup["HEC1"]        = CaloSampling::HEC1;       
  m_samplingFromNameLookup["HEC2"]        = CaloSampling::HEC2;       
  m_samplingFromNameLookup["HEC3"]        = CaloSampling::HEC3;       
  m_samplingFromNameLookup["TileBar0"]    = CaloSampling::TileBar0;    // tile barrel
  m_samplingFromNameLookup["TileBar1"]    = CaloSampling::TileBar1;   
  m_samplingFromNameLookup["TileBar2"]    = CaloSampling::TileBar2;   
  m_samplingFromNameLookup["TileGap1"]    = CaloSampling::TileGap1;    // tile gap scintillators
  m_samplingFromNameLookup["TileGap2"]    = CaloSampling::TileGap2;   
  m_samplingFromNameLookup["TileGap3"]    = CaloSampling::TileGap3;   
  m_samplingFromNameLookup["TileExt0"]    = CaloSampling::TileExt0;    // tile extended barrel
  m_samplingFromNameLookup["TileExt1"]    = CaloSampling::TileExt1;   
  m_samplingFromNameLookup["TileExt2"]    = CaloSampling::TileExt2;   
  m_samplingFromNameLookup["FCal1"]       = CaloSampling::FCAL0;       // fcal
  m_samplingFromNameLookup["FCal2"]       = CaloSampling::FCAL1;      
  m_samplingFromNameLookup["FCal3"]       = CaloSampling::FCAL2;      
  m_samplingFromNameLookup["unknown"]     = CaloSampling::Unknown;     

  // fill calo lookup table
  m_caloLookup[CaloSampling::PreSamplerB] = CaloCell_ID::LAREM;
  m_caloLookup[CaloSampling::EMB1]        = CaloCell_ID::LAREM;
  m_caloLookup[CaloSampling::EMB2]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EMB3]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::PreSamplerE] = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EME1]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EME2]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EME3]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::HEC0]        = CaloCell_ID::LARHEC; 
  m_caloLookup[CaloSampling::HEC1]        = CaloCell_ID::LARHEC;  
  m_caloLookup[CaloSampling::HEC2]        = CaloCell_ID::LARHEC;  
  m_caloLookup[CaloSampling::HEC3]        = CaloCell_ID::LARHEC;  
  m_caloLookup[CaloSampling::TileBar0]    = CaloCell_ID::TILE;  
  m_caloLookup[CaloSampling::TileBar1]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileBar2]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileGap1]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileGap2]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileGap3]    = CaloCell_ID::TILE;    
  m_caloLookup[CaloSampling::TileExt0]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileExt1]    = CaloCell_ID::TILE;    
  m_caloLookup[CaloSampling::TileExt2]    = CaloCell_ID::TILE;    
  m_caloLookup[CaloSampling::FCAL0]       = CaloCell_ID::LARFCAL; 
  m_caloLookup[CaloSampling::FCAL1]       = CaloCell_ID::LARFCAL;  
  m_caloLookup[CaloSampling::FCAL2]       = CaloCell_ID::LARFCAL;  

  // fill ADC to MeV conversion map (for HIGH gain !!)
  m_adcToMeV.resize (CaloSampling::getNumberOfSamplings());
  m_adcToMeV[CaloSampling::EME2]  = 29.0;
  m_adcToMeV[CaloSampling::EME3]  = 13.7;
  m_adcToMeV[CaloSampling::HEC0]  = 11.2;
  m_adcToMeV[CaloSampling::HEC1]  = 11.2;
  m_adcToMeV[CaloSampling::HEC2]  = 11.2;
  m_adcToMeV[CaloSampling::FCAL0] = 83.4;
  m_adcToMeV[CaloSampling::FCAL1] = 163.9;
  m_adcToMeV[CaloSampling::FCAL2] = 181.8;

  return StatusCode::SUCCESS;
}
