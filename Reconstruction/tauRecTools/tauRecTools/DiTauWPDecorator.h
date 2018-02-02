/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_DITAUWPDECORATOR_H
#define TAURECTOOLS_DITAUWPDECORATOR_H

#include "AsgTools/AsgTool.h"

#include "tauRecTools/IDiTauToolBase.h"
#include "xAODTau/DiTauJet.h"

class TH2;

namespace tauRecTools{

class AccessorAsDouble;
    
class DiTauWPDecorator
    : public IDiTauToolBase
    , public asg::AsgTool{
public:

    DiTauWPDecorator(const std::string& name="TauWPDecorator") ;
    ASG_TOOL_CLASS(DiTauWPDecorator,
                   tauRecTools::IDiTauToolBase)
    ~DiTauWPDecorator();

    virtual StatusCode initialize();
    virtual StatusCode execute(const xAOD::DiTauJet& xDiTau);

    virtual StatusCode retrieveHistos();
    virtual StatusCode storeLimits();
    virtual double transformScore(double score, double cut_lo, double eff_lo, double cut_hi, double eff_hi);

private:
    std::string m_sFile; //!
    
    typedef std::pair<double, TH2* > m_pair_t;
    
    std::vector<m_pair_t> m_vpHists;

    double m_dXmin;
    double m_dYmin;
    double m_dXmax;
    double m_dYmax;
    
    std::vector<int> m_viCutBits;
    std::vector<float> m_vfCutEffs;

    std::vector<std::string> m_vsDecorationNames;
    std::vector<float> m_vfCutEffsDecoration;

    std::string m_sScoreName;
    std::string m_sNewScoreName;
    std::string m_sXVar;
    std::string m_sTypeXVar;
    std::string m_sYVar;
    std::string m_sTypeYVar;
    std::string m_sDecayChannel;
    
    SG::AuxElement::ConstAccessor<double>* m_accScore;
    SG::AuxElement::Decorator<double>* m_accNewScore;

    AccessorAsDouble *m_accX;
    AccessorAsDouble *m_accY;
};
}

#endif
