/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

/** @file ByteStreamMetadataTool.cxx
 *  @brief This file contains the implementation for the ByteStreamMetadataTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadataTool.cxx,v 1.4 2009-05-13 22:06:51 cranshaw Exp $
 **/

#include "ByteStreamMetadataTool.h"

#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "StoreGate/StoreGateSvc.h"


/******************************************************************************/
ByteStreamMetadataTool::ByteStreamMetadataTool(
    const std::string& type,
    const std::string& name,
    const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_metadataStore("StoreGateSvc/MetaDataStore", name)
  , m_inputStore   ("StoreGateSvc/InputMetaDataStore", name)
{
  declareInterface<IMetaDataTool>(this);
}


/******************************************************************************/
ByteStreamMetadataTool::~ByteStreamMetadataTool()
{}


/******************************************************************************/
StatusCode
ByteStreamMetadataTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);

  ATH_CHECK(::AthAlgTool::initialize());
  ATH_CHECK(m_metadataStore.retrieve());
  ATH_CHECK(m_inputStore.retrieve());

  return(StatusCode::SUCCESS);
}


/******************************************************************************/
StatusCode
ByteStreamMetadataTool::finalize()
{
  ATH_MSG_INFO("in finalize()");
  return(::AthAlgTool::finalize());
}


/******************************************************************************/
StatusCode
ByteStreamMetadataTool::beginInputFile(const SG::SourceID&)
{
  return this->beginInputFile();
}


/******************************************************************************/
StatusCode
ByteStreamMetadataTool::beginInputFile()
{
  std::set<std::string> keys = keysFromInput();

  std::vector<std::unique_ptr<ByteStreamMetadata> > copy;
  std::set<std::string> transGuids;


  for(const auto& key : keys) {
    ATH_MSG_DEBUG("Processing Input ByteStreamMetadata, key = " << key);
    copy.clear();

    if(m_inputStore->contains<ByteStreamMetadata>(key)) {

      std::list<SG::ObjectWithVersion<ByteStreamMetadata> > allVersions;
      ATH_CHECK(m_inputStore->retrieveAllVersions(allVersions, key));

      for(SG::ObjectWithVersion<ByteStreamMetadata>& obj : allVersions)
        copy.push_back(std::make_unique<ByteStreamMetadata>(*obj.dataObject));

    }


    if(m_inputStore->contains<ByteStreamMetadataContainer>(key)) {

      std::list<SG::ObjectWithVersion<ByteStreamMetadataContainer> > allVersions;
      ATH_CHECK(m_inputStore->retrieveAllVersions(allVersions, key));

      for(SG::ObjectWithVersion<ByteStreamMetadataContainer>& obj : allVersions)
        for(const ByteStreamMetadata* md : *obj.dataObject)
          copy.push_back(std::make_unique<ByteStreamMetadata>(*md));
    }


    if(!copy.empty()) {

      transGuids.clear();
      // Check for existing container
      ByteStreamMetadataContainer* bsmdc = 0;

      if (m_metadataStore->contains<ByteStreamMetadataContainer>(key)) {

        ATH_MSG_DEBUG("Pre-existing ByteStreamMetadataContainer found");
        ATH_CHECK(m_metadataStore->retrieve(bsmdc, key));

        for (const auto& bsmd : *bsmdc)
          transGuids.insert(bsmd->getGuid());

      } else {

        bsmdc = new ByteStreamMetadataContainer;
        ATH_CHECK(m_metadataStore->record(bsmdc, key));

      }

      for(auto& pBSMD : copy) {
        // Only insert new metadata records (with GUID not yet in container)
        if(transGuids.insert(pBSMD->getGuid()).second)
          bsmdc->push_back(std::move(pBSMD));
      }
    }
  }

  return StatusCode::SUCCESS;
}



/******************************************************************************/
inline
std::set<std::string>
ByteStreamMetadataTool::keysFromInput() const
{
  std::vector<std::string> vKeys;
  std::set<std::string> keys;

  m_inputStore->keys<ByteStreamMetadata>(vKeys);
  keys.insert(vKeys.begin(), vKeys.end());

  m_inputStore->keys<ByteStreamMetadataContainer>(vKeys);
  keys.insert(vKeys.begin(), vKeys.end());

  return keys;
}


/******************************************************************************/
StatusCode
ByteStreamMetadataTool::endInputFile(const SG::SourceID&)
{
  return StatusCode::SUCCESS;
}


StatusCode
ByteStreamMetadataTool::endInputFile()
{
  return StatusCode::SUCCESS;
}


StatusCode
ByteStreamMetadataTool::metaDataStop(const SG::SourceID&)
{
  return StatusCode::SUCCESS;
}


StatusCode
ByteStreamMetadataTool::metaDataStop()
{
  return StatusCode::SUCCESS;
}
