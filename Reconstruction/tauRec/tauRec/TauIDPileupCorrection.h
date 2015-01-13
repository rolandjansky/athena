/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUIDPILEUPCORRECTION_H
#define TAUREC_TAUIDPILEUPCORRECTION_H

#include "tauRec/TauToolBase.h"
#include "xAODTau/TauDefs.h"

class TH1;
class TF1;

/**
 * @brief Implementation of tool to correct tau ID variables for pile-up in a generic way
 * 
 *  Input comes from 2 ROOT files with TF1 functions containing the mean variable value
 *  as a function of the chosen pile-up estimator (Nvtx offline, mu online)
 *
 *  The correspondence between the ROOT File curve names and the TauParameters is handled
 *  via a small structure defined in initialize
 *
 * @author M. Beckingham
 * @author P.O. DeViveiros
 * @author M. Janus
 *                                                                              
 */

class TauIDPileupCorrection : public TauToolBase {
public:

    TauIDPileupCorrection(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    ~TauIDPileupCorrection();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute(TauCandidateData *data);

private:
    std::string m_tauContainerKey;
    std::string m_vertexContainerKey;
    std::string m_file1P; //!< energy calibration file
    std::string m_file3P; //!< energy calibration file
    
    std::map<std::string, TF1> m_calibFunctions1P;
    std::map<std::string, TF1> m_calibFunctions3P;
    
    struct TauConversion {
      std::string detailName;
      xAOD::TauJetParameters::Detail detailUncorr;
      xAOD::TauJetParameters::Detail detailCorr;
      
    TauConversion(std::string name, xAOD::TauJetParameters::Detail detail1, xAOD::TauJetParameters::Detail detail2)
    : detailName(name), detailUncorr(detail1), detailCorr(detail2)
      { }
    };
    
    std::vector<TauConversion> m_conversion;
    
    double m_averageEstimator;
    unsigned int m_minNTrackAtVertex;
    bool m_printMissingContainerINFO;
    
};

#endif
