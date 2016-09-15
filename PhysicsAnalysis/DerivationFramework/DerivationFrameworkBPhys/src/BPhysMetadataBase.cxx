/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BPhysMetadataBase.cxx, (c) ATLAS Detector software
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Store JO metadata in the output file.
//
// It uses the IOVDbMetaDataTool to store the metadata in a specific
// branch whose name needs to start with the deriviation format name.
//
// This is a base class.  Inherit from it to add the job options you want
// to store.  For a usage example, see
//   Bmumu_metadata.h / Bmumu_metadata.cxx
// and
//   BPHY8.py .
//
// Job options provided by the base class:
// - DerivationName     -- assign the name of the derivation format
// - MetadataFolderName -- assign the name of the metadata folder,
//                         should start with the derivation format name,
//                         defaults to DerivationName if not set.
//                           
//============================================================================
//

#include "DerivationFrameworkBPhys/BPhysMetadataBase.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

namespace DerivationFramework {

  //--------------------------------------------------------------------------
  BPhysMetadataBase::BPhysMetadataBase(const std::string& t,
				       const std::string& n,
				       const IInterface*  p)
    : AthAlgTool(t,n,p), m_metaDataTool("IOVDbMetaDataTool") {

    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare derivation format name
    declareProperty("DerivationName", m_derivationName = "_NOSUCHFORMAT_");

    // Declare metadata folder name (should start with derivation name)
    declareProperty("MetadataFolderName", m_mdFolderName = "_NONE_");

  }
  //--------------------------------------------------------------------------
  StatusCode BPhysMetadataBase::initialize() {
  
    ATH_MSG_DEBUG("BPhysMetaDataBase::initialize() -- begin");

    CHECK( saveMetaData() );

    ATH_MSG_DEBUG("BPhysMetaDataBase::initialize() -- end");

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode BPhysMetadataBase::finalize() {

    ATH_MSG_DEBUG("BPhysMetaDataBase::finalize()");

    // everything all right
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode BPhysMetadataBase::addBranches() const {

    // nothing to do here
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode BPhysMetadataBase::saveMetaData() const {

    ATH_MSG_DEBUG("BPhysMetaDataBase::saveMetaData() -- begin");

    // folder name
    std::string fname("/");
    if ( m_mdFolderName != "_NONE_" && m_mdFolderName != "" ) {
      fname += m_mdFolderName;
    } else {
      if ( m_derivationName != "_NOSUCHFORMAT_" && m_derivationName != "" ) {
	fname += m_derivationName;
      } else {
	// default to the tool's name
	fname += name();
      }
    }
    
    // register specific metadata folder
    CHECK( m_metaDataTool->registerFolder(fname) );
    
    // create a new attribute list collection
    // true: will specify IOV using runNumbers, not timestamps
    CondAttrListCollection* cont = new CondAttrListCollection(true);
    
    // create new attribute list
    CondAttrListCollection::AttributeList attrList;
    
    // add contents
    attrList.extend("DerivationName", "string");
    attrList["DerivationName"].data<std::string>() = m_derivationName;;

    attrList.extend("MetadataFolderName", "string");
    attrList["MetadataFolderName"].data<std::string>() = m_mdFolderName;;

    // add contents from different maps
    for (auto const &ent : m_propInt) {
      attrList.extend(ent.first, "int");
      attrList[ent.first].data<int>() = ent.second;
    }
    for (auto const &ent : m_propDouble) {
      attrList.extend(ent.first, "double");
      attrList[ent.first].data<double>() = ent.second;
    }
    for (auto const &ent : m_propBool) {
      attrList.extend(ent.first, "bool");
      attrList[ent.first].data<bool>() = ent.second;
    }
    for (auto const &ent : m_propString) {
      attrList.extend(ent.first, "string");
      attrList[ent.first].data<std::string>() = ent.second;
    }
    for (auto const &ent : m_propVInt) {
      attrList.extend(ent.first, "string");
      attrList[ent.first].data<std::string>() = vecToString(ent.second);
    }
    for (auto const &ent : m_propVDouble) {
      attrList.extend(ent.first, "string");
      attrList[ent.first].data<std::string>() = vecToString(ent.second);
    }
    for (auto const &ent : m_propVBool) {
      attrList.extend(ent.first, "string");
      attrList[ent.first].data<std::string>() = vecToString(ent.second);
    }
    for (auto const &ent : m_propVString) {
      attrList.extend(ent.first, "string");
      attrList[ent.first].data<std::string>() = vecToString(ent.second);
    }
    
    // register attribute list with channel number 0
    cont->add(0, attrList);
    
    //ensures payload correctly inserted into IOVMetaDataContainer
    CHECK( m_metaDataTool->addPayload(fname, cont) );
    
    ATH_MSG_DEBUG("BPhysMetaDataBase::saveMetaData() -- end");
    
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  void BPhysMetadataBase::recordPropertyI(std::string name, int val) {
    ATH_MSG_INFO("Calling recordProperty(int)");
    declareProperty(name, m_propInt[name] = val);
  }
  //--------------------------------------------------------------------------
  void BPhysMetadataBase::recordPropertyD(std::string name, double val) {
    ATH_MSG_INFO("Calling recordProperty(double)");
    declareProperty(name, m_propDouble[name] = val);
  }
  //--------------------------------------------------------------------------
  void BPhysMetadataBase::recordPropertyB(std::string name, bool val) {
    ATH_MSG_INFO("Calling recordProperty(bool)");
    declareProperty(name, m_propBool[name] = val);
  }
  //--------------------------------------------------------------------------
  void BPhysMetadataBase::recordPropertyS(std::string name, std::string val) {
    ATH_MSG_INFO("Calling recordProperty(string)");
    declareProperty(name, m_propString[name] = val);
  }
  //--------------------------------------------------------------------------
  void BPhysMetadataBase::recordPropertyVI(std::string name,
					   std::vector<int> val) {
    ATH_MSG_INFO("Calling recordProperty(vector<int>)");
    declareProperty(name, m_propVInt[name] = val);
  }
  //--------------------------------------------------------------------------
  void BPhysMetadataBase::recordPropertyVD(std::string name,
					   std::vector<double> val) {
    ATH_MSG_INFO("Calling recordProperty(vector<double>)");
    declareProperty(name, m_propVDouble[name] = val);
  }
  //--------------------------------------------------------------------------
  void BPhysMetadataBase::recordPropertyVB(std::string name,
					std::vector<bool> val) {
    ATH_MSG_INFO("Calling recordProperty(vector<bool>)");
    declareProperty(name, m_propVBool[name] = val);
  }
  //--------------------------------------------------------------------------
  void BPhysMetadataBase::recordPropertyVS(std::string name,
					   std::vector<std::string> val) {
    ATH_MSG_INFO("Calling recordProperty(vector<string>)");
    declareProperty(name, m_propVString[name] = val);
  }
  //--------------------------------------------------------------------------
  std::string BPhysMetadataBase::vecToString(std::vector<int> v) const {
    std::string str("[");
    for (unsigned int i=0; i<v.size(); ++i) {
      str += std::to_string(v[i]);
      if ( i < v.size()-1 ) str += ",";
    }
    str += "]";
    return str;
  }
  //--------------------------------------------------------------------------
  std::string BPhysMetadataBase::vecToString(std::vector<double> v) const {
    std::string str("[");
    for (unsigned int i=0; i<v.size(); ++i) {
      str += std::to_string(v[i]);
      if ( i < v.size()-1 ) str += ",";
    }
    str += "]";
    return str;
  }
  //--------------------------------------------------------------------------
  std::string BPhysMetadataBase::vecToString(std::vector<bool> v) const {
    std::string str("[");
    for (unsigned int i=0; i<v.size(); ++i) {
      str += std::to_string(v[i]);
      if ( i < v.size()-1 ) str += ",";
    }
    str += "]";
    return str;
  }
  //--------------------------------------------------------------------------
  std::string BPhysMetadataBase::vecToString(std::vector<std::string> v) const {
    std::string str("[");
    for (unsigned int i=0; i<v.size(); ++i) {
      str += "'";
      str += v[i];
      str += "'";
      if ( i < v.size()-1 ) str += ",";
    }
    str += "]";
    return str;
  }
  //--------------------------------------------------------------------------
}
