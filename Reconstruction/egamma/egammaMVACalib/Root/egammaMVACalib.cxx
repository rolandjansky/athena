/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <map>
#include <memory>

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

#include "egammaMVACalib/egammaMVACalib.h"
#include "egammaMVACalib/BDT.h"

#include "PathResolver/PathResolver.h"

#ifdef ROOTCORE
ClassImp(egammaMVACalib)
#endif

using namespace std;
using namespace egammaMVACalibNmsp; 

#define FATAL(x) cout << "-- <FATAL> egammaMVACalib: " << x << endl//; assert(false);
#define WARNING(x) cout << "-- <WARNING> egammaMVACalib: " << x << endl
#define INFO(x) cout << "-- <INFO> egammaMVACalib: " << x << endl
#define DEBUG(x) cout << "-- <DEBUG> egammaMVACalib: " << x << endl

// TODO:
// - DOCUMENTATION
// - Print "empty" bins ?

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
    : TObject(), 
      m_useNewBDTs(useNewBDTs),
      fMethodName(method), 
      m_debug(debug),
      m_etaVar(etaBinVar),
      m_energyVar(energyBinVar),
      m_particleTypeVar(particleTypeVar),
      m_ignoreSpectators(ignoreSpectators),
      m_hasEnergyBins(false), m_binMultiplicity(0),
      m_particleType(0), m_eta(0), m_energy(0), m_initialEnergy(0),
      m_hPoly(0),
      m_tree(0), m_input_tree(0),
      m_useInternalTree(true), m_shiftType(NOSHIFT), m_clusterFormula(0)
{
    if (m_debug) {
	DEBUG("creating egammaMVACalib in debug mode with options:"
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
    }
    
    setPeakCorrection(MEAN10TOTRUE);
    useClusterIf0(true);
    
    if (particle < 0 || particle >= static_cast<int>(NEGAMMATYPES) )
    {
	FATAL( "Invalid argument for particle " << particle );
	assert(false);
    }
    
    m_egammaType = static_cast<egammaType>(particle);
    
    if (calibrationType < 0 || calibrationType >= static_cast<int>(NCalibrationTypes) )
    {
	FATAL( "Invalid argument for calibrationType " << calibrationType );
	assert(false);
    }
    m_calibrationType = static_cast<CalibrationType>(calibrationType);
    
    if (filePattern.Length() == 0)
	filePattern = "MVACalib_(.*?)_(.*?)_<calibType>_<method>.weights.xml";
    m_fileNamePattern = new TPRegexp( filePattern.ReplaceAll("<calibType>", getCalibTypeString()).ReplaceAll("<method>", fMethodName) );   
    if (m_debug) DEBUG("regex pattern: " << m_fileNamePattern->GetPattern());
    
    // Use default formula for etaBin if not given
    if (m_etaVar.Length() == 0) {
	m_etaVar = (m_egammaType == egPHOTON) ? "ph_cl_eta" : "el_cl_eta";
	if (m_debug) DEBUG("etavar not specified: set to default " << m_etaVar);
    }
    
    // Use default formula for energyBin if not given
    if (m_energyVar.Length() == 0)  {
	m_energyVar = (m_egammaType == egPHOTON) ?
	    "(ph_rawcl_Es1 + ph_rawcl_Es2 + ph_rawcl_Es3)/cosh(ph_cl_eta)" :
	    "(el_rawcl_Es1 + el_rawcl_Es2 + el_rawcl_Es3)/cosh(el_cl_eta)";
	if (m_debug) DEBUG("energy var not specified: set to default " << m_energyVar);
    }
    
    // Define a formula for particleType if needed
    if (m_egammaType != egELECTRON && m_particleTypeVar.Length() == 0)
    {
	// old: 0=unconverted, 1=converted not Si+Si, 2=converted Si+Si
	// m_particleTypeVar = "(ph_convFlag%10 > 0) + (ph_convFlag%10 == 2 && (ph_convtrk1nPixHits + ph_convtrk1nSCTHits) > 1 && (ph_convtrk2nPixHits + ph_convtrk2nSCTHits) > 1)";
	// current: 0=unconverted, 1=converted
	m_particleTypeVar = "ph_Rconv > 0. && ph_Rconv < 800.";
	INFO( "Variable that defines particleType not set. "
	      << "Using default formula: " << m_particleTypeVar );
    }
    
    if (folder.Length() == 0) {
      // initializa MVA electron / photon
      std::string weightDir = PathResolver::find_directory ("egammaMVACalib/v1", "CALIBPATH");
      folder = weightDir;
      if (!m_useNewBDTs){
        folder += Form("/weights_%s", (m_egammaType == egPHOTON ? "photon" : "electron"));
      }
    }
    INFO("Reading weights from " << folder.Data());
    
    if (m_useNewBDTs){
        getBDTs(folder);
    }		
    else{
        getReaders(folder); // setup m_readers and m_additional_infos
    }	
    if (!getNreaders())
    {
	FATAL( "No reader defined" );
	assert(false);
    }
    INFO( "Number of variables: " << m_formulae.size() );
    if (m_debug) {
        for (map<TString, egammaMVACalib::VarFormula>::const_iterator it = m_formulae.begin(); it != m_formulae.end(); ++it)
	  DEBUG("formula: " << it->second.expression);
    }
    // Define the formulae to retrieve the reader corresponding to each particle
    setupFormulasForReaderID();
    // Define the formula to retrieve the initial energy (used in getMVAResponseTree)
    defineInitialEnergyFormula();
    //if (m_debug) printFormulae();
}



/** Destructor **/
egammaMVACalib::~egammaMVACalib()
{
    if (m_debug) DEBUG("deleting formulae");
    // Delete the formulae
    std::map< TString, egammaMVACalib::VarFormula >::iterator it;
    for (it = m_formulae.begin(); it != m_formulae.end(); ++it)
	delete it->second.formula;
    
    if (m_debug) DEBUG("deleting the readers");
    // Delete the readers
    std::map< egammaMVACalib::ReaderID, TMVA::Reader* >::iterator itR;
    for (itR = m_readers.begin(); itR != m_readers.end(); ++itR)
	delete itR->second;
    
    if (m_debug) DEBUG("deleting the BDTs");
    // Delete the readers
    std::map< egammaMVACalib::ReaderID, BDT* >::iterator itB;
    for (itB = m_BDTs.begin(); itB != m_BDTs.end(); ++itB) {
	delete itB->second;
    }    
    
    if (m_debug) DEBUG("deleting filenamepattern at " << m_fileNamePattern);
    delete m_fileNamePattern;
    if (m_debug) DEBUG("deleting m_hPoly at " << m_hPoly);
    delete m_hPoly;
    
    // Delete shift formulae
    if (m_debug) DEBUG("deleting m_shiftMap");
    ShiftMap::iterator itS;
    for (itS = m_shiftMap.begin(); itS != m_shiftMap.end(); ++itS)
   	delete itS->second;
    
    if (m_debug) DEBUG("deleting clusterFormula");
    delete m_clusterFormula;
    
    if (m_debug) DEBUG("deleting internal tree");
    delete m_tree;
    
    if (m_debug) DEBUG("finishing");
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
	FATAL(shift_type + " non valid shift");
	assert(false);
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
	FATAL("file not found " << xmlfilename << " current directory is: " << gSystem->WorkingDirectory());
	xml.FreeDoc(xmldoc);
	return result;
    }
    XMLNodePointer_t mainnode = xml.DocGetRootElement(xmldoc);
    
    // loop to find <UserInfo>
    XMLNodePointer_t user_infos_node = xml.GetChild(mainnode);
    while (string(xml.GetNodeName(user_infos_node)) != "UserInfo")
    {
	user_infos_node = xml.GetNext(user_infos_node);
	if (user_infos_node == 0) break;
    }
    if (!user_infos_node) {
	FATAL("user information (<UserInfo> section) not found");
	xml.FreeDoc(xmldoc);
	return result;
    }
    
    // loop over all children inside <UserInfo>
    XMLNodePointer_t info_node = xml.GetChild(user_infos_node);
    while (info_node != 0)
    {
	XMLAttrPointer_t attr = xml.GetFirstAttr(info_node);
	TString name, value;
	while (attr!=0) {
	    const TString key_name = xml.GetAttrName(attr);
	    const TString key_value = xml.GetAttrValue(attr);
	    if (key_name == "name") {
		if (!name.Length()) name = key_value;
		else {
		    WARNING("more than one name attribute in xml");
		    xml.FreeDoc(xmldoc);
		    return result;
		}
	    }
	    else if (key_name == "value") {
		if (!value.Length()) value = key_value;
		else {
		    WARNING("ERROR: more than one value attribute in xml");
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
	    FATAL("ERROR: error reading UserInfo section xml file " << xmlfilename);
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
    if (m_debug) DEBUG("<getReaders> in folder: " << folder);
    m_hPoly = new TH2Poly();
    std::vector< TMVA::Reader* > readers;
        
    TString path(folder);
    gSystem->ExpandPathName(path); // expand special characters
    TSystemDirectory dir("", path.Data());
    TList* list_of_files = dir.GetListOfFiles();
    
    if (!list_of_files)
    {
	FATAL( "Directory " << folder << " doesn't exist" );
	assert(false);
	return;
    }
    if (m_debug) DEBUG("found " << list_of_files->GetEntries() << " files");
    if (m_debug) DEBUG( "searching all xml files with pattern " << m_fileNamePattern->GetPattern());
    
    TSystemFile *file;
    TIter next(list_of_files);
    egammaMVACalib::ReaderID key;
    
    while ((file=(TSystemFile*)next()))
    {
	TString fileName = file->GetName();
	if (!fileName.EndsWith("xml")) continue;
	if ( !parseFileName( fileName, key ) ) {
	    WARNING("filename " << fileName << " not valid");
	    continue;
	}
	
	if (m_readers.find(key) != m_readers.end()) {
	    cerr << "FATAL: trying to create the same reader twice" << endl;
	    assert(false);
	}
	
// 	cout << "create " << key.bin << endl;
	m_readers[key] = new TMVA::Reader("Silent");
	
	setupReader( m_readers[key], path + "/" + fileName );
	AdditionalInfoMap infos = getUserInfo(path + "/" + fileName);
	m_additional_infos.insert(std::make_pair(key, infos));
    }
        
    delete list_of_files;
    printReadersInfo();
}

void egammaMVACalib::getBDTs(const TString & folder)
{
    if (m_debug) DEBUG("<getBDTs> in folder: " << folder);
    if (m_egammaType == egPHOTON)
    {
	setupBDT(folder + "/MVACalib_unconvertedPhoton.weights.root");
	setupBDT(folder + "/MVACalib_convertedPhoton.weights.root");
    }
    else
	setupBDT(folder + "/MVACalib_electron.weights.root");
    
    m_binMultiplicity = 2; // just to print...
    printReadersInfo();
}

void egammaMVACalib::setupBDT(const TString& fileName)
{
    egammaMVACalib::ReaderID key;
    if (!parseFileName(fileName, key.particleType)) return;
    if (m_debug) DEBUG("Setup BDT for particle " << key.particleType);
  
    TFile *f = TFile::Open(fileName);
    if (!f || f->IsZombie())
    {
        WARNING("Invalid file, skipping " << fileName.Data());
        return;
    }

    TH2Poly* hPoly = dynamic_cast<TH2Poly*> (f->Get("hPoly"));
    if (!hPoly)
    {
        WARNING("File does not contain hPoly, skipping " << fileName.Data());
	return;
    }
    if (!m_hPoly) m_hPoly = (TH2Poly*) hPoly->Clone();
    m_hPoly->SetDirectory(0);
    assert( m_hPoly );
    
    TObjArray *formulae = dynamic_cast<TObjArray*>( f->Get("formulae") );
    if (!formulae)
    {
        WARNING("File does not contain formulae, skipping " << fileName.Data());
	return;
    }

    TNamed *formula;
    TIter nextFormula(formulae);
    while ( (formula = (TNamed*) nextFormula()) )
	predefineFormula(formula->GetName(), formula->GetTitle(), "variable");
		
    TObjArray *trees = dynamic_cast<TObjArray*>( f->Get("trees") );
    if (!trees)
    {
        WARNING("File does not contain BDTs, skipping " << fileName.Data());
	return;
    }
		
    TObjArray *variables = dynamic_cast<TObjArray*>( f->Get("variables") );
    if (!variables)
    {
	WARNING("File does not contain variables, skipping " << fileName.Data());
	return;
    }
    
    assert(trees->GetEntries() == variables->GetEntries());
    
    // Loop simultaneously over trees, variables and shifts
    // Define the BDTs, the list of variables and the shift for each BDT
    TObjString *str, *str2, *shift;
    TTree *tree;
    TIter nextTree(trees);
    TIter nextVariables(variables);		
    TObjArray *shifts = dynamic_cast<TObjArray*>( f->Get("shifts") );
    TIter nextShift(shifts);
    for (int i=0; (tree = (TTree*) nextTree()) && (str = (TObjString*) nextVariables()); ++i)
    {
        key.bin = i+1; // bin has an offset
	BDT *bdt = new BDT(tree);
	m_BDTs[key] = bdt;
	std::vector<float*> pointers;
	
	// Loop over variables, which are separated by comma	
	auto_ptr<TObjArray> tokens(getString(variables->At(i)).Tokenize(","));
	TIter nextVar( tokens.get() ); 
	while ( (str2 = (TObjString*) nextVar()) )
	{
	    TString varName = getString(str2);
	    if (!varName.Length()) continue;
	    std::map< TString, VarFormula >::iterator it = m_formulae.find( varName );
	    if (it == m_formulae.end())
	    {
	      FATAL("Variable not defined: " << varName);
	      assert(false);
	      return;
	    }
	    pointers.push_back(&(it->second.variable));
	}
	bdt->SetPointers(pointers);
	shift = (TObjString*) nextShift();
	if (shift) m_additional_infos[key]["Mean10"] = getString(shift);
    }
    f->Close();

}


bool egammaMVACalib::parseFileName(const TString & fileName, egammaMVACalib::ReaderID &key)
{
    // Check if the fileName matches the expected pattern for readers
    auto_ptr<TObjArray> match(m_fileNamePattern->MatchS(fileName));
    if (!match.get() or match->GetEntries() != 3) {
	return false;
    }
    
    // Define the ParticleType, convert the string to the enum
    TString pType = getString(match->At(1));
    egammaMVACalib::ParticleType particleType = getParticleType(pType);
    if ( particleType == INVALIDPARTICLE ) return false;
    
    // Use the fileName to define the fields (eta/energy) and the range of each field
    // (split the fileName assuming the fields are separated by "_" )
    TString binDef = getString(match->At(2));   // e.g. Et20-40_eta1.55-1.74
    //   delete match;
    
    auto_ptr<TObjArray> binArray(binDef.Tokenize("_"));
    if (!binArray.get()) return false;
    
    TIter next(binArray.get());
    
    // Define the bin multiplicity (1D, 2D)
    if (!m_binMultiplicity)
	m_binMultiplicity = binArray->GetSize() - 1;
    else if (m_binMultiplicity != binArray->GetSize() - 1)
    {
	FATAL( "(parseFileName) Mismatch in bin definition from file names" );
	assert(false);
    }
    
    TPRegexp binPattern("(eta|Et|E)(.*)-(.*)"); // TODO: better, * -> +
    float etaMin = 0., etaMax = 1e99, energyMin = 0., energyMax = 1e99;
    TObjString *sObj;
    while ( (sObj = (TObjString*) next()) )
    {
	TObjArray* binMatch = binPattern.MatchS(sObj->GetString());
	if (!binMatch || binMatch->GetEntries() != 4)
	{
	    WARNING( "Could not interpret (from fileName): " << binDef );
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
    auto_ptr<TObjArray> match(fileNamePattern.MatchS(fileName));
    if (!match.get() or match->GetEntries() != 2) {
	return false;
    }
    if (m_debug) DEBUG("Checking: " << fileName.Data());
    // Define the ParticleType, convert the string to the enum
    TString pType = getString(match->At(1));
    particleType = getParticleType(pType);
    if ( particleType == INVALIDPARTICLE ) return false;
    return true;
}

TString egammaMVACalib::getBinName( const TString & binField )
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
    //    TIter varNext( m_binVars.Tokenize(",") ); // possible fields are separated by a comma
    //    TObjString *varCandidate;
    //    while ( (varCandidate = (TObjString*) varNext()) )
    //    {
    //       TString binName = varCandidate->GetString();
    //       if ( binField.BeginsWith( binName ) )
    //          return binName;
    //    }
    //    return TString();
    
}

int egammaMVACalib::getBin(float etaMin, float etaMax, float energyMin, float energyMax)
{
    int bin = m_hPoly->FindBin( 0.5*(etaMin + etaMax), 0.5*(energyMin + energyMax) );
    // Insert bin or check the edges to avoid overlapping
    // subtract 1e-10 because TH2Poly does not include low edge
    if (bin < 0)
    {
	bin = m_hPoly->AddBin( etaMin - 1e-10, energyMin - 1e-10, etaMax, energyMax );
	m_hPoly->SetBinContent(bin, bin); // just for drawing, no use
	if (m_debug) DEBUG("New bin (" << bin << "):"
			   << " etaMin = " << etaMin << " / etaMax = " << etaMax
			   << " / Emin = " << energyMin << " / Emax = " << energyMax);
    }
    else if (!checkBin(bin, etaMin, etaMax, energyMin, energyMax) )
	assert(false);
    
    return bin;
}   

void egammaMVACalib::setupReader(TMVA::Reader *reader, const TString & xml_filename)
{
    //DEBUG( "In setupReader: " <<  xml_filename );
    vector<egammaMVACalib::XmlVariableInfo> variable_infos = parseXml(xml_filename);
    
    // Add variables and spectators using the floats from m_formulae
    // Each TTreeFormula is defined later in InitTree
    for (vector<egammaMVACalib::XmlVariableInfo>::const_iterator itvar = variable_infos.begin();
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
	//if (m_debug) DEBUG("Adding " << infoType << " " << varDefinition );
	
	if (infoType == "variable")
	    reader->AddVariable( varDefinition, &(m_formulae[varName].variable) );
	else if (infoType == "spectator" && !m_ignoreSpectators)
	    reader->AddSpectator( varDefinition, &(m_formulae[varName].variable) );
	else if (infoType == "spectator")
	    reader->AddSpectator( varDefinition, &m_dummyFloat);
	else // should never happen
	{
	    FATAL( "(setupReader) : Unknown type from parser " << infoType );
	    assert(false);
	}
    }   
    
    reader->BookMVA(fMethodName.Data(), xml_filename.Data());	    
}

void egammaMVACalib::setupFormulasForReaderID()
{
    std::map< TString, egammaMVACalib::VarFormula >::iterator it = m_formulae.end();
    if (m_egammaType != egELECTRON && m_particleTypeVar.Length())
    {
	predefineFormula( "particleType", m_particleTypeVar, "other" );
	m_particleType = &(m_formulae["particleType"].variable);
	assert(m_particleType);
    }
    
    if (/*m_hasEnergyBins &&*/ m_energyVar.Length())
    {
	it = m_formulae.find(m_energyVar);
	if (it == m_formulae.end()) // does not correspond to a variable
	{
	    predefineFormula( "energyBin", m_energyVar, "other" );
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
	    predefineFormula( "etaBin", m_etaVar, "other" );
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
    std::map< TString, VarFormula >::iterator it = m_formulae.find( name );
    if (it == m_formulae.end()) // variable not yet defined
    {
        VarFormula v = { 0, expression, infoType, valueType, 0, 0 };
	m_formulae[name] = v;
	if (m_debug) DEBUG("Formula " << name << " := " << expression);
    }
    // variable defined, compare the expressions
    else if ( it->second.expression != expression )
    {
        FATAL( "(setupReader) Expressions for " << name <<
	       " do not match. Old: " << it->second.expression <<
	       " New: " << expression );
	assert(false);
    }
    else if ( infoType == "variable" && it->second.infoType != "variable" )
    {
        DEBUG("Setting " << name << " to \"variable\"");
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
    const float eta = TMath::Abs(m_eta ? *m_eta : 0. );
    const float energy = (m_energy ? (*m_energy)/1e3 : 0. );
    if (m_debug) {
        DEBUG("(getBin) eta = " << eta 
	      << " energy = " << energy << " bin = " 
	      << m_hPoly->FindBin(eta, energy));
	  }
    return m_hPoly->FindBin(eta, energy);
}


/** Return the eta bin for the current entry using the etaBinVar(iable) **/
// int egammaMVACalib::getEtaBin() 
// {
//    if (!m_eta || !fAxisEta) return 0;
// //    TAxis *axis = m_binAxis[ getParticleType(index) ];
// //    return axis->FindFixBin( TMath::Abs(*m_eta) );
//    return fAxisEta->FindFixBin( TMath::Abs(*m_eta) );
//    
// //    if (ietabin==0 or ietabin==fAxisEta->GetNbins()+1) { // under(over)flow
// //       cout << "WARNING: eta not in range: " << eta << " aeta = " << aeta << " bin: ietabin" << endl;
// //       return 0; 
// //    }
// //    return ietabin;
// }
// 
// int egammaMVACalib::getEnergyBin() 
// {
//    if (!m_energy || !fAxisEnergy) return 0; 
//    return fAxisEnergy->FindFixBin( (*m_energy)/1e3 );
// }


TTree* egammaMVACalib::createInternalTree(egammaType egamma_type, TTree *tree)
{
    if (m_debug) DEBUG("Creating internal tree");
    TTree* new_tree = new TTree();
    new_tree->SetDirectory(0);
    new_tree->Branch(fMethodName.Data(), &m_mvaOutput, Form("%s/F", fMethodName.Data()));
    if (tree) {
	m_useInternalTree = false;
	checkShowerDepth(tree);
    }
    else if (egamma_type == egPHOTON) {
	new_tree->Branch("ph_rawcl_Es0", &m_ph_rawcl_Es0);
	new_tree->Branch("ph_rawcl_Es1", &m_ph_rawcl_Es1);
	new_tree->Branch("ph_rawcl_Es2", &m_ph_rawcl_Es2);
	new_tree->Branch("ph_rawcl_Es3", &m_ph_rawcl_Es3);
	new_tree->Branch("ph_cl_eta", &m_ph_cl_eta);
	new_tree->Branch("ph_cl_E", &m_ph_cl_E);
	new_tree->Branch("ph_ptconv", &m_ph_ptconv);
	new_tree->Branch("ph_pt1conv", &m_ph_pt1conv);
	new_tree->Branch("ph_pt2conv", &m_ph_pt2conv);
	new_tree->Branch("ph_cl_etaCalo", &m_ph_cl_etaCalo);
	new_tree->Branch("ph_cl_phiCalo", &m_ph_cl_phiCalo);
	new_tree->Branch("ph_convtrk1nPixHits", &m_ph_convtrk1nPixHits);
	new_tree->Branch("ph_convtrk1nSCTHits", &m_ph_convtrk1nSCTHits);
	new_tree->Branch("ph_convtrk2nPixHits", &m_ph_convtrk2nPixHits);
	new_tree->Branch("ph_convtrk2nSCTHits", &m_ph_convtrk2nSCTHits);
	new_tree->Branch("ph_rawcl_calibHitsShowerDepth", &m_ph_rawcl_calibHitsShowerDepth);
	new_tree->Branch("ph_Rconv", &m_ph_Rconv);
    }
    else if (egamma_type == egELECTRON) {
	new_tree->Branch("el_rawcl_Es0", &m_el_rawcl_Es0);
	new_tree->Branch("el_rawcl_Es1", &m_el_rawcl_Es1);
	new_tree->Branch("el_rawcl_Es2", &m_el_rawcl_Es2);
	new_tree->Branch("el_rawcl_Es3", &m_el_rawcl_Es3);
	new_tree->Branch("el_cl_eta", &m_el_cl_eta);
	new_tree->Branch("el_cl_E", &m_el_cl_E);
	new_tree->Branch("el_cl_etaCalo", &m_el_cl_etaCalo);
	new_tree->Branch("el_cl_phiCalo", &m_el_cl_phiCalo);
	new_tree->Branch("el_rawcl_calibHitsShowerDepth", &m_el_rawcl_calibHitsShowerDepth);
    }
    else assert(false);
    return new_tree;
}


void egammaMVACalib::InitTree(TTree* tree, bool doNotify)
{
    if (m_debug) DEBUG("InitTree");
    if (!m_tree) m_tree = createInternalTree(m_egammaType, tree);
    if (tree)
    {
	if (m_input_tree == tree) return;
	m_input_tree = tree;
	if (doNotify)
	{
	  if (m_input_tree->GetNotify()) WARNING("Tree already had an object to notify");
	  m_input_tree->SetNotify(this);
	}
	m_tree->AddFriend(m_input_tree);
    }

    // tree changed (eg new file or first time InitTree is called)    
    if (m_debug) DEBUG("tree changed / new tree");
    
    
    // Loop over the formula map and instantiate the formulae
    INFO("Instantiating formulae");
    std::map< TString, egammaMVACalib::VarFormula >::iterator formulaIt;
    for (formulaIt = m_formulae.begin(); formulaIt != m_formulae.end(); ++formulaIt)
    {
	TString varName(formulaIt->first);
	egammaMVACalib::VarFormula *varFormula = &(formulaIt->second);
	// TODO: delete old formula
	varFormula->formula = defineFormula(varName, varFormula->expression, m_tree);
	
	if (!formulaIt->second.formula)
	{
	    FATAL( "Could not define formula" );
	    assert(false);
	}
    }
    
    // Instantiate the formulae for the shifts
    std::map< egammaMVACalib::ReaderID, egammaMVACalib::AdditionalInfoMap>::iterator it;
    for (it = m_additional_infos.begin(); it != m_additional_infos.end(); ++it)
	defineShiftFormula(it->first);
    
    // Instantiate the formula for cluster energy
    defineClusterEnergyFormula();
}

TTreeFormula* egammaMVACalib::defineFormula(const TString & varName, const TString & expression, TTree *tree)
{
    // Define and check the formula
    TTreeFormula* formula = new TTreeFormula(varName.Data(), expression.Data(), tree);
    if (!formula->GetNdim())
    {
	FATAL( "(defineFormula) : Invalid formula: " << varName << " := " <<
	       expression <<  " (probably branch not present)" );
	assert(false);
    }
    if (m_debug) DEBUG("Formula: " << formula->GetName() << " := " << formula->GetTitle());
    
    return formula;
}


/** Set the variables for the reader, copying the results from the formulae **/
void egammaMVACalib::LoadVariables(int index ) 
{
    if (m_debug) DEBUG("LoadVariables(" << index << ")");
    std::map< TString, egammaMVACalib::VarFormula >::iterator formulaIt;
    for (formulaIt = m_formulae.begin(); formulaIt != m_formulae.end(); ++formulaIt)
    {
	egammaMVACalib::VarFormula *varFormula = &(formulaIt->second);
	assert(varFormula);
	if (!varFormula->external) {
	    assert(varFormula->formula);
	    varFormula->formula->GetNdata(); // Avoids getting wrong values...
	    if (m_debug) DEBUG("evaluating formula " << varFormula->expression 
			       << " TTreeFormula::Title=" << varFormula->formula->GetTitle() 
			       << " for index " << index);
	    varFormula->variable = varFormula->formula->EvalInstance(index);
	}
	if (m_debug) DEBUG("Value of " << formulaIt->first << " : " << varFormula->variable);
    }
}

float* egammaMVACalib::getAddress(const TString & input_name)
{
    std::map< TString, egammaMVACalib::VarFormula >::iterator it = m_formulae.find(input_name);
    if (it == m_formulae.end()) {
	std::string formulae_string = "";
	for (std::map< TString, egammaMVACalib::VarFormula >::iterator it = m_formulae.begin();
	     it != m_formulae.end(); ++it) {
	    formulae_string += std::string(" ") + it->first + std::string(" ");
	}
	FATAL("<getAddress> formula " << input_name << " not found. Should be one of: " << formulae_string);
	assert(true);
	return 0;
    }
    return &(it->second.variable);
}

void egammaMVACalib::setExternal(const TString & input_name)
{
    std::map< TString, egammaMVACalib::VarFormula >::iterator it = m_formulae.find(input_name);
    if (it == m_formulae.end()) {
	std::string formulae_string = "";
	for (std::map< TString, egammaMVACalib::VarFormula >::iterator it = m_formulae.begin();
	     it != m_formulae.end(); ++it) {
	    formulae_string += std::string(" ") + it->first + std::string(" ");
	}
	FATAL("<setExternal> formula " << input_name << " not found. Should be one of: " << formulae_string);
	assert(true);
	return;
    }
    it->second.external = true;
}

void egammaMVACalib::printValueInput()
{
    std::map< TString, egammaMVACalib::VarFormula >::iterator formulaIt;
    INFO("information about last evaluation");
    for (formulaIt = m_formulae.begin(); formulaIt != m_formulae.end(); ++formulaIt)
    {
	egammaMVACalib::VarFormula *varFormula = &(formulaIt->second);
	assert(varFormula);
	assert(varFormula->formula);
	INFO("formula: " << formulaIt->first
	     << "\n   expression: " << varFormula->expression 
	     << "\n   TTreeFormula::Title=" << varFormula->formula->GetTitle() 
	     << "\n    value: " << varFormula->variable);
    }
}

int egammaMVACalib::getNdata()
{
    TTreeFormula *testFormula = m_formulae.begin()->second.formula;
    if (!testFormula || testFormula->GetNdim() == 0)
    {
	FATAL("(GetNdata) Could not get TTreeFormula, returning 0");
	return 0;
    }
    return testFormula->GetNdata();
}


TTree* egammaMVACalib::getMVAResponseTree(TTree *tree, int Nentries, TString branchName,
					     TString copyBranches, int first_event, bool flatten, int update)
{
    if (!tree)
    {
	WARNING( "Null pointer to tree" );
	return 0;
    }
    
    InitTree(tree);
    
    // Choose a TTreeFormula to define the number of instances per entry
    // (and define if we have vectors...)
    TTreeFormula *testFormula = m_formulae.begin()->second.formula;
    if (!testFormula || testFormula->GetNdim() == 0)
    {
	FATAL("(getMVAResponseTree) Could not get TTreeFormula for testing, aborting");
	assert(false);
	return 0;
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
    INFO("*** Entries to process: " << Nentries << " ***");
    for (int ientry=first_event; ientry < Nentries + first_event; ++ientry)
    {
	if (m_debug) DEBUG("Processing entry " << ientry );
	if (ientry%update == 0) INFO("Processing entry " << ientry );
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
		mvaVector.push_back( mvaOutput );
	}
	if (useVector) mvaTree->Fill();
    }
    INFO("*** Processing done: " << Nentries << " written ***");
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
    if (m_debug)
    {
	stringstream ss;
	ss << "index = " << index << " / particle = " << getParticleType();
	if (m_eta) ss << " / eta = " << *m_eta;
	if (m_energy) ss << " / energy = " << *m_energy;
	if (!reader)
	    DEBUG( "-- No reader found, " << ss.str() );
	else
	    DEBUG( "-- Found reader, " << ss.str() );
    }
    
    if (!reader)
	return 0;
    return reader->EvaluateRegression( fMethodName.Data() )[0];
}

float egammaMVACalib::getMVAEnergyPhoton(float ph_rawcl_Es0,
					 float ph_rawcl_Es1,
					 float ph_rawcl_Es2,
					 float ph_rawcl_Es3,
					 float ph_cl_eta,
					 float ph_cl_E,
					 float ph_cl_etaCalo,
					 float ph_cl_phiCalo,
					 float ph_ptconv,
					 float ph_pt1conv,
					 float ph_pt2conv,
					 int ph_convtrk1nPixHits,
					 int ph_convtrk1nSCTHits,
					 int ph_convtrk2nPixHits,
					 int ph_convtrk2nSCTHits,
					 float ph_Rconv)
{
    m_ph_rawcl_Es0 = ph_rawcl_Es0;
    m_ph_rawcl_Es1 = ph_rawcl_Es1;
    m_ph_rawcl_Es2 = ph_rawcl_Es2;
    m_ph_rawcl_Es3 = ph_rawcl_Es3;
    m_ph_cl_etaCalo = ph_cl_etaCalo;
    m_ph_cl_phiCalo = ph_cl_phiCalo;
    m_ph_cl_eta = ph_cl_eta;
    m_ph_cl_E = ph_cl_E;
    m_ph_ptconv = ph_ptconv;
    m_ph_pt1conv = ph_pt1conv;
    m_ph_pt2conv = ph_pt2conv;
    m_ph_convtrk1nPixHits = ph_convtrk1nPixHits;
    m_ph_convtrk1nSCTHits = ph_convtrk1nSCTHits;
    m_ph_convtrk2nPixHits = ph_convtrk2nPixHits;
    m_ph_convtrk2nSCTHits = ph_convtrk2nSCTHits;
    m_ph_rawcl_calibHitsShowerDepth = get_shower_depth(
	ph_cl_eta, 
        ph_rawcl_Es0, 
        ph_rawcl_Es1,
        ph_rawcl_Es2,
        ph_rawcl_Es3);
    m_ph_Rconv = ph_Rconv;
    return getMVAEnergy();
}


float egammaMVACalib::getMVAEnergyElectron(float el_rawcl_Es0,
					   float el_rawcl_Es1,
					   float el_rawcl_Es2,
					   float el_rawcl_Es3,
					   float el_cl_eta,
					   float el_cl_E,
					   float el_cl_etaCalo,
					   float el_cl_phiCalo)
{
    m_el_rawcl_Es0 = el_rawcl_Es0;
    m_el_rawcl_Es1 = el_rawcl_Es1;
    m_el_rawcl_Es2 = el_rawcl_Es2;
    m_el_rawcl_Es3 = el_rawcl_Es3;
    m_el_cl_eta = el_cl_eta;
    m_el_cl_E = el_cl_E;
    m_el_cl_etaCalo = el_cl_etaCalo;
    m_el_cl_phiCalo = el_cl_phiCalo;
    m_el_rawcl_calibHitsShowerDepth = get_shower_depth(
        el_cl_eta, 
        el_rawcl_Es0, 
        el_rawcl_Es1,
        el_rawcl_Es2,
        el_rawcl_Es3);
    return getMVAEnergy();
}



float egammaMVACalib::getMVAEnergy(int index /* = 0 */,
				   ShiftType shift_type /* =NSHIFTCORRECTIONS */)
{
    m_mvaOutput = getMVAOutput(index);
    if (m_debug) DEBUG("MVA = " << m_mvaOutput);
    if (m_mvaOutput == 0.) 
    {
	if (m_clusterEif0 && m_clusterFormula)
	{
	    m_clusterFormula->GetNdata(); // FIXME: seems this is needed to prevent 0s...
	    float clusterE = m_clusterFormula->EvalInstance(index);
	    if (m_debug) DEBUG("Returning cluster E: " << clusterE );
	    return clusterE;
	}
	return 0.;
    }
    float energy = 0;
    if (m_calibrationType == fullCalibration)
	energy = m_mvaOutput;
    else if (m_initialEnergy)
	energy = (*m_initialEnergy) * m_mvaOutput;
    
    ShiftType shift_to_use = m_shiftType;
    if (shift_type != NSHIFTCORRECTIONS) shift_to_use = shift_type;
    
    egammaMVACalib::ReaderID key = getReaderID();
    float shift = getShift(energy / cosh(*m_eta), key, shift_to_use);
    if (shift > 0.5)
	    return energy / shift;
	  WARNING("Shift not applied (too small or negative value?)");
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
	INFO("Trying to define formula for initial energy..." );
	TString expr = (m_egammaType == egPHOTON) ? 
	    "ph_rawcl_Es1 + ph_rawcl_Es2 + ph_rawcl_Es3" :
	    "el_rawcl_Es1 + el_rawcl_Es2 + el_rawcl_Es3" ;
	
	predefineFormula("initEnergy", expr, "other", "F");
	it = m_formulae.find("initEnergy");
    }
    
    if (it != m_formulae.end())
	m_initialEnergy = &(it->second.variable);
    else
    {
	WARNING( "Could not define formula for initial energy" );
    }
    // TODO: define formula
}


vector<egammaMVACalib::XmlVariableInfo>
egammaMVACalib::parseXml(const TString & xml_filename)
{
    vector<egammaMVACalib::XmlVariableInfo> result;
    
    TXMLEngine xml;
    XMLDocPointer_t xmldoc = xml.ParseFile(xml_filename);
    if (!xmldoc) {
	FATAL("file not found " << xml_filename << " current directory is: " << gSystem->WorkingDirectory());
	return result;
    }
    XMLNodePointer_t mainnode = xml.DocGetRootElement(xmldoc);
    
    // loop to find <Variables> and <Spectators>
    XMLNodePointer_t node = xml.GetChild(mainnode);
    while (node)
    {
	node = xml.GetNext(node);
	TString nodeName = xml.GetNodeName(node);
	if (nodeName == "Variables" || nodeName == "Spectators") {
	    vector<egammaMVACalib::XmlVariableInfo> r = parseVariables(&xml, node, nodeName);
	    result.insert(result.end(), r.begin(), r.end());
	}
    }
    xml.FreeDoc(xmldoc);
    return result;
}

vector<egammaMVACalib::XmlVariableInfo>
egammaMVACalib::parseVariables(TXMLEngine *xml, void* node, const TString & nodeName)
{
    vector<egammaMVACalib::XmlVariableInfo> result;
    if (!xml || !node) return result;
    
    // loop over all children inside <Variables> or <Spectators>
    for (XMLNodePointer_t info_node = xml->GetChild(node); info_node != 0; 
	 info_node = xml->GetNext(info_node) )
    {
	XMLAttrPointer_t attr = xml->GetFirstAttr(info_node);
	XmlVariableInfo o;
	// loop over the attributes of each child
	while (attr!=0) 
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
	//    	DEBUG("Expression: " << expression << " Label: " << label << " varType: " << varType);
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
        o->Add( new TObjString(varInfo[i].label) );
    return o;
}

TString* egammaMVACalib::getVariables(TMVA::Reader *reader)
{
    TString *str = new TString();		
    std::vector<TMVA::VariableInfo>::const_iterator it;
    for (it = reader->DataInfo().GetVariableInfos().begin(); it != reader->DataInfo().GetVariableInfos().end(); ++it)
        str->Append( TString(str->Length() ? "," : "") + it->GetLabel() );
    
    return str;
}


egammaMVACalib::ParticleType egammaMVACalib::getParticleType( const TString & s )
{
    if (s == "unconvertedPhoton")
	return UNCONVERTED;
    if (s == "convertedPhoton")
	return CONVERTED;
    if (s == "convertedSiSiPhoton")
	return SiSi;
    if (s == "electron")
	return ELECTRON;
    
    WARNING("Invalid particle name " << s);
    return INVALIDPARTICLE;
}

void egammaMVACalib::printReadersInfo() const
{
    
    INFO(getNreaders() << " readers created" );
    TAxis* fAxisEta = m_hPoly->GetXaxis(); // TODO: ???
    TAxis* fAxisEnergy = (m_binMultiplicity > 1 ? m_hPoly->GetYaxis() : 0);
    
    if (fAxisEta)
	INFO( fAxisEta->GetNbins() << " eta bin(s) -- (" 
	      << fAxisEta->GetXmin() + 1e-10 
	      << " < abs(" << m_etaVar << ") < " << fAxisEta->GetXmax() << ")" );
    if (fAxisEnergy)
	INFO( fAxisEnergy->GetNbins() << " energy bin(s) --  (" 
	      << fAxisEnergy->GetXmin() + 1e-10
	      << " < " << m_energyVar << " < " << fAxisEnergy->GetXmax() << " GeV)" );
}

void egammaMVACalib::printFormulae() const
{
    std::map< TString, egammaMVACalib::VarFormula >::const_iterator it;
    for (it = m_formulae.begin(); it !=  m_formulae.end(); ++it)
    {
	cout << it->first << ": ";
	if (it->second.formula) cout << it->second.formula->GetTitle();
	cout << endl;
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
	WARNING("(getListOfBranches) No tree defined");
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
	if (br && !branches->FindObject(br) )
	    branches->Add( br );
    }
}

void egammaMVACalib::activateBranches()
{
    TBranch *branch;
    TIter next( getListOfBranches() );
    while ((branch = (TBranch*) next())) {
	INFO("activating " << branch->GetName());
	m_input_tree->SetBranchStatus(branch->GetName(), true); 
    }
    // WARNING: TBranch::SetStatus does not work for chains
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
    TIter next( copyBranches.Tokenize(",") );
    while ((sObj = (TObjString*) next()))
    {
	if (getString(sObj) == "input") // copy branches used to calculate MVA energy
	    activateBranches();
	else
	    m_input_tree->SetBranchStatus(getString(sObj), 1);
    }
    
    //TTree *mvaTree = new TTree(fMethodName.Data(), Form("%s tree", fMethodName.Data())); 
    TTree *mvaTree = m_input_tree->CloneTree(0);
    //mvaTree->AddFriend( m_input_tree );
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
	FATAL( "(checkBin) Could not get bin " << bin 
	       << ". Size: " << m_hPoly->GetBins()->GetEntries() );
	return false;
    }
    
    if (!TMath::AreEqualAbs(polyBin->GetXMin(), etaMin, 1e-9) ||
	!TMath::AreEqualAbs(polyBin->GetXMax(), etaMax, 1e-9) ||
	!TMath::AreEqualAbs(polyBin->GetYMin(), energyMin, 1e-9) ||
	!TMath::AreEqualAbs(polyBin->GetYMax(), energyMax, 1e-9) )
    {
	FATAL( "(checkBin) Bins do not coincide:" );
	DEBUG("Previous bin xmin: " << polyBin->GetXMin() << " / new: " << etaMin);
	DEBUG("Previous bin xmax: " << polyBin->GetXMax() << " / new: " << etaMax);
	DEBUG("Previous bin ymin: " << polyBin->GetYMin() << " / new: " << energyMin);
	DEBUG("Previous bin ymax: " << polyBin->GetYMax() << " / new: " << energyMax);      
	return false;
    }
    return true;
}

float egammaMVACalib::getShift(float Et, ReaderID key, ShiftType shift_type) const
{
    ShiftType shift_to_use = m_shiftType;
    if (shift_type != NSHIFTCORRECTIONS) shift_to_use = shift_type;
   
    egammaMVACalib::ShiftMap::const_iterator it;
    it = m_shiftMap.find( std::make_pair(key, shift_to_use) );
    if (it == m_shiftMap.end() || !it->second) return 1;
    float shift = it->second->Eval( Et / 1e3 );
    if (m_debug) DEBUG("Shift (Et = " << Et/1e3 << "): " << shift);
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
// 	if (m_debug) DEBUG("Defining shift for bin " << key.bin << ": " << expr);
TString name = Form("shift_%d_%d_%s", key.particleType, key.bin, shift_name.Data());
// 	TTreeFormula* formula = defineFormula(name, expr, m_tree);
	m_shiftMap[std::make_pair(key, shift)] = new TF1(name, expr);
    }
}

void egammaMVACalib::defineClusterEnergyFormula()
{
    if (m_debug) {
	DEBUG("setting cluster energy formula");
    }
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
    INFO("Replacements made: " << count);
}

/** Check if shower depth variable is present or if it needs to be calculated **/	
void egammaMVACalib::checkShowerDepth(TTree *tree)
{
		TString prefix = (m_egammaType == egPHOTON) ?  "ph_" : "el_";
		TString showerDepth(prefix);
		showerDepth.Append("rawcl_calibHitsShowerDepth");
		
		if (m_formulae.count(showerDepth) && !tree->GetBranch(showerDepth.Data()) )
		{
	INFO("Using CalibrationHitOffline to calculate shower depth");
	TString newShowerDepth = "egammaMVACalib::get_shower_depth(ph_cl_eta, ph_rawcl_Es0, ph_rawcl_Es1, ph_rawcl_Es2, ph_rawcl_Es3)";
	newShowerDepth.ReplaceAll("ph_", prefix);
	 m_formulae[showerDepth].expression = newShowerDepth;
		}
}
				
void egammaMVACalib::writeROOTfile(const TString& directory, int particle)
{
		if (!m_readers.size())
		{
	WARNING("No reader defined, not dumping ROOT file");
	return;
		}
		if (particle == INVALIDPARTICLE)
		{
	if	(m_egammaType == egPHOTON)
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
	
		TString particleName("electron");
		if (particle == UNCONVERTED) 
	particleName = "unconvertedPhoton";
		else if (particle == CONVERTED)
	particleName = "convertedPhoton";
			
		TString fileName = directory + TString("/MVACalib_") + particleName + TString(".weights.root");
		TFile *f = TFile::Open(fileName, "UPDATE");		
		TObjArray trees, variables, formulae, shifts;
		
		std::map< egammaMVACalib::ReaderID, TMVA::Reader* >::const_iterator itReader;
		for (itReader = m_readers.begin(); itReader != m_readers.end(); ++itReader)
		{
	if (itReader->first.particleType != particle) continue;
	// bin has an offset so index = bin - 1
	addReaderInfoToArrays(itReader->second, &trees, &variables, itReader->first.bin - 1);
		}
    
		// Expressions (formulae)
		map<TString, egammaMVACalib::VarFormula>::const_iterator it;
		for (it = m_formulae.begin(); it != m_formulae.end(); ++it)
	formulae.Add( new TNamed(it->first, it->second.expression));
		
		// Shifts
		for(ShiftMap::iterator itS = m_shiftMap.begin(); itS != m_shiftMap.end(); ++itS)
		{
	if (itS->first.first.particleType != particle || itS->first.second != MEAN10TOTRUE)
			continue;	
	shifts.Add( new TObjString( itS->second->GetTitle() ) );
		}
		
		// Write and close
		int option = (TObject::kSingleKey | TObject::kOverwrite);
		trees.Write("trees", option);
		variables.Write("variables", option);
		formulae.Write("formulae", option);
		shifts.Write("shifts", option);
		getTH2Poly()->Write(0, option);
		f->Close();
		INFO("Wrote ROOT file: " << fileName.Data() );

}

void egammaMVACalib::addReaderInfoToArrays(TMVA::Reader *reader,
					   TObjArray *trees,
					   TObjArray *variables,
					   int index)
{
    if (index < 0) index = variables->GetSize();
    TString *vars = getVariables(reader);
    assert (vars);
    
    TMVA::MethodBDT* tbdt = dynamic_cast<TMVA::MethodBDT*>(reader->FindMVA("BDTG"));
    assert (tbdt);
    BDT *bdt = new BDT(tbdt);
    TTree *tree = bdt->WriteTree();
    
    variables->AddAtAndExpand(new TObjString(*vars), index);
    trees->AddAtAndExpand( tree, index );
    delete bdt;
}

				
// helper function to compute shower depth
// copied and pasted from: 
// https://svnweb.cern.ch/trac/atlasoff/browser/Calorimeter/CaloClusterCorrection/trunk/src/CaloSwCalibHitsShowerDepth.cxx
// https://svnweb.cern.ch/trac/atlasoff/browser/Calorimeter/CaloClusterCorrection/trunk/python/CaloSwCalibHitsCalibration_v9leakdata.py#L2639

std::vector<double> egammaMVACalib::get_radius(double eta)
{
    eta = fabs(eta);
    vector<double> radius(4);
    if (eta / 0.025 < 57)
    {
	static const float 
	    X0IN = 1.6,          // 1.7 X0 before strips
	    X0S1 = 6.0,          //  6  X0 till end of strips
// 	    BEND1 = 152.1000,    // first 3 bends radii (cm)
// 	    BEND2 = 155.9659,
// 	    BEND3 = 159.7202,
	    ETASTEP = 0.8,       // where lead changes
// 	    DENSHI = 2.137,      // cm/X0 high density 
// 	    DENSLO = 2.695,      // cm/X0  low density
	    X0THI = 23.74,       // total depth high density part (not indep?)
	    X0TLO = 19.177;      // total depth  low density part (not indep?)
  
  float X0T;  
// 	float DENS, X0T, RLO, RHI ;
	float R12, R23, R0, R1, R2, R3;

	if (eta < ETASTEP) {
// 	    DENS = DENSHI;
	    X0T = X0THI;
// 	    RLO = BEND2;             // break strips betw bends 2 and 3
// 	    RHI = BEND3;
	}
	else {
// 	    DENS = DENSLO;
	    X0T = X0TLO;
// 	    RLO = BEND1;             // break strips betw bends 1 and 2
// 	    RHI = BEND2;
	}
	
	R0 = (X0IN) * cosh(eta);
	
	//..   actual radius where S1 should end
	
	R12 = X0S1;
    
	//..   total depth of calo at given eta
	
	float RADLTOT = X0T*cosh(eta);
	
	
	if (eta < 0.5){
	    R23 = 22 * cosh(eta) ;
	}else{
	    R23 = 25;
	}
	
	R1 = R0  + (R12 - R0)/2 ; 
	R2 = R12 + (R23 - R12)/2;
	R3 = R23 + (RADLTOT - R23)/2; 
	
	radius[0] = R0;
	radius[1] = R1;
	radius[2] = R2;
	radius[3] = R3;
    }
    else
    {
	double x0=0., x1=0., x2=0., x3=0.;
    
	float abs_ETA = TMath::Abs(eta);

	// TODO: find a better solution from cell 60, 61 (first two)
	if(abs_ETA>1.500 && abs_ETA<=1.525) {x0=0.; x1=8.85762; x2=19.5168; x3=30.0369;}       // COPIED FROM 62 !!!
	else if(abs_ETA>1.525 && abs_ETA<=1.550) {x0=0.; x1=8.85762; x2=19.5168; x3=30.0369;}  // COPIED FROM 62 !!!
	else if(abs_ETA>1.550 && abs_ETA<=1.575) {x0=0.; x1=8.85762; x2=19.5168; x3=30.0369;}
	else if(abs_ETA>1.575 && abs_ETA<=1.600) {x0=0.; x1=7.24613; x2=17.9683; x3=28.5757;}
	else if(abs_ETA>1.600 && abs_ETA<=1.625) {x0=0.; x1=7.11249; x2=17.9466; x3=28.6607;}
	else if(abs_ETA>1.625 && abs_ETA<=1.650) {x0=0.; x1=7.03737; x2=17.7263; x3=28.4004;}
	else if(abs_ETA>1.650 && abs_ETA<=1.675) {x0=0.; x1=7.44517; x2=18.1864; x3=28.9547;}
	else if(abs_ETA>1.675 && abs_ETA<=1.700) {x0=0.; x1=7.74611; x2=18.5419; x3=29.3555;}
	else if(abs_ETA>1.700 && abs_ETA<=1.725) {x0=0.; x1=8.06184; x2=18.8982; x3=29.8214;}
	else if(abs_ETA>1.725 && abs_ETA<=1.750) {x0=0.; x1=6.90503; x2=17.8396; x3=28.7947;}
	else if(abs_ETA>1.750 && abs_ETA<=1.775) {x0=0.; x1=6.53893; x2=17.5996; x3=28.7122;}
	else if(abs_ETA>1.775 && abs_ETA<=1.800) {x0=0.; x1=6.42897; x2=17.1062; x3=28.3578;}
	else if(abs_ETA>1.800 && abs_ETA<=1.825) {x0=0.; x1=6.41139; x2=17.1382; x3=28.4191;}
	else if(abs_ETA>1.825 && abs_ETA<=1.850) {x0=0.; x1=5.72471; x2=16.4543; x3=27.8796;}
	else if(abs_ETA>1.850 && abs_ETA<=1.875) {x0=0.; x1=5.19018; x2=16.1606; x3=27.7097;}
	else if(abs_ETA>1.875 && abs_ETA<=1.900) {x0=0.; x1=5.04008; x2=16.1174; x3=27.7649;}
	else if(abs_ETA>1.900 && abs_ETA<=1.925) {x0=0.; x1=5.01225; x2=16.184;  x3=27.9175;}
	else if(abs_ETA>1.925 && abs_ETA<=1.950) {x0=0.; x1=4.95996; x2=16.2482; x3=28.1358;}
	else if(abs_ETA>1.950 && abs_ETA<=1.975) {x0=0.; x1=4.98242; x2=16.3591; x3=28.3336;}
	else if(abs_ETA>1.975 && abs_ETA<=2.000) {x0=0.; x1=5.04856; x2=16.4194; x3=28.5289;}
	else if(abs_ETA>2.000 && abs_ETA<=2.025) {x0=0.; x1=5.32584; x2=16.8194; x3=29.0619;}
	else if(abs_ETA>2.025 && abs_ETA<=2.050) {x0=0.; x1=5.2971;  x2=16.8203; x3=29.1545;}
	else if(abs_ETA>2.050 && abs_ETA<=2.075) {x0=0.; x1=5.41049; x2=17.0891; x3=29.5856;}
	else if(abs_ETA>2.075 && abs_ETA<=2.100) {x0=0.; x1=5.48954; x2=17.2965; x3=29.9571;}
	else if(abs_ETA>2.100 && abs_ETA<=2.125) {x0=0.; x1=5.50876; x2=16.8573; x3=29.6199;}
	else if(abs_ETA>2.125 && abs_ETA<=2.150) {x0=0.; x1=5.45123; x2=16.885;  x3=29.8321;}
	else if(abs_ETA>2.150 && abs_ETA<=2.175) {x0=0.; x1=5.54906; x2=17.1228; x3=30.2044;}
	else if(abs_ETA>2.175 && abs_ETA<=2.200) {x0=0.; x1=5.53662; x2=17.1833; x3=30.4852;}
	else if(abs_ETA>2.200 && abs_ETA<=2.225) {x0=0.; x1=5.46801; x2=17.1857; x3=30.5842;}
	else if(abs_ETA>2.225 && abs_ETA<=2.250) {x0=0.; x1=5.45581; x2=16.6621; x3=30.2958;}
	else if(abs_ETA>2.250 && abs_ETA<=2.275) {x0=0.; x1=5.46682; x2=16.7899; x3=30.5222;}
	else if(abs_ETA>2.275 && abs_ETA<=2.300) {x0=0.; x1=5.52693; x2=16.9916; x3=30.9557;}
	else if(abs_ETA>2.300 && abs_ETA<=2.325) {x0=0.; x1=5.53211; x2=17.0801; x3=31.1637;}
	else if(abs_ETA>2.325 && abs_ETA<=2.350) {x0=0.; x1=4.79919; x2=16.4387; x3=31.5318;}
	else if(abs_ETA>2.350 && abs_ETA<=2.375) {x0=0.; x1=4.80101; x2=16.5808; x3=31.8533;}
	else if(abs_ETA>2.375 && abs_ETA<=2.400) {x0=0.; x1=4.77726; x2=15.9355; x3=31.4454;}
	else if(abs_ETA>2.400 && abs_ETA<=2.425) {x0=0.; x1=4.76845; x2=16.0665; x3=31.8002;}
	else if(abs_ETA>2.425 && abs_ETA<=2.450) {x0=0.; x1=4.5959;  x2=15.9977; x3=32.1175;}
	else if(abs_ETA>2.450 && abs_ETA<=2.475) {x0=0.; x1=4.68513; x2=16.2385; x3=32.5042;}
	else if(abs_ETA>2.475 && abs_ETA<=2.500) {x0=0.; x1=4.7232;  x2=16.4144; x3=33.028;}
  
	radius[0] = x0;
	radius[1] = x1;
	radius[2] = x2;
	radius[3] = x3;
    }
    return radius;
}

double egammaMVACalib::get_shower_depth(double eta,
					double raw_cl_0,
					double raw_cl_1,
					double raw_cl_2,
					double raw_cl_3)
{
    vector<double> radius = get_radius(eta);
    
    const double denominator = raw_cl_0 + raw_cl_1 + raw_cl_2 + raw_cl_3;
    if (denominator == 0) return 0.;

    return ( radius[0] * raw_cl_0
	     + radius[1] * raw_cl_1
	     + radius[2] * raw_cl_2
	     + radius[3] * raw_cl_3 ) / denominator;
}


TMVA::Reader* egammaMVACalib::getDummyReader(const TString &xmlFileName)
{
		float dummyFloat;
		TMVA::Reader *reader = new TMVA::Reader("Silent");
		
    vector<egammaMVACalib::XmlVariableInfo> variable_infos = parseXml(xmlFileName);
    for (vector<egammaMVACalib::XmlVariableInfo>::const_iterator itvar = variable_infos.begin();
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
	    reader->AddVariable( varDefinition, &dummyFloat );
	else if (infoType == "spectator")
	    reader->AddSpectator( varDefinition, &dummyFloat);
	else // should never happen
	{
	    FATAL( "(setupReader) : Unknown type from parser " << infoType );
	    assert(false);
	}
    }   
		
	reader->BookMVA("BDTG", xmlFileName);
	return reader;		
}

bool egammaMVACalib::Notify()
{
    if (m_debug) DEBUG("Notify called");
    
    // Update the formulae defined in m_formulae
    std::map< TString, egammaMVACalib::VarFormula >::iterator formulaIt;
    for (formulaIt = m_formulae.begin(); formulaIt != m_formulae.end(); ++formulaIt) {
	if (m_debug) DEBUG("updating address for variable " << formulaIt->first);
	checkFormula(formulaIt->second.formula);
    }
    
    // Update the formulae defined in m_shiftMap
//     if (m_debug) DEBUG("Updating shifts");
//     ShiftMap::iterator shiftIt;
//     for (shiftIt = m_shiftMap.begin(); shiftIt != m_shiftMap.end(); ++shiftIt) {
// 	checkFormula(shiftIt->second);
//     }
    
    // Update cluster formula
    if (m_clusterFormula)
	checkFormula(m_clusterFormula);
    
    if (m_debug) DEBUG("Notify done");
    return true;
}
