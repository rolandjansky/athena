/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArAffectedRegions.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Jessica Leveque
//
// Class for monitoring : Map of Affected Regions
//                        built from "Affected Region" metadata, stored in ESDs.
// ********************************************************************

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <functional>
#include <map>
#include <utility>

#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "TMath.h"
#include "TString.h"

#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "LArMonTools/LArAffectedRegions.h"

using namespace std;

/*---------------------------------------------------------*/
LArAffectedRegions::LArAffectedRegions(const std::string& type, 
			 const std::string& name,
			 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_rootStore(0)
 {
  declareProperty("IsOnline",m_isonline=false);
  declareProperty("EtaGranularity",m_delta_eta=0.01);
  declareProperty("PhiGranularity",m_delta_phi=0.01);

  m_eventsCounter = 0;

  m_hLArAffectedRegionsEMBPS[0]		= NULL;
  m_hLArAffectedRegionsEMBPS[1]		= NULL;

  m_hLArAffectedRegionsEMB[0]		= NULL;
  m_hLArAffectedRegionsEMB[1]		= NULL;

  m_hLArAffectedRegionsEMECPS[0]	= NULL;
  m_hLArAffectedRegionsEMECPS[1]	= NULL;

  m_hLArAffectedRegionsEMEC[0]		= NULL;
  m_hLArAffectedRegionsEMEC[1]		= NULL;

  m_hLArAffectedRegionsHECA[0]		= NULL;
  m_hLArAffectedRegionsHECA[1]		= NULL;
  m_hLArAffectedRegionsHECA[2]		= NULL;
  m_hLArAffectedRegionsHECA[3]		= NULL;

  m_hLArAffectedRegionsHECC[0]		= NULL;
  m_hLArAffectedRegionsHECC[1]		= NULL;
  m_hLArAffectedRegionsHECC[2]		= NULL;
  m_hLArAffectedRegionsHECC[3]		= NULL;

  m_hLArAffectedRegionsFCALA[0]		= NULL;
  m_hLArAffectedRegionsFCALA[1]		= NULL;
  m_hLArAffectedRegionsFCALA[2]		= NULL;

  m_hLArAffectedRegionsFCALC[0]		= NULL;
  m_hLArAffectedRegionsFCALC[1]		= NULL;
  m_hLArAffectedRegionsFCALC[2]		= NULL;

 }

/*---------------------------------------------------------*/
LArAffectedRegions::~LArAffectedRegions()
{
}

/*---------------------------------------------------------*/
StatusCode 
LArAffectedRegions::initialize()
{
  ATH_MSG_INFO( "Initialize LArAffectedRegions"  );

  // End Initialize
  ManagedMonitorToolBase::initialize().ignore();
  ATH_MSG_DEBUG( "Successful Initialize LArAffectedRegions "  );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode 
LArAffectedRegions::bookHistograms()
{
  ATH_MSG_DEBUG( "in bookHists()"  );

  //  if(isNewRun ){ // Commented by B.Trocme to comply with new ManagedMonitorToolBase
  
    // Create top folder for histos
  MonGroup generalGroup( this, "/LAr/AffectedRegions", run, ATTRIB_MANAGED,"", "weightedAverage");
    
    // EM Calorimeter - Barrel - PS 
    m_hLArAffectedRegionsEMBPS[0] = TH2I_LW::create("LArAffectedRegionsEMBAPS",
						    "HV Affected Regions - EMBA - Presampler", 
						    8, 0.,1.6,32,-3.15, 3.15);
    SetHistoStyle(m_hLArAffectedRegionsEMBPS[0]);
    generalGroup.regHist(m_hLArAffectedRegionsEMBPS[0]).ignore();
    
    m_hLArAffectedRegionsEMBPS[1] = TH2I_LW::create("LArAffectedRegionsEMBCPS",
						    "HV Affected Regions - EMBC - Presampler", 
						    8,-1.61, -0.01,32,-3.15, 3.15);
    SetHistoStyle(m_hLArAffectedRegionsEMBPS[1]);
    generalGroup.regHist(m_hLArAffectedRegionsEMBPS[1]).ignore();
    
    // EM Calorimeter - Barrel - Layers 1-3
    m_hLArAffectedRegionsEMB[0] = TH2I_LW::create("LArAffectedRegionsEMBA",
						  "HV Affected Regions - EMBA - Samplings 1-3", 
						  7,0.,1.4,128,-3.15, 3.15);
    SetHistoStyle(m_hLArAffectedRegionsEMB[0]);
    generalGroup.regHist(m_hLArAffectedRegionsEMB[0]).ignore();
    
    m_hLArAffectedRegionsEMB[1] = TH2I_LW::create("LArAffectedRegionsEMBC",
						  "HV Affected Regions - EMBC - Samplings 1-3", 
						  7,-1.41, -0.01,128,-3.15, 3.15);
    SetHistoStyle(m_hLArAffectedRegionsEMB[1]);
    generalGroup.regHist(m_hLArAffectedRegionsEMB[1]).ignore();

    // EM Calorimeter - Endcap - PS

    float etabin_EMECP[] = {1.375,1.50,1.6,1.8,2.0,2.1,2.3,2.5,2.8,3.2};
    float etabin_EMECM[] = {-3.21,-2.81,-2.51,-2.31,-2.11,-2.01,-1.81,-1.61,-1.51,-1.385};
    float phibin_EMECPS[65];float phibin_EMEC[257];

    for(int ibin=0;ibin<=64;ibin++) phibin_EMECPS[ibin] = -TMath::Pi()+ibin*2*TMath::Pi()/64;
    for(int ibin=0;ibin<=256;ibin++) phibin_EMEC[ibin] = -TMath::Pi()+ibin*2*TMath::Pi()/256;

    m_hLArAffectedRegionsEMECPS[0] = TH2I_LW::create("LArAffectedRegionsEMECAPS",
						  "HV Affected Regions - EMECA - Presampler", 
						  9,etabin_EMECP,64,phibin_EMECPS);
    SetHistoStyle(m_hLArAffectedRegionsEMECPS[0]);
    generalGroup.regHist(m_hLArAffectedRegionsEMECPS[0]).ignore();

    m_hLArAffectedRegionsEMECPS[1] = TH2I_LW::create("LArAffectedRegionsEMECCPS",
						  "HV Affected Regions - EMECC - Presampler", 
						  9,etabin_EMECM,64,phibin_EMECPS);
    SetHistoStyle(m_hLArAffectedRegionsEMECPS[1]);
    generalGroup.regHist(m_hLArAffectedRegionsEMECPS[1]).ignore();

    // EM Calorimeter - Endcap - Layers 1-3
    m_hLArAffectedRegionsEMEC[0] = TH2I_LW::create("LArAffectedRegionsEMECA",
						  "HV Affected Regions - EMECA - Samplings 1-3", 
						  9,etabin_EMECP,256,phibin_EMEC);
    SetHistoStyle(m_hLArAffectedRegionsEMEC[0]);
    generalGroup.regHist(m_hLArAffectedRegionsEMEC[0]).ignore();
    
    m_hLArAffectedRegionsEMEC[1] = TH2I_LW::create("LArAffectedRegionsEMECC",
						  "HV Affected Regions - EMECC - Samplings 1-3", 
						  9,etabin_EMECM,256,phibin_EMEC);
    SetHistoStyle(m_hLArAffectedRegionsEMEC[1]);
    generalGroup.regHist(m_hLArAffectedRegionsEMEC[1]).ignore();


    // HEC
    for(int iS=0;iS<4;iS++){
      m_hLArAffectedRegionsHECA[iS] = TH2I_LW::create(Form("LArAffectedRegionsHECA%i",iS),
						     Form("HV Affected Regions - HECA - Layer %i",iS+1), 
						     1,1.5, 3.2, 32, -3.15, 3.15);
      SetHistoStyle(m_hLArAffectedRegionsHECA[iS]);
      generalGroup.regHist(m_hLArAffectedRegionsHECA[iS]).ignore();

      m_hLArAffectedRegionsHECC[iS] = TH2I_LW::create(Form("LArAffectedRegionsHECC%i",iS),
						     Form("HV Affected Regions - HECC - Layer %i",iS+1), 
						     1,-3.21, -1.51, 32, -3.15, 3.15);
      SetHistoStyle(m_hLArAffectedRegionsHECC[iS]);
      generalGroup.regHist(m_hLArAffectedRegionsHECC[iS]).ignore();
    }
    
    // FCAL
    for(int iS=0;iS<3;iS++){
      m_hLArAffectedRegionsFCALA[iS] = TH2I_LW::create(Form("LArAffectedRegionsFCALA%i",iS),
						 Form("LAr Affected Regions - FCALA - Layer %i",iS+1), 
						 1, 2.9, 4.9, 16, -3.2, 3.2);
      SetHistoStyle(m_hLArAffectedRegionsFCALA[iS]);
      generalGroup.regHist(m_hLArAffectedRegionsFCALA[iS]).ignore();

      m_hLArAffectedRegionsFCALC[iS] = TH2I_LW::create(Form("LArAffectedRegionsFCALC%i",iS),
						 Form("LAr Affected Regions - FCALC - Layer %i",iS+1), 
						 1, -4.91, -3.11,  16, -3.2, 3.2);
      SetHistoStyle(m_hLArAffectedRegionsFCALC[iS]);
      generalGroup.regHist(m_hLArAffectedRegionsFCALC[iS]).ignore();
    }
 
    return StatusCode::SUCCESS;
    
    //}// end isNewRun// Commented by B.Trocme to comply with new ManagedMonitorToolBase
    
    //    return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode 
LArAffectedRegions::fillHistograms()
{
  ATH_MSG_DEBUG( "in fillHists()"  );

  m_eventsCounter++;

  if(m_eventsCounter > 1 ) return StatusCode::SUCCESS;
  
  // Retrieve CaloAffected Regions
  if(!m_isonline){
    const CaloAffectedRegionInfoVec* affRegVec=0;
    StatusCode sc = detStore()->retrieve(affRegVec, "LArAffectedRegion");
    if (sc.isFailure() || !affRegVec) {
      ATH_MSG_WARNING( " Cannot find LArAffectedRegion in DetectorStore "  );
      return StatusCode::SUCCESS;
    }

    ///////////////////////
    //   Problem Types   //
    ///////////////////////
    // 0: Missing FEB    //
    // 1: Non nominal HV //
    // 2: Dead HV        //
    ///////////////////////

    std::vector<CaloAffectedRegionInfo>::const_iterator reg1 = affRegVec->begin();
    std::vector<CaloAffectedRegionInfo>::const_iterator reg2 = affRegVec->end();
    for (;reg1 != reg2; ++reg1) {
      const CaloAffectedRegionInfo* region = &(*reg1);
      int problem=region->get_problem();
      if (problem == 0) continue;
      int layermin=region->get_layer_min();
      int layermax=region->get_layer_max();
      float phimin=region->get_phi_min();
      float phimax=region->get_phi_max();
      float etamin=region->get_eta_min();
      float etamax=region->get_eta_max();
     
      ATH_MSG_DEBUG( " Print list of affected regions:"  );
      ATH_MSG_DEBUG( region << " " 
		      << region->get_eta_min() << " " << region->get_eta_max() << " " 
		      << region->get_phi_min() << " " << region->get_phi_max() << " "
		      << region->get_layer_min() << " " << region->get_layer_max() << " " << region->get_problem()  );
      /////////////////////////////////
      // Build Affected Regions maps //
      /////////////////////////////////
      //          CaloSample 
      /////////////////////////////////
      //  PreSamplerB=0, EMB1=1, EMB2=2, EMB3=3   // LAr barrel
      //  PreSamplerE=4, EME1=5, EME2=6, EME3=7   // LAr EM endcap 
      //  HEC0=8, HEC1=9, HEC2=10, HEC3=11        // Hadronic end cap cal.
      //  TileBar0=12, TileBar1=13, TileBar2=14   // Tile barrel
      //  TileGap1=15, TileGap2=16, TileGap3=17   // Tile gap (ITC & scint)
      //  TileExt0=18, TileExt1=19, TileExt2=20   // Tile extended barrel
      //  FCAL0=21, FCAL1=22, FCAL2=23            // Forward EM endcap
      /////////////////////////////////

      // EM Barrel - PS
      if(layermin==0){
	float eta = etamin+0.01; 
	while(eta<etamax){
	  // phimin < phimax
	  if(phimin<=phimax){
	    float phi = phimin+0.01;
	    while(phi<phimax){
	      if(eta>=0){
		m_hLArAffectedRegionsEMBPS[0]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMBAPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      if(eta<0){
		m_hLArAffectedRegionsEMBPS[1]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMBCPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/32);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	   // phimin > phimax
	  }else{
	    float phi = phimin+0.01;
	    while(phi<TMath::Pi()){
	      if(eta>=0){
		m_hLArAffectedRegionsEMBPS[0]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMBAPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      if(eta<0){
		m_hLArAffectedRegionsEMBPS[1]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMBCPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/32);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	    phi = -TMath::Pi();
	    while(phi<phimax){
	      if(eta>=0){
		m_hLArAffectedRegionsEMBPS[0]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMBAPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      if(eta<0){
		m_hLArAffectedRegionsEMBPS[1]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMBCPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/32);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	  }
	  eta+=0.2;
	}
      }

      // EM Barrel - Other Layers
      if(layermin==1){
	float eta = etamin; 
	while(eta < etamax){
	  // phimin < phimax
	  if(phimin<=phimax){
	    float phi = phimin+0.01;
	    while(phi<phimax){
	      if(eta >=0){
		m_hLArAffectedRegionsEMB[0]->Fill(eta,phi,problem);
		ATH_MSG_DEBUG(  "EMBA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      if(eta <0){
		m_hLArAffectedRegionsEMB[1]->Fill(eta,phi,problem);
		ATH_MSG_DEBUG(  "EMBC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/128);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	  // phimin > phimax: go over Pi
	  }else{
	    float phi = phimin+0.01;
	    while(phi<TMath::Pi()){
	      if(eta >=0){
		m_hLArAffectedRegionsEMB[0]->Fill(eta,phi,problem);
		ATH_MSG_DEBUG(  "EMBA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      if(eta <0){
		m_hLArAffectedRegionsEMB[1]->Fill(eta,phi,problem);
		ATH_MSG_DEBUG(  "EMBC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/128);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	    phi = -TMath::Pi();
	    while(phi<phimax){
	      if(eta >=0){
		m_hLArAffectedRegionsEMB[0]->Fill(eta,phi,problem);
		ATH_MSG_DEBUG(  "EMBA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      if(eta <0){
		m_hLArAffectedRegionsEMB[1]->Fill(eta,phi,problem);
		ATH_MSG_DEBUG(  "EMBC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/128);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	  }
	  eta+=0.2;
	}
      }
      
      // EM endcap - PS
      if(layermin==4){
	float eta = etamin; 
	while(eta<etamax){
	  // phimin < phimax
	  if(phimin<=phimax){
	    float phi = phimin;
	    while(phi<phimax){
	      if(eta>=0){
		m_hLArAffectedRegionsEMECPS[0]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMECAPS " << eta << " " << phi << " " << problem << "  " << layermin << " " << layermax << " " << region  );
	      }
	      if(eta<0){
		m_hLArAffectedRegionsEMECPS[1]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMECCPS " << eta << " " << phi << " " << problem << "  " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/64);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	  // phimin > phimax
	  }else{
	    float phi = phimin+0.01;
	    while(phi<TMath::Pi()){
	      if(eta>=0){
		m_hLArAffectedRegionsEMECPS[0]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMECAPS " << eta << " " << phi << " " << problem << "  " << layermin << " " << layermax << " " << region  );
	      }
	      if(eta<0){
		m_hLArAffectedRegionsEMECPS[1]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMECCPS " << eta << " " << phi << " " << problem << "  " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/64);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	    phi = -TMath::Pi();
	    while(phi<phimax){
	      if(eta>=0){
		m_hLArAffectedRegionsEMECPS[0]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMECAPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      if(eta<0){
		m_hLArAffectedRegionsEMECPS[1]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMECCPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/64);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	  }
	  if(fabs(eta)<2.5)eta+=0.2;
	  if(fabs(eta)>=2.5)eta+=0.3;
	}
      }

      // EM Endcap - Other Layers
      if(layermin==5){
	float eta = etamin+0.01; 
	while(eta<etamax){
	  // phimin < phimax
	  if(phimin<=phimax){
	    float phi = phimin+0.01;
	    while(phi<phimax){
	      if(eta>=0){
		m_hLArAffectedRegionsEMEC[0]->Fill(eta,phi,problem);
		ATH_MSG_DEBUG(  "EMECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      } 
	      if(eta<0){
		m_hLArAffectedRegionsEMEC[1]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/256);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	  // phimin > phimax
	  }else{
	    float phi = phimin+0.01;
	    while(phi<TMath::Pi()){
	      if(eta>=0){
		m_hLArAffectedRegionsEMEC[0]->Fill(eta,phi,problem);
		ATH_MSG_DEBUG(  "EMECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      } 
	      if(eta<0){
		m_hLArAffectedRegionsEMEC[1]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/256);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	    phi = -TMath::Pi()+0.01;
	    while(phi<phimax){
	      if(eta>=0){
		m_hLArAffectedRegionsEMEC[0]->Fill(eta,phi,problem);
		ATH_MSG_DEBUG(  "EMECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      } 
	      if(eta<0){
		m_hLArAffectedRegionsEMEC[1]->Fill(eta,phi,problem); 
		ATH_MSG_DEBUG(  "EMECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	      }
	      phi+=(2*TMath::Pi()/256);
              if(fabs(phi)<1.e-4) phi=0.;
	    }
	  }
	  if(fabs(eta)>=2.8) eta+=0.4;
	  else if(fabs(eta)>=2.5)eta+=0.3;
	  else eta+=0.2;
	}
      }
      
      // HEC
      if(layermin>=8 && layermin<=11){
	float eta = etamin; 
	// phimin < phimax
	if(phimin<=phimax){
	  float phi = phimin+0.01;
	  while(phi<phimax){
	    if(eta>=0){
	      m_hLArAffectedRegionsHECA[layermin-8]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "HECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	    } 
	    if(eta<0){
	      m_hLArAffectedRegionsHECC[layermin-8]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "HECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " "  << region  );
	    } 
	    phi+=(2*TMath::Pi()/32);
            if(fabs(phi)<1.e-4) phi=0.;
	  }
	// phimin < phimax
	}else{
	  float phi = phimin+0.01;
	  while(phi<TMath::Pi()){
	    if(eta>=0){
	      m_hLArAffectedRegionsHECA[layermin-8]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "HECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	    } 
	    if(eta<0){
	      m_hLArAffectedRegionsHECC[layermin-8]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "HECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " "  << region  );
	    } 
	    phi+=(2*TMath::Pi()/32);
            if(fabs(phi)<1.e-4) phi=0.;
	  }
	  phi = -TMath::Pi();
	  while(phi<phimax){
	    if(eta>=0){
	      m_hLArAffectedRegionsHECA[layermin-8]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "HECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	    } 
	    if(eta<0){
	      m_hLArAffectedRegionsHECC[layermin-8]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "HECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " "  << region  );
	    } 
	    phi+=(2*TMath::Pi()/32);
            if(fabs(phi)<1.e-4) phi=0.;
	  }
	}
      }
      
      // FCAL
      if(layermin>=21 && layermin<=23){
	float eta = etamin; 
	// phimin < phimax
	if(phimin<=phimax){
	  float phi = phimin+0.1;
	  while(phi<phimax){
	    if(eta>=0){
	      m_hLArAffectedRegionsFCALA[layermin-21]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "FCALA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	    } 
	    if(eta<0){
	      m_hLArAffectedRegionsFCALC[layermin-21]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "FCALC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	    } 
	    phi+=(2*TMath::Pi()/16);
            if(fabs(phi)<1.e-4) phi=0.;
	  }
	// phimin > phimax
	}else{
	  float phi = phimin+0.1;
	  while(phi<TMath::Pi()){
	    if(eta>=0){
	      m_hLArAffectedRegionsFCALA[layermin-21]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "FCALA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	    } 
	    if(eta<0){
	      m_hLArAffectedRegionsFCALC[layermin-21]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "FCALC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	    } 
	    phi+=(2*TMath::Pi()/16);
            if(fabs(phi)<1.e-4) phi=0.;
	  }
	  phi = -TMath::Pi()+0.1;
	  while(phi<phimax){
	    if(eta>=0){
	      m_hLArAffectedRegionsFCALA[layermin-21]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "FCALA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	    } 
	    if(eta<0){
	      m_hLArAffectedRegionsFCALC[layermin-21]->Fill(eta,phi,problem); 
	      ATH_MSG_DEBUG(  "FCALC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << region  );
	    } 
	    phi+=(2*TMath::Pi()/16);
            if(fabs(phi)<1.e-4) phi=0.;
	  }
	}
      }	
      
    } // End Calo Affected Regions
  } // End of event treatment
  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/
StatusCode LArAffectedRegions::procHistograms()
{

  ATH_MSG_DEBUG( "In procHistograms "  );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void LArAffectedRegions::SetHistoStyle(TH2I_LW* h)
{
    h->GetXaxis()->SetTitle("#eta");
    h->GetYaxis()->SetTitle("#phi");
    h->SetMinimum(0);
    h->SetMaximum(2);
    return;
}
/*---------------------------------------------------------*/

