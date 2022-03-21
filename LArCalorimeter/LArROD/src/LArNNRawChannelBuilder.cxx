/*
   Copyright (C) 2022 CERN for the benefit of the ATLAS collaboration
 */

#include "LArNNRawChannelBuilder.h"
#include "LArROD/LArRawChannelBuilderAlg.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "LArRawEvent/LArRawChannelContainer.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCOOLConditions/LArDSPThresholdsFlat.h"
#include <cmath>

#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include <map>
#include <fstream>


LArNNRawChannelBuilder::LArNNRawChannelBuilder(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator) {
}


StatusCode LArNNRawChannelBuilder::initialize() {
  ATH_CHECK(m_digitKey.initialize());
  ATH_CHECK(m_cellKey.initialize(m_isSC));
  ATH_CHECK(m_rawChannelKey.initialize(!m_isSC));
  ATH_CHECK(m_pedestalKey.initialize());
  ATH_CHECK(m_adc2MeVKey.initialize());
  ATH_CHECK(m_cablingKey.initialize() );


  if ( m_isSC ) {
    const LArOnline_SuperCellID* ll;
    ATH_CHECK(detStore()->retrieve(ll, "LArOnline_SuperCellID"));
    m_onlineId = (const LArOnlineID_Base*)ll;
    ATH_CHECK( detStore()->retrieve (m_sem_mgr, "CaloSuperCellMgr") );
  }
  else {
    const LArOnlineID* ll;
    ATH_CHECK(detStore()->retrieve(ll, "LArOnlineID"));
    m_onlineId = (const LArOnlineID_Base*)ll;
  }


  return StatusCode::SUCCESS;
}


StatusCode LArNNRawChannelBuilder::execute(const EventContext& ctx) const {

  //Get event inputs from read handles:
  SG::ReadHandle<LArDigitContainer>inputContainer(m_digitKey, ctx);

  //Write output via write handle

  auto outputContainerLRPtr = std::make_unique<LArRawChannelContainer>();

  if (m_isSC) {
      ATH_MSG_ERROR("No energy computation for super cells");
      return StatusCode::FAILURE;

    }


  //Get Conditions input
  SG::ReadCondHandle<ILArPedestal>pedHdl(m_pedestalKey, ctx);
  const ILArPedestal* peds = *pedHdl;

  SG::ReadCondHandle<LArADC2MeV>adc2mevHdl(m_adc2MeVKey, ctx);
  const LArADC2MeV* adc2MeVs = *adc2mevHdl;

  SG::ReadCondHandle<LArOnOffIdMapping>cabling(m_cablingKey, ctx);

  //Loop over digits:
  for (const LArDigit* digit : *inputContainer) {

    const HWIdentifier id = digit->hardwareID();
    const bool connected = (*cabling)->isOnlineConnected(id);


    ATH_MSG_VERBOSE("Working on channel " << m_onlineId->channel_name(id));

    const std::vector<short>& samples = digit->samples();
    const size_t nSamples = samples.size();
    const int gain = digit->gain();
    const float p = peds->pedestal(id, gain);


    //The following autos will resolve either into vectors or vector-proxies
    const auto& adc2mev = adc2MeVs->ADC2MEV(id, gain);

    if (ATH_UNLIKELY(p == ILArPedestal::ERRORCODE)) {
      if (!connected) continue;       //No conditions for disconencted channel, who cares?
      ATH_MSG_ERROR("No valid pedestal for connected channel " << m_onlineId->channel_name(id)
                                                               << " gain " << gain);
      return StatusCode::FAILURE;
    }

    if (ATH_UNLIKELY(adc2mev.size() < 2)) {
      if (!connected) continue;       //No conditions for disconencted channel, who cares?
      ATH_MSG_ERROR("No valid ADC2MeV for connected channel " << m_onlineId->channel_name(id)
                                                              << " gain " << gain);
      return StatusCode::FAILURE;
    }

    // Compute amplitude
    float A = 0;
    bool saturated = false;

    // Check saturation AND discount pedestal
    std::vector<float>samp_no_ped(nSamples, 0.0);
    for (size_t i = 0; i < nSamples; ++i) {
      if (samples[i] == 4096 || samples[i] == 0) saturated = true;
      samp_no_ped[i] = samples[i]-p;
    }

    // LWTNN configuration
    std::map<std::string, std::map<std::string, double> >inputs;
    std::map<std::string, std::map<std::string, std::vector<double> > >input_sequences;


    // Read in the network file
    std::ifstream in_file(m_nn_json);


    std::vector<double>nn_in;

    nn_in.clear();

    for (auto d : samples) {

      nn_in.push_back((d-p)/4096.0);

    }


    auto config = lwt::parse_json_graph(in_file);
    lwt::LightweightGraph graph(config, m_network_output);

    // Define some sequences used for the recurrent NN
    input_sequences["node_0"] = {
      {m_input_node,  nn_in}
    };

    // More variables for the two other inputs defined
    inputs["node_0"] = {{"variable_0", 0}};

    // Calculate the output value of the NN based on the inputs given
    auto outputs = graph.compute(inputs, input_sequences);


    A = outputs.begin()->second;

    

    //Apply Ramp
    const float E = adc2mev[0]+A*adc2mev[1];

    uint16_t iquaShort = 0;
    float tau = 0;


    uint16_t prov = 0xa5;     //Means all constants from DB
    if (saturated) prov |= 0x0400;


    outputContainerLRPtr->emplace_back(id, static_cast<int>(std::floor(E+0.5)),
                                        static_cast<int>(std::floor(tau+0.5)),
                                        iquaShort, prov, (CaloGain::CaloGain)gain);
    
  }

  SG::WriteHandle<LArRawChannelContainer>outputContainer(m_rawChannelKey, ctx);
  ATH_CHECK(outputContainer.record(std::move(outputContainerLRPtr) ) );
  

  return StatusCode::SUCCESS;
}
