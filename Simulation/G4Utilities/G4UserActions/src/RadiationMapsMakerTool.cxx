/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TH2D.h"
#include "TH3D.h"
#include "TFile.h"
#include "G4UserActions/RadiationMapsMakerTool.h"

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
    declareProperty("Material"  , m_config.material);
    /// map granularities 
    /// number of bins in r and z for all 2D maps
    declareProperty("NBinsR"    , m_config.nBinsr);
    declareProperty("NBinsZ"    , m_config.nBinsz);
    /// number of bins in r, z and phi for all 3D maps
    declareProperty("NBinsR3D"  , m_config.nBinsr3d);
    declareProperty("NBinsZ3D"  , m_config.nBinsz3d);
    declareProperty("NBinsPhi3D", m_config.nBinsphi3d);
    /// map ranges
    /// for Zoomed area in 2D and 3D
    declareProperty("RMinZoom"  , m_config.rMinZoom);
    declareProperty("RMaxZoom"  , m_config.rMaxZoom);
    declareProperty("ZMinZoom"  , m_config.zMinZoom);
    declareProperty("ZMaxZoom"  , m_config.zMaxZoom);
    /// for Full detector in 2D
    declareProperty("RMinFull"  , m_config.rMinFull);
    declareProperty("RMaxFull"  , m_config.rMaxFull);
    declareProperty("ZMinFull"  , m_config.zMinFull);
    declareProperty("ZMaxFull"  , m_config.zMaxFull);
    /// for Zoomed area in 3D 
    declareProperty("PhiMinZoom", m_config.phiMinZoom);
    declareProperty("PhiMaxZoom", m_config.phiMaxZoom);
  }

  //---------------------------------------------------------------------------
  // Initialize Configurable Properties
  //---------------------------------------------------------------------------
  StatusCode RadiationMapsMakerTool::initialize()
  {
    ATH_MSG_INFO( "Initializing  " << name() << "\n" <<
                  "OutputFile:   " << m_radMapsFileName   << "\n"                << 
                  "Material:     " << m_config.material   << "\n"                << 
                  "2D Maps:      " << m_config.nBinsz     << " |z|-bins, "       << 
                                      m_config.nBinsr     << " r-bins"           << "\n"                << 
                  "Zoom:         " << m_config.zMinZoom   << " < |z|/cm < "      << m_config.zMaxZoom   << ", " << 
                                      m_config.rMinZoom   << " < r/cm < "        << m_config.rMaxZoom   << "\n" << 
                  "Full:         " << m_config.zMinFull   << " < |z|/cm < "      << m_config.zMaxFull   << ", " << 
                                      m_config.rMinFull   << " < r/cm < "        << m_config.rMaxFull   << "\n" << 
                  "3D Maps:      " << m_config.nBinsz3d   << " |z|-bins, "       << 
                                      m_config.nBinsr3d   << " r-bins, "         << 
                                      m_config.nBinsphi3d << " phi-bins"         << "\n"                << 
                  "Zoom:         " << m_config.zMinZoom   << " < |z|/cm < "      << m_config.zMaxZoom   << ", " << 
                                      m_config.rMinZoom   << " < r/cm < "        << m_config.rMaxZoom   << ", " <<
                                      m_config.phiMinZoom << " < phi/degrees < " << m_config.phiMaxZoom );
      
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Merge results from all threads
  //---------------------------------------------------------------------------
  StatusCode RadiationMapsMakerTool::finalize()
  {
    ATH_MSG_DEBUG( "Finalizing " << name() );

    // first make sure the vectors are empty

    RadiationMapsMaker::Report maps;

    maps.m_rz_tid .resize(0);
    maps.m_rz_eion.resize(0);
    maps.m_rz_niel.resize(0);
    maps.m_rz_h20 .resize(0);
    maps.m_rz_neut.resize(0);
    maps.m_rz_chad.resize(0);

    maps.m_full_rz_tid .resize(0);
    maps.m_full_rz_eion.resize(0);
    maps.m_full_rz_niel.resize(0);
    maps.m_full_rz_h20 .resize(0);
    maps.m_full_rz_neut.resize(0);
    maps.m_full_rz_chad.resize(0);

    maps.m_3d_tid .resize(0);
    maps.m_3d_eion.resize(0);
    maps.m_3d_niel.resize(0);
    maps.m_3d_h20 .resize(0);
    maps.m_3d_neut.resize(0);
    maps.m_3d_chad.resize(0);

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

    maps.m_rz_tid .resize(m_config.nBinsz*m_config.nBinsr,0.0);
    maps.m_rz_eion.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    maps.m_rz_niel.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    maps.m_rz_h20 .resize(m_config.nBinsz*m_config.nBinsr,0.0);
    maps.m_rz_neut.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    maps.m_rz_chad.resize(m_config.nBinsz*m_config.nBinsr,0.0);

    maps.m_full_rz_tid .resize(m_config.nBinsz*m_config.nBinsr,0.0);
    maps.m_full_rz_eion.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    maps.m_full_rz_niel.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    maps.m_full_rz_h20 .resize(m_config.nBinsz*m_config.nBinsr,0.0);
    maps.m_full_rz_neut.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    maps.m_full_rz_chad.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    
    maps.m_3d_tid .resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    maps.m_3d_eion.resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    maps.m_3d_niel.resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    maps.m_3d_h20 .resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    maps.m_3d_neut.resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    maps.m_3d_chad.resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);

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

    TH2D * h_rz_tid  = new TH2D("rz_tid" ,"rz_tid" ,m_config.nBinsz,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr,m_config.rMinZoom,m_config.rMaxZoom);
    TH2D * h_rz_eion = new TH2D("rz_eion","rz_eion",m_config.nBinsz,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr,m_config.rMinZoom,m_config.rMaxZoom);
    TH2D * h_rz_niel = new TH2D("rz_niel","rz_niel",m_config.nBinsz,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr,m_config.rMinZoom,m_config.rMaxZoom);
    TH2D * h_rz_h20  = new TH2D("rz_h20" ,"rz_h20" ,m_config.nBinsz,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr,m_config.rMinZoom,m_config.rMaxZoom);
    TH2D * h_rz_neut = new TH2D("rz_neut","rz_neut",m_config.nBinsz,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr,m_config.rMinZoom,m_config.rMaxZoom);
    TH2D * h_rz_chad = new TH2D("rz_chad","rz_chad",m_config.nBinsz,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr,m_config.rMinZoom,m_config.rMaxZoom);

    h_rz_tid  ->SetXTitle("|z| [cm]");
    h_rz_eion ->SetXTitle("|z| [cm]");
    h_rz_niel ->SetXTitle("|z| [cm]");
    h_rz_h20  ->SetXTitle("|z| [cm]");
    h_rz_neut ->SetXTitle("|z| [cm]");
    h_rz_chad ->SetXTitle("|z| [cm]");

    h_rz_tid  ->SetYTitle("r [cm]");
    h_rz_eion ->SetYTitle("r [cm]");
    h_rz_niel ->SetYTitle("r [cm]");
    h_rz_h20  ->SetYTitle("r [cm]");
    h_rz_neut ->SetYTitle("r [cm]");
    h_rz_chad ->SetYTitle("r [cm]");

    h_rz_tid  ->SetZTitle("TID [Gy]");
    h_rz_eion ->SetZTitle("E_{ion}/V [MeV/cm^{3}]");
    h_rz_niel ->SetZTitle("NIEL [n_{eq}/cm^{2}]");
    h_rz_h20  ->SetZTitle("SEE [h_{>20 MeV}/cm^{2}]");
    h_rz_neut ->SetZTitle("FLUX [n_{>100 keV}/cm^{2}]");
    h_rz_chad ->SetZTitle("FLUX [h_{charged}/cm^{2}]");

    TH2D *h_full_rz_tid  = new TH2D("full_rz_tid" ,"full_rz_tid" ,m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull);
    TH2D *h_full_rz_eion = new TH2D("full_rz_eion","full_rz_eion",m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull);
    TH2D *h_full_rz_niel = new TH2D("full_rz_niel","full_rz_niel",m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull);
    TH2D *h_full_rz_h20  = new TH2D("full_rz_h20" ,"full_rz_h20" ,m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull);
    TH2D *h_full_rz_neut = new TH2D("full_rz_neut","full_rz_neut",m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull);
    TH2D *h_full_rz_chad = new TH2D("full_rz_chad","full_rz_chad",m_config.nBinsz,m_config.zMinFull,m_config.zMaxFull,m_config.nBinsr,m_config.rMinFull,m_config.rMaxFull);

    h_full_rz_tid  ->SetXTitle("|z| [cm]");
    h_full_rz_eion ->SetXTitle("|z| [cm]");
    h_full_rz_niel ->SetXTitle("|z| [cm]");
    h_full_rz_h20  ->SetXTitle("|z| [cm]");
    h_full_rz_neut ->SetXTitle("|z| [cm]");
    h_full_rz_chad ->SetXTitle("|z| [cm]");

    h_full_rz_tid  ->SetYTitle("r [cm]");
    h_full_rz_eion ->SetYTitle("r [cm]");
    h_full_rz_niel ->SetYTitle("r [cm]");
    h_full_rz_h20  ->SetYTitle("r [cm]");
    h_full_rz_neut ->SetYTitle("r [cm]");
    h_full_rz_chad ->SetYTitle("r [cm]");

    h_full_rz_tid  ->SetZTitle("TID [Gy]");
    h_full_rz_eion ->SetZTitle("E_{ion}/V [MeV/cm^{3}]");
    h_full_rz_niel ->SetZTitle("NIEL [n_{eq}/cm^{2}]");
    h_full_rz_h20  ->SetZTitle("SEE [h_{>20 MeV}/cm^{2}]");
    h_full_rz_neut ->SetZTitle("FLUX [n_{>100 keV}/cm^{2}]");
    h_full_rz_chad ->SetZTitle("FLUX [h_{charged}/cm^{2}]");

    TH3D * h_3d_tid  = new TH3D("h3d_tid" ,"h3d_tid" ,m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);
    TH3D * h_3d_eion = new TH3D("h3d_eion","h3d_eion",m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);
    TH3D * h_3d_niel = new TH3D("h3d_niel","h3d_niel",m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);
    TH3D * h_3d_h20  = new TH3D("h3d_h20" ,"h3d_h20" ,m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);
    TH3D * h_3d_neut = new TH3D("h3d_neut","h3d_neut",m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);
    TH3D * h_3d_chad = new TH3D("h3d_chad","h3d_chad",m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);

    h_3d_tid  ->SetXTitle("|z| [cm]");
    h_3d_eion ->SetXTitle("|z| [cm]");
    h_3d_niel ->SetXTitle("|z| [cm]");
    h_3d_h20  ->SetXTitle("|z| [cm]");
    h_3d_neut ->SetXTitle("|z| [cm]");
    h_3d_chad ->SetXTitle("|z| [cm]");

    h_3d_tid  ->SetYTitle("r [cm]");
    h_3d_eion ->SetYTitle("r [cm]");
    h_3d_niel ->SetYTitle("r [cm]");
    h_3d_h20  ->SetYTitle("r [cm]");
    h_3d_neut ->SetYTitle("r [cm]");
    h_3d_chad ->SetYTitle("r [cm]");

    h_3d_tid  ->SetZTitle("#phi [#circ]");
    h_3d_eion ->SetZTitle("#phi [#circ]");
    h_3d_niel ->SetZTitle("#phi [#circ]");
    h_3d_h20  ->SetZTitle("#phi [#circ]");
    h_3d_neut ->SetZTitle("#phi [#circ]");
    h_3d_chad ->SetZTitle("#phi [#circ]");

    h_3d_tid  ->SetTitle("TID [Gy]");
    h_3d_eion ->SetTitle("E_{ion}/V [MeV/cm^{3}]");
    h_3d_niel ->SetTitle("NIEL [n_{eq}/cm^{2}]");
    h_3d_h20  ->SetTitle("SEE [h_{>20 MeV}/cm^{2}]");
    h_3d_neut ->SetTitle("FLUX [n_{>100 keV}/cm^{2}]");
    h_3d_chad ->SetTitle("FLUX [h_{charged}/cm^{2}]");

    TH2D * h_rz_vol  = 0;
    TH2D * h_rz_norm = 0;
    TH2D * h_full_rz_vol  = 0;
    TH2D * h_full_rz_norm = 0;
    TH3D * h_3d_vol  = 0;
    TH3D * h_3d_norm = 0;
    if (!m_config.material.empty()) {
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
      h_3d_vol  = new TH3D("h3d_vol" ,"h3d_vol" ,m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);
      h_3d_norm = new TH3D("h3d_norm","h3d_norm",m_config.nBinsz3d,m_config.zMinZoom,m_config.zMaxZoom,m_config.nBinsr3d,m_config.rMinZoom,m_config.rMaxZoom,m_config.nBinsphi3d,m_config.phiMinZoom,m_config.phiMaxZoom);

      h_rz_vol  ->SetXTitle("|z| [cm]");
      h_rz_norm ->SetXTitle("|z| [cm]");
      h_rz_vol  ->SetYTitle("r [cm]");
      h_rz_norm ->SetYTitle("r [cm]");
      std::string hname("Volume fraction of ");
      hname += m_config.material;
      h_rz_vol  ->SetZTitle(hname.data());
      h_rz_norm ->SetZTitle("Volume norm");

      h_full_rz_vol  ->SetXTitle("|z| [cm]");
      h_full_rz_norm ->SetXTitle("|z| [cm]");
      h_full_rz_vol  ->SetYTitle("r [cm]");
      h_full_rz_norm ->SetYTitle("r [cm]");
      h_full_rz_vol  ->SetZTitle(hname.data());
      h_full_rz_norm ->SetZTitle("Volume norm");

      h_3d_vol  ->SetXTitle("|z| [cm]");
      h_3d_norm ->SetXTitle("|z| [cm]");
      h_3d_vol  ->SetYTitle("r [cm]");
      h_3d_norm ->SetYTitle("r [cm]");
      h_3d_vol  ->SetZTitle("#phi [#circ]");
      h_3d_norm ->SetZTitle("#phi [#circ]");
      h_3d_vol  ->SetTitle(hname.data());
      h_3d_norm ->SetTitle("Volume norm");
    }


    // normalize to volume element per bin
    for(int i=0;i<h_rz_tid->GetNbinsX();i++) { 
      for(int j=0;j<h_rz_tid->GetNbinsY();j++) { 
	int iBin = h_rz_tid->GetBin(i+1,j+1); 
	int vBin = m_config.nBinsr*i+j;
	double r0=h_rz_tid->GetYaxis()->GetBinLowEdge(j+1);
	double r1=h_rz_tid->GetYaxis()->GetBinUpEdge(j+1);
	double z0=h_rz_tid->GetXaxis()->GetBinLowEdge(i+1);
	double z1=h_rz_tid->GetXaxis()->GetBinUpEdge(i+1); 
	double vol=2*(z1-z0)*M_PI*(r1*r1-r0*r0); 
	double val;
	// TID
	val =maps.m_rz_tid[vBin];
	h_rz_tid->SetBinContent(iBin,val/vol);
	// EION
	val =maps.m_rz_eion[vBin];
	h_rz_eion->SetBinContent(iBin,val/vol);
	// NIEL
	val =maps.m_rz_niel[vBin];
	h_rz_niel->SetBinContent(iBin,val/vol);
	// SEE
	val =maps.m_rz_h20[vBin];
	h_rz_h20->SetBinContent(iBin,val/vol);
	// NEUT
	val =maps.m_rz_neut[vBin];
	h_rz_neut->SetBinContent(iBin,val/vol);
	// CHAD
	val =maps.m_rz_chad[vBin];
	h_rz_chad->SetBinContent(iBin,val/vol);
	if (!m_config.material.empty()) {
	  // need volume fraction only if particular material is selected
	  // VOL
	  val =maps.m_rz_vol[vBin];
	  h_rz_vol->SetBinContent(iBin,val/vol);
	  // NORM
	  val =maps.m_rz_norm[vBin];
	  h_rz_norm->SetBinContent(iBin,val/vol);
	}
      }
    }
    h_rz_tid->Write();
    h_rz_eion->Write();
    h_rz_niel->Write();
    h_rz_h20->Write();
    h_rz_neut->Write();
    h_rz_chad->Write();

    // normalize to volume element per bin
    for(int i=0;i<h_full_rz_tid->GetNbinsX();i++) { 
      for(int j=0;j<h_full_rz_tid->GetNbinsY();j++) { 
	int iBin = h_full_rz_tid->GetBin(i+1,j+1); 
	int vBin = m_config.nBinsr*i+j;
	double r0=h_full_rz_tid->GetYaxis()->GetBinLowEdge(j+1);
	double r1=h_full_rz_tid->GetYaxis()->GetBinUpEdge(j+1);
	double z0=h_full_rz_tid->GetXaxis()->GetBinLowEdge(i+1);
	double z1=h_full_rz_tid->GetXaxis()->GetBinUpEdge(i+1); 
	double vol=2*(z1-z0)*M_PI*(r1*r1-r0*r0); 
	double val;
	// TID
	val =maps.m_full_rz_tid[vBin];
	h_full_rz_tid->SetBinContent(iBin,val/vol);
	// EION
	val =maps.m_full_rz_eion[vBin];
	h_full_rz_eion->SetBinContent(iBin,val/vol);
	// NIEL
	val =maps.m_full_rz_niel[vBin];
	h_full_rz_niel->SetBinContent(iBin,val/vol);
	// SEE
	val =maps.m_full_rz_h20[vBin];
	h_full_rz_h20->SetBinContent(iBin,val/vol);
	// NEUT
	val =maps.m_full_rz_neut[vBin];
	h_full_rz_neut->SetBinContent(iBin,val/vol);
	// CHAD
	val =maps.m_full_rz_chad[vBin];
	h_full_rz_chad->SetBinContent(iBin,val/vol);
	if (!m_config.material.empty()) {
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
    h_full_rz_tid->Write();
    h_full_rz_eion->Write();
    h_full_rz_niel->Write();
    h_full_rz_h20->Write();
    h_full_rz_neut->Write();
    h_full_rz_chad->Write();

    // normalize to volume element per bin
    for(int i=0;i<h_3d_tid->GetNbinsX();i++) { /* |z| */
      for(int j=0;j<h_3d_tid->GetNbinsY();j++) { /* r */
	for(int k=0;k<h_3d_tid->GetNbinsZ();k++) { /* phi */
	  int vBin = m_config.nBinsr3d*m_config.nBinsphi3d*i+m_config.nBinsphi3d*j+k;
	  int iBin = h_3d_tid->GetBin(i+1,j+1,k+1); 
	  double phi0=h_3d_tid->GetZaxis()->GetBinLowEdge(k+1);
	  double phi1=h_3d_tid->GetZaxis()->GetBinUpEdge(k+1);
	  double r0=h_3d_tid->GetYaxis()->GetBinLowEdge(j+1);
	  double r1=h_3d_tid->GetYaxis()->GetBinUpEdge(j+1);
	  double z0=h_3d_tid->GetXaxis()->GetBinLowEdge(i+1);
	  double z1=h_3d_tid->GetXaxis()->GetBinUpEdge(i+1); 
	  double vol=2*(z1-z0)*M_PI*(r1*r1-r0*r0)*(phi1-phi0)/360.; 
	  // assume that phi-range corresponds to full 360 degrees in case 
	  // lower phi boundary is 0 - i.e. all phi-segments mapped to first
	  if ( m_config.phiMinZoom == 0 ) {
	    vol *= 360./m_config.phiMaxZoom;
	  }
	  double val;
	  // TID
	  val =maps.m_3d_tid[vBin];
	  h_3d_tid->SetBinContent(iBin,val/vol);
	  // EION
	  val =maps.m_3d_eion[vBin];
	  h_3d_eion->SetBinContent(iBin,val/vol);
	  // NIEL
	  val =maps.m_3d_niel[vBin];
	  h_3d_niel->SetBinContent(iBin,val/vol);
	  // SEE
	  val =maps.m_3d_h20[vBin];
	  h_3d_h20->SetBinContent(iBin,val/vol);
	  // NEUT
	  val =maps.m_3d_neut[vBin];
	  h_3d_neut->SetBinContent(iBin,val/vol);
	  // CHAD
	  val =maps.m_3d_chad[vBin];
	  h_3d_chad->SetBinContent(iBin,val/vol);
	  if (!m_config.material.empty()) {
	    // need volume fraction only if particular material is selected
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
    h_3d_tid->Write();
    h_3d_eion->Write();
    h_3d_niel->Write();
    h_3d_h20->Write();
    h_3d_neut->Write();
    h_3d_chad->Write();

    if (!m_config.material.empty()) {
      // need volume fraction only if particular material is selected
      h_rz_vol->Write();
      h_rz_norm->Write();
      h_full_rz_vol->Write();
      h_full_rz_norm->Write();
      h_3d_vol->Write();
      h_3d_norm->Write();
    }

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
