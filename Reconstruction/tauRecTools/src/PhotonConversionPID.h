/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_PHOTONCONVERSIONPID_H
#define TAUREC_PHOTONCONVERSIONPID_H

#include "tauRecTools/TauRecToolBase.h"

/**
 * @brief This tool identifies Conversion Candidates via a cut on the electron probability provided by the TRT PID Tool. 
 * 
 *  Such Photon Conversions are needed e.g. to find Photon Conversions within the tau decay cone.
 * 
 * @author M. Boehler
 */

class PhotonConversionPID : public TauRecToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------

  PhotonConversionPID(const std::string& name);
  ASG_TOOL_CLASS2(PhotonConversionPID, TauRecToolBase, ITauToolBase);

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    ~PhotonConversionPID();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode eventFinalize();
    virtual StatusCode execute(xAOD::TauJet&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }

    virtual void print() const { }

private:
    int m_ownPolicy;

    std::string m_ConversionCandidatesName;
    std::string m_ConversionOutputName;

    double m_eProb_cut;
};

#endif
