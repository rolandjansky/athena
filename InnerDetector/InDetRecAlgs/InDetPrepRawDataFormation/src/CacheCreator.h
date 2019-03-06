/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**
* @file CacheCreator.h
* @author a.barton
*/

#ifndef INDETPREPRAWDATAFORMATION_CACHECREATOR
#define INDETPREPRAWDATAFORMATION_CACHECREATOR

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Container.h"


#include "AthenaPoolUtilities/CondAttrListCollection.h" 

#include <atomic>

class TRT_ID;
class PixelID;
class SCT_ID;

namespace InDet{

    class CacheCreator : public AthReentrantAlgorithm 
    {
    public:

        CacheCreator(const std::string &name,ISvcLocator *pSvcLocator);
        virtual ~CacheCreator()  ;
        virtual StatusCode initialize () override;
        virtual StatusCode execute (const EventContext& ctx) const override;
        //No need for finalize
    protected:
        const TRT_ID* m_pTRTHelper;
        const PixelID* m_pix_idHelper;
        const SCT_ID*  m_sct_idHelper;
        SG::WriteHandleKey<InDet::TRT_DriftCircleContainerCache> m_rioContainerCacheKey
          {this, "TRT_DriftCircleKey", ""};
        SG::WriteHandleKey<SCT_ClusterContainerCache>            m_SCTclusterContainerCacheKey
          {this, "SCT_ClusterKey", ""};
        SG::WriteHandleKey<InDet::PixelClusterContainerCache>    m_PIXclusterContainerCacheKey
          {this,"Pixel_ClusterKey", ""};
        SG::WriteHandleKey<SpacePointCache>    m_PIXSpacePointCacheKey
          {this, "SpacePointCachePix", ""};
        SG::WriteHandleKey<SpacePointCache>    m_SCTSpacePointCacheKey
          {this, "SpacePointCacheSCT", ""};
        SG::WriteHandleKey<SCT_RDO_Cache>      m_SCTRDOCacheKey
          {this, "SCTRDOCacheKey", ""};
        SG::WriteHandleKey<PixelRDO_Cache>     m_PixRDOCacheKey
          {this, "PixRDOCacheKey", ""};
        BooleanProperty m_disableTRT{this, "disableTRT", false};
        BooleanProperty m_disableWarning{this, "DisableViewWarning", false};
        mutable std::atomic_bool m_disableWarningCheck;
	//Temporary workarounds for problem in scheduler - remove later
        bool isInsideView(const EventContext&) const;
        template<typename T>
        StatusCode createContainer(const SG::WriteHandleKey<T>& , long unsigned int , const EventContext& ) const;
    };

    template<typename T>
    StatusCode CacheCreator::createContainer(const SG::WriteHandleKey<T>& containerKey, long unsigned int size, const EventContext& ctx) const{
        if(containerKey.key().empty()){
            ATH_MSG_DEBUG( "Creation of container "<< containerKey.key() << " is disabled (no name specified)");
            return StatusCode::SUCCESS;
        }
        SG::WriteHandle<T> ContainerCacheKey(containerKey, ctx);
        ATH_CHECK( ContainerCacheKey.recordNonConst ( std::make_unique<T>(IdentifierHash(size), nullptr) ));
        ATH_MSG_DEBUG( "Container "<< containerKey.key() << " created to hold " << size );
        return StatusCode::SUCCESS;
    }

}


#endif //INDETPREPRAWDATAFORMATION_CACHECREATOR
