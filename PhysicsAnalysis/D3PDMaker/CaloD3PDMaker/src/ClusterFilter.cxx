/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "CaloEvent/CaloClusterContainer.h"

// Local include(s):
#include "ClusterFilter.h"

#include "DataModel/ConstDataVector.h"

ClusterFilter::ClusterFilter( const std::string& name, 
                              ISvcLocator* pSvcLocator)
   : AthAlgorithm( name, pSvcLocator ), 
     m_inputName( "CaloCalTopoCluster" ),
     m_outputName( "SelectedClusters" ),
     m_ptCut( 5000. ),
     m_etaCut( 5. ) {

   declareProperty( "InputName", m_inputName );
   declareProperty( "OutputName", m_outputName );
   declareProperty( "ptCut", m_ptCut );
   declareProperty( "etaCut", m_etaCut );
}

StatusCode ClusterFilter::initialize() {

   ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
   ATH_MSG_DEBUG( "  InputName  : " << m_inputName );
   ATH_MSG_DEBUG( "  OutputName : " << m_outputName );
   ATH_MSG_DEBUG( "  pT cut     : " << m_ptCut );
   ATH_MSG_DEBUG( "  eta cut    : " << m_etaCut );

   return StatusCode::SUCCESS;
}

StatusCode ClusterFilter::execute() {

   REPORT_MESSAGE( MSG::VERBOSE ) << " in execute";

   const CaloClusterContainer* cluscoll = 0;
   CHECK( evtStore()->retrieve( cluscoll, m_inputName ) );

   ConstDataVector<CaloClusterContainer>* outputCont =
     new ConstDataVector<CaloClusterContainer>( SG::VIEW_ELEMENTS );
   CHECK( evtStore()->record( outputCont, m_outputName) ); 

   CaloClusterContainer::const_iterator f_clus = cluscoll->begin();
   CaloClusterContainer::const_iterator l_clus = cluscoll->end();
   for ( ; f_clus != l_clus; ++f_clus ) {

      const CaloCluster* cluster = ( *f_clus );

      const float pt = cluster->pt();
      const float eta = cluster->eta();

      if( ( pt > m_ptCut ) && ( fabs( eta ) < m_etaCut ) ) {
        outputCont->push_back (cluster);
      }
   }

   return StatusCode::SUCCESS;
}
