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

//! Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//! xAOD EDM
#include "xAODTau/TauJet.h"
// #include "xAODTau/TauDefs.h"

//! PanTau
#include "PanTauInterfaces/ITool_DetailsArranger.h"
#include "PanTauInterfaces/ITool_InformationStore.h"


namespace PanTau {
    class PanTauSeed;
    class PanTauDetails;
}

namespace Analysis {
    class TauDetailsContainer;
}

class StoreGateSvc;


namespace PanTau {

    /** @class Tool_DetailsArranger
        Tool for PID of Tau Seeds
        @author Sebastian Fleischmann
        @author Christian Limbach <limbach@physik.uni-bonn.de>
    */
    class Tool_DetailsArranger : public AthAlgTool, virtual public PanTau::ITool_DetailsArranger {
    
    
    public:
        
        enum PanTauDetailsType {
            t_Int,
            t_Float,
            t_NTypes
        };
        
        Tool_DetailsArranger(const std::string&,const std::string&,const IInterface*);
        virtual ~Tool_DetailsArranger ();
        
        virtual StatusCode initialize();
//         virtual StatusCode finalize();
        
        virtual StatusCode arrangeDetails(PanTau::PanTauSeed* inSeed);//, Analysis::TauDetailsContainer* detailsCont);
        
    protected:
        
        StoreGateSvc*                                       m_sgSvc;
        
        ToolHandle<PanTau::ITool_InformationStore>  m_Tool_InformationStore;
        
        void                        addPanTauDetailToTauJet(PanTauSeed*                            inSeed,
                                                            std::string                            featName,
                                                            xAOD::TauJetParameters::PanTauDetails  detailEnum,
                                                            PanTauDetailsType                      detailType) const;
        void                        arrangeScalarDetail(PanTau::PanTauDetails* targetDetails, std::string featName, int featEnumFromPanTauDetails) const;
        void                        arrangeVectorDetail(PanTau::PanTauDetails* targetDetails, std::string featName, int featEnumFromPanTauDetails) const;
        void                        arrangePFOLinks(PanTau::PanTauSeed* inSeed, xAOD::TauJet* tauJet);
        std::vector<unsigned int>   helper_IndicesOfNeutralsToBePi0( std::vector< ElementLink<xAOD::PFOContainer> > neutralPFOLinks, int nMaxPi0s);
        int                         helper_CopyNeutralsAndSetPi0(xAOD::TauJet* tauJet, int nMaxPi0s, bool isSpecialCase_1pXnTo1p1n);
        
        bool        m_expectInvalidFeatures;
        
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
        
    };
} // end of namespace PanTau
#endif // PANTAUALGS_TAUDISCRIMINANTTOOL_H
