 /*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVACalibTool.h"

#include "egammaMVACalib/egammaMVALayerDepth.h"
#include "egammaMVACalib/egammaMVATreeHelpers.h"
#include "PathResolver/PathResolver.h"
#include "xAODEgamma/PhotonxAODHelpers.h"

#include "TFile.h"
#include "TMath.h"

#include <cmath>


egammaMVACalibTool::egammaMVACalibTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

egammaMVACalibTool::~egammaMVACalibTool()
{
}

StatusCode egammaMVACalibTool::initialize()
{

  ATH_MSG_DEBUG("Initializing with particle " << m_particleType);

  if (m_shiftType == MEAN10TOTRUE) {
    ATH_MSG_DEBUG("Using Mean10 shift");
  } else if (m_shiftType == NOSHIFT) {
    ATH_MSG_DEBUG("Not using a shift");
  } else {
    ATH_MSG_FATAL("Unsupported shift: " << m_shiftType);
    return StatusCode::FAILURE;
  }

  /// A dictionary of all available functions
  std::unordered_map<std::string,
		     std::function<float(const xAOD::Egamma*, const xAOD::CaloCluster*)> > funcLibrary;

  
  // get the BDTs
  ATH_MSG_DEBUG("get BDTs in folder: " << m_folder);
  switch (m_particleType) {
  case xAOD::EgammaParameters::electron:
    ATH_CHECK(initializeElectronFuncs(funcLibrary));
    ATH_CHECK(setupBDT(funcLibrary, 
		       PathResolverFindCalibFile(m_folder + "/MVACalib_electron.weights.root")));
    break;
  case xAOD::EgammaParameters::unconvertedPhoton:
    ATH_CHECK(initializeUnconvertedPhotonFuncs(funcLibrary));
    ATH_CHECK(setupBDT(funcLibrary,
		       PathResolverFindCalibFile(m_folder + "/MVACalib_unconvertedPhoton.weights.root")));
    break;
  case xAOD::EgammaParameters::convertedPhoton:
    ATH_CHECK(initializeConvertedPhotonFuncs(funcLibrary));
    ATH_CHECK(setupBDT(funcLibrary,
		       PathResolverFindCalibFile(m_folder + "/MVACalib_convertedPhoton.weights.root")));
    break;
  default:
    ATH_MSG_FATAL("Particle type not set properly: " << m_particleType);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode egammaMVACalibTool::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode egammaMVACalibTool::setupBDT(const std::unordered_map<std::string,
					std::function<float(const xAOD::Egamma*, 
							    const xAOD::CaloCluster*)> >& funcLibrary,
					std::string fileName)
{

  ATH_MSG_DEBUG("Trying to open " << fileName);

  std::unique_ptr<TFile> f(TFile::Open(fileName.c_str()));
  if (!f || f->IsZombie() ) {
    ATH_MSG_FATAL("Could not open file: " << fileName);
    return StatusCode::FAILURE;
  }

  // Load hPoly
  TH2Poly *hPoly = nullptr;
  f->GetObject("hPoly", hPoly);
  if (!hPoly) {
    ATH_MSG_FATAL("Could not find hPoly");
    return StatusCode::FAILURE;
  }
  
  m_hPoly.reset(static_cast<TH2Poly*>(hPoly->Clone()));
  m_hPoly->SetDirectory(0);

  // Load variables
  TObjArray *variablesTmp = nullptr;
  f->GetObject("variables", variablesTmp);
  if (!variablesTmp) {
    ATH_MSG_FATAL("Could not find variables");
    return StatusCode::FAILURE;
  }
  auto variables = std::unique_ptr<TObjArray>(variablesTmp);
  variables->SetOwner(); // to delete the objects when d-tor is called

  // Load shifts
  TObjArray *shiftsTmp = nullptr;
  f->GetObject("shifts", shiftsTmp);
  if (!shiftsTmp) {
    ATH_MSG_FATAL("Could not find shifts");
    return StatusCode::FAILURE;
  }
  auto shifts = std::unique_ptr<TObjArray>(shiftsTmp);
  shifts->SetOwner(); // to delete the objects when d-tor is called

  // Load trees
  TObjArray *treesTmp = nullptr;
  std::unique_ptr<TObjArray> trees;
  f->GetObject("trees", treesTmp);
  if (treesTmp) {
    trees = std::unique_ptr<TObjArray>(treesTmp);
    trees->SetOwner(); // to delete the objects when d-tor is called
    ATH_MSG_DEBUG("setupBDT " << "BDTs read from TObjArray");
  } else {
    ATH_MSG_DEBUG("setupBDT " << "Reading trees individually");
    trees = std::make_unique<TObjArray>();
    trees->SetOwner(); // to delete the objects when d-tor is called
    for (int i = 0; i < variables->GetEntries(); ++i)
    {
      TTree *tree = nullptr;
      f->GetObject(Form("BDT%d", i), tree);
      if (tree) tree->SetCacheSize(0);
      trees->AddAtAndExpand(tree, i);
    }
  }

  // Ensure the objects have (the same number of) entries
  if (!trees->GetEntries() || !(trees->GetEntries() == variables->GetEntries())) {
    ATH_MSG_FATAL("Tree has size " << trees->GetEntries() 
		  << " while variables has size " << variables->GetEntries());
    return StatusCode::FAILURE;
  }

  // Loop simultaneously over trees, variables and shifts
  // Define the BDTs, the list of variables and the shift for each BDT
  TObjString *str2, *shift;
  TTree *tree;
  TIter nextTree(trees.get());
  TIter nextVariables(variables.get());
  TIter nextShift(shifts.get());
  for (int i=0; (tree = (TTree*) nextTree()) && ((TObjString*) nextVariables()); ++i)
  {
    m_BDTs.emplace_back(tree);

    std::vector<std::function<float(const xAOD::Egamma*, const xAOD::CaloCluster*)> > funcs;
    // Loop over variables, which are separated by comma
    char separator_var = ';';
    if (getString(variables->At(i)).Index(";") < 1) separator_var = ','; // old versions
    std::unique_ptr<TObjArray> tokens(getString(variables->At(i)).Tokenize(separator_var));
    TIter nextVar(tokens.get());
    while ((str2 = (TObjString*) nextVar()))
    {
      const TString& varName = getString(str2);
      if (!varName.Length()) {
	ATH_MSG_FATAL("There was an empty variable name!");
	return StatusCode::FAILURE;
      }
      try {
	funcs.push_back(funcLibrary.at(varName.Data()));
      } catch(const std::out_of_range& e) {
	ATH_MSG_FATAL("Could not find formula for variable " << varName << ", error: " << e.what());
	return StatusCode::FAILURE;	
      } 
    }
    m_funcs.push_back(std::move(funcs));

    if (m_shiftType == MEAN10TOTRUE) {
      shift = (TObjString*) nextShift();
      const TString& shiftFormula = getString(shift);
      m_shifts.emplace_back("", shiftFormula);
    }
  }
  return StatusCode::SUCCESS;

}

StatusCode egammaMVACalibTool::initializeElectronFuncs(std::unordered_map<std::string,
						       std::function<float(const xAOD::Egamma*, 
									   const xAOD::CaloCluster*)> >& funcLibrary) const
{
  ATH_CHECK(initializeClusterFuncs(funcLibrary, "el"));
  ATH_CHECK(initializeEgammaFuncs(funcLibrary, "el"));

  using namespace egammaMVATreeHelpers;

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
  
  return StatusCode::SUCCESS;
}

StatusCode egammaMVACalibTool::initializeUnconvertedPhotonFuncs(std::unordered_map<std::string,
								std::function<float(const xAOD::Egamma*, 
										    const xAOD::CaloCluster*)> >& funcLibrary) const
{
  ATH_CHECK(initializeClusterFuncs(funcLibrary, "ph"));
  ATH_CHECK(initializeEgammaFuncs(funcLibrary, "ph"));

  return StatusCode::SUCCESS;
}

StatusCode egammaMVACalibTool::initializeConvertedPhotonFuncs(std::unordered_map<std::string,
							      std::function<float(const xAOD::Egamma*, 
										  const xAOD::CaloCluster*)> >& funcLibrary) const
{
  ATH_CHECK(initializeClusterFuncs(funcLibrary, "ph"));
  ATH_CHECK(initializeEgammaFuncs(funcLibrary, "ph"));

  using namespace egammaMVATreeHelpers;

  funcLibrary["ph_Rconv"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)->float
    { return xAOD::EgammaHelpers::conversionRadius(static_cast<const xAOD::Photon*>(eg)); };

  funcLibrary["convR"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*)->float
    { 
      auto ph = static_cast<const xAOD::Photon*>(eg);
      if (compute_ptconv(ph) > 3*CLHEP::GeV) {
	return xAOD::EgammaHelpers::conversionRadius(ph);
      } else {
	return 799.0;
      }
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
      } else {
	return 1.0f;
      }
    };

  if (m_use_layer_corrected) {
    funcLibrary["convEtOverPt"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*cl)->float
      {
	auto ph = static_cast<const xAOD::Photon*>(eg);

	float rv = 0.0;
	if (xAOD::EgammaHelpers::numberOfSiTracks(ph) == 2) {
	  rv = std::max(0.0f, compute_correctedcl_Eacc(*cl)/std::cosh(compute_cl_eta(*cl)*compute_ptconv(ph)));
	} 
	return std::min(rv, 2.0f);
      };
  } else {
    funcLibrary["convEtOverPt"] = [](const xAOD::Egamma* eg, const xAOD::CaloCluster*cl)->float
      {
	auto ph = static_cast<const xAOD::Photon*>(eg);

	float rv = 0.0;
	if (xAOD::EgammaHelpers::numberOfSiTracks(ph) == 2) {
	  rv = std::max(0.0f, compute_rawcl_Eacc(*cl)/std::cosh(compute_cl_eta(*cl)*compute_ptconv(ph)));
	} 
	return std::min(rv, 2.0f);
      };
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaMVACalibTool::initializeClusterFuncs(std::unordered_map<std::string,
						      std::function<float(const xAOD::Egamma*, 
									  const xAOD::CaloCluster*)> >& funcLibrary,
						      const std::string& prefix) const
{

  using namespace egammaMVATreeHelpers;

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

  if (m_use_layer_corrected) {
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
    // and everything that depends on
    // static cast here is needed to resolve overload (std::function is not able to)
    funcLibrary[prefix + "_rawcl_calibHitsShowerDepth"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return compute_rawcl_calibHitsShowerDepth(*cl); };
    funcLibrary["R12"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return compute_rawcl_Es1(*cl)/compute_rawcl_Es2(*cl); };
    funcLibrary["fTG3"] = [](const xAOD::Egamma*, const xAOD::CaloCluster* cl)
      { return cl->eSample(CaloSampling::TileGap3)/compute_rawcl_Eacc(*cl); };
  }

  return StatusCode::SUCCESS;
}

StatusCode egammaMVACalibTool::initializeEgammaFuncs(std::unordered_map<std::string,
						     std::function<float(const xAOD::Egamma*, 
									 const xAOD::CaloCluster*)> >& funcLibrary,
						     const std::string& prefix) const
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

  return StatusCode::SUCCESS;
}

const TString& egammaMVACalibTool::getString(TObject* obj) const
{
  TObjString *objS = dynamic_cast<TObjString*>(obj);
  if (!objS) {
    throw std::runtime_error("egammaMVACalibTool::getString was passed something that was not a string object");
  }
  return objS->GetString();
}

float egammaMVACalibTool::getEnergy(const xAOD::Egamma* eg,
				    const xAOD::CaloCluster* clus) const
{

  ATH_MSG_DEBUG("calling getEnergy with cluster (" << clus << ") and eg (" << eg <<")");


  if (!clus && eg) {
    clus = eg->caloCluster();
  }
  if (!clus) {
    ATH_MSG_FATAL("The cluster pointer must not be null!");
    throw std::runtime_error("egammaMVACalibTool::getEnergy called with a null cluster");
    return 0.0;
  }

  // find the bin of BDT

  const auto initEnergy = (m_use_layer_corrected ? 
			   egammaMVATreeHelpers::compute_correctedcl_Eacc(*clus) :
                           egammaMVATreeHelpers::compute_rawcl_Eacc(*clus));
  
  const auto energyVarGeV = (initEnergy / std::cosh(clus->eta())) / CLHEP::GeV;
  const auto etaVar = std::abs(clus->eta());

  const auto bin = m_hPoly->FindBin(etaVar, energyVarGeV) - 1; // poly bins are shifted by one

  ATH_MSG_DEBUG("Using bin: " << bin);

  // select the bdt and funcsions. (shifts are done later if needed)
  const auto& bdt = m_BDTs[bin];
  const auto& funcs = m_funcs[bin];

  const size_t sz = funcs.size();

  // could consider adding std::array support to the BDTs
  std::vector<float> vars(sz);

  for (size_t i = 0; i < sz; i++) {
    vars[i] = funcs[i](eg,clus);
  }

  // evaluate the BDT response
  const float mvaOutput = bdt.GetResponse(vars);

  // what to do if the MVA response is 0;
  if (mvaOutput == 0.) {
    if (m_clusterEif0) {
      return clus->e();
    } else {
      return 0.;
    }
  }

  // calcluate the unshifted energy
  const auto energy = (m_calibrationType == fullCalibration) ?
    mvaOutput : (initEnergy * mvaOutput);

  ATH_MSG_DEBUG("energy after MVA = " << energy);

  if (m_shiftType == NOSHIFT) {
    // if no shift, just return the unshifted energy
    return energy;
  }

  // have to do a shift if here. It's based on the corrected Et in GeV
  const auto etGeV = (energy / std::cosh(clus->eta())) / CLHEP::GeV;

  // evaluate the TFormula associated with the bin
  const auto shift = m_shifts[bin].Eval(etGeV);
  ATH_MSG_DEBUG("shift = " << shift);
  if (shift > 0.5) {
    return energy / shift;
  } else {
    ATH_MSG_WARNING("Shift value too small: " << shift << "; not applying shift");
    return energy;
  }
}
