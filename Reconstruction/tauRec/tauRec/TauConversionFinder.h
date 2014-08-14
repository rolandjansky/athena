/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCONVERSIONFINDER_H
#define TAUREC_TAUCONVERSIONFINDER_H

#include <tauRec/TauToolBase.h>

/**
 * @brief This tool identifies if a tau track is reconstructed as photon conversion track too.
 * 
 * @author M. Boehler
 */

class TauConversionFinder : public TauToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------
    TauConversionFinder(const std::string& type,
            const std::string& name,
            const IInterface* parent);

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    ~TauConversionFinder();

    virtual StatusCode initialize();
    virtual StatusCode eventFinalize(TauCandidateData *data);
    virtual StatusCode finalize();

private:
    std::string m_vxCandidatesName;
    std::string m_trackContainerName;
    std::string m_ConversionCandidatesName;

    bool m_do_normal;
    double m_eProb_cut;
    bool m_adjust_tau_charge;

    int m_numLooseProng;
    int m_numProng;

};

#endif
