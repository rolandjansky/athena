/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TOOL_DETAILSARRANGER_H
#define PANTAUALGS_TOOL_DETAILSARRANGER_H

#include <string>
#include <vector>

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "xAODTau/TauJet.h"
#include "xAODParticleEvent/ParticleContainer.h"

#include "PanTauAlgs/ITool_DetailsArranger.h"
#include "PanTauAlgs/ITool_InformationStore.h"
#include "PanTauAlgs/TauConstituent.h"

#include <TString.h>

namespace PanTau {
    class PanTauSeed;
    class PanTauDetails;
}

namespace PanTau {

    /** @class Tool_DetailsArranger
        Tool for PID of Tau Seeds
        @author Sebastian Fleischmann
        @author Christian Limbach <limbach@physik.uni-bonn.de>
    */
  class Tool_DetailsArranger : public asg::AsgTool, virtual public PanTau::ITool_DetailsArranger {
    
    ASG_TOOL_CLASS1(Tool_DetailsArranger, PanTau::ITool_DetailsArranger)
    
    public:
        
        enum PanTauDetailsType {
            t_Int,
            t_Float,
            t_NTypes
        };
        
        Tool_DetailsArranger(const std::string &name);
        virtual ~Tool_DetailsArranger ();
        
        virtual StatusCode initialize();
        
        virtual StatusCode execute(PanTau::PanTauSeed* inSeed, xAOD::ParticleContainer& pi0Container);
        
    protected:
        
        ToolHandle<PanTau::ITool_InformationStore>  m_Tool_InformationStore;
	std::string m_Tool_InformationStoreName;
        
        void                        addPanTauDetailToTauJet(PanTauSeed*                            inSeed,
                                                            std::string                            featName,
                                                            xAOD::TauJetParameters::PanTauDetails  detailEnum,
                                                            PanTauDetailsType                      detailType) const;

        StatusCode                        arrangePFOLinks(PanTau::PanTauSeed* inSeed, xAOD::TauJet* tauJet, xAOD::ParticleContainer& pi0Container);

        void                        SetHLVTau(PanTau::PanTauSeed* inSeed, xAOD::TauJet* tauJet, std::string inputAlg, std::string varTypeName_Basic);

	std::vector< ElementLink< xAOD::PFOContainer > > PreselectNeutralLinks(std::vector< ElementLink<xAOD::PFOContainer> > neutralPFOLinks, xAOD::TauJet* tauJet);

	void StripPi0ConstsFromNeutralConsts(std::vector< ElementLink< xAOD::PFOContainer > > &neutralPFOLinks, std::vector< ElementLink< xAOD::PFOContainer > > pi0PFOLinks);

	bool HasMultPi0sInOneCluster(const xAOD::PFO* pfo, int decayModeProto, TString inputAlg);

	void SetNeutralConstituentMass(xAOD::PFO* neutral_pfo, double mass);

	void SetNeutralConstituentVectorMasses(std::vector< ElementLink<xAOD::PFOContainer> > neutralPFOLinks, double mass);

	std::vector< ElementLink< xAOD::PFOContainer > > CollectConstituentsAsPFOLinks( PanTau::PanTauSeed* inSeed, std::vector< ElementLink< xAOD::PFOContainer > > cellbased_neutralPFOLinks, PanTau::TauConstituent::Type type );

	void createPi0Vectors(xAOD::TauJet* tauJet, std::vector<TLorentzVector>& vPi0s, std::vector< std::vector< ElementLink<xAOD::PFOContainer> > > &vec_pi0pfos);
    
        bool        m_expectInvalidFeatures;
        
	static const constexpr float MASS_PI0 = 134.98; // in MeV
        
        double      m_CoreCone;
        std::vector<double> m_EtaBinEdges;
        std::vector<double> m_EtaBinnedEtCuts;
        
        std::string m_varTypeName_Sum;
        std::string m_varTypeName_Ratio;
        std::string m_varTypeName_EtInRing;
        std::string m_varTypeName_Isolation;
        std::string m_varTypeName_Num;
        std::string m_varTypeName_Mean;
        std::string m_varTypeName_StdDev;
        std::string m_varTypeName_HLV;
        std::string m_varTypeName_Angle;
        std::string m_varTypeName_DeltaR;
        std::string m_varTypeName_JetMoment;
        std::string m_varTypeName_Combined;
        std::string m_varTypeName_JetShape;
        std::string m_varTypeName_ImpactParams;
        std::string m_varTypeName_Basic;
        std::string m_varTypeName_PID;
        std::string m_varTypeName_Shots;

	bool m_init=false;
  public:
	inline bool isInitialized(){return m_init;}
        
    };
} // end of namespace PanTau
#endif // PANTAUALGS_TAUDISCRIMINANTTOOL_H
