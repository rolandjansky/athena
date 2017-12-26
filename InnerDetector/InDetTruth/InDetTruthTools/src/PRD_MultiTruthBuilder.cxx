/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTruthTools/PRD_MultiTruthBuilder.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"
//#include <stdexcept>
//#include <ext/functional>


namespace InDet {
#if 0 /*no-op block for better indentation in Emacs*/
} 
#endif

//================================================================
PRD_MultiTruthBuilder::PRD_MultiTruthBuilder(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent)
  , m_idHelperPixel(0)
{
  declareInterface<IPRD_MultiTruthBuilder>(this);
}

StatusCode PRD_MultiTruthBuilder::initialize() {
  ATH_MSG_DEBUG("PRD truth builder initialization");
  
  StatusCode sc = detStore()->retrieve(m_idHelperPixel, "PixelID");
  if (!sc.isSuccess()) {
    ATH_MSG_FATAL("Could not get Pixel ID helper");
    return sc;
  }
  
  return sc;
}

//================================================================
void PRD_MultiTruthBuilder::addPrepRawDatum(PRD_MultiTruthCollection *prdTruth,
					    const Trk::PrepRawData* prd,
					    const InDetSimDataCollection* simDataMap,
					    bool pixels
					    ) const
{
  if(!prdTruth || !prd || !simDataMap) {
    return;
  }
  ATH_MSG_VERBOSE("addPrepRawDatum(): new PRD "<<prd<<", id="<<prd->identify()<<", number of RDOs: " << prd->rdoList().size());

  bool gotSDO = false;
  bool gotValidParticle = false;

  // loop over RDOs
  std::vector<Identifier>::const_iterator nextRDO = prd->rdoList().begin();
  std::vector<Identifier>::const_iterator lastRDO = prd->rdoList().end();	
  for (; nextRDO!=lastRDO; ++nextRDO) {
    InDetSimDataCollection::const_iterator iter(simDataMap->find(*nextRDO));

    if ( pixels && iter == simDataMap->end() ) {
      ATH_MSG_VERBOSE ( "Pixel=" << m_idHelperPixel->eta_index(*nextRDO) << "," <<m_idHelperPixel->phi_index(*nextRDO) << " does not match any SDO");
    }

    if(iter != simDataMap->end() )  {
      gotSDO = true;
      // Got an SDO.  Try to associate the PRD to MC particles we have info about.
      const InDetSimData& sdo = iter->second;
      const std::vector< InDetSimData::Deposit >& deposits = sdo.getdeposits();
      std::vector< InDetSimData::Deposit >::const_iterator nextdeposit = deposits.begin();
      std::vector< InDetSimData::Deposit >::const_iterator lastdeposit = deposits.end();
      for( ; nextdeposit!=lastdeposit; ++nextdeposit) {
	const HepMcParticleLink& particleLink = nextdeposit->first;

	ATH_MSG_VERBOSE("addPrepRawDatum(): Barcode " << particleLink.barcode());

	if (particleLink.isValid()) {
	  gotValidParticle = true;
	  // Associate the particle to the PRD. But don't add duplicates.
	  // Note: it may be more efficient to filter out duplicates among particles for the current PRD, then check-and-add the reduced set to the large multimap.
	  // But may be not for the typically small RDO/PRD ratio.
	  typedef PRD_MultiTruthCollection::iterator truthiter;
	  std::pair<truthiter, truthiter> r = prdTruth->equal_range(prd->identify());
          if(r.second == std::find_if(r.first, r.second, 
                          [ particleLink ](const PRD_MultiTruthCollection::value_type &prd_to_truth) {
                             return prd_to_truth.second == particleLink;
                          } ))
            {
	      prdTruth->insert(std::make_pair(prd->identify(), particleLink));
	    }
	}
      }
    }
  }

  if(gotSDO && !gotValidParticle) {
    // Looked at all the deposits from all the SDOs, but did not find any valid particle link.
    //prdTruth->insert(std::make_pair(prd, particleLinkUnknown));
    ATH_MSG_VERBOSE("addPrepRawDatum(): got SDO but no particles");
  }
  
  // No SDO found after all, for all the RDOs? Then (and only then) this PRD is noise.
  //if(!gotSDO) {
    //prdTruth->insert(std::make_pair(prd, particleLinkNoise) );
  //}

}

//================================================================

} // namespace InDet

