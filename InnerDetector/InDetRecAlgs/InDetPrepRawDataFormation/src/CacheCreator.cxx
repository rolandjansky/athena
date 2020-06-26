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
    AthReentrantAlgorithm(name,pSvcLocator),
    m_pTRTHelper(nullptr),
    m_pix_idHelper(nullptr),
    m_sct_idHelper(nullptr),
    m_disableWarningCheck(false)
    {
    }


    StatusCode CacheCreator::initialize(){
        ATH_CHECK( m_rioContainerCacheKey.initialize(!m_rioContainerCacheKey.key().empty()) );
        ATH_CHECK( m_SCTclusterContainerCacheKey.initialize(!m_SCTclusterContainerCacheKey.key().empty()) );
        ATH_CHECK( m_PIXclusterContainerCacheKey.initialize(!m_PIXclusterContainerCacheKey.key().empty()) );
        ATH_CHECK( m_PIXSpacePointCacheKey.initialize(!m_PIXSpacePointCacheKey.key().empty()) );
        ATH_CHECK( m_SCTSpacePointCacheKey.initialize(!m_SCTSpacePointCacheKey.key().empty()) );
        ATH_CHECK( m_SCTRDOCacheKey.initialize(!m_SCTRDOCacheKey.key().empty()) );
        ATH_CHECK( m_SCTBSErrCacheKey.initialize(!m_SCTBSErrCacheKey.key().empty()) );
        ATH_CHECK( m_PixRDOCacheKey.initialize(!m_PixRDOCacheKey.key().empty()) );
	ATH_CHECK( m_PixBSErrCacheKey.initialize(!m_PixBSErrCacheKey.key().empty()) );
        if (!m_disableTRT.value()) ATH_CHECK(detStore()->retrieve(m_pTRTHelper  , "TRT_ID"));
        ATH_CHECK(detStore()->retrieve(m_sct_idHelper, "SCT_ID"));
        ATH_CHECK(detStore()->retrieve(m_pix_idHelper, "PixelID"));
        return StatusCode::SUCCESS;
    }

    CacheCreator::~CacheCreator() {}

    bool CacheCreator::isInsideView(const EventContext& context) const
    {
        const IProxyDict* proxy = Atlas::getExtendedEventContext(context).proxy();
        const SG::View* view = dynamic_cast<const SG::View*>(proxy);
        return view != nullptr;
    }

    StatusCode CacheCreator::execute (const EventContext& ctx) const
    {

        if (!m_disableWarningCheck and !m_disableWarning.value()){
          if (isInsideView(ctx)){
            ATH_MSG_ERROR("CacheCreator is running inside a view, this is probably a misconfiguration");
            return StatusCode::FAILURE;
          }
          m_disableWarningCheck = true; //only check once
        }

        if (!m_disableTRT.value()) ATH_CHECK(createContainer(m_rioContainerCacheKey, m_pTRTHelper->straw_layer_hash_max(), ctx));
        
        ATH_CHECK(createContainer(m_SCTclusterContainerCacheKey, m_sct_idHelper->wafer_hash_max(), ctx));
        
        ATH_CHECK(createContainer(m_PIXclusterContainerCacheKey, m_pix_idHelper->wafer_hash_max(), ctx));

        ATH_CHECK(createContainer(m_PIXSpacePointCacheKey, m_pix_idHelper->wafer_hash_max(), ctx));

        ATH_CHECK(createContainer(m_SCTSpacePointCacheKey, m_sct_idHelper->wafer_hash_max(), ctx));

        ATH_CHECK(createContainer(m_SCTRDOCacheKey, m_sct_idHelper->wafer_hash_max(), ctx));

        ATH_CHECK(createValueContainer(m_SCTBSErrCacheKey, m_sct_idHelper->wafer_hash_max(), ctx, std::numeric_limits<uint64_t>::min()));

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

