/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_PHOTONCONVERSIONPID_H
#define TAUREC_PHOTONCONVERSIONPID_H

#include "tauRec/TauToolBase.h"

/**
 * @brief This tool identifies Conversion Candidates via a cut on the electron probability provided by the TRT PID Tool. 
 * 
 *  Such Photon Conversions are needed e.g. to find Photon Conversions within the tau decay cone.
 * 
 * @author M. Boehler
 */

class PhotonConversionPID : public TauToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------

    PhotonConversionPID(const std::string& type,
            const std::string& name,
            const IInterface* parent);

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    ~PhotonConversionPID();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode eventFinalize(TauCandidateData *data);

private:
    int m_ownPolicy;

    std::string m_ConversionCandidatesName;
    std::string m_ConversionOutputName;

    double m_eProb_cut;
};

#endif
