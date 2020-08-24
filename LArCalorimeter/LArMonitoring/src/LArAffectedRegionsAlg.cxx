/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArAffectedRegionsAlg.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Margherita Spalla (migrated from algorithm by Jessica Leveque)
//
// Class for monitoring : Map of Affected Regions
//                        built from "Affected Region" metadata, stored in ESDs.
// ********************************************************************

#include "LArAffectedRegionsAlg.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"

#include "TMath.h"
#include <vector>







/*---------------------------------------------------------*/
LArAffectedRegionsAlg::LArAffectedRegionsAlg(const std::string& name,ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}

/*---------------------------------------------------------*/
LArAffectedRegionsAlg::~LArAffectedRegionsAlg()
{}

/*---------------------------------------------------------*/
StatusCode 
LArAffectedRegionsAlg::initialize()
{
  ATH_MSG_INFO( "Initialize LArAffectedRegionsAlg"  );

  ATH_CHECK( m_affKey.initialize() );

  return AthMonitorAlgorithm::initialize();
}


/*---------------------------------------------------------*/
StatusCode 
LArAffectedRegionsAlg::fillHistograms( const EventContext& ctx ) const
{
  using namespace Monitored;

  ATH_MSG_DEBUG( "in fillHists()"  );

  if(ctx.evt() > 0) { //only run on first event (it has ctx.evt()=0)
    ATH_MSG_DEBUG( "won't run on: evt()="<<ctx.evt()<<">1");  
    return StatusCode::SUCCESS;
  }

  //monitored variables
  auto problem = Monitored::Scalar<int>("problem",0);
  auto phi = Monitored::Scalar<float>("phi",0.0);
  auto etaPOS = Monitored::Scalar<float>("etaPOS",0.0);
  auto etaNEG = Monitored::Scalar<float>("etaNEG",0.0);
  float eta; //this we will need later


  //do nothing if online
  if(m_IsOnline)  return StatusCode::SUCCESS;
  
  // Retrieve CaloAffected Regions
  SG::ReadCondHandle<CaloAffectedRegionInfoVec> affHdl{m_affKey, ctx};
  const CaloAffectedRegionInfoVec* affRegVec=*affHdl;
  if (!affRegVec) {
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
    problem=region->get_problem();
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
      //now get the proper eta-phi
      eta = etamin+0.01; 
      while(eta<etamax){
	// phimin < phimax
	if(phimin<=phimax){
	  phi = phimin+0.01;
	  while(phi<phimax){
	    if(eta>=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_embpsName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMBAPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    if(eta<0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_embpsName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMBCPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    phi+=(2*TMath::Pi()/32);
	    if(fabs(phi)<1.e-4) phi=0.;
	  }
	  // phimin > phimax
	}else{
	  phi = phimin+0.01;
	  while(phi<TMath::Pi()){
	    if(eta>=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_embpsName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMBAPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    if(eta<0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_embpsName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMBCPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    phi+=(2*TMath::Pi()/32);
	    if(fabs(phi)<1.e-4) phi=0.;
	  }
	  phi = -TMath::Pi();
	  while(phi<phimax){
	    if(eta>=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_embpsName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMBAPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    if(eta<0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_embpsName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMBCPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
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
      //now get the proper eta-phi
      eta = etamin; 
      while(eta < etamax){
	// phimin < phimax
	if(phimin<=phimax){
	  phi = phimin+0.01;
	  while(phi<phimax){
	    if(eta >=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_embName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMBA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    if(eta <0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_embName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMBC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    phi+=(2*TMath::Pi()/128);
	    if(fabs(phi)<1.e-4) phi=0.;
	  }
	  // phimin > phimax: go over Pi
	}else{
	  phi = phimin+0.01;
	  while(phi<TMath::Pi()){
	    if(eta >=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_embName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMBA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    if(eta <0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_embName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMBC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    phi+=(2*TMath::Pi()/128);
	    if(fabs(phi)<1.e-4) phi=0.;
	  }
	  phi = -TMath::Pi();
	  while(phi<phimax){
	    if(eta >=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_embName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMBA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    if(eta <0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_embName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMBC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
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
      //now get the proper eta-phi
      eta = etamin; 
      while(eta<etamax){
	// phimin < phimax
	if(phimin<=phimax){
	  phi = phimin+0.01;
	  while(phi<phimax){
	    if(eta>=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_emecpsName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMECAPS " << eta << " " << phi << " " << problem << "  " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    if(eta<0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_emecpsName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMECCPS " << eta << " " << phi << " " << problem << "  " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    phi+=(2*TMath::Pi()/64);
	    if(fabs(phi)<1.e-4) phi=0.;
	  }
	  // phimin > phimax
	}else{
	  phi = phimin+0.01;
	  while(phi<TMath::Pi()){
	    if(eta>=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_emecpsName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMECAPS " << eta << " " << phi << " " << problem << "  " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    if(eta<0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_emecpsName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMECCPS " << eta << " " << phi << " " << problem << "  " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    phi+=(2*TMath::Pi()/64);
	    if(fabs(phi)<1.e-4) phi=0.01;
	  }
	  phi = -TMath::Pi()+0.01;
	  while(phi<phimax){
	    if(eta>=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_emecpsName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMECAPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    if(eta<0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_emecpsName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMECCPS " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
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
      //now get the proper eta-phi
      eta = etamin+0.01; 
      while(eta<etamax){
	// phimin < phimax
	if(phimin<=phimax){
	  phi = phimin+0.01;
	  while(phi<phimax){
	    if(eta>=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_emecName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    } 
	    if(eta<0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_emecName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    phi+=(2*TMath::Pi()/256);
	    if(fabs(phi)<1.e-4) phi=0.;
	  }
	  // phimin > phimax
	}else{
	  phi = phimin+0.01;
	  while(phi<TMath::Pi()){
	    if(eta>=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_emecName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    } 
	    if(eta<0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_emecName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    }
	    phi+=(2*TMath::Pi()/256);
	    if(fabs(phi)<1.e-4) phi=0.;
	  }
	  phi = -TMath::Pi()+0.01;
	  while(phi<phimax){
	    if(eta>=0){
	      etaPOS=eta;
	      fill(m_MonGroupName.value()+m_emecName,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "EMECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	    } 
	    if(eta<0){
	      etaNEG=eta;
	      fill(m_MonGroupName.value()+m_emecName,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "EMECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
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
      //set the group to fill(only depends on layer)
      std::string hec_layer_name=m_MonGroupName;
      switch(layermin-8) {
      case 0:
	hec_layer_name=hec_layer_name+m_hec0Name;
	break;
      case 1:
	hec_layer_name=hec_layer_name+m_hec1Name;
	break;
      case 2:
	hec_layer_name=hec_layer_name+m_hec2Name;
	break;
      default: //this can only be ==3, because of previous if statement
	hec_layer_name=hec_layer_name+m_hec3Name;
	break;
      }

      //now get the proper eta-phi
      eta = etamin; 
      // phimin < phimax
      if(phimin<=phimax){
	phi = phimin+0.01;
	while(phi<phimax){
	  if(eta>=0){
	      etaPOS=eta;
	      fill(hec_layer_name,etaPOS,phi,problem);
	      ATH_MSG_DEBUG(  "HECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	  } 
	  if(eta<0){
	      etaNEG=eta;
	      fill(hec_layer_name,etaNEG,phi,problem);
	      ATH_MSG_DEBUG(  "HECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " "  << region  );
	  } 
	  phi+=(2*TMath::Pi()/32);
	  if(fabs(phi)<1.e-4) phi=0.;
	}
	// phimin < phimax
      }else{
	phi = phimin+0.01;
	while(phi<TMath::Pi()){
	  if(eta>=0){
	    etaPOS=eta;
	    fill(hec_layer_name,etaPOS,phi,problem);
	    ATH_MSG_DEBUG(  "HECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	  } 
	  if(eta<0){
	    etaNEG=eta;
	    fill(hec_layer_name,etaNEG,phi,problem);
	    ATH_MSG_DEBUG(  "HECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " "  << region  );
	  } 
	  phi+=(2*TMath::Pi()/32);
	  if(fabs(phi)<1.e-4) phi=0.;
	}
	phi = -TMath::Pi();
	while(phi<phimax){
	  if(eta>=0){
	    etaPOS=eta;
	    fill(hec_layer_name,etaPOS,phi,problem);
	    ATH_MSG_DEBUG(  "HECA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	  } 
	  if(eta<0){
	    etaNEG=eta;
	    fill(hec_layer_name,etaNEG,phi,problem);
	    ATH_MSG_DEBUG(  "HECC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " "  << region  );
	  } 
	  phi+=(2*TMath::Pi()/32);
	  if(fabs(phi)<1.e-4) phi=0.;
	}
      }
    }
    
    // FCAL
    ATH_MSG_INFO("layermin=" << layermin << " layermin-21=" << layermin-21);
    if(layermin>=21 && layermin<=23){
      //set the group to fill(only depends on layer)
      std::string fcal_layer_name=m_MonGroupName;
      switch(layermin-21) {
      case 0:
	fcal_layer_name=fcal_layer_name+m_fcal0Name;
	break;
      case 1:
	fcal_layer_name=fcal_layer_name+m_fcal1Name;
	break;
      default: //this can only be ==2, because of previous if statement
	fcal_layer_name=fcal_layer_name+m_fcal2Name;
	break;
      }

      //now get the proper eta-phi
      eta = etamin; 
      // phimin < phimax
      if(phimin<=phimax){
	phi = phimin+0.1;
	while(phi<phimax){
	  if(eta>=0){
	    etaPOS=eta;
	    fill(fcal_layer_name,etaPOS,phi,problem);
	    ATH_MSG_DEBUG(  "FCALA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	  } 
	  if(eta<0){
	    etaNEG=eta;
	    fill(fcal_layer_name,etaNEG,phi,problem);
	    ATH_MSG_DEBUG(  "FCALC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	  } 
	  phi+=(2*TMath::Pi()/16);
	  if(fabs(phi)<1.e-4) phi=0.;
	}
	// phimin > phimax
      }else{
	phi = phimin+0.1;
	while(phi<TMath::Pi()){
	  if(eta>=0){
	    etaPOS=eta;
	    fill(fcal_layer_name,etaPOS,phi,problem);
	    ATH_MSG_DEBUG(  "FCALA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	  } 
	  if(eta<0){
	    etaNEG=eta;
	    fill(fcal_layer_name,etaNEG,phi,problem);
	    ATH_MSG_DEBUG(  "FCALC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	  } 
	  phi+=(2*TMath::Pi()/16);
	  if(fabs(phi)<1.e-4) phi=0.;
	}
	phi = -TMath::Pi()+0.1;
	while(phi<phimax){
	  if(eta>=0){
	    etaPOS=eta;
	    fill(fcal_layer_name,etaPOS,phi,problem);
	    ATH_MSG_DEBUG(  "FCALA " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	  } 
	  if(eta<0){
	    etaNEG=eta;
	    fill(fcal_layer_name,etaNEG,phi,problem);
	    ATH_MSG_DEBUG(  "FCALC " << eta << " " << phi << " " << problem << " " << layermin << " " << layermax << " " << phimin << " " << phimax << " " << etamin << " " << etamax << " " << region  );
	  } 
	  phi+=(2*TMath::Pi()/16);
	  if(fabs(phi)<1.e-4) phi=0.;
	}
      }
    }  
  } // End Calo Affected Regions

  return StatusCode::SUCCESS;
}



