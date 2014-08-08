/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/BTagRemoving.h"
#include "BTagging/IBTagRemoving.h"

#include "JetEvent/Jet.h"

namespace Analysis {

  BTagRemoving::BTagRemoving(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p),
    m_storeTrackAssociation(false),
    m_storeSecondaryVertices(false)
  {
      declareInterface<IBTagRemoving>(this);
      declareProperty("listOfTracksToRemove",           m_listOfTracksToRemove);
      declareProperty("listOfSvxToRemove",              m_listOfSvxToRemove);
      declareProperty("storeTrackAssociation",          m_storeTrackAssociation);
      declareProperty("storeSecondaryVerticesInJet",    m_storeSecondaryVertices);
  }

  BTagRemoving::~BTagRemoving() {
  }

  StatusCode BTagRemoving::initialize()
  {
     return StatusCode::SUCCESS;
  }

  StatusCode BTagRemoving::finalize()
  {
      return StatusCode::SUCCESS;
  }
 
  StatusCode BTagRemoving::BTagRemoving_exec(Jet& myJet) {

    /* ----------------------------------------------------------------------------------- */
    /*               Do not store permanently sec vtx info for the moment                  */
    /* ----------------------------------------------------------------------------------- */
    
    if(!m_storeTrackAssociation){
      std::vector<std::string>::const_iterator assoctrkIter=m_listOfTracksToRemove.begin();
      for(; assoctrkIter!=m_listOfTracksToRemove.end(); ++assoctrkIter){
	myJet.removeAssociation(*assoctrkIter);
      }
    }


/*    if(!m_storeTrackAssociation){
      std::map<std::string, std::vector<const Trk::TrackParticleBase*> >::iterator assoctrkIter=tracksInJetList.begin();
      for(; assoctrkIter!=tracksInJetList.end(); ++assoctrkIter){
	if( "Tracks" == assoctrkIter->first ) continue;
	myJet.removeAssociation(assoctrkIter->first);
      }
    }
*/

    if (!m_storeSecondaryVertices) {
      std::vector<std::string>::const_iterator SecListIter=m_listOfSvxToRemove.begin();
      std::vector<std::string>::const_iterator SecListEnd=m_listOfSvxToRemove.end();
      for ( ; SecListIter!=SecListEnd ; ++SecListIter) {
	myJet.removeAssociation(*SecListIter);
      }
    }

/*    if (!m_storeSecondaryVertices) {
      std::vector<std::string>::const_iterator SecListIter=listOfSvxAssociationNames.begin();
      std::vector<std::string>::const_iterator SecListEnd=listOfSvxAssociationNames.end();
      for ( ; SecListIter!=SecListEnd ; ++SecListIter) {
	myJet.removeAssociation(*SecListIter);
      }
    }
*/
    /*
      std::vector<std::string>::const_iterator SecListIter=listOfSvxAssociationNames.begin();
      std::vector<std::string>::const_iterator SecListEnd=listOfSvxAssociationNames.end();
      for ( ; SecListIter!=SecListEnd ; ++SecListIter) {
	const Analysis::ISvxAssociation* newSvxAssociation=myJet.getAssociation<Analysis::ISvxAssociation>(*SecListIter);
	const Trk::VxSecVertexInfo* myVertexInfo=newSvxAssociation->vertexInfo();
	const std::vector<Trk::VxCandidate*> & myVertices=myVertexInfo->vertices();
	int sizeVertices=myVertices.size();
	std::cout << " string: " << *SecListIter << " size vertices: " << sizeVertices << " pointer: " << &myVertices << std::endl;
      }
    */

      return StatusCode::SUCCESS;
   }

} // namespace
