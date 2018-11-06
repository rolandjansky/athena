/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TauAllCaloDRFex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTau
//
// AUTHOR:   Olga Igonkina (Nikhef), M. Pilar Casado (IFAE), Mogens Dam (NBI)
//           based on TauEmEnFex.cxx
//
// CREATED: Jun-09
//
// DESCRIPTION: Tool to compute LVL2 Calo tau variables at
//              EM sampling 2
// ********************************************************************

#include "CaloIdentifier/LArEM_ID.h"

#include "CaloEvent/CaloCluster.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigT2CaloTau/TauAllCaloDRFex.h"
#include "TrigT2CaloCommon/Calo_Def.h"
#include "TrigSteeringEvent/Enums.h"

#define NEG_ENERGY_CLUSTER HLT::Reason::USERDEF_1
#define NULL_ENERGY_SAMPLING HLT::Reason::USERDEF_2

TauAllCaloDRFex::TauAllCaloDRFex(const std::string& type, const std::string& name, const IInterface* parent) : IAlgToolCalo(type, name, parent)
{
    declareProperty("StripEthr",      m_stripEthr);
    declareProperty("dRSeed",         m_dRSeed,         "dR cut for reconstruction of the seed ");    
    declareProperty("CaloNoiseTool",  m_noiseTool,      "Tool Handle for noise tool");
    declareProperty("applyNoiseCut",  m_applyNoiseCut,  "swithch on/off noise suppression");
    declareProperty("noiseNSigmaCut", m_noiseNSigmaCut, "number of sigmas for 2 sided noise substraction");
    declareProperty("hecQualityCut",  m_hecQualityCut,  "Control for HEC Quality");
    declareProperty("defaultWidth",   m_defaultWidth,   "default width to be saved");
    declareProperty("dRConeNarrow",   m_dRConeNarrow,   "dR cut for full region (Narrow) ");
    declareProperty("dRConeMedium",   m_dRConeMedium,   "dR cut for full region (Medium) ");
    declareProperty("dRConeWide",     m_dRConeWide,     "dR cut for full region (Wide) ");
    m_saveCells = false;
}

TauAllCaloDRFex::~TauAllCaloDRFex()
{
}

StatusCode TauAllCaloDRFex::initialize()
{
  StatusCode sc = IAlgToolCalo::initialize();
  if( sc.isFailure() ) return sc;
  msg()  << MSG::INFO << "REGTEST initialized with:" << endmsg;
  msg()  << MSG::INFO << "REGTEST dRSeed="           << m_dRSeed         << endmsg;
  msg()  << MSG::INFO << "REGTEST StripEthr="        << m_stripEthr      << endmsg;
  msg()  << MSG::INFO << "REGTEST CaloNoiseTool="    << m_noiseTool      << endmsg;
  msg()  << MSG::INFO << "REGTEST applyNoiseCut="    << m_applyNoiseCut  << endmsg;
  msg()  << MSG::INFO << "REGTEST noiseNSigmaCut="   << m_noiseNSigmaCut << endmsg;
  msg()  << MSG::INFO << "REGTEST hecQualityCut="    << m_hecQualityCut  << endmsg;
  msg()  << MSG::INFO << "REGTEST defaultWidth="     << m_defaultWidth   << endmsg;
  msg()  << MSG::INFO << "REGTEST dRConeNarrow="     << m_dRConeNarrow   << endmsg;
  msg()  << MSG::INFO << "REGTEST dRConeMedium="     << m_dRConeMedium   << endmsg;
  msg()  << MSG::INFO << "REGTEST dRConeWide="       << m_dRConeWide     << endmsg;
  
  if(   m_saveCells && msg().level() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "REGTEST: store cells with Et> " << m_cellkeepthr << endmsg;
  
  if ( !m_timersvc.empty() )
    {     
      m_timer[0] = m_timersvc->addItem("T2CaloTau.Tools.Total");
      m_timer[1] = m_timersvc->addItem("T2CaloTau.Tools.RegSel");
      m_timer[2] = m_timersvc->addItem("T2CaloTau.Tools.DataPrep");
      m_timer[3] = m_timersvc->addItem("T2CaloTau.Tools.Algorithmic");
      m_timer[4] = m_timersvc->addItem("T2CaloTau.Tools.Saving");
    }
  
  // noise suppression
  if(m_applyNoiseCut!=0)
    {
      if(m_noiseTool.retrieve().isFailure())
        {
          msg() << MSG::FATAL << "Unable to find CaloNoiseTool" << endmsg;
          return StatusCode::FAILURE;
        }
    }
  
  return sc;
}


HLT::ErrorCode TauAllCaloDRFex::execute( TrigTauCluster &rtrigTauCluster,
					 const IRoiDescriptor& roi,
					 const CaloDetDescrElement*& /*caloDDE*/)
{
  
  // Time total AlgTool time 
  if (!m_timersvc.empty()) m_timer[0]->start();      
  // reset error
  m_error = 0x0;
  
  //these aren't used:
  //  phiWidthEM = 0.;
  //  etaWidthEM = 0.;

  //samplings
  const unsigned int NSamplings = 7;
  int samplings[NSamplings]     = {0,1,2,3,0,1,2};
  DETID detectorID[NSamplings]  = {TTEM,TTEM,TTEM,TTEM,TTHEC,TTHEC,TTHEC};
  

  // Time to access RegionSelector
  if (!m_timersvc.empty()) m_timer[1]->start();      
  if (!m_timersvc.empty()) m_timer[1]->pause();      
  // Time to access Collection (and ByteStreamCnv ROBs)
  if (!m_timersvc.empty()) m_timer[2]->start();      
  if (!m_timersvc.empty()) m_timer[2]->pause();    

  
  // Algorithmic time
  if (!m_timersvc.empty()) m_timer[3]->start();      
  double seedPhi = roi.phi();
  double seedEta = roi.eta();
  
  if (msg().level()<=MSG::DEBUG)
    msg() << MSG::DEBUG << " Seed position (L1) eta/phi=" << seedEta << "/" << seedPhi << endmsg;
  

  double energyEta = 0.;
  double energyPhi = 0.;
  
  // Variables to take care of phi wrap-around
  double energyNegPhi     = 0.; 
  double energyNegPhiConv = 0.; 
  double energyPosPhi     = 0.; 
  
  double EnergyMediumPosPhi = 0;
  double EnergyMediumNegPhi = 0;
  
  //  double etamin = std::max(-2.5,RoIeta - etaWidth);
  //  double etamax = std::min( 2.5,RoIeta + etaWidth);
    
  //  double phimin = RoIphi - phiWidth;
  //  double phimax = RoIphi + phiWidth;

  //  while (phimin < 0) phimin += 2. * M_PI;
  //  while (phimax > 2*M_PI) phimax -= 2. * M_PI;


  //------------------ step 1 : clusterization -----------------------------------
  // loop over all samplings 
  if (msg().level()<=MSG::DEBUG)
    msg() << MSG::DEBUG << " Start clusterization "<< endmsg;

  for(unsigned int is=0; is<NSamplings;++is)
    {
      if (msg().level()<=MSG::DEBUG)
	msg() << MSG::DEBUG << "LAr sampling "<< samplings[is]<< endmsg;

      if (!m_timersvc.empty()) { m_timer[3]->pause();  m_timer[1]->resume();}
      //      m_data->RegionSelector(samplings[is], etamin, etamax, phimin, phimax, detectorID[is]);
      m_data->RegionSelector(samplings[is], roi, detectorID[is]);
      if (!m_timersvc.empty()) {m_timer[1]->pause();  m_timer[2]->resume(); }
      if ( m_data->LoadCollections(m_iBegin,m_iEnd).isFailure() ){
	if (msg().level()<=MSG::DEBUG)
	  msg() << MSG::DEBUG << " can not LoadCollections " << *m_iBegin << " " << *m_iEnd << endmsg;
	return  HLT::TOOL_FAILURE; 
      }
      m_error|=m_data->report_error(); 
      if (!m_timersvc.empty()){ m_timer[2]->pause();  m_timer[3]->resume(); }
      
      for(m_it = m_iBegin;m_it != m_iEnd; ++m_it)
	{
	  const LArCell*  cell = *m_it;
	  double etaCell = cell->eta();
	  double phiCell = cell->phi();
          double dR;
          if( ! getdR(seedPhi, seedEta, etaCell, phiCell, m_dRSeed, dR) )
              continue;

	  float energyCell = cell->energy();
	  if (m_applyNoiseCut) {
	    double rms = m_noiseTool->getNoise(cell, ICalorimeterNoiseTool::TOTALNOISE);
	    if ( fabs(energyCell) < m_noiseNSigmaCut * rms) 
	      continue;
	  }

	  if (detectorID[is]==TTHEC) { 
	    if (energyCell < -5e3) continue;
	    if ((m_hecQualityCut!=0) && (((cell->quality())&0xffff) > m_hecQualityCut) ) continue;
	  }
	  energyEta += energyCell * etaCell ;
	  
	  if (phiCell > 0.){
	    EnergyMediumPosPhi += energyCell;
	    energyPosPhi       += energyCell * phiCell;
	  } else {
	    energyNegPhi       += energyCell * phiCell;
	    energyNegPhiConv   += energyCell * (phiCell+2.0*M_PI);
	    EnergyMediumNegPhi += energyCell;
	  }
	  
	} // end of loop over EM sampling   
    } // end loop over EM LAr
  
  // Region Selector, no sample needed
  // Get detector offline ID's for Collections
  
  //  m_data->RegionSelector(0,etamin,etamax,phimin,phimax,TILE);
  m_data->RegionSelector( 0, roi, TILE );
  if (!m_timersvc.empty()) m_timer[1]->pause();

  for (unsigned int iR=0;iR< m_data->TileContSize();iR++)
    {
      if (!m_timersvc.empty()) m_timer[2]->resume();
      // For the first sample you will create the containers
      // For the others no
      if ( m_data->LoadCollections(m_itBegin,m_itEnd,iR,!iR).isFailure() ){
	return  HLT::TOOL_FAILURE;
      }
      m_error|=m_data->report_error(); 
      // Finished to access Collection
      if (!m_timersvc.empty()) { m_timer[2]->pause(); m_timer[3]->resume(); }
      
      
      for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt)
	{
	  const CaloCell* cell = (*m_itt);

	  double etaCell = cell->eta();
	  double phiCell = cell->phi();
          double dR;
          if (! getdR(seedPhi, seedEta, etaCell, phiCell, m_dRSeed, dR) )
              continue;

	  float energyCell = cell->energy();
	  
	  if (m_applyNoiseCut) {
	      if (! m_noiseTool->isEOverNSigma( cell, m_noiseNSigmaCut, 
						ICalorimeterNoiseTool::MAXSYMMETRYHANDLING, 
						ICalorimeterNoiseTool::TOTALNOISE) )
                  continue;
          }
	  
	  energyEta += energyCell * etaCell ;
	  
	  if (msg().level()<=MSG::DEBUG)
	    msg() << MSG::DEBUG << "take cell E="<<energyCell << " Eta/Phi="<<etaCell << "/"<<phiCell<< endmsg;
	  
	  if (phiCell > 0.){
	    EnergyMediumPosPhi += energyCell;
	    energyPosPhi +=energyCell * phiCell;
	  } else {
	    energyNegPhi +=energyCell * phiCell;
	    energyNegPhiConv += energyCell * ( phiCell+2.0*M_PI);
	    EnergyMediumNegPhi += energyCell;
	  }
	  
	} // end of loop over Tile cells
    } // end loop over Tile samplings
  

  


  // End options for cluster position
  //---------------- step 2 calculation of variables
  
  HLT::ErrorCode StatError = HLT::OK; // define flag for cluster with negative energy or sampling with 0 energy.
  
  // Phi wrap-around
  if ( (EnergyMediumNegPhi + EnergyMediumPosPhi) > 0. ){           // dont divide by zero
    energyEta /= (EnergyMediumNegPhi + EnergyMediumPosPhi) ;
    energyPhi = calcEnergyPhi(energyNegPhi, energyPosPhi, EnergyMediumNegPhi, EnergyMediumPosPhi, energyNegPhiConv);
  } else {
    if (msg().level()<=MSG::DEBUG)
      msg() << MSG::DEBUG <<"REGTEST problems finding seed: negative energy = "<< EnergyMediumNegPhi + EnergyMediumPosPhi 
	       <<" eta/phi = "<< energyEta<<" / "<<energyPhi <<" . Seed set to L1 direction: eta/phi = "
	       <<roi.eta()<<" / "<<roi.phi()<<  endmsg;
    energyEta = roi.eta() ; // if Cluster energy is null or negative, set L1 position
    energyPhi = roi.phi() ;
    SetClusterError(TAUCLUSTERROR::FAILSEED);

    StatError =  HLT::ErrorCode(HLT::Action::CONTINUE, NEG_ENERGY_CLUSTER);
  }
  
  if (msg().level()<=MSG::DEBUG)
    msg() << MSG::DEBUG <<"REGTEST Pre-seed eta/phi " << seedEta<<"/"<<seedPhi << " => Cluster eta/phi = "<< energyEta << "/"<< energyPhi << endmsg;
  
  while (energyPhi < -M_PI) energyPhi= energyPhi + 2. * M_PI;
  while (energyPhi > M_PI)  energyPhi= energyPhi - 2. * M_PI;
  
  float dPhiL1 = fabs(energyPhi - seedPhi);
  if( dPhiL1 > M_PI ) dPhiL1 = 2*M_PI - dPhiL1;  
  
  // If seed is too far from pre seed, means that something has happened, probably too small energy deposition
  if ( fabs(energyEta - seedEta)> m_dRSeed || fabs(dPhiL1)> m_dRSeed){
    if (msg().level()<=MSG::DEBUG)
      msg() << MSG::DEBUG <<"REGTEST problems finding seed: eta/phi = "<<energyEta<<" / "<<energyPhi
	       <<" too far from L1 eta/phi = "<< seedEta<<" / "<<seedPhi 
	       <<" . Energy = "<< EnergyMediumNegPhi + EnergyMediumPosPhi<<  endmsg;
    energyEta=roi.eta();
    energyPhi=roi.phi();
    // this a similar case as above: energy is too small to calculate a good seed position. they should be monitored together.
    SetClusterError(TAUCLUSTERROR::FAILSEED);
    StatError =  HLT::ErrorCode(HLT::Action::CONTINUE, NEG_ENERGY_CLUSTER); 
  }
  
  const TrigTauClusterDetails* pDetailsConst = rtrigTauCluster.clusterDetails();
  TrigTauClusterDetails* pDetails = const_cast<TrigTauClusterDetails*> (pDetailsConst); // why do not we have function for that, if needed?
  
  if (!m_timersvc.empty()) m_timer[3]->pause();      
  // Time to store cluster quantities
  if (!m_timersvc.empty()) m_timer[4]->start();      
  rtrigTauCluster.setEta(energyEta);
  rtrigTauCluster.setPhi(energyPhi);
  if (!m_timersvc.empty()) m_timer[4]->pause();      
  if (!m_timersvc.empty()) m_timer[3]->resume();      
  
  ClearClusterError(TAUCLUSTERROR::EMS0E0); // this bit might at some point be set by the common data access
  ClearClusterError(TAUCLUSTERROR::EMS1E0); // this bit might at some point be set by the common data access
  ClearClusterError(TAUCLUSTERROR::EMS2E0); // this bit might at some point be set by the common data access
  ClearClusterError(TAUCLUSTERROR::EMS3E0); // this bit might at some point be set by the common data access
  
  ClearClusterError(TAUCLUSTERROR::HADS1E0);  
  ClearClusterError(TAUCLUSTERROR::HADS2E0);  
  ClearClusterError(TAUCLUSTERROR::HADS3E0);  
  
  if (msg().level()<=MSG::DEBUG)
	msg() << MSG::DEBUG << " Start shape calculation "<< endmsg;
  

  int    numStripCell           = 0;
  int    numTotalCells          = 0;
                                
  int    nEmCellsNarrow         = 0;
  int    nEmCellsMed            = 0;
  int    nEmCellsWide           = 0;

  int    nHadCellsNarrow[3]     = {0,0,0};
  int    nHadCellsMed[3]        = {0,0,0};
  int    nHadCellsWide [3]      = {0,0,0};
  
  
  double emRadiusNarrow         = 0;
  double emRadiusMed            = 0;
  double emRadiusWide           = 0;

  double hadRadiusNarrow[3]     = {0.0,0.0,0.0};
  double hadRadiusMed[3]        = {0.0,0.0,0.0};
  double hadRadiusWide[3]       = {0.0,0.0,0.0};


  double emEnergyNarrow         = 0;
  double emEnergyMed            = 0;
  double emEnergyWide           = 0;

  double hadEnergyNarrow[3]     = {0.0,0.0,0.0};
  double hadEnergyMed[3]        = {0.0,0.0,0.0};
  double hadEnergyWide[3]       = {0.0,0.0,0.0};


  double emWeightEtaNarrow      = 0.0;
  double emWeightEtaMed         = 0.0;
  double emWeightEtaWide        = 0.0;

  double hadWeightEtaNarrow[3]  = {0.0,0.0,0.0};
  double hadWeightEtaMed[3]     = {0.0,0.0,0.0};
  double hadWeightEtaWide[3]    = {0.0,0.0,0.0};


  double emWeightEta2Narrow     = 0.0;
  double emWeightEta2Med        = 0.0;
  double emWeightEta2Wide       = 0.0;

  double hadWeightEta2Narrow[3] = {0.0,0.0,0.0};
  double hadWeightEta2Med[3]    = {0.0,0.0,0.0};
  double hadWeightEta2Wide[3]   = {0.0,0.0,0.0};


  
  



  //loop over LAr samplings
  for(unsigned int is=0; is<NSamplings;++is){
    
    emRadiusNarrow     = 0.;
    emRadiusMed        = 0.;
    emRadiusWide       = 0.;
                       
    nEmCellsNarrow     = 0;
    nEmCellsMed        = 0;
    nEmCellsWide       = 0;
                       
    emRadiusNarrow     = 0.;
    emRadiusMed        = 0.;
    emRadiusWide       = 0.;
                       
    emEnergyNarrow     = 0.;
    emEnergyMed        = 0.;
    emEnergyWide       = 0.;

    emWeightEtaNarrow  = 0.;
    emWeightEtaMed     = 0.;
    emWeightEtaWide    = 0.;

    emWeightEta2Narrow = 0.;
    emWeightEta2Med    = 0.;
    emWeightEta2Wide   = 0.;

    
    //get data
    if (!m_timersvc.empty()){ m_timer[3]->pause();  m_timer[1]->resume(); }

    // m_data->RegionSelector(samplings[is],etamin,etamax,phimin,phimax,detectorID[is]);
    m_data->RegionSelector( samplings[is], roi, detectorID[is] );

    if (!m_timersvc.empty()) { m_timer[1]->pause(); m_timer[2]->resume(); }

    if ( m_data->LoadCollections(m_iBegin,m_iEnd).isFailure() ){
      if (msg().level()<=MSG::DEBUG)
          msg() << MSG::DEBUG << " can not LoadCollections " << *m_iBegin << " " << *m_iEnd << endmsg;
      return  HLT::TOOL_FAILURE; 
    }
    m_error|=m_data->report_error(); 
    if (!m_timersvc.empty()){ m_timer[2]->pause();   m_timer[3]->resume(); }
    
    if ( m_saveCells ){ 
      m_data->storeCells(m_iBegin,m_iEnd,*m_CaloCellContPoint,m_cellkeepthr,100000);
    } 
  

    //loop over cells per sampling
    for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {
      const LArCell*  cell = *m_it;
      double etaCell = cell->eta();
      double phiCell = cell->phi();          
      double dR      = 0.0;
      //effectively applying the dR<m_dRConeWide cut
      if ( !getdR(energyPhi, energyEta, etaCell, phiCell,m_dRConeWide , dR) ) continue;

      float energyCell = cell->energy();
      
      
      // Count cells
      if ( is < 4 ) {
        nEmCellsWide += 1;
        if( dR < m_dRConeMedium ) {
          nEmCellsMed += 1;
          if( dR < m_dRConeNarrow )
              nEmCellsNarrow += 1;
        }
      } else {
        nHadCellsWide[samplings[is]] += 1;
        if( dR < m_dRConeMedium ) {
          nHadCellsMed[samplings[is]] += 1;
          if( dR < m_dRConeNarrow )
              nHadCellsNarrow[samplings[is]] += 1;
        }
      }        

      //count n cells
      numTotalCells = rtrigTauCluster.numTotCells();
      numTotalCells = numTotalCells + 1;
      rtrigTauCluster.setNumTotCells(numTotalCells);
      
      //Noise subtraction
      if (m_applyNoiseCut) {
        double rms = m_noiseTool->getNoise(cell, ICalorimeterNoiseTool::TOTALNOISE);
        if ( fabs(energyCell) < m_noiseNSigmaCut * rms) 
          continue;
      }
      
      if (detectorID[is]==TTHEC) {
        if (energyCell < -5e3) continue;
        if ((m_hecQualityCut!=0) && (((cell->quality())&0xffff) > m_hecQualityCut) ) continue;
      }           

      if ( is < 4 ) {
        emRadiusWide     += energyCell * dR;
        emEnergyWide     += energyCell; 
        emWeightEta2Wide += energyCell * etaCell * etaCell;
        emWeightEtaWide  += energyCell * etaCell;
        
      } else {
        hadRadiusWide[samplings[is]]     += energyCell * dR;
        hadEnergyWide[samplings[is]]     += energyCell; 
        hadWeightEta2Wide[samplings[is]] += energyCell * etaCell * etaCell;
        hadWeightEtaWide[samplings[is]]  += energyCell * etaCell;
      }

      if ( (is==1) && (energyCell > m_stripEthr ) ) numStripCell += 1;  

      
      if( dR < m_dRConeMedium ) {
	if( is < 4 ) {
          emRadiusMed     += energyCell * dR; 
          emEnergyMed     += energyCell;
          emWeightEta2Med += energyCell * etaCell * etaCell;
          emWeightEtaMed  += energyCell * etaCell;
        } else {
          hadRadiusMed[samplings[is]]     += energyCell * dR;
          hadEnergyMed[samplings[is]]     += energyCell;
          hadWeightEta2Med[samplings[is]] += energyCell * etaCell * etaCell;
          hadWeightEtaMed[samplings[is]]  += energyCell * etaCell;
        }
        
        if( dR < m_dRConeNarrow ) {
          if( is < 4 ) {
            emRadiusNarrow     += energyCell * dR; 
            emEnergyNarrow     += energyCell;
            emWeightEta2Narrow += energyCell * etaCell * etaCell;
            emWeightEtaNarrow  += energyCell * etaCell;
          } else {
            hadRadiusNarrow[samplings[is]]     += energyCell * dR;
            hadEnergyNarrow[samplings[is]]     += energyCell;
            hadWeightEta2Narrow[samplings[is]] += energyCell * etaCell * etaCell;
            hadWeightEtaNarrow[samplings[is]]  += energyCell * etaCell;
          }
        }
      }
    } // end of loop over cells  

    
    //Set clusterWidth and emRadius for EM Samplings
    if ( is < 4 ) {

      double clusterWidthWide;
      double clusterWidthMed;
      double clusterWidthNarrow;
      
      //Wide
      if ( (emEnergyWide > 0.) && (nEmCellsWide > 0) ) {
        clusterWidthWide = 
            (emWeightEta2Wide/emEnergyWide) -
            (emWeightEtaWide/emEnergyWide)*
            (emWeightEtaWide/emEnergyWide);
        
        clusterWidthWide > 0.? clusterWidthWide = 
            sqrt(clusterWidthWide) : -99.;
        
        emRadiusWide = emRadiusWide / emEnergyWide ;
      } else { 
        if(nEmCellsWide == 0) {
          clusterWidthWide = 0.;
          emRadiusWide     = 0. ;

	  /// What is this code for?? why is it hardcoded with values like eta=1.8 ???
	  
          // if E==0 in a dR<0.3 region, probably means problems with unpacking.
          if ((is < 4) && 
              (samplings[is]==0) && 
              ((fabs(energyEta) - m_dRConeWide) < (1.8 - (0.025/2) - 0.05) ) &&
              ((fabs(roi.etaMinus())<(1.8-1e-7)) || (fabs(roi.etaPlus())<(1.8-1e-7)) ) 
              ) {
            SetClusterError(TAUCLUSTERROR::EMS0E0);
            StatError =  HLT::ErrorCode(HLT::Action::CONTINUE, NULL_ENERGY_SAMPLING);
          }
          if ((is < 4) && (samplings[is]==1)) {
            SetClusterError(TAUCLUSTERROR::EMS1E0);
            StatError =  HLT::ErrorCode(HLT::Action::CONTINUE, NULL_ENERGY_SAMPLING);
          }
          if ((is < 4) && (samplings[is]==2)) {
            SetClusterError(TAUCLUSTERROR::EMS2E0);
            StatError =  HLT::ErrorCode(HLT::Action::CONTINUE, NULL_ENERGY_SAMPLING);
          } 
          if ((is < 4) && (samplings[is]==3)) {
            SetClusterError(TAUCLUSTERROR::EMS3E0);          
            StatError =  HLT::ErrorCode(HLT::Action::CONTINUE, NULL_ENERGY_SAMPLING);
          }
        }
        else {
          clusterWidthWide = -99.;
          emRadiusWide     = -99.;
        }
      }
      
      //Medium
      if ( (emEnergyMed > 0.) && (nEmCellsMed > 0) ) {
        clusterWidthMed = 
	  (emWeightEta2Med/emEnergyMed) -
	  (emWeightEtaMed/emEnergyMed)*
	  (emWeightEtaMed/emEnergyMed);
        
        clusterWidthMed > 0.? clusterWidthMed = 
            sqrt(clusterWidthMed) : -99.;
        
        emRadiusMed = emRadiusMed / emEnergyMed ;
      } else { 
        if( nEmCellsMed == 0 ) {
          clusterWidthMed = 0.;
          emRadiusMed     = 0. ;
        } else {
          clusterWidthMed = -99.;
          emRadiusMed     = -99.;
        }
      }
      
    
      //Narrow
      if ( (emEnergyNarrow > 0.) && (nEmCellsNarrow > 0) ) {
        clusterWidthNarrow = 
	  (emWeightEta2Narrow/emEnergyNarrow) -
	  (emWeightEtaNarrow /emEnergyNarrow)*
	  (emWeightEtaNarrow /emEnergyNarrow);
        
        clusterWidthNarrow > 0.? clusterWidthNarrow = 
	  sqrt(clusterWidthNarrow) : -99.;
        
        emRadiusNarrow = emRadiusNarrow / emEnergyNarrow ;
      } else { 
        if( nEmCellsNarrow == 0 ) {
          clusterWidthNarrow = 0.;
          emRadiusNarrow     = 0. ;
	} else {
          clusterWidthNarrow = -99.;
          emRadiusNarrow     = -99.;
        }
      }
      
      if (!m_timersvc.empty()){ m_timer[3]->pause();  m_timer[4]->resume(); }
      
      float emRad     = -1.0;
      float clusWidth = -1.0;
      switch (m_defaultWidth) {
      case 0:
        emRad     = emRadiusNarrow;
        clusWidth = clusterWidthNarrow;
        break;
      case 1:
        emRad     = emRadiusMed;
        clusWidth = clusterWidthMed;
        break;
      case 2:
        emRad     = emRadiusWide;
        clusWidth = clusterWidthWide;
        break;
      }
      
      pDetails->setEMRadius      (samplings[is],emRad);
      pDetails->setEMenergyWidth (samplings[is],clusWidth);
      
      pDetails->setEMenergyNarrow(samplings[is],emEnergyNarrow);
      pDetails->setEMenergyMedium(samplings[is],emEnergyMed);
      pDetails->setEMenergyWide  (samplings[is],emEnergyWide);
      
      //if samplings==2 in the first 4 samples
      if (samplings[is]==2) {
	if(emEnergyMed != 0) rtrigTauCluster.setIsoFrac( (emEnergyMed - emEnergyNarrow) / emEnergyMed );
        else rtrigTauCluster.setIsoFrac(-99. );
      }
    } 
    if (!m_timersvc.empty()){ m_timer[4]->pause();   m_timer[3]->resume(); }
  } 
  
  if (!m_timersvc.empty()) m_timer[3]->pause();      

  if (!m_timersvc.empty()) m_timer[4]->resume();      
  rtrigTauCluster.setNumStripCells    (numStripCell);
  rtrigTauCluster.setStripWidthOffline(rtrigTauCluster.EMenergyWidth(1));
  rtrigTauCluster.setStripWidth       (rtrigTauCluster.EMenergyWidth(2));
  rtrigTauCluster.setEMRadius2        (rtrigTauCluster.EMRadius(2));
  rtrigTauCluster.setEMenergy         (getEMEnergy(pDetails, m_defaultWidth) );

  if (!m_timersvc.empty()){ m_timer[4]->pause(); m_timer[3]->resume(); }
  if (!m_timersvc.empty()) { m_timer[3]->pause(); m_timer[1]->resume();}

  //  m_data->RegionSelector(0,etamin,etamax,phimin,phimax,TILE);
  m_data->RegionSelector( 0, roi, TILE );
  if (!m_timersvc.empty()) m_timer[1]->pause();

  

  // Tile energy
  for (unsigned int iR=0; iR<m_data->TileContSize(); iR++) {
    
    if (!m_timersvc.empty()) m_timer[2]->resume();
    if ( m_data->LoadCollections(m_itBegin,m_itEnd,iR,!iR).isFailure() ){
      return  HLT::TOOL_FAILURE;
    }
    m_error|=m_data->report_error(); 
    if ( m_saveCells ){ 
      m_data->storeCells(m_itBegin,m_itEnd,*m_CaloCellContPoint,m_cellkeepthr,100000); 
    } 
    if (!m_timersvc.empty()) { m_timer[2]->pause(); m_timer[3]->resume();}
    
    //loop over tile cells
    for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) {
      const CaloCell*  cell = (*m_itt);
      double etaCell = cell->eta();
      double phiCell = cell->phi();
      double dR      = 0.0;
      if (!getdR(energyPhi, energyEta, etaCell, phiCell, m_dRConeWide, dR) )
          continue;

      float energyCell = cell->energy();
      
      
      //samp = CaloSampling::getSampling(*cell);
      //CaloSampling::CaloSample samp = CaloSampling::getSampling(*(*m_itt));
      CaloSampling::CaloSample samp = (*m_itt)->caloDDE()->getSampling();

      int idxsamp = -99;
      if (CaloSampling::TileBar0 == samp ||
	  CaloSampling::TileExt0 == samp) {
	idxsamp = 0; 
      } else if (CaloSampling::TileBar1 == samp ||
		 CaloSampling::TileExt1 == samp) {
	idxsamp = 1;
      } else if (CaloSampling::TileBar2 == samp ||
		 CaloSampling::TileExt2 == samp) {
	idxsamp = 2;
      }else continue;
      
      // Count this cell
      numTotalCells=rtrigTauCluster.numTotCells();
      rtrigTauCluster.setNumTotCells(numTotalCells++);

      // Count cells
      nHadCellsWide[idxsamp] += 1;
      if( dR < m_dRConeMedium ) {
        nHadCellsMed[idxsamp] += 1;
        if( dR < m_dRConeNarrow )
            nHadCellsNarrow[idxsamp] += 1;
      }

      if (m_applyNoiseCut) {
        if (! m_noiseTool->isEOverNSigma( cell, m_noiseNSigmaCut,
                                          ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                          ICalorimeterNoiseTool::TOTALNOISE) )
            continue;
      }
      
      hadRadiusWide[idxsamp]     += energyCell * dR;
      hadEnergyWide[idxsamp]     += energyCell;          
      hadWeightEta2Wide[idxsamp] += energyCell * etaCell * etaCell;
      hadWeightEtaWide[idxsamp]  += energyCell * etaCell;

      if( dR < m_dRConeMedium ) {
        hadRadiusMed[idxsamp]     += energyCell * dR;
        hadEnergyMed[idxsamp]     += energyCell;
        hadWeightEta2Med[idxsamp] += energyCell * etaCell * etaCell;
        hadWeightEtaMed[idxsamp]  += energyCell * etaCell;

        if( dR < m_dRConeNarrow ) {
          hadRadiusNarrow[idxsamp]     += energyCell * dR;
          hadEnergyNarrow[idxsamp]     += energyCell;
          hadWeightEta2Narrow[idxsamp] += energyCell * etaCell * etaCell;
          hadWeightEtaNarrow[idxsamp]  += energyCell * etaCell;
	}
      }
    } // end loop over cells
  } // end loop over collections
  
  
  //Fix had variables
  for(int sampling=0; sampling<3;++sampling){
    // calculate cluster width in a region (Eta/Phi)StripsEM2Nor from jobO
    // ??? ClearClusterError(TAUCLUSTERROR::EMS2E0); // this bit might at some point be set by the common data access
    
    double clusterWidthWide;
    double clusterWidthMed;
    double clusterWidthNarrow;

    //HadWide
    if ( hadEnergyWide[sampling] > 0.  &&  nHadCellsWide[sampling] > 0 ) {
      clusterWidthWide = 
          (hadWeightEta2Wide[sampling]/hadEnergyWide[sampling]) -
          (hadWeightEtaWide[sampling]/hadEnergyWide[sampling])*
          (hadWeightEtaWide[sampling]/hadEnergyWide[sampling]);

      clusterWidthWide > 0.? clusterWidthWide = 
          sqrt(clusterWidthWide) : -99.;
      
      hadRadiusWide[sampling] = hadRadiusWide[sampling] / hadEnergyWide[sampling] ;
    } else {
      if(nHadCellsWide[sampling] == 0) {
        clusterWidthWide         = 0.;
        hadRadiusWide[sampling]  = 0. ;

        // if E==0 in a dR<0.3 region, probably means problems with unpacking.
	if ( sampling==0)  SetClusterError(TAUCLUSTERROR::HADS1E0);
	if ( sampling==1)  SetClusterError(TAUCLUSTERROR::HADS2E0);
	if ( sampling==2)  SetClusterError(TAUCLUSTERROR::HADS3E0);
        StatError =  HLT::ErrorCode(HLT::Action::CONTINUE, NULL_ENERGY_SAMPLING);
      } else {
        clusterWidthWide         = -99.;
        hadRadiusWide[sampling]  = -99.;
      }
    }

    //HadMed
    if ( hadEnergyMed[sampling] > 0.  &&  nHadCellsMed[sampling] > 0 ) {
      clusterWidthMed = 
          (hadWeightEta2Med[sampling]/hadEnergyMed[sampling]) -
          (hadWeightEtaMed[sampling]/hadEnergyMed[sampling])*
          (hadWeightEtaMed[sampling]/hadEnergyMed[sampling]);

      clusterWidthMed > 0.? clusterWidthMed = 
          sqrt(clusterWidthMed) : -99.;
      
      hadRadiusMed[sampling] = hadRadiusMed[sampling] / hadEnergyMed[sampling] ;
    } else {
      if(nHadCellsMed[sampling] == 0) {
        clusterWidthMed         = 0.;
        hadRadiusMed[sampling]  = 0. ;
      } else {
        clusterWidthMed         = -99.;
        hadRadiusMed[sampling]  = -99.;
      }
    }


    //HadNarrow
    if ( hadEnergyNarrow[sampling] > 0.  &&  nHadCellsNarrow[sampling] > 0 ) {
      clusterWidthNarrow = 
          (hadWeightEta2Narrow[sampling]/hadEnergyNarrow[sampling]) -
          (hadWeightEtaNarrow[sampling]/hadEnergyNarrow[sampling])*
          (hadWeightEtaNarrow[sampling]/hadEnergyNarrow[sampling]);

      clusterWidthNarrow > 0.? clusterWidthNarrow = 
          sqrt(clusterWidthNarrow) : -99.;
      
      hadRadiusNarrow[sampling] = hadRadiusNarrow[sampling] / hadEnergyNarrow[sampling] ;
    } else {
      if(nHadCellsNarrow[sampling] == 0) {
        clusterWidthNarrow         = 0.;
        hadRadiusNarrow[sampling]  = 0. ;
      } else {
        clusterWidthNarrow         = -99.;
        hadRadiusNarrow[sampling]  = -99.;
      }
    }

    if (!m_timersvc.empty()) {m_timer[3]->pause();  m_timer[4]->resume(); }

    //set values
    float hadRad     = -1.0;
    float clusWidth = -1.0;
    switch (m_defaultWidth) {
    case 0:
      hadRad    = hadRadiusNarrow[sampling];
      clusWidth = clusterWidthNarrow;
      break;
    case 1:
      hadRad    = hadRadiusMed[sampling];
      clusWidth = clusterWidthMed;
      break;
    case 2:
      hadRad    = hadRadiusWide[sampling];
      clusWidth = clusterWidthWide;
      break;
    }

    pDetails->setHADRadius      (sampling,hadRad);
    pDetails->setHADenergyWidth (sampling,clusWidth);

    pDetails->setHADenergyNarrow(sampling,hadEnergyNarrow[sampling]);
    pDetails->setHADenergyMedium(sampling,hadEnergyMed[sampling]);
    pDetails->setHADenergyWide  (sampling,hadEnergyWide[sampling]);
    
    if (!m_timersvc.empty()){ m_timer[4]->pause();  m_timer[3]->resume(); }
  }
  
  if (!m_timersvc.empty()){ m_timer[3]->pause();  m_timer[4]->resume();}
  

  //set Raw and Had energy
  rtrigTauCluster.setHADenergy( getHADEnergy(pDetails, m_defaultWidth) );
  rtrigTauCluster.setRawEnergy( rtrigTauCluster.rawEnergy() + rtrigTauCluster.EMenergy() + rtrigTauCluster.HADenergy() );

  if ( msg().level() <= MSG::DEBUG)
      msg()<<MSG::DEBUG 
              << "REGTEST  Record energy RawE" << rtrigTauCluster.rawEnergy() 
              << "Had " << rtrigTauCluster.HADenergy() << endmsg;
  
  if (!m_timersvc.empty()){ m_timer[4]->pause();  m_timer[3]->resume();}
  

  //set Radius
  float caloRad    = caloRadius(pDetails);
  float emRadius3S = emRadiusAllSampl(pDetails, 3);
  float coreFrac   = coreFraction(pDetails);
  float emFrac     = emFraction(&rtrigTauCluster);
  float hadRad     = hadRadius(pDetails);
  
  if (!m_timersvc.empty()){ m_timer[3]->pause();  m_timer[4]->resume();}
  
  rtrigTauCluster.setCaloRadius(caloRad);
  rtrigTauCluster.setEMRadius3S(emRadius3S);
  rtrigTauCluster.setCoreFrac(coreFrac);
  rtrigTauCluster.setEMFrac(emFrac);
  rtrigTauCluster.setHadRadius(hadRad);
  
  if (!m_timersvc.empty()) {
    m_timer[1]->stop(); 
    m_timer[2]->stop(); 
    m_timer[3]->stop();   
    m_timer[4]->stop(); 
    m_timer[0]->stop();      
  }
  
  //  return HLT::OK;
  return StatError;
  
}

double TauAllCaloDRFex::emRadiusAllSampl(const TrigTauClusterDetails* clusterDetails, int maxEmSamp)
{
  double emRadAll = 0.0;
  double totEn    = 0.0;
  
  double emEnergy[4] = {0.0, 0.0, 0.0, 0.0};
  
  for (int iS=0; iS<4 && iS < maxEmSamp; iS++) {
    switch (m_defaultWidth) {
    case 0:
      emEnergy[iS] = clusterDetails->EMenergyNarrow(iS);
      break;
    case 1:
      emEnergy[iS] = clusterDetails->EMenergyMedium(iS);
      break;
    case 2:
      emEnergy[iS] = clusterDetails->EMenergyWide(iS);
      break;
    }
  }

  for(int emSamp = 0; emSamp <= 3 && emSamp < maxEmSamp; emSamp++) {
    if( emEnergy[emSamp] > 0.0 && fabs(clusterDetails->EMRadius(emSamp)) < 90.0) {
      emRadAll += emEnergy[emSamp] * clusterDetails->EMRadius(emSamp);
      totEn    += emEnergy[emSamp];
    }
  }
  
  if (totEn > 0.0) {
    return emRadAll / totEn;
  }
  return -99.0;
}

double TauAllCaloDRFex::caloRadius(const TrigTauClusterDetails* clusterDetails)
{
  double caloRad  = 0.0;
  double totEn    = 0.0;

  double emEnergy[4] = {0.0, 0.0, 0.0, 0.0};
  double hadEnergy[3] = {0.0, 0.0, 0.0};

  for (int iS=0; iS<4; iS++) {
    switch (m_defaultWidth) {
    case 0:
      emEnergy[iS] = clusterDetails->EMenergyNarrow(iS);
      if (iS<3) 
          hadEnergy[iS] = clusterDetails->HADenergyNarrow(iS);
      break;
    case 1:
      emEnergy[iS] = clusterDetails->EMenergyMedium(iS);
      if (iS<3) 
          hadEnergy[iS] = clusterDetails->HADenergyMedium(iS);
      break;
    case 2:
      emEnergy[iS] = clusterDetails->EMenergyWide(iS);
      if (iS<3) 
          hadEnergy[iS] = clusterDetails->HADenergyWide(iS);
      break;
    }
  }
  
  
  for(int emSamp = 0; emSamp <= 3; emSamp++) {
    if( emEnergy[emSamp] > 0.0  &&  fabs(clusterDetails->EMRadius(emSamp)) < 90.0) {
      caloRad += emEnergy[emSamp] * clusterDetails->EMRadius(emSamp);
      totEn   += emEnergy[emSamp];
    }
  }
  for(int hadSamp = 0; hadSamp <= 2; hadSamp++) {
    if( hadEnergy[hadSamp] > 0.0 && fabs(clusterDetails->HADRadius(hadSamp)) < 90.0) {
      caloRad += hadEnergy[hadSamp] * clusterDetails->HADRadius(hadSamp);
      totEn   += hadEnergy[hadSamp];
    }
  }
  if(totEn > 0.0) {
        return caloRad / totEn;
  }
  return -99.0;
}

double TauAllCaloDRFex::hadRadius(const TrigTauClusterDetails* clusterDetails)
{
  double hadRad = 0.0;
  double totEn  = 0.0;
  
  double hadEnergy[3] = {0.0, 0.0, 0.0};
  
  for (int iS=0; iS<3; iS++) {
    switch (m_defaultWidth) {
    case 0:
      hadEnergy[iS] = clusterDetails->HADenergyNarrow(iS);
      break;
    case 1:
      hadEnergy[iS] = clusterDetails->HADenergyMedium(iS);
      break;
    case 2:
      hadEnergy[iS] = clusterDetails->HADenergyWide(iS);
      break;
    }
  }
  
  
  for(int hadSamp = 0; hadSamp <= 2; hadSamp++) {
    if(hadEnergy[hadSamp] > 0.0 && fabs(clusterDetails->HADRadius(hadSamp)) < 90.0) {
      hadRad += hadEnergy[hadSamp] * clusterDetails->HADRadius(hadSamp);
      totEn  += hadEnergy[hadSamp];
    }
  }

  if(totEn > 0.0) {
    return hadRad / totEn;
  }
  return -99.0;
}



double TauAllCaloDRFex::coreFraction(const TrigTauClusterDetails* clusterDetails)
{
    double ERawNarrow = (clusterDetails->EMenergyNarrow(0)
                       + clusterDetails->EMenergyNarrow(1)
                       + clusterDetails->EMenergyNarrow(2)
                       + clusterDetails->EMenergyNarrow(3)
                       + clusterDetails->HADenergyNarrow(0)
                       + clusterDetails->HADenergyNarrow(1)
                       + clusterDetails->HADenergyNarrow(2));
    
    double ERawMedium = (clusterDetails->EMenergyMedium(0)
                     + clusterDetails->EMenergyMedium(1)
                     + clusterDetails->EMenergyMedium(2)
                     + clusterDetails->EMenergyMedium(3)
                     + clusterDetails->HADenergyMedium(0)
                     + clusterDetails->HADenergyMedium(1)
                     + clusterDetails->HADenergyMedium(2));
    
    if(ERawMedium > 0.0) {
      return ERawNarrow / ERawMedium;
    }
    return -99.0;  
}

double TauAllCaloDRFex::emFraction(const TrigTauCluster* ptrigTauCluster)
{
  float totEn = ptrigTauCluster->EMenergy() + ptrigTauCluster->HADenergy();
  if(totEn != 0.0) {
    return ptrigTauCluster->EMenergy() / totEn;
  } 

  return -99.0;
}

double TauAllCaloDRFex::calcEnergyPhi(double energyNegPhi, double energyPosPhi, double EnergyMediumNegPhi, double EnergyMediumPosPhi, double energyNegPhiConv)
{
  double AvgNegPhi = 0.;
  double AvgPosPhi = 0.;
  double energyPhi = 0.;

    if (EnergyMediumNegPhi > 0. ){
      AvgNegPhi = energyNegPhi / EnergyMediumNegPhi;
    } else {
      AvgNegPhi = -999.0;
    }
    
    if (EnergyMediumPosPhi > 0. ){
      AvgPosPhi = energyPosPhi / EnergyMediumPosPhi;
    } else {
      AvgPosPhi = -999.0;
    }
    
    if (AvgPosPhi==-999.0) {
      if (AvgNegPhi != -999.0) {
        energyPhi = AvgNegPhi;
      }
    }
    
    if (AvgNegPhi==-999.0) {
      if (AvgPosPhi != -999.0) {
        energyPhi = AvgPosPhi;
      }
    }
    
    if (AvgNegPhi != -999.0 && AvgPosPhi != -999.0) {
      if ( (AvgNegPhi > (-M_PI/2.0)) && (AvgPosPhi < (M_PI/2.0)) ) {
        energyPhi = (energyNegPhi + energyPosPhi)/ 
	  (EnergyMediumNegPhi + EnergyMediumPosPhi);
      } else {
        if ((AvgNegPhi < (-M_PI/2.0)) && (AvgPosPhi > (M_PI/2.0))) {
          energyPhi = (energyNegPhiConv + energyPosPhi)/
	    (EnergyMediumNegPhi + EnergyMediumPosPhi);
          if (energyPhi > M_PI) {
            energyPhi = energyPhi - 2*M_PI;
          }
        }
      }
    }
  return energyPhi;
}

bool TauAllCaloDRFex::getdR(double compPhi, double compEta, double etaCell, double phiCell, double dRCut, double& dR)
{

  float dEta = fabs(etaCell - compEta);
  if( dEta > dRCut ) return false;
  float dPhi = fabs(phiCell - compPhi);
  if( dPhi > M_PI ) dPhi = 2*M_PI - dPhi;
  if( dPhi > dRCut ) return false;
  dR = std::sqrt(dEta*dEta + dPhi*dPhi);
  if( dR > dRCut )  return false;
  return true;

}

double TauAllCaloDRFex::getEMEnergy(const TrigTauClusterDetails* clusterDetails, int widthChoice)
{
  double EMEnergy;
  switch (widthChoice)
  {
    case 0:
       EMEnergy = (clusterDetails->EMenergyNarrow(0)+
                   clusterDetails->EMenergyNarrow(1)+
                   clusterDetails->EMenergyNarrow(2)+
                   clusterDetails->EMenergyNarrow(3));
       break;
    case 1:
       EMEnergy = (clusterDetails->EMenergyMedium(0)+
                   clusterDetails->EMenergyMedium(1)+
                   clusterDetails->EMenergyMedium(2)+
                   clusterDetails->EMenergyMedium(3));
       break;
    case 2:
       EMEnergy = (clusterDetails->EMenergyWide(0)+
                   clusterDetails->EMenergyWide(1)+
                   clusterDetails->EMenergyWide(2)+
                   clusterDetails->EMenergyWide(3));
       break;       
    default:
       EMEnergy = (clusterDetails->EMenergyNarrow(0)+
                   clusterDetails->EMenergyNarrow(1)+
                   clusterDetails->EMenergyNarrow(2)+
                   clusterDetails->EMenergyNarrow(3));

  }  
  return EMEnergy;
}

double TauAllCaloDRFex::getHADEnergy(const TrigTauClusterDetails* clusterDetails, int widthChoice)
{
  double HADEnergy;
  switch (widthChoice)
  {
    case 0:
       HADEnergy = (clusterDetails->HADenergyNarrow(0)+
                    clusterDetails->HADenergyNarrow(1)+
                    clusterDetails->HADenergyNarrow(2));
       break;
    case 1:
       HADEnergy = (clusterDetails->HADenergyMedium(0)+
                    clusterDetails->HADenergyMedium(1)+
                    clusterDetails->HADenergyMedium(2));
       break;
    case 2:
       HADEnergy = (clusterDetails->HADenergyWide(0)+
                    clusterDetails->HADenergyWide(1)+
                    clusterDetails->HADenergyWide(2));
       break;       
    default:
       HADEnergy = (clusterDetails->HADenergyNarrow(0)+
                    clusterDetails->HADenergyNarrow(1)+
                    clusterDetails->HADenergyNarrow(2));

  }  
  
  return HADEnergy;
}
