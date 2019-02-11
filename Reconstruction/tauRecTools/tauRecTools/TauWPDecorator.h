/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUWPDECORATOR_H
#define TAUREC_TAUWPDECORATOR_H

#include "tauRecTools/TauRecToolBase.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauJet.h"
#include <map>

class TH2;

/**
 * @brief Implementation of tool to decorate flattened BDT score and working points
 * 
 *  Input comes from 2 ROOT files with lists of TH2s containing BDTScore distributions
 *  as a function of the dependent variables
 *
 * @author P.O. DeViveiros
 * @author W. Davey
 * @author L. Hauswald
 *                                                                              
 */

class TauWPDecorator : public TauRecToolBase {
public:

    TauWPDecorator(const std::string& name="TauWPDecorator") ;
    ASG_TOOL_CLASS2(TauWPDecorator, TauRecToolBase, ITauToolBase)
    ~TauWPDecorator();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }

    virtual StatusCode retrieveHistos(int nProng);
    virtual StatusCode storeLimits(int nProng);
    virtual double transformScore(double score, double cut_lo, double eff_lo, double cut_hi, double eff_hi);

    virtual void print() const { }
    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }
    virtual void cleanup(xAOD::TauJet* ) { }


private:
    std::string m_file1P; //!< energy calibration file
    std::string m_file3P; //!< energy calibration file
    
    typedef std::pair<double, std::unique_ptr<TH2> > m_pair_t;
    
    std::vector<m_pair_t> m_hists1P; //!
    std::vector<m_pair_t> m_hists3P; //!

    // Limits, probably not needed
    std::map<int, double> m_xmin;
    std::map<int, double> m_ymin;
    std::map<int, double> m_xmax;
    std::map<int, double> m_ymax;
    
    // Ugly, but makes configuration easier to read
    //double m_effVeryLoose1P;
    //double m_effLoose1P;
    //double m_effMedium1P;
    //double m_effTight1P;
    
    //double m_effVeryLoose3P;
    //double m_effLoose3P;
    //double m_effMedium3P;
    //double m_effTight3P;
    
    
    bool m_defineWP;
    bool m_electronMode;

    std::vector<int> m_cut_bits;
    std::vector<float> m_cut_effs_1p;
    std::vector<float> m_cut_effs_3p;

    std::vector<std::string> m_decoration_names;
    std::vector<float> m_cut_effs_decoration_1p;
    std::vector<float> m_cut_effs_decoration_3p;

    std::string m_scoreName;
    std::string m_newScoreName;

};

#endif
