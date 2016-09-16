/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class Tool_DetailsArranger
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for PID of TauSeeds
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef PANTAUALGS_TOOL_DETAILSARRANGER_H
#define PANTAUALGS_TOOL_DETAILSARRANGER_H

#include <string>
#include <vector>

//! ASG
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

//! xAOD EDM
#include "xAODTau/TauJet.h"
// #include "xAODTau/TauDefs.h"

//! PanTau
#include "PanTauAlgs/ITool_PanTauTools.h"
#include "PanTauAlgs/ITool_InformationStore.h"

#include "PanTauAlgs/TauConstituent.h"

namespace PanTau {
    class PanTauSeed2;
    class PanTauDetails;
}

//namespace Analysis {
//    class TauDetailsContainer;
//}

class StoreGateSvc;


namespace PanTau {

    /** @class Tool_DetailsArranger
        Tool for PID of Tau Seeds
        @author Sebastian Fleischmann
        @author Christian Limbach <limbach@physik.uni-bonn.de>
    */
  class Tool_DetailsArranger : public asg::AsgTool, virtual public PanTau::ITool_PanTauTools {
    
    ASG_TOOL_CLASS1(Tool_DetailsArranger, PanTau::ITool_PanTauTools)
    
    public:
        
        enum PanTauDetailsType {
            t_Int,
            t_Float,
            t_NTypes
        };
        
        Tool_DetailsArranger(const std::string &name);
        virtual ~Tool_DetailsArranger ();
        
        virtual StatusCode initialize();
//         virtual StatusCode finalize();
        
        virtual StatusCode execute(PanTau::PanTauSeed2* inSeed);//, Analysis::TauDetailsContainer* detailsCont);
        
    protected:
        
        StoreGateSvc*                                       m_sgSvc;
        
        ToolHandle<PanTau::ITool_InformationStore>  m_Tool_InformationStore;
	std::string m_Tool_InformationStoreName;
        
        void                        addPanTauDetailToTauJet(PanTauSeed2*                            inSeed,
                                                            std::string                            featName,
                                                            xAOD::TauJetParameters::PanTauDetails  detailEnum,
                                                            PanTauDetailsType                      detailType) const;

        StatusCode                        arrangePFOLinks(PanTau::PanTauSeed2* inSeed, xAOD::TauJet* tauJet);

        void                        SetHLVTau(PanTau::PanTauSeed2* inSeed, xAOD::TauJet* tauJet, std::string inputAlg, std::string m_varTypeName_Basic);

	std::vector< ElementLink< xAOD::PFOContainer > > PreselectNeutralLinks(std::vector< ElementLink<xAOD::PFOContainer> > neutralPFOLinks, xAOD::TauJet* tauJet);

	void StripPi0ConstsFromNeutralConsts(std::vector< ElementLink< xAOD::PFOContainer > > &neutralPFOLinks, std::vector< ElementLink< xAOD::PFOContainer > > pi0PFOLinks);

	bool HasMultPi0sInOneCluster(const xAOD::PFO* pfo, int decayModeProto, TString inputAlg);

	void SetNeutralConstituentMass(xAOD::PFO* neutral_pfo, double mass);

	void SetNeutralConstituentVectorMasses(std::vector< ElementLink<xAOD::PFOContainer> > neutralPFOLinks, double mass);

	std::vector< ElementLink< xAOD::PFOContainer > > CollectConstituentsAsPFOLinks( PanTau::PanTauSeed2* inSeed, std::vector< ElementLink< xAOD::PFOContainer > > cellbased_neutralPFOLinks, PanTau::TauConstituent2::Type type );

    void createPi0Vectors(xAOD::TauJet* tauJet, std::vector<TLorentzVector>& vPi0s, std::vector< std::vector< ElementLink<xAOD::PFOContainer> > > &vec_pi0pfos);
    

        /* std::vector<unsigned int>   helper_IndicesOfNeutralsToBePi0(xAOD::TauJet* tauJet,  */
        /*                                                             std::vector< ElementLink<xAOD::PFOContainer> > neutralPFOLinks,  */
        /*                                                             int nMaxPi0s); */
        /* int                         helper_CopyNeutralsAndSetPi0(xAOD::TauJet* tauJet, int nMaxPi0s, bool isSpecialCase_1pXnTo1p1n); */
        
        bool        m_expectInvalidFeatures;
        
	const float MASS_PI0 = 134.98; // in MeV
        
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
