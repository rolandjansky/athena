/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "MbtsFexMT.h"

MbtsFexMT::MbtsFexMT(const std::string& name, ISvcLocator* pSvcLocator):
AthReentrantAlgorithm(name, pSvcLocator),
m_dataAccessSvc( "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", name )
{
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
  ATH_CHECK( m_TileHelperKey.initialize() );
  ATH_CHECK( m_dataAccessSvc.retrieve() );
  CHECK( m_tileDecoder.retrieve() );
  CHECK( m_bcidAvgKey.initialize() );

  if (! m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );
  return StatusCode::SUCCESS;
}
StatusCode MbtsFexMT::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode MbtsFexMT::execute(const EventContext& context) const
{

  TileCellCollection mbts (SG::VIEW_ELEMENTS); // testing
  m_dataAccessSvc->loadMBTS( context, mbts).ignore();
  ATH_MSG_DEBUG ("Successfully retrieved mbts collection of size " << mbts.size());

  CaloConstCellContainer cal(SG::VIEW_ELEMENTS);
  m_dataAccessSvc->loadFullCollections( context, cal).ignore();

  SG::ReadHandle<CaloCellContainer> CaloContainer(m_calocellcollectionKey,context);
  SG::ReadHandle<TileTBID> tileHelper(m_TileHelperKey, context );

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

    const TileCell* tilecell = dynamic_cast<const TileCell*> (CaloContainerIndx);

    Identifier triggerID = CaloContainerIndx->ID();
    if ( CaloContainerIndx->caloDDE()->is_tile() ) continue;

    Identifier id  = tilecell->ID();
    triggerTimes=tilecell->time();

// MBTS Id type is  "side"  +/- 1
int type_id = tileHelper->type(triggerID);

// MBTS Id channel is "eta"  0-1   zero is closer to beam pipe
unsigned int channel_id = tileHelper->channel(triggerID);

// MBTS Id module is "phi"  0-7
unsigned int module_id = tileHelper->module(triggerID);

// unsigned int nTriggerTimes = tileHelper.size();

ATH_MSG_DEBUG("type_id = " << type_id);
ATH_MSG_DEBUG("channel_id = " << channel_id);
ATH_MSG_DEBUG("module_id = " << module_id);

// Catch errors
if(abs(type_id) != 1) {
  ATH_MSG_WARNING("MBTS identifier type is out of range");
  continue;
}
if( channel_id > 1 ){
  ATH_MSG_WARNING("MBTS identifier channel is out of range");
  continue;
}
if( module_id > 7 ){
  ATH_MSG_WARNING("MBTS identifier module is out of range");
  continue;
}

for (size_t i=0;i<mbts.size(); ++i){
  ATH_MSG_DEBUG(tilecell->eta() << " " << tilecell->phi() << " " << tilecell->caloDDE()->getSampling() << " " << tilecell->energy() );}

  for ( auto hltcell : cal) {
    const CaloDetDescrElement* hltdde = hltcell->caloDDE();
    if ( hltdde && hltdde->is_tile() ) continue;
    if ( triggerID == hltcell->ID() ) {
      triggerEnergies = tilecell->energy();
      triggerEta = tilecell->eta();
      triggerPhi = tilecell->phi();

      ATH_MSG_DEBUG("Cell info: " << tilecell->energy() << " " << " " << tilecell->eta() << " " << tilecell->phi());


        }
      }
    } // end of for CaloContainerIndx


    auto mon_triggerEnergies = Monitored::Scalar<double>("triggerEnergies",triggerEnergies);
    auto mon_triggerEta = Monitored::Scalar<double>("eta",triggerEta);
    auto mon_triggerPhi = Monitored::Scalar<double>("phi",triggerPhi);
    auto mon_triggerTimes = Monitored::Scalar<int>("triggerTimes ",triggerTimes);
    Monitored::Group(m_monTool,mon_triggerEnergies,mon_triggerEta,mon_triggerPhi);

    // Recording Data
    SG::WriteHandle<xAOD::TrigCompositeContainer> mbtsHandle (m_MbtsKey, context);

    auto MbtsContainer = std::make_unique< xAOD::TrigCompositeContainer >();
    auto MbtsAuxContainer = std::make_unique< xAOD::TrigCompositeAuxContainer>();
    MbtsContainer->setStore(MbtsAuxContainer.get());

    xAOD::TrigComposite * MBTSCont = new xAOD::TrigComposite();
    MbtsContainer->push_back(MBTSCont);

    MBTSCont->setDetail("triggerEnergies", triggerEnergies);
    MBTSCont->setDetail("triggerTimes",triggerTimes);
    MBTSCont->setDetail("triggerPhi", triggerEta);
    MBTSCont->setDetail("triggerEta", triggerPhi);



    ATH_CHECK(mbtsHandle.record( std::move( MbtsContainer), std::move( MbtsAuxContainer ) ) );




    return StatusCode::SUCCESS;
  }
