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

#ifndef PANTAUALGS_TOOL_FOURMOMENTUMCALCULATOR_H
#define PANTAUALGS_TOOL_FOURMOMENTUMCALCULATOR_H


//! C++
#include <map>
#include <string>

//! Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//! PanTau
#include "PanTauInterfaces/ITool_FourMomentumCalculator.h"
#include "PanTauInterfaces/ITool_InformationStore.h"


class eflowObjectContainer;
class StoreGateSvc;

namespace PanTau {
    class PanTauSeed;
}


namespace PanTau {

/** @class Tool_FourMomentumCalculator
    Tool to store information needed in PanTau Algorithms
    @author Christian Limbach (limbach@physik.uni-bonn.de)
*/
class Tool_FourMomentumCalculator : public AthAlgTool, virtual public PanTau::ITool_FourMomentumCalculator {
    
    
    
    public:
        
        Tool_FourMomentumCalculator(const std::string&,const std::string&,const IInterface*);
        virtual ~Tool_FourMomentumCalculator ();
        
        virtual StatusCode initialize();
//         virtual StatusCode finalize  ();
        
        virtual StatusCode calculateFourMomentum(PanTau::PanTauSeed* inSeed) const;
        
    private:
        ToolHandle<PanTau::ITool_InformationStore>  m_Tool_InformationStore;
        std::string m_varTypeName_Basic;

        
};


} // end of namespace PanTau
#endif // PANTAUALGS_TAUIMPACTPARAMETEREXTRACTIONTOOL_H
 
