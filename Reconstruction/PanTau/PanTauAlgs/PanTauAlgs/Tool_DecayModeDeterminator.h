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
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

//! PanTau
#include "PanTauAlgs/ITool_PanTauTools.h"
#include "PanTauAlgs/ITool_ModeDiscriminator.h"
#include "PanTauAlgs/ITool_InformationStore.h"

class StoreGateSvc;

namespace PanTau {
    class PanTauSeed2;
}


namespace PanTau {

/** @class Tool_DecayModeDeterminator
    Tool to store information needed in PanTau Algorithms
    @author Christian Limbach (limbach@physik.uni-bonn.de)
*/
  class Tool_DecayModeDeterminator : public asg::AsgTool, virtual public PanTau::ITool_PanTauTools {

    ASG_TOOL_CLASS1(Tool_DecayModeDeterminator, PanTau::ITool_PanTauTools)
    
    public:
        
        enum DecayModeTest {
            t_1p0n_vs_1p1n,
            t_1p1n_vs_1pXn,
            t_3p0n_vs_3pXn,
            t_UnknownTest,
            t_nTests
        };
        
        Tool_DecayModeDeterminator(const std::string &name);
        virtual ~Tool_DecayModeDeterminator ();
        
        virtual StatusCode initialize();
//         virtual StatusCode finalize  ();
        
        virtual StatusCode execute(PanTau::PanTauSeed2* inSeed);
        
    private:
        
        StoreGateSvc*   m_sgSvc;
        
        ToolHandle<PanTau::ITool_InformationStore>  m_Tool_InformationStore;
        
        ToolHandle<PanTau::ITool_ModeDiscriminator> m_Tool_ModeDiscriminator_1p0n_vs_1p1n;
        ToolHandle<PanTau::ITool_ModeDiscriminator> m_Tool_ModeDiscriminator_1p1n_vs_1pXn;
        ToolHandle<PanTau::ITool_ModeDiscriminator> m_Tool_ModeDiscriminator_3p0n_vs_3pXn;


	std::string m_Tool_InformationStoreName;

	std::string m_Tool_ModeDiscriminator_1p0n_vs_1p1nName;
	std::string m_Tool_ModeDiscriminator_1p1n_vs_1pXnName;
	std::string m_Tool_ModeDiscriminator_3p0n_vs_3pXnName;
        
        
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

	bool m_init=false;
  public:
	inline bool isInitialized(){return m_init;}
};


} // end of namespace PanTau
#endif // PANTAUALGS_TAUIMPACTPARAMETEREXTRACTIONTOOL_H
 
