/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrkDetDescrUtils/BinUtility.h"
#undef private
#undef protected

#include "TrkDetDescrUtils/BinningData.h"
#include "TrkDetDescrUtils/BinningType.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtilityCnv_p1.h"

void BinUtilityCnv_p1::persToTrans( const Trk::BinUtility_p1 *persObj,
                                    Trk::BinUtility    *transObj,
                                    MsgStream&)
{
    // the loop has to go over the type
    std::vector<int>::const_iterator tIter  = (persObj->type).begin();
    std::vector<int>::const_iterator tIterE = (persObj->type).end();
    for (size_t ib=0; tIter != tIterE; ++tIter, ++ib){
         // check the type
         if ( (*tIter) == Trk::equidistant )      // equidistant binning
             (*transObj) += Trk::BinUtility(size_t((persObj->bins)[ib]),
                                            float((persObj->min)[ib]),
                                            float((persObj->max)[ib]),
                                            Trk::BinningOption((persObj->option)[ib]),
                                            Trk::BinningValue((persObj->binvalue)[ib]));
         else if ( (*tIter) == Trk::biequidistant ) {
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
    std::vector<Trk::BinningData>::const_iterator bdIter  = (transObj->m_binningData).begin();
    std::vector<Trk::BinningData>::const_iterator bdIterE = (transObj->m_binningData).end();
    for ( ; bdIter != bdIterE; ++bdIter){
        persObj->type.push_back((*bdIter).type);
        persObj->option.push_back((*bdIter).option);
        persObj->binvalue.push_back((*bdIter).binvalue);
        persObj->bins.push_back((*bdIter).bins);
        persObj->min.push_back((*bdIter).min);
        persObj->max.push_back((*bdIter).max);
        persObj->step.push_back((*bdIter).step);
        persObj->subStep.push_back((*bdIter).subStep);
        persObj->boundaries.push_back((*bdIter).boundaries);
        persObj->refphi.push_back((*bdIter).refphi);
        persObj->hbounds.push_back((*bdIter).hbounds);
    }   
}