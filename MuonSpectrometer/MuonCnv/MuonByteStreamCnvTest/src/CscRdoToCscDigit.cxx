/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscRdoToCscDigit.h"
#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

CscRdoToCscDigit::CscRdoToCscDigit(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode CscRdoToCscDigit::initialize()
{
  ATH_CHECK(m_idHelperSvc.retrieve());
  /** CSC calibration tool for the Condtiions Data base access */
  ATH_CHECK(m_cscCalibTool.retrieve());
  ATH_CHECK(m_cscRdoKey.initialize());
  ATH_CHECK(m_cscDigitKey.initialize());
  ATH_CHECK(m_cscRdoDecoderTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode CscRdoToCscDigit::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "in execute()"  );
  // retrieve the collection of RDO
  SG::ReadHandle<CscRawDataContainer> rdoRH(m_cscRdoKey, ctx);
  if (!rdoRH.isValid()) {
    ATH_MSG_WARNING( "No CSC RDO container found!"  );
    return StatusCode::SUCCESS;
  }
  const CscRawDataContainer* rdoContainer=rdoRH.cptr();
  ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " CSC RDOs." );

  SG::WriteHandle<CscDigitContainer> wh_cscDigit(m_cscDigitKey, ctx);
  ATH_CHECK(wh_cscDigit.record(std::make_unique<CscDigitContainer> (m_idHelperSvc->cscIdHelper().module_hash_max())));
  ATH_MSG_DEBUG( "Decoding CSC RDO into CSC Digit"  );

  Identifier oldId;
  CscDigitCollection * collection(nullptr);
  // now decode RDO into digits
  auto it_rdoColl = rdoContainer->begin();
  auto lastRdoColl = rdoContainer->end();

  for (; it_rdoColl!=lastRdoColl; ++it_rdoColl) {
    ATH_CHECK(this->decodeCsc(*it_rdoColl, wh_cscDigit.ptr(), collection, oldId));
  }

  return StatusCode::SUCCESS;
}

StatusCode CscRdoToCscDigit::decodeCsc(const CscRawDataCollection *rdoColl, CscDigitContainer *cscContainer, CscDigitCollection*& collection,
                                       Identifier& oldId) const
{
  ATH_MSG_DEBUG( " Number of RawData in this rdo "
                 << rdoColl->size()  );

  const double samplingTime = rdoColl->rate();
  const unsigned int numSamples   = rdoColl->numSamples();

  //      decoder.setParams(samplingTime);

  const IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();

  /** for each Rdo, loop over RawData, converter RawData to digit
      retrieve/create digit collection, and insert digit into collection */
  for (const CscRawData * data : *rdoColl) {
    if (!data) {
      ATH_MSG_WARNING("NULL pointer to Digit!");
      continue;
    }
    const uint16_t width = data->width();
    Identifier stationId = m_cscRdoDecoderTool->stationIdentifier(data);
    for (int j=0; j<width; ++j) {
      Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, j);
      std::vector<uint16_t> samples;
      const bool extractSamples = data->samples(j, numSamples, samples);
      if (!extractSamples) {
        ATH_MSG_WARNING( "Unable to extract samples for strip " << j
                         << " Online Cluster width = " << width
                         << " for number of Samples = " << numSamples
                         << " continuing ..."  );
        continue;
      }
      std::vector<float> charges;
      const bool adctocharge = m_cscCalibTool->adcToCharge(samples, data->hashId(), charges);
      double charge = 0.0;
      double time   = 0.0;
      unsigned int samplingPhase = 0;
      const int errorfindCharge = m_cscCalibTool->findCharge(samplingTime, samplingPhase, charges, charge, time);
      // WPARK: Potential problem! error code includes time out of range and aa>0
      // If this is the case, only very nice samples are converted to Digit. Sep 11, 2009
      if ( !adctocharge || !errorfindCharge ) {
        ATH_MSG_WARNING( " CSC conversion ADC to Charge failed "
                         << "CSC Digit not build ... "
                         << " or charge finding failed " << " ... skipping "  );
        continue;
      }
      ATH_MSG_DEBUG( "CSC RDO->CscDigit: " << m_idHelperSvc->cscIdHelper().show_to_string(channelId) );
      const int theCharge = static_cast<int>(charge);
      CscDigit * newDigit = new CscDigit(channelId, theCharge, time);
      ATH_MSG_DEBUG( "CSC RDO->Digit: " << m_idHelperSvc->cscIdHelper().show_to_string(newDigit->identify())
                     << " " << newDigit->charge() << " " << charge << " time= " << time );

      for (uint16_t i=0; i< samples.size(); ++i) {
        ATH_MSG_DEBUG( "CSC RDO->Digit: " << samples[i] );
      }
      IdentifierHash coll_hash;
      if (m_idHelperSvc->cscIdHelper().get_hash(stationId, coll_hash, &cscContext)) {
        ATH_MSG_WARNING( "Unable to get CSC digiti collection hash id "
                         << "context begin_index = " << cscContext.begin_index()
                         << " context end_index  = " << cscContext.end_index()
                         << " the identifier is " );
        stationId.show();
      }

      if (oldId != stationId) {
        const CscDigitCollection * coll = cscContainer->indexFindPtr(coll_hash);
        if (nullptr ==  coll) {
          CscDigitCollection *newCollection = new CscDigitCollection(stationId, coll_hash);
          newCollection->push_back(newDigit);
          collection = newCollection;
          if (cscContainer->addCollection(newCollection, coll_hash).isFailure()) {
            ATH_MSG_WARNING( "Couldn't record CscDigitCollection with key=" << coll_hash
                             << " in StoreGate!"  );
          }
        }
        else {
          CscDigitCollection *oldCollection ATLAS_THREAD_SAFE = const_cast<CscDigitCollection*>( coll ); // FIXME
          oldCollection->push_back(newDigit);
          collection = oldCollection;
        }
        oldId = stationId;
      }
      else {
        collection->push_back(newDigit);
      }
    }
  }
  return StatusCode::SUCCESS;
}
