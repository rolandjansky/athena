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

  // Load formulae
  TObjArray *formulaeTmp = nullPtr;
  f->GetObject("formulae", formulaeTmp);
  if (!formulaeTmp) {
    ATH_MSG_FATAL("Could not find formulae");
    return StatusCode::FAILURE;
  }
  auto formulae = std::unique_ptr<TObjArray>(formulaeTmp);
  formulae->SetOwner(); // to delete the objects when d-tor is called

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
  return StatusCode::SUCCESS;
}
