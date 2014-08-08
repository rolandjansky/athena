/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/BJetBuilderFast.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"

#include "JetTagTools/ITagTool.h"
#include "JetTagInfo/AtlfInfo.h"

namespace Analysis {

  BJetBuilderFast::BJetBuilderFast( const std::string &n, ISvcLocator *p ) :
    AthAlgorithm( n, p ) {
      declareProperty( "JetCollectionName",  m_JetCollectionName = "AtlfastParticleJetContainer" );
      //      declareProperty( "CollectionAppendix", m_collectionAppendix = "AOD" );
      declareProperty( "CollectionAppendix", m_collectionAppendix = "ReTagged" );
      // List of the tagging tools to be used
      declareProperty("TagToolList", m_bTagToolHandleArray);
  }

  BJetBuilderFast::~BJetBuilderFast() {}

  StatusCode BJetBuilderFast::initialize() {

    if ( m_bTagToolHandleArray.retrieve().isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_bTagToolHandleArray);
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved " << m_bTagToolHandleArray);
    }

    return StatusCode::SUCCESS;
  }

  StatusCode BJetBuilderFast::execute() {
    
    StatusCode sc;

    const JetCollection* tmpSGParticleJetContainer( 0 );
    sc = evtStore()->retrieve( tmpSGParticleJetContainer, m_JetCollectionName );
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("#BTAG# No " << m_JetCollectionName << " collection found");
      return StatusCode::SUCCESS;
    }

    JetCollection* particleJetContainer = new JetCollection(SG::OWN_ELEMENTS);
    sc = evtStore()->record( particleJetContainer, m_JetCollectionName+m_collectionAppendix );
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("#BTAG# Could not record the new Jet collection");
      return StatusCode::SUCCESS;
    }
    for ( JetCollection::const_iterator i = tmpSGParticleJetContainer->begin(); i != tmpSGParticleJetContainer->end(); i++ ) {
      Jet* newJet = new Jet(*i,false,false);
      const Analysis::AtlfInfo* Tinfo = (*i)->tagInfo<Analysis::AtlfInfo>("AtlfInfo");
      if (Tinfo) {
	Analysis::AtlfInfo* atlfInfo = new Analysis::AtlfInfo(*Tinfo);
	newJet->addInfo(atlfInfo);
      }
      particleJetContainer->push_back(newJet);
    }

    if ( particleJetContainer->size() > 0 ) {
      for ( JetCollection::iterator jetOfCol = particleJetContainer->begin(); jetOfCol != particleJetContainer->end(); jetOfCol++ ) {
	if (fabs((*jetOfCol)->eta()) <= 2.5) {
	  ToolHandleArray< Analysis::ITagTool >::iterator itTagTools = m_bTagToolHandleArray.begin(), 
	    itTagToolsEnd = m_bTagToolHandleArray.end();
	  for (  ; itTagTools != itTagToolsEnd; ++itTagTools ) {
	    (*itTagTools)->tagJet(**jetOfCol);
	  }
	}
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode BJetBuilderFast::finalize()
  {
    return StatusCode::SUCCESS;
  }

}

