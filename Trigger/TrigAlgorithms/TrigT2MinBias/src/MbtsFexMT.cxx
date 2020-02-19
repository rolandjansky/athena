/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "MbtsFexMT.h"

MbtsFexMT::MbtsFexMT(const std::string& name, ISvcLocator* pSvcLocator):
AthReentrantAlgorithm(name, pSvcLocator),
m_dataAccessSvc( "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", name )
{
// declareProperty("RoIs", m_roiCollectionKey = std::string("OutputRoIs"), "RoIs to read in");
//   declareProperty("CellsName", m_cellContainerKey = std::string("CellsClusters"), "Calo cells container");
  declareProperty("TrigDataAccessMT",m_dataAccessSvc,"Data Access for LVL2 Calo Algorithms in MT");
  m_calocellcollectionKey = "AllCalo";
}

MbtsFexMT::~MbtsFexMT()
{
}

StatusCode MbtsFexMT::initialize()
{
  ATH_CHECK( m_calocellcollectionKey.initialize() );
  ATH_CHECK(m_MbtsKey.initialize());
     ATH_CHECK( m_cellContainerKey.initialize() );
   ATH_CHECK( m_dataAccessSvc.retrieve() );
   if (! m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );
   return StatusCode::SUCCESS;
}
StatusCode MbtsFexMT::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode MbtsFexMT::execute(const EventContext& context) const
{

  CaloConstCellContainer c(SG::VIEW_ELEMENTS);
  m_dataAccessSvc->loadFullCollections( context, c).ignore();


  SG::ReadHandle<CaloCellContainer> CaloContainer(m_calocellcollectionKey,context);

  const EventIDBase eventInfo = context.eventID();
    uint32_t lumiBlock = eventInfo.lumi_block();
    uint32_t runNumber = eventInfo.run_number();
    uint32_t bcid = eventInfo.bunch_crossing_id();
    uint32_t eventn = eventInfo.event_number();

    ATH_MSG_DEBUG("EVENT INFO : " << lumiBlock << " " << runNumber << " " << bcid << " " << eventn);

    double triggerEnergies{};
    double triggerTimes{};
    double triggerEta{} ;
      double triggerPhi{} ;

	for ( auto CaloContainerIndx : *CaloContainer) {

  Identifier triggerID = CaloContainerIndx->ID();
	if ( CaloContainerIndx->caloDDE()->is_tile() ) continue;
	for ( auto hltcell : c) {
		const CaloDetDescrElement* hltdde = hltcell->caloDDE();
		if ( hltdde && hltdde->is_tile() ) continue;
		if ( triggerID == hltcell->ID() ) {
    triggerEnergies = CaloContainerIndx->energy();
triggerEta = CaloContainerIndx->eta();
triggerPhi = CaloContainerIndx->phi();

			ATH_MSG_DEBUG("Cell info: " << CaloContainerIndx->energy() << " " << CaloContainerIndx->energy() << " " << CaloContainerIndx->eta() << " " << CaloContainerIndx->phi());
}
}
  } // end of for CaloContainerIndx


  auto mon_triggerEnergies = Monitored::Scalar<double>("triggerEnergies",triggerEnergies);
  auto mon_triggerEta = Monitored::Scalar<double>("eta",triggerEta);
  auto mon_triggerPhi = Monitored::Scalar<double>("phi",triggerPhi);
  // auto mon_triggerTimes = Monitored::Scalar<int>("triggerTimes ",triggerTimes);
  Monitored::Group(m_monTool,mon_triggerEnergies,mon_triggerEta,mon_triggerPhi);

  // Recording Data
  SG::WriteHandle<xAOD::TrigCompositeContainer> mbtsHandle (m_MbtsKey, context);

  auto MbtsContainer = std::make_unique< xAOD::TrigCompositeContainer >();
  auto MbtsAuxContainer = std::make_unique< xAOD::TrigCompositeAuxContainer>();
  MbtsContainer->setStore(MbtsAuxContainer.get());


  xAOD::TrigComposite * mbts = new xAOD::TrigComposite();
  MbtsContainer->push_back(mbts);

  mbts->setDetail("triggerEnergies", triggerEnergies);
  // mbts->setDetail("triggerTimes",triggerTimes);
  mbts->setDetail("triggerPhi", triggerEta);
  mbts->setDetail("triggerEta", triggerPhi);



  ATH_CHECK(mbtsHandle.record( std::move( MbtsContainer), std::move( MbtsAuxContainer ) ) );




return StatusCode::SUCCESS;
}
