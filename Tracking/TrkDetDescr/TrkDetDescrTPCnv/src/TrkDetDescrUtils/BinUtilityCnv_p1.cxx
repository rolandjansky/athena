/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinningData.h"
#include "TrkDetDescrUtils/BinningType.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtilityCnv_p1.h"

void BinUtilityCnv_p1::persToTrans( const Trk::BinUtility_p1 *persObj,
                                    Trk::BinUtility    *transObj,
                                    MsgStream&)
{
    transObj->clear();

    // the loop has to go over the type
    for (size_t ib = 0; ib < persObj->type.size(); ++ib) {
         int typ = persObj->type[ib];

         // check the type
         if ( typ == Trk::equidistant )      // equidistant binning
             (*transObj) += Trk::BinUtility(size_t((persObj->bins)[ib]),
                                            float((persObj->min)[ib]),
                                            float((persObj->max)[ib]),
                                            Trk::BinningOption((persObj->option)[ib]),
                                            Trk::BinningValue((persObj->binvalue)[ib]));
         else if ( typ == Trk::biequidistant ) {
             // biequidistant binning
             size_t subbins = size_t(0.5*((persObj->bins)[ib]-1));
             (*transObj) += Trk::BinUtility(size_t(subbins),
                                            float((persObj->subStep)[ib]),
                                            float((persObj->min)[ib]),
                                            float((persObj->max)[ib]),
                                            Trk::BinningOption((persObj->option)[ib]),
                                            Trk::BinningValue((persObj->binvalue)[ib]));
         } else {
             // arbitary
             if ((persObj->binvalue)[ib] != Trk::binH){
                std::vector<float> cboundaries = (persObj->boundaries)[ib];
                (*transObj) += Trk::BinUtility(cboundaries,
                                               Trk::BinningOption((persObj->option)[ib]),
                                               Trk::BinningValue((persObj->binvalue)[ib]));
             } else {
                 std::vector< std::pair<int,float> > chbounds = ((persObj->hbounds)[ib]); 
                 (*transObj) += Trk::BinUtility(float((persObj->refphi)[ib]),
                                                chbounds);
            }
        }
    }
}

void BinUtilityCnv_p1::transToPers( const Trk::BinUtility    *transObj,
                                            Trk::BinUtility_p1 *persObj,
                                            MsgStream& )
{      
    // loop over the BinningData and dump it into the persistent object
    for (const Trk::BinningData& bd : transObj->binningData()) {
        persObj->type.push_back(bd.type);
        persObj->option.push_back(bd.option);
        persObj->binvalue.push_back(bd.binvalue);
        persObj->bins.push_back(bd.bins);
        persObj->min.push_back(bd.min);
        persObj->max.push_back(bd.max);
        persObj->step.push_back(bd.step);
        persObj->subStep.push_back(bd.subStep);
        persObj->boundaries.push_back(bd.boundaries);
        persObj->refphi.push_back(bd.refphi);
        persObj->hbounds.push_back(bd.hbounds);
    }   
}
