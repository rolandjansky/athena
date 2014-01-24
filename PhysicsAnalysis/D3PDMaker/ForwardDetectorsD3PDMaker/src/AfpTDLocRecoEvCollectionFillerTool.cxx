/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// Local include(s):
#include "AfpTDLocRecoEvCollectionFillerTool.h"

namespace D3PD {

    AfpTDLocRecoEvCollectionFillerTool::AfpTDLocRecoEvCollectionFillerTool( const std::string& type, const std::string& name, const IInterface* parent)
        : AfpTDLocRecoEvCollectionFillerBase( type, name, parent )
    {
        ATH_MSG_INFO( "AfpTDLocRecoEvCollectionFillerTool---> Constructor" );

        m_pnTotTracksCnt=NULL;
        m_pvecTrackStationID=NULL;
        m_pvecTrackQuarticID=NULL;
        m_pvecTrackTrainID=NULL;
        m_pvecTrackTrainTime=NULL;
        m_pvecTrackTrainAmplitude=NULL;
        m_pvecTrackSaturatedBars=NULL;
    }

    StatusCode AfpTDLocRecoEvCollectionFillerTool::initialize()
    {
        // Initialize the base class:
        CHECK( AfpTDLocRecoEvCollectionFillerBase::initialize() );
        ATH_MSG_INFO( "AfpTDLocRecoEvCollectionFillerTool---> Initialized" );

        return StatusCode::SUCCESS;
    }

    StatusCode AfpTDLocRecoEvCollectionFillerTool::book()
    {
        ATH_MSG_DEBUG( "AfpTDLocRecoEvCollectionFillerTool---> Booking Histograms" );

        CHECK(addVariable("tid_tot_num_tracks",m_pnTotTracksCnt));
        CHECK(addVariable("tid_stID",m_pvecTrackStationID));
        CHECK(addVariable("tid_qID",m_pvecTrackQuarticID));
        CHECK(addVariable("tid_trainID",m_pvecTrackTrainID));
        CHECK(addVariable("tid_time",m_pvecTrackTrainTime));
        CHECK(addVariable("tid_amplitude",m_pvecTrackTrainAmplitude));
        CHECK(addVariable("tid_num_saturated_bars",m_pvecTrackSaturatedBars));

        return StatusCode::SUCCESS;
    }

    StatusCode AfpTDLocRecoEvCollectionFillerTool::fill(const AFP_TDLocRecoEvCollection& DataCollection)
    {
        int i;
        AFP_TDLocRecoEvCollection::const_iterator iterColl;

        ClearData(DataCollection);

        *m_pnTotTracksCnt=DataCollection.size();
        for(i=0,iterColl=DataCollection.begin();iterColl!=DataCollection.end();i++,iterColl++)
        {
            (*m_pvecTrackStationID)[i]=(*iterColl)->getStationID();
            (*m_pvecTrackQuarticID)[i]=(*iterColl)->getDetectorID();
            (*m_pvecTrackTrainID)[i]=(*iterColl)->getTrainID();
            (*m_pvecTrackTrainTime)[i]=(*iterColl)->getTrainTime();
            (*m_pvecTrackTrainAmplitude)[i]=(*iterColl)->getTrainSize();
            (*m_pvecTrackSaturatedBars)[i]=(*iterColl)->getSaturation();
        }

        return StatusCode::SUCCESS;
    }

    void AfpTDLocRecoEvCollectionFillerTool::ClearData(const AFP_TDLocRecoEvCollection& DataCollection)
    {
        int nItemsCnt;

        nItemsCnt=DataCollection.size();
        *m_pnTotTracksCnt=-1;
        m_pvecTrackStationID->resize(nItemsCnt);
        fill_n(m_pvecTrackStationID->begin(),m_pvecTrackStationID->size(),UNDEFINTVALUE);
        m_pvecTrackQuarticID->resize(nItemsCnt);
        fill_n(m_pvecTrackQuarticID->begin(),m_pvecTrackQuarticID->size(),UNDEFINTVALUE);
        m_pvecTrackTrainID->resize(nItemsCnt);
        fill_n(m_pvecTrackTrainID->begin(),m_pvecTrackTrainID->size(),UNDEFINTVALUE);
        m_pvecTrackTrainTime->resize(nItemsCnt);
        fill_n(m_pvecTrackTrainTime->begin(),m_pvecTrackTrainTime->size(),UNDEFFLOATVALUE);
        m_pvecTrackTrainAmplitude->resize(nItemsCnt);
        fill_n(m_pvecTrackTrainAmplitude->begin(),m_pvecTrackTrainAmplitude->size(),UNDEFFLOATVALUE);
        m_pvecTrackSaturatedBars->resize(nItemsCnt);
        fill_n(m_pvecTrackSaturatedBars->begin(),m_pvecTrackSaturatedBars->size(),UNDEFINTVALUE);
    }

} // namespace D3PD

