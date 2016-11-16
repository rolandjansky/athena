/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCONVERSIONFINDER_H
#define TAUREC_TAUCONVERSIONFINDER_H

#include "tauRecTools/TauRecToolBase.h"

/**
 * @brief This tool identifies if a tau track is reconstructed as photon conversion track too.
 * 
 * @author M. Boehler
 */

class TauConversionFinder : public TauRecToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------
    TauConversionFinder(const std::string& name);
    ASG_TOOL_CLASS2(TauConversionFinder, TauRecToolBase, ITauToolBase);

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    ~TauConversionFinder();

    virtual StatusCode initialize();
    virtual StatusCode eventFinalize();
    virtual StatusCode finalize();
    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode execute(xAOD::TauJet&) { return StatusCode::SUCCESS; }

    virtual void print() const { }

private:
    std::string m_vxCandidatesName;
    std::string m_trackContainerName;
    std::string m_ConversionCandidatesName;

    bool m_do_normal;
    double m_eProb_cut;
    bool m_adjust_tau_charge;

    int m_numProng;

};

#endif
