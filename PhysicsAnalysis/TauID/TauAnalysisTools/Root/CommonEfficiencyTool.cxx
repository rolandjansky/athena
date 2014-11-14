/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMMONEFFICIENCYTOOL_CXX
#define COMMONEFFICIENCYTOOL_CXX

#include "TauAnalysisTools/CommonEfficiencyTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

using namespace TauAnalysisTools;

//______________________________________________________________________________
CommonEfficiencyTool::CommonEfficiencyTool(std::string sName)
  : asg::AsgTool( sName )
  , m_mSF(0)
  , m_tTECT(0)
  , m_sSystematicSet(0)
{
  m_mSystematics = {};
}

CommonEfficiencyTool::~CommonEfficiencyTool()
{
  delete m_mSF;
}

StatusCode CommonEfficiencyTool::initialize()
{
  ATH_MSG_INFO( "Initializing CommonEfficiencyTool" );
  // only read in histograms once
  if (m_mSF==nullptr)
  {
    std::string sInputFilePath = PathResolverFindCalibFile(m_tTECT->m_sInputFilePath);

    m_mSF = new SFMAP();
    TFile* fSF = TFile::Open(sInputFilePath.c_str(), "READ");
    if(!fSF || !fSF->IsOpen())
    {
      ATH_MSG_FATAL("Could not open file " << sInputFilePath.c_str());
      return StatusCode::FAILURE;
    }
    ReadInputs(fSF, m_mSF);
    fSF->Close();
    delete fSF;
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
void CommonEfficiencyTool::setParent(TauEfficiencyCorrectionsTool* tTECT)
{
  m_tTECT = tTECT;
}

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
