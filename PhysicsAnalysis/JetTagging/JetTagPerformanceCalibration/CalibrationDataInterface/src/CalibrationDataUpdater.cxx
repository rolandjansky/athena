/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataUpdater.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "CalibrationDataInterface/CalibrationDataUpdater.h"

#include "TObject.h"
#include "TDirectory.h"
#include "TFile.h"

using std::string;

//================ Constructor =================================================

Analysis::CalibrationDataUpdater::CalibrationDataUpdater(const string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator)
{
  //  property declarations
  declareProperty("inputRootFile", m_inputRootFile = "", "ROOT file containing new parametrisations");
  declareProperty("DbRootFile", m_DbRootFile = "", "ROOT file to be entered in COOL");
  declareProperty("paramNames", m_paramNames, "list of input and output parametrisation names");
  declareProperty("overrideDefaults", m_overrideDefaults = false, "true if new results are to be used as default");

}

//================ Destructor =================================================

Analysis::CalibrationDataUpdater::~CalibrationDataUpdater()
{}


//================ Initialisation =================================================

StatusCode Analysis::CalibrationDataUpdater::initialize()
{
  // Code entered here will be executed once at program start.
  
  ATH_MSG_INFO( name() << " initialize()"  );

  // basic sanity and consistency checks

  TFile* fIn = 0;
  bool newInput = (m_inputRootFile != "");
  if (newInput) {
    fIn = new TFile(m_inputRootFile.c_str());
    if (fIn->IsZombie()) {
      ATH_MSG_FATAL( name() << ": requested input ROOT file invalid: "
                     << m_inputRootFile  );
      return StatusCode::FAILURE;
    }
  } else if (! m_overrideDefaults) {
    ATH_MSG_FATAL( name() << ": no input ROOT file given and no defaults to be modified:"
                   << " no action to be taken"  );
    return StatusCode::FAILURE;
  }
  if (m_DbRootFile == "") {
    ATH_MSG_FATAL( name() << ": no DB ROOT file given"  );
    return StatusCode::FAILURE;
  }
  TFile* fOut = new TFile(m_DbRootFile.c_str(), "UPDATE");
  if (fOut->IsZombie()) {
    ATH_MSG_FATAL( name() << ": DB ROOT file invalid: "
                   << m_DbRootFile  );
    return StatusCode::FAILURE;
  }

  for (std::vector<string>::const_iterator itname = m_paramNames.begin(),
	 itend = m_paramNames.end(); itname != itend; ++itname) {
    string to = *itname;
    // 1. determine whether to insert new results
    if (newInput) {
      // if no "->" is found, assume that the input and output object names are identical
      const string::size_type delim = itname->find("->");
      string from = (delim == string::npos) ? *itname : itname->substr(0, delim);
      if (delim != string::npos) to = itname->substr(delim+2);
      copyResults(from, to, fIn, fOut);
    } else if (! isValidName(to)) {
      ATH_MSG_WARNING( name() << ": invalid parametrisation name: " << to  );
      continue;
    }
    // 2. make these new results the default (if requested or if no default exists yet)
    setDefaultResults(to, fOut);
  }
  ATH_MSG_DEBUG( name() << ": all done"  );

  if (fIn) fIn->Close();
  fOut->Close();

  ATH_MSG_INFO( "initialize() successful in " << name()  );
  return StatusCode::SUCCESS;
}

void Analysis::CalibrationDataUpdater::setDefaultResults(string Name, TFile* fOut) const
{
  fOut->cd("/");
  TObject* obj = fOut->Get(Name.c_str());
  if (! obj) {
    ATH_MSG_WARNING( name() << ": nonexistent parametrisation name: " << Name );
    return;
  }

  string::size_type delim = Name.rfind("/");
  string dir = Name.substr(0, delim);
  TDirectory* newdir = fOut->GetDirectory(dir.c_str());
  if (! newdir->cd()) {
    // This should never happen, given that the object above apparently exists...
    ATH_MSG_WARNING( name() << ": Name now: " << Name << "; pwd: "
                     << gDirectory->GetPath() << "; cd() to subdir "
                     << dir << " failed!"  );
  };

  // if no change of defaults is requested to begin with, check whether a default
  // exists already
  delim = Name.rfind("_");
  string defaultName("default"); defaultName += Name.substr(delim);
  if (! m_overrideDefaults) {
    string fullName(dir); fullName += "/"; fullName += defaultName;
    if (fOut->Get(fullName.c_str())) return;
  }

  ATH_MSG_INFO( name() << ": copying parametrisation name: "
                << Name << " (+ stat., syst.) as default"  );

  // do the actual copying for the parametrisation itself
  obj->Write(defaultName.c_str());

  // and for the parameter covariance matrix
  string nameCov(Name); nameCov += "_stat";
  string defaultNameCov(defaultName); defaultNameCov += "_stat";
  fOut->Get(nameCov.c_str())->Write(defaultNameCov.c_str());

  // and for the systematic uncertainty
  string nameSyst(Name); nameSyst += "_syst";
  string defaultNameSyst(defaultName); defaultNameSyst += "_syst";
  fOut->Get(nameSyst.c_str())->Write(defaultNameSyst.c_str());
}

void Analysis::CalibrationDataUpdater::copyResults(const string& from, const string& to,
						   TFile* fIn, TFile* fOut) const
{
  if (! isValidName(to)) {
    ATH_MSG_WARNING( name() << ": invalid parametrisation name: " << to  );
    return;
  }

  // copy the parametrisation itself
  TObject* in = fIn->Get(from.c_str());
  if (! in) {
    ATH_MSG_WARNING( name() << ": input parametrisation name not found: "
                     << from  );
    return;
  }
  writeOutput(in, to, fOut);

  // copy the corresponding covariance matrix
  std::string fromCov(from); fromCov += "_stat";
  std::string toCov(to); toCov += "_stat";
  in = fIn->Get(fromCov.c_str());
  if (! in) {
    ATH_MSG_WARNING( name() << ": input parametrisation covariance matrix name not found: "
                     << fromCov  );
    return;
  }
  writeOutput(in, toCov, fOut);
  
  // copy the corresponding covariance matrix
  std::string fromSyst(from); fromSyst += "_syst";
  std::string toSyst(to); toSyst += "_syst";
  in = fIn->Get(fromSyst.c_str());
  if (! in) {
    ATH_MSG_WARNING( name() << ": input parametrisation systematics name not found: "
                     << fromSyst  );
    return;
  }
  writeOutput(in, toSyst, fOut);
  
}

bool Analysis::CalibrationDataUpdater::isValidName(string Name) const
{
  // this is a very basic check: the name should contain four slashes
  // and should end either in "_SF" or "_Eff"
  int slashes = 0;
  string::size_type delim = Name.find("/");
  while (delim != string::npos) {
    ++slashes;
    Name = Name.substr(delim+1);
    delim = Name.find("/");
  }
  if (slashes != 4) return false;

  delim = Name.rfind("_");
  if (delim == string::npos) return false;
  Name = Name.substr(delim+1);
  return (Name == "SF" || Name == "Eff");
}

void Analysis::CalibrationDataUpdater::writeOutput(TObject* in, string Name, TFile* fOut) const
{
  // first check whether the output directory exists, and if not, create it
  fOut->cd("/");
  TDirectory* tdir = dynamic_cast<TDirectory*>(fOut);
  string::size_type delim = Name.find("/");
  while (delim != string::npos) {
    string dir = Name.substr(0, delim);
    TDirectory* newdir = tdir->GetDirectory(dir.c_str());
    if (! newdir) newdir = tdir->mkdir(dir.c_str());
    if (! newdir->cd()) {
      ATH_MSG_WARNING( name() << ": Name now: " << Name << "; pwd: "
                       << gDirectory->GetPath() << "; cd() to subdir "
                       << dir << " failed!"  );
    };
    tdir = newdir;
    Name = Name.substr(delim+1, string::npos);
    delim = Name.find("/");
  }

  // then write the named object
  ATH_MSG_INFO( name() << ": writing object " << Name << " to directory "
                << gDirectory->GetPath()  );
  in->Write(Name.c_str());
}

//================ Finalisation =================================================

StatusCode Analysis::CalibrationDataUpdater::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Analysis::CalibrationDataUpdater::execute()
{
  // Code entered here will be executed once per event
  return StatusCode::SUCCESS;
}

//============================================================================================

