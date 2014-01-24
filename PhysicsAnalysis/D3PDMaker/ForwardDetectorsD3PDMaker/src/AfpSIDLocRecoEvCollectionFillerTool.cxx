/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// Local include(s):
#include "AfpSIDLocRecoEvCollectionFillerTool.h"

namespace D3PD {

    AfpSIDLocRecoEvCollectionFillerTool::AfpSIDLocRecoEvCollectionFillerTool( const std::string& type, const std::string& name, const IInterface* parent)
        : AfpSIDLocRecoEvCollectionFillerBase( type, name, parent )
    {
        ATH_MSG_INFO( "AfpSIDLocRecoEvCollectionFillerTool---> Constructor" );

        m_pnTotTracksCnt=NULL;
        m_pvecTrackStationID=NULL;
        m_pvecTrackXPos=NULL;
        m_pvecTrackYPos=NULL;
        m_pvecTrackZPos=NULL;
        m_pvecTrackXSlope=NULL;
        m_pvecTrackYSlope=NULL;
        m_pvecTrackUsedPixels=NULL;
        m_pvecTrackHoles=NULL;
        m_pvecTrackQuality=NULL;
    }

    StatusCode AfpSIDLocRecoEvCollectionFillerTool::initialize()
    {
        // Initialize the base class:
        CHECK( AfpSIDLocRecoEvCollectionFillerBase::initialize() );
        ATH_MSG_INFO( "AfpSIDLocRecoEvCollectionFillerTool---> Initialized" );

        return StatusCode::SUCCESS;
    }

    StatusCode AfpSIDLocRecoEvCollectionFillerTool::book()
    {
        ATH_MSG_DEBUG( "AfpSIDLocRecoEvCollectionFillerTool---> Booking Histograms" );

        CHECK(addVariable("trk_tot_num_tracks",m_pnTotTracksCnt));
        CHECK(addVariable("trk_stID",m_pvecTrackStationID));
        CHECK(addVariable("trk_x",m_pvecTrackXPos));
        CHECK(addVariable("trk_y",m_pvecTrackYPos));
        CHECK(addVariable("trk_z",m_pvecTrackZPos));
        CHECK(addVariable("trk_xslope",m_pvecTrackXSlope));
        CHECK(addVariable("trk_yslope",m_pvecTrackYSlope));
        CHECK(addVariable("trk_npix",m_pvecTrackUsedPixels));
        CHECK(addVariable("trk_nholes",m_pvecTrackHoles));
        CHECK(addVariable("trk_quality",m_pvecTrackQuality));

        return StatusCode::SUCCESS;
    }

    StatusCode AfpSIDLocRecoEvCollectionFillerTool::fill(const AFP_SIDLocRecoEvCollection& DataCollection)
    {
        int i;
        AFP_SIDLocRecoEvCollection::const_iterator iterColl;

        ClearData(DataCollection);

        *m_pnTotTracksCnt=DataCollection.size();
        for(i=0,iterColl=DataCollection.begin();iterColl!=DataCollection.end();i++,iterColl++)
        {
            (*m_pvecTrackStationID)[i]=(*iterColl)->getStationID();
            (*m_pvecTrackXPos)[i]=(*iterColl)->getXposition();
            (*m_pvecTrackYPos)[i]=(*iterColl)->getYposition();
            (*m_pvecTrackZPos)[i]=(*iterColl)->getZposition();
            (*m_pvecTrackXSlope)[i]=(*iterColl)->getXslope();
            (*m_pvecTrackYSlope)[i]=(*iterColl)->getYslope();
            (*m_pvecTrackUsedPixels)[i]=(*iterColl)->getNHits();
            (*m_pvecTrackHoles)[i]=(*iterColl)->getNHoles();
            (*m_pvecTrackQuality)[i]=(*iterColl)->getChi2();
        }

        return StatusCode::SUCCESS;
    }

    void AfpSIDLocRecoEvCollectionFillerTool::ClearData(const AFP_SIDLocRecoEvCollection& DataCollection)
    {
        int nItemsCnt;

        nItemsCnt=DataCollection.size();
        *m_pnTotTracksCnt=-1;
        m_pvecTrackStationID->resize(nItemsCnt);
        fill_n(m_pvecTrackStationID->begin(),m_pvecTrackStationID->size(),-1);
        m_pvecTrackXPos->resize(nItemsCnt);
        fill_n(m_pvecTrackXPos->begin(),m_pvecTrackXPos->size(),UNDEFFLOATVALUE);
        m_pvecTrackYPos->resize(nItemsCnt);
        fill_n(m_pvecTrackYPos->begin(),m_pvecTrackYPos->size(),UNDEFFLOATVALUE);
        m_pvecTrackZPos->resize(nItemsCnt);
        fill_n(m_pvecTrackZPos->begin(),m_pvecTrackZPos->size(),UNDEFFLOATVALUE);
        m_pvecTrackXSlope->resize(nItemsCnt);
        fill_n(m_pvecTrackXSlope->begin(),m_pvecTrackXSlope->size(),UNDEFFLOATVALUE);
        m_pvecTrackYSlope->resize(nItemsCnt);
        fill_n(m_pvecTrackYSlope->begin(),m_pvecTrackYSlope->size(),UNDEFFLOATVALUE);
        m_pvecTrackUsedPixels->resize(nItemsCnt);
        fill_n(m_pvecTrackUsedPixels->begin(),m_pvecTrackUsedPixels->size(),-1);
        m_pvecTrackHoles->resize(nItemsCnt);
        fill_n(m_pvecTrackHoles->begin(),m_pvecTrackHoles->size(),-1);
        m_pvecTrackQuality->resize(nItemsCnt);
        fill_n(m_pvecTrackQuality->begin(),m_pvecTrackQuality->size(),UNDEFFLOATVALUE);
    }

} // namespace D3PD

