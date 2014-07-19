/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMMONEFFICIENCYTOOL_CXX
#define COMMONEFFICIENCYTOOL_CXX
#include "TauAnalysisTools/CommonEfficiencyTool.h"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "TDirectory.h"
#include "TParameter.h"
#include "TKey.h"

using namespace TauAnalysisTools;

//______________________________________________________________________________
CommonEfficiencyTool::CommonEfficiencyTool(std::string sName) :
  asg::AsgTool( sName ),
  m_mSF(0)
{
  ATH_MSG_INFO( "Initialising CommonEfficiencyTool" );
}

CommonEfficiencyTool::~CommonEfficiencyTool()
{
  delete m_mSF;
}

StatusCode CommonEfficiencyTool::configure()
{
  ATH_MSG_WARNING("Nothing to configure, use StatusCode CommonEfficiencyTool::configure(PropertyMgr* pPropertyMgr) instead");
  return StatusCode::SUCCESS;
}

StatusCode CommonEfficiencyTool::configure(PropertyMgr* pPropertyMgr)
{
  // only read in histograms once
  if (m_mSF==nullptr){
    std::string sSharepath, sAbspath, sFileName;
#ifdef ASGTOOL_ATHENA
    ATH_CHECK(pPropertyMgr->getProperty("SharePath", sSharepath));
    ATH_CHECK(pPropertyMgr->getProperty("FileName", sFileName));
#else
    pPropertyMgr->getProperty("SharePath", sSharepath);
    pPropertyMgr->getProperty("FileName", sFileName);
#endif
    
    if(sSharepath.size() != 0 && sSharepath.compare(0,1,"/") != 0)
      sAbspath = (std::string)(getenv("PWD")) +  "/" + sSharepath;
    else
      sAbspath = sSharepath;
    
    m_mSF = new SFMAP();
    TFile* fSF = TFile::Open((sAbspath + "/" + sFileName).c_str(), "READ");
    if(!fSF || !fSF->IsOpen()){
          ATH_MSG_FATAL("Could not open file " << (sAbspath + "/" + sFileName).c_str());
          return StatusCode::FAILURE;
    }
    this->ReadInputs(fSF, m_mSF);
    fSF->Close();
    delete fSF;
  }
  std::cout<<"--3\n";

#ifdef ASGTOOL_ATHENA
  assert(false && pPropertyMgr && "PropertyMgr->getProperty(std::string s, T& val); not working within athena");
#else
  pPropertyMgr->getProperty("SysDirection", m_iSysDirection);
#endif
  return StatusCode::SUCCESS;
}

// //______________________________________________________________________________
// TauAnalysisTools::CorrectionCode CommonEfficiencyTool::getEfficiencyScaleFactor(const xAOD::TauJet& tau,
// 										double& efficiencyScaleFactor,
// 										int iConfig1,
// 										int iConfig2,
// 										int iConfig3)
// {
//   configure(iConfig1, iConfig2, iConfig3);
//   return getEfficiencyScaleFactor(tau, efficiencyScaleFactor);
// }

// //______________________________________________________________________________
// TauAnalysisTools::CorrectionCode CommonEfficiencyTool::applyEfficiencyScaleFactor(xAOD::TauJet& tau,
// 										  int iConfig1,
// 										  int iConfig2,
// 										  int iConfig3)
// {
//   configure(iConfig1, iConfig2, iConfig3);
//   return applyEfficiencyScaleFactor(tau);
// }

//______________________________________________________________________________
std::string CommonEfficiencyTool::ConvertProngToString(const int& fProngness)
{
  std::string prong = "";
  if (fProngness == 0)
    ATH_MSG_WARNING("passed tau with 0 tracks, which is not supported, taking multiprong SF for now");
  fProngness == 1 ? prong = "_1p" : prong = "_3p";
  return prong;
}


template<class T>
void CommonEfficiencyTool::ReadInputs(TFile* fFile, std::map<std::string, T>* mMap)
{
  if(!fFile || !fFile->IsOpen())
    {
      Fatal("ReadInputs", "Could not open %s", fFile->GetName());
      return;
    }
  assert(fFile->IsOpen());
  TKey *kKey;
  TIter itNext(fFile->GetListOfKeys());
  while ((kKey = (TKey*)itNext()))
    {
      TClass *cClass = gROOT->GetClass(kKey->GetClassName());
      if (!cClass->InheritsFrom("TH1"))
	continue;
      T tObj = (T)kKey->ReadObj();
      tObj->SetDirectory(0);
      (*mMap)[std::string(tObj->GetName())] = tObj;
    }
  ATH_MSG_INFO("data loaded from " << fFile->GetName());
}


#endif // COMMONEFFICIENCYTOOL_CXX
