
/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "CompareCells.h"
#include "CaloEvent/CaloConstCellContainer.h"

#include <iostream>
CompareCells::CompareCells( const std::string& name,
                                        ISvcLocator* pSvcLocator ) :
  ::AthReentrantAlgorithm( name, pSvcLocator ),
  m_dataAccessSvc( "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", name )
{
  declareProperty("TrigDataAccessMT",m_dataAccessSvc,"Data Access for LVL2 Calo Algorithms in MT");
  m_calocellcollectionKey = "AllCalo";
}

StatusCode CompareCells::initialize() {
  CHECK( m_dataAccessSvc.retrieve() );
  CHECK( m_calocellcollectionKey.initialize() );
  CHECK( m_bcidAvgKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode CompareCells::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  CaloConstCellContainer c(SG::VIEW_ELEMENTS);
  m_dataAccessSvc->loadFullCollections( context, c).ignore();

  TileCellCollection mbts (SG::VIEW_ELEMENTS); // testing
  m_dataAccessSvc->loadMBTS( context, mbts).ignore();

  ATH_MSG_DEBUG("MBTS Dump");
  for (size_t i=0;i<mbts.size(); ++i)
     ATH_MSG_DEBUG(mbts[i]->eta() << " " << mbts[i]->phi() << " " << mbts[i]->caloDDE()->getSampling() << " " << mbts[i]->energy() );


  SG::ReadHandle<CaloCellContainer> off(m_calocellcollectionKey,context);

  const EventIDBase eventInfo = context.eventID();
  uint32_t lumiBlock = eventInfo.lumi_block();
  uint32_t runNumber = eventInfo.run_number();
  uint32_t bcid = eventInfo.bunch_crossing_id();
  uint32_t eventn = eventInfo.event_number();

  ATH_MSG_DEBUG("EVENT INFO : " << lumiBlock << " " << runNumber << " " << bcid << " " << eventn); 

  float max_diff=-999.0;
  float max_diff_restricted_et=-999.0;
  float max_diff_restricted_samp=-999.0;
  float mean_diff=0.0;
  float mean_sum1=0.0;
  float mean_sum2=0.0;
  float mean_diff_n=0.0;
  float mean_diff_restricted_et=0.0;
  float mean_sum1_restricted_et=0.0;
  float mean_sum2_restricted_et=0.0;
  float mean_diff_restricted_et_n=0.0;
  float mean_diff_restricted_samp=0.0;
  float mean_sum1_restricted_samp=0.0;
  float mean_sum2_restricted_samp=0.0;
  float mean_diff_restricted_samp_n=0.0;
  for( auto offcell : *off ) {
	Identifier id = offcell->ID();
	if ( offcell->caloDDE()->is_tile() ) continue;
	for ( auto hltcell : c) {
		const CaloDetDescrElement* hltdde = hltcell->caloDDE();
		if ( hltdde && hltdde->is_tile() ) continue;
		if ( id == hltcell->ID() ) {
		   int samp = hltdde->getSampling();
		   if ( offcell->et() > 5e3 )
			ATH_MSG_DEBUG("CHECK Cell " << hltcell->energy() << " " << offcell->energy() << " " << hltcell->eta() << " " << hltcell->phi() << " " << samp);
		   float diff = hltcell->energy() - offcell->energy();
		   float diff_et = hltcell->et() - offcell->et();
		   if ( diff < 0 ) diff = -diff;
		   if ( diff > max_diff ) max_diff = diff;
		   mean_diff += diff;
		   mean_sum1 += hltcell->energy();
		   mean_sum2 += offcell->energy();
		   mean_diff_n++;
		   if ( diff_et > 2e3 ) 
			ATH_MSG_DEBUG("CHECK Diff et " << diff_et << " " << hltcell->energy() << " " << offcell->energy() << " " << hltcell->eta() << " " << hltcell->phi() << " " << samp);
		   if ( diff > 10e3 ) 
			ATH_MSG_DEBUG("CHECK Diff " << diff << " " << hltcell->energy() << " " << offcell->energy() << " " << hltcell->eta() << " " << hltcell->phi() << " " << samp);
		   if ( offcell->et() < 3e3 ) {
			if ( diff > max_diff_restricted_et ) max_diff_restricted_et = diff;
			mean_sum1_restricted_et+=hltcell->energy();
			mean_sum2_restricted_et+=offcell->energy();
			mean_diff_restricted_et+=diff;
			mean_diff_restricted_et_n++;
		   }
		   if ( samp < 21 ) {
			if ( diff > max_diff_restricted_samp ) max_diff_restricted_samp = diff;
			mean_sum1_restricted_samp+=hltcell->energy();
			mean_sum2_restricted_samp+=offcell->energy();
			mean_diff_restricted_samp+=diff;
			mean_diff_restricted_samp_n++;
		   }
		   break;
		} // end of if hltcell ID
	} // end of for hltcell
  } // end of for offcell
  ATH_MSG_DEBUG( "RESULTS : " << bcid << " " << eventn << " " << max_diff << " " << max_diff_restricted_et << " " << mean_diff << " " << mean_diff_n << " " << mean_diff_restricted_et << " " << mean_diff_restricted_et_n << " " << mean_diff_restricted_samp << " " << mean_diff_restricted_samp_n << " " << mean_sum1 << " " << mean_sum2 << " " << mean_sum1_restricted_et << " " << mean_sum2_restricted_et << " " << mean_sum1_restricted_samp << " " << mean_sum2_restricted_samp); 
  

  return StatusCode::SUCCESS;

}
