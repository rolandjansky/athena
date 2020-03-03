/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "MbtsFexMT.h"

MbtsFexMT::MbtsFexMT(const std::string& name, ISvcLocator* pSvcLocator):
AthReentrantAlgorithm(name, pSvcLocator),
m_dataAccessSvc( "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", name )
{
  declareProperty("TrigDataAccessMT",m_dataAccessSvc,"Data Access for LVL2 Calo Algorithms in MT");
}

MbtsFexMT::~MbtsFexMT()
{
}
StatusCode MbtsFexMT::initialize()
{
   ATH_CHECK(m_MbtsKey.initialize());
  ATH_CHECK( m_TileHelperKey.initialize() );
  ATH_CHECK( m_dataAccessSvc.retrieve() );
  ATH_CHECK(m_t2MbtsBits.initialize());
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

SG::ReadHandle<TileTBID> TileHelperKey(m_TileHelperKey, context );

  const EventIDBase eventInfo = context.eventID();

  uint32_t lumiBlock = eventInfo.lumi_block();
  uint32_t runNumber = eventInfo.run_number();
  uint32_t bcid = eventInfo.bunch_crossing_id();
  uint32_t eventn = eventInfo.event_number();

  ATH_MSG_DEBUG("EVENT INFO : " << lumiBlock << " " << runNumber << " " << bcid << " " << eventn);


    uint32_t bit_pos;

  double triggerEnergy{};
  double triggerTime{};
  double triggerEta{} ;
  double triggerPhi{} ;

SG::WriteHandle<xAOD::TrigT2MbtsBits> mbtsHandle (m_MbtsKey, context);

std::vector<float> triggerEnergies;
std::vector<float> triggerTimes;
std::vector<int> triggerID;

for(auto mbtsCont: mbts ){
  Identifier id=mbtsCont->ID();

  // MBTS Id type is  "side"  +/- 1
  int type_id = TileHelperKey->type(id);

  // MBTS Id channel is "eta"  0-1   zero is closer to beam pipe
  unsigned int channel_id = TileHelperKey->channel(id);

  // MBTS Id module is "phi"  0-7
  unsigned int module_id = TileHelperKey->module(id);

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

  bit_pos = 0; // The position of the bit

  if(type_id == -1) { // C-side
    bit_pos += 16;
  }

  bit_pos += channel_id*8;
  bit_pos += module_id;

  unsigned int nTriggerEnergies = triggerEnergies.size();
  if(bit_pos > (nTriggerEnergies-1)) {
    ATH_MSG_ERROR("Bit pos " << bit_pos << " is greater than the size of the energy vector " << nTriggerEnergies);
  }
  else {
    triggerEnergies[bit_pos] = mbtsCont->energy();
    triggerID[bit_pos] = bit_pos;
    ATH_MSG_DEBUG("Counter id = " << bit_pos << ", energy = " << triggerEnergies[bit_pos] << " pC");
  }

  unsigned int nTriggerTimes = triggerTimes.size();
  if(bit_pos > (nTriggerTimes-1)) {
    ATH_MSG_ERROR("Bit pos " << bit_pos << " is greater than the size of the time vector " << nTriggerTimes);
  }
  else {
    triggerTimes[bit_pos] = mbtsCont->time();
    ATH_MSG_DEBUG("Counter id = " << bit_pos << ", time = " << triggerTimes[bit_pos] << " ns");
  }
}
    auto mon_triggerEnergies = Monitored::Scalar<double>("triggerEnergy",triggerEnergy);
    auto mon_triggerEta = Monitored::Scalar<double>("eta",triggerEta);
    auto mon_triggerPhi = Monitored::Scalar<double>("phi",triggerPhi);
    auto mon_triggerTimes = Monitored::Scalar<int>("triggerTime ",triggerTime);
    Monitored::Group(m_monTool,mon_triggerEnergies,mon_triggerEta,mon_triggerPhi);

     auto MbtsTrigMbtsContainer = std::make_unique< xAOD::TrigT2MbtsBits>();
     auto MbtsTrigMbtsAuxContainer = std::make_unique< xAOD::TrigT2MbtsBitsAuxContainer>();
     MbtsTrigMbtsContainer->setStore(MbtsTrigMbtsAuxContainer.get());

    mbtsHandle->makePrivateStore();
    mbtsHandle->setTriggerEnergies(triggerEnergies);
    mbtsHandle->setTriggerTimes(triggerTimes);

     ATH_CHECK(mbtsHandle.record( std::move( MbtsTrigMbtsContainer), std::move( MbtsTrigMbtsAuxContainer ) ) );
    return StatusCode::SUCCESS;
  }
