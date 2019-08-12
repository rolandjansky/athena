/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETJVTEFFICIENCYSCALEFACTORS_H_
#define JETJVTEFFICIENCYSCALEFACTORS_H_

#include "JetAnalysisInterfaces/IJetJvtEfficiency.h"
#include "PATInterfaces/SystematicsTool.h"
#include "AsgTools/AsgTool.h"

#include "xAODEventInfo/EventInfo.h"

#include <TH2.h>
#include <string>
#include <memory>

namespace CP {

enum SystApplied {
  NONE,
  JVT_EFFICIENCY_DOWN,
  JVT_EFFICIENCY_UP,
  FJVT_EFFICIENCY_DOWN,
  FJVT_EFFICIENCY_UP,
  MVFJVT_EFFICIENCY_DOWN,
  MVFJVT_EFFICIENCY_UP
};

class JetJvtEfficiency: public asg::AsgTool, public CP::SystematicsTool, virtual public CP::IJetJvtEfficiency {
    ASG_TOOL_CLASS( JetJvtEfficiency, CP::IJetJvtEfficiency)

public:
    JetJvtEfficiency( const std::string& name);

    virtual StatusCode initialize();

    StatusCode histInitialize();

    virtual CorrectionCode getEfficiencyScaleFactor(const xAOD::Jet& jet,float& sf);
    virtual CorrectionCode getInefficiencyScaleFactor(const xAOD::Jet& jet,float& sf);
    virtual CorrectionCode applyEfficiencyScaleFactor(const xAOD::Jet& jet);
    virtual CorrectionCode applyInefficiencyScaleFactor(const xAOD::Jet& jet);
    virtual CorrectionCode applyAllEfficiencyScaleFactor(const xAOD::IParticleContainer *jets,float& sf);
    virtual bool passesJvtCut(const xAOD::Jet& jet);
    virtual bool isInRange(const xAOD::Jet& jet);

    bool isAffectedBySystematic(const CP::SystematicVariation& var) const{return CP::SystematicsTool::isAffectedBySystematic(var);}
    CP::SystematicSet affectingSystematics() const {return CP::SystematicsTool::affectingSystematics();}
    CP::SystematicSet recommendedSystematics() const {return CP::SystematicsTool::recommendedSystematics();}
    CP::SystematicCode applySystematicVariation(const CP::SystematicSet& set) {return CP::SystematicsTool::applySystematicVariation(set);}
    CP::SystematicCode sysApplySystematicVariation(const CP::SystematicSet&);

    float getJvtThresh() const {return m_jvtCut;}
    float getUserPtMax() const {return m_maxPtForJvt;}
    StatusCode tagTruth(const xAOD::IParticleContainer *jets,const xAOD::IParticleContainer *truthJets);

private:

    JetJvtEfficiency();

    SystApplied m_appliedSystEnum;

    std::string m_wp;
    std::string m_file;
    std::unique_ptr<SG::AuxElement::Decorator< float > > m_sfDec;
    std::unique_ptr<SG::AuxElement::Decorator< char > > m_isHSDec;
    std::unique_ptr<SG::AuxElement::ConstAccessor< char > > m_isHSAcc;
    std::unique_ptr<TH2> h_JvtHist;
    std::unique_ptr<TH2> h_EffHist;
    std::string m_sf_decoration_name;
    std::string m_isHS_decoration_name;
    std::string m_truthJetContName;
    float m_jvtCut;
    float m_jvtCutBorder;
    std::string m_jetJvtMomentName;
    std::string m_jetfJvtMomentName;
    std::string m_jetMVfJvtMomentName;
    std::string m_jetEtaName;
    float m_maxPtForJvt;
    bool m_dofJVT;
    bool m_doMVfJVT;
    bool m_doTruthRequirement;
    std::string m_ORdec;
    bool m_doOR;
};

} /* namespace CP */

#endif /* JETJVTEFFICIENCYSCALEFACTORS_H_ */
