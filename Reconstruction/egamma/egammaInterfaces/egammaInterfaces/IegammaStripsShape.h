/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

///////////////////////////////////////////////////////////////////
// IegammaStripsShape.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMASTRIPSSHAPE_H
#define EGAMMAINTERFACES_IEGAMMASTRIPSSHAPE_H

/// @class IegammaStripsShape
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaStripsShape
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

static const InterfaceID IID_IegammaStripsShape("IegammaStripsShape", 1, 0);

class IegammaStripsShape : virtual public IAlgTool {


public:
    /** @brief Virtual destructor */
    virtual ~IegammaStripsShape(){};

    /** @brief AlgTool interface methods */
    static const InterfaceID& interfaceID();

    class Info{
    public :
        double etamax               = 0;
        double phimax               = 0;
        double ncetamax             = -1;
        double ncetaseed            = -1;
        double wstot                = -999. ;
        double etot                 = 0;
        double etas3                = 0;  
        double deltaEtaTrackShower  = -999. ; 
        double deltaEtaTrackShower7 = -999. ;
        double e132                 = 0; 
        double e1152                = 0; 
        double ws3                  = -999. ; 
        double ws3c                 = -999. ; 
        double poscs1               = -999. ; 
        double etaincell            = -999. ; 
        double asymmetrys3          = -999. ; 
        double f1                   = 0;  
        double f1core               = 0;  
        double f2                   = 0; 
        double widths5              = -999.; 
        double esec                 = 0; 
        double val                  = 0;  
        double fside                = 0;
        double emaxs1               = 0; 
        double emins1               = 0; 
        double esec1                = 0;
        bool success              = false ;


    };
    /** @brief AlgTool method.*/
    virtual StatusCode execute(const xAOD::CaloCluster& cluster, 
            const CaloCellContainer& cell_container, Info& info) const = 0;

    };

inline const InterfaceID& IegammaStripsShape::interfaceID()
{
    return IID_IegammaStripsShape;
}

#endif // EGAMMAINTERFACES_IEGAMMASTRIPSSHAPE_H
