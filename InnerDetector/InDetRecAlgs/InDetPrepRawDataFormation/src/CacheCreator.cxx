/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "CacheCreator.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "Identifier/IdentifierHash.h"
#include "AthViews/View.h"
namespace InDet{

    CacheCreator::CacheCreator(const std::string &name, ISvcLocator *pSvcLocator) :
    IDCCacheCreatorBase(name,pSvcLocator),
    m_pTRTHelper(nullptr),
    m_pix_idHelper(nullptr),
    m_sct_idHelper(nullptr)
    {
    }


    StatusCode CacheCreator::initialize(){
        ATH_CHECK( m_rioContainerCacheKey.initialize(SG::AllowEmpty) );
        ATH_CHECK( m_SCTclusterContainerCacheKey.initialize(SG::AllowEmpty) );
        ATH_CHECK( m_PIXclusterContainerCacheKey.initialize(SG::AllowEmpty) );
        ATH_CHECK( m_PIXSpacePointCacheKey.initialize(SG::AllowEmpty) );
        ATH_CHECK( m_SCTSpacePointCacheKey.initialize(SG::AllowEmpty) );
        ATH_CHECK( m_SCTRDOCacheKey.initialize(SG::AllowEmpty) );
        ATH_CHECK( m_SCTBSErrCacheKey.initialize(SG::AllowEmpty) );
        ATH_CHECK( m_SCTFlaggedCondCacheKey.initialize(SG::AllowEmpty) );
        ATH_CHECK( m_PixRDOCacheKey.initialize(SG::AllowEmpty) );
        ATH_CHECK( m_PixBSErrCacheKey.initialize(SG::AllowEmpty) );
        if (!m_disableTRT.value()) ATH_CHECK(detStore()->retrieve(m_pTRTHelper  , "TRT_ID"));
        ATH_CHECK(detStore()->retrieve(m_sct_idHelper, "SCT_ID"));
        ATH_CHECK(detStore()->retrieve(m_pix_idHelper, "PixelID"));
        if(m_disableWarning) m_disableWarningCheck.store(true, std::memory_order_relaxed);
        return StatusCode::SUCCESS;
    }

    CacheCreator::~CacheCreator() {}



    StatusCode CacheCreator::execute (const EventContext& ctx) const
    {

        ATH_CHECK(checkInsideViewOnce(ctx));

        if (!m_disableTRT.value()) ATH_CHECK(createContainer(m_rioContainerCacheKey, m_pTRTHelper->straw_layer_hash_max(), ctx));
        
        ATH_CHECK(createContainer(m_SCTclusterContainerCacheKey, m_sct_idHelper->wafer_hash_max(), ctx));
        
        ATH_CHECK(createContainer(m_PIXclusterContainerCacheKey, m_pix_idHelper->wafer_hash_max(), ctx));

        ATH_CHECK(createContainer(m_PIXSpacePointCacheKey, m_pix_idHelper->wafer_hash_max(), ctx));

        ATH_CHECK(createContainer(m_SCTSpacePointCacheKey, m_sct_idHelper->wafer_hash_max(), ctx));

        ATH_CHECK(createContainer(m_SCTRDOCacheKey, m_sct_idHelper->wafer_hash_max(), ctx));

        ATH_CHECK(createValueContainer(m_SCTBSErrCacheKey, m_sct_idHelper->wafer_hash_max(), ctx, std::numeric_limits<uint64_t>::min()));

        ATH_CHECK(createValueContainer(m_SCTFlaggedCondCacheKey, m_sct_idHelper->wafer_hash_max(), ctx, std::numeric_limits<uint64_t>::min()));

        ATH_CHECK(createContainer(m_PixRDOCacheKey, m_pix_idHelper->wafer_hash_max(), ctx));

        //=========================================================
        //  Size of Pixel BS Error container
        //
        //  The error would be stored not only for module but also each FE (x16) per module.
        //  In addition, IBL FEI4 provides error counter between trigger, this also extends 
        //  the size beyond nominal module + FE ID. These numbers come from the hardware 
        //  specification so that there is no easy way to document in the header file.
        //  Rather, put the hardcoded number here since m_pix_idHelper does not have it.
        //
        //      Total number of pixel modules: 2048
        //      Number of FE chips per module:   16
        //     -------------------------------------
        //          2048 x 17 (module + FE) = 34816
        //
        //      IBL extra error information  : 
        //          280(module) x 2(FE) x 32(error counter) = 35840
        //     -------------------------------------
        //                             Total : 70656
        //=========================================================
        ATH_CHECK(createValueContainer(m_PixBSErrCacheKey, 70656, ctx, std::numeric_limits<uint64_t>::min()));

        return StatusCode::SUCCESS;
    }
}

