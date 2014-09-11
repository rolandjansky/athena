/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TOOL_TAUCONSTITUENTGETTER
#define PANTAUALGS_TOOL_TAUCONSTITUENTGETTER

#include <map>
#include <vector>
#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "PanTauInterfaces/ITool_InformationStore.h"
#include "PanTauInterfaces/ITool_HelperFunctions.h"
#include "PanTauInterfaces/ITool_TauConstituentGetter.h"
#include "PanTauInterfaces/ITool_InputConverter.h"

//! xAOD EDM
#include "xAODTau/TauJet.h"

namespace Analysis {
    class TauJet;
}

namespace PanTau {
    class TauConstituent;
}





namespace PanTau {

class Tool_TauConstituentGetter : public AthAlgTool, virtual public PanTau::ITool_TauConstituentGetter  {
        
    public:
        
        Tool_TauConstituentGetter(const std::string&,const std::string&,const IInterface*);
        virtual ~Tool_TauConstituentGetter ();
        
        virtual StatusCode initialize();
//         virtual StatusCode finalize();
        
        virtual StatusCode GetTauConstituents(//const Analysis::TauJet*,
                                              const xAOD::TauJet* tauJet,
                                              std::vector<TauConstituent*>& outputList,
                                              std::string algName) const;
        
        
    protected:
        
        //member variables 
        ToolHandle<PanTau::ITool_InformationStore>  m_Tool_InformationStore;
        ToolHandle<PanTau::ITool_HelperFunctions>   m_Tool_HelperFunctions;
        ToolHandle<PanTau::ITool_InputConverter>    m_Tool_InputConverter;
        
        double                                      m_eflowRec_Assoc_DeltaR;

        
        
        
    }; //end class ConstituentGetter


}//end namespace PanTau




#endif // PANTAUALGS_TOOL_TAUCONSTITUENTGETTER