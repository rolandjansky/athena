 /*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVACalibTool.h"

#include "egammaMVACalib/egammaMVALayerDepth.h"
#include "egammaMVACalib/egammaMVATreeHelper.h"
#include "MVAUtils/BDT.h"
#include "PathResolver/PathResolver.h"

#include "TFile.h"


egammaMVACalibTool::egammaMVACalibTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

egammaMVACalibTool::~egammaMVACalibTool()
{
}

StatusCode egammaMVACalibTool::initialize()
{

  // get the BDTs
  ATH_MSG_DEBUG("get BDTs in folder: " << m_folder);
  switch (m_particleType) {
  case xAOD::EgammaParameters::electron:
    ATH_CHECK(initializeElectronFuncs());
    ATH_CHECK(setupBDT(PathResolverFindCalibFile(folder + "/MVACalib_electron.weights.root")));
    break;
  case xAOD::EgammaParameters::uncovertedPhoton:
    ATH_CHECK(initializeUnconvertedPhotonFuncs());
    ATH_CHECK(setupBDT(PathResolverFindCalibFile(folder + "/MVACalib_unconvertedPhoton.weights.root")));
    break;
  case xAOD::EgammaParameters::covertedPhoton:
    ATH_CHECK(initializeConvertedPhotonFuncs());
    ATH_CHECK(setupBDT(PathResolverFindCalibFile(folder + "/MVACalib_convertedPhoton.weights.root")));
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

StatusCode egammmMVACalibTool::setupBDT(std::string fileName)
{
  std::unique_ptr<TFile> f(TFile::Open(fileName));
  if (!f || f->IsZombie() ) {
    ATH_MSG_FATAL("Could not open file: " << fileName);
    return StatusCode::FAILURE;
  }

  // Load hPoly
  TH2Poly *hPoly = nullPtr;
  f->GetObject("hPoly", hPoly);
  if (!hPoly) {
    ATH_MSG_FATAL("Could not find hPoly");
    return StatusCode::FAILURE;
  }
  
  m_hPoly = hPoly->Clone();
  m_hPoly->SetDirectory(0);

  // // Load formulae
  // TObjArray *formulaeTmp = nullPtr;
  // f->GetObject("formulae", formulaeTmp);
  // if (!formulaeTmp) {
  //   ATH_MSG_FATAL("Could not find formulae");
  //   return StatusCode::FAILURE;
  // }
  // auto formulae = std::unique_ptr<TObjArray>(formulaeTmp);
  // formulae->SetOwner(); // to delete the objects when d-tor is called

  // Load variables
  TObjArray *variablesTmp = nullPtr;
  f->GetObject("variables", variablesTmp);
  if (!variablesTmp) {
    ATH_MSG_FATAL("Could not find variables");
    return StatusCode::FAILURE;
  }
  auto variables = std::unique_ptr<TObjArray>(variablesTmp);
  variables->SetOwner(); // to delete the objects when d-tor is called

  // Load shifts
  TObjArray *shiftsTmp = nullPtr;
  f->GetObject("shifts", shiftsTmp);
  if (!shiftsTmp) {
    ATH_MSG_FATAL("Could not find shifts");
    return StatusCode::FAILURE;
  }
  auto shifts = std::unique_ptr<TObjArray>(shiftsTmp);
  shifts->SetOwner(); // to delete the objects when d-tor is called

  // Load trees
  TObjArray *treesTmp = nullPtr;
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

  // (pre) define formulae
  TNamed *formula;
  TIter nextFormula(formulae.get());
  while ((formula = (TNamed*) nextFormula())){
    predefineFormula(formula->GetName(), formula->GetTitle(), "variable");
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
    key.bin = i+1; // bin has an offset
    BDT *bdt = new BDT(tree);
    m_BDTs[key] = bdt;
    std::vector<float*> pointers;

    // Loop over variables, which are separated by comma
    char separator_var = ';';
    if (getString(variables->At(i)).Index(";") < 1) separator_var = ','; // old versions
    std::unique_ptr<TObjArray> tokens(getString(variables->At(i)).Tokenize(separator_var));
    TIter nextVar(tokens.get());
    while ((str2 = (TObjString*) nextVar()))
    {
      TString varName = getString(str2);
      if (!varName.Length()) continue;
      std::map< TString, VarFormula >::iterator it = m_formulae.find(varName);
      if (it == m_formulae.end())
      {
        ATH_MSG_FATAL("egammaMVACalib::setupBDT Variable not defined: "<< varName.Data());
        throw std::runtime_error("Variable not defined");
      }
      pointers.push_back(&(it->second.variable));
    }
    bdt->SetPointers(pointers);
    shift = (TObjString*) nextShift();
    if (shift) m_additional_infos[key]["Mean10"] = getString(shift);
  }
  return StatusCode::SUCCESS;

}

StatusCode egammmMVACalibTool::initializeElectronFuncs()
{
  ATH_CHECK(initializeClusterFuncs("el"));
  ATH_CHECK(initializeEgammaFuncs("el"));

  m_electronFuncs["el_charge"] = compute_el_charge;
  m_electronFuncs["el_tracketa"] = compute_el_tracketa;
  m_electronFuncs["el_trackpt"] = compute_el_trackpt;
  m_electronFuncs["el_trackz0"] = compute_el_trackz0;
  m_electronFuncs["el_refittedTrack_qoverp"] = compute_el_refittedTrack_qoverp;
  
  return StatusCode::SUCCESS;
}

StatusCode egammmMVACalibTool::initializeUnconvertedPhotonFuncs()
{
  ATH_CHECK(initializeClusterFuncs("ph"));
  ATH_CHECK(initializeEgammaFuncs("ph"));

  return StatusCode::SUCCESS;
}

StatusCode egammmMVACalibTool::initializeConvertedPhotonFuncs()
{
  ATH_CHECK(initializeClusterFuncs("ph"));
  ATH_CHECK(initializeEgammaFuncs("ph"));

  m_convFunc["ph_Rconv"] = [](const ConversionHelper& ch) { return ch.ph_Rconv(); };
  m_convFunc["ph_zconv"] = [](const ConversionHelper& ch) { return ch.ph_zconv(); };
  m_convFunc["ph_pt1conv"] = [](const ConversionHelper& ch) { return ch.ph_pt1conv(); };
  m_convFunc["ph_pt2conv"] = [](const ConversionHelper& ch) { return ch.ph_pt2conv(); };
  m_convFunc["ph_ptconv"] = [](const ConversionHelper& ch) { return ch.ph_ptconv(); };
  
  return StatusCode::SUCCESS;
}

StatusCode egammmMVACalibTool::initializeClusterFuncs(const std::string& prefix)
{
  m_clusterFuncs[prefix + "_cl_eta"] = compute_cl_eta;
  m_clusterFuncs[prefix + "_cl_phi"] = compute_cl_phi;
  m_clusterFuncs[prefix + "_cl_E"] = [](const xAOD::CaloCluster& cl) { return cl.e(); };
  m_clusterFuncs[prefix + "_cl_etaCalo"] = compute_cl_etaCalo;
  m_clusterFuncs[prefix + "_cl_phiCalo"] = compute_cl_phiCalo;
  m_clusterFuncs[prefix + "_cl_E_TileGap3"] = [](const xAOD::CaloCluster& cl) { return cl.eSample(CaloSampling::TileGap3); };

  if (m_use_layer_corrected) {
    m_clusterFuncs[prefix + "_rawcl_Es0"] = compute_correctedcl_Es0;
    m_clusterFuncs[prefix + "_rawcl_Es1"] = compute_correctedcl_Es1;
    m_clusterFuncs[prefix + "_rawcl_Es2"] = compute_correctedcl_Es2;
    m_clusterFuncs[prefix + "_rawcl_Es3"] = compute_correctedcl_Es3;
    m_clusterFuncs[prefix + "_rawcl_calibHitsShowerDepth"] = compute_correctedcl_calibHitsShowerDepth;
  } else {
    m_clusterFuncs[prefix + "_rawcl_Es0"] = compute_rawcl_Es0;
    m_clusterFuncs[prefix + "_rawcl_Es1"] = compute_rawcl_Es1;
    m_clusterFuncs[prefix + "_rawcl_Es2"] = compute_rawcl_Es2;
    m_clusterFuncs[prefix + "_rawcl_Es3"] = compute_rawcl_Es3;
    // and everything that depends on
    // static cast here is needed to resolve overload (std::function is not able to)
    m_clusterFuncs[prefix + "_rawcl_calibHitsShowerDepth"] = static_cast<float(*)(const xAOD::CaloCluster&)>(compute_rawcl_calibHitsShowerDepth);
  }

  return StatusCode::SUCCESS;
}

StatusCode egammmMVACalibTool::initializeEgammaFuncs(const std::string& prefix)
{
  m_egammaFuncs[prefix + "_e011"] = SG::AuxElement::Accessor<float>("e011");
  m_egammaFuncs[prefix + "_e033"] = SG::AuxElement::Accessor<float>("e033");
  m_egammaFuncs[prefix + "_e132"] = SG::AuxElement::Accessor<float>("e132");
  m_egammaFuncs[prefix + "_e1152"] = SG::AuxElement::Accessor<float>("e1152");
  m_egammaFuncs[prefix + "_ethad1"] = SG::AuxElement::Accessor<float>("ethad1");
  m_egammaFuncs[prefix + "_ethad"] = SG::AuxElement::Accessor<float>("ethad");
  m_egammaFuncs[prefix + "_f1"] = SG::AuxElement::Accessor<float>("f1");
  m_egammaFuncs[prefix + "_f3"] = SG::AuxElement::Accessor<float>("f3");
  m_egammaFuncs[prefix + "_f1core"] = SG::AuxElement::Accessor<float>("f1core");
  m_egammaFuncs[prefix + "_f3core"] = SG::AuxElement::Accessor<float>("f3core");
  m_egammaFuncs[prefix + "_e233"] = SG::AuxElement::Accessor<float>("e233");
  m_egammaFuncs[prefix + "_e235"] = SG::AuxElement::Accessor<float>("e235");
  m_egammaFuncs[prefix + "_e255"] = SG::AuxElement::Accessor<float>("e255");
  m_egammaFuncs[prefix + "_e237"] = SG::AuxElement::Accessor<float>("e237");
  m_egammaFuncs[prefix + "_e277"] = SG::AuxElement::Accessor<float>("e277");
  m_egammaFuncs[prefix + "_e333"] = SG::AuxElement::Accessor<float>("e333");
  m_egammaFuncs[prefix + "_e335"] = SG::AuxElement::Accessor<float>("e335");
  m_egammaFuncs[prefix + "_e337"] = SG::AuxElement::Accessor<float>("e337");
  m_egammaFuncs[prefix + "_e377"] = SG::AuxElement::Accessor<float>("e377");
  m_egammaFuncs[prefix + "_weta1"] = SG::AuxElement::Accessor<float>("weta1");
  m_egammaFuncs[prefix + "_weta2"] = SG::AuxElement::Accessor<float>("weta2");
  m_egammaFuncs[prefix + "_e2ts1"] = SG::AuxElement::Accessor<float>("e2ts1");
  m_egammaFuncs[prefix + "_e2tsts1"] = SG::AuxElement::Accessor<float>("e2tsts1");
  m_egammaFuncs[prefix + "_fracs1"] = SG::AuxElement::Accessor<float>("fracs1");
  m_egammaFuncs[prefix + "_widths1"] = SG::AuxElement::Accessor<float>("widths1");
  m_egammaFuncs[prefix + "_widths2"] = SG::AuxElement::Accessor<float>("widths2");
  m_egammaFuncs[prefix + "_poscs1"] = SG::AuxElement::Accessor<float>("poscs1");
  m_egammaFuncs[prefix + "_poscs2"] = SG::AuxElement::Accessor<float>("poscs2");
  m_egammaFuncs[prefix + "_asy1"] = SG::AuxElement::Accessor<float>("asy1");
  m_egammaFuncs[prefix + "_pos"] = SG::AuxElement::Accessor<float>("pos");
  m_egammaFuncs[prefix + "_pos7"] = SG::AuxElement::Accessor<float>("pos7");
  m_egammaFuncs[prefix + "_barys1"] = SG::AuxElement::Accessor<float>("barys1");
  m_egammaFuncs[prefix + "_wtots1"] = SG::AuxElement::Accessor<float>("wtots1");
  m_egammaFuncs[prefix + "_emins1"] = SG::AuxElement::Accessor<float>("emins1");
  m_egammaFuncs[prefix + "_emaxs1"] = SG::AuxElement::Accessor<float>("emaxs1");
  m_egammaFuncs[prefix + "_r33over37allcalo"] = SG::AuxElement::Accessor<float>("r33over37allcalo");
  m_egammaFuncs[prefix + "_ecore"] = SG::AuxElement::Accessor<float>("ecore");
  m_egammaFuncs[prefix + "_Reta"] = SG::AuxElement::Accessor<float>("Reta");
  m_egammaFuncs[prefix + "_Rphi"] = SG::AuxElement::Accessor<float>("Rphi");
  m_egammaFuncs[prefix + "_Eratio"] = SG::AuxElement::Accessor<float>("Eratio");
  m_egammaFuncs[prefix + "_Rhad"] = SG::AuxElement::Accessor<float>("Rhad");
  m_egammaFuncs[prefix + "_Rhad1"] = SG::AuxElement::Accessor<float>("Rhad1");
  m_egammaFuncs[prefix + "_DeltaE"] = SG::AuxElement::Accessor<float>("DeltaE");

  return StatusCode::SUCCESS;
}

