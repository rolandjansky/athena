/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "MM_DigitToRDO.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

MM_DigitToRDO::MM_DigitToRDO(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_idHelper(0)
{
}

StatusCode MM_DigitToRDO::initialize()
{
  declareProperty("RDOContainerName", m_rdoContainer = "MMRDO");
  declareProperty("DigitContainerName", m_digitContainer = "MM_DIGITS");
  
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_rdoContainer.initialize() );
  ATH_CHECK( m_digitContainer.initialize() );
  ATH_CHECK( detStore()->retrieve(m_idHelper, "MMIDHELPER") );  
  return StatusCode::SUCCESS;
}

StatusCode MM_DigitToRDO::execute()
{  
  
  using namespace Muon;
  ATH_MSG_DEBUG( "in execute()"  );
  SG::WriteHandle<MM_RawDataContainer> rdos (m_rdoContainer);
  SG::ReadHandle<MmDigitContainer> digits (m_digitContainer);

  ATH_CHECK( rdos.record(std::unique_ptr<MM_RawDataContainer>(new MM_RawDataContainer(m_idHelper->module_hash_max())) ) );

  if (digits.isValid()){
    for (const MmDigitCollection* digitColl : *digits ){

      // the identifier of the digit collection is the detector element Id ( multilayer granularity )
      Identifier digitId = digitColl->identify();

      // get the hash of the RDO collection
      IdentifierHash hash;
      int getRdoCollHash = m_idHelper->get_module_hash(digitId,hash);
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
	  int stationName = m_idHelper->stationName(id);
	  int stationEta  = m_idHelper->stationEta(id);
	  int stationPhi  = m_idHelper->stationPhi(id);
	  int multilayer  = m_idHelper->multilayer(id);
	  int gasGap      = m_idHelper->gasGap(id);
	  ///
	  int channel     = digit->stripResponsePosition().at(i);

	  Identifier newId = m_idHelper->channelID(stationName,stationEta,
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

StatusCode MM_DigitToRDO::finalize() {
  return StatusCode::SUCCESS;
}
