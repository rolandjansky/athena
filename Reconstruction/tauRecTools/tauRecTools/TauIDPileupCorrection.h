/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUIDPILEUPCORRECTION_H
#define TAUREC_TAUIDPILEUPCORRECTION_H

#include "tauRecTools/TauRecToolBase.h"
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

struct TauConversion {
  std::string detailName;
  xAOD::TauJetParameters::Detail detailUncorr;
  xAOD::TauJetParameters::Detail detailCorr;
  
  TauConversion(){ };
TauConversion(std::string name, xAOD::TauJetParameters::Detail detail1, xAOD::TauJetParameters::Detail detail2)
: detailName(name), detailUncorr(detail1), detailCorr(detail2)
  { }
};


class TauIDPileupCorrection : public TauRecToolBase {
public:

    TauIDPileupCorrection(const std::string& name="TauIDPileupCorrection") ;
    ASG_TOOL_CLASS2(TauIDPileupCorrection, TauRecToolBase, ITauToolBase)
    ~TauIDPileupCorrection();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
    
    virtual void print() const { }
    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }
    virtual void cleanup(xAOD::TauJet* ) { }


private:
    std::string m_configPath;
    std::string m_tauContainerKey;
    std::string m_vertexContainerKey;
    std::string m_file1P; //!< energy calibration file
    std::string m_file3P; //!< energy calibration file
    
    std::map<std::string, TF1*> m_calibFunctions1P;
    std::map<std::string, TF1*> m_calibFunctions3P;
    

    StatusCode fillCalibMap( const std::string& file_name, std::map<std::string, TF1*> &calib_map);
    
    std::vector<TauConversion> m_conversion;
    
    double m_averageEstimator;
    unsigned int m_minNTrackAtVertex;
    bool m_printMissingContainerINFO;
    bool m_useMu;
    
};

#endif
