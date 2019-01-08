/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//////////////////////////////////////////////////////////////////////
//  L1DynamicPedestalProviderRoot.cxx 
///////////////////////////////////////////////////////////////////////

#include "TrigT1CaloTools/L1DynamicPedestalProviderRoot.h"

#include "PathResolver/PathResolver.h"

#include "TFile.h"
#include "TProfile.h"
#include "TString.h"

namespace LVL1 
{

//================ Constructor ================================================
L1DynamicPedestalProviderRoot::L1DynamicPedestalProviderRoot(const std::string& t,
                                                   const std::string& n,
                                                   const IInterface*  p)
  :
  AthAlgTool(t, n, p)
{
  declareInterface<IL1DynamicPedestalProvider>(this);

  declareProperty( "FirRefFile", m_firRefFileName );
}

//================ Destructor =================================================
L1DynamicPedestalProviderRoot::~L1DynamicPedestalProviderRoot()
{
}

//================ Initialisation =============================================
StatusCode L1DynamicPedestalProviderRoot::initialize()
{
  // read the values of the BCID correction from the root-file
  // code copied from L1TriggerTowerTool.cxx@599780
  std::string fullFileName = PathResolver::find_file(m_firRefFileName, "DATAPATH");
  if(fullFileName.empty()) {
    // path could not be resolved
    ATH_MSG_FATAL("Could not resolve input file " << m_firRefFileName);
    return StatusCode::FAILURE;
  }

  TFile firRefFile(fullFileName.c_str(), "READ");
  if (firRefFile.IsZombie()) {
    ATH_MSG_FATAL("Could not open file " << fullFileName);
    return StatusCode::FAILURE;
  }
  for (int iBin = 0; iBin < s_nElement; ++iBin) {
      TString histoNameEm  = "p1_bcidFir_em_"+(TString)std::to_string(iBin);
      TString histoNameHad = "p1_bcidFir_had_"+(TString)std::to_string(iBin);
      TProfile* h_em  = (TProfile*)firRefFile.Get(histoNameEm);
      TProfile* h_had = (TProfile*)firRefFile.Get(histoNameHad);
      for (int iBcid = 0; iBcid < s_nBcid; ++iBcid) {
          //m_firRefEm[iBin][iBcid]  = floor(h_em->GetBinContent(iBcid+1)+0.5);
          //m_firRefHad[iBin][iBcid] = floor(h_had->GetBinContent(iBcid+1)+0.5);
          // IRH
          m_firRefEm[s_nElement-iBin-1][iBcid]  = floor(h_em->GetBinContent(iBcid+1)+0.5);
          m_firRefHad[s_nElement-iBin-1][iBcid] = floor(h_had->GetBinContent(iBcid+1)+0.5);
      }
  }
  firRefFile.Close();
  
  return StatusCode::SUCCESS;
}

//================ Correction =================================================
// Return the dynamic pedestal.
// The correction is currently not available as a function of mu.
// In case no correction is available or applicable this function
// returns the uncorrected pedestal.
int L1DynamicPedestalProviderRoot::dynamicPedestal(int iEta, int layer, int pedestal, int iBCID, float /* mu */)
{
    if(iEta < 0 || iEta >= s_nElement) {
      ATH_MSG_ERROR("Inputs out of range: iEta = " << iEta);
      return pedestal;
    }

    if(iBCID < 0 || iBCID >= s_nBcid) {
      return pedestal;
    }

    if (layer == 0) { // Em 
      if (m_firRefEm[iEta][iBCID] > 0) return m_firRefEm[iEta][iBCID];
    } else if (layer == 1) { // Had
      if (m_firRefHad[iEta][iBCID] > 0) return m_firRefHad[iEta][iBCID];
    } else {
      ATH_MSG_ERROR("Wrong layer index. Give 0 for Em,  1 for Had.");
    }
    return pedestal;
}

} // end of namespace
