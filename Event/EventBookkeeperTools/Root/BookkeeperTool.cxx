///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for class BookkeeperTool
// Authors: Joao Firmino da Costa <joao.costa@cern.ch> and David Cote <david.cote@cern.ch>
///////////////////////////////////////////////////////////////////

#include "EventBookkeeperTools/BookkeeperTool.h"

// STL include
#include <algorithm>

// #include "FillEBCFromFlat.h"

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaBaseComps/AthCheckMacros.h"


BookkeeperTool::BookkeeperTool(const std::string& name)
  : asg::AsgMetadataTool(name)
{
  declareProperty("OutputCollName", m_completeCollName="CutBookkeepers",  
    "The default name of the xAOD::CutBookkeeperContainer for fully processed files");
  declareProperty("OutputIncompleteCollName", m_incompleteCollName = "IncompleteCutBookkeepers",
    "The default name of the xAOD::CutBookkeeperContainer for partially processed files");
#ifdef ASGTOOL_ATHENA
  declareInterface< ::IMetaDataTool >( this );
#endif // ASGTOOL_ATHENA
}



BookkeeperTool::~BookkeeperTool()
{
}



StatusCode
BookkeeperTool::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name() << " - package version " << PACKAGE_VERSION );
  ATH_MSG_DEBUG("OutputCollName = " << m_completeCollName);
  ATH_MSG_DEBUG("OutputIncompleteCollName = " << m_incompleteCollName);

  return StatusCode::SUCCESS;
}


//__________________________________________________________________________
StatusCode BookkeeperTool::beginInputFile()
{
  //OPENING NEW INPUT FILE
  //Things to do:
  // 1) note that a file is currently opened
  // 2) Load CutBookkeepers from input file
  //    2a) if incomplete from input, directly propagate to output
  //    2b) if complete from input, wait for EndInputFile to decide what to do in output

  // Get the incomplete bookkeeper collection of the input metadata store
  const xAOD::CutBookkeeperContainer* input_inc = 0;
  StatusCode ssc = inputMetaStore()->retrieve( input_inc, m_incompleteCollName );
  if (ssc.isSuccess()) {
  //if ( (inputMetaStore()->retrieve( input_inc, m_incompleteCollName )).isSuccess() ) {
    // First make sure there is an incomplete container in the output store
    if( !(outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_incompleteCollName)) ) {
      xAOD::CutBookkeeperContainer* inc = new xAOD::CutBookkeeperContainer();
      xAOD::CutBookkeeperAuxContainer* auxinc = new xAOD::CutBookkeeperAuxContainer();
      inc->setStore(auxinc);
      ATH_CHECK(outputMetaStore()->record(inc,m_incompleteCollName));
      ATH_CHECK(outputMetaStore()->record(auxinc,m_incompleteCollName+"Aux."));
    }
    // retrieve the incomplete output container
    xAOD::CutBookkeeperContainer* incompleteBook(NULL);
    ATH_CHECK(outputMetaStore()->retrieve( incompleteBook, m_incompleteCollName));
    // update incomplete output with any incomplete input
    ATH_CHECK(this->updateContainer(incompleteBook,input_inc));
    ATH_MSG_DEBUG("Successfully merged input incomplete bookkeepers with output");
  }
  else {
    ATH_MSG_INFO("No incomplete bookkeepers in this file " << m_incompleteCollName);
  }

  // Get the complete bookkeeper collection of the input metadata store
  const xAOD::CutBookkeeperContainer* input_com = 0;
  if ( (inputMetaStore()->retrieve( input_com, m_completeCollName )).isSuccess() ) {
    // Check if a tmp is there. IT SHOULD NOT BE
    //xAOD::CutBookkeeperContainer* incompleteBook(NULL);
    if( !(outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_completeCollName+"tmp")) ) {
      // Now create the tmp container
      xAOD::CutBookkeeperContainer* tmp = new xAOD::CutBookkeeperContainer();
      xAOD::CutBookkeeperAuxContainer* auxtmp = new xAOD::CutBookkeeperAuxContainer();
      tmp->setStore(auxtmp);
      if (updateContainer(tmp,input_com).isSuccess()) {
        ATH_CHECK(outputMetaStore()->record(tmp,m_completeCollName+"tmp"));
        ATH_CHECK(outputMetaStore()->record(auxtmp,m_completeCollName+"tmpAux."));
      }
      else {
        ATH_MSG_WARNING("Could not update tmp container from input complete conatiner");
      }
    }
    else {
      ATH_MSG_WARNING("tmp collection already exists");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully copied complete bookkeepers to temp container");
  }
  
  return StatusCode::SUCCESS;
}


StatusCode BookkeeperTool::endInputFile()
{
  //CLOSING INPUT FILE
  //Things to do:
  //  Make sure complete container exists in output
  if( !(outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_completeCollName)) ) {
      // Now create the complete container
    xAOD::CutBookkeeperContainer* inc = new xAOD::CutBookkeeperContainer();
    xAOD::CutBookkeeperAuxContainer* auxinc = new xAOD::CutBookkeeperAuxContainer();
    inc->setStore(auxinc);
    ATH_CHECK(outputMetaStore()->record(inc,m_completeCollName));
    ATH_CHECK(outputMetaStore()->record(auxinc,m_completeCollName+"Aux."));
  }
  else {
    ATH_MSG_WARNING("complete collection already exists");
    //return StatusCode::SUCCESS;
  }

  // Get the complete bookkeeper collection of the output meta-data store
  xAOD::CutBookkeeperContainer* completeBook(NULL); 
  if( !(outputMetaStore()->retrieve( completeBook, m_completeCollName) ).isSuccess() ) {
    ATH_MSG_ERROR( "Could not get complete CutBookkeepers from output MetaDataStore" );
    return StatusCode::FAILURE;
  }

  // Get the tmp bookkeeper from the input
  const xAOD::CutBookkeeperContainer* tmpCompleteBook(NULL);
  if( !(outputMetaStore()->retrieve( tmpCompleteBook, m_completeCollName+"tmp") ).isSuccess() ) {
    ATH_MSG_WARNING( "Could not get tmp CutBookkeepers from output MetaDataStore" );
  }
  else {
    // update the complete output with the complete input
    ATH_CHECK(this->updateContainer(completeBook,tmpCompleteBook));
    // remove the tmp container
    const SG::IConstAuxStore* tmpCompleteBookAux = tmpCompleteBook->getConstStore();
    ATH_CHECK(outputMetaStore()->removeDataAndProxy(tmpCompleteBook));
    ATH_CHECK(outputMetaStore()->removeDataAndProxy(tmpCompleteBookAux));
  }
  
  return StatusCode::SUCCESS;
}

StatusCode BookkeeperTool::metaDataStop()
{
  //TERMINATING THE JOB (EndRun)
  //Things to do:
  // 1) Create new incomplete CutBookkeepers if relevant
  // 2) Print cut flow summary
  // 3) Write root file if requested
  //  Make sure complete container exists in output
  if( !(outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_incompleteCollName)) ) {
      // Now create the complete container
    xAOD::CutBookkeeperContainer* coll = new xAOD::CutBookkeeperContainer();
    xAOD::CutBookkeeperAuxContainer* auxcoll = new xAOD::CutBookkeeperAuxContainer();
    coll->setStore(auxcoll);
    ATH_CHECK(outputMetaStore()->record(coll,m_incompleteCollName));
    ATH_CHECK(outputMetaStore()->record(auxcoll,m_incompleteCollName+"Aux."));
  }
  else {
    ATH_MSG_WARNING("incomplete collection already exists");
  }

  // Get the complete bookkeeper collection of the output meta-data store
  xAOD::CutBookkeeperContainer* incompleteBook(NULL); 
  if( !(outputMetaStore()->retrieve( incompleteBook, m_incompleteCollName) ).isSuccess() ) {
    ATH_MSG_WARNING( "Could not get incomplete CutBookkeepers from output MetaDataStore" );
  }

  // Get the tmp bookkeeper from the input
  const xAOD::CutBookkeeperContainer* tmpCompleteBook(NULL);
  if( !(outputMetaStore()->retrieve( tmpCompleteBook, m_completeCollName+"tmp") ).isSuccess() ) {
    ATH_MSG_WARNING( "Could not get tmp CutBookkeepers from output MetaDataStore for " << m_completeCollName+"tmp");
  }
  else {
    // update the complete output with the complete input
    ATH_CHECK(this->updateContainer(incompleteBook,tmpCompleteBook));
    // remove the tmp container
    const SG::IConstAuxStore* tmpCompleteBookAux = tmpCompleteBook->getConstStore();
    ATH_CHECK(outputMetaStore()->removeDataAndProxy(tmpCompleteBook));
    ATH_CHECK(outputMetaStore()->removeDataAndProxy(tmpCompleteBookAux));
  }

  return StatusCode::SUCCESS;
}


StatusCode
BookkeeperTool::finalize()
{
  ATH_MSG_DEBUG( "Finalizing " << name() << " - package version " << PACKAGE_VERSION );
  return StatusCode::SUCCESS;
}



StatusCode
BookkeeperTool::updateCollFromInputStore(xAOD::CutBookkeeperContainer* coll,
                                     const std::string &collName)
{
  ATH_MSG_DEBUG("calling updateCollFromInputStore(" << collName << ") with coll-size=" << coll->size() );

  if ( inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(collName) ) {
    ATH_MSG_VERBOSE("Input MetaData contains type xAOD::CutBookkeeperContainer with name" << collName);

    // There can always only be a single object in the input store. As the store
    // is connected to just a single input file.
    const xAOD::CutBookkeeperContainer* tmpColl = 0;
    ATH_CHECK( inputMetaStore()->retrieve( tmpColl, collName ) );

    // Check that we succeeded:
    if( ! tmpColl->hasStore() ) {
       ATH_MSG_FATAL( "Object doesn't have an auxiliary store" );
       return StatusCode::FAILURE;
    }

    //...and update coll with each list.
    ATH_CHECK( this->updateContainer(coll, tmpColl) );
  }
  else {
    ATH_MSG_INFO( "Cannot find xAOD::CutBookkeeperContainer "
                  << "with name " << collName << " in the input file.");
  }
  return StatusCode::SUCCESS;
}



StatusCode
BookkeeperTool::updateContainer( xAOD::CutBookkeeperContainer* contToUpdate,
                             const xAOD::CutBookkeeperContainer* otherCont ) {
  ATH_MSG_DEBUG("calling updateContainer(...)" );
  ATH_MSG_DEBUG("Have container to update with size=" << contToUpdate->size()
                  << ", and other container with size=" << otherCont->size() );

  // Create an vector of indices of all the newly transferred CutBookkeepers
  std::vector< std::size_t > newEBKIndices;
  // Loop through otherCont.
  // If element already in contToUpdate, update event counts, otherwise create new element
  for ( std::size_t i=0; i<otherCont->size(); ++i ) {
    const xAOD::CutBookkeeper* otherEBK = otherCont->at(i);
    ATH_MSG_VERBOSE("Looping through otherCont at index " << i);
    ATH_MSG_VERBOSE("Have otherEBK with: name=" << otherEBK->name()
                    << ", cycle=" << otherEBK->cycle()
                    << ", nAcceptedEvents=" << otherEBK->nAcceptedEvents()
                    << ", inputStream=" << otherEBK->inputStream() );


    // Loop through the container to be updated (contToUpdate) and see if we find a match
    bool foundEBKToUpdate(false);
    for ( std::size_t j=0; j<contToUpdate->size(); ++j ) {
      xAOD::CutBookkeeper* ebkToUpdate = contToUpdate->at(j);
      // Check if they are identical, if so, update; else add otherEBK
      if ( otherEBK->isEqualTo(ebkToUpdate) ) {
        ebkToUpdate->setPayload( ebkToUpdate->payload() + otherEBK->payload() );
        foundEBKToUpdate = true;
        break;
      }
    } // End: Inner loop over contToUpdate
    if (!foundEBKToUpdate) {
      xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
      if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
      newEBK->makePrivateStore(otherEBK);
      contToUpdate->push_back( newEBK );
      std::size_t ebIdx = newEBK->index();
      newEBKIndices.push_back(ebIdx);
    }
  } // End: Outer loop over contToUpdate

  // Now, we still need to fix the cross-referencing of the newly added CutBookkkeepers
  for ( std::size_t i=0; i<newEBKIndices.size(); ++i ) {
    std::size_t otherIndex = newEBKIndices.at(i);
    xAOD::CutBookkeeper* ebkToModify = contToUpdate->at(otherIndex);

    // Parent check
    if ( ebkToModify->hasParent() ) {
      const xAOD::CutBookkeeper* oldParent = ebkToModify->parent();
      xAOD::CutBookkeeperContainer::iterator matchIter =  std::find( contToUpdate->begin(),
                                                                       contToUpdate->end(),
                                                                       oldParent );
      // If we found it, we can modify
      if ( matchIter != contToUpdate->end() ) {
        ATH_MSG_VERBOSE("Updating ElementLink to parent");
        ebkToModify->setParent( *matchIter );
      }
      // If we didn't find it, we need to add it
      else {
        ATH_MSG_VERBOSE("Adding new parent");
        xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
        if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
        newEBK->makePrivateStore(oldParent);
        contToUpdate->push_back( newEBK );
        ebkToModify->setParent( newEBK );
      }
    } // Done fixing parent

    // Children check
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nChildren(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->child(oldIdx);
      xAOD::CutBookkeeperContainer::iterator matchIter =  std::find( contToUpdate->begin(),
                                                                       contToUpdate->end(),
                                                                       oldEBK );
      // If we found it, we can modify
      if ( matchIter != contToUpdate->end() ) {
        ATH_MSG_VERBOSE("Updating ElementLink to child");
        ebkToModify->addChild( *matchIter );
      }
      // If we didn't find it, we need to add it
      else {
        ATH_MSG_VERBOSE("Adding new child");
        xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
        if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
        newEBK->makePrivateStore(oldEBK);
        contToUpdate->push_back( newEBK );
        ebkToModify->addChild( newEBK );
      }
    } // Done fixing children

    // Used others check
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nUsedOthers(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->usedOther(oldIdx);
      xAOD::CutBookkeeperContainer::iterator matchIter =  std::find( contToUpdate->begin(),
                                                                       contToUpdate->end(),
                                                                       oldEBK );
      // If we found it, we can modify
      if ( matchIter != contToUpdate->end() ) {
        ATH_MSG_VERBOSE("Updating ElementLink to usedOther");
        ebkToModify->addUsedOther( *matchIter );
      }
      // If we didn't find it, we need to add it
      else {
        ATH_MSG_VERBOSE("Adding new usedOther");
        xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
        if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
        newEBK->makePrivateStore(oldEBK);
        contToUpdate->push_back( newEBK );
        ebkToModify->addUsedOther( newEBK );
      }
    } // Done fixing used others

    // Siblings check
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nSiblings(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->sibling(oldIdx);
      xAOD::CutBookkeeperContainer::iterator matchIter =  std::find( contToUpdate->begin(),
                                                                       contToUpdate->end(),
                                                                       oldEBK );
      // If we found it, we can modify
      if ( matchIter != contToUpdate->end() ) {
        ATH_MSG_VERBOSE("Updating ElementLink to sibling");
        ebkToModify->addSibling( *matchIter );
      }
      // If we didn't find it, we need to add it
      else {
        ATH_MSG_VERBOSE("Adding new sibling");
        xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
        if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
        newEBK->makePrivateStore(oldEBK);
        contToUpdate->push_back( newEBK );
        ebkToModify->addSibling( newEBK );
      }
    } // Done fixing siblings
  } // Done fixing all cross references
  return StatusCode::SUCCESS;
}


