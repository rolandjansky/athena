/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
        ATH_CHECK( m_PixRDOCacheKey.initialize(!m_PixRDOCacheKey.key().empty()) );
        if (!m_disableTRT.value()) ATH_CHECK(detStore()->retrieve(m_pTRTHelper  , "TRT_ID"));
        ATH_CHECK(detStore()->retrieve(m_sct_idHelper, "SCT_ID"));
        ATH_CHECK(detStore()->retrieve(m_pix_idHelper, "PixelID"));
        return StatusCode::SUCCESS;
    }

    CacheCreator::~CacheCreator() {}

    bool CacheCreator::isInsideView(const EventContext& context) const
    {
        const IProxyDict* proxy = context.getExtension<Atlas::ExtendedEventContext>().proxy();
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

        ATH_CHECK(createContainer(m_PixRDOCacheKey, m_pix_idHelper->wafer_hash_max(), ctx));

        return StatusCode::SUCCESS;
    }
}

