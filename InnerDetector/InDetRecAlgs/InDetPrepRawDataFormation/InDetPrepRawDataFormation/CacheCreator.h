/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "SCT_ConditionsData/SCT_MonitorConditionsCondData.h"
#include "SCT_ConditionsData/SCT_TdaqEnabledCondData.h"

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
        virtual StatusCode execute_r (const EventContext& ctx) const override;
        //No need for finalize
    protected:
        const TRT_ID* m_pTRTHelper;
        const PixelID* m_pix_idHelper;
        const SCT_ID*  m_sct_idHelper;
        SG::WriteHandleKey<InDet::TRT_DriftCircleContainerCache> m_rioContainerCacheKey;
        SG::WriteHandleKey<SCT_ClusterContainerCache>            m_SCTclusterContainerCacheKey;
        SG::WriteHandleKey<InDet::PixelClusterContainerCache>    m_PIXclusterContainerCacheKey;
        SG::WriteHandleKey<SpacePointCache>    m_PIXSpacePointCacheKey;
        SG::WriteHandleKey<SpacePointCache>    m_SCTSpacePointCacheKey;
        bool m_disableTRT;
        SG::ReadCondHandleKey<SCT_MonitorConditionsCondData> m_condKey;//Temporary workarounds for problem in scheduler - remove later
        SG::ReadCondHandleKey<SCT_TdaqEnabledCondData> m_condKey2;

        template<typename T>
        StatusCode CreateContainer(const SG::WriteHandleKey<T>& , long unsigned int , const EventContext& ) const;
    };

    template<typename T>
    StatusCode CacheCreator::CreateContainer(const SG::WriteHandleKey<T>& containerKey, long unsigned int size, const EventContext& ctx) const{
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
