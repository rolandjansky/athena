/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUWPDECORATOR_H
#define TAUREC_TAUWPDECORATOR_H

#include "tauRecTools/TauRecToolBase.h"

#include "xAODTau/TauDefs.h"
#include "xAODEventInfo/EventInfo.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"

#include <utility>
#include <memory>
#include <vector>
#include <map>

class TH2;

/**
 * @brief Implementation of tool to decorate flattened BDT score and working points
 * 
 *  Input comes from ROOT files with lists of TH2s containing BDT/RNN score distributions
 *  as a function of the dependent variables. For eVeto, the score distributions depend on
 *  tau pT and |eta| of the leading track. Otherwise, the score distributions depend on
 *  tau pT and pileup.
 *
 * @author P.O. DeViveiros
 * @author W. Davey
 * @author L. Hauswald
 */

class TauWPDecorator : public TauRecToolBase {
  public:
    
    ASG_TOOL_CLASS2(TauWPDecorator, TauRecToolBase, ITauToolBase)
    
    /** @brief Constructor */
    TauWPDecorator(const std::string& name="TauWPDecorator");
    
    /** @brief Destructor */
    ~TauWPDecorator();

    /** @brief Initialization of this tool */
    virtual StatusCode initialize() override;

    /** @brief Executation of this tool */
    virtual StatusCode execute(xAOD::TauJet& pTau) const override;
    
    /** @brief Finalization of this tool */
    virtual StatusCode finalize() override;

  private:

    /** 
     * @brief Retrieve the histograms containing BDT/RNN score distributions as a function of dependent variables 
     * @param nProng Prong of the tau candidate
     */
    StatusCode retrieveHistos(int nProng);

    /**
     * @brief Obtain the limit of the dependent variables 
     * @param nProng Prong of the tau candidate
     */ 
    StatusCode storeLimits(int nProng);

    /**
     * @brief Obtain the flattened score
     * @param score Original BDT/RNN score
     * @param cutLow Lower score cut
     * @param effLow Efficiency of the lower cut
     * @param cutHigh Higher score cut
     * @param effHigh Efficiency of the higher cut
     */ 
    double transformScore(double score, double cutLow, double effLow, double cutHigh, double effHigh) const;

    bool m_electronMode; //!< Whether we are flatterning electron veto WP
    bool m_defineWPs; //!< Whether to decorate the WPs
    
    std::string m_scoreName; //!< Name of the original score
    std::string m_scoreNameTrans; //!< Name of the transformed score

    std::string m_file0p; //!< Calibration file name of 0-prong taus
    std::string m_file1p; //!< Calibration file name of 1-prong taus
    std::string m_file3p; //!< Calibration file name of 3-prong taus

    std::vector<int> m_EDMWPs; //!< Vector of WPs in the EDM
    std::vector<float> m_EDMWPEffs0p; //!< Efficiency of each WP in EDP for 0-prong taus
    std::vector<float> m_EDMWPEffs1p; //!< Efficiency of each WP in EDP for 1-prong taus
    std::vector<float> m_EDMWPEffs3p; //!< Efficiency of each WP in EDP for 3-prong taus

    std::vector<std::string> m_decorWPs; //!< Vector of WPs not in the EDM (to be decorated)
    std::vector<SG::AuxElement::Decorator<char>> m_charDecors; //!
    std::vector<float> m_decorWPEffs0p; //!< Efficiency of each WP to be docorated for 0-prong taus
    std::vector<float> m_decorWPEffs1p; //!< Efficiency of each WP to be docorated for 1-prong taus
    std::vector<float> m_decorWPEffs3p; //!< Efficiency of each WP to be docorated for 3-prong taus
    
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_aveIntPerXKey {this, 
        "AveIntPerXKey", 
        "EventInfo.AveIntPerXDecor",
        "Decoration for Average Interaction Per Crossing"};
    
    typedef std::pair<double, std::shared_ptr<TH2> > m_pair_t;

    std::shared_ptr<std::vector<m_pair_t>> m_hists0p; //!< Efficiency and corresponding score distributions of 0-prong taus
    std::shared_ptr<std::vector<m_pair_t>> m_hists1p; //!< Efficiency and corresponding score distributions of 1-prong taus
    std::shared_ptr<std::vector<m_pair_t>> m_hists3p; //!< Efficiency and corresponding score distributions of 3-prong taus
    
    std::map<int, double> m_xMin; //!< Map of n-prong and the minimum value of x variables
    std::map<int, double> m_yMin; //!< Map of n-prong and the minimum value of y variables
    std::map<int, double> m_xMax; //!< Map of n-prong and the maximum value of x variables
    std::map<int, double> m_yMax; //!< Map of n-prong and the maximum value of y variables
};

#endif
