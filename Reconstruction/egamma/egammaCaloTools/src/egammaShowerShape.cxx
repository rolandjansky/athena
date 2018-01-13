/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "egammaShowerShape.h"
#include "egammaInterfaces/IegammaPreSamplerShape.h"
#include "egammaInterfaces/IegammaStripsShape.h"
#include "egammaInterfaces/IegammaMiddleShape.h"
#include "egammaInterfaces/IegammaBackShape.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <cmath>

egammaShowerShape::egammaShowerShape(const std::string& type,
        const std::string& name,
        const IInterface* parent)
: AthAlgTool(type, name, parent)
{ 
    // declare Interface
    declareInterface<IegammaShowerShape>(this);

    InitVariables();
}

egammaShowerShape::~egammaShowerShape()
{ 
}

StatusCode egammaShowerShape::initialize(){
    ATH_MSG_DEBUG(" Initializing egammaShowerShape");

    if(m_egammaPreSamplerShape.retrieve().isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve "<<m_egammaPreSamplerShape);
        return StatusCode::FAILURE;
    } 
    else ATH_MSG_DEBUG("Tool " << m_egammaPreSamplerShape << " retrieved"); 

    if(m_egammaStripsShape.retrieve().isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve "<<m_egammaStripsShape);
        return StatusCode::FAILURE;
    } 
    else ATH_MSG_DEBUG("Tool " << m_egammaStripsShape << " retrieved"); 

    if(m_egammaMiddleShape.retrieve().isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve "<<m_egammaMiddleShape);
        return StatusCode::FAILURE;
    } 

    if(m_egammaBackShape.retrieve().isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve "<<m_egammaBackShape);
        return StatusCode::FAILURE;
    } 
    else ATH_MSG_DEBUG("Tool " << m_egammaBackShape << " retrieved"); 

    return StatusCode::SUCCESS;
}

StatusCode egammaShowerShape::finalize(){
    return StatusCode::SUCCESS;
}

StatusCode egammaShowerShape::execute(const xAOD::CaloCluster& cluster, 
        const CaloCellContainer& cell_container,Info& info) 
{

    // check if cluster is in barrel or in the end-cap
    if(!cluster.inBarrel() && !cluster.inEndcap() ) { 
        ATH_MSG_WARNING(" egammaShowerShape: Cluster is neither in Barrel nor in Endcap, cannot calculate ShowerShape ");
    }

    // shower shapes in presampler
    if ( m_ExecAllVariables && m_ExecOtherVariables && m_ExecPreSampler ) {
        // call execute method
        StatusCode sc = m_egammaPreSamplerShape->execute(cluster,cell_container);
        if ( sc.isFailure() ) {
            ATH_MSG_WARNING("Presampler shape returned failure ");
        }
    }

    // shower shapes in 1st compartment
    if ( m_ExecEMFirst ) {
        // call the execute method
        StatusCode sc = m_egammaStripsShape->execute(cluster,cell_container);
        if ( sc.isFailure() ) {
            ATH_MSG_WARNING("Strip shape returned failure ");
        }
    }

    // shower shapes in 2nd compartment
    if ( m_ExecEMSecond ) {
        // call the execute method
        StatusCode sc = m_egammaMiddleShape->execute(cluster,cell_container);
        if ( sc.isFailure() ) {
            ATH_MSG_WARNING("Middle shape returned failure ");
        }
    }

    // shower shapes in 3rd compartment
    if ( m_ExecAllVariables && m_ExecEMThird ) {
        // call execute method
        StatusCode sc = m_egammaBackShape->execute(cluster,cell_container);
        if ( sc.isFailure() ) {
            ATH_MSG_DEBUG("Back shape returned failure ");
        }
    }

    // Fill shower object with previously filled values
    FillShower(info);

    // shower shapes combined in different samplings
    if ( m_ExecAllVariables && m_ExecEMCombined ) {
        CombinedShape(info);
    }
    return StatusCode::SUCCESS;  
}

void egammaShowerShape::FillShower(Info& info) const {
    /*
     * Fill egammaShowerShape object
     * with values from other tools
     */

    // presampler
    info.e011 = m_egammaPreSamplerShape->e011();
    info.e033 = m_egammaPreSamplerShape->e033();

    // strips
    info.etot                 = m_egammaStripsShape->etot();
    info.e1152                = m_egammaStripsShape->e1152();
    info.e132                 = m_egammaStripsShape->e132();
    info.emaxs1               = m_egammaStripsShape->emaxs1();
    info.emins1               = m_egammaStripsShape->emins1();
    info.esec                 = m_egammaStripsShape->esec();
    info.esec1                = m_egammaStripsShape->esec1();
    info.f1                   = m_egammaStripsShape->f1();
    info.f1core               = m_egammaStripsShape->f1core();
    info.f2                   = m_egammaStripsShape->f2();
    info.ncetamax             = m_egammaStripsShape->ncetamax();
    info.ncetaseed            = m_egammaStripsShape->ncetaseed();
    info.etas3                = m_egammaStripsShape->etas3();
    info.poscs1               = m_egammaStripsShape->poscs1();
    info.deltaEtaTrackShower  = m_egammaStripsShape->deltaEtaTrackShower();
    info.deltaEtaTrackShower7 = m_egammaStripsShape->deltaEtaTrackShower7();
    info.wstot                = m_egammaStripsShape->wstot();
    info.ws3                  = m_egammaStripsShape->ws3();
    info.ws3c                 = m_egammaStripsShape->ws3c();
    info.widths5              = m_egammaStripsShape->widths5();
    info.asymmetrys3          = m_egammaStripsShape->asymmetrys3();
    info.val                  = m_egammaStripsShape->val();
    info.fside                = m_egammaStripsShape->fside();
    info.success              = m_egammaStripsShape->success();

    // middle
    info.e233   = m_egammaMiddleShape->e233();
    info.e235   = m_egammaMiddleShape->e235();
    info.e255   = m_egammaMiddleShape->e255();
    info.e237   = m_egammaMiddleShape->e237();
    info.e277   = m_egammaMiddleShape->e277();
    info.etaw   = m_egammaMiddleShape->etaw();
    info.phiw   = m_egammaMiddleShape->phiw();
    info.poscs2 = m_egammaMiddleShape->poscs2();

    // back
    info.e333   = m_egammaBackShape->e333();
    info.e335   = m_egammaBackShape->e335();
    info.e355   = m_egammaBackShape->e355();
    info.e337   = m_egammaBackShape->e337();
    info.e377   = m_egammaBackShape->e377();
    info.f3     = m_egammaBackShape->f3();
    info.f3core = m_egammaBackShape->f3core();
}

void egammaShowerShape::CombinedShape(Info& info) const{ 

    /* Shower shapes combining different samplings
     * all energy values have to be initialised to zero 
     * which is equivalent to have nothing (if it does not exists)
     * or 0 if there is no deposit
     */

    // energy in 3 strips in the 1st sampling
    double e132  = (info.e132>-999.) ? info.e132 : 0.;

    if (m_ExecOtherVariables) {
        // energy in 1X1 in the presampler
        double e011  = (info.e011>-999.) ? info.e011 : 0.; 
        // energy in 3X3 in the presampler
        double e033  = (info.e033>-999.) ? info.e033 : 0.;
        // energy in 15 strips in the 1st sampling
        double e1152  = (info.e1152>-999.) ? info.e1152 : 0.;
        // energy in 3X3 in the 2nd sampling
        double e233  = (info.e233>-999.) ? info.e233 : 0.;
        // energy in 5X5 in the 2nd sampling
        double e255  = (info.e255>-999.) ? info.e255 : 0.;
        // energy in 3X7 in the 2nd sampling
        double e237  = (info.e237>-999.) ? info.e237 : 0.;
        // energy in 3X3 in the 3rd sampling
        double e333  = (info.e333>-999.) ? info.e333 : 0.;
        // energy in 3X5 in the 3rd sampling
        double e335  = (info.e335>-999.) ? info.e335 : 0.;
        // energy in 3X7 in the 3rd sampling
        double e337  = (info.e337>-999.) ? info.e337 : 0.;

        /*energy in core of the shower
         * 3X3 in the presampler (info.e033)
         * 15X2 in the strips     (info.e1152)
         * 5X5 in the middle     (info.e255)
         * 3X5 in the back       (info.e335)
         */
        info.ecore = e033 + e1152 + e255 + e335;

        /* e 3X3 / e 3X7, the ratio of energies deposited
         * in a 3X3 and 3X7 clusters. More precisely, 
         * for the 3X3 cluster, sampling by sampling, the cell energies
         * are summed on the following eta by phi windows
         * 1X1 in the presampler (info.e011)
         * 3X2 in the strips     (info.e132)
         * 3X3 in the middle     (info.e233)
         * 3X3 in the back       (info.e333)
         * for the 7X3 cluster, sampling by sampling, the cell energies
         * are summed on the following eta by phi windows
         * 3X3 in the presampler (info.e033)
         * 15X2 in the strips     (info.e1152) 
         * 3X7 in the middle     (info.e237)
         * 3X7 in the back       (info.e337)
         */
        double e33 = e011 + e132 + e233 + e333;
        double e37 = e033 + e1152 + e237 + e337;
        info.reta3337_allcalo = fabs(e37) > 0. ? 1.-e33/e37 : 1.;
    }
}

