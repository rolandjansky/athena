/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_DigitToRDO.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

MM_DigitToRDO::MM_DigitToRDO(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode MM_DigitToRDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK( m_rdoContainer.initialize() );
  ATH_CHECK( m_digitContainer.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode MM_DigitToRDO::execute(const EventContext& ctx) const
{

  using namespace Muon;
  ATH_MSG_DEBUG( "in execute()"  );
  SG::WriteHandle<MM_RawDataContainer> rdos (m_rdoContainer, ctx);
  SG::ReadHandle<MmDigitContainer> digits (m_digitContainer, ctx);

  ATH_CHECK( rdos.record(std::make_unique<MM_RawDataContainer>(m_idHelperSvc->mmIdHelper().module_hash_max())) );

  if (digits.isValid()){
    for (const MmDigitCollection* digitColl : *digits ){

      // the identifier of the digit collection is the detector element Id ( multilayer granularity )
      Identifier digitId = digitColl->identify();

      // get the hash of the RDO collection
      IdentifierHash hash;
      int getRdoCollHash = m_idHelperSvc->mmIdHelper().get_module_hash(digitId,hash);
      if ( getRdoCollHash!=0 ) {
        ATH_MSG_ERROR("Could not get the module hash Id");
        return StatusCode::FAILURE;
      }

      MM_RawDataCollection* coll = new MM_RawDataCollection(hash);
      if (rdos->addCollection(coll,hash).isFailure() ){
        ATH_MSG_WARNING("Failed to add collection with hash " << (int)hash );
        delete coll;
        continue;
      }

      for (const MmDigit* digit : *digitColl ){
        Identifier id = digit->identify();

        // for now keep the digit structure as vector of firing strips
        // (will have to be reviewed )
        // number of strips
        unsigned int nstrips = digit->stripResponsePosition().size();

        for ( unsigned int i=0 ; i<nstrips ; ++i ) {

          ///
          /// set the rdo id to a value consistent with the channel number
          ///
          bool isValid;
          int stationName = m_idHelperSvc->mmIdHelper().stationName(id);
          int stationEta  = m_idHelperSvc->mmIdHelper().stationEta(id);
          int stationPhi  = m_idHelperSvc->mmIdHelper().stationPhi(id);
          int multilayer  = m_idHelperSvc->mmIdHelper().multilayer(id);
          int gasGap      = m_idHelperSvc->mmIdHelper().gasGap(id);
          ///
          int channel     = digit->stripResponsePosition().at(i);

          Identifier newId = m_idHelperSvc->mmIdHelper().channelID(stationName,stationEta,
                                                   stationPhi,multilayer,gasGap,channel,true,&isValid);

          if (!isValid) {
            ATH_MSG_WARNING("Invalid MM identifier. StationName="<<stationName <<
                            " stationEta=" << stationEta << " stationPhi=" << stationPhi <<
                            " multi=" << multilayer << " gasGap=" << gasGap << " channel=" << channel);
            continue;
          }

          MM_RawData* rdo = new MM_RawData(newId,
                                           digit->stripResponsePosition().at(i),
                                           digit->stripResponseTime().at(i),
                                           digit->stripResponseCharge().at(i));

          coll->push_back(rdo);

        }
      }

    }
  } else {
    ATH_MSG_WARNING("Unable to find MM digits");
  }

  ATH_MSG_DEBUG( "done execute()"  );
  return StatusCode::SUCCESS;
}
