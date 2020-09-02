/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "TH2D.h"
#include "TH3D.h"
#include "TFile.h"
#include "RadiationMapsMakerTool.h"

namespace G4UA{ 

  RadiationMapsMakerTool::RadiationMapsMakerTool(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
    : UserActionToolBase<RadiationMapsMaker>(type, name, parent),
      m_radMapsFileName("RadMaps.root")  
  {
    /// Output Filename for the Radiation Maps
    declareProperty("RadMapsFileName", m_radMapsFileName);
    /// Name of the material to make radiation maps for (take all if empty) 
    declareProperty("Material"       , m_config.material);
    /// If true consider hits with y>0 only -- useful for shafts
    declareProperty("PositiveYOnly"  , m_config.posYOnly);
    /// map granularities 
    /// number of bins in r and z for all 2D maps
    declareProperty("NBinsR"         , m_config.nBinsr);
    declareProperty("NBinsZ"         , m_config.nBinsz);
    /// number of bins in logE for energy spectra of neutrons in 2D grids
    declareProperty("NBinsLogEn"     , m_config.nBinslogEn);
    /// number of bins in logE for energy spectra of other particles in 2D grids
    declareProperty("NBinsLogEo"     , m_config.nBinslogEo);
    /// number of bins in dphi for dphi x theta dependent energy spectra
    declareProperty("NBinsDPhi"      , m_config.nBinsdphi);
    /// number of bins in theta for dphi x theta dependent energy spectra
    declareProperty("NBinsTheta"     , m_config.nBinstheta);
    /// number of bins in r, z and phi for all 3D maps
    declareProperty("NBinsR3D"       , m_config.nBinsr3d);
    declareProperty("NBinsZ3D"       , m_config.nBinsz3d);
    declareProperty("NBinsPhi3D"     , m_config.nBinsphi3d);
    /// number of bins in logTimeCut for time dependent TID 2D maps
    declareProperty("NBinsLogTimeCut", m_config.nBinslogT);
    /// map ranges
    /// for Zoomed area in 2D and 3D
    declareProperty("RMinZoom"       , m_config.rMinZoom);
    declareProperty("RMaxZoom"       , m_config.rMaxZoom);
    declareProperty("ZMinZoom"       , m_config.zMinZoom);
    declareProperty("ZMaxZoom"       , m_config.zMaxZoom);
    /// for Full detector in 2D
    declareProperty("RMinFull"       , m_config.rMinFull);
    declareProperty("RMaxFull"       , m_config.rMaxFull);
    declareProperty("ZMinFull"       , m_config.zMinFull);
    declareProperty("ZMaxFull"       , m_config.zMaxFull);
    /// for Zoomed area in 3D 
    declareProperty("PhiMinZoom"     , m_config.phiMinZoom);
    declareProperty("PhiMaxZoom"     , m_config.phiMaxZoom);
    /// for logE of neutrons in 2D spectra
    declareProperty("LogEMinn"       , m_config.logEMinn);
    declareProperty("LogEMaxn"       , m_config.logEMaxn);
    /// for logE of other particles in 2D spectra
    declareProperty("LogEMino"       , m_config.logEMino);
    declareProperty("LogEMaxo"       , m_config.logEMaxo);
    /// for Theta in 2D spectra
    declareProperty("ThetaMin"       , m_config.thetaMin);
    declareProperty("ThetaMax"       , m_config.thetaMax);
    /// for logT in time-dependent TID 2D maps
    declareProperty("LogTMin"        , m_config.logTMin);
    declareProperty("LogTMax"        , m_config.logTMax);
    /// for elements mass fracion 2D maps
    declareProperty("ElemZMin"       , m_config.elemZMin);
    declareProperty("ElemZMax"       , m_config.elemZMax);
  }

  //---------------------------------------------------------------------------
  // Initialize Configurable Properties
  //---------------------------------------------------------------------------
  StatusCode RadiationMapsMakerTool::initialize()
  {
    ATH_MSG_INFO( "Initializing     " << name()              << "\n"                                              <<
                  "OutputFile:      " << m_radMapsFileName   << "\n"                                              << 
                  "Material:        " << m_config.material   << "\n"                                              << 
                  "PositiveYOnly:   " << m_config.posYOnly   << "\n"                                              << 
                  "2D Maps:         " << m_config.nBinsz     << (m_config.zMinFull<0?" z-bins, ":" |z|-bins, ")   << 
		                         m_config.nBinsr     << " r-bins"                                         << "\n"                << 
		  "Zoom:            " << m_config.zMinZoom   << (m_config.zMinFull<0?" < z/cm < ":" < |z|/cm < ") << m_config.zMaxZoom   << ", " << 
                                         m_config.rMinZoom   << " < r/cm < "                                      << m_config.rMaxZoom   << "\n" << 
                  "Full:            " << m_config.zMinFull   << (m_config.zMinFull<0?" < z/cm < ":" < |z|/cm < ") << m_config.zMaxFull   << ", " << 
                                         m_config.rMinFull   << " < r/cm < "                                      << m_config.rMaxFull   << "\n" << 
                  "Neutron Spectra: " << m_config.nBinslogEn << " log10E-bins"                                    << ", "                <<
                                         m_config.logEMinn   << " < log10(E/MeV) < "                              << m_config.logEMaxn   << "\n" << 
                  "Other Spectra:   " << m_config.nBinslogEo << " log10E-bins"                                    << ", "                <<
                                         m_config.logEMino   << " < log10(E/MeV) < "                              << m_config.logEMaxo   << "\n" << 
                  "DPhi-Bins:       " << m_config.nBinsdphi << " dphi-bins, 0 < dphi < 360 \n" << 
		  "Theta-Bins:      " << m_config.nBinstheta << " theta-bins"                                     << ", "                  <<
                                         m_config.thetaMin   << " < theta < "                                     << m_config.thetaMax   << "\n" << 
                  "3D Maps:         " << m_config.nBinsz3d   << (m_config.zMinFull<0?" z-bins, ":" |z|-bins, ")   << 
                                         m_config.nBinsr3d   << " r-bins, "                                       << 
                                         m_config.nBinsphi3d << " phi-bins"                                       << "\n"                << 
                  "Zoom:            " << m_config.zMinZoom   << (m_config.zMinFull<0?" < z/cm < ":" < |z|/cm < ") << m_config.zMaxZoom   << ", " << 
                                         m_config.rMinZoom   << " < r/cm < "                                      << m_config.rMaxZoom   << ", " <<
                                         m_config.phiMinZoom << " < phi/degrees < "                               << m_config.phiMaxZoom << "\n" <<
                  "Time TID Maps:   " << m_config.nBinslogT  << " Time-cut bins, "                                <<
		                         m_config.logTMin    << " < log10(t_cut/s) < "                            << m_config.logTMax << "\n" <<
                  "Mass frac. Maps: " << m_config.elemZMax-m_config.elemZMin+1 << " Element bins, "                                <<
		                         m_config.elemZMin    << " <= Z <= < "                            << m_config.elemZMax );
      
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Merge results from all threads
  //---------------------------------------------------------------------------
  StatusCode RadiationMapsMakerTool::finalize()
  {
    ATH_MSG_DEBUG( "Finalizing " << name() );

    RadiationMapsMaker::Report maps;

    // vector of pointers to vectors of double to save space
    std::vector<std::vector<double> *> pVal = {
      &(maps.m_rz_tid),&(maps.m_rz_eion),&(maps.m_rz_niel),&(maps.m_rz_h20),&(maps.m_rz_neut),&(maps.m_rz_chad),
      &(maps.m_full_rz_tid),&(maps.m_full_rz_eion),&(maps.m_full_rz_niel),&(maps.m_full_rz_h20),&(maps.m_full_rz_neut),&(maps.m_full_rz_chad)
    };

    std::vector<std::vector<double> *> pVal3d = {
      &(maps.m_3d_tid),&(maps.m_3d_eion),&(maps.m_3d_niel),&(maps.m_3d_h20),&(maps.m_3d_neut),&(maps.m_3d_chad)
    };

    std::vector<std::vector<double> *> pValSpec = {
      &(maps.m_rz_neut_spec),&(maps.m_rz_gamm_spec),&(maps.m_rz_elec_spec),&(maps.m_rz_muon_spec),&(maps.m_rz_pion_spec),&(maps.m_rz_prot_spec),&(maps.m_rz_rest_spec),     
      &(maps.m_full_rz_neut_spec),&(maps.m_full_rz_gamm_spec),&(maps.m_full_rz_elec_spec),&(maps.m_full_rz_muon_spec),&(maps.m_full_rz_pion_spec),&(maps.m_full_rz_prot_spec),&(maps.m_full_rz_rest_spec)
    };

    std::vector<int> nBinslogE = {
      m_config.nBinslogEn,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,
      m_config.nBinslogEn,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo      
    };

    std::vector<std::vector<double> *> pValThetaSpec = {
      &(maps.m_theta_full_rz_neut_spec),&(maps.m_theta_full_rz_gamm_spec),&(maps.m_theta_full_rz_elec_spec),&(maps.m_theta_full_rz_muon_spec),&(maps.m_theta_full_rz_pion_spec),&(maps.m_theta_full_rz_prot_spec),&(maps.m_theta_full_rz_rchgd_spec),&(maps.m_theta_full_rz_rneut_spec)
    };

    std::vector<int> nBinsThetalogE = {
      m_config.nBinslogEn,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo,m_config.nBinslogEo
    };

    for(unsigned int hi=0;hi<pVal.size();hi++) {

      // first make sure the vectors are empty

      (pVal[hi])->resize(0);

      // then resize to proper size and initialize with 0's 
      // all maps are needed for the merge - so have to do resize
      // for all before merging any ...

      (pVal[hi])->resize(m_config.nBinsz*m_config.nBinsr,0.0);
    }

    // same for 3d 

    for(unsigned int hi=0;hi<pVal3d.size();hi++) {
      (pVal3d[hi])->resize(0);
      (pVal3d[hi])->resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    }

    // same for spectra

    for(unsigned int hi=0;hi<pValSpec.size();hi++) {
      (pValSpec[hi])->resize(0);
      (pValSpec[hi])->resize(m_config.nBinsz*m_config.nBinsr*nBinslogE[hi],0.0);
    }

    // same for theta x dphi spectra

    for(unsigned int hi=0;hi<pValThetaSpec.size();hi++) {
      (pValThetaSpec[hi])->resize(0);
      (pValThetaSpec[hi])->resize(m_config.nBinsdphi*m_config.nBinstheta*m_config.nBinsz*m_config.nBinsr*nBinsThetalogE[hi],0.0);
    }

    // same for misc individual maps
    
    maps.m_rz_tid_time      .resize(0);
    maps.m_full_rz_tid_time .resize(0);

    maps.m_rz_element       .resize(0);
    maps.m_full_rz_element  .resize(0);

    if (!m_config.material.empty()) {
      // need volume fraction only if particular material is selected
      // 2d zoom
      maps.m_rz_vol .resize(0);
      maps.m_rz_norm.resize(0);
      // 2d full
      maps.m_full_rz_vol.resize(0);
      maps.m_full_rz_norm.resize(0);
      // 3d
      maps.m_3d_vol .resize(0);
      maps.m_3d_norm.resize(0);
    }

    // then resize to proper size and initialize with 0's 
    // all maps are needed for the merge - so have to do resize
    // for all first ...

    maps.m_rz_tid_time      .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogT,0.0);
    maps.m_full_rz_tid_time .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogT,0.0);

    maps.m_rz_element       .resize(m_config.nBinsz*m_config.nBinsr*(m_config.elemZMax-m_config.elemZMin+1),0.0);
    maps.m_full_rz_element  .resize(m_config.nBinsz*m_config.nBinsr*(m_config.elemZMax-m_config.elemZMin+1),0.0);

    if (!m_config.material.empty()) {
      // need volume fraction only if particular material is selected
      // 2d zoom
      maps.m_rz_vol .resize(m_config.nBinsz*m_config.nBinsr,0.0);
      maps.m_rz_norm.resize(m_config.nBinsz*m_config.nBinsr,0.0);
      // 2d full
      maps.m_full_rz_vol .resize(m_config.nBinsz*m_config.nBinsr,0.0);
      maps.m_full_rz_norm.resize(m_config.nBinsz*m_config.nBinsr,0.0);
      // 3d
      maps.m_3d_vol .resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
      maps.m_3d_norm.resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    }

    // merge radiation map vectors from threads
    // Accumulate the results across threads
    m_actions.accumulate(maps, &RadiationMapsMaker::getReport,
                         &RadiationMapsMaker::Report::merge);

    TFile * f = new TFile(m_radMapsFileName.c_str(),"RECREATE");

    // histograms can be booked, filled, written and deleted in a loop
    // in order to save memory. Need vectors to loop over them ...

    std::vector<const char *> h2dNames = {
      "rz_tid","rz_eion","rz_niel","rz_h20","rz_neut","rz_chad",
      "full_rz_tid","full_rz_eion","full_rz_niel","full_rz_h20","full_rz_neut","full_rz_chad"
    };

    std::vector<const char *> h2dZTitles = {
      "TID [Gy]","E_{ion}/V [MeV/cm^{3}]","NIEL [n_{eq}/cm^{2}]","SEE [h_{>20 MeV}/cm^{2}]","FLUX [n_{>100 keV}/cm^{2}]","FLUX [h_{charged}/cm^{2}]",
      "TID [Gy]","E_{ion}/V [MeV/cm^{3}]","NIEL [n_{eq}/cm^{2}]","SEE [h_{>20 MeV}/cm^{2}]","FLUX [n_{>100 keV}/cm^{2}]","FLUX [h_{charged}/cm^{2}]"
    };

    std::vector<double> zMin = {
      m_config.zMinZoom,m_config.zMinZoom,m_config.zMinZoom,m_config.zMinZoom,m_config.zMinZoom,m_config.zMinZoom,
      m_config.zMinFull,m_config.zMinFull,m_config.zMinFull,m_config.zMinFull,m_config.zMinFull,m_config.zMinFull
    };
 
    std::vector<double> zMax = {
      m_config.zMaxZoom,m_config.zMaxZoom,m_config.zMaxZoom,m_config.zMaxZoom,m_config.zMaxZoom,m_config.zMaxZoom,
      m_config.zMaxFull,m_config.zMaxFull,m_config.zMaxFull,m_config.zMaxFull,m_config.zMaxFull,m_config.zMaxFull
    };
 
    std::vector<double> rMin = {
      m_config.rMinZoom,m_config.rMinZoom,m_config.rMinZoom,m_config.rMinZoom,m_config.rMinZoom,m_config.rMinZoom,
      m_config.rMinFull,m_config.rMinFull,m_config.rMinFull,m_config.rMinFull,m_config.rMinFull,m_config.rMinFull
    };
 
    std::vector<double> rMax = {
      m_config.rMaxZoom,m_config.rMaxZoom,m_config.rMaxZoom,m_config.rMaxZoom,m_config.rMaxZoom,m_config.rMaxZoom,
      m_config.rMaxFull,m_config.rMaxFull,m_config.rMaxFull,m_config.rMaxFull,m_config.rMaxFull,m_config.rMaxFull
    };

    const char * xtitle =  (m_config.zMinFull<0?"z [cm]":"|z| [cm]");

    TH2D * h_rz_vol  = 0;
    TH2D * h_rz_norm = 0;
    TH2D * h_full_rz_vol  = 0;
    TH2D * h_full_rz_norm = 0;

    for(unsigned int hi=0;hi<h2dNames.size();hi++) {
      TH2D *h2d = new TH2D(h2dNames[hi],h2dNames[hi],m_config.nBinsz,zMin[hi],zMax[hi],m_config.nBinsr,rMin[hi],rMax[hi]);
      h2d->SetXTitle(xtitle);
      h2d->SetYTitle("r [cm]");
      h2d->SetZTitle(h2dZTitles[hi]);
      if (hi==0 && !m_config.material.empty()) {
	// need volume fraction only if particular material is selected
	//
	// the maps for TID, NIEL and SEE need to be divided by the ratio of (vol/norm) in order to get
	// the proper estimate per volume bin for the selected material. 
	// This is *not* done in the tool directly and left to the user after having summed the histograms
	// from many individual jobs.
	// 
	h_rz_vol  = new TH2D("rz_vol" ,"rz_vol" ,m_config.nBinsz,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr,m_config.rMinZoom,m_config.rMaxZoom);
	h_rz_norm = new TH2D("rz_norm","rz_norm",m_config.nBinsz,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr,m_config.rMinZoom,m_config.rMaxZoom);
	h_full_rz_vol  = new TH2D("full_rz_vol" ,"full_rz_vol" ,m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull);
	h_full_rz_norm = new TH2D("full_rz_norm","full_rz_norm",m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull);
	
	h_rz_vol  ->SetXTitle(xtitle);
	h_rz_norm ->SetXTitle(xtitle);
	h_rz_vol  ->SetYTitle("r [cm]");
	h_rz_norm ->SetYTitle("r [cm]");
	std::string hname("Volume fraction of ");
	hname += m_config.material;
	h_rz_vol  ->SetZTitle(hname.data());
	h_rz_norm ->SetZTitle("Volume norm");

	h_full_rz_vol  ->SetXTitle(xtitle);
	h_full_rz_norm ->SetXTitle(xtitle);
	h_full_rz_vol  ->SetYTitle("r [cm]");
	h_full_rz_norm ->SetYTitle("r [cm]");
	h_full_rz_vol  ->SetZTitle(hname.data());
	h_full_rz_norm ->SetZTitle("Volume norm");
      }
      
      // normalize to volume element per bin
      for(int i=0;i<h2d->GetNbinsX();i++) { 
	for(int j=0;j<h2d->GetNbinsY();j++) { 
	  int iBin = h2d->GetBin(i+1,j+1); 
	  int vBin = m_config.nBinsr*i+j;
	  double r0=h2d->GetYaxis()->GetBinLowEdge(j+1);
	  double r1=h2d->GetYaxis()->GetBinUpEdge(j+1);
	  double z0=h2d->GetXaxis()->GetBinLowEdge(i+1);
	  double z1=h2d->GetXaxis()->GetBinUpEdge(i+1); 
	  double vol=(z1-z0)*M_PI*(r1*r1-r0*r0);
	  // if |z| instead of z double the volume
	  if ( m_config.zMinFull >= 0 ) vol *= 2; 
	  // if positive y hemisphere is used only -- half the volume
	  if ( m_config.posYOnly ) vol *= 0.5; 
	  double val = (*(pVal[hi]))[vBin];
	  h2d->SetBinContent(iBin,val/vol);
	  if (hi ==0 && !m_config.material.empty()) {
	    // need volume fraction only if particular material is selected
	    // VOL
	    val =maps.m_rz_vol[vBin];
	    h_rz_vol->SetBinContent(iBin,val/vol);
	    // NORM
	    val =maps.m_rz_norm[vBin];
	    h_rz_norm->SetBinContent(iBin,val/vol);
	  }
	  if (hi ==h2dNames.size()/2 && !m_config.material.empty()) {
	    // need volume fraction only if particular material is selected
	    // VOL
	    val =maps.m_full_rz_vol[vBin];
	    h_full_rz_vol->SetBinContent(iBin,val/vol);
	    // NORM
	    val =maps.m_full_rz_norm[vBin];
	    h_full_rz_norm->SetBinContent(iBin,val/vol);
	  }
	}
      }
      h2d->Write();
      h2d->Delete();
      if (hi ==h2dNames.size()/2 && !m_config.material.empty()) {
	h_rz_vol->Write();
	h_rz_vol->Delete();
	h_rz_norm->Write();
	h_rz_norm->Delete();
	h_full_rz_vol->Write();
	h_full_rz_vol->Delete();
	h_full_rz_norm->Write();
	h_full_rz_norm->Delete();
      }
    }

    std::vector<const char *> h3dNames = {
      "h3d_tid","h3d_eion","h3d_niel","h3d_h20","h3d_neut","h3d_chad"
    };

    std::vector<const char *> h3dTitles = {
      "TID [Gy]","E_{ion}/V [MeV/cm^{3}]","NIEL [n_{eq}/cm^{2}]","SEE [h_{>20 MeV}/cm^{2}]","FLUX [n_{>100 keV}/cm^{2}]","FLUX [h_{charged}/cm^{2}]"
    };

    TH3D * h_3d_vol  = 0;
    TH3D * h_3d_norm = 0;

    for(unsigned int hi=0;hi<h3dNames.size();hi++) {
      TH3D *h3d = new TH3D(h3dNames[hi],h3dNames[hi],m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);
      h3d->SetXTitle(xtitle);
      h3d->SetYTitle("r [cm]");
      h3d->SetZTitle("#phi [#circ]");
      h3d->SetTitle(h3dTitles[hi]);
      if (hi == 0 && !m_config.material.empty()) {
	h_3d_vol  = new TH3D("h3d_vol" ,"h3d_vol" ,m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);
	h_3d_norm = new TH3D("h3d_norm","h3d_norm",m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);
	h_3d_vol  ->SetXTitle(xtitle);
	h_3d_norm ->SetXTitle(xtitle);
	h_3d_vol  ->SetYTitle("r [cm]");
	h_3d_norm ->SetYTitle("r [cm]");
	h_3d_vol  ->SetZTitle("#phi [#circ]");
	h_3d_norm ->SetZTitle("#phi [#circ]");
	std::string hname("Volume fraction of ");
	hname += m_config.material;
	h_3d_vol  ->SetTitle(hname.data());
	h_3d_norm ->SetTitle("Volume norm");
      }
      // normalize to volume element per bin
      for(int i=0;i<h3d->GetNbinsX();i++) { 
	for(int j=0;j<h3d->GetNbinsY();j++) { 
	  for(int k=0;k<h3d->GetNbinsZ();k++) { 
	    int vBin = m_config.nBinsr3d*m_config.nBinsphi3d*i+m_config.nBinsphi3d*j+k;
	    int iBin = h3d->GetBin(i+1,j+1,k+1); 
	    double phi0=h3d->GetZaxis()->GetBinLowEdge(k+1);
	    double phi1=h3d->GetZaxis()->GetBinUpEdge(k+1);
	    double r0=h3d->GetYaxis()->GetBinLowEdge(j+1);
	    double r1=h3d->GetYaxis()->GetBinUpEdge(j+1);
	    double z0=h3d->GetXaxis()->GetBinLowEdge(i+1);
	    double z1=h3d->GetXaxis()->GetBinUpEdge(i+1); 
	    double vol=(z1-z0)*M_PI*(r1*r1-r0*r0)*(phi1-phi0)/360.; 
	    // if |z| instead of z double the volume
	    if ( m_config.zMinFull >= 0 ) vol *= 2; 
	    // assume that phi-range corresponds to full 360 degrees in case 
	    // lower phi boundary is 0 - i.e. all phi-segments mapped to first
	    if ( m_config.phiMinZoom == 0 ) {
	      vol *= 360./m_config.phiMaxZoom;
	      // if positive y hemisphere is used only -- half the volume
	      if ( m_config.posYOnly ) 
		vol *= 0.5; 
	    }
	    double val = (*(pVal3d[hi]))[vBin];
	    h3d->SetBinContent(iBin,val/vol);
	    if (hi == 0 && !m_config.material.empty()) {
	      // VOL
	      val =maps.m_3d_vol[vBin];
	      h_3d_vol->SetBinContent(iBin,val/vol);
	      // NORM
	      val =maps.m_3d_norm[vBin];
	      h_3d_norm->SetBinContent(iBin,val/vol);
	    }
	  }
	}
      }
      h3d->Write();
      h3d->Delete();
      if (hi == 0 && !m_config.material.empty()) {
	h_3d_vol->Write();
	h_3d_vol->Delete();
	h_3d_norm->Write();
	h_3d_norm->Delete();
      }
    }

    // spectra
    
    std::vector<const char *> hSpecNames = {
      "rz_neut_spec","rz_gamm_spec","rz_elec_spec","rz_muon_spec","rz_pion_spec","rz_prot_spec","rz_rest_spec",
      "full_rz_neut_spec","full_rz_gamm_spec","full_rz_elec_spec","full_rz_muon_spec","full_rz_pion_spec","full_rz_prot_spec","full_rz_rest_spec"
    };

    std::vector<const char *> hSpecTitles = {
      "FLUX [n(log_{10}(E/MeV))/cm^{2}]","FLUX [#gamma(log_{10}(E/MeV))/cm^{2}]","FLUX [e^{#pm}(log_{10}(E/MeV))/cm^{2}]","FLUX [#mu^{#pm}(log_{10}(E/MeV))/cm^{2}]","FLUX [#pi^{#pm}(log_{10}(E/MeV))/cm^{2}]","FLUX [p(log_{10}(E/MeV))/cm^{2}]","FLUX [rest(log_{10}(E/MeV))/cm^{2}]",
      "FLUX [n(log_{10}(E/MeV))/cm^{2}]","FLUX [#gamma(log_{10}(E/MeV))/cm^{2}]","FLUX [e^{#pm}(log_{10}(E/MeV))/cm^{2}]","FLUX [#mu^{#pm}(log_{10}(E/MeV))/cm^{2}]","FLUX [#pi^{#pm}(log_{10}(E/MeV))/cm^{2}]","FLUX [p(log_{10}(E/MeV))/cm^{2}]","FLUX [rest(log_{10}(E/MeV))/cm^{2}]"
    };

    std::vector<double> zMinSpec = {
      m_config.zMinZoom,m_config.zMinZoom,m_config.zMinZoom,m_config.zMinZoom,m_config.zMinZoom,m_config.zMinZoom,m_config.zMinZoom,
      m_config.zMinFull,m_config.zMinFull,m_config.zMinFull,m_config.zMinFull,m_config.zMinFull,m_config.zMinFull,m_config.zMinFull
    };
 
    std::vector<double> zMaxSpec = {
      m_config.zMaxZoom,m_config.zMaxZoom,m_config.zMaxZoom,m_config.zMaxZoom,m_config.zMaxZoom,m_config.zMaxZoom,m_config.zMaxZoom,
      m_config.zMaxFull,m_config.zMaxFull,m_config.zMaxFull,m_config.zMaxFull,m_config.zMaxFull,m_config.zMaxFull,m_config.zMaxFull
    };
 
    std::vector<double> rMinSpec = {
      m_config.rMinZoom,m_config.rMinZoom,m_config.rMinZoom,m_config.rMinZoom,m_config.rMinZoom,m_config.rMinZoom,m_config.rMinZoom,
      m_config.rMinFull,m_config.rMinFull,m_config.rMinFull,m_config.rMinFull,m_config.rMinFull,m_config.rMinFull,m_config.rMinFull
    };
 
    std::vector<double> rMaxSpec = {
      m_config.rMaxZoom,m_config.rMaxZoom,m_config.rMaxZoom,m_config.rMaxZoom,m_config.rMaxZoom,m_config.rMaxZoom,m_config.rMaxZoom,
      m_config.rMaxFull,m_config.rMaxFull,m_config.rMaxFull,m_config.rMaxFull,m_config.rMaxFull,m_config.rMaxFull,m_config.rMaxFull
    };

    std::vector<double> logEMin = {
      m_config.logEMinn,m_config.logEMino,m_config.logEMino,m_config.logEMino,m_config.logEMino,m_config.logEMino,m_config.logEMino,
      m_config.logEMinn,m_config.logEMino,m_config.logEMino,m_config.logEMino,m_config.logEMino,m_config.logEMino,m_config.logEMino
    };
 
    std::vector<double> logEMax = {
      m_config.logEMaxn,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,
      m_config.logEMaxn,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo
    };
 
    for(unsigned int hi=0;hi<hSpecNames.size();hi++) {
      TH3D *hSpec = new TH3D(hSpecNames[hi],hSpecNames[hi],m_config.nBinsz,zMinSpec[hi],zMaxSpec[hi],m_config.nBinsr,rMinSpec[hi],rMaxSpec[hi],nBinslogE[hi],logEMin[hi],logEMax[hi]);
      hSpec->SetXTitle(xtitle);
      hSpec->SetYTitle("r [cm]");
      hSpec->SetZTitle("log_{10}(E/MeV)");
      hSpec->SetTitle(hSpecTitles[hi]);
      // normalize to volume element per bin
      for(int i=0;i<hSpec->GetNbinsX();i++) { 
	for(int j=0;j<hSpec->GetNbinsY();j++) {
	  double r0=hSpec->GetYaxis()->GetBinLowEdge(j+1);
	  double r1=hSpec->GetYaxis()->GetBinUpEdge(j+1);
	  double z0=hSpec->GetXaxis()->GetBinLowEdge(i+1);
	  double z1=hSpec->GetXaxis()->GetBinUpEdge(i+1); 
	  double vol=(z1-z0)*M_PI*(r1*r1-r0*r0);
	  // if |z| instead of z double the volume
	  if ( m_config.zMinFull >= 0 ) vol *= 2; 
	  // if positive y hemisphere is used only -- half the volume
	  if ( m_config.posYOnly ) vol *= 0.5; 
	  double val;
	  // Energy Spectra
	  for(int k=0;k<hSpec->GetNbinsZ();k++) { 
	    int kBin = hSpec->GetBin(i+1,j+1,k+1); 
	    int vBinE = m_config.nBinsr*nBinslogE[hi]*i+j*nBinslogE[hi]+k;
	    val = (*(pValSpec[hi]))[vBinE];
	    hSpec->SetBinContent(kBin,val/vol);
	  }
	}
      }
      hSpec->Write();
      hSpec->Delete();
    }
    
    std::vector<const char *> hThetaSpecNames = {
      "theta_dphi_full_rz_neut_spec","theta_dphi_full_rz_gamm_spec","theta_dphi_full_rz_elec_spec","theta_dphi_full_rz_muon_spec","theta_dphi_full_rz_pion_spec","theta_dphi_full_rz_prot_spec","theta_dphi_full_rz_rchgd_spec","theta_dphi_full_rz_rneut_spec"
    };

    std::vector<const char *> hThetaSpecTitles = {
      "FLUX [n(log_{10}(E/MeV))/cm^{2}]","FLUX [#gamma(log_{10}(E/MeV))/cm^{2}]","FLUX [e^{#pm}(log_{10}(E/MeV))/cm^{2}]","FLUX [#mu^{#pm}(log_{10}(E/MeV))/cm^{2}]","FLUX [#pi^{#pm}(log_{10}(E/MeV))/cm^{2}]","FLUX [p(log_{10}(E/MeV))/cm^{2}]","FLUX [rest^{chgd}(log_{10}(E/MeV))/cm^{2}]","FLUX [rest^{neut}(log_{10}(E/MeV))/cm^{2}]"
    };

    std::vector<double> thetalogEMin = {
      m_config.logEMinn,m_config.logEMino,m_config.logEMino,m_config.logEMino,m_config.logEMino,m_config.logEMino,m_config.logEMino,m_config.logEMino
    };
 
    std::vector<double> thetalogEMax = {
      m_config.logEMaxn,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo,m_config.logEMaxo
    };
 
    for(unsigned int hi=0;hi<hThetaSpecNames.size();hi++) {
      for(int i=0;i<m_config.nBinstheta;i++) {
	for(int j=0;j<m_config.nBinsdphi;j++) {
	  TString hname(hThetaSpecNames[hi]);
	  hname += "_";
	  hname += (int)(m_config.thetaMin +   i  *(m_config.thetaMax-m_config.thetaMin)/m_config.nBinstheta);
	  hname += "_";
	  hname += (int)(m_config.thetaMin + (i+1)*(m_config.thetaMax-m_config.thetaMin)/m_config.nBinstheta);
	  hname += "_";
	  hname += (int)(j*360./m_config.nBinsdphi);
	  hname += "_";
	  hname += (int)((j+1)*360./m_config.nBinsdphi);
	  TH3D * hThetaSpec = new TH3D(hname.Data(),hname.Data(),m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull,nBinsThetalogE[hi],thetalogEMin[hi],thetalogEMax[hi]);
	  
	  hThetaSpec->SetXTitle(xtitle);
	  hThetaSpec->SetYTitle("r [cm]");
	  hThetaSpec->SetZTitle("log_{10}(E/MeV)");
	  hname = TString(hThetaSpecTitles[hi]);
	  hname += " Theta: ";
	  hname += (int)(m_config.thetaMin +   i  *(m_config.thetaMax-m_config.thetaMin)/m_config.nBinstheta);
	  hname += " - ";
	  hname += (int)(m_config.thetaMin + (i+1)*(m_config.thetaMax-m_config.thetaMin)/m_config.nBinstheta);
	  hname += " DPhi: ";
	  hname += (int)(j*360./m_config.nBinsdphi);
	  hname += " - ";
	  hname += (int)((j+1)*360./m_config.nBinsdphi);
	  hThetaSpec->SetTitle(hname.Data());
	  // normalize to volume element per bin
	  for(int k=0;k<hThetaSpec->GetNbinsX();k++) { 
	    for(int l=0;l<hThetaSpec->GetNbinsY();l++) {
	      double r0=hThetaSpec->GetYaxis()->GetBinLowEdge(l+1);
	      double r1=hThetaSpec->GetYaxis()->GetBinUpEdge(l+1);
	      double z0=hThetaSpec->GetXaxis()->GetBinLowEdge(k+1);
	      double z1=hThetaSpec->GetXaxis()->GetBinUpEdge(k+1); 
	      double vol=(z1-z0)*M_PI*(r1*r1-r0*r0);
	      // if |z| instead of z double the volume
	      if ( m_config.zMinFull >= 0 ) vol *= 2; 
	      // if positive y hemisphere is used only -- half the volume
	      if ( m_config.posYOnly ) vol *= 0.5; 
	      double val;
	      // Energy Spectra
	      for(int m=0;m<hThetaSpec->GetNbinsZ();m++) { 
		int mBin = hThetaSpec->GetBin(k+1,l+1,m+1); 
		int vBinETh = m_config.nBinsr*nBinsThetalogE[hi]*m_config.nBinsdphi*m_config.nBinstheta*k+l*nBinsThetalogE[hi]*m_config.nBinsdphi*m_config.nBinstheta+m*m_config.nBinsdphi*m_config.nBinstheta+i*m_config.nBinsdphi+j;
		val = (*(pValThetaSpec[hi]))[vBinETh];
		hThetaSpec->SetBinContent(mBin,val/vol);
	      }
	    }
	  }
	  hThetaSpec->Write();
	  hThetaSpec->Delete();
	}
      }
    }
    
    // time dependent TID maps zoom
    TH3D * h_rz_tid_time       = new TH3D("rz_tid_time" ,"rz_tid_time"           ,m_config.nBinsz,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinslogT,m_config.logTMin,m_config.logTMax);
    h_rz_tid_time     ->SetXTitle(xtitle);
    h_rz_tid_time     ->SetYTitle("r [cm]");
    h_rz_tid_time     ->SetZTitle("log_{10}(t_{cut}/s)");

    // element mass fraction maps zoom
    TH3D * h_rz_element       = new TH3D("rz_element" ,"rz_element"           ,m_config.nBinsz,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr,m_config.rMinZoom,m_config.rMaxZoom,m_config.elemZMax-m_config.elemZMin+1,m_config.elemZMin-0.5,m_config.elemZMax+0.5);
    h_rz_element     ->SetXTitle(xtitle);
    h_rz_element     ->SetYTitle("r [cm]");
    h_rz_element     ->SetZTitle("Z");

    // normalize to volume element per bin
    for(int i=0;i<h_rz_tid_time->GetNbinsX();i++) { 
      for(int j=0;j<h_rz_tid_time->GetNbinsY();j++) { 
	double r0=h_rz_tid_time->GetYaxis()->GetBinLowEdge(j+1);
	double r1=h_rz_tid_time->GetYaxis()->GetBinUpEdge(j+1);
	double z0=h_rz_tid_time->GetXaxis()->GetBinLowEdge(i+1);
	double z1=h_rz_tid_time->GetXaxis()->GetBinUpEdge(i+1); 
	double vol=(z1-z0)*M_PI*(r1*r1-r0*r0);
	// if |z| instead of z double the volume
	if ( m_config.zMinFull >= 0 ) vol *= 2; 
	// if positive y hemisphere is used only -- half the volume
	if ( m_config.posYOnly ) vol *= 0.5; 
	double val;
	// Time dependent TID maps
	for(int k=0;k<h_rz_tid_time->GetNbinsZ();k++) { 
	  int kBin = h_rz_tid_time->GetBin(i+1,j+1,k+1); 
	  int vBinT = m_config.nBinsr*m_config.nBinslogT*i+j*m_config.nBinslogT+k;
	  val =maps.m_rz_tid_time[vBinT];
	  h_rz_tid_time->SetBinContent(kBin,val/vol);
	}
	// Element mass fraction maps
	for(int k=0;k<h_rz_element->GetNbinsZ();k++) { 
	  int kBin = h_rz_element->GetBin(i+1,j+1,k+1); 
	  int vBinElem = m_config.nBinsr*(m_config.elemZMax-m_config.elemZMin+1)*i+j*(m_config.elemZMax-m_config.elemZMin+1)+k;
	  val =maps.m_rz_element[vBinElem];
	  h_rz_element->SetBinContent(kBin,val/vol);
	}
      }
    }

    h_rz_tid_time->Write();
    h_rz_tid_time->Delete();
    h_rz_element->Write();
    h_rz_element->Delete();
    
    // time dependent TID maps full
    TH3D * h_full_rz_tid_time  = new TH3D("full_rz_tid_time" ,"full_rz_tid_time" ,m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull,m_config.nBinslogT,m_config.logTMin,m_config.logTMax);
    h_full_rz_tid_time->SetXTitle(xtitle);
    h_full_rz_tid_time->SetYTitle("r [cm]");
    h_full_rz_tid_time->SetZTitle("log_{10}(t_{cut}/s)");

    // element mass fraction maps full
    TH3D * h_full_rz_element  = new TH3D("full_rz_element" ,"full_rz_element" ,m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull,m_config.elemZMax-m_config.elemZMin+1,m_config.elemZMin-0.5,m_config.elemZMax+0.5);
    h_full_rz_element->SetXTitle(xtitle);
    h_full_rz_element->SetYTitle("r [cm]");
    h_full_rz_element->SetZTitle("Z");

    // normalize to volume element per bin
    for(int i=0;i<h_full_rz_tid_time->GetNbinsX();i++) { 
      for(int j=0;j<h_full_rz_tid_time->GetNbinsY();j++) { 
	double r0=h_full_rz_tid_time->GetYaxis()->GetBinLowEdge(j+1);
	double r1=h_full_rz_tid_time->GetYaxis()->GetBinUpEdge(j+1);
	double z0=h_full_rz_tid_time->GetXaxis()->GetBinLowEdge(i+1);
	double z1=h_full_rz_tid_time->GetXaxis()->GetBinUpEdge(i+1); 
	double vol=(z1-z0)*M_PI*(r1*r1-r0*r0); 
	// if |z| instead of z double the volume
	if ( m_config.zMinFull >= 0 ) vol *= 2; 
	// if positive y hemisphere is used only -- half the volume
	if ( m_config.posYOnly ) vol *= 0.5; 
	double val;
	// Time dependent TID maps
	for(int k=0;k<h_full_rz_tid_time->GetNbinsZ();k++) { 
	  int kBin = h_full_rz_tid_time->GetBin(i+1,j+1,k+1); 
	  int vBinT = m_config.nBinsr*m_config.nBinslogT*i+j*m_config.nBinslogT+k;
	  val =maps.m_full_rz_tid_time[vBinT];
	  h_full_rz_tid_time->SetBinContent(kBin,val/vol);
	}
	// Element mass fraction maps
	for(int k=0;k<h_full_rz_element->GetNbinsZ();k++) { 
	  int kBin = h_full_rz_element->GetBin(i+1,j+1,k+1); 
	  int vBinElem = m_config.nBinsr*(m_config.elemZMax-m_config.elemZMin+1)*i+j*(m_config.elemZMax-m_config.elemZMin+1)+k;
	  val =maps.m_full_rz_element[vBinElem];
	  h_full_rz_element->SetBinContent(kBin,val/vol);
	}
      }
    }
    h_full_rz_tid_time->Write();
    h_full_rz_tid_time->Delete();
    h_full_rz_element->Write();
    h_full_rz_element->Delete();

    f->Close();

    return StatusCode::SUCCESS;
  }

  std::unique_ptr<RadiationMapsMaker>  RadiationMapsMakerTool::makeAndFillAction(G4AtlasUserActions& actionList){
    ATH_MSG_INFO("Making a RadiationMapsMaker action");
    auto action = std::make_unique<RadiationMapsMaker>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA 
