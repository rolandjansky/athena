/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMABACKSHAPE_H
#define EGAMMACALOTOOLS_EGAMMABACKSHAPE_H

/// @class egammaBackShape
/// @brief     EM cluster shower shape calculations in 3rd sampling. 
///  Calculate the width in the back layer around the eta,phi of
///  the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

class CaloCellContainer;
class CaloDetDescrManager;

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "egammaInterfaces/IegammaBackShape.h"
#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"

class egammaBackShape : public AthAlgTool, virtual public IegammaBackShape {

public:

    /** @brief Default constructor*/
    egammaBackShape(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    /** @brief Destructor*/
    ~egammaBackShape();  

    /** @brief AlgTool initialize method.*/
    StatusCode initialize() override ;
    /** @brief AlgTool finalize method */
    StatusCode finalize() override ;

    /** @brief AlgTool main method */
    virtual StatusCode execute(const xAOD::CaloCluster& cluster, 
            const CaloCellContainer& cell_container, Info& info) const override final;
private:

 const CaloDetDescrManager* m_calo_dd;
 
 /** @brief tool to calculate sum of energy in all samples */
    ToolHandle<IegammaEnergyPositionAllSamples>  m_egammaEnergyPositionAllSamples {this,
        "egammaEnergyPositionAllSamplesTool", 
        "egammaEnergyPositionAllSamples/egammaEnergyPositionAllSamples"};

    /** @brief boolean to calculate less important variables*/
    Gaudi::Property<bool> m_ExecOtherVariables {this, 
        "ExecOtherVariables", true,
        "Calculate some less important variables"};
};

#endif
