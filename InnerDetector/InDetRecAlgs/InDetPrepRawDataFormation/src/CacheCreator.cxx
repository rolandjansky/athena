/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetPrepRawDataFormation/CacheCreator.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "Identifier/IdentifierHash.h"

namespace InDet{

    CacheCreator::CacheCreator(const std::string &name, ISvcLocator *pSvcLocator) :
    AthReentrantAlgorithm(name,pSvcLocator),
    m_pTRTHelper(nullptr),
    m_pix_idHelper(nullptr),
    m_sct_idHelper(nullptr),
    m_rioContainerCacheKey(""),
    m_SCTclusterContainerCacheKey(""),
    m_PIXclusterContainerCacheKey("")
    {
        declareProperty("TRT_DriftCircleKey", m_rioContainerCacheKey);
        declareProperty("SCT_ClusterKey"    , m_SCTclusterContainerCacheKey);
        declareProperty("Pixel_ClusterKey"  , m_PIXclusterContainerCacheKey);
    }


    StatusCode CacheCreator::initialize(){
        ATH_CHECK( m_rioContainerCacheKey.initialize(!m_rioContainerCacheKey.key().empty()) );
        ATH_CHECK( m_SCTclusterContainerCacheKey.initialize(!m_SCTclusterContainerCacheKey.key().empty()) );
        ATH_CHECK( m_PIXclusterContainerCacheKey.initialize(!m_PIXclusterContainerCacheKey.key().empty()) );

        ATH_CHECK(detStore()->retrieve(m_pTRTHelper  , "TRT_ID"));
        ATH_CHECK(detStore()->retrieve(m_sct_idHelper, "SCT_ID"));
        ATH_CHECK(detStore()->retrieve(m_pix_idHelper, "PixelID"));
        return StatusCode::SUCCESS;
    }

    CacheCreator::~CacheCreator() {}

    StatusCode CacheCreator::execute_r (const EventContext& ctx) const
    {

        if(m_rioContainerCacheKey.key().empty()==false){
            ATH_CHECK(CreateContainer(m_rioContainerCacheKey, m_pTRTHelper->straw_layer_hash_max(), ctx));
        }else{
            ATH_MSG_DEBUG( "Creation of container "<< m_rioContainerCacheKey.key() << " is disabled (no name specified)");    
        }
        
        if(m_SCTclusterContainerCacheKey.key().empty()==false){
            ATH_CHECK(CreateContainer(m_SCTclusterContainerCacheKey, m_sct_idHelper->wafer_hash_max(), ctx));
        }else{
            ATH_MSG_DEBUG( "Creation of container "<< m_SCTclusterContainerCacheKey.key() << " is disabled (no name specified)");    
        }
        
        if(m_PIXclusterContainerCacheKey.key().empty()==false){
            ATH_CHECK(CreateContainer(m_PIXclusterContainerCacheKey, m_pix_idHelper->wafer_hash_max(), ctx));
        }else{
            ATH_MSG_DEBUG( "Creation of container "<< m_PIXclusterContainerCacheKey.key() << " is disabled (no name specified)");    
        }

        return StatusCode::SUCCESS;
    }
}
