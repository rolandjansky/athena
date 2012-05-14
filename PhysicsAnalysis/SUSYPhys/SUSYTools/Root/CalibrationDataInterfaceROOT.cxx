/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataInterfaceROOT.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SUSYTools/CalibrationDataInterfaceROOT.h"

#include "TF1.h"
#include "TMatrixT.h"
#include "TMatrixDSym.h"
#include "TMath.h"
#include "TEnv.h"
#include "TFile.h"
#include <iostream>

// This constant denotes the array dimension for the function arguments
const int maxdim = 6;

using std::string;
using std::cout;
using std::cerr;
using std::endl;

//================ Constructor =================================================

SUSY::CalibrationDataInterfaceROOT::CalibrationDataInterfaceROOT(const string& taggerName, string configname, string pathname)
{
  m_taggerName = taggerName;

  TEnv env(configname.c_str());

  // ROOT file containing the calibrations
  TString filename = env.GetValue("File", "BTaggingPerformanceCalibrations.root");
  TString filenameEff = env.GetValue("FileEff", "");
  TString filenameSF = env.GetValue("FileSF", "");
  if (filenameEff == "") {
    filenameEff = pathname + filename;
  }
  if (filenameSF == "") {
    filenameSF = pathname + filename;
  }

  cout << "=== CalibrationDataInterfaceROOT::CalibrationDataInterfaceROOT ===" << endl;
  cout << " Config name          : " << configname.c_str() << endl;
  cout << " taggerName           : " << taggerName.c_str() << endl;
  cout << " Efficiency file name : " << filenameEff.Data() << endl 
       << " SF         file name : " << filenameSF.Data() << endl
       << endl;
  // cout << "     : " << << endl;

  m_fileEff = new TFile(filenameEff.Data(), "READ");
  if (filenameEff == filenameSF)
    m_fileSF = m_fileEff;
  else
    m_fileSF = new TFile(filenameSF.Data(), "READ");
  
  // Calibration names for the efficiencies
  string testPrefix(taggerName); testPrefix += ".";
  string test(testPrefix); test += "EfficiencyCalibrationBName";
  string calibrationBName(env.GetValue(test.c_str(), "default"));
  test = testPrefix; test += "EfficiencyCalibrationCName";
  string calibrationCName(env.GetValue(test.c_str(), "default"));
  test = testPrefix; test += "EfficiencyCalibrationLightName";
  string calibrationLightName(env.GetValue(test.c_str(), "default"));

  // insert the calibration names into a common object
  std::map<string, string> names;
  names["B"] = calibrationBName;
  names["C"] = calibrationCName;
  names["Light"] = calibrationLightName;
  setEffCalibrationNames(names);

  // Calibration names for the efficiency scale factors
  test = testPrefix; test += "ScaleFactorCalibrationBName";
  calibrationBName = env.GetValue(test.c_str(), "default");
  test = testPrefix; test += "ScaleFactorCalibrationCName";
  calibrationCName = env.GetValue(test.c_str(), "default");
  test = testPrefix; test += "ScaleFactorCalibrationLightName";
  calibrationLightName = env.GetValue(test.c_str(), "default");

  // insert the calibration names into a common object
  names["B"] = calibrationBName;
  names["C"] = calibrationCName;
  names["Light"] = calibrationLightName;
  setSFCalibrationNames(names);

  // Since TEnv doesn't allow for straight retrieval of vectors of strings, expect
  // semicolon-separated entries (semicolon because ROOT considers this as a "special"
  // token anyway in object names).
  test = testPrefix; test += "operatingPoints";
  string OP(env.GetValue(test.c_str(), ""));
  string::size_type end;
  if (OP.size() > 0) {
    do {
      end = OP.find(";");
      m_operatingPoints.push_back(OP.substr(0,end));
      if (end != string::npos) OP = OP.substr(end+1);
    } while (end != string::npos);
  }

  // Do the same thing for the aliases. Don't prefix this since the aliases are
  // common to all taggers (even if they are read again for each tagger).
  string AL(env.GetValue("aliases", ""));
  if (AL.size() > 0) {
    do {
      end = AL.find(";");
      string alias = AL.substr(0, end);
      // Each alias specification uses an arrow ("->"). Forget about entries
      // not properly following this specification.
      // NB: TEnv imposes a maximum string length of 1024 characters -- is this a problem?
      string::size_type arrow = alias.find("->");
      if (arrow == string::npos) continue;
      m_aliases[alias.substr(0,arrow)] = alias.substr(arrow+2);
      if (end != string::npos) AL = AL.substr(end+1);
    } while (end != string::npos);
  }

}

SUSY::CalibrationDataInterfaceROOT::CalibrationDataInterfaceROOT() //For PROOF
{
	m_fileEff=0;
	m_fileSF=0;
}

//================ Destructor =================================================

SUSY::CalibrationDataInterfaceROOT::~CalibrationDataInterfaceROOT()
{
	if((m_fileEff!=0)&&(m_fileSF!=0)) {
		if(m_fileEff==m_fileSF){
    		m_fileEff->Close();  
			delete m_fileEff;m_fileEff=0;
  		}
  		else{
    		m_fileEff->Close();
    		m_fileSF->Close();
    		delete m_fileEff;m_fileEff=0;
    		delete m_fileSF;m_fileSF=0;
  		}
	}

	// delete also the stored objects (these are owned by us)
	for (std::map<string,TObject*>::iterator it = m_objects.begin();
		it != m_objects.end(); ++it) {
		if (it->second) delete it->second;
	}
}

//====================== efficiency scale factor retrieval =============================

SUSY::CalibResult
SUSY::CalibrationDataInterfaceROOT::getScaleFactor (const CalibrationDataVariables& variables,
							const string& label,
							const string& OP, Uncertainty unc) const
{
  // // For now, a calibration for the charm efficiency scale factor is assumed not to exist
  // if (label == "C") return getScaleFactor(variables, "B", OP, unc);

  // for light-flavour jets, rename from "N/A"
  string flavour(label);
  if (flavour == "N/A") flavour = "Light";

  // handle aliases here
  string author = getAlias(variables.jetAuthor);

  Double_t x[maxdim], xsyst[maxdim];

  string sfName(getBasename(OP, flavour, "_SF"));
  string suffix("_"); suffix += m_taggerName; suffix += "_SF";
  string fctName(getBasename(OP, flavour, suffix));

  /* The functional arguments are cached for faster computation. */

  // Return a dummy result if the object is not found
  TF1* fct = dynamic_cast<TF1*>(retrieveTObject(fullName(author, sfName), false));
  if (! fct) {
    cerr << "getScaleFactor: unable to find SF calibration for object with "
	 << "tagger/jetCollection/flavour/operating point = "
	 << m_taggerName << "/" << author << "/" << flavour << "/" << OP << endl;
    return SUSY::dummyResult;
  }
  if (! existFunctionArguments(fctName))
    computeFunctionArguments(fctName, fct->GetTitle());
  retrieveFunctionArguments(variables, fctName, x);

  TMatrixDSym* cov = 0;
  if (unc == Total || unc == Statistical) {
    string sfStatName(sfName);
    sfStatName += "_stat";
    cov = dynamic_cast<TMatrixDSym*>(retrieveTObject(fullName(author, sfStatName), false));
    if (! cov) 
      cerr << "getScaleFactor: error retrieving Scale factor parameter covariance matrix!"
	   << endl;
  }
  TF1* fctsys = 0;
  if (unc == Total || unc == Systematic) {
    //    cout << "btag Calib DEBUG: Preparing syst..." << endl;
    string sfSystName(sfName);
    sfSystName += "_syst";
    string fctSystName(fctName);
    fctSystName += "_syst";
    fctsys = dynamic_cast<TF1*>(retrieveTObject(fullName(author, sfSystName), false));
    if (! existFunctionArguments(fctSystName))
      computeFunctionArguments(fctSystName, fctsys->GetTitle());
    retrieveFunctionArguments(variables, fctSystName, xsyst);
  }

  /*
    cout << "btag Calib DEBUG: fct=" << fct
    << " fctsys=" << fctsys 
    << " cov=" << cov
    << endl;
  */

  SUSY::CalibResult result = getResults(x, xsyst, fct, fctsys, cov);
  result.first = std::max(0., result.first);
  if (TMath::Abs(result.first) < SUSY::CalibZERO)
    result.first = 1.;
  return result;

}

//====================== "MC" efficiency retrieval ======================================

SUSY::CalibResult
SUSY::CalibrationDataInterfaceROOT::getMCEfficiency (const CalibrationDataVariables& variables,
							 const string& label,
							 const string& OP, Uncertainty unc) const
{
  // extract the relevant jet quantities: kinematic variables and jet author

  // for light-flavour jets, rename from "N/A"
  string flavour(label);
  if (flavour == "N/A") flavour = "Light";

  // handle aliases here
  string author = getAlias(variables.jetAuthor);

  Double_t x[maxdim], xsyst[maxdim];

  string effName(getBasename(OP, flavour, "_Eff", false));
  string suffix("_"); suffix += m_taggerName; suffix += "_Eff";
  string fctName(getBasename(OP, flavour, suffix, false));

  /* The functional arguments are cached for faster computation. */

  // Return a dummy result if the object is not found
  TF1* fct = dynamic_cast<TF1*>(retrieveTObject(fullName(author, effName), true));
  if (! fct) {
    cerr << "getMCEfficiency: unable to find Eff calibration for object with "
	 << "tagger/jetCollection/flavour/operating point = "
	 << m_taggerName << "/" << author << "/" << flavour << "/" << OP << endl;
    return SUSY::dummyResult;
  }
  if (! existFunctionArguments(fctName))
    computeFunctionArguments(fctName, fct->GetTitle());
  retrieveFunctionArguments(variables, fctName, x);
  
  // No further checks are carried out of the existence of the parameter covariance matrix
  // and parametrisation of the systematic uncertainty

  TMatrixDSym* cov = 0;
  if (unc == Total || unc == Statistical) {
    string effStatName(effName);
    effStatName += "_stat";
    cov = dynamic_cast<TMatrixDSym*>(retrieveTObject(fullName(author, effStatName), true));
    if (! cov)
      cerr << "getMCEfficiency: error retrieving Efficiency parameter covariance matrix!"
	   << endl;
  }
  TF1* fctsys = 0;
  if (unc == Total || unc == Systematic) {
    string effSystName(effName);
    effSystName += "_syst";
    string fctSystName(fctName);
    fctSystName += "_syst";
    fctsys = dynamic_cast<TF1*>(retrieveTObject(fullName(author, effSystName), true));
    if (! existFunctionArguments(fctSystName))
      computeFunctionArguments(fctSystName, fctsys->GetTitle());
    retrieveFunctionArguments(variables, fctSystName, xsyst);
    if (!fctsys) {
      cerr << "getEfficiency: unable to find Eff syst " << fctSystName.c_str()
	   << " for object with " 
	   << "tagger/jetCollection/flavour/operating point = " 
	   << m_taggerName << "/" << author << "/" << flavour << "/" << OP << endl; 
     }
  }

  /*
    cout << "Will use fct " << fct -> GetName() << endl;
    for (int i = 0; i < maxdim; ++i)
    cout << " x[" << i << "]" << x[i] << endl;
  */

  SUSY::CalibResult result = getResults(x, xsyst, fct, fctsys, cov);
  result.first = std::max(0., std::min(1., result.first));
  return result;
}

//====================== efficiency retrieval ==========================================

SUSY::CalibResult
SUSY::CalibrationDataInterfaceROOT::getEfficiency (const CalibrationDataVariables& variables,
						       const string& label,
						       const string& OP, Uncertainty unc) const
{
  SUSY::CalibResult sfResult = getScaleFactor(variables, label, OP, unc);
  SUSY::CalibResult effResult = getMCEfficiency(variables, label, OP, unc);

  double relative = 0;
  double value = effResult.first;
  if (TMath::Abs(sfResult.first) > SUSY::CalibZERO) {
    value = std::min(effResult.first*sfResult.first, 1.);
    
    if (value > 0.) {
      relative = effResult.second/effResult.first;
      double sfRelative = sfResult.second/sfResult.first;
      /*
	cout << "sferr=" << sfResult.second
	<< "btag Calib relative=" << relative << " sfRelative=" << sfRelative << endl;
      */
      relative = TMath::Sqrt(sfRelative*sfRelative + relative*relative);
    }
  } else {
    // now never happens due to protection of SF return value:
    cerr << "ERROR: CalibrationDataInterfaceROOT::getEfficiency: SF null result, SF=" << sfResult.first
	 << " MC eff=" << effResult.first 
	 << "; setting SF=1."
	 << endl;
    relative = SUSY::dummyValue;
  }

  return std::make_pair(value,value*relative);
}


//====================== "MC" inefficiency scale factor retrieval ======================================

SUSY::CalibResult
SUSY::CalibrationDataInterfaceROOT::getInefficiencyScaleFactor(const CalibrationDataVariables& variables,
								   const string& label,
								   const string& OP, Uncertainty unc) const
{

  SUSY::CalibResult effResult = this -> getMCEfficiency(variables, label, OP, unc);
  SUSY::CalibResult sfResult = this -> getScaleFactor(variables, label, OP, unc);

  double eff = std::min(effResult.first, 1.);
  double efferr = effResult.second;
  double sf = sfResult.first; 
  double sferr = sfResult.second; 

  double val = 0.; // SUSY::dummyValue;
  double err = 0.; // SUSY::dummyValue;
  if (1. - eff > CalibZERO) {
    val = (1. - eff*sf) / (1. - eff);
    err = pow((1. - sf) / (1. - eff) * efferr, 2) + pow(eff*sferr, 2);
    if (err > 0.)
      err = 1./(1. - eff) * TMath::Sqrt(err);
    // cout << "btag Calib Ineff err=" << err << endl;
  }

  return std::make_pair(std::max(0., val), err);

}

//====================== inefficiency retrieval ======================================

SUSY::CalibResult
SUSY::CalibrationDataInterfaceROOT::getInefficiency (const CalibrationDataVariables& variables,
							 const string& label,
							 const string& OP, Uncertainty unc) const
{
  SUSY::CalibResult effResult = this -> getMCEfficiency(variables, label, OP, unc);
  SUSY::CalibResult sfResult = this -> getScaleFactor(variables, label, OP, unc);
  
  double val = std::max(0., 1. - effResult.first * sfResult.first);
  double err = 0.; // SUSY::dummyValue;
  if (effResult.first > 0. && sfResult.first > 0.) 
    // safer than pow(x, 2):
    err = effResult.second/effResult.first*effResult.second/effResult.first + sfResult.second/sfResult.first*sfResult.second/sfResult.first;
  //  cout << "btag Calib Ineff err*err=" << err << endl;
  if (err > 0.)
    err = val*TMath::Sqrt(err);
  //  else err = SUSY::dummyValue;
  return std::make_pair(std::max(0., std::min(1., val)), err);

}

//====================== "MC" inefficiency retrieval ======================================

SUSY::CalibResult
SUSY::CalibrationDataInterfaceROOT::getMCInefficiency (const CalibrationDataVariables& variables,
							   const string& label,
							   const string& OP, Uncertainty unc) const
{

  SUSY::CalibResult effResult = this -> getMCEfficiency(variables, label, OP, unc);
  return std::make_pair(std::max(0., 1. - effResult.first), effResult.second);

}

//Preloads objects necessary so that input calibration file can be closed. Needed if using PROOF, does not need to be called if not
void SUSY::CalibrationDataInterfaceROOT::initialize(const string& jetauthor, const string& OP, Uncertainty unc)
{
	if((!m_fileEff)||(!m_fileSF)) {
		cerr << "initialize can only be called once per CalibrationDataInterfaceROOT object" << endl;
		return;
	}
	else {
		cout << "initializing BTagCalibrationDataInterfaceROOT for PROOF with jetAuthor = " << jetauthor 
			<< ", tagger = " << m_taggerName << ", operating point = " << OP << ", uncertainty = " << unc << endl;
	}

    // quark flavours
    std::vector<string> label;
    label.push_back("C");
    label.push_back("B");
    label.push_back("N/A");

    CalibrationDataVariables BTagVars;
    BTagVars.jetAuthor = jetauthor;
    BTagVars.jetPt  = 100000.; //Irrelevant, just has to be valid to retrive objects
    BTagVars.jetEta = 1.5; //Irrelevant, just has to be valid to retrive objects

	std::vector<string>::iterator flav = label.begin();
	for(; flav!=label.end(); flav++) {
        std::pair<double, double> BTagCalibResult;
        BTagCalibResult = getScaleFactor(BTagVars,(*flav), OP, unc);

        std::pair<double, double> BTagCalibMCEff;
        BTagCalibMCEff = getMCEfficiency(BTagVars,(*flav), OP, unc);
	}	

	if(m_fileEff!=m_fileSF) {
		m_fileEff->Close();
		delete m_fileEff;
	}
	m_fileSF->Close();
	delete m_fileSF;
	m_fileEff = 0; //prevents repeat deletion in destructor
	m_fileSF = 0; //prevents repeat deletion in destructor
}



void
SUSY::CalibrationDataInterfaceROOT::retrieveFunctionArguments (const CalibrationDataVariables& variables,
								   const string& object, Double_t* x) const
{
  //  msg() << MSG::DEBUG << "object " << object << "argument values:";
  std::vector<int> argumentTypes = functionArguments(object);
  for (unsigned int t = 0; t < argumentTypes.size(); ++t) {
    switch(argumentTypes[t]) {
    case kPt:
      // NB convert from MeV to GeV!
      x[t] = variables.jetPt * 0.001;
      // msg() << MSG::DEBUG << " pt=" << x[t];
      break;
    case kEta:
      x[t] = variables.jetEta;
      // msg() << MSG::DEBUG << " eta=" << x[t];
      break;
    case kAbsEta:
      x[t] = variables.jetEta;
      if (x[t] < 0) x[t] *= -1.;
      // msg() << MSG::DEBUG << " abs(eta)=" << x[t];
    }
  }
  //  if (argumentTypes.size() == 0) msg() << " none";
  //  msg() << MSG::DEBUG << endreq;
}

TObject* SUSY::CalibrationDataInterfaceROOT::retrieveTObject(const string& name, bool eff) const
{
  std::map<string, TObject*>::iterator it = m_objects.find(name);
  if (it != m_objects.end()) return it->second;

  // If the object cannot be found, then each call will result in a new attempt to
  // retrieve the object from the ROOT file. Hopefully this will not happen too often...
  TObject* obj = m_objects[name] = (eff ? m_fileEff : m_fileSF) ->Get(name.c_str());
  if (!obj)
    cout << "btag Calib: retrieveTObject: failed 2nd attempt to get " << name.c_str() << endl;
  return obj;
}

string
SUSY::CalibrationDataInterfaceROOT::getAlias(const string& author) const
{
  std::map<string,string>::const_iterator it = m_aliases.find(author);
  return (it == m_aliases.end()) ? author : it->second;
}

string
SUSY::CalibrationDataInterfaceROOT::fullName(const string& author, const string& name) const
{
  string full(m_taggerName); full += "/";
  full += author; full += "/";
  full += name;
  return full;
}
