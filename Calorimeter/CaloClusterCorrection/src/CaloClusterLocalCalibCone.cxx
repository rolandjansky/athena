/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloClusterLocalCalibCone.cxx,v 1.5 2009-04-22 17:26:23 ssnyder Exp $
//
// Description: see CaloClusterLocalCalibCone.h
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
#include "CaloClusterLocalCalibCone.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloRecoStatus.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "PathResolver/PathResolver.h"
#include "TFile.h"
#include "TProfile2D.h"
//#include "CLHEP/Vector/LorentzVector.h"

#include "xAODCaloEvent/CaloClusterKineHelper.h"

//using CLHEP::HepLorentzVector;

CaloClusterLocalCalibCone::CaloClusterLocalCalibCone(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  :AthAlgTool(type, name, parent),
    m_coneSchemaName("Angle"),
    m_coneSchema(Angle),
    m_coneDistance(0.2),
    m_recoStatus(CaloRecoStatus::UNKNOWNSTATUS),
    m_hadWeightFileName(""),
    m_signalOverNoiseCut(2),
    m_hadWeightFile(nullptr),
    m_noiseTool(nullptr)
{ 
  m_etaBins.resize(0);
  m_caloIndices.resize(0);
  m_samplingIndices.resize(0);
  m_data.resize(0);

  // Combination Schema for the Cones
  declareProperty("ConeSchema",m_coneSchemaName);
  // Combination Distance 
  declareProperty("ConeDistance",m_coneDistance);
  // Reco Status of Clusters to be considered for weighting
  declareProperty("ClusterRecoStatus",m_recoStatus) ;
  // Name of File containing the had weights
  declareProperty("HadWeightFileName",m_hadWeightFileName);
  // Eta Bins of the classification Data
  declareProperty("EtaBins",m_etaBins);
  // Calorimeter indices for each CaloCell_ID::CaloSample
  declareProperty("CaloIndices",m_caloIndices);
  // Sampling indices for each CaloCell_ID::CaloSample
  declareProperty("SamplingIndices",m_samplingIndices);
  // Minimal Signal Over Noise (|E|/sigma) level for cells 
  declareProperty("SignalOverNoiseCut",m_signalOverNoiseCut);
  
  declareProperty("UpdateSamplingVars",m_updateSamplingVars=false);
}

StatusCode CaloClusterLocalCalibCone::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;


   
  //---- retrieve the noise tool ----------------
   
  IToolSvc* p_toolSvc = nullptr;// Pointer to Tool Service
  StatusCode sc = service("ToolSvc", p_toolSvc);
  if (sc.isFailure()) {
    msg(MSG::FATAL)
	<< " Tool Service not found "
	<< endmsg;
    return StatusCode::FAILURE;
  }
   
  IAlgTool* algtool;
  ListItem corr("CaloNoiseTool/calonoisetool");	  
  sc = p_toolSvc->retrieveTool(corr.type(), corr.name(), algtool);
  if (sc.isFailure()) {
    msg(MSG::INFO)
	<< "Unable to find noise tool" 
	<< endmsg;
  }  
   
  m_noiseTool=dynamic_cast<ICalorimeterNoiseTool*>(algtool); 

  // set cone schema
  if ( m_coneSchemaName == "Angle" ) {
    m_coneSchema = Angle;
    msg( MSG::INFO)
        << "Angle schema with Delta Angle = " << m_coneDistance << " is set!"
	<< endmsg;
  }
  else if ( m_coneSchemaName == "DeltaR" ) {
    m_coneSchema = DeltaR;
    msg( MSG::INFO) 
        << "DeltaR schema with Delta R = " << m_coneDistance << " is set!"
	<< endmsg;
  }
  else {
    msg(MSG::ERROR)
        << "Unknown cone schema <" << m_coneSchemaName << "> selected. "
	<< "Please specify Angle or DeltaR and try again!"
	<< endmsg;
    return StatusCode::FAILURE;
  }

  // do some sanity checks on the properties given
  if ( m_hadWeightFileName.empty() ) {
    msg(  MSG::ERROR )
        << "You need to specify name of the file holding the data "
	<< "with the property HadWeightFileName in order to use this tool!"
	<< endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_etaBins.size() <= 1 ) {
    msg( MSG::ERROR)
        << "You need to specify the |eta| bins with the property "
	<< "EtaBins in order to use this tool!"
	<< endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_caloIndices.empty() ) {
    msg(MSG::ERROR)
        << "You need to specify the calorimeter indices with the property "
	<< "CaloIndices in order to use this tool!"
	<< endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_samplingIndices.empty() ) {
    msg(MSG::ERROR)
        << "You need to specify the sampling indices with the property "
	<< "SamplingIndices in order to use this tool!"
	<< endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_caloIndices.size() != m_samplingIndices.size() ) {
    msg(MSG::ERROR)
        << "You need to specify the same number of entries with the properties "
	<< "CaloIndices and SamplingIndices in order to use this tool!"
	<< endmsg;
    return StatusCode::FAILURE;
  }
  sc = initDataFromFile(m_hadWeightFileName);
  if ( sc.isFailure() ) {
    msg(MSG::ERROR)
        << "Unable to initialize hadronic weights from file <" 
	<< m_hadWeightFileName << ">" << endmsg;
  }

  return sc;
}

StatusCode CaloClusterLocalCalibCone::initDataFromFile(const std::string& hadWeightFileName)
{
  
  // Find the full path to filename:
  std::string file = PathResolver::find_file (hadWeightFileName, "DATAPATH");
  msg(MSG::INFO) << "Reading file  " << file << endmsg;
  m_hadWeightFile = new TFile(file.c_str());
  if ( !m_hadWeightFile ) {
    return StatusCode::FAILURE;
  }
  
  int neta    = m_etaBins.size()-1;
  int nsamplings = m_samplingIndices.size();
  m_data.resize(nsamplings);
  for(int isamp=0;isamp<nsamplings;isamp++) {
    m_data[isamp].resize(neta);
    for(int ieta=0;ieta<neta;ieta++) {
      if ( m_caloIndices[isamp] >= 0 && m_samplingIndices[isamp] >= 0 ) { 
	std::ostringstream sname;
	sname << "weight_ieta_" << ieta << "_icalo_" 
	      << m_caloIndices[isamp] << "_isamp_" << m_samplingIndices[isamp];
	m_data[isamp][ieta]=(TProfile2D *)m_hadWeightFile->Get(sname.str().c_str());
      }
      else 
	m_data[isamp][ieta] = nullptr;
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode
CaloClusterLocalCalibCone::execute(const EventContext& /*ctx*/,
                                   xAOD::CaloClusterContainer*  clusColl) const
{
  // make jets
  std::vector<TLorentzVector *> jets;
  std::vector<TLorentzVector *> pjet(clusColl->size(),nullptr);
  // assume clusters are ordered in E_T
  // loop over cluster collection and add each cluster to the map for 
  // each member cell
  xAOD::CaloClusterContainer::iterator clusIter = clusColl->begin();
  xAOD::CaloClusterContainer::iterator clusIterEnd = clusColl->end();
  unsigned int iClus=0;
  for (; clusIter != clusIterEnd; clusIter++, iClus++) {
    xAOD::CaloCluster* thisCluster = *clusIter;
    // check existing jets
    for (unsigned int ijet = 0; ijet < jets.size(); ijet++) {
      if ((m_coneSchema == Angle && jets[ijet]->Angle(thisCluster->p4().Vect()) < m_coneDistance) ||
          (m_coneSchema == DeltaR && jets[ijet]->DeltaR(thisCluster->p4()) < m_coneDistance)) {
        if (thisCluster->e() + jets[ijet]->E() > 0) {
          pjet[iClus] = jets[ijet];
          // add energy only - don't change jet direction
          (*jets[ijet]) *= ((thisCluster->e() + jets[ijet]->E()) / jets[ijet]->E());

          ATH_MSG_DEBUG("added cluster with E/eta/phi = "
                        << thisCluster->e() << "/" << thisCluster->eta() << "/" << thisCluster->phi() << " to jet "
                        << ijet << " which has now E/eta/phi/M = " << jets[ijet]->E() << "/" << jets[ijet]->Eta() << "/"
                        << jets[ijet]->Phi() << "/" << jets[ijet]->M() << endmsg);
        }
        break;
      }
    }
    if (!pjet[iClus] && thisCluster->e() > 0) {
      // make new jet
      TLorentzVector* theJet = new TLorentzVector(thisCluster->p4());
      pjet[iClus] = theJet;
      jets.push_back(theJet);
      ATH_MSG_DEBUG("cluster with E/eta/phi = "
                    << thisCluster->e() << "/" << thisCluster->eta() << "/" << thisCluster->phi() << " started new jet "
                    << jets.size() - 1 << " which has now E/eta/phi/M = " << theJet->E() << "/" << theJet->Eta() << "/"
                    << theJet->Phi() << "/" << theJet->M() << endmsg);
    }
  }
  const CaloDetDescrManager* calo_dd_man = nullptr;
  ATH_CHECK( detStore()->retrieve(calo_dd_man,"CaloMgr") );

  clusIter = clusColl->begin();
  iClus = 0;
  for (; clusIter != clusIterEnd; clusIter++, iClus++) {
    xAOD::CaloCluster* thisCluster = *clusIter;
    if (pjet[iClus] && thisCluster->recoStatus().checkStatus(CaloRecoStatus::StatusIndicator(m_recoStatus))) {
      TLorentzVector* thisJet = pjet[iClus];
      // weight all cells in selected cluster
      xAOD::CaloCluster::cell_iterator itrCell = thisCluster->cell_begin();
      xAOD::CaloCluster::cell_iterator itrCell_e = thisCluster->cell_end();
      for (; itrCell != itrCell_e; ++itrCell) {
        const CaloCell* thisCell = *itrCell;
        double weight = itrCell.weight();
        // check calo and sampling index for current cell
        Identifier myId = thisCell->ID();
        CaloCell_ID::CaloSample theSample = thisCell->caloDDE()->getSampling();
        if (theSample < (int)m_caloIndices.size() && m_caloIndices[theSample] >= 0 &&
            m_samplingIndices[theSample] >= 0) {
          // check noise level and energy density of the current cell
          double sigma = m_noiseTool->getNoise(thisCell, ICalorimeterNoiseTool::ELECTRONICNOISE);
          double energy = fabs(thisCell->e());
          double ratio = 0;
          if (finite(sigma) && sigma > 0)
            ratio = energy / sigma;
          if (ratio > m_signalOverNoiseCut) {
            double volume = 0;
            double density = 0;
            const CaloDetDescrElement* myCDDE = calo_dd_man->get_element(myId);
            if (myCDDE) {
              volume = myCDDE->volume();
            }
            if (volume > 0)
              density = energy / volume;
            if (density > 0) {
              int neta = m_etaBins.size() - 1;
              int ieta;
              double abseta = fabs(thisCell->eta());
              for (ieta = 0; ieta < neta; ieta++) {
                if (abseta >= m_etaBins[ieta] && abseta < m_etaBins[ieta + 1])
                  break;
              }
              if (ieta < neta && m_data[theSample][ieta]) {
                double log10edens = log10(density);
                double log10conee = log10(thisJet->E());
                double lemax = m_data[theSample][ieta]->GetXaxis()->GetBinCenter(m_data[theSample][ieta]->GetNbinsX());
                if (log10conee > lemax)
                  log10conee = lemax;
                int iBin = m_data[theSample][ieta]->FindBin(log10conee, log10edens);
                double num = m_data[theSample][ieta]->GetBinEntries(iBin);
                if (num > 10) {
                  weight *= m_data[theSample][ieta]->GetBinContent(iBin);
                  thisCluster->reweightCell(itrCell, weight);
                  ATH_MSG_VERBOSE("cell with E/eta/phi = "
                                  << thisCell->e() << "/" << thisCell->eta() << "/" << thisCell->phi()
                                  << " in jet with E/eta/phi/M = " << thisJet->E() << "/" << thisJet->Eta() << "/"
                                  << thisJet->Phi() << "/" << thisJet->M() << " weighted with w = " << weight
                                  << endmsg);
                }
              }
            } // end if density >0
          }   // end if noise cut
        }
      } // end loop over cells
      CaloClusterKineHelper::calculateKine(thisCluster,true,m_updateSamplingVars);

    } // end if reco status
  }   // end loop over clusters
  for(unsigned int ijet=0;ijet<jets.size();ijet++) {
    delete jets[ijet];
  }
  return StatusCode::SUCCESS;
}

CaloClusterLocalCalibCone::~CaloClusterLocalCalibCone()
{
  if ( m_hadWeightFile ) {
    m_hadWeightFile->Close();
    delete m_hadWeightFile;
    m_hadWeightFile = nullptr;
  }
}





