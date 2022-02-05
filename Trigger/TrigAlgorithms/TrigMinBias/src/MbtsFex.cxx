/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "MbtsFex.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "TileEvent/TileCellCollection.h"
#include "TileEvent/TileCell.h"

MbtsFex::MbtsFex(const std::string& name, ISvcLocator* pSvcLocator):
AthReentrantAlgorithm(name, pSvcLocator)
{

}

StatusCode MbtsFex::initialize()
{
  ATH_CHECK( m_emScaleKey.initialize() );
  ATH_CHECK( m_badChannelsKey.initialize() );
  ATH_CHECK( m_TileHelperKey.initialize() );
  ATH_CHECK( m_dataAccessSvc.retrieve() );
  ATH_CHECK(m_MbtsBitsKey.initialize());
  if (! m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );
  ATH_MSG_DEBUG("Init done");
  return StatusCode::SUCCESS;
}

StatusCode MbtsFex::execute(const EventContext& context) const
{

  TileCellCollection mbtsContainer (SG::VIEW_ELEMENTS);
  ATH_CHECK(m_dataAccessSvc->loadMBTS( context, mbtsContainer));
  ATH_MSG_DEBUG ("Successfully retrieved mbtsContainer collection of size " << mbtsContainer.size());

  SG::ReadHandle<TileTBID> TileHelper(m_TileHelperKey, context );
  std::vector<float> triggerEnergies(xAOD::TrigT2MbtsBits::NUM_MBTS, 0.);
  std::vector<float> triggerTimes(xAOD::TrigT2MbtsBits::NUM_MBTS, 0.);

  for(auto mbtsTile: mbtsContainer ){
    Identifier id=mbtsTile->ID();

    // MBTS Id type is  "side"  +/- 1
    int type_id = TileHelper->type(id);

    // MBTS Id channel is "eta"  0-1   zero is closer to beam pipe
    unsigned int channel_id = TileHelper->channel(id);

    // MBTS Id module is "phi"  0-7
    unsigned int module_id = TileHelper->module(id);

    ATH_MSG_DEBUG("type_id = " << type_id<<" channel_id = " << channel_id<<" module_id = " << module_id);

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

    unsigned int bit_pos = 0; // The position of the bit

    if(type_id == -1) { // C-side
      bit_pos += 16;
    }

    bit_pos += channel_id*8;
    bit_pos += module_id;

    ATH_CHECK(bit_pos < triggerEnergies.size());
    triggerEnergies[bit_pos] = mbtsTile->energy();
    ATH_MSG_DEBUG("Counter id = " << bit_pos << ", energy = " << triggerEnergies[bit_pos] << " pC");


    ATH_CHECK(bit_pos < triggerTimes.size());
    triggerTimes[bit_pos] = mbtsTile->time();
    ATH_MSG_DEBUG("Counter id = " << bit_pos << ", time = " << triggerTimes[bit_pos] << " ns");

  }
  // channel IDs are deined in this tedious way in order to allow for a quick remappings or channels removal if needed
  // e.g. entering -1 in any position will move that channel entries out of scope of the monitoring histograms
  static const std::vector<int> channelID({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, 
                                           16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31});
  auto mon_triggerEnergies = Monitored::Collection("triggerEnergies",triggerEnergies);
  auto mon_channelID       = Monitored::Collection("channelID", channelID);
  auto mon_triggerTimes = Monitored::Collection("triggerTimes",triggerTimes);
  double weightedTime = 0; 
  double weights = 0;

  for ( size_t i = 0; i < xAOD::TrigT2MbtsBits::NUM_MBTS; ++i) {
    weightedTime += triggerTimes[i] * triggerEnergies[i];
    weights += triggerEnergies[i];
  }  
  const double timeDelta = (weights == 0 ? -999.0 : weightedTime/weights);
  auto mon_weightedTimeDelta = Monitored::Scalar("timeDelta",  timeDelta);

  Monitored::Group(m_monTool, mon_triggerEnergies, mon_channelID, mon_triggerTimes, mon_weightedTimeDelta);

  SG::WriteHandle<xAOD::TrigT2MbtsBitsContainer> mbtsHandle (m_MbtsBitsKey, context);

  auto trigMbtsContainer = std::make_unique< xAOD::TrigT2MbtsBitsContainer>();
  auto trigMbtsAuxContainer = std::make_unique< xAOD::TrigT2MbtsBitsAuxContainer>();
  trigMbtsContainer->setStore(trigMbtsAuxContainer.get());

  xAOD::TrigT2MbtsBits * mbtsObj = new xAOD::TrigT2MbtsBits();
  trigMbtsContainer->push_back(mbtsObj);

  mbtsObj->setTriggerEnergies(triggerEnergies);
  mbtsObj->setTriggerTimes(triggerTimes);

  ATH_MSG_DEBUG("Recording " << mbtsHandle.key());
  ATH_CHECK(mbtsHandle.record( std::move(trigMbtsContainer), std::move( trigMbtsAuxContainer ) ) );
  return StatusCode::SUCCESS;
}
