/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EGAMMACALOTOOLS_EGAMMASHOWERSHAPE_H
#define EGAMMACALOTOOLS_EGAMMASHOWERSHAPE_H


/// @ @class egammaShowerShape
/// @brief    EM cluster shower shape calculations. 
///  Calculate the width in the strip layer around the eta,phi of
///the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

#include "xAODCaloEvent/CaloClusterFwd.h"

class CaloCellContainer;
class LArEM_ID;
class CaloDetDescrManager;

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IegammaShowerShape.h"
#include "egammaCaloUtils/egammaPreSamplerShape.h"
#include "egammaCaloUtils/egammaStripsShape.h"
#include "egammaCaloUtils/egammaMiddleShape.h"
#include "egammaCaloUtils/egammaBackShape.h"

class egammaShowerShape : public AthAlgTool, virtual public IegammaShowerShape {

public:

    /** @brief Default constructor*/
    egammaShowerShape(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    /** @brief Destructor*/
    ~egammaShowerShape();  

    /** @brief AlgTool initialize method.*/
    StatusCode initialize() override;
    /** @brief AlgTool finalize method */
    StatusCode finalize() override;

    /** @brief AlgTool main method */
    virtual StatusCode execute(const xAOD::CaloCluster& cluster,
                               const CaloDetDescrManager& cmgr,
                               const CaloCellContainer& cell_container,
                               Info& info) const override final;

  private:
    
    /** @brief boolean to calculate all variables*/
    Gaudi::Property<bool> m_ExecAllVariables {this, 
        "ExecAllVariables", true, "flag used by trigger"};

    /** @brief boolean to calculate variables for presempler*/
    Gaudi::Property<bool> m_ExecPreSampler {this, 
        "ExecPreSampler", true, "Calculate EM presampler variables"};

    /** @brief boolean to calculate variables for 1st sampling*/
    Gaudi::Property<bool> m_ExecEMFirst {this, 
        "ExecEMFirst", true, "Calculate EM 1st sampling variables"};

    /** @brief boolean to calculate variables for 2nd sampling*/
    Gaudi::Property<bool> m_ExecEMSecond {this,
        "ExecEMSecond", true, "Calculate EM 2nd sampling variables"};

    /** @brief boolean to calculate variables for 3rd sampling*/
    Gaudi::Property<bool> m_ExecEMThird {this,
        "ExecEMThird", true, "Calculate EM 3rd sampling variables"};

    /** @brief boolean to calculate combined variables*/
    Gaudi::Property<bool> m_ExecEMCombined {this,
        "ExecEMCombined", true, 
        "Calculate EM variables combining all samplings"};
};

#endif
