/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class TauImpactParameterExtractionTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to store information needed in PanTau Algorithms
///////////////////////////////////////////////////////////////////
// limbach@physik.uni-bonn.de
///////////////////////////////////////////////////////////////////

#ifndef PANTAUALGS_TOOL_DECAYMODEDETERMINATOR_H
#define PANTAUALGS_TOOL_DECAYMODEDETERMINATOR_H


//! C++
#include <vector>
#include <string>

//! Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//! PanTau
#include "PanTauInterfaces/ITool_DecayModeDeterminator.h"
#include "PanTauInterfaces/ITool_ModeDiscriminator.h"
#include "PanTauInterfaces/ITool_InformationStore.h"

class StoreGateSvc;

namespace PanTau {
    class PanTauSeed;
}


namespace PanTau {

/** @class Tool_DecayModeDeterminator
    Tool to store information needed in PanTau Algorithms
    @author Christian Limbach (limbach@physik.uni-bonn.de)
*/
class Tool_DecayModeDeterminator : public AthAlgTool, virtual public PanTau::ITool_DecayModeDeterminator {
    
    
    
    public:
        
        enum DecayModeTest {
            t_1p0n_vs_1p1n,
            t_1p1n_vs_1pXn,
            t_3p0n_vs_3pXn,
            t_UnknownTest,
            t_nTests
        };
        
        Tool_DecayModeDeterminator(const std::string&,const std::string&,const IInterface*);
        virtual ~Tool_DecayModeDeterminator ();
        
        virtual StatusCode initialize();
//         virtual StatusCode finalize  ();
        
        virtual StatusCode determineDecayMode(PanTau::PanTauSeed* inSeed) const;
        
    private:
        
        StoreGateSvc*   m_sgSvc;
        
        ToolHandle<PanTau::ITool_InformationStore>  m_Tool_InformationStore;
        
        ToolHandle<PanTau::ITool_ModeDiscriminator> m_Tool_ModeDiscriminator_1p0n_vs_1p1n;
        ToolHandle<PanTau::ITool_ModeDiscriminator> m_Tool_ModeDiscriminator_1p1n_vs_1pXn;
        ToolHandle<PanTau::ITool_ModeDiscriminator> m_Tool_ModeDiscriminator_3p0n_vs_3pXn;
        
        
        //configurables to be retrieved from information store
        
        //variable prefix for the decay mode
        std::string m_varTypeName_Prefix_Basic;
        
        //cut values for CellBased
        double           m_BDTCutValue_R10X_CellBased;
        double           m_BDTCutValue_R11X_CellBased;
        double           m_BDTCutValue_R110_CellBased;
        double           m_BDTCutValue_R1XX_CellBased;
        double           m_BDTCutValue_R30X_CellBased;
        double           m_BDTCutValue_R3XX_CellBased;
        
        //cut values for eflowRec
        double           m_BDTCutValue_R10X_eflowRec;
        double           m_BDTCutValue_R11X_eflowRec;
        double           m_BDTCutValue_R110_eflowRec;
        double           m_BDTCutValue_R1XX_eflowRec;
        double           m_BDTCutValue_R30X_eflowRec;
        double           m_BDTCutValue_R3XX_eflowRec;
        
};


} // end of namespace PanTau
#endif // PANTAUALGS_TAUIMPACTPARAMETEREXTRACTIONTOOL_H
 
