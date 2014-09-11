/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TOOL_INPUTCONVERTER
#define PANTAUALGS_TOOL_INPUTCONVERTER

#include <map>
#include <vector>
#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "PanTauInterfaces/ITool_InformationStore.h"
#include "PanTauInterfaces/ITool_HelperFunctions.h"
#include "PanTauInterfaces/ITool_InputConverter.h"

//! xAOD EDM
#include "xAODPFlow/PFO.h"
#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFODefs.h"

namespace Analysis {
    class TauPi0Cluster;
    class TauJet;
}

namespace Rec {
    class TrackParticle;
}

namespace PanTau {
    class TauConstituent;
}

class eflowObject;


namespace PanTau {

class Tool_InputConverter : public AthAlgTool, virtual public PanTau::ITool_InputConverter  {
        
    public:
        
        Tool_InputConverter(const std::string&,const std::string&,const IInterface*);
        virtual ~Tool_InputConverter ();
        
        virtual StatusCode initialize();
//         virtual StatusCode finalize();
        
        virtual StatusCode ConvertToTauConstituent(xAOD::PFO* pfo,
                                                   PanTau::TauConstituent* &tauConstituent,
                                                   const xAOD::TauJet* tauJet,
                                                   std::string algName) const;
        
    protected:
        
        //member variables 
        ToolHandle<PanTau::ITool_InformationStore>   m_Tool_InformationStore;
        ToolHandle<PanTau::ITool_HelperFunctions>    m_Tool_HelperFunctions;

        
        virtual bool       passesPreselectionEnergy(double itsEnergy) const;
        
        //cluster based converter
//             virtual StatusCode ConvertToTauConstituent(eflowObject* efo, TauConstituent* &tauConstituent) const;
        
        int     m_Config_UsePionMass;
        int     m_Config_TauConstituents_UseShrinkingCone;
        
        double  m_Config_TauConstituents_Types_DeltaRCore;
        double  m_Config_TauConstituents_PreselectionMinEnergy;
        
        int     m_Config_eflowRec_UseMomentumAsEnergy;
        double  m_Config_eflowRec_MinBDTValue_Pi0Neut;
        
        std::vector<double>  m_Config_CellBased_BinEdges_Eta;
        std::vector<double>  m_Config_CellBased_EtaBinned_Pi0MVACut_1prong;
        std::vector<double>  m_Config_CellBased_EtaBinned_Pi0MVACut_3prong;
        
        std::vector<double>  m_Config_eflowRec_BinEdges_Eta;
        std::vector<double>  m_Config_eflowRec_EtaBinned_Pi0MVACut_1prong;
        std::vector<double>  m_Config_eflowRec_EtaBinned_Pi0MVACut_3prong;
        std::vector<double>  m_Config_eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_1prong;
        std::vector<double>  m_Config_eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_3prong;
        
    }; //end class ConstituentGetter


}//end namespace PanTau




#endif // PANTAUALGS_TOOL_INPUTCONVERTER