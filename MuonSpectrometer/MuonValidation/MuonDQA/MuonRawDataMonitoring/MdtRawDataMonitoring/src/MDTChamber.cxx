/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTChamber.h"

#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "TH2F.h"

#include <sstream>
#include <cmath>
#include <iostream>

MDTChamber::MDTChamber(std::string_view name) :
  mdttdc(nullptr),
  mdttdccut_ML1(nullptr),
  mdttdccut_ML2(nullptr),
  mdtadc_onSegm_ML1(nullptr),
  mdtadc_onSegm_ML2(nullptr),
  //mdttdccut_RPCtrig_ML1(0),
  //mdttdccut_TGCtrig_ML1(0),
  //mdttdccut_RPCtrig_ML2(0),
  //mdttdccut_TGCtrig_ML2(0),
  mdtadc(nullptr),
  mdttdcadc(nullptr),
  //mdtmultil(0),
  mdtlayer(nullptr),
  mdttube(nullptr),
  //mdttube_bkgrd(0),
  //mdttube_fornoise(0),
  mdttube_masked(nullptr),
  mdtmezz(nullptr),
  //NEW
  mdt_effEntries(nullptr),
  mdt_effCounts(nullptr),
  mdt_effPerTube(nullptr),
  mdt_DRvsDT(nullptr),
  mdt_DRvsDRerr(nullptr),
  mdt_DRvsSegD(nullptr),
  mdttubenoise(nullptr),
  mdttdctube(nullptr),
  m_hardware_name(std::string(name)),
  m_mdthitsperchamber_InnerMiddleOuterLumi_bin(0),
  m_mdthitsperchamber_InnerMiddleOuterLumi_binx(0),
  m_mdthitsperchamber_InnerMiddleOuterLumi_biny(0),
  m_mdthitspermultilayerLumi_bin_m1(0),
  m_mdthitspermultilayerLumi_bin_m2(0),
  m_mdthitspermultilayerLumi_bin_m1_binx(0),
  m_mdthitspermultilayerLumi_bin_m1_biny(0),
  m_mdthitspermultilayerLumi_bin_m2_biny(0),
  m_mdthitsperML_byLayer_bin_m1(0),
  m_mdthitsperML_byLayer_bin_m2(0),
  m_binx(0),
  m_biny_m1(0),
  m_biny_m2(0)
{

  m_barrel_region = 0;
  m_layer_region = 0;
  m_station_phi = 0;
  m_station_eta = 0;
  m_crate = 0;
  m_crate_str = "XXXX";
  if(name.size() >= 7){

    //BOS6A16
    //0123456
    char Barrel = name.at(0);
    char Side = name.at(4);
    
    //set m_barrel_region
    if( Barrel=='B' && (Side=='A' || Side=='B') ) m_barrel_region = 0;
    else if( Barrel=='B' && Side=='C' ) m_barrel_region = 1;
    else if( Barrel=='E' && (Side=='A' || Side=='B') ) m_barrel_region = 2;
    else if( Barrel=='E' && Side=='C' ) m_barrel_region = 3;
    else {
      //      ATH_MSG_DEBUG("hardware_name doesn't make sense!" );
    }
    
    std::stringstream ss;
    //set m_layer_region
    char layerName = m_hardware_name.at(1);
    if(layerName=='I') m_layer_region = 0;
    else if(layerName=='M') m_layer_region = 1;
    else if(layerName=='O') m_layer_region = 2;
    else if(layerName=='E') m_layer_region = 3;
    
    //set m_station_eta
    ss << name.at(3);
    ss >> m_station_eta;
    ss.clear();
    
    //set m_station_phi
    std::string_view statphi_s(name.substr(5,2));
    ss << statphi_s;
    ss >> m_station_phi;
    m_station_phi--;
    ss.clear();
  std::string_view name_v(m_hardware_name);
  //First there were only BMF1,2,3, then BMGs were added which are between the BMFs, so doing the eta-station assignment by hand.
  if( name_v.substr(0, 4) == "BMF2") m_station_eta = 3; 
  if( name_v.substr(0, 4) == "BMF3") m_station_eta = 5; 
    
    //station_phi is used as an iterator, and is thus 1 less than its value (since it always starts at 01, not 00).

    if((Barrel == 'B' && name.substr(0,3)!="BEE" && name.substr(0,3) != "BIS") || (name.substr(0,3) == "BIS" && m_station_eta < 6) ){
	if(m_station_phi < 4 || name.substr(0,3) == "BME"){
	    m_crate = 1;
	    if(Side == 'A') m_crate_str = "BA01";
	    else if(Side == 'C') m_crate_str = "BC01";
	}
	else if(m_station_phi < 8 && m_station_phi > 3){
    	    m_crate = 2;
	    if(Side == 'A') m_crate_str = "BA02";
	    else if(Side == 'C') m_crate_str = "BC02";
	}
	else if(m_station_phi < 12 && m_station_phi > 7){
 	    m_crate = 3;
	    if(Side == 'A' || Side == 'B') m_crate_str = "BA03";
	    else if(Side == 'C') m_crate_str = "BC03";
	}
  	else if(m_station_phi > 11){
	    m_crate = 4;
	    if(Side == 'A' || Side == 'B') m_crate_str = "BA04";
	    else if(Side == 'C') m_crate_str = "BC04";
	}    
     }
    else if( (Barrel == 'E') || name.substr(0,3)=="BEE" || (name.substr(0,3)=="BIS" && m_station_eta > 5) ){
	if(m_station_phi < 4){
	    m_crate = 1;
	    if(Side == 'A') m_crate_str = "EA01";
	    else if(Side == 'C') m_crate_str = "EC01";
	}
	else if(m_station_phi < 8 && m_station_phi > 3){
	    m_crate = 2;
	    if(Side == 'A') m_crate_str = "EA02";
	    else if(Side == 'C') m_crate_str = "EC02";
	}
	else if(m_station_phi < 12 && m_station_phi > 7){
	    m_crate = 3;
	    if(Side == 'A') m_crate_str = "EA03";
	    else if(Side == 'C') m_crate_str = "EC03";
	}
  	else if(m_station_phi > 11){
	    m_crate = 4;
	    if(Side == 'A') m_crate_str = "EA04";
	    else if(Side == 'C') m_crate_str = "EC04";
         }
    }
    else {
    }
  }
  else {
    //    ATH_MSG_DEBUG("BAD CHAMBER NAME");
  }
  
}

MDTChamber::~MDTChamber(){}

void MDTChamber::SetMDTHitsPerChamber_IMO_Bin(TH2F* h){
  //Plots BEE Chambers at the edge BIS plots, and EES/EEL on EIS/EIL plots
  //EIL goes up to EIL5 in hardware name but in reality there are only 4 eta stations!
  std::string_view name_v(m_hardware_name);
  std::string_view station_eta_s = name_v.substr(3,1);
  std::string_view statphi_s = name_v.substr(5,2);
  if(name_v.substr(0,4)=="BMF2") station_eta_s = "3";
  if(name_v.substr(0,4)=="BMF3") station_eta_s = "5";

  std::string ecap_layer_IMO = std::string(name_v.substr(0,1));
  ecap_layer_IMO += name_v.substr(4,1);
  ecap_layer_IMO += station_eta_s;

  if( m_station_phi == 12 && (m_station_eta == 4 || m_station_eta == 5) ) {//m_station_phi+1==actual phi station
    if(name_v == "BML4A13") ecap_layer_IMO = "BA5";
    if(name_v == "BML5A13") ecap_layer_IMO = "BA6";
    if(name_v == "BML4C13") ecap_layer_IMO = "BC5";
    if(name_v == "BML5C13") ecap_layer_IMO = "BC6";
  }

  if(name_v == "BME1A14" || name_v == "BME1A13" ){ //protect against future fix in mdt helper
	  ecap_layer_IMO = "BA4";
	  statphi_s = "13";
  }
  if(name_v == "BME1C14" || name_v == "BME1C13"){
	  ecap_layer_IMO = "BC4";
	  statphi_s = "13";
  }
  //First there were only BMF1,2,3, then BMGs were added which are between the BMFs, so doing the ecap-layer assignment by hand.
  if( name_v.substr(0, 5) == "BMF2A") ecap_layer_IMO  = "BA3";
  if( name_v.substr(0, 5) == "BMF3A") ecap_layer_IMO  = "BA5";
  if( name_v.substr(0, 5) == "BMF2C") ecap_layer_IMO = "BC3";
  if( name_v.substr(0, 5) == "BMF3C") ecap_layer_IMO = "BC5";

  std::string statphi_IMO_s = std::string(name_v.substr(1,1));
  statphi_IMO_s+=',';
  statphi_IMO_s+=statphi_s;
  //Separate pesky BIR/BIM 11,15
  if(name_v.substr(0,3)=="BIR" || name_v.substr(0,3)=="BIM"){
    statphi_IMO_s += ',';
    statphi_IMO_s += name_v.at(2);
  }

  int binx = h->GetXaxis()->FindBin( ecap_layer_IMO.c_str() );
  int biny = h->GetYaxis()->FindBin( statphi_IMO_s.c_str() );
  m_mdthitsperchamber_InnerMiddleOuterLumi_binx = binx;
  m_mdthitsperchamber_InnerMiddleOuterLumi_biny = biny;
  m_mdthitsperchamber_InnerMiddleOuterLumi_bin = h->GetBin(binx, biny);

}


void MDTChamber::SetMDTHitsPerML_byLayer_Bins(TH2F* h_mdthitspermultilayerLumi, TH2F* h_mdthitsperML_byLayer){
  std::string statphi_s = m_hardware_name.substr(5,2);

  if(m_hardware_name == "BME1A14" || m_hardware_name == "BME1C14") statphi_s = "13";
  std::string_view name_v(m_hardware_name);
  std::string statphi_ml1_s = statphi_s+",1";
  std::string statphi_ml2_s = statphi_s+",2";

  //Separate pesky BIR/BIM 11,15
  if(m_hardware_name.substr(0,3)=="BIR" || m_hardware_name.substr(0,3)=="BIM"){
    statphi_ml1_s = statphi_ml1_s +","+m_hardware_name.at(2);
    statphi_ml2_s = statphi_ml2_s +","+m_hardware_name.at(2);
  }

  std::string stateta_s = m_hardware_name.substr(3,1);
  if(name_v.substr(0,4)=="BMF2") stateta_s = "3";
  if(name_v.substr(0,4)=="BMF3") stateta_s = "5";
  std::string ecap_layer = m_hardware_name.substr(0,2) + m_hardware_name.at(4) + stateta_s;
  
  if( m_station_phi == 12 && (m_station_eta == 4 || m_station_eta == 5) ) {//m_station_phi+1==actual phi station
      if(m_hardware_name == "BML4A13") ecap_layer = "BMA5";
      if(m_hardware_name == "BML5A13") ecap_layer = "BMA6";
      if(m_hardware_name == "BML4C13") ecap_layer = "BMC5";
      if(m_hardware_name == "BML5C13") ecap_layer = "BMC6";
    }
    if(m_hardware_name == "BME1A14" || m_hardware_name == "BME1A13"){
  	  ecap_layer = "BMA4";
    }
    if(m_hardware_name == "BME1C14" || m_hardware_name == "BME1C13"){
  	  ecap_layer = "BMC4";
    }
  
  // Setting BMF by hand because of irregular naming convention. BMF and BMG chambers alternate; historical BMF naming is BMF1,2,3 but BMG it is 2,4,6  
  if( name_v.substr(0, 5) == "BMF2A") ecap_layer = "BMA3";
  if( name_v.substr(0, 5) == "BMF3A") ecap_layer = "BMA5";
  if( name_v.substr(0, 5) == "BMF2C") ecap_layer = "BMC3";
  if( name_v.substr(0, 5) == "BMF3C") ecap_layer = "BMC5";

  int binx = h_mdthitspermultilayerLumi->GetXaxis()->FindBin(ecap_layer.c_str());
  int biny_m1 = h_mdthitspermultilayerLumi->GetYaxis()->FindBin(statphi_ml1_s.c_str());
  int biny_m2 = biny_m1; 
  if(!(TString(h_mdthitspermultilayerLumi->GetName()) == "NumberOfHitsInBAExtraPerMultiLayer_ADCCut" || 
       TString(h_mdthitspermultilayerLumi->GetName()) == "NumberOfHitsInBCExtraPerMultiLayer_ADCCut")) 
    biny_m2 = h_mdthitspermultilayerLumi->GetYaxis()->FindBin(statphi_ml2_s.c_str()); // Do not create extra bins

  m_mdthitspermultilayerLumi_bin_m1 = h_mdthitspermultilayerLumi->GetBin(binx, biny_m1);
  m_mdthitspermultilayerLumi_bin_m2 = h_mdthitspermultilayerLumi->GetBin(binx, biny_m2);


  m_mdthitspermultilayerLumi_bin_m1_binx = binx;  
  m_mdthitspermultilayerLumi_bin_m1_biny = biny_m1;
  m_mdthitspermultilayerLumi_bin_m2_biny = biny_m2;

  
  if((statphi_ml1_s.substr(0,2)=="11"||statphi_ml1_s.substr(0,2)=="15")  && m_hardware_name.at(0) =='E'  && (m_layer_region == 0 || m_layer_region == 3)) {
    statphi_ml1_s+=",R";   //Correction for EIR/EIM
    statphi_ml2_s+=",R";   //Correction for EIR/EIM
  }

  m_binx = h_mdthitsperML_byLayer->GetXaxis()->FindBin(ecap_layer.c_str());
  m_biny_m1 = h_mdthitsperML_byLayer->GetYaxis()->FindBin(statphi_ml1_s.c_str());
  m_biny_m2 = h_mdthitsperML_byLayer->GetYaxis()->FindBin(statphi_ml2_s.c_str());

  m_mdthitsperML_byLayer_bin_m1 = h_mdthitsperML_byLayer->GetBin(binx, biny_m1);
  m_mdthitsperML_byLayer_bin_m2 = h_mdthitsperML_byLayer->GetBin(binx, biny_m2);


}

