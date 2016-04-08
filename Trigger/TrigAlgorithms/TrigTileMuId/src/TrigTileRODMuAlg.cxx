/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TrigTileRODMuAlg.cxx
//  Author   : Aranzazu Ruiz Martinez
//  Created  : December 2006
//
//  DESCRIPTION:
//     LVL2 trigger algorithm to extract the low pT muons tagged at the TileCal ROD DSP level.
//     The algorithm is processed taking as input the energy previously reconstructed at the 
//     DSP with Optimal Filtering. Only possible to use with events which contain the frag 
//     types 0x10, 0x11, 0x12 or 0x13 (inside TileCal ROD fragments).
//
//  HISTORY:
//
//  BUGS:
//    
//    
//*****************************************************************************

#include <vector>
#include <math.h>

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "RegionSelector/IRegSelSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "CaloIdentifier/TileID.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigTileMuId/TrigTileRODMuAlg.h"
//#include "TH1.h"

class ISvcLocator;



TrigTileRODMuAlg::TrigTileRODMuAlg(const std::string& name,ISvcLocator* pSvcLocator)
  :HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty("TileMuTagsOutputName", m_tileTagContainer);
  declareProperty("UseRS", m_commissioning=false);
  declareProperty("ROBIDLIST", m_ROBList);
  declareProperty("EtaMin", m_etaMin = -1.609430);
  declareProperty("EtaMax", m_etaMax =  1.609430);
  declareProperty("PhiMin", m_phiMin =  0.0);
  declareProperty("PhiMax", m_phiMax =  6.283185);
  declareProperty("LooseSelection",m_LooseSelection=true);

  declareProperty("ReadRoIsFromL1", m_ReadRoIsFromL1=false);	

  // declare monitoring histograms
  declareMonitoredVariable("NTileRODMu", m_allNMuon);
  declareMonitoredStdContainer("EtaTileRODMu", m_allEta);
  declareMonitoredStdContainer("PhiTileRODMu", m_allPhi);
  declareMonitoredStdContainer("ETileRODMu", m_allEne);
  declareMonitoredStdContainer("EtaTileRODMuLoose", m_allEta_LS);
  declareMonitoredStdContainer("PhiTileRODMuLoose", m_allPhi_LS);
  declareMonitoredStdContainer("ETileRODMuLoose", m_allEne_LS);

  // --> For matching study with RoIs and TileMu
  declareMonitoredVariable("NROBFrags", m_NROBFrags);
  declareMonitoredVariable("EffEvtswRoI", m_EffEvtswRoI); 
  //declareMonitoredVariable("EffEvtswRoI_LimitedEta",m_EffEvtswRoI_LimitedEta);
  declareMonitoredStdContainer("EffEvtswRoI_LimitedEta",m_EffEvtswRoI_LimitedEta); 
  declareMonitoredStdContainer("EtaTileMu_wRoI", m_EtaTileMu_wRoI);
  declareMonitoredStdContainer("EtaRoI_wTileMu", m_EtaRoI_wTileMu);
  declareMonitoredStdContainer("PhiTileMu_wRoI", m_PhiTileMu_wRoI); 
  declareMonitoredStdContainer("PhiRoI_wTileMu", m_PhiRoI_wTileMu);  
  declareMonitoredStdContainer("EtaRoI_NoTileMu", m_EtaRoI_NoTileMu);
  declareMonitoredStdContainer("PhiRoI_NoTileMu", m_PhiRoI_NoTileMu);
 
}



TrigTileRODMuAlg::~TrigTileRODMuAlg()
{
}



HLT::ErrorCode TrigTileRODMuAlg::hltInitialize()
{
  m_timerTotal = addTimer("Total");
  m_timerAccess = addTimer("Access");
  m_timerAddRoi = addTimer("AddRoi");

  // retrieve TileID helper and TileInfo from det store
  StatusCode sc = detStore()->retrieve(m_tileID);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve TileID helper from DetectorStore");
    return HLT::BAD_JOB_SETUP;
  }

  // get RegionSelector 
  if (service("RegSelSvc", m_pRegionSelector).isFailure()) {
    ATH_MSG_FATAL("Could not find RegionSelector service");
    return HLT::BAD_JOB_SETUP;
  }

  // get ROBDataProvider
  if(service("ROBDataProviderSvc",m_robDataProvider).isFailure()){
    ATH_MSG_ERROR("Can't get ROBDataProviderSvc");
    return HLT::BAD_JOB_SETUP;
  } 

  if(toolSvc()->retrieveTool("TileROD_Decoder",m_tiledecoder).isFailure()){
    ATH_MSG_FATAL("Could not find TileRodDecoder");
    return HLT::BAD_JOB_SETUP;
  }

  ATH_MSG_DEBUG("TrigTileRODMuAlg initialization completed");

  // TileL2Container
  m_container = new TileL2Container();
  m_container->reserve(256);
  for(int ros=1;ros<5;++ros) {
    for(int drawer=0;drawer<64;++drawer) {
      int collId = ros<<8 | drawer;
      TileL2 *l2 = new TileL2(collId);
      m_container->push_back(l2);
    }
  }

  m_vrobid.reserve(64);
  m_vid.reserve(64);
  m_vrob.reserve(64);

  return HLT::OK;
}



HLT::ErrorCode TrigTileRODMuAlg::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputVector, unsigned int type_out)
{
  // total execution time
  if ( timerSvc() ) m_timerTotal->start();

  // time of data access
  if ( timerSvc() ) m_timerAccess->start();

  ATH_MSG_DEBUG("exec HLTAlgorithm started");

  if (!m_robDataProvider) {
    ATH_MSG_ERROR(" ROBDataProviderSvc not loaded. Can't read ByteStream.");
    return HLT::BAD_JOB_SETUP;
  }

  const double User_Pi = 3.141592654;
  const double DelEta_RS = 0.1;
  const double DelPhi_RS = 0.1;
 
  const int NMax_Muons = 30;
  int NRoIs = 0;			// RoIs from |eta| < 1.7
  int NRoIs_LimitedEta = 0;		// RoIs from |eta| < 1.4		
  float Eta_RoIs[NMax_Muons], Phi_RoIs[NMax_Muons];
  float Eta_Mus[NMax_Muons],  Phi_Mus[NMax_Muons];

  // monitored variables
  beforeExecMonitors().ignore();

  m_allNMuon = 0;
  m_allPhi.clear();
  m_allEta.clear();
  m_allEne.clear();
  m_allEta_LS.clear();
  m_allPhi_LS.clear();
  m_allEne_LS.clear();

  m_NROBFrags = 0;
  m_EffEvtswRoI = 0.0;
  //m_EffEvtswRoI_LimitedEta = 0.0;
  m_EffEvtswRoI_LimitedEta.clear(); 
  m_EtaTileMu_wRoI.clear();
  m_EtaRoI_wTileMu.clear();
  m_PhiTileMu_wRoI.clear();
  m_PhiRoI_wTileMu.clear();
  m_EtaRoI_NoTileMu.clear(); 
  m_PhiRoI_NoTileMu.clear();

  m_vrobid.clear();
  m_vid.clear();
  m_vrob.clear();

  TileL2Container::iterator collItr=m_container->begin();
  TileL2Container::iterator lastColl=m_container->end();
  for(; collItr!=lastColl; ++collItr) (*collItr)->clear();
  // ----------- Try to read RoIs from L1_RPC (by HL, 2008-12-12) -----------//
  if (m_ReadRoIsFromL1) {  

    ATH_MSG_DEBUG("Found " << inputVector.size()
		           << " TE vectors");
    if ( inputVector.size() != 1 ) {
      ATH_MSG_WARNING("inputVector.size()(=" << inputVector.size()
               << ") != 1, Please check TE from LvL1!!");
    }

    int ii=0;
    for (std::vector<HLT::TEVec>::const_iterator it = inputVector.begin();it != inputVector.end(); ++it) {

      ATH_MSG_DEBUG("TE vector " << ii++ << "contains "
               << (*it).size() << " TEs");

      for (HLT::TEVec::const_iterator inner_it = (*it).begin();inner_it != (*it).end(); ++inner_it) {
        const TrigRoiDescriptor* roiDescriptor = 0;
        HLT::ErrorCode sc = getFeature((*inner_it),roiDescriptor, "");


        if (sc != HLT::OK) {
          ATH_MSG_ERROR(" Failed to find RoiDescriptor ");

          return HLT::BAD_JOB_SETUP;            //
        }

	ATH_MSG_DEBUG(" REGTEST: RoI id "
                 << roiDescriptor->roiId()
                 << " located at   phi = " <<  roiDescriptor->phi()
                 << ", eta = " << roiDescriptor->eta());

        //if ( fabs(roiDescriptor->eta()) < 1.7 ) {
          Eta_RoIs[NRoIs] = roiDescriptor->eta();
          Phi_RoIs[NRoIs] = roiDescriptor->phi();
          NRoIs++;
         
          //if ( fabs(roiDescriptor->eta()) < 1.4 ) NRoIs_LimitedEta++;  
          if ( fabs(roiDescriptor->eta()) < 0.7 ) NRoIs_LimitedEta++; 

          double now_eta = roiDescriptor->eta();
          double now_phi = roiDescriptor->phi();
          if (now_phi < 0) now_phi = now_phi + 2*User_Pi; // phi --> 0 ~ 2pi

          double now_etamin = now_eta - DelEta_RS;
          double now_etamax = now_eta + DelEta_RS;
          double now_phimin = now_phi - DelPhi_RS;
          double now_phimax = now_phi + DelPhi_RS;
          if (now_phimin < 0.)        { now_phimin = now_phimin + 2*User_Pi; }
          if (now_phimax > 2*User_Pi) { now_phimax = now_phimax - 2*User_Pi; }

          //if (now_phimin > now_phimax) {
          //  double temp = now_phimin;
          //  now_phimin = now_phimax;
          //  now_phimax = temp;
          //}

          if ( fabs(now_eta) > 0.6 && fabs(now_eta) < 1.2 ) {
            if (now_eta<0) { now_etamin = m_etaMin; now_etamax = -0.2; }
            if (now_eta>0) { now_etamin = 0.2;     now_etamax = m_etaMax; }
          }
 
          //m_pRegionSelector->DetHashIDList(TILE,
          //      now_etamin,now_etamax,now_phimin,now_phimax,&m_listID);

	  ATH_MSG_DEBUG("For DetROBIDListUint, EtaMin="
                   << now_etamin << " EtaMax=" << now_etamax
                   << " PhiMin=" << now_phimin
                   << " PhiMax=" << now_phimax);

	  TrigRoiDescriptor _roi( 0.5*(now_etamin+now_etamax),          now_etamin, now_etamax, 
				  HLT::phiMean(now_phimin, now_phimax), now_phimin, now_phimax );

          m_pRegionSelector->DetROBIDListUint(TILE, _roi, m_vrobid);
        //}
      }
    }

  } else { 
    if(m_commissioning) {
      for (std::vector<int>::const_iterator iter=m_ROBList.begin(); iter!=m_ROBList.end(); ++iter){
        m_vrobid.push_back(*iter);
      }
    }else{
      TrigRoiDescriptor _roi( 0.5*(m_etaMin+m_etaMax),          m_etaMin, m_etaMax, 
			      HLT::phiMean(m_phiMin, m_phiMax), m_phiMin, m_phiMax );
     
      m_pRegionSelector->DetROBIDListUint(TILE, _roi, m_vrobid);
    }
  }

  ATH_MSG_VERBOSE("ROBIDLIST size= " << m_vrobid.size());

  m_NROBFrags = m_vrobid.size();
  if ( m_vrobid.size() > 60 ) {

    ATH_MSG_DEBUG("Too many access for TileROBFragments!! ROBFrags="
             << m_vrobid.size() << " NRoIs=" << NRoIs);

    for (unsigned int it = 0; it < m_vrobid.size(); it++) {

      ATH_MSG_DEBUG("ROBID nr= " << it << " equal to 0x" << MSG::hex
               << m_vrobid[it] << MSG::dec);
    }
  }


  /*for (unsigned int it = 0; it < m_vrobid.size(); it++) {
    ATH_MSG::VERBOSE("ROBID nr= " << it << " equal to 0x" << MSG::hex 
	     << m_vrobid[it] << MSG::dec);
  }*/

  m_robDataProvider->addROBData(m_vrobid);

  for (unsigned int i = 0; i < m_vrobid.size(); ++i) {

    bool OK_Next = true;
    if ( i > 0 ) {
      for (unsigned int j=0; j<i; ++j) {
        if (m_vrobid[i] == m_vrobid[j]) {
          OK_Next = false;
	  break;
        }
      }
    }   

    if (OK_Next) {
      m_vid.push_back(m_vrobid[i]);

      // Retrieve the service
      m_robDataProvider->getROBData(m_vid, m_vrob);

      if (m_vrob.size() == 0) {

	ATH_MSG_DEBUG("For ROBID 0x" << MSG::hex << m_vrobid[i] << MSG::dec << " size = 0");

      }else{

	ATH_MSG_VERBOSE(" m_vrob.size()=" << m_vrob.size() 
	  	 << ", So go to fillCollectionL2"); 
        m_tiledecoder->fillCollectionL2(m_vrob[0],*m_container);
      }
    }

    m_vrob.clear();
    m_vid.clear();

  }

  ATH_MSG_DEBUG("m_container size= " << m_container->size());

  
  // stop of time for data access
  if ( timerSvc() ) m_timerAccess->stop();

  // start of iterator
  if ( timerSvc() ) m_timerAddRoi->start();

  int ntag=0;

  if ( m_vrobid.size() > 0 ) {  
    // create one output TE for each particle found during the scan:
    TileL2Container::const_iterator it= m_container->begin(); 
    TileL2Container::const_iterator end=m_container->end();

    // Apr. 22. 2008 HL (double -> float)
    float eta, phi, qf;
    std::vector<float> ene(4);

    for(; it != end; ++it) {

      for(unsigned int a=0; a<((*it)->NMuons()); ++a) {

        // roiDescriptor position
        eta = ( (*it)->eta(a) );
        phi = ( (*it)->phi(a) );

        // Energy deposited in TileCal by the muon (MeV)
        ene.at(0) = ( (*it)->enemu0(a) );
        ene.at(1) = ( (*it)->enemu1(a) );
        ene.at(2) = ( (*it)->enemu2(a) );
        ene.at(3) = ( (*it)->enemu0(a)+(*it)->enemu1(a)+(*it)->enemu2(a) );

        // Muon quality factor
        qf = (*it)->qual(a);

        if (qf > 0.) {
	  m_allEta_LS.push_back(eta);
	  m_allPhi_LS.push_back(phi);
	  m_allEne_LS.push_back(ene.at(3)/1000.);
        }

        bool OK_Take = true;

        if (!m_LooseSelection && qf != 0) OK_Take = false;

        if (OK_Take) {

	  float roiwidth=0.1;
	  TrigRoiDescriptor *tmproi = new
	    TrigRoiDescriptor(eta, eta-roiwidth,eta+roiwidth,
			      phi, phi-roiwidth,phi+roiwidth);

	  HLT::TriggerElement* te = addRoI(type_out, tmproi); // note: addRoI is defined in AllTEAlgo base class

	  te->setActiveState(true); // set this trigger element to be active

	  // Prepare Feature
	  TileMuFeature* muon = new TileMuFeature(eta, phi, ene, qf);

	  //HLT::ErrorCode HLTStatus = attachFeature(te, muon, name());
	  HLT::ErrorCode HLTStatus = attachFeature(te, muon);

	  if ( HLTStatus != HLT::OK ) {
	    ATH_MSG_ERROR("Write of TrigTileRODMu feature into outputTE failed");
	    return HLTStatus;
	  } else {
	    ATH_MSG_DEBUG("Write of TrigTileRODMu feature into outputTE OK!");
	  }

	  ATH_MSG_DEBUG("Muon tagged:  Ntag = " << ntag
                   << "  Eta = " << eta
                   << "  Phi = " << phi
                   << "  Energy = " << ene.at(3) << " MeV"
                   << "  Quality factor = " << qf);

	  // Fill histograms
	  m_allEta.push_back(eta);
	  m_allPhi.push_back(phi);
	  m_allEne.push_back(ene.at(3)/1000.);

          Eta_Mus[ntag]	= eta;
          Phi_Mus[ntag]	= phi; 

	  ntag++;

        }
      }

      if (ntag >= NMax_Muons) break;
    }
  }

  ATH_MSG_DEBUG("NMuons = " << ntag);

  // Fill histogram
  m_allNMuon = ntag;

  //if ( m_ReadRoIsFromL1 && ntag > 0 ) { 
  if ( m_ReadRoIsFromL1 ) {
    if ( NRoIs_LimitedEta > 0) {
      if ( ntag > 0 ) { m_EffEvtswRoI_LimitedEta.push_back( 1.0 );
      } else          { m_EffEvtswRoI_LimitedEta.push_back( 0.0 ); 
      } 
    }
 
    if ( ntag > 0 ) { 
      m_EffEvtswRoI = 1.0;

      //if ( NRoIs_LimitedEta > 0) m_EffEvtswRoI_LimitedEta = 1.0;

      float DelRMin = 9999.9;
      int Id_RoI = -1, Id_Mu= -1;
      for(int iMu=0; iMu<ntag; iMu++) {
        for (int iRoI=0; iRoI<NRoIs; iRoI++) { 
          float DelPhi = Phi_Mus[iMu] - Phi_RoIs[iRoI];
          if (DelPhi >      User_Pi) DelPhi -= 2*User_Pi;
          if (DelPhi < (-1)*User_Pi) DelPhi += 2*User_Pi;

          float DelEta = Eta_Mus[iMu] - Eta_RoIs[iRoI];
          float DelR = sqrt( pow(DelEta,2) + pow(DelPhi,2) );
          if (DelR < DelRMin) {
	      Id_Mu = iMu;
              Id_RoI = iRoI;
              DelRMin = DelR;
          }
        }
      }

      if ( Id_RoI > -1 && Id_Mu > -1 ) {
        m_EtaTileMu_wRoI.push_back( Eta_Mus[Id_Mu] );
        m_EtaRoI_wTileMu.push_back( Eta_RoIs[Id_RoI] );
        m_PhiTileMu_wRoI.push_back( Phi_Mus[Id_Mu]);
        m_PhiRoI_wTileMu.push_back( Phi_RoIs[Id_RoI] );
            
	ATH_MSG_DEBUG("Matched Eta(TileMu)=" << Eta_Mus[Id_Mu]
                     << " Eta(RoI)=" << Eta_RoIs[Id_RoI]
                     << " Phi(TileMu)=" << Phi_Mus[Id_Mu]
                     << " Phi(RoI)=" << Phi_RoIs[Id_RoI]
                     << " DelR=" << DelRMin);
       
      } else {
	ATH_MSG_DEBUG("No Matching between TileMus and RoIs!"
               << " Something Wrong?");

      } 
    } else {
      for(int iRoI=0; iRoI<NRoIs; iRoI++) {
        m_EtaRoI_NoTileMu.push_back( Eta_RoIs[iRoI] );
        m_PhiRoI_NoTileMu.push_back( Phi_RoIs[iRoI] );
      }
    }
  }

  // store the muon tags container on the TDS
  /*if ( evtStore()->record(m_container, m_tileTagContainer, false).isFailure()) {
    ATH_MSG_ERROR("Unable to record the muon tag Container on the TDS");
    return HLT::BAD_JOB_SETUP;
  }*/

  // stop of iterator
  if ( timerSvc() ) m_timerAddRoi->stop();

  // stop of total execution time
  if ( timerSvc() ) m_timerTotal->stop();

  // stop monitoring
  afterExecMonitors().ignore();

  return HLT::OK;
}   



HLT::ErrorCode TrigTileRODMuAlg::hltFinalize()
{
  ATH_MSG_DEBUG("TrigTileRODMuAlg finalization completed");

  // clean up memory used
  m_container->clear();

  delete m_container;

  return HLT::OK;
}
