/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// IegammaIso.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMAISO_H
#define EGAMMAINTERFACES_IEGAMMAISO_H

/// @class IegammaIso
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaIso
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
class CaloDetDescrManager;

static const InterfaceID IID_IegammaIso("IEgammaIso", 1, 0);
class IegammaIso : virtual public IAlgTool {

public:
    /** @brief Virtual destructor */
    virtual ~IegammaIso(){};

    /** @brief AlgTool interface methods */
    static const InterfaceID& interfaceID();

    class Info{
    public:
        double ethad=0;
        double ethad1=0;
        double ehad1=0;
	unsigned short nBadT0 = 0;
	unsigned short nBadT12 = 0;
    };
    /** @brief  method: Method to just calculate hadronic leakage*/
    virtual StatusCode execute(const xAOD::CaloCluster& cluster,
                               const CaloDetDescrManager& cmgr,                                                           
                               const CaloCellContainer& cellcoll,
                               Info& info)  const =0;
};

inline const InterfaceID& IegammaIso::interfaceID()
{
    return IID_IegammaIso;
}

#endif // EGAMMAINTERFACES_IEGAMMAISO_H
