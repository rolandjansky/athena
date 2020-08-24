/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**
* @file CacheCreator.h
* @author a.barton
*/

#ifndef INDETPREPRAWDATAFORMATION_CACHECREATOR
#define INDETPREPRAWDATAFORMATION_CACHECREATOR

#include "ViewAlgs/IDCCacheCreatorBase.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"

class TRT_ID;
class PixelID;
class SCT_ID;

namespace InDet{

    class CacheCreator : public IDCCacheCreatorBase
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
	SG::WriteHandleKey<IDCInDetBSErrContainer_Cache> m_SCTBSErrCacheKey
	  {this, "SCTBSErrCacheKey", ""};
        SG::WriteHandleKey<PixelRDO_Cache>     m_PixRDOCacheKey
          {this, "PixRDOCacheKey", ""};
	SG::WriteHandleKey<IDCInDetBSErrContainer_Cache>  m_PixBSErrCacheKey
	  {this, "PixBSErrCacheKey", ""};

        BooleanProperty m_disableTRT{this, "disableTRT", false};
        BooleanProperty m_disableWarning{this, "DisableViewWarning", false};

    };

}


#endif //INDETPREPRAWDATAFORMATION_CACHECREATOR
