// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavAnalysisTool_H
#define TrigEgammaNavAnalysisTool_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavBaseTool.h"
class TrigEgammaNavAnalysisTool : public TrigEgammaNavBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool
{
    ASG_TOOL_CLASS(TrigEgammaNavAnalysisTool, ITrigEgammaAnalysisBaseTool)
    public:
        TrigEgammaNavAnalysisTool( const std::string& myname );
        ~TrigEgammaNavAnalysisTool() {};

        StatusCode childInitialize();
        StatusCode childBook();
        StatusCode childExecute();
        StatusCode childFinalize();
    protected:

    private:

        /*! Method to book histograms for each trigger */
        void bookPerSignature(const std::string);
        /*! Method to book histograms for each MaM category */
        void bookPerCategory(const std::string);
        void efficiency(const std::string,const float,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
        void inefficiency(const std::string,const float,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
        void resolution(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
        void resolutionPhoton(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
        void resolutionElectron(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
        void resolutionL2Photon(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
        void resolutionL2Electron(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
        void resolutionEFCalo(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
        
        /*! Online persisted containers */
        const xAOD::EventInfo *m_eventInfo;
        const xAOD::ElectronContainer *m_onlElectrons;
        const xAOD::PhotonContainer *m_onlPhotons;
        const xAOD::TrigElectronContainer *m_trigElectrons;
        const xAOD::CaloClusterContainer *m_caloClusters;
        const xAOD::TrigEMClusterContainer *m_trigEMClusters;
        const xAOD::EmTauRoIContainer *m_emTauRoI;

        unsigned int        m_eventCounter;
};
#endif
