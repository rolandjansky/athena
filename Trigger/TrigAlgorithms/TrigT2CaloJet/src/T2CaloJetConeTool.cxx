/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     T2CaloJetConeTool.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  November, 2005

PURPOSE: Cone Tool for all T2CaloJet.  Does simple cone alg after data
         preparation.

********************************************************************/
#include <math.h>
#include "TrigT2CaloJet/T2CaloJetConeTool.h"
#include "TrigCaloEvent/TrigT2Jet.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/IToolSvc.h"
#include "AthenaKernel/Timeout.h"

#include <sstream>


T2CaloJetConeTool::T2CaloJetConeTool(const std::string& type,
				     const std::string& name,
				     const IInterface* parent):
  T2CaloJetBaseTool(type, name, parent),
  m_inCone(0),
  m_outCone(0),
  m_totalCell(0),
  m_pTimerService(NULL),
  m_fcal_timer(NULL),
  m_other_timer(NULL)
{  
  declareProperty("numberOfIterations", m_nIterations = 1);
  declareProperty("coneRadius", m_coneRadius = 0.7);
  declareProperty("coneFJRadius", m_fj_cone_sizes);
  // cleaning:
  declareProperty("cellQualityThresholdLAr", m_cellQualityThresholdLAr = 4000);   // for jet cleaning
  declareProperty("leadingCellFraction", m_leadingCellFraction = 0.9);            // for jet cleaning, e.g.: n90 -> leadingCellFraction = 0.9
}

T2CaloJetConeTool::~T2CaloJetConeTool() 
{
}

StatusCode T2CaloJetConeTool::initialize() 
{
  MsgStream log(msgSvc(), name());
  // Initialize timing service
  if( service( "TrigTimerSvc", m_pTimerService).isFailure() ) {
    log << MSG::WARNING << name() << ": Unable to locate TrigTimer Service" 
	<< endmsg;
  } 

  if (m_pTimerService){
    //Add timers
    m_fcal_timer = m_pTimerService->addItem("LVL2_FCal_Cone_Time");
    m_other_timer = m_pTimerService->addItem("LVL2_Other_Cone_Time");
  }
  m_inCone=0;
  m_outCone=0;
  m_totalCell=0;
  
  // cleaning:
  m_cellenergies.reserve(8192);

  // from IAlgToolCalo
  m_geometryTool.disable();
  m_data.disable();

  return StatusCode::SUCCESS;
}

StatusCode T2CaloJetConeTool::execute()
{
  return StatusCode::SUCCESS;
}


//StatusCode T2CaloJetConeTool::execute(TrigT2Jet* jet,double etamin, double etamax, double phimin, double phimax)
//StatusCode T2CaloJetConeTool::execute(TrigT2Jet* jet,double /*etamin*/, double /*etamax*/, double /*phimin*/, double /*phimax*/)
StatusCode T2CaloJetConeTool::execute(TrigT2Jet* jet, const IRoiDescriptor& /*roi*/ )
{

  MsgStream mLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
  if(outputLevel <= MSG::DEBUG)
     mLog << MSG::DEBUG << " executing T2CaloJetConeTool " << endmsg;


  // reset error
  m_error = 0x0;

  // do cone iterations on jet. 
  bool isFCal = fabs(jet->eta())>3.2 ? true : false;
  if(isFCal){m_fcal_timer->start(); }
  else{ m_other_timer->start(); }
  for(int i=0; i<m_nIterations; i++){
    this->coneIteration(jet,i);
    if ( Athena::Timeout::instance().reached() ) {
       mLog << MSG::ERROR << "Timeout reached in coneIteration" << endmsg;
       return StatusCode::FAILURE;
    }
  }
  if(isFCal){m_fcal_timer->stop(); }
  else{ m_other_timer->stop(); }
  
  if(m_fj_cone_sizes.size()==0||m_fj_cone_sizes.size()>3){
    mLog << MSG::WARNING << "FJ cone sizes are incorrectly setup.  Using 1/7/4 technique." << endmsg;
  }
  return StatusCode::SUCCESS;
}

void T2CaloJetConeTool::coneIteration(TrigT2Jet* jet, int coneIter) 
{
  MsgStream mLog(msgSvc(), name());

  std::vector<Trig3Momentum>* grid = jet->grid();
  
  std::vector<Trig3Momentum>::iterator gridItr = grid->begin();
  std::vector<Trig3Momentum>::iterator gridEnd = grid->end();
  
  double coneR = m_coneRadius;

  /////////////////////////
  // FCal cone algorithm //
  /////////////////////////
  if(fabs(jet->eta())>3.2){
    if(m_fj_cone_sizes.size()==0||m_fj_cone_sizes.size()>3){
      mLog << MSG::DEBUG << "FJ cone sizes are incorrectly setup.  Using 1/7/4 technique." << endmsg;
      // Cone 1.0/0.7/0.4 Technique
      //  -Find inital seed looking at the full eta window
      //  -Subsequent iterations reduce cone size to 0.7 then 0.4 (or default)
      if(fabs(jet->eta())>3.2&&coneIter==0){
	      coneR=1;
      } else if(fabs(jet->eta())>3.2&&coneIter==1){
	      coneR=0.7;
      } 
    } else {
      coneR = m_fj_cone_sizes.at(coneIter);
    }
    mLog << MSG::DEBUG << "Iteration " << coneIter 
	 << ", using cone size: " << coneR << endmsg;
  }
  double dEta = 0, dPhi=0;
  
  //Check if in FCal, if so find most energetic cell
  //double max_e(-999);
  double max_eta = jet->eta();
  double max_phi = jet->phi();
 
  //Set jet eta & phi
  jet->setEta(max_eta);
  jet->setPhi(max_phi);

  // activate cells in cone
  for(; gridItr!=gridEnd; gridItr++){
    dEta = fabs(jet->eta() - (*gridItr).eta());
    dPhi = fabs(jet->phi() - (*gridItr).phi());
    if (dPhi > M_PI) dPhi = 2*M_PI - dPhi;
    
    if( dEta*dEta + dPhi*dPhi < coneR*coneR ) {
      m_inCone++;
      (*gridItr).setInCone(true);
    } else {
      (*gridItr).setInCone(false);
      m_outCone++;
    }
    m_totalCell++;
  }

  // calculate e, eta, phi.
  gridItr = grid->begin();
  double eTot=0, eta=0, phi=0, tmpE=0, eThresh=0, tmpPhi=0;
  double ehad=0, eem=0;
  
  double tmpdPhi=0, dphi=0;
  
  // cleaning:
  // initialize cleaning variables
  int nLeadingCells = 0;            // number of leading cells
  float hecf = 0.;                  // fraction of jet energy in hec
  float jetQuality = 0.;            // quality of the jet
  float emf = 0.;                   // fraction of jet energy in emc
  float jetTimeCells = 0.;          // jet time calculated from cells
  
  // helper functions / variables for cleaning:
  m_cellenergies.clear();
  float HECEnergy = 0.;
  float EinBadCells = 0.;
	bool isTile = false;
	bool isLAr = false;
	bool qualityTimeIsDefined = false;
	float totaltime = 0.;
	float norm = 0.;
	float cellNorm = 0.;
	float totCellE = 0.;     // sum of energy in LAr cells with quality defined
  
  for(; gridItr!=gridEnd; gridItr++){
    if( !(gridItr->inCone()) ) continue;
    
    // reset flags:
    isTile = false;
		isLAr = false;
		
    tmpE = (*gridItr).e();    
    eTot += tmpE;  // maybe this should be Et weighted  
    
    // Summ contribution to hadronic/EM energy:  
    const CaloSampling::CaloSample s = (*gridItr).caloSample();
    if( s == CaloSampling::PreSamplerB || s == CaloSampling::PreSamplerE || 
	   s == CaloSampling::EMB1 || s == CaloSampling::EME1 || 
	   s == CaloSampling::EMB2 || s == CaloSampling::EME2 || 
	   s == CaloSampling::EMB3 || s == CaloSampling::EME3 ||
	   s == CaloSampling::FCAL0){
	    eem += tmpE;
	    isLAr = true;  // for cleaning
	  }
    if( s == CaloSampling::HEC0 || s ==  CaloSampling::HEC1|| 
	   s == CaloSampling::HEC2 || s == CaloSampling::HEC3 || 
	   s == CaloSampling::TileBar0 || s == CaloSampling::TileExt0 || 
	   s == CaloSampling::TileBar1 || s == CaloSampling::TileExt1 || 
	   s == CaloSampling::TileBar2 || s == CaloSampling::TileExt2 || 
	   s == CaloSampling::TileGap1 || s == CaloSampling::TileGap2 || 
	   s == CaloSampling::TileGap3 || s == CaloSampling::FCAL1 || 
	   s == CaloSampling::FCAL2)
	    ehad += tmpE;
    
    if (tmpE>0){ // threshold from job options?
      eThresh += tmpE;    
      eta += tmpE*(*gridItr).eta();
      tmpPhi = (*gridItr).phi();      

      tmpdPhi = tmpPhi - jet->phi();   // w.r.t Jet
      if (tmpdPhi > M_PI) tmpdPhi -= 2*M_PI;    
      if (tmpdPhi < -1*M_PI) tmpdPhi += 2*M_PI; 

      dphi += tmpE*tmpdPhi; // this needs the wrap-around check

    }
    
    // cleaning:
    m_cellenergies.push_back( tmpE );
    if(	s == CaloSampling::HEC0 || s == CaloSampling::HEC1 || 
				s == CaloSampling::HEC2 || s == CaloSampling::HEC3 ) {
        HECEnergy += tmpE;
        isLAr = true;
		}
    if( s == CaloSampling::FCAL1 || s == CaloSampling::FCAL2 ){
			isLAr = true;
	  }
    if( s == CaloSampling::TileBar0 || s == CaloSampling::TileBar1 || 
        s == CaloSampling::TileBar2 || s == CaloSampling::TileGap1 ||
        s == CaloSampling::TileGap2 || s == CaloSampling::TileGap3 || 
        s == CaloSampling::TileExt0 || s == CaloSampling::TileExt1 || 
        s == CaloSampling::TileExt2 ) {
          isTile = true;
    }
    
    uint16_t cellprovenance = (*gridItr).provenance();
    // grab cell quality and cell time.
    uint16_t cellquality =  (*gridItr).quality();
    float celltime = (*gridItr).time();
    qualityTimeIsDefined = false;
		
    // check for tile
    if(isTile){
      // cells with bad quality are masked -> don't take tile into account
      //   for jet quality calculation.
      // provenance not set for tile cells. but time is defined.
      // timing:
      cellNorm = tmpE * tmpE;
      totaltime += cellNorm * celltime;
      norm += cellNorm;
    }
    // check for LAr. only take cells into accout which have time and quality defined
    if( (isLAr) && (cellprovenance & 0x2000) ){
      qualityTimeIsDefined = true;
      totCellE += tmpE;
      // timing:
      cellNorm = tmpE * tmpE;
      totaltime += cellNorm * celltime;
      norm += cellNorm;
    }
				
    if( (isLAr) && (qualityTimeIsDefined) && (cellquality > m_cellQualityThresholdLAr) ){
      EinBadCells += tmpE;
    }
    
  } // end loop over cells
  
  if(eThresh != 0) {
    eta /= eThresh;
    phi = dphi/eThresh + jet->phi(); // recover absolute value
  
    // phi must be between -pi and pi :
    if(phi > M_PI) phi -= 2.0* M_PI;
    if(phi < -1.0*M_PI) phi += 2.0*M_PI;
  }

  // calculate the jet cleaning variables:
  // -------------------------------------

	// sort cell-energy vector. (most energetic cell at end of vector)
	std::sort(m_cellenergies.begin(), m_cellenergies.end());
	float sumE = 0.;
	for(int k = m_cellenergies.size()-1; k > -1; k--){
		sumE += m_cellenergies[k];
		nLeadingCells++;
		if(sumE > m_leadingCellFraction*eTot){
			break;
		}
	}

	float DELTA = 0.001;     // distance to compare floats
	if(fabs(eTot)>DELTA){
		hecf = HECEnergy/eTot;
		emf = eem/eTot;
	}
	if(fabs(totCellE)>DELTA){
		jetQuality = EinBadCells/totCellE;
	}
	if(fabs(norm)>DELTA){
		jetTimeCells = totaltime/norm;
	}


  jet->setE(eTot);
  jet->setEem0(eem);
  jet->setEhad0(ehad);
  jet->setEta(eta);
  jet->setPhi(phi);
  
  // cleaning:
  jet -> setNLeadingCells(nLeadingCells);
  jet -> setHecf(hecf);
  jet -> setJetQuality(jetQuality);
	jet -> setEmf(emf);
	jet -> setJetTimeCells(jetTimeCells);
}

StatusCode T2CaloJetConeTool::finalize()
{
  m_cellenergies.clear();
  return StatusCode::SUCCESS;
}
