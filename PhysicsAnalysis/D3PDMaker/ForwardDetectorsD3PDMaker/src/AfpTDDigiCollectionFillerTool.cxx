/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// Local include(s):
#include "AfpTDDigiCollectionFillerTool.h"

namespace D3PD {

    AfpTDDigiCollectionFillerTool::AfpTDDigiCollectionFillerTool( const std::string& type, const std::string& name, const IInterface* parent)
        : AfpTDDigiCollectionFillerBase( type, name, parent )
    {
        ATH_MSG_INFO( "AfpTDDigiCollectionFillerTool---> Constructor" );

        //m_pvecXDetCS=NULL;
        //m_pnMaxTrackCnt=NULL;
    }

    StatusCode AfpTDDigiCollectionFillerTool::initialize()
    {
        // Initialize the base class:
        CHECK( AfpTDDigiCollectionFillerBase::initialize() );
        ATH_MSG_INFO( "AfpTDDigiCollectionFillerTool---> Initialized" );

        return StatusCode::SUCCESS;
    }

    StatusCode AfpTDDigiCollectionFillerTool::book()
    {
        ATH_MSG_DEBUG( "AfpTDDigiCollectionFillerTool---> Booking Histograms" );

        //CHECK( addVariable ("x_Det", m_pvecXDetCS));
        //CHECK( addVariable ("NumTrack", m_pnMaxTrackCnt));

        return StatusCode::SUCCESS;
    }

    StatusCode AfpTDDigiCollectionFillerTool::fill(const AFP_TDDigiCollection& /*DataCollection*/)
    {
        ClearData();

        return StatusCode::SUCCESS;
    }

    void AfpTDDigiCollectionFillerTool::ClearData()
    {
        //*m_pnMaxTrackCnt=nMaxTrackCnt;
        //m_pvecXDetCS->resize(RPOTSCNT*nMaxTrackCnt);
        //fill_n(m_pvecXDetCS->begin(),m_pvecXDetCS->size(),-9999);
    }


} // namespace D3PD

