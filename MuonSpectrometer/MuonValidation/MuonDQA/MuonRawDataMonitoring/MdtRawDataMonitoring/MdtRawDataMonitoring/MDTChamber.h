/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////////////////
// HelperClass : MDTChamber
// Author:   Orin Harris, Justin Griffiths, John Keller (University of Washington) 
// Sept. 2010
//
// DESCRIPTION:
// Subject: MDT-->Offline Muon Data Quality
///////////////////////////////////////////////////////////////////////////////////////////
#include <string>

class TH1F_LW;
class TH2F_LW;
class TH2F; // no TH2Fs are ever allocated by this class or owned

class MDTChamber {

 public:
  MDTChamber( std::string name );
  ~MDTChamber();

  TH1F_LW* mdttdc;
  TH1F_LW* mdttdccut_ML1;
  TH1F_LW* mdttdccut_ML2;
  TH1F_LW* mdtadc_onSegm_ML1;
  TH1F_LW* mdtadc_onSegm_ML2;

  TH1F_LW* mdtadc;
  TH2F_LW* mdttdcadc;
  TH1F_LW* mdtlayer;
  TH1F_LW* mdttube;

  TH1F_LW* mdttube_masked;
  TH1F_LW* mdtmezz;

  //NEW
  TH1F_LW* mdt_effEntries;
  TH1F_LW* mdt_effCounts;
  TH1F_LW* mdt_effPerTube;
  TH2F_LW* mdt_DRvsDT;
  TH2F_LW* mdt_DRvsDRerr;
  TH2F_LW* mdt_DRvsSegD;

  //remove unused plots to reduce memory consumption
/*  
  TH1F_LW* mdttdccut_RPCtrig_ML1;
  TH1F_LW* mdttdccut_TGCtrig_ML1;
  TH1F_LW* mdttdccut_RPCtrig_ML2;
  TH1F_LW* mdttdccut_TGCtrig_ML2;
  TH1F_LW* mdttube_bkgrd;
  TH1F_LW* mdttube_fornoise;
  TH1F_LW* mdtmultil;
*/
  
  //  MonGroup * mdtprd_expert; 

  // TH2F* mdthitspermultilayer[3][3];
 
  TH1F_LW* mdttubenoise; // just added
  TH1F_LW* mdttdctube;   // just added

  inline std::string getName() { return m_hardware_name; }
  //return 0 for barrel, 1 for endcap, based on m_barrel_region
  inline int GetBarrelEndcapEnum(){ return (m_barrel_region < 2 ? 0 : 1); }
  inline int GetRegionEnum(){ return m_barrel_region; }
  inline int GetLayerEnum(){ return m_layer_region; }
  inline int GetStationEta() { return m_station_eta; }
  inline int GetStationPhi() { return m_station_phi; }

  inline int GetMDTHitsPerChamber_IMO_Bin() { return m_mdthitsperchamber_InnerMiddleOuterLumi_bin; }
  inline int GetMDTHitsPerML_byLayer_Bin(int multi_layer) { return ( multi_layer == 1 ? m_mdthitsperML_byLayer_bin_m1 : m_mdthitsperML_byLayer_bin_m2); }
  inline int GetMDTHitsPerML_Bin(int multi_layer) { return ( multi_layer == 1 ? m_mdthitspermultilayerLumi_bin_m1 : m_mdthitspermultilayerLumi_bin_m2); }

  inline int GetCrate(){ return m_crate; }
  inline std::string GetCrate_String(){ return m_crate_str; }

  void SetMDTHitsPerChamber_IMO_Bin(TH2F*);
  void SetMDTHitsPerML_byLayer_Bins(TH2F*, TH2F*);

 private:
  std::string m_hardware_name;
  //BA=0,BC,EA,EC=3
  short int m_barrel_region;
  short int m_layer_region;
  short int m_station_eta;
  short int m_station_phi;

  int m_crate;
//m_crate returns the number of the crate: 1,2,3,4
//m_crate_str returns the entire string of the crate, e.g. BA01.
  std::string m_crate_str;

  //global bin vals for the colorful TH2F plots
  short int m_mdthitsperchamber_InnerMiddleOuterLumi_bin;
  short int m_mdthitspermultilayerLumi_bin_m1;
  short int m_mdthitspermultilayerLumi_bin_m2;
  short int m_mdthitsperML_byLayer_bin_m1;
  short int m_mdthitsperML_byLayer_bin_m2;
};
