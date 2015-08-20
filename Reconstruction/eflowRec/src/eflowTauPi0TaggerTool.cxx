/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowTauPi0TaggerTool.h"
#include "eflowRec/phicorr.h"

#include "eflowEvent/eflowTauObject.h"

#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"

#include "CaloEvent/CaloClusterCellLink.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "StoreGate/StoreGateSvc.h"

#include "TMVA/Reader.h"

#include "PathResolver/PathResolver.h"

#include <string>
#include <cstdlib>

const double eflowTauPi0TaggerTool::DETA_EM1=0.031;
const double eflowTauPi0TaggerTool::DPHI_EM1=0.0245*4;

eflowTauPi0TaggerTool::eflowTauPi0TaggerTool(const std::string& type,const std::string& name,const IInterface* parent) :
  AthAlgTool( type, name, parent)
{
  m_emFracEnhanced = 0;
  m_eng_frac_core = 0;
  m_first_eng_dens = 0;
  m_centerLambda = 0;
  m_secondR = 0;
  m_deltaAlpha = 0;
  m_HotStripFrac = 0;
  m_ThreeCellStripFrac = 0;
  m_eta = 0;
  m_theXMLFile = "";
  m_storeGate = 0;
  m_Reader = nullptr;
}

eflowTauPi0TaggerTool::~eflowTauPi0TaggerTool(){
}

StatusCode eflowTauPi0TaggerTool::initialize(){
  
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
    {
      msg(MSG::WARNING )
	  << "Unable to retrieve pointer to StoreGateSvc"
	  << endreq;
      return StatusCode::SUCCESS;
    }

  m_Reader = new TMVA::Reader( "!Color:!Silent" );
  
  m_Reader->AddVariable("emFracEnhanced",&m_emFracEnhanced);
  m_Reader->AddVariable("eng_frac_core",&m_eng_frac_core);
  m_Reader->AddVariable("Trans1stEDens:= -1.*TMath::Log(1/first_eng_dens+1)",&m_first_eng_dens);
  m_Reader->AddVariable("centerLambda",&m_centerLambda);
  m_Reader->AddVariable("Trans2nR:= TMath::Log( 1/(secondR+5) + 1)",&m_secondR);
  m_Reader->AddVariable("deltaAlpha",&m_deltaAlpha);
  m_Reader->AddVariable("HotStripFrac",&m_HotStripFrac);
  m_Reader->AddVariable("ThreeCellStripFrac",&m_ThreeCellStripFrac);
  m_Reader->AddVariable("eta",&m_eta);

  m_theXMLFile = PathResolver::find_file ("TMVAClassification_Pi0BDT.weights.xml", "DATAPATH");

  if ("" != m_theXMLFile)  m_Reader->BookMVA("eflowPi0BDT", m_theXMLFile);
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not find TMVAClassification_Pi0BDT.weights.xml " << endreq; 
  
  return StatusCode::SUCCESS;
}

StatusCode eflowTauPi0TaggerTool::execute(const eflowTauObject& eflowTauObject){

  xAOD::PFOContainer* PFOContainer = eflowTauObject.getPFOContainer();

  if ("" == m_theXMLFile) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not find TMVAClassification_Pi0BDT.weights.xml " << endreq; 
    return StatusCode::SUCCESS;
  }

  xAOD::PFOContainer::const_iterator firstPFO = PFOContainer->begin();
  xAOD::PFOContainer::const_iterator lastPFO = PFOContainer->end();

  for (; firstPFO != lastPFO; ++firstPFO){

    if (0.0 != (*firstPFO)->charge()) continue;

    const xAOD::PFO* thisPFO = *firstPFO;
      
    const xAOD::CaloCluster* thisClus = thisPFO->cluster(0);

    if (thisClus){

      double eng_frac_core = 0.0;
      bool isRetrieved = thisClus->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_CORE, eng_frac_core );
      if (true == isRetrieved) m_eng_frac_core = static_cast<float>(eng_frac_core);
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not retrieved ENG_FRAC_CORE from the CaloCluster " << endreq;

      double first_eng_dens = 0.0;
      isRetrieved = thisClus->retrieveMoment(xAOD::CaloCluster::FIRST_ENG_DENS, first_eng_dens);
      if (true == isRetrieved) m_first_eng_dens = static_cast<float>(first_eng_dens);
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not retrieved FIRST_ENG_DENS from the CaloCluster " << endreq;

      double centerLambda = 0.0;
      isRetrieved = thisClus->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA, centerLambda);
      if (true == isRetrieved) m_centerLambda = static_cast<float>(centerLambda);
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not retrieved CENTERLAMBDA from the CaloCluster " << endreq;

      double secondR = 0.0;
      isRetrieved = thisClus->retrieveMoment(xAOD::CaloCluster::SECOND_R, secondR);
      if (true == isRetrieved) m_secondR = static_cast<float>(secondR);
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not retrieved SECONDR from the CaloCluster " << endreq;

      double deltaAlpha = 0.0;
      isRetrieved = thisClus->retrieveMoment(xAOD::CaloCluster::DELTA_ALPHA, deltaAlpha);
      if (true == isRetrieved) m_deltaAlpha = static_cast<float>(deltaAlpha);
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not retrieved DELTA_ALPHA from the CaloCluster " << endreq;

      m_eta = thisClus->eta();
      
      //our egamma style variables
	
      const CaloClusterCellLink* theCellLink = thisClus->getCellLinks();

      CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
      CaloClusterCellLink::const_iterator lastCell = theCellLink->end();
      
      double highEnergyStripCell = -99.0, highEnergyStripEta = -99.0, highEnergyStripPhi = -99.0;
      double stripEnergySum = 0.0;

      float emCellEnergy = 0.0;

      for (; firstCell != lastCell; ++firstCell){
	
	const CaloCell* thisCell = *firstCell;
	
	const CaloDetDescrElement* theDDE = thisCell->caloDDE();
	if (theDDE){
	  CaloCell_ID::CaloSample theSample = theDDE->getSampling(); 
	  double cellWeight = firstCell.weight();
	  if (CaloCell_ID::PreSamplerB == theSample || CaloCell_ID::EMB1 == theSample || CaloCell_ID::EMB2 == theSample || CaloCell_ID::EMB3 == theSample || CaloCell_ID::PreSamplerE == theSample || CaloCell_ID::EME1 == theSample || CaloCell_ID::EME2 == theSample || CaloCell_ID::EME3 == theSample) emCellEnergy +=  (*firstCell)->e()*cellWeight;
	  
	  if (CaloCell_ID::EMB1 == theSample || CaloCell_ID::EME1 == theSample){
	    double cellEnergy = (*firstCell)->e()*cellWeight;
	    stripEnergySum += cellEnergy;
	    if (cellEnergy > highEnergyStripCell) {
	      highEnergyStripCell = cellEnergy;
	      highEnergyStripEta = thisCell->eta();
	      highEnergyStripPhi = thisCell->phi();
	    }	      

	  }//if in EM1 layer
	}//if can get DDE element
	else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get DDE element for a CaloCell" << endreq;
	
      }//cell loop
      
      if (0.0 != thisClus->e()){
	m_emFracEnhanced = (emCellEnergy)/thisClus->e();
      }
      else m_emFracEnhanced = 0.0;
      
      if (-99.== highEnergyStripCell){
	m_HotStripFrac = 0.0;
	m_ThreeCellStripFrac = 0.0;
      }
      else{
	if (0.0 != thisClus->e()) m_HotStripFrac = highEnergyStripCell/thisClus->e();
	else m_HotStripFrac = 0.0;
	
	firstCell = theCellLink->begin();

	double highStripEnergySum = 0.0;
	  
	for (; firstCell != lastCell; ++firstCell){
	  
	  const CaloCell* thisCell = *firstCell;
	  
	  const CaloDetDescrElement* theDDE = thisCell->caloDDE();
	  if (theDDE){
	    CaloCell_ID::CaloSample theSample = theDDE->getSampling(); 
	    if (CaloCell_ID::EMB1 == theSample || CaloCell_ID::EME1 == theSample){
	      double cellEta = thisCell->eta();
	      double cellPhi = thisCell->phi();
	      if ( fabs(highEnergyStripEta - cellEta) < ((3*DETA_EM1/2)) && fabs ( phicorr(highEnergyStripPhi) - phicorr(cellPhi) ) < (DPHI_EM1/2)){

		double cellWeight = firstCell.weight();
		double cellEnergy = (*firstCell)->e()*cellWeight;
		highStripEnergySum += cellEnergy;
	      }//if on either side of high energy strip
	    }//if in EM1 layer
	  }//DDE
	  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get DDE element for a CaloCell" << endreq;
	}//cell loop
	
	if (0.0 != stripEnergySum) m_ThreeCellStripFrac = highStripEnergySum/stripEnergySum;
	else  m_ThreeCellStripFrac = 0.0;
	
      }

      double mva_val = 0.0;
      if ( "" != m_theXMLFile) mva_val = m_Reader->EvaluateMVA("eflowPi0BDT");
      xAOD::PFO* nonConstPFO = const_cast<xAOD::PFO*>(thisPFO);

      nonConstPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::eflowRec_EM_FRAC_ENHANCED, m_emFracEnhanced);
      nonConstPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::eflowRec_ENG_FRAC_CORE, m_eng_frac_core);
      nonConstPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::eflowRec_FIRST_ENG_DENS, m_first_eng_dens);
      nonConstPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::eflowRec_CENTER_LAMBDA,m_centerLambda );
      nonConstPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::eflowRec_SECOND_R, m_secondR);
      nonConstPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::eflowRec_DELTA_ALPHA, m_deltaAlpha);
      nonConstPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::eflowRec_HOT_STRIP_FRAC, m_HotStripFrac);
      nonConstPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::eflowRec_THREE_CELL_STRIP_FRAC, m_ThreeCellStripFrac);

      nonConstPFO->setBDTPi0Score(mva_val);
      
    }//if valid cluster pointer
    else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get a pointer to a CaloCluster" << endreq;
    
  }//loop on eflowObjects
  
  return StatusCode::SUCCESS;
}

StatusCode eflowTauPi0TaggerTool::finalize(){ 

  if (m_Reader) delete m_Reader ; 
  return StatusCode::SUCCESS;

}
