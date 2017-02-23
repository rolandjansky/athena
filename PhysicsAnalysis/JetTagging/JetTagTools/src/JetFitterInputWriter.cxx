/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


  JetFitterInputWriter::JetFitterInputWriter(const std::string& name,
                                   const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p),
    m_useCombinedIPNN(true)
  {
    declareProperty("useCombinedIPNN",m_useCombinedIPNN);
    declareProperty("usePtCorrectedMass",m_usePtCorrectedMass = false);
    declareInterface<IJetFitterClassifierTool>(this);
  }

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

  JetFitterInputWriter::~JetFitterInputWriter() {

  }

  StatusCode JetFitterInputWriter::initialize() {

    ATH_MSG_INFO(" Initialization of JetFitterInputWriter succesfull");
    return StatusCode::SUCCESS;
  }

  StatusCode JetFitterInputWriter::finalize() {
    ATH_MSG_INFO(" Finalization of JetFitterInputWriter succesfull");
    return StatusCode::SUCCESS;
  }




  StatusCode JetFitterInputWriter::fillLikelihoodValues(
    xAOD::BTagging* BTag,
    const std::string & jetauthor,
    const std::string& inputbasename,
    const std::string& /*outputbasename*/,
    double /*jetpT*/,
    double /*jeteta*/,
    double /*IP3dlike*/) {

    if (jetauthor=="") {
      ATH_MSG_WARNING(" Hypothesis or jetauthor is empty. No likelihood value given back. ");
    }

    int nVTX, nTracksAtVtx, nSingleTracks;
    float energyFraction, mass, significance3d;
    bool status = true;

    if("JetFitter" == inputbasename){
      status &= BTag->taggerInfo(nVTX, xAOD::BTagInfo::JetFitter_nVTX);
      status &= BTag->taggerInfo(nTracksAtVtx, xAOD::BTagInfo::JetFitter_nTracksAtVtx);
      status &= BTag->taggerInfo(nSingleTracks, xAOD::BTagInfo::JetFitter_nSingleTracks);
      status &= BTag->taggerInfo(energyFraction, xAOD::BTagInfo::JetFitter_energyFraction);

      if(m_usePtCorrectedMass){
        status &= BTag->taggerInfo(mass, xAOD::BTagInfo::JetFitter_mass);
      }
      else{
        status &= BTag->variable<float>(inputbasename, "massUncorr",mass );
      }
      status &= BTag->taggerInfo(significance3d, xAOD::BTagInfo::JetFitter_significance3d);
    }
    else{
      if(m_usePtCorrectedMass){
        status &= BTag->variable<float>(inputbasename, "mass",mass );
      }
      else{
        status &= BTag->variable<float>(inputbasename, "massUncorr",mass );
      }
      status &= BTag->variable<float>(inputbasename, "significance3d", significance3d);
      status &= BTag->variable<float>(inputbasename, "energyFraction", energyFraction);
      status &= BTag->variable<int>(inputbasename, "nVTX", nVTX);
      status &= BTag->variable<int>(inputbasename, "nTracksAtVtx", nTracksAtVtx);
      status &= BTag->variable<int>(inputbasename, "nSingleTracks", nSingleTracks);
    }

    return StatusCode::SUCCESS;
  }


}//end Analysis namespace
