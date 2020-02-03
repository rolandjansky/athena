/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

///////////////////////////////////////////////////////////////////
// IegammaShowerShape.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMASHOWERSHAPE_H
#define EGAMMAINTERFACES_IEGAMMASHOWERSHAPE_H

/** @class IegammaShowerShape
  Interface for the Reconstruction/egamma/egammaCaloTools/egammaShowerShape

  @author Frederic Derue derue@lpnhe.in2p3.fr

CREATED : 01/09/2008
MODIFIED :
*/

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellContainer;
class CaloDetDescrManager;
static const InterfaceID IID_IegammaShowerShape("IegammaShowerShape", 1, 0);

class IegammaShowerShape : virtual public IAlgTool {


public:
    /** @brief Virtual destructor */
    virtual ~IegammaShowerShape(){};

    /** @brief AlgTool interface methods */
    static const InterfaceID& interfaceID();

    /** @brief the info payload */
    class Info{
    public :

        // pre sampler
        double e011 = 0;
        double e033 = 0; 

        // first sampling
        double etaseed              = 0;
        double phiseed              = 0;
        double etamax               = 0;
        double phimax               = 0;
        double ncetamax             = -1;
        double ncetaseed            = -1;
        double wstot                = -999;
        double etot                 = 0;
        double etas3                = 0;  
        double deltaEtaTrackShower  = -999; 
        double deltaEtaTrackShower7 = -999;
        double e132                 = 0; 
        double e1152                = 0; 
        double ws3                  = -999; 
        double ws3c                 = -999; 
        double poscs1               = -999; 
        double etaincell            = -999; 
        double asymmetrys3          = -999; 
        double f1                   = 0;  
        double f1core               = 0;  
        double f2                   = 0; 
        double widths5              = -999; 
        double esec                 = 0; 
        double val                  = 0;  
        double fside                = 0;
        double emaxs1               = 0; 
        double emins1               = 0; 
        double esec1                = 0;
        bool success              = false ;

        // second sampling
        double e233 = 0;
        double e235 = 0; 
        double e237 = 0; 
        double e255 = 0;
        double e277 = 0; 
        double etaw = 0;
        double phiw = 0;
        double width= 0;
        double poscs2=0;
        double etagranularity = 0;
        double phigranularity = 0;

        // third sampling
        double f3     = 0;
        double f3core = 0;
        double e333   = 0;
        double e335   = 0; 
        double e355   = 0; 
        double e337   = 0; 
        double e377   = 0; 

        //combination of samplings
        double ecore = 0;
        double reta3337_allcalo = 0;
    };

    /** @brief AlgTool method.*/
    virtual StatusCode execute(const xAOD::CaloCluster& cluster,
                               const CaloDetDescrManager& cmgr,
                               const CaloCellContainer& cell_container,
                               Info& info) const = 0;
};

inline const InterfaceID& IegammaShowerShape::interfaceID()
{
    return IID_IegammaShowerShape;
}

#endif // EGAMMAINTERFACES_IEGAMMASHOWERSHAPE_H
