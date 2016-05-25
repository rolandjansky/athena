/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PixelGangedClusterAmbiguitiesCnv.cxx
 * @brief Implementation file for AthenaPool converter for PixelGangedClusterAmbiguities 
 * @author Andreas.Wildauer@cern.ch
 */

#include "PixelGangedClusterAmbiguitiesCnv.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelGangedClusterAmbiguities_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelGangedClusterAmbiguitiesCnv_p1.h"
#include "MsgUtil.h"

static PixelGangedClusterAmbiguitiesCnv_p1   TPconverter;

PixelGangedClusterAmbiguities_PERS* PixelGangedClusterAmbiguitiesCnv::createPersistent(InDet::PixelGangedClusterAmbiguities* transObj) {
    //repeated twice, should really be in an initialize method
    IDEvtAthPool::setMsgName(this,"PixelGangedClusterAmbiguitiesConverter");//So msg() won't use name "AthenaPoolConverter" 

    ATH_MSG_DEBUG("PixelGangedClusterAmbiguities write");
    PixelGangedClusterAmbiguities_PERS *persObj = TPconverter.createPersistent( transObj, msg() );
    ATH_MSG_DEBUG("Success");
    return persObj; 
}
    
InDet::PixelGangedClusterAmbiguities* PixelGangedClusterAmbiguitiesCnv::createTransient() {
    //repeated twice, should really be in an initialize method
    IDEvtAthPool::setMsgName(this,"PixelGangedClusterAmbiguitiesConverter");//So msg() won't use name "AthenaPoolConverter" 

    static pool::Guid   p1_guid("FE36CE7E-EADF-481F-A55A-26DA0030DFAA");
//     static pool::Guid   p0_guid("380D8BB9-B34F-470F-92CC-06C3D60F7BE4");
    if( compareClassGuid(p1_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr< InDet::PixelGangedClusterAmbiguities_p1 > col_vect( poolReadObject< InDet::PixelGangedClusterAmbiguities_p1 >() );
        ATH_MSG_DEBUG("Reading PixelGangedClusterAmbiguities_p1"); 
        return TPconverter.createTransient( col_vect.get(), msg() );
    }
//     else if( compareClassGuid(p0_guid) ) {
//         dont think there was a "before T/P" split converter
//     } 
    throw std::runtime_error("Unsupported persistent version of EventInfo");
}

