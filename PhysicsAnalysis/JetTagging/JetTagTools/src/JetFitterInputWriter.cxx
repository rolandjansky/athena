/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterInputWriter.h
/// Package : BTagTools
/// Author  : Dan Guest
/// Created : January 2017
///
/// DESCRIPTION:
///
/// Write JetFitter inputs to xAOD
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "GaudiKernel/ITHistSvc.h"

// #include <TMath.h>
// #include <TH1.h>
// #include <TH1F.h>
// #include <TH2F.h>
#include <vector>
#include <string>
#include <map>


#include "JetTagTools/IJetFitterClassifierTool.h"
#include "JetTagTools/JetFitterInputWriter.h"

#include <TString.h>

namespace Analysis {


  StatusCode JetFitterInputWriter::initialize()
  {
    ATH_MSG_DEBUG(" Initialization of JetFitterInputWriter succesfull");
    return StatusCode::SUCCESS;
  }

  StatusCode JetFitterInputWriter::finalize() {
    ATH_MSG_DEBUG(" Finalization of JetFitterInputWriter succesfull");
    return StatusCode::SUCCESS;
  }




  StatusCode JetFitterInputWriter::fillLikelihoodValues(
    xAOD::BTagging* BTag,
    const std::string & jetauthor,
    const std::string& inputbasename,
    const std::string& /*outputbasename*/,
    float /*jetpT*/,
    float /*jeteta*/,
    float /*IP3dlike*/) const
  {
    if (jetauthor=="") {
      ATH_MSG_WARNING(" Hypothesis or jetauthor is empty. No likelihood value given back. ");
    }

    int nVTX, nTracksAtVtx, nSingleTracks;
    float energyFraction, mass, significance3d;

    if("JetFitter" == inputbasename){
      BTag->taggerInfo(nVTX, xAOD::BTagInfo::JetFitter_nVTX);
      BTag->taggerInfo(nTracksAtVtx, xAOD::BTagInfo::JetFitter_nTracksAtVtx);
      BTag->taggerInfo(nSingleTracks, xAOD::BTagInfo::JetFitter_nSingleTracks);
      BTag->taggerInfo(energyFraction, xAOD::BTagInfo::JetFitter_energyFraction);

      if(m_usePtCorrectedMass){
        BTag->taggerInfo(mass, xAOD::BTagInfo::JetFitter_mass);
      }
      else{
        BTag->variable<float>(inputbasename, "massUncorr",mass );
      }
      BTag->taggerInfo(significance3d, xAOD::BTagInfo::JetFitter_significance3d);
    }
    else{
      if(m_usePtCorrectedMass){
        BTag->variable<float>(inputbasename, "mass",mass );
      }
      else{
        BTag->variable<float>(inputbasename, "massUncorr",mass );
      }
      BTag->variable<float>(inputbasename, "significance3d", significance3d);
      BTag->variable<float>(inputbasename, "energyFraction", energyFraction);
      BTag->variable<int>(inputbasename, "nVTX", nVTX);
      BTag->variable<int>(inputbasename, "nTracksAtVtx", nTracksAtVtx);
      BTag->variable<int>(inputbasename, "nSingleTracks", nSingleTracks);
    }

    return StatusCode::SUCCESS;
  }


}//end Analysis namespace
