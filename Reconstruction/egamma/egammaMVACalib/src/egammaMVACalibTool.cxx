 /*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVACalibTool.h"

#include "PathResolver/PathResolver.h"

#include "TFile.h"
#include "TMath.h"
#include "TObjString.h"

#include <cmath>

#ifndef XAOD_ANALYSIS
#include "GaudiKernel/SystemOfUnits.h"
using Gaudi::Units::GeV;
#else
#define GeV 1000
#endif

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

  // get the BDTs
  ATH_MSG_DEBUG("get BDTs in folder: " << m_folder);
  switch (m_particleType) {
  case xAOD::EgammaParameters::electron:
    {
      std::unique_ptr<egammaMVAFunctions::funcMap_t> funcLibraryPtr = 
	egammaMVAFunctions::initializeElectronFuncs(m_useLayerCorrected);
      ATH_CHECK(setupBDT(*funcLibraryPtr, 
			 PathResolverFindCalibFile(m_folder + "/MVACalib_electron.weights.root")));
    }
    break;
  case xAOD::EgammaParameters::unconvertedPhoton:
    {
      std::unique_ptr<egammaMVAFunctions::funcMap_t> funcLibraryPtr = 
	egammaMVAFunctions::initializeUnconvertedPhotonFuncs(m_useLayerCorrected);
      ATH_CHECK(setupBDT(*funcLibraryPtr, 
			 PathResolverFindCalibFile(m_folder + "/MVACalib_unconvertedPhoton.weights.root")));
    }
    break;
  case xAOD::EgammaParameters::convertedPhoton:
    {
      std::unique_ptr<egammaMVAFunctions::funcMap_t> funcLibraryPtr = 
	egammaMVAFunctions::initializeConvertedPhotonFuncs(m_useLayerCorrected);
      ATH_CHECK(setupBDT(*funcLibraryPtr, 
			 PathResolverFindCalibFile(m_folder + "/MVACalib_convertedPhoton.weights.root")));
    }
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

StatusCode egammaMVACalibTool::setupBDT(const egammaMVAFunctions::funcMap_t& funcLibrary,
					const std::string& fileName)
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
  m_hPoly->SetDirectory(nullptr);

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
  TObjString *str2;
  TObjString *shift;
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

const TString& egammaMVACalibTool::getString(TObject* obj) const
{
  TObjString *objS = dynamic_cast<TObjString*>(obj);
  if (!objS) {
    throw std::runtime_error("egammaMVACalibTool::getString was passed something that was not a string object");
  }
  return objS->GetString();
}

float egammaMVACalibTool::getEnergy(const xAOD::CaloCluster& clus, 
                                    const xAOD::Egamma* eg) const
{

  ATH_MSG_DEBUG("calling getEnergy with cluster index (" << clus.index());

  // find the bin of BDT
  const auto initEnergy = (m_useLayerCorrected ? 
			   egammaMVAFunctions::compute_correctedcl_Eacc(clus) :
                           egammaMVAFunctions::compute_rawcl_Eacc(clus));
  
  const auto energyVarGeV = (initEnergy / std::cosh(clus.eta())) / GeV; 
  const auto etaVar = std::abs(clus.eta());

  ATH_MSG_DEBUG("Looking at object with initEnergy = " << initEnergy 
		<< ", energyVarGeV = " <<  energyVarGeV
		<< ", etaVar = " << etaVar
		<< ", clus->e() = " << clus.e());

  const int bin = m_hPoly->FindBin(etaVar, energyVarGeV) - 1; // poly bins are shifted by one

  ATH_MSG_DEBUG("Using bin: " << bin);

  if (bin < 0) {
    ATH_MSG_DEBUG("The bin is under/overflow; just return the energy");
    return clus.e();
  }

  if (bin >= static_cast<int>(m_BDTs.size())) {
    ATH_MSG_WARNING("The bin is outside the range, so just return the energy");
    return clus.e();
  }

  // select the bdt and funcsions. (shifts are done later if needed)
  const auto& bdt = m_BDTs[bin];
  const auto& funcs = m_funcs[bin];

  const size_t sz = funcs.size();

  // could consider adding std::array support to the BDTs
  std::vector<float> vars(sz);

  for (size_t i = 0; i < sz; i++) {
    vars[i] = funcs[i](eg,&clus);
  }

  // evaluate the BDT response
  const float mvaOutput = bdt.GetResponse(vars);

  // what to do if the MVA response is 0;
  if (mvaOutput == 0.) {
    if (m_clusterEif0) {
      return clus.e();
    } 
      return 0.;
    
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
  const auto etGeV = (energy / std::cosh(clus.eta())) / GeV; 

  // evaluate the TFormula associated with the bin
  const auto shift = m_shifts[bin].Eval(etGeV);
  ATH_MSG_DEBUG("shift = " << shift);
  if (shift > 0.5) {
    return energy / shift;
  } 
    ATH_MSG_WARNING("Shift value too small: " << shift << "; not applying shift");
    return energy;
  
}
