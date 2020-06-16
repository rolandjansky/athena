/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaMVACalib/egammaMVAFunctions.h"

#ifndef XAOD_ANALYSIS
#include "GaudiKernel/SystemOfUnits.h"
using Gaudi::Units::GeV;
#else
#define GeV 1000
#endif

/**
 * These functions are for building the maps of functions
 **/

namespace egammaMVAFunctions
{

  // forward declarations
  void initializeClusterFuncs(funcMap_t& funcLibrary, 
			      const std::string& prefix, 
			      bool useLayerCorrected);

  void initializeEgammaFuncs(funcMap_t& funcLibrary,
			     const std::string& prefix, 
			     bool useLayerCorrected);

  /// A function to build the map for electrons
  std::unique_ptr<funcMap_t> initializeElectronFuncs(bool useLayerCorrected)
  {
    auto funcLibraryPtr = std::make_unique<funcMap_t>();
    funcMap_t& funcLibrary = *funcLibraryPtr;

    initializeClusterFuncs(funcLibrary, "el", useLayerCorrected);
    initializeEgammaFuncs(funcLibrary, "el", useLayerCorrected);

    funcLibrary["el_charge"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return compute_el_charge(*(static_cast<const xAOD::Electron*>(eg))); };
    funcLibrary["el_tracketa"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return compute_el_tracketa(*(static_cast<const xAOD::Electron*>(eg))); };
    funcLibrary["el_trackpt"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return compute_el_trackpt(*(static_cast<const xAOD::Electron*>(eg))); };
    funcLibrary["el_trackz0"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return compute_el_trackz0(*(static_cast<const xAOD::Electron*>(eg))); };
    funcLibrary["el_refittedTrack_qoverp"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return compute_el_refittedTrack_qoverp(*(static_cast<const xAOD::Electron*>(eg))); };
    
    return funcLibraryPtr;
  }

  /// A function to build the map for uncoverted photons
  std::unique_ptr<funcMap_t> initializeUnconvertedPhotonFuncs(bool useLayerCorrected)
  {
    auto funcLibraryPtr = std::make_unique<funcMap_t>();
    funcMap_t& funcLibrary = *funcLibraryPtr;
 
    initializeClusterFuncs(funcLibrary, "ph", useLayerCorrected);
    initializeEgammaFuncs(funcLibrary, "ph", useLayerCorrected);

    return funcLibraryPtr;
  }


  /// A function to build the map for converted photons
  std::unique_ptr<funcMap_t> initializeConvertedPhotonFuncs(bool useLayerCorrected)
  {
    auto funcLibraryPtr = std::make_unique<funcMap_t>();
    funcMap_t& funcLibrary = *funcLibraryPtr;
    
    initializeClusterFuncs(funcLibrary, "ph", useLayerCorrected);
    initializeEgammaFuncs(funcLibrary, "ph", useLayerCorrected);
    

    funcLibrary["ph_Rconv"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)->float
      { return xAOD::EgammaHelpers::conversionRadius(static_cast<const xAOD::Photon*>(eg)); };
    
    funcLibrary["convR"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)->float
      { 
	auto ph = static_cast<const xAOD::Photon*>(eg);
	if (compute_ptconv(ph) > 3*GeV) { 
	  return xAOD::EgammaHelpers::conversionRadius(ph);
	} 
	  return 799.0;
	
      };
    funcLibrary["ph_zconv"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return static_cast<const xAOD::Photon*>(eg)->vertex()->position().z(); };
    funcLibrary["ph_pt1conv"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)->float
      { return compute_pt1conv(static_cast<const xAOD::Photon*>(eg)); };    
    funcLibrary["ph_pt2conv"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)->float
      { return compute_pt2conv(static_cast<const xAOD::Photon*>(eg)); }; 
    funcLibrary["ph_ptconv"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return compute_ptconv(static_cast<const xAOD::Photon*>(eg)); };
    
    funcLibrary["convPtRatio"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)->float
      {
	auto ph = static_cast<const xAOD::Photon*>(eg);
	if (xAOD::EgammaHelpers::numberOfSiTracks(ph) == 2) {
	  auto pt1 = compute_pt1conv(ph);
	  auto pt2 = compute_pt2conv(ph);
	  return std::max(pt1, pt2)/(pt1+pt2);
	} 
	  return 1.0f;
	
      };
    
    if (useLayerCorrected) {
      funcLibrary["convEtOverPt"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*cl)->float
	{
	  auto ph = static_cast<const xAOD::Photon*>(eg);
	  
	  float rv = 0.0;
	  if (xAOD::EgammaHelpers::numberOfSiTracks(ph) == 2) {
	    rv = std::max(0.0f, compute_correctedcl_Eacc(*cl)/(std::cosh(compute_cl_eta(*cl))*compute_ptconv(ph)));
	  } 
	  return std::min(rv, 2.0f);
	};
    } else {
      funcLibrary["convEtOverPt"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*cl)->float
	{
	  auto ph = static_cast<const xAOD::Photon*>(eg);
	  
	  float rv = 0.0;
	  if (xAOD::EgammaHelpers::numberOfSiTracks(ph) == 2) {
	    rv = std::max(0.0f, compute_rawcl_Eacc(*cl)/(std::cosh(compute_cl_eta(*cl))*compute_ptconv(ph)));
	  } 
	  return std::min(rv, 2.0f);
	};
    }
    
    return funcLibraryPtr;
  }


  // Initialize the functions that just depend on the cluster.
  // This helper function is not meant for external use.
  void initializeClusterFuncs(funcMap_t& funcLibrary, 
			      const std::string& prefix, 
			      bool useLayerCorrected)
  {

    funcLibrary[prefix + "_cl_eta"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return compute_cl_eta(*cl); };
    funcLibrary[prefix + "_cl_phi"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return compute_cl_phi(*cl); };
    funcLibrary[prefix + "_cl_E"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return compute_cl_e(*cl); };
    funcLibrary[prefix + "_cl_etaCalo"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return compute_cl_etaCalo(*cl); };
    funcLibrary[prefix + "_cl_phiCalo"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return compute_cl_phiCalo(*cl); };
    funcLibrary[prefix + "_cl_E_TileGap3"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return cl->eSample(CaloSampling::TileGap3); };
    
    funcLibrary["cellIndexCalo"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return std::floor(std::abs(compute_cl_etaCalo(*cl))/0.025); };
    funcLibrary["phiModCalo"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return ((abs(compute_cl_eta(*cl)) < 1.425) ? 
		std::fmod(compute_cl_phiCalo(*cl), TMath::Pi()/512) :
		std::fmod(compute_cl_phiCalo(*cl), TMath::Pi()/384)); 
      };
    funcLibrary["etaModCalo"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return std::fmod(std::abs(compute_cl_etaCalo(*cl)), 0.025); };
    funcLibrary["dPhiTG3"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return std::fmod(2.*TMath::Pi()+compute_cl_phi(*cl),TMath::Pi()/32.)-TMath::Pi()/64.0; };
    
    if (useLayerCorrected) {
      funcLibrary[prefix + "_rawcl_Es0"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_correctedcl_Es0(*cl); };
      funcLibrary[prefix + "_rawcl_Es1"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_correctedcl_Es1(*cl); };
      funcLibrary[prefix + "_rawcl_Es2"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_correctedcl_Es2(*cl); };
      funcLibrary[prefix + "_rawcl_Es3"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_correctedcl_Es3(*cl); };
      funcLibrary[prefix + "_rawcl_Eacc"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_correctedcl_Eacc(*cl); };
      funcLibrary[prefix + "_rawcl_f0"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_correctedcl_f0(*cl); };
      funcLibrary[prefix + "_rawcl_calibHitsShowerDepth"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_correctedcl_calibHitsShowerDepth(*cl); };
      funcLibrary["R12"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_correctedcl_Es1(*cl)/compute_correctedcl_Es2(*cl); };
      funcLibrary["fTG3"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return cl->eSample(CaloSampling::TileGap3)/compute_correctedcl_Eacc(*cl); };
    } else {
      funcLibrary[prefix + "_rawcl_Es0"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_rawcl_Es0(*cl); };
      funcLibrary[prefix + "_rawcl_Es1"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_rawcl_Es1(*cl); };
      funcLibrary[prefix + "_rawcl_Es2"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_rawcl_Es2(*cl); };
      funcLibrary[prefix + "_rawcl_Es3"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_rawcl_Es3(*cl); };
      funcLibrary[prefix + "_rawcl_Eacc"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_rawcl_Eacc(*cl); };
      funcLibrary[prefix + "_rawcl_f0"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_rawcl_f0(*cl); };
      funcLibrary[prefix + "_rawcl_calibHitsShowerDepth"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_rawcl_calibHitsShowerDepth(*cl); };
      funcLibrary["R12"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return compute_rawcl_Es1(*cl)/compute_rawcl_Es2(*cl); };
      funcLibrary["fTG3"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
	{ return cl->eSample(CaloSampling::TileGap3)/compute_rawcl_Eacc(*cl); };
    }
  }

  // Initialize the variables that just depend on egamma.
  // This helper function is not meant for external use.
  void initializeEgammaFuncs(funcMap_t& funcLibrary, 
			     const std::string& prefix, 
			     bool /* useLayerCorrected */)
  {
    funcLibrary[prefix + "_e011"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e011); };
    funcLibrary[prefix + "_e033"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e033); };
    funcLibrary[prefix + "_e132"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e132); };
    funcLibrary[prefix + "_e1152"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e1152); };
    funcLibrary[prefix + "_ethad1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::ethad1); };
    funcLibrary[prefix + "_ethad"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::ethad); };
    funcLibrary[prefix + "_f1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::f1); };
    funcLibrary[prefix + "_f3"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::f3); };
    funcLibrary[prefix + "_f1core"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::f1core); };
    funcLibrary[prefix + "_f3core"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::f3core); };
    funcLibrary[prefix + "_e233"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e233); };
    funcLibrary[prefix + "_e235"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e235); };
    funcLibrary[prefix + "_e255"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e255); };
    funcLibrary[prefix + "_e237"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e237); };
    funcLibrary[prefix + "_e277"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e277); };
    funcLibrary[prefix + "_e333"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e333); };
    funcLibrary[prefix + "_e335"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e335); };
    funcLibrary[prefix + "_e337"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e337); };
    funcLibrary[prefix + "_e377"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e377); };
    funcLibrary[prefix + "_weta1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::weta1); };
    funcLibrary[prefix + "_weta2"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::weta2); };
    funcLibrary[prefix + "_e2ts1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e2ts1); };
    funcLibrary[prefix + "_e2tsts1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::e2tsts1); };
    funcLibrary[prefix + "_fracs1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::fracs1); };
    funcLibrary[prefix + "_widths1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::widths1); };
    funcLibrary[prefix + "_widths2"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::widths2); };
    funcLibrary[prefix + "_poscs1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::poscs1); };
    funcLibrary[prefix + "_poscs2"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::poscs2); };
    funcLibrary[prefix + "_asy1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::asy1); };
    funcLibrary[prefix + "_pos"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::pos); };
    funcLibrary[prefix + "_pos7"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::pos7); };
    funcLibrary[prefix + "_barys1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::barys1); };
    funcLibrary[prefix + "_wtots1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::wtots1); };
    funcLibrary[prefix + "_emins1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::emins1); };
    funcLibrary[prefix + "_emaxs1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::emaxs1); };
    funcLibrary[prefix + "_r33over37allcalo"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::r33over37allcalo); };
    funcLibrary[prefix + "_ecore"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::ecore); };
    funcLibrary[prefix + "_Reta"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::Reta); };
    funcLibrary[prefix + "_Rphi"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::Rphi); };
    funcLibrary[prefix + "_Eratio"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::Eratio); };
    funcLibrary[prefix + "_Rhad"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::Rhad); };
    funcLibrary[prefix + "_Rhad1"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::Rhad1); };
    funcLibrary[prefix + "_DeltaE"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)
      { return eg->showerShapeValue(xAOD::EgammaParameters::DeltaE); };
    
  }
}
