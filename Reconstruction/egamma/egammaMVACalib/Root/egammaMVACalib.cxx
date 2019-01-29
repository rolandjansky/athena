/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>
#include "CxxUtils/make_unique.h"

#include <TPRegexp.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TSystem.h>
#include <TMath.h>
#include <TTreeFormula.h>
#include <TTree.h>
#include <TList.h>
#include <TChain.h>
#include <TFile.h>
#include <TXMLEngine.h>
#include <TH2Poly.h>
#include <TAxis.h>
#include <TF1.h>
#include <TFile.h>

#include <TMVA/Reader.h>
#include <TMVA/MethodBDT.h>

#include "egammaMVACalib/egammaMVALayerDepth.h"
#include "egammaMVACalib/egammaMVACalib.h"
#include "MVAUtils/BDT.h"
#include "PathResolver/PathResolver.h"

using namespace MVAUtils;

#define CHECK_SETUPBDT(EXP) { \
  if (!EXP) { \
	 	ATH_MSG_WARNING( #EXP << " returned false (not present?), skipping " << f->GetName() ); \
    f->Close(); \
    return; \
  } \
}

// TODO:
// - DOCUMENTATION
// - Print "empty" bins ?

template<typename T>
std::unique_ptr<T> loadFromFile(TFile* f, std::string key)
{
  return std::unique_ptr<T>( dynamic_cast<T*>(f->Get(key.c_str())));
}

egammaMVACalib::egammaMVACalib(int particle,
                               bool useNewBDTs,
                               TString folder,
                               const TString & method,
                               int calibrationType,
                               bool debug,
                               const TString& etaBinVar,
                               const TString& energyBinVar,
                               const TString& particleTypeVar,
                               TString filePattern,
                               bool ignoreSpectators)
  : asg::AsgMessaging("egammaMVACalib"),
    m_useNewBDTs(useNewBDTs),
    fMethodName(method),
    m_etaVar(etaBinVar),
    m_energyVar(energyBinVar),
    m_particleTypeVar(particleTypeVar),
    m_ignoreSpectators(ignoreSpectators),
    m_hasEnergyBins(false), m_binMultiplicity(0),
    m_particleType(0), m_eta(0), m_energy(0), m_initialEnergy(0),
    m_hPoly(0),
    m_tree(0), m_input_tree(0),
    m_mvaOutput(0),
    m_dummyFloat(0),
    m_shiftType(NOSHIFT),
    m_clusterFormula(0)
{
    ATH_MSG_DEBUG("creating egammaMVACalib in debug mode with options:"
          << "\n  particle     : " << particle
          << "\n  new BDTs     : " << useNewBDTs
          << "\n  folder       : " << folder
          << "\n  method name  : " << fMethodName
          << "\n  calib type   : " << calibrationType
          << "\n  etavar       : " << m_etaVar
          << "\n  energy var   : " << m_energyVar
          << "\n  particle type: " << m_particleType
          << "\n  file pattern : " << filePattern
          << "\n  ignore spect : " << ignoreSpectators);

  if (debug) { ATH_MSG_WARNING("debug flag is deprecated and has no effect"); }

  setPeakCorrection(MEAN10TOTRUE);
  useClusterIf0(true);

  if (particle < 0 || particle >= static_cast<int>(NEGAMMATYPES))
  {
    ATH_MSG_FATAL("Invalid argument for particle "<< particle);
    throw std::runtime_error("Invalid argument for particle");
  }

  m_egammaType = static_cast<egammaType>(particle);

  if (calibrationType < 0 || calibrationType >= static_cast<int>(NCalibrationTypes))
  {
    ATH_MSG_FATAL("Invalid argument for calibrationType "<< calibrationType);
    throw std::runtime_error("Invalid argument for calibrationType");
  }
  m_calibrationType = static_cast<CalibrationType>(calibrationType);

  if (filePattern.Length() == 0)
    filePattern = "MVACalib_(.*?)_(.*?)_<calibType>_<method>.weights.xml";
  m_fileNamePattern = new TPRegexp(filePattern.ReplaceAll("<calibType>", getCalibTypeString()).ReplaceAll("<method>", fMethodName));
  ATH_MSG_DEBUG("regex pattern: " << m_fileNamePattern->GetPattern());

  // Use default formula for etaBin if not given
  if (m_etaVar.Length() == 0) {
    m_etaVar = (m_egammaType == egPHOTON) ? "ph_cl_eta" : "el_cl_eta";
    ATH_MSG_DEBUG("etavar not specified: set to default " << m_etaVar);
  }

  // Use default formula for energyBin if not given
  if (m_energyVar.Length() == 0)  {
    m_energyVar = (m_egammaType == egPHOTON) ?
      "(ph_rawcl_Es1 + ph_rawcl_Es2 + ph_rawcl_Es3)/cosh(ph_cl_eta)" :
      "(el_rawcl_Es1 + el_rawcl_Es2 + el_rawcl_Es3)/cosh(el_cl_eta)";
    ATH_MSG_DEBUG("energy var not specified: set to default " << m_energyVar);
  }

  // Define a formula for particleType if needed
  if (m_egammaType != egELECTRON && m_particleTypeVar.Length() == 0)
  {
    // old: 0=unconverted, 1=converted not Si+Si, 2=converted Si+Si
    // m_particleTypeVar = "(ph_convFlag%10 > 0) + (ph_convFlag%10 == 2 && (ph_convtrk1nPixHits + ph_convtrk1nSCTHits) > 1 && (ph_convtrk2nPixHits + ph_convtrk2nSCTHits) > 1)";
    // current: 0=unconverted, 1=converted
    m_particleTypeVar = "ph_Rconv > 0. && ph_Rconv < 800.";
    ATH_MSG_INFO( "Variable that defines particleType not set. Using default formula: " << m_particleTypeVar.Data());
  }

  if (folder.Length() == 0) {
    ATH_MSG_FATAL( "No calibration folder defined");
    throw std::runtime_error("No calibration folder defined");
  }
  ATH_MSG_INFO("Accessing calibration from " << folder.Data());
  // initializa MVA electron / photon

  ATH_MSG_INFO("Reading weights from " <<folder.Data());

  if (m_useNewBDTs) {
    getBDTs(folder.Data());
  }
  else {
    getReaders(folder);  // setup m_readers and m_additional_infos
  }
  if (!getNreaders())
  {
    ATH_MSG_FATAL("No reader defined");
    throw std::runtime_error("No reader defined");
  }

  printReadersInfo();
  ATH_MSG_INFO("Number of variables:" << m_formulae.size());
  if (msgLvl(MSG::DEBUG)) {
    for (std::map<TString, egammaMVACalib::VarFormula>::const_iterator it = m_formulae.begin(); it != m_formulae.end(); ++it)
      ATH_MSG_DEBUG("formula: " << it->second.expression);
  }
  // Define the formulae to retrieve the reader corresponding to each particle
  setupFormulasForReaderID();
  // Define the formula to retrieve the initial energy (used in getMVAResponseTree)
  defineInitialEnergyFormula();
}



/** Destructor **/
egammaMVACalib::~egammaMVACalib()
{
  ATH_MSG_DEBUG("deleting formulae");
  // Delete the formulae
  std::map< TString, egammaMVACalib::VarFormula >::iterator it;
  for (it = m_formulae.begin(); it != m_formulae.end(); ++it)
    delete it->second.formula;

  ATH_MSG_DEBUG("deleting the readers");
  // Delete the readers
  std::map< egammaMVACalib::ReaderID, TMVA::Reader* >::iterator itR;
  for (itR = m_readers.begin(); itR != m_readers.end(); ++itR)
    delete itR->second;

  ATH_MSG_DEBUG("deleting the BDTs");
  // Delete the readers
  std::map< egammaMVACalib::ReaderID, BDT* >::iterator itB;
  for (itB = m_BDTs.begin(); itB != m_BDTs.end(); ++itB) {
    delete itB->second;
  }

  ATH_MSG_DEBUG("deleting filenamepattern at " << m_fileNamePattern);
  delete m_fileNamePattern;
  ATH_MSG_DEBUG("deleting m_hPoly at " << m_hPoly);
  delete m_hPoly;

  ATH_MSG_DEBUG("deleting clusterFormula");
  delete m_clusterFormula;

  ATH_MSG_DEBUG("deleting internal tree");
  delete m_tree;

  ATH_MSG_DEBUG("finishing");
}

void egammaMVACalib::setPeakCorrection(TString shift_type)
{
  ShiftType shift = NOSHIFT;
  if (shift_type == "NOSHIFT") shift = NOSHIFT;
  else if (shift_type == "PEAKTOTRUE") shift = PEAKTOTRUE;
  else if (shift_type == "MEANTOTRUE") shift = MEANTOTRUE;
  else if (shift_type == "MEDIANTOTRUE") shift = MEDIANTOTRUE;
  else if (shift_type == "MEAN10TOTRUE") shift = MEAN10TOTRUE;
  else if (shift_type == "MEAN20TOTRUE") shift = MEAN20TOTRUE;
  else if (shift_type == "MEDIAN10TOTRUE") shift = MEDIAN10TOTRUE;
  else if (shift_type == "MEDIAN20TOTRUE") shift = MEDIAN20TOTRUE;
  else {
    ATH_MSG_FATAL("egammaMVACalib::setPeakCorrection non valid shift " << shift_type.Data());
    throw std::runtime_error("not valid shift");
  }
  setPeakCorrection(shift);
}


TH2Poly* egammaMVACalib::getTH2Poly() const
{
  return dynamic_cast<TH2Poly*>(m_hPoly->Clone("hPoly"));
}

egammaMVACalib::AdditionalInfoMap
egammaMVACalib::getUserInfo(const TString & xmlfilename)
{
  egammaMVACalib::AdditionalInfoMap result;

  TXMLEngine xml;
  XMLDocPointer_t xmldoc = xml.ParseFile(xmlfilename);
  if (!xmldoc) {

    xml.FreeDoc(xmldoc);
    return result;
  }
  XMLNodePointer_t mainnode = xml.DocGetRootElement(xmldoc);

  // loop to find <UserInfo>
  XMLNodePointer_t user_infos_node = xml.GetChild(mainnode);
  while (std::string(xml.GetNodeName(user_infos_node)) != "UserInfo")
  {
    user_infos_node = xml.GetNext(user_infos_node);
    if (user_infos_node == 0) break;
  }
  if (!user_infos_node) {
    xml.FreeDoc(xmldoc);
    return result;
  }

  // loop over all children inside <UserInfo>
  XMLNodePointer_t info_node = xml.GetChild(user_infos_node);
  while (info_node != 0)
  {
    XMLAttrPointer_t attr = xml.GetFirstAttr(info_node);
    TString name, value;
    while (attr != 0) {
      const TString key_name = xml.GetAttrName(attr);
      const TString key_value = xml.GetAttrValue(attr);
      if (key_name == "name") {
        if (!name.Length()) name = key_value;
        else {
          xml.FreeDoc(xmldoc);
          return result;
        }
      }
      else if (key_name == "value") {
        if (!value.Length()) value = key_value;
        else {
          xml.FreeDoc(xmldoc);
          return result;
        }
      }
      attr = xml.GetNextAttr(attr);
    }
    if (name.Length() and value.Length())
    {
      result.insert(std::make_pair(name, value));
    }
    else
    {
      static asg::AsgMessaging m_msg("egammaMVACalib::getUserInfo");
      m_msg.msg(MSG::ERROR)<<"error reading UserInfo section xml file " << xmlfilename.Data()<<endmsg;
    }
    info_node = xml.GetNext(info_node);
  }

  xml.FreeDoc(xmldoc);
  return result;
}

/** Loop over the files in the folder and instantiate one TMVA::Reader for each file
 * Files that do not match the required pattern are removed
 **/
void egammaMVACalib::getReaders(const TString & folder)
{
  ATH_MSG_DEBUG("<getReaders> in folder: " << folder);
  m_hPoly = new TH2Poly();
  std::vector< TMVA::Reader* > readers;

  TString path(folder);
  gSystem->ExpandPathName(path); // expand special characters
  TSystemDirectory dir("", path.Data());
  TList* list_of_files = dir.GetListOfFiles();

  if (!list_of_files)
  {
    ATH_MSG_FATAL("egammaMVACalib::getReaders Directory doesn't exist" << folder.Data());
    throw std::runtime_error("Directory doesn't exist");
  }
  ATH_MSG_DEBUG("found " << list_of_files->GetEntries() << " files");
  ATH_MSG_DEBUG("searching all xml files with pattern " << m_fileNamePattern->GetPattern());

  TSystemFile *file;
  TIter next(list_of_files);
  egammaMVACalib::ReaderID key;

  while ((file=(TSystemFile*)next()))
  {
    TString fileName = file->GetName();
    if (!fileName.EndsWith("xml")) continue;
    if (!parseFileName(fileName, key)) {
      ATH_MSG_WARNING("egammaMVACalib::getReaders filename not valid, skipping " << fileName.Data());
      continue;
    }

    if (m_readers.find(key) != m_readers.end()) {
      ATH_MSG_FATAL("egammaMVACalib::getReaders trying to create the same reader twice");
      throw std::runtime_error("trying to create the same reader twice");
    }

    //  cout << "create " << key.bin << endl;
    m_readers[key] = new TMVA::Reader("Silent");

    setupReader(m_readers[key], path + "/" + fileName);
    AdditionalInfoMap infos = getUserInfo(path + "/" + fileName);
    m_additional_infos.insert(std::make_pair(key, infos));
  }

  delete list_of_files;
}

void egammaMVACalib::getBDTs(const std::string & folder)
{
  ATH_MSG_DEBUG("<getBDTs> in folder: " << folder);
  if (m_egammaType == egPHOTON)
  {
    setupBDT(PathResolverFindCalibFile(folder + "/MVACalib_unconvertedPhoton.weights.root"));
    setupBDT(PathResolverFindCalibFile(folder + "/MVACalib_convertedPhoton.weights.root"));
  }
  else
    setupBDT(PathResolverFindCalibFile(folder + "/MVACalib_electron.weights.root"));

  m_binMultiplicity = 2; // just to print...
}

void egammaMVACalib::setupBDT(const TString& fileName)
{
  egammaMVACalib::ReaderID key;
  if (!parseFileName(fileName, key.particleType)) return;
  ATH_MSG_DEBUG("Setup BDT for particle " << key.particleType);

  TString filePath = PathResolverFindCalibFile(fileName.Data());
  std::unique_ptr<TFile> f(TFile::Open(filePath));
  CHECK_SETUPBDT( f.get() && f->IsZombie() );

  // Load hPoly
  auto hPoly = loadFromFile<TH2Poly>(f.get(), "hPoly");
  CHECK_SETUPBDT( hPoly );

  if (!m_hPoly) m_hPoly = (TH2Poly*) hPoly.get()->Clone();
  m_hPoly->SetDirectory(0);
  CHECK_SETUPBDT( m_hPoly );

  // Load formulae
  auto formulae = loadFromFile<TObjArray>(f.get(), "formulae");
  CHECK_SETUPBDT( formulae );
  formulae->SetOwner(); // to delete the objects when d-tor is called

  // Load variables
  auto variables = loadFromFile<TObjArray>(f.get(), "variables");
  CHECK_SETUPBDT( variables );
  variables->SetOwner(); // to delete the objects when d-tor is called

  // Load shifts
  auto shifts = loadFromFile<TObjArray>(f.get(), "shifts");
  CHECK_SETUPBDT( shifts );
  shifts->SetOwner(); // to delete the objects when d-tor is called

  // Load trees
  auto trees = loadFromFile<TObjArray>(f.get(), "trees");
  if (trees.get()) {
    trees->SetOwner(); // to delete the objects when d-tor is called
    ATH_MSG_DEBUG("setupBDT " << "BDTs read from TObjArray");
  }
  else{
    ATH_MSG_DEBUG("setupBDT " << "Reading trees individually");
    trees = CxxUtils::make_unique<TObjArray>();
    trees->SetOwner(); // to delete the objects when d-tor is called
    for (int i = 0; i < variables->GetEntries(); ++i)
    {
      TTree *tree = dynamic_cast<TTree*>(f->Get(Form("BDT%d", i)));
      if (tree) tree->SetCacheSize(0);
      trees->AddAtAndExpand(tree, i);
    }
  }

  // Ensure the objects have (the same number of) entries
  CHECK_SETUPBDT( trees->GetEntries() );
  CHECK_SETUPBDT( (trees->GetEntries() == variables->GetEntries()) );

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

}


bool egammaMVACalib::parseFileName(const TString & fileName, egammaMVACalib::ReaderID &key)
{
  // Check if the fileName matches the expected pattern for readers
  std::unique_ptr<TObjArray> match(m_fileNamePattern->MatchS(fileName));
  if (!match.get() or match->GetEntries() != 3) {
    return false;
  }

  // Define the ParticleType, convert the string to the enum
  TString pType = getString(match->At(1));
  egammaMVACalib::ParticleType particleType = getParticleType(pType);
  if (particleType == INVALIDPARTICLE) return false;

  // Use the fileName to define the fields (eta/energy) and the range of each field
  // (split the fileName assuming the fields are separated by "_")
  TString binDef = getString(match->At(2));   // e.g. Et20-40_eta1.55-1.74
  //   delete match;

  std::unique_ptr<TObjArray> binArray(binDef.Tokenize("_"));
  if (!binArray.get()) return false;

  TIter next(binArray.get());

  // Define the bin multiplicity (1D, 2D)
  if (!m_binMultiplicity)
    m_binMultiplicity = binArray->GetSize() - 1;
  else if (m_binMultiplicity != binArray->GetSize() - 1)
  {
    ATH_MSG_FATAL("parseFileName Mismatch in bin definition from file names");
    throw std::runtime_error("(parseFileName) Mismatch in bin definition from file names");
  }

  TPRegexp binPattern("(eta|Et|E)(.*)-(.*)"); // TODO: better, * -> +
  float etaMin = 0., etaMax = 1e99, energyMin = 0., energyMax = 1e99;
  TObjString *sObj;
  while ((sObj = (TObjString*) next()))
  {
    TObjArray* binMatch = binPattern.MatchS(sObj->GetString());
    if (!binMatch || binMatch->GetEntries() != 4)
    {
      ATH_MSG_WARNING("egammaMVACalib::setupBDT " << "Could not interpret (from fileName): " << binDef.Data());
      return false;
    }
    // Define the min/max of eta/E
    TString binName = getString(binMatch->At(1));
    if (binName == "eta")
    {
      etaMin = getString(binMatch->At(2)).Atof();
      etaMax = getString(binMatch->At(3)).Atof();
    }
    else if (binName == "E" || binName == "Et")
    {
      energyMin = getString(binMatch->At(2)).Atof();
      energyMax = getString(binMatch->At(3)).Atof();
    }
    delete binMatch;
  }

  key.particleType = particleType;
  key.bin = getBin(etaMin, etaMax, energyMin, energyMax);
  return true;
}

bool egammaMVACalib::parseFileName(const TString & fileName, egammaMVACalib::ParticleType &particleType)
{
  // Check if the fileName matches the expected pattern for readers
  TPRegexp fileNamePattern("MVACalib_(.*?).weights.root");
  std::unique_ptr<TObjArray> match(fileNamePattern.MatchS(fileName));
  if (!match.get() or match->GetEntries() != 2) {
    return false;
  }
  ATH_MSG_DEBUG("Checking: " << fileName.Data());
  // Define the ParticleType, convert the string to the enum
  TString pType = getString(match->At(1));
  particleType = getParticleType(pType);
  if (particleType == INVALIDPARTICLE) return false;
  return true;
}

TString egammaMVACalib::getBinName(const TString & binField)
{
  if (binField.BeginsWith("eta"))
    return TString("eta");
  if (binField.BeginsWith("Et"))
    return TString("Et");
  if (binField.BeginsWith("E"))
    return TString("E");
  return TString();

  // TString m_binVars("eta,E"),
  // Loop over the possible fields (defined by m_binVars)
  // and return the (first) one that matches the given string
  //    TIter varNext(m_binVars.Tokenize(",")); // possible fields are separated by a comma
  //    TObjString *varCandidate;
  //    while ((varCandidate = (TObjString*) varNext()))
  //    {
  //       TString binName = varCandidate->GetString();
  //       if (binField.BeginsWith(binName))
  //          return binName;
  //    }
  //    return TString();

}

int egammaMVACalib::getBin(float etaMin, float etaMax, float energyMin, float energyMax)
{
  int bin = m_hPoly->FindBin(0.5*(etaMin + etaMax), 0.5*(energyMin + energyMax));
  // Insert bin or check the edges to avoid overlapping
  // subtract 1e-10 because TH2Poly does not include low edge
  if (bin < 0)
  {
    bin = m_hPoly->AddBin(etaMin - 1e-10, energyMin - 1e-10, etaMax, energyMax);
    m_hPoly->SetBinContent(bin, bin); // just for drawing, no use
    ATH_MSG_DEBUG("New bin ("
			       << bin << "):"
			       << " etaMin = "
			       << etaMin << " / etaMax = "
			       << etaMax
			       << " / Emin = "
			       << energyMin
			       << " / Emax = "
			       << energyMax);
  }
  else if (!checkBin(bin, etaMin, etaMax, energyMin, energyMax))
    assert(false);

  return bin;
}

void egammaMVACalib::setupReader(TMVA::Reader *reader, const TString & xml_filename)
{
  //ATH_MSG_DEBUG("In setupReader: " <<  xml_filename);
  std::vector<egammaMVACalib::XmlVariableInfo> variable_infos = parseXml(xml_filename);

  // Add variables and spectators using the floats from m_formulae
  // Each TTreeFormula is defined later in InitTree
  for (std::vector<egammaMVACalib::XmlVariableInfo>::const_iterator itvar = variable_infos.begin();
       itvar != variable_infos.end(); ++itvar)
  {
    // Define the variable / spectator name and expression
    TString infoType  = (TString(itvar->nodeName).Contains("Variable") ?
                         "variable" : "spectator");
    TString expression = itvar->expression;
    TString varName    = itvar->label;

    if (!m_ignoreSpectators || infoType != "spectator")
      predefineFormula(varName, expression, infoType, itvar->varType);
    // Add variable / spectator to the reader
    // Define expression (if different than variable name)
    TString varDefinition(varName);
    if (varName != expression)
      varDefinition += " := " + expression;
    //ATH_MSG_DEBUG("Adding " << infoType << " " << varDefinition);

    if (infoType == "variable")
      reader->AddVariable(varDefinition, &(m_formulae[varName].variable));
    else if (infoType == "spectator" && !m_ignoreSpectators)
      reader->AddSpectator(varDefinition, &(m_formulae[varName].variable));
    else if (infoType == "spectator")
      reader->AddSpectator(varDefinition, &m_dummyFloat);
    else // should never happen
    {
      ATH_MSG_FATAL("(setupReader) : Unknown type from parser " << infoType.Data());
      throw std::runtime_error("(setupReader) : Unknown type from parser");
    }
  }

  reader->BookMVA(fMethodName.Data(), xml_filename.Data());
}

void egammaMVACalib::setupFormulasForReaderID()
{
  std::map< TString, egammaMVACalib::VarFormula >::iterator it = m_formulae.end();
  if (m_egammaType != egELECTRON && m_particleTypeVar.Length())
  {
    predefineFormula("particleType", m_particleTypeVar, "other");
    m_particleType = &(m_formulae["particleType"].variable);
    assert(m_particleType);
  }

  if (/*m_hasEnergyBins &&*/ m_energyVar.Length())
  {
    it = m_formulae.find(m_energyVar);
    if (it == m_formulae.end()) // does not correspond to a variable
    {
      predefineFormula("energyBin", m_energyVar, "other");
      m_energy = &(m_formulae["energyBin"].variable);
    }
    else
      m_energy = &(it->second.variable);
    assert(m_energy);
  }

  if (m_etaVar.Length())
  {
    it = m_formulae.find(m_etaVar);
    if (it == m_formulae.end()) // does not correspond to a variable
    {
      predefineFormula("etaBin", m_etaVar, "other");
      m_eta = &(m_formulae["etaBin"].variable);
    }
    else
      m_eta = &(it->second.variable);
    assert(m_eta);
  }
}

void egammaMVACalib::predefineFormula(const TString & name, const TString & expression,
                                      const TString & infoType, const TString & valueType)
{
  // Look for a previously defined formula
  std::map< TString, VarFormula >::iterator it = m_formulae.find(name);
  if (it == m_formulae.end()) // variable not yet defined
  {
    VarFormula v = { 0, expression, infoType, valueType, 0, 0 };
    m_formulae[name] = v;
    ATH_MSG_DEBUG("Formula " << name << " := " << expression);
  }
  // variable defined, compare the expressions
  else if (it->second.expression != expression)
  {
    ATH_MSG_FATAL("predefineFormula, Expressions for do not match. Old: New:" <<name.Data() << it->second.expression.Data() << expression.Data());
    throw std::runtime_error("Expressions do not match");
  }
  else if (infoType == "variable" && it->second.infoType != "variable")
  {
    ATH_MSG_DEBUG("Setting " << name << " to \"variable\"");
    it->second.infoType = "variable";
  }
}


egammaMVACalib::ReaderID egammaMVACalib::getReaderID()
{
  egammaMVACalib::ReaderID key = { (ParticleType) getParticleType(), getBin() }; //getEtaBin(), getEnergyBin() };
  return key;
}

int egammaMVACalib::getParticleType() const
{
  if (m_egammaType == egELECTRON)
    return ELECTRON;
  assert(m_particleType);
  return static_cast<int>(*m_particleType + 0.5); // +0.5 to avoid bad truncation
}

/** Return the eta / Et bin for the current entry using the etaBinVar(iable)/energy... **/
int egammaMVACalib::getBin() const
{
  const float eta = TMath::Abs(m_eta ? *m_eta : 0.);
  const float energy = (m_energy ? (*m_energy)/1e3 : 0.);
  ATH_MSG_DEBUG("(getBin) eta = " << eta << " energy = " << energy << " bin = " << m_hPoly->FindBin(eta, energy));
  return m_hPoly->FindBin(eta, energy);
}


TTree* egammaMVACalib::createInternalTree(TTree *tree)
{
  ATH_MSG_DEBUG("Creating internal tree");
  if (!tree) { 
      ATH_MSG_FATAL("tree is null"); 
      throw std::runtime_error("Null pointer");
  }
  TTree* new_tree = new TTree();
  new_tree->SetDirectory(0);
  new_tree->SetCacheSize(0);

  new_tree->Branch(fMethodName.Data(), &m_mvaOutput, Form("%s/F", fMethodName.Data()));
  checkShowerDepth(tree);

  return new_tree;
}


void egammaMVACalib::InitTree(TTree* tree, bool doNotify)
{
  ATH_MSG_DEBUG("InitTree");
  if (!m_tree) m_tree = createInternalTree(tree);
  if (tree)
  {
    if (m_input_tree == tree) return;
    m_input_tree = tree;
    m_tree->AddFriend(m_input_tree);
  }

  // tree changed (eg new file or first time InitTree is called)
  ATH_MSG_DEBUG("tree changed / new tree");


  // Loop over the formula map and instantiate the formulae
  ATH_MSG_DEBUG("Instantiating formulae");
  std::map< TString, egammaMVACalib::VarFormula >::iterator formulaIt;
  for (formulaIt = m_formulae.begin(); formulaIt != m_formulae.end(); ++formulaIt)
  {
    TString varName(formulaIt->first);
    egammaMVACalib::VarFormula *varFormula = &(formulaIt->second);
    // TODO: delete old formula
    varFormula->formula = defineFormula(varName, varFormula->expression, m_tree);

    if (!formulaIt->second.formula)
    {
      ATH_MSG_FATAL("InitTree Could not define formula");
      throw std::runtime_error("Could not define formula");
    }
  }

  // Instantiate the formulae for the shifts
  ATH_MSG_DEBUG("Instantiating formulae for the shifts");
  std::map< egammaMVACalib::ReaderID, egammaMVACalib::AdditionalInfoMap>::iterator it;
  for (it = m_additional_infos.begin(); it != m_additional_infos.end(); ++it)
    defineShiftFormula(it->first);

  // Instantiate the formula for cluster energy
  ATH_MSG_DEBUG("Instantiating formulae for cluster energy");
  defineClusterEnergyFormula();

  if (tree and doNotify) {
    for (formulaIt = m_formulae.begin(); formulaIt != m_formulae.end(); ++formulaIt) {
      m_notify_dispatcher.add_object(formulaIt->second.formula);
    }

    if (m_clusterFormula) { m_notify_dispatcher.add_object(m_clusterFormula); }

    m_input_tree->SetNotify(&m_notify_dispatcher);
  }
  ATH_MSG_DEBUG("Init tree done");
}

TTreeFormula* egammaMVACalib::defineFormula(const TString & varName, const TString & expression, TTree *tree)
{
  // Define and check the formula
  TTreeFormula* formula = new TTreeFormula(varName.Data(), expression.Data(), tree);
  if (!formula->GetNdim())
  {
    ATH_MSG_FATAL("defineFormula Invalid formula: (probably branch not present) " << varName.Data() << expression.Data());
    throw std::runtime_error("Invalid formula");
  }
  ATH_MSG_DEBUG("Formula: " << formula->GetName() << " := " << formula->GetTitle());

  return formula;
}


/** Set the variables for the reader, copying the results from the formulae **/
void egammaMVACalib::LoadVariables(int index)
{
  ATH_MSG_DEBUG("LoadVariables(" << index << ")");
  std::map< TString, egammaMVACalib::VarFormula >::iterator formulaIt;
  for (formulaIt = m_formulae.begin(); formulaIt != m_formulae.end(); ++formulaIt)
  {
    egammaMVACalib::VarFormula *varFormula = &(formulaIt->second);
    assert(varFormula);
    if (!varFormula->external) {
      assert(varFormula->formula);
      varFormula->formula->GetNdata(); // Avoids getting wrong values...
      ATH_MSG_DEBUG("evaluating formula " << varFormula->expression
                         << " TTreeFormula::Title=" << varFormula->formula->GetTitle()
                         << " for index " << index);
      varFormula->variable = varFormula->formula->EvalInstance(index);
    }
    ATH_MSG_DEBUG("Value of " << formulaIt->first << " : " << varFormula->variable);
  }
}

float* egammaMVACalib::getAddress(const TString & input_name)
{
  std::map< TString, egammaMVACalib::VarFormula >::iterator it = m_formulae.find(input_name);
  if (it == m_formulae.end()) {
    std::string formulae_string = "";
    for (std::map< TString, egammaMVACalib::VarFormula >::iterator it2 = m_formulae.begin();
         it2 != m_formulae.end(); ++it2) {
      formulae_string += std::string(" ") + it2->first.Data() + std::string(" ");
    }
    ATH_MSG_FATAL("getAddress , formula not found. "  << input_name.Data() <<" Should be one of "<< formulae_string.c_str());
    throw std::runtime_error("formula not found");
  }
  return &(it->second.variable);
}

void egammaMVACalib::setExternal(const TString & input_name)
{
  std::map< TString, egammaMVACalib::VarFormula >::iterator it = m_formulae.find(input_name);
  if (it == m_formulae.end()) {
    std::string formulae_string = "";
    for (std::map< TString, egammaMVACalib::VarFormula >::iterator it2 = m_formulae.begin();
         it2 != m_formulae.end(); ++it2) {
      formulae_string += std::string(" ") + it2->first.Data() + std::string(" ");
    }
    ATH_MSG_FATAL("setExternal , formula not found. "  << input_name.Data() <<" Should be one of "<< formulae_string.c_str());
    throw std::runtime_error("formula not found");
  }
  it->second.external = true;
}

void egammaMVACalib::printValueInput()
{
  std::map< TString, egammaMVACalib::VarFormula >::iterator formulaIt;
  ATH_MSG_INFO("printValueInput" << "information about last evaluation");
  for (formulaIt = m_formulae.begin(); formulaIt != m_formulae.end(); ++formulaIt)
  {
    egammaMVACalib::VarFormula *varFormula = &(formulaIt->second);
    assert(varFormula);
    assert(varFormula->formula);
    ATH_MSG_INFO ("printValueInput formula: "
		  << formulaIt->first.Data()
		  <<"\n   expression: "
		  << varFormula->expression.Data()
		  <<"\n   TTreeFormula::Title= "
		  << varFormula->formula->GetTitle()
		  <<"\n  value: " << varFormula->variable);
  }
}

int egammaMVACalib::getNdata()
{
  TTreeFormula *testFormula = m_formulae.begin()->second.formula;
  if (!testFormula || testFormula->GetNdim() == 0)
  {
    ATH_MSG_FATAL("getNdata , Could not get TTreeFormula");
    throw std::runtime_error("Could not get TTreeFormula");
  }
  return testFormula->GetNdata();
}


TTree* egammaMVACalib::getMVAResponseTree(TTree *tree, int Nentries, TString branchName,
                                          TString copyBranches, int first_event, bool flatten, int update)
{
  if (!tree)
  {
    ATH_MSG_WARNING("getMVAResponseTree " << "Null pointer to tree");
    return 0;
  }

  InitTree(tree);

  // Choose a TTreeFormula to define the number of instances per entry
  // (and define if we have vectors...)
  TTreeFormula *testFormula = m_formulae.begin()->second.formula;
  if (!testFormula || testFormula->GetNdim() == 0)
  {
    ATH_MSG_FATAL("getMVAResponseTree Could not get TTreeFormula for testing");
    throw std::runtime_error("Could not get TTreeFormula for testing");
  }

  TTree *mvaTree = getOutputTree(copyBranches, true);
  activateBranches();

  // Define the output branch and the variables to fill it
  if (branchName == "")
    branchName = fMethodName;
  bool useVector(false);
  float mvaOutput;
  std::vector<float> mvaVector;

  // Use a vector (the dimension of the formula should tell) ?
  if (!flatten && testFormula->GetMultiplicity() == 1)
  {
    useVector = true;
    mvaTree->Branch(branchName.Data(), &mvaVector);
  }
  else
    mvaTree->Branch(branchName.Data(), &mvaOutput, Form("%s/F", fMethodName.Data()));

  // Loop over the entries of the input tree
  if (Nentries == -1) Nentries = tree->GetEntries() - first_event;
  ATH_MSG_INFO("Entries to process: " << Nentries);
  for (int ientry=first_event; ientry < Nentries + first_event; ++ientry)
  {
    ATH_MSG_DEBUG("Processing entry " << ientry);
    if (ientry%update == 0) ATH_MSG_INFO ("Processing entry " << ientry);
    if (useVector) mvaVector.clear();
    if (tree->GetEntry(ientry) < 0) return mvaTree;

    // Loop over the indices inside the entry
    // Fill the tree (or the vector)
    for (int index = 0; index < testFormula->GetNdata(); ++index)
    {
      mvaOutput = getMVAEnergy(index);
      if (!useVector)
        mvaTree->Fill();
      else
        mvaVector.push_back(mvaOutput);
    }
    if (useVector) mvaTree->Fill();
  }
  ATH_MSG_INFO("Processing done: " << Nentries << " written");
  return mvaTree;
}

/** Get the MVA output (or 0 if the correponding TMVA::Reader is not found) **/
float egammaMVACalib::getMVAOutput(int index /* = 0 */)
{
  LoadVariables(index);
  if (m_useNewBDTs)
  {
    BDT *bdt = getBDT(getReaderID());
    if (!bdt) return 0;
    return bdt->GetResponse();
  }
  // Find the corresponding reader
  TMVA::Reader *reader = getReader(getReaderID());
  if (msgLvl(MSG::DEBUG))
  {
    std::stringstream ss;
    ss << "index = " << index << " / particle = " << getParticleType();
    if (m_eta) ss << " / eta = " << *m_eta;
    if (m_energy) ss << " / energy = " << *m_energy;
    if (!reader)
      ATH_MSG_DEBUG("-- No reader found, " << ss.str());
    else
      ATH_MSG_DEBUG("-- Found reader, " << ss.str());
  }

  if (!reader)
    return 0;
  return reader->EvaluateRegression(fMethodName.Data())[0];
}


float egammaMVACalib::getMVAEnergy(int index /* = 0 */,
                                   ShiftType shift_type /* =NSHIFTCORRECTIONS */)
{
  m_mvaOutput = getMVAOutput(index);
  ATH_MSG_DEBUG("MVA output = " << m_mvaOutput);
  if (m_mvaOutput == 0.)
  {
    if (m_clusterEif0 && m_clusterFormula)
    {
      m_clusterFormula->GetNdata(); // FIXME: seems this is needed to prevent 0s...
      float clusterE = m_clusterFormula->EvalInstance(index);
      ATH_MSG_DEBUG("Returning cluster E: " << clusterE);
      return clusterE;
    }
    return 0.;
  }
  float energy = 0;
  if (m_calibrationType == fullCalibration)
    energy = m_mvaOutput;
  else if (m_initialEnergy)
    energy = (*m_initialEnergy) * m_mvaOutput;

  ATH_MSG_DEBUG("energy after MVA = " << energy);

  ShiftType shift_to_use = m_shiftType;
  if (shift_type != NSHIFTCORRECTIONS) shift_to_use = shift_type;

  egammaMVACalib::ReaderID key = getReaderID();
  float shift = getShift(energy / cosh(*m_eta), key, shift_to_use);
  ATH_MSG_DEBUG("shift = " << shift);
  if (shift > 0.5)
    return energy / shift;
  ATH_MSG_WARNING("getMVAEnergy " << "Shift not applied (too small or negative value?)");
  return energy;
}

void egammaMVACalib::defineInitialEnergyFormula()
{
  if (m_calibrationType == fullCalibration)
    return;

  std::map< TString, egammaMVACalib::VarFormula >::iterator it = m_formulae.end();

  if (m_calibrationType == correctEcluster)
  {
    if (m_egammaType == egPHOTON)
      it = m_formulae.find("ph_cl_E");
    else
      it = m_formulae.find("el_cl_E");
  }
  else if (m_calibrationType == correctEaccordion)
  {
    if (m_egammaType == egPHOTON)
      it = m_formulae.find("ph_rawcl_Eacc");
    else
      it = m_formulae.find("el_rawcl_Eacc");
  }
  else assert(false);

  if (it == m_formulae.end())
  {
    ATH_MSG_INFO("defineInitialEnergyFormula" << "Trying to define formula for initial energy...");
    TString expr = (m_egammaType == egPHOTON) ?
      "ph_rawcl_Es1 + ph_rawcl_Es2 + ph_rawcl_Es3" :
      "el_rawcl_Es1 + el_rawcl_Es2 + el_rawcl_Es3";

    predefineFormula("initEnergy", expr, "other", "F");
    it = m_formulae.find("initEnergy");
  }

  if (it != m_formulae.end())
    m_initialEnergy = &(it->second.variable);
  else
  {
    ATH_MSG_WARNING("defineInitialEnergyFormula " << "Could not define formula for initial energy");
  }
  // TODO: define formula
}


std::vector<egammaMVACalib::XmlVariableInfo>
egammaMVACalib::parseXml(const TString & xml_filename)
{
  std::vector<egammaMVACalib::XmlVariableInfo> result;

  TXMLEngine xml;
  XMLDocPointer_t xmldoc = xml.ParseFile(xml_filename);
  if (!xmldoc) {
      static asg::AsgMessaging m_msg("egammaMVACalib::parseXml");
      m_msg.msg(MSG::FATAL)<<" file not found "  <<xml_filename.Data() << " current directory is:  " <<  gSystem->WorkingDirectory()<<endmsg;
      throw std::runtime_error("file not found");
  }
  XMLNodePointer_t mainnode = xml.DocGetRootElement(xmldoc);

  // loop to find <Variables> and <Spectators>
  XMLNodePointer_t node = xml.GetChild(mainnode);
  while (node)
  {
    node = xml.GetNext(node);
    TString nodeName = xml.GetNodeName(node);
    if (nodeName == "Variables" || nodeName == "Spectators") {
      std::vector<egammaMVACalib::XmlVariableInfo> r = parseVariables(&xml, node, nodeName);
      result.insert(result.end(), r.begin(), r.end());
    }
  }
  xml.FreeDoc(xmldoc);
  return result;
}

std::vector<egammaMVACalib::XmlVariableInfo>
egammaMVACalib::parseVariables(TXMLEngine *xml, void* node, const TString & nodeName)
{
  std::vector<egammaMVACalib::XmlVariableInfo> result;
  if (!xml || !node) return result;

  // loop over all children inside <Variables> or <Spectators>
  for (XMLNodePointer_t info_node = xml->GetChild(node); info_node != 0;
       info_node = xml->GetNext(info_node))
  {
    XMLAttrPointer_t attr = xml->GetFirstAttr(info_node);
    XmlVariableInfo o;
    // loop over the attributes of each child
    while (attr != 0)
    {
      TString name = xml->GetAttrName(attr);
      if (name == "Expression")
        o.expression = xml->GetAttrValue(attr);
      else if (name == "Label")
        o.label = xml->GetAttrValue(attr);
      else if (name == "Type")
        o.varType = xml->GetAttrValue(attr);
      attr = xml->GetNextAttr(attr);
    }
    //          ATH_MSG_DEBUG("Expression: " << expression << " Label: " << label << " varType: " << varType);
    o.nodeName = nodeName;
    result.push_back(o);
  }
  return result;
}



TString egammaMVACalib::getString(TObject* obj)
{
  TObjString *objS = dynamic_cast<TObjString*>(obj);
  if (!objS) return TString();
  return objS->GetString();
}

TObjArray* egammaMVACalib::getVariables(const TString &xmlFileName)
{
  std::vector<XmlVariableInfo> varInfo = parseXml(xmlFileName);
  TObjArray *o = new TObjArray();
  for (unsigned int i = 0; i < varInfo.size(); ++i)
    o->Add(new TObjString(varInfo[i].label));
  return o;
}

TString* egammaMVACalib::getVariables(TMVA::Reader *reader)
{
  TString *str = new TString();
  std::vector<TMVA::VariableInfo>::const_iterator it;
  for (it = reader->DataInfo().GetVariableInfos().begin(); it != reader->DataInfo().GetVariableInfos().end(); ++it)
    str->Append(TString(str->Length() ? ";" : "") + it->GetLabel());

  return str;
}


egammaMVACalib::ParticleType egammaMVACalib::getParticleType(const TString & s)
{
  if (s == "unconvertedPhoton")
    return UNCONVERTED;
  if (s == "convertedPhoton")
    return CONVERTED;
  if (s == "convertedSiSiPhoton")
    return SiSi;
  if (s == "electron")
    return ELECTRON;

  return INVALIDPARTICLE;
}

void egammaMVACalib::printReadersInfo() const
{

  ATH_MSG_INFO(getNreaders() << " readers created");
  TAxis* fAxisEta = m_hPoly->GetXaxis(); // TODO: ???
  TAxis* fAxisEnergy = (m_binMultiplicity > 1 ? m_hPoly->GetYaxis() : 0);

  if (fAxisEta){
    ATH_MSG_INFO("egammaMVACalib::printReadersInfo "
		 << fAxisEta->GetNbins()
		 <<" eta bin(s) -- ( "
		 <<fAxisEta->GetXmin() + 1e-10
		 <<" < abs("
		 <<   m_etaVar.Data()
		 << ") < "
		 << fAxisEta->GetXmax());
  }
  if (fAxisEnergy){
    ATH_MSG_INFO("egammaMVACalib::printReadersInfo "
		 << fAxisEnergy->GetNbins()
		 << " energy bin(s) -- ( "
		 << fAxisEnergy->GetXmin() + 1e-10
		 << "  < "
		 <<  m_energyVar.Data()
		 << " < "
		 << fAxisEnergy->GetXmax()
		 << "GeV");
  }
}

void egammaMVACalib::printFormulae() const
{
  std::map< TString, egammaMVACalib::VarFormula >::const_iterator it;
  for (it = m_formulae.begin(); it !=  m_formulae.end(); ++it)
  {
    ATH_MSG_DEBUG( it->first << ": ");
    if (it->second.formula)  ATH_MSG_DEBUG( it->second.formula->GetTitle());
  }
}

void egammaMVACalib::checkFormula(TTreeFormula *formula)
{
  assert(formula);
  formula->UpdateFormulaLeaves();
}

TObjArray* egammaMVACalib::getListOfBranches()
{
  if (!m_input_tree)
  {
    ATH_MSG_WARNING("getListOfBranches " << " No tree defined");
    return 0;
  }

  TObjArray* branches = new TObjArray();
  std::map< TString, egammaMVACalib::VarFormula >::iterator it;
  for (it = m_formulae.begin(); it !=  m_formulae.end(); ++it)
    addBranchesFromFormula(branches, it->second.formula);

  if (m_clusterFormula)
    addBranchesFromFormula(branches, m_clusterFormula);

  return branches;
}

void egammaMVACalib::addBranchesFromFormula(TObjArray* branches, TTreeFormula *formula)
{
  if (!formula || !branches)
    return;
  for (int i = 0; i < formula->GetNcodes(); ++i)
  {
    TBranch *br = formula->GetLeaf(i)->GetBranch();
    if (br && !branches->FindObject(br))
      branches->Add(br);
  }
}

void egammaMVACalib::activateBranches()
{
  TBranch *branch;
  TIter next(getListOfBranches());
  while ((branch = (TBranch*) next())) {
    ATH_MSG_INFO("activating " << branch->GetName());
    m_input_tree->SetBranchStatus(branch->GetName(), true);
  }
}

TString egammaMVACalib::getCalibTypeString()
{
  if (m_calibrationType == correctEcluster)
    return "Ecluster";
  else if (m_calibrationType == correctEaccordion)
    return "Eaccordion";
  else if (m_calibrationType == fullCalibration)
    return "Efull";
  return "";
}

TTree* egammaMVACalib::getOutputTree(TString copyBranches, bool deactivateFirst)
{
  // Deactivate all branches before cloning the tree
  if (deactivateFirst)
    m_input_tree->SetBranchStatus("*", 0);

  // Activate back the ones defined by <copyBranches>
  TObjString *sObj;
  TIter next(copyBranches.Tokenize(","));
  while ((sObj = (TObjString*) next()))
  {
    if (getString(sObj) == "input") // copy branches used to calculate MVA energy
      activateBranches();
    else
      m_input_tree->SetBranchStatus(getString(sObj), 1);
  }

  //TTree *mvaTree = new TTree(fMethodName.Data(), Form("%s tree", fMethodName.Data()));
  TTree *mvaTree = m_input_tree->CloneTree(0);
  //mvaTree->AddFriend(m_input_tree);
  mvaTree->SetName("MVA");
  return mvaTree;
}

bool egammaMVACalib::checkBin(int bin, float etaMin, float etaMax,
                              float energyMin, float energyMax)
{
  // TH2Poly stores the bins in the sequence they appear, so bin 1 is stored at bins[0]
  TH2PolyBin *polyBin = dynamic_cast<TH2PolyBin*>(m_hPoly->GetBins()->At(bin - 1));
  if (!polyBin)
  {
    ATH_MSG_FATAL("checkBin Could not get bin " << bin << " size " << m_hPoly->GetBins()->GetEntries());

    throw std::runtime_error("Could not get bin");
  }

  if (!TMath::AreEqualAbs(polyBin->GetXMin(), etaMin, 1e-9) ||
      !TMath::AreEqualAbs(polyBin->GetXMax(), etaMax, 1e-9) ||
      !TMath::AreEqualAbs(polyBin->GetYMin(), energyMin, 1e-9) ||
      !TMath::AreEqualAbs(polyBin->GetYMax(), energyMax, 1e-9))
  {
    ATH_MSG_FATAL("checkBin Bins do not coincide");
    ATH_MSG_DEBUG("Previous bin xmin: " << polyBin->GetXMin() << " / new: " << etaMin);
    ATH_MSG_DEBUG("Previous bin xmax: " << polyBin->GetXMax() << " / new: " << etaMax);
    ATH_MSG_DEBUG("Previous bin ymin: " << polyBin->GetYMin() << " / new: " << energyMin);
    ATH_MSG_DEBUG("Previous bin ymax: " << polyBin->GetYMax() << " / new: " << energyMax);
    throw std::runtime_error("Bins do not coincide");
  }
  return true;
}

float egammaMVACalib::getShift(float Et, ReaderID key, ShiftType shift_type) const
{
  ShiftType shift_to_use = m_shiftType;
  if (shift_type != NSHIFTCORRECTIONS) shift_to_use = shift_type;

  egammaMVACalib::ShiftMap::const_iterator it;
  it = m_shiftMap.find(std::make_pair(key, shift_to_use));
  if (it == m_shiftMap.end() || !it->second) return 1;
  float shift = it->second->Eval(Et / 1e3);
  ATH_MSG_DEBUG("Shift (Et = " << Et/1e3 << "): " << shift);
  return shift;
}

const TString& egammaMVACalib::getShiftName(ShiftType shift) const
{
  static TString result;
  result = "";
  switch (shift) {
  case NOSHIFT:        result = ""; break;
  case PEAKTOTRUE:     result = "Peak"; break;
  case MEANTOTRUE:     result = "Mean"; break;
  case MEDIANTOTRUE:   result = "Median"; break;
  case MEAN10TOTRUE:   result = "Mean10"; break;
  case MEAN20TOTRUE:   result = "Mean20"; break;
  case MEDIAN10TOTRUE: result = "Median10"; break;
  case MEDIAN20TOTRUE: result = "Median20"; break;
  default: assert(false);
  }
  return result;
}


void egammaMVACalib::defineShiftFormula(ReaderID key)
{
  // Loop over the possible shifts and define a formula for each
  for (unsigned int i = 1; i < static_cast<int>(NSHIFTCORRECTIONS); ++i)
  {
    ShiftType shift = static_cast<ShiftType>(i);
    TString shift_name = getShiftName(shift);
    if (!m_additional_infos[key].count(shift_name)) continue;
    TString expr = m_additional_infos[key][shift_name];
    expr.ReplaceAll("BDTG*(ph_rawcl_Es1 + ph_rawcl_Es2 + ph_rawcl_Es3)/cosh(ph_cl_eta)/1e3", "x");
    expr.ReplaceAll("BDTG*(el_rawcl_Es1 + el_rawcl_Es2 + el_rawcl_Es3)/cosh(el_cl_eta)/1e3", "x");
    //  ATH_MSG_DEBUG("Defining shift for bin " << key.bin << ": " << expr);
    TString name = Form("shift_%d_%d_%s", key.particleType, key.bin, shift_name.Data());
    //  TTreeFormula* formula = defineFormula(name, expr, m_tree);
    std::unique_ptr<TF1> formula(new TF1(name, expr));
    m_shiftMap[std::make_pair(key, shift)] = std::move(formula);
  }
}

void egammaMVACalib::defineClusterEnergyFormula()
{
  ATH_MSG_DEBUG("setting cluster energy formula");
  TString expr = (m_egammaType == egPHOTON) ? "ph_cl_E" : "el_cl_E";
  m_clusterFormula = defineFormula("clusterE", expr, m_tree);
}


void egammaMVACalib::setDefinition(const TString & variable, const TString & expression)
{
  int count = 0;
  // Loop over the formula map and replace variable by expression
  std::map< TString, egammaMVACalib::VarFormula >::iterator formulaIt;
  for (formulaIt = m_formulae.begin(); formulaIt != m_formulae.end(); ++formulaIt)
  {
    egammaMVACalib::VarFormula *varFormula = &(formulaIt->second);
    if (varFormula->expression.Contains(variable))
    {
      ++count;
      varFormula->expression = varFormula->expression.ReplaceAll(variable, expression);
    }
  }
  ATH_MSG_INFO("setDefinition " << "Replacements made: " << count);
}

/** Check if shower depth variable is present or if it needs to be calculated **/
void egammaMVACalib::checkShowerDepth(TTree *tree)
{
  TString prefix = (m_egammaType == egPHOTON) ?  "ph_" : "el_";
  TString showerDepth(prefix);
  showerDepth.Append("rawcl_calibHitsShowerDepth");

  if (m_formulae.count(showerDepth) && !tree->GetBranch(showerDepth.Data()))
  {
    ATH_MSG_INFO("checkShowerDepth " << "Using CalibrationHitOffline to calculate shower depth");
    TString newShowerDepth = "egammaMVACalib::get_shower_depth(ph_cl_eta, ph_rawcl_Es0, ph_rawcl_Es1, ph_rawcl_Es2, ph_rawcl_Es3)";
    newShowerDepth.ReplaceAll("ph_", prefix);
    m_formulae[showerDepth].expression = newShowerDepth;
  }
}

void egammaMVACalib::writeROOTfile(const TString& directory, int particle)
{
  if (m_useNewBDTs ? !m_BDTs.size() : !m_readers.size())
  {
    ATH_MSG_WARNING("writeROOTfile " << "No reader defined, not dumping ROOT file");
    return;
  }
  if (particle == INVALIDPARTICLE)
  {
    if (m_egammaType == egPHOTON)
    {
      writeROOTfile(directory, UNCONVERTED);
      writeROOTfile(directory, CONVERTED);
      return;
    }
    else
    {
      writeROOTfile(directory, ELECTRON);
      return;
    }
  }

  if (m_useNewBDTs)
  {
    ATH_MSG_WARNING("writeROOTfile " << "not implemented when reading ROOT files");
    return;
  }

  TString particleName("electron");
  if (particle == UNCONVERTED)
    particleName = "unconvertedPhoton";
  else if (particle == CONVERTED)
    particleName = "convertedPhoton";

  TString fileName = directory + TString("/MVACalib_") + particleName + TString(".weights.root");
  TFile *f = TFile::Open(fileName, "UPDATE");
  TObjArray trees, variables, formulae, shifts;

  // Convert readers to TTrees and fill variables
  // The index of each object in the array is determined by the binning
  std::map< egammaMVACalib::ReaderID, TMVA::Reader* >::const_iterator itReader;
  for (itReader = m_readers.begin(); itReader != m_readers.end(); ++itReader)
  {
    if (itReader->first.particleType != particle) continue;
    // bin has an offset so index = bin - 1
    addReaderInfoToArrays(itReader->second, &trees, &variables, itReader->first.bin - 1);
  }

  // Expressions (formulae)
  std::map<TString, egammaMVACalib::VarFormula>::const_iterator it;
  for (it = m_formulae.begin(); it != m_formulae.end(); ++it)
    formulae.Add(new TNamed(it->first, it->second.expression));

  // Shifts
  for (ShiftMap::iterator itS = m_shiftMap.begin(); itS != m_shiftMap.end(); ++itS)
  {
    if (itS->first.first.particleType != particle || itS->first.second != MEAN10TOTRUE)
      continue;
    shifts.Add(new TObjString(itS->second->GetTitle()));
  }

  // Write and close
  int option = (TObject::kSingleKey | TObject::kOverwrite);
//   trees.Write("trees", option);
  ATH_MSG_INFO("writeROOTfile " << "Ntrees: " << trees.GetEntries());
  trees.Print();
  trees.Write();
  variables.Write("variables", option);
  formulae.Write("formulae", option);
  shifts.Write("shifts", option);
  getTH2Poly()->Write(0, option);
  f->Close();
  ATH_MSG_INFO("writeROOTfile " << "Wrote ROOT file: " <<fileName.Data());

}

void egammaMVACalib::addReaderInfoToArrays(TMVA::Reader *reader,
                                           TObjArray *trees,
                                           TObjArray *variables,
                                           int index)
{
  if (index < 0) index = variables->GetSize();
  TString *vars = getVariables(reader);
  assert(vars);

  TMVA::MethodBDT* tbdt = dynamic_cast<TMVA::MethodBDT*>(reader->FindMVA("BDTG"));
  assert(tbdt);
  BDT *bdt = new BDT(tbdt);
  TTree *tree = bdt->WriteTree(Form("BDT%d", index));

  variables->AddAtAndExpand(new TObjString(*vars), index);
  trees->AddAtAndExpand(tree, index);
  delete bdt;
}


double egammaMVACalib::get_shower_depth(double eta,
                                        double raw_cl_0,
                                        double raw_cl_1,
                                        double raw_cl_2,
                                        double raw_cl_3)
{
  const std::array<float, 4> radius(get_MVAradius(eta));

  const double denominator = raw_cl_0 + raw_cl_1 + raw_cl_2 + raw_cl_3;
  if (denominator == 0) return 0.;

  return (radius[0] * raw_cl_0
           + radius[1] * raw_cl_1
           + radius[2] * raw_cl_2
           + radius[3] * raw_cl_3) / denominator;
}


TMVA::Reader* egammaMVACalib::getDummyReader(const TString &xmlFileName)
{
  float dummyFloat;
  TMVA::Reader *reader = new TMVA::Reader("Silent");

  std::vector<egammaMVACalib::XmlVariableInfo> variable_infos = parseXml(xmlFileName);
  for (std::vector<egammaMVACalib::XmlVariableInfo>::const_iterator itvar = variable_infos.begin();
       itvar != variable_infos.end(); ++itvar)
  {
    // Define the variable / spectator name and expression
    TString infoType  = (TString(itvar->nodeName).Contains("Variable") ?
                         "variable" : "spectator");
    TString expression = itvar->expression;
    TString varName    = itvar->label;

    // Add variable / spectator to the reader
    // Define expression (if different than variable name)
    TString varDefinition(varName);
    if (varName != expression)
      varDefinition += " := " + expression;

    if (infoType == "variable")
      reader->AddVariable(varDefinition, &dummyFloat);
    else if (infoType == "spectator")
      reader->AddSpectator(varDefinition, &dummyFloat);
    else // should never happen
    {
      static asg::AsgMessaging m_msg("egammaMVACalib::getDummyReader");
      m_msg.msg(MSG::FATAL)<<"Unknown type from parser "<< infoType.Data()<<endmsg;
      throw std::runtime_error("Unknown type from parser");
    }
  }

  reader->BookMVA("BDTG", xmlFileName);
  return reader;
}

//  LocalWords:  TObjArray
