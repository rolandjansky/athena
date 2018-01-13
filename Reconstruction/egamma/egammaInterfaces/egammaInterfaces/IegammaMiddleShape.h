/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

///////////////////////////////////////////////////////////////////
// IegammaMiddleShape.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMAMIDDLESHAPE_H
#define EGAMMAINTERFACES_IEGAMMAMIDDLESHAPE_H

/// @class IegammaMiddleShape
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaMiddleShape
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///


// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellContainer;

static const InterfaceID IID_IegammaMiddleShape("IegammaMiddleShape", 1, 0);

class IegammaMiddleShape : virtual public IAlgTool {


public:
    /** @brief Virtual destructor */
    virtual ~IegammaMiddleShape(){};

    /** @brief AlgTool interface methods */
    static const InterfaceID& interfaceID();

    class Info {
    public:
        double e233=0;
        double e235=0; 
        double e237=0; 
        double e255=0;
        double e277=0; 
        double etaw=0;
        double phiw=0;  
    };
    /** @brief AlgTool method.*/
    virtual StatusCode execute(const xAOD::CaloCluster& cluster, 
            const CaloCellContainer& cell_container, Info& info) const = 0;
};

inline const InterfaceID& IegammaMiddleShape::interfaceID()
{
    return IID_IegammaMiddleShape;
}

#endif // EGAMMAINTERFACES_IEGAMMAMIDDLESHAPE_H
