/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #include "SiHitTruthAssociationTool.h"
 
 #include "AthenaKernel/errorcheck.h"
 #include "AtlasHepMC/GenParticle.h"
//#include "TrkTrack/Track.h"
//#include "TrkTrack/TrackCollection.h"
//#include "TrkTruthData/TrackTruthCollection.h"
 #include <map>
 
 namespace D3PD {
 
 SiHitTruthAssociationTool::SiHitTruthAssociationTool (const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent)
   : Base (type, name, parent)
 {
   //   declareProperty ("MapKey", m_TruthMap = "TrackTruthMap");
   //declareProperty ("SGKey", m_TrkContainer = "Tracks");
 }
 
 
 StatusCode SiHitTruthAssociationTool::initialize()
 {
 
   CHECK( Base::initialize() );
 
   // StatusCode sc = service("StoreGateSvc", m_storeGate);
   //if(sc.isFailure()) {
   //  return StatusCode::FAILURE;
   //}
 
   return StatusCode::SUCCESS;
 }
 
 
 StatusCode SiHitTruthAssociationTool::book ()
 {
   //CHECK( addVariable ("probability", m_trueProb) );
   //CHECK( addVariable ("barcode", m_trueBarcode) );
 
   return StatusCode::SUCCESS;
 }
 
const HepMC::GenParticle* SiHitTruthAssociationTool::get (const SiHit& sihit){
  const HepMcParticleLink& mcLink = sihit.particleLink();
const HepMC::GenParticle* genPart= mcLink.cptr();
 std::cout<< "Identifier: "<< sihit.identify();
  if(genPart){
	std::cout<<" genPart.barcode: " << genPart->barcode() << " type " << genPart->pdg_id() << " status " << genPart->status();
        
	}else {
	std::cout<<" no genPart ";
	}

 std::cout<< std::endl;

return genPart;
}
 }
