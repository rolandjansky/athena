/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "egammaIso.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include <cmath> 

namespace{
    const double size = 0.12;
    const std::vector<CaloCell_ID::SUBCALO> theVecCalo = {
      CaloCell_ID::LARHEC, CaloCell_ID::TILE};
}

egammaIso::egammaIso(const std::string& type,
        const std::string& name,
        const IInterface* parent)
    : AthAlgTool(type, name, parent){
        // declare Interface
        declareInterface<IegammaIso>(this);
    }

egammaIso::~egammaIso(){ 
}

StatusCode egammaIso::initialize(){
    ATH_MSG_DEBUG(" Initializing egammaIso");
    return StatusCode::SUCCESS;
}
StatusCode egammaIso::finalize(){
    return StatusCode::SUCCESS;
}

StatusCode egammaIso::execute(const xAOD::CaloCluster& cluster,
                              const CaloDetDescrManager& cmgr,                                                           
                              const CaloCellContainer& cellcoll,
                              Info& info) const {
    const double eta  = cluster.eta(); 
    const double phi  = cluster.phi(); 
    double egap=0.;
    double ehad=0.;
    // define a new Calo Cell list corresponding to HAD Calo
    // retrieve the corresponding CaloCell_ID for LarHec and TILE
   // define a new Calo Cell list
    CaloCellList HADCellList(&cmgr,&cellcoll, theVecCalo);
    // increase window size a bit to avoid problems due to 4cm shift.
    // considering that in the tile the granularity is 0.1, the value
    // choosen below is safe. All hadrons in 0.2X0.2
    HADCellList.select(eta,phi,size,size);       
    ehad +=HADCellList.energy(); 
    HADCellList.select(eta,phi,size,size,CaloSampling::TileGap3);       
    egap +=HADCellList.energy(); 

    HADCellList.select(eta,phi,size,size,CaloSampling::HEC0);       
    info.ehad1 +=HADCellList.energy(); 

    HADCellList.select(eta,phi,size,size,CaloSampling::TileBar0);       
    info.ehad1 +=HADCellList.energy(); 

    // Fix had leakage in crack (TileGap1 and TileGap2 missing before 14.2)
    HADCellList.select(eta,phi,size,size,CaloSampling::TileGap1);
    info.ehad1 +=HADCellList.energy();
    HADCellList.select(eta,phi,size,size,CaloSampling::TileGap2);       
    info.ehad1 +=HADCellList.energy(); 

    HADCellList.select(eta,phi,size,size,CaloSampling::TileExt0);       
    info.ehad1 +=HADCellList.energy(); 

    const double eta2 = cluster.etaBE(2);
    if (eta2==-999.) {
        info.ethad1 = -999;
        info.ethad = -999;
    } else {
        const double cheta2 = cosh(eta2);
        info.ethad1 = cheta2!=0. ? info.ehad1/cheta2 : 0.; 
        // substract scintillator energy from ehad! 
        info.ethad = cheta2!=0. ? (ehad-egap)/cheta2 : 0.;
    }
    return StatusCode::SUCCESS;
}


