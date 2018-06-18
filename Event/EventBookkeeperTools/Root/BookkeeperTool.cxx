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

#ifdef ASGTOOL_ATHENA
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/MetaCont.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/WriteMetaHandle.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthContainersInterfaces/IConstAuxStoreMeta.h"
#endif

BookkeeperTool::BookkeeperTool(const std::string& name)
  : asg::AsgMetadataTool(name),
    m_cutflowTaken(false)
{
  declareProperty("OutputCollName", m_outputCollName="CutBookkeepers",  
    "The default name of the xAOD::CutBookkeeperContainer for output files");
  declareProperty("InputCollName", m_inputCollName = "CutBookkeepers",
    "The default name of the xAOD::CutBookkeeperContainer for input files");
  declareProperty("CutFlowCollName", m_cutflowCollName = "CutBookkeepersFile",
    "The default name of the xAOD::CutBookkeeperContainer for CutFlowSvc");
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

  ATH_MSG_DEBUG("InputCollName = " << m_inputCollName);
  ATH_MSG_DEBUG("OutputCollName = " << m_outputCollName);
  ATH_MSG_DEBUG("CutFlowCollName = " << m_cutflowCollName);

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

  //  Make sure the output containers are in the output store
  //
  //  Make sure complete container exists in output
  if( !(outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_outputCollName)) ) {
      // Now create the complete container
    xAOD::CutBookkeeperContainer* inc = new xAOD::CutBookkeeperContainer();
    xAOD::CutBookkeeperAuxContainer* auxinc = new xAOD::CutBookkeeperAuxContainer();
    inc->setStore(auxinc);
    ATH_CHECK(outputMetaStore()->record(inc,m_outputCollName));
    ATH_CHECK(outputMetaStore()->record(auxinc,m_outputCollName+"Aux."));
  }
  else {
    ATH_MSG_WARNING("complete collection already exists");
    //return StatusCode::SUCCESS;
  }
  //  Make sure incomplete container exists in output
  std::string inc_name = "Incomplete"+m_outputCollName;
  if( !(outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(inc_name)) ) {
      // Now create the complete container
    xAOD::CutBookkeeperContainer* coll = new xAOD::CutBookkeeperContainer();
    xAOD::CutBookkeeperAuxContainer* auxcoll = new xAOD::CutBookkeeperAuxContainer();
    coll->setStore(auxcoll);
    ATH_CHECK(outputMetaStore()->record(coll,inc_name));
    ATH_CHECK(outputMetaStore()->record(auxcoll,inc_name+"Aux."));
  }
  else {
    ATH_MSG_WARNING("incomplete collection already exists");
  }

  // reset cutflow taken marker
  m_cutflowTaken = false;

  // Get the incomplete bookkeeper collection of the input metadata store
  const xAOD::CutBookkeeperContainer* input_inc = 0;
  // Construct input and output incomplete names
  std::string inCollName = "Incomplete" + m_inputCollName;
  std::string outCollName = "Incomplete" + m_outputCollName;
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(inCollName) ) {
    StatusCode ssc = inputMetaStore()->retrieve( input_inc, inCollName );
    if (ssc.isSuccess()) {
      // retrieve the incomplete output container
      xAOD::CutBookkeeperContainer* incompleteBook(NULL);
      ATH_CHECK(outputMetaStore()->retrieve( incompleteBook, outCollName));
      // update incomplete output with any incomplete input
      ATH_CHECK(this->updateContainer(incompleteBook,input_inc));
      ATH_MSG_DEBUG("Successfully merged input incomplete bookkeepers with output");
    }
  }
  else {
    ATH_MSG_INFO("No incomplete bookkeepers in this file " << inCollName);
  }

  // Get the complete bookkeeper collection of the input metadata store
  const xAOD::CutBookkeeperContainer* input_com = 0;
  inCollName = m_inputCollName;
  outCollName = m_outputCollName;
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(inCollName) ) {
    if ( (inputMetaStore()->retrieve( input_com, inCollName )).isSuccess() ) {
      // Check if a tmp is there. IT SHOULD NOT BE
      //xAOD::CutBookkeeperContainer* incompleteBook(NULL);
      if( !(outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(outCollName+"tmp")) ) {
        // Now create the tmp container
        xAOD::CutBookkeeperContainer* tmp = new xAOD::CutBookkeeperContainer();
        xAOD::CutBookkeeperAuxContainer* auxtmp = new xAOD::CutBookkeeperAuxContainer();
        tmp->setStore(auxtmp);
        if (updateContainer(tmp,input_com).isSuccess()) {
          ATH_CHECK(outputMetaStore()->record(tmp,outCollName+"tmp"));
          ATH_CHECK(outputMetaStore()->record(auxtmp,outCollName+"tmpAux."));
        }
        else {
          ATH_MSG_WARNING("Could not update tmp container from input complete conatiner");
        }
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

#ifdef ASGTOOL_ATHENA
StatusCode BookkeeperTool::beginInputFile(const SG::SourceID& sid)
{
  //OPENING NEW INPUT FILE
  //Things to do:
  // 1) note that a file is currently opened
  // 2) Load CutBookkeepers from input file
  //    2a) if incomplete from input, directly propagate to output
  //    2b) if complete from input, wait for EndInputFile to decide what to do in output

  std::string tmp_name = m_outputCollName+"tmpCont";
  std::string inc_name = "Incomplete"+m_outputCollName+"Cont"; 
  //  IF NO METACONT IN OUTPUT STORE YET
  //     Initialize MetaCont for incomplete and tmp containers in output store
  //
  if( !(outputMetaStore()->contains<MetaCont<xAOD::CutBookkeeperContainer> >(m_outputCollName)) ) {
    ATH_CHECK(this->initOutputContainer(tmp_name));
  }
  else {
    ATH_MSG_WARNING("complete collection already exists");
  }
  if( !(outputMetaStore()->contains<MetaCont<xAOD::CutBookkeeperContainer> >(inc_name)) ) {
    ATH_CHECK(this->initOutputContainer(inc_name));
  }
  else {
    ATH_MSG_WARNING("incomplete collection already exists");
  }

  // Now retrieve pointers for the MetaConts
  MetaCont<xAOD::CutBookkeeperContainer>* tmp;
  MetaCont<xAOD::CutBookkeeperContainer>* inc;
  ATH_CHECK(outputMetaStore()->retrieve(tmp,tmp_name));
  ATH_CHECK(outputMetaStore()->retrieve(inc,inc_name));

  // Make sure sid does not already exist in container
  if (std::find(tmp->sources().begin(),tmp->sources().end(),sid)!=tmp->sources().end() ||  
      std::find(inc->sources().begin(),inc->sources().end(),sid)!=inc->sources().end() ) {
    ATH_MSG_ERROR("Metadata already exists for sid " << sid);
    return StatusCode::FAILURE;
  }

  // reset cutflow taken marker
  m_cutflowTaken = false;

  // NOW GET NEW INFORMATION FROM INPUT STORE
  //
  // Get the incomplete bookkeeper collection of the input metadata store
  std::string inCollName = "Incomplete" + m_inputCollName;
  xAOD::CutBookkeeperContainer* input_inc = 0;
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(inCollName) ) {
    StatusCode ssc = inputMetaStore()->retrieve( input_inc, inCollName );
    if (ssc.isSuccess()) {
      // retrieve the incomplete output container
      if ( !inc->insert(sid,input_inc) ) {
        ATH_MSG_ERROR("Unable to insert " << inCollName << " for " << sid << " with key " << inc_name);
      }
    }
  }
  else {
    ATH_MSG_INFO("No incomplete bookkeepers in this file " << inCollName);
  }

  // Get the complete bookkeeper collection of the input metadata store
  inCollName = m_inputCollName;
  xAOD::CutBookkeeperContainer* input_com = 0;
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(inCollName) ) {
    if ( (inputMetaStore()->retrieve( input_com, inCollName )).isSuccess() ) {
      // retrieve the incomplete output container
      if ( !tmp->insert(sid,input_com) ) {
        ATH_MSG_ERROR("Unable to insert " << inCollName << " for " << sid << "with key " << tmp_name);
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
#endif


StatusCode BookkeeperTool::endInputFile()
{
  if (copyContainerToOutput(m_outputCollName).isFailure()) return StatusCode::FAILURE;

  if (!m_cutflowTaken) {
    if (addCutFlow().isFailure()) {
      ATH_MSG_ERROR("Could not add CutFlow information");
    }
    m_cutflowTaken = true;
  }
  else {
    ATH_MSG_DEBUG("Cutflow information written into container before endInputFile");
  }
    
  return StatusCode::SUCCESS;
}


//#ifdef ASGTOOL_ATHENA
StatusCode BookkeeperTool::endInputFile(const SG::SourceID& sid)
{
  // Add the sid to the list of complete sids
  m_completes.insert(sid);

  return StatusCode::SUCCESS;
}

StatusCode BookkeeperTool::metaDataStop(const SG::SourceID&)
{
  //TERMINATING THE JOB (EndRun)
  //Things to do:
  // 1) Create new incomplete CutBookkeepers if relevant
  // 2) Print cut flow summary
  // 3) Write root file if requested
  // Now retrieve pointers for the MetaConts
  std::string tmp_name = m_outputCollName+"tmpCont";
  std::string inc_name = "Incomplete"+m_outputCollName+"Cont"; 
  MetaCont<xAOD::CutBookkeeperContainer>* tmp;
  MetaCont<xAOD::CutBookkeeperContainer>* inc;
  ATH_CHECK(outputMetaStore()->retrieve(tmp,tmp_name));
  ATH_CHECK(outputMetaStore()->retrieve(inc,inc_name));

  // Output containers
  xAOD::CutBookkeeperContainer* outcom = new xAOD::CutBookkeeperContainer();
  xAOD::CutBookkeeperContainer* outinc = new xAOD::CutBookkeeperContainer();
  xAOD::CutBookkeeperAuxContainer* outcom_aux = new xAOD::CutBookkeeperAuxContainer();
  xAOD::CutBookkeeperAuxContainer* outinc_aux = new xAOD::CutBookkeeperAuxContainer();
  outcom->setStore(outcom_aux);
  outinc->setStore(outinc_aux);
  // Incomplete can just be merged
  auto sids_inc = inc->sources();
  xAOD::CutBookkeeperContainer* contptr(nullptr);
  for (auto it = sids_inc.begin(); it != sids_inc.end(); ++it) {
    if (!inc->find(*it,contptr)) {
      ATH_MSG_ERROR("Container sid list did not match contents");
    } else {
      ATH_CHECK(updateContainer(outinc,contptr));
    }
    contptr = nullptr; 
  }
  // Loop over containers and mark complete/incomplete based on end files seen
  auto sids_tmp = tmp->sources();
  contptr = nullptr;
  for (auto it = sids_tmp.begin(); it != sids_tmp.end(); ++it) {
    if (!inc->find(*it,contptr)) {
      ATH_MSG_ERROR("Container sid list did not match contents");
    } else {
      if (std::find(m_completes.begin(), 
                    m_completes.end(),
                    *it) == m_completes.end()) {
        ATH_CHECK(updateContainer(outinc,contptr));
      } else {
        ATH_CHECK(updateContainer(outcom,contptr));
      }
    }
  }
  // Record container objects directly in store for output
  std::string incout_name = "Incomplete"+m_outputCollName;
  // Do any cleanup
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(incout_name) ) {
    ATH_MSG_INFO("Cleaning up xAOD::CutBookkeeperContainer for " << incout_name);
    const xAOD::CutBookkeeperContainer* tmpBook(nullptr);
    if ( outputMetaStore()->retrieve(tmpBook,incout_name).isSuccess() ) {
      const SG::IConstAuxStore* tmpBookAux = tmpBook->getConstStore();
      ATH_CHECK(outputMetaStore()->removeDataAndProxy(tmpBook));
      ATH_CHECK(outputMetaStore()->removeDataAndProxy(tmpBookAux));
    }
    else ATH_MSG_ERROR("StoreGate failed retrieve after contains=true");
  }
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_outputCollName) ) {
    ATH_MSG_INFO("Cleaning up xAOD::CutBookkeeperContainer for " << m_outputCollName);
    const xAOD::CutBookkeeperContainer* tmpBook(nullptr);
    if ( outputMetaStore()->retrieve(tmpBook,m_outputCollName).isSuccess() ) {
      const SG::IConstAuxStore* tmpBookAux = tmpBook->getConstStore();
      ATH_CHECK(outputMetaStore()->removeDataAndProxy(tmpBook));
      ATH_CHECK(outputMetaStore()->removeDataAndProxy(tmpBookAux));
    }
    else ATH_MSG_ERROR("StoreGate failed retrieve after contains=true");
  }
  ATH_CHECK(outputMetaStore()->record(outinc,incout_name));
  ATH_CHECK(outputMetaStore()->record(outinc_aux,incout_name+"Aux."));
  ATH_CHECK(outputMetaStore()->record(outcom,m_outputCollName));
  ATH_CHECK(outputMetaStore()->record(outcom_aux,m_outputCollName+"Aux."));


  if (!m_cutflowTaken) {
    if (addCutFlow().isFailure()) {
      ATH_MSG_ERROR("Could not add CutFlow information");
    }
  }
  else {
    ATH_MSG_DEBUG("Cutflow information written into container before metaDataStop");
  }

  // Reset after metadata stop
  m_cutflowTaken = false;
  
  return StatusCode::SUCCESS;
}
//#endif

StatusCode BookkeeperTool::metaDataStop()
{
  //TERMINATING THE JOB (EndRun)
  //Things to do:
  // 1) Create new incomplete CutBookkeepers if relevant
  // 2) Print cut flow summary
  // 3) Write root file if requested
  //  Make sure incomplete container exists in output
  std::string inc_name = "Incomplete"+m_outputCollName;
  if (copyContainerToOutput(inc_name).isFailure()) return StatusCode::FAILURE;


  if (!m_cutflowTaken) {
    if (addCutFlow().isFailure()) {
      ATH_MSG_ERROR("Could not add CutFlow information");
    }
  }
  else {
    ATH_MSG_DEBUG("Cutflow information written into container before metaDataStop");
  }

  // Reset after metadata stop
  m_cutflowTaken = false;
  
  return StatusCode::SUCCESS;
}


StatusCode
BookkeeperTool::finalize()
{
  ATH_MSG_DEBUG( "Finalizing " << name() << " - package version " << PACKAGE_VERSION );
  return StatusCode::SUCCESS;
}


StatusCode BookkeeperTool::initOutputContainer( const std::string& sgkey)
{
  // Now create the primary container
  xAOD::CutBookkeeperContainer* coll = new xAOD::CutBookkeeperContainer();
  xAOD::CutBookkeeperAuxContainer* auxcoll = new xAOD::CutBookkeeperAuxContainer();
  coll->setStore(auxcoll);
  // Put it in a MetaCont
  MetaCont<xAOD::CutBookkeeperContainer>* mcont = new MetaCont<xAOD::CutBookkeeperContainer>(DataObjID("xAOD::CutBookkeeperContainer",sgkey));
  // Do the same for the auxiliary container
  std::string auxkey(sgkey+"Aux.");
  MetaCont<xAOD::CutBookkeeperAuxContainer>* acont = new MetaCont<xAOD::CutBookkeeperAuxContainer>(DataObjID("xAOD::CutBookkeeperAuxContainer",auxkey));
  ATH_CHECK(outputMetaStore()->record(std::move(mcont),sgkey));
  ATH_CHECK(outputMetaStore()->record(std::move(acont),auxkey));
  ATH_CHECK(outputMetaStore()->symLink
          (
            ClassID_traits<MetaCont<xAOD::CutBookkeeperAuxContainer> >::ID(),
            auxkey,
            ClassID_traits<xAOD::CutBookkeeperAuxContainer>::ID()
          ));

  return StatusCode::SUCCESS;
}

StatusCode BookkeeperTool::addCutFlow()
{
  // Add the information from the current processing to the complete output
  // --> same paradigm as original CutFlowSvc
  // Get the complete bookkeeper collection of the output meta-data store
  xAOD::CutBookkeeperContainer* completeBook(NULL); 
  if( !(outputMetaStore()->retrieve( completeBook, m_outputCollName) ).isSuccess() ) {
    ATH_MSG_ERROR( "Could not get complete CutBookkeepers from output MetaDataStore" );
    return StatusCode::FAILURE;
  }

  // Get the bookkeeper from the current processing
  const xAOD::CutBookkeeperContainer* fileCompleteBook(NULL);
  if( outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_cutflowCollName) ) {
    if( !(outputMetaStore()->retrieve( fileCompleteBook, m_cutflowCollName) ).isSuccess() ) {
      ATH_MSG_WARNING( "Could not get CutFlowSvc CutBookkeepers from output MetaDataStore" );
    }
    else {
      // update the complete output with the complete input
      ATH_CHECK(this->updateContainer(completeBook,fileCompleteBook));
    }
  }
  else {
    ATH_MSG_INFO("No cutflow container " << m_cutflowCollName);
  }

  return StatusCode::SUCCESS;
}


namespace {


xAOD::CutBookkeeper*
resolveLink (const xAOD::CutBookkeeper* old,
             xAOD::CutBookkeeperContainer& contToUpdate,
             const xAOD::CutBookkeeperContainer& otherCont,
             const std::vector<size_t>& otherIndices)
{
  {
    xAOD::CutBookkeeperContainer::iterator matchIter = 
      std::find( contToUpdate.begin(),
                 contToUpdate.end(),
                 old );
    if (matchIter != contToUpdate.end())
      return *matchIter;
  }

  {
    xAOD::CutBookkeeperContainer::const_iterator matchIter = 
      std::find( otherCont.begin(),
                 otherCont.end(),
                 old );
    if (matchIter != contToUpdate.end()) {
      size_t pos = matchIter - otherCont.begin();
      return contToUpdate[otherIndices[pos]];
    }
  }

  // If we didn't find it, we need to add it
  xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
  if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
  newEBK->makePrivateStore(old);
  contToUpdate.push_back( newEBK );
  return newEBK;
}


} // anonymous namespace


StatusCode
BookkeeperTool::updateContainer( xAOD::CutBookkeeperContainer* contToUpdate,
                             const xAOD::CutBookkeeperContainer* otherCont ) 
{
  ATH_MSG_DEBUG("calling updateContainer(...)" );
  ATH_MSG_VERBOSE("Have container to update with size=" << contToUpdate->size()
                  << ", and other container with size=" << otherCont->size() );

  size_t origSize = contToUpdate->size();

  // Vector of indices in contToUpdate of elements in otherCont.
  std::vector< std::size_t > otherIndices (otherCont->size());
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
        otherIndices[i] = j;
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
      otherIndices[i] = ebIdx;
    }
  } // End: Outer loop over contToUpdate

  // Now, we still need to fix the cross-referencing of the newly added CutBookkkeepers
  for ( std::size_t i=origSize; i<contToUpdate->size(); ++i ) {
    xAOD::CutBookkeeper* ebkToModify = contToUpdate->at(i);

    // Parent check
    if ( ebkToModify->hasParent() ) {
      const xAOD::CutBookkeeper* oldParent = ebkToModify->parent();
      const xAOD::CutBookkeeper* newParent = resolveLink (oldParent,
                                                          *contToUpdate,
                                                          *otherCont,
                                                          otherIndices);
      ebkToModify->setParent (newParent);
    } // Done fixing parent

    // Children check
    std::vector< xAOD::CutBookkeeper* > newChildren;
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nChildren(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->child(oldIdx);
      newChildren.push_back (resolveLink (oldEBK,
                                          *contToUpdate,
                                          *otherCont,
                                          otherIndices));
    } // Done fixing children
    ebkToModify->setChildren (newChildren);

    // Used others check
    std::vector< xAOD::CutBookkeeper* > newOthers;
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nUsedOthers(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->usedOther(oldIdx);
      newOthers.push_back (resolveLink (oldEBK,
                                        *contToUpdate,
                                        *otherCont,
                                        otherIndices));
    } // Done fixing used others
    ebkToModify->setUsedOthers (newOthers);

    // Siblings check
    std::vector< xAOD::CutBookkeeper* > newSiblings;
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nSiblings(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->sibling(oldIdx);
      newSiblings.push_back (resolveLink (oldEBK,
                                          *contToUpdate,
                                          *otherCont,
                                          otherIndices));
    } // Done fixing siblings
    ebkToModify->setSiblings (newSiblings);
  } // Done fixing all cross references
  return StatusCode::SUCCESS;
}


StatusCode BookkeeperTool::copyContainerToOutput(const SG::SourceID& sid, 
                                                 const std::string& outname)
{

  // Get Incomplete container from storegate
  xAOD::CutBookkeeperContainer* contBook(nullptr);
  // If MT look for container, not object
  if (sid != "Serial") {
    // Get the complete bookkeeper collection of the output meta-data store
    MetaCont<xAOD::CutBookkeeperContainer>* contBookCont(nullptr); 
    if( !(outputMetaStore()->retrieve( contBookCont, outname) ).isSuccess() ) {
      ATH_MSG_ERROR( "Could not get " << outname << " CutBookkeepers from output MetaDataStore" );
      ATH_MSG_INFO(outputMetaStore()->dump());
      return StatusCode::FAILURE;
    }
    if (!contBookCont->find(sid,contBook)) {
      ATH_MSG_ERROR( "Could not get " << outname << " CutBookkeepers from MetaCont for sid=" << sid );
      return StatusCode::FAILURE;
    }
  }
  else {
    //xAOD::CutBookkeeperContainer* contBook(nullptr); 
    if( !(outputMetaStore()->retrieve( contBook, outname) ).isSuccess() ) {
      ATH_MSG_ERROR( "Could not get " << outname << " CutBookkeepers from output MetaDataStore" );
      ATH_MSG_INFO(outputMetaStore()->dump());
      return StatusCode::FAILURE;
    }
  }

  // Get the tmp bookkeeper from the input
  const xAOD::CutBookkeeperContainer* tmpBook(NULL);
  if ( outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(outname+"tmp") ) {
    if( !(outputMetaStore()->retrieve( tmpBook, outname+"tmp") ).isSuccess() ) {
      ATH_MSG_WARNING( "Could not get tmp CutBookkeepers from output MetaDataStore" );
    }
    else {
      // update the complete output with the complete input
      ATH_CHECK(this->updateContainer(contBook,tmpBook));
      // remove the tmp container
      const SG::IConstAuxStore* tmpBookAux = tmpBook->getConstStore();
      ATH_CHECK(outputMetaStore()->removeDataAndProxy(tmpBook));
      ATH_CHECK(outputMetaStore()->removeDataAndProxy(tmpBookAux));
    }
  }
    
  return StatusCode::SUCCESS;
}

