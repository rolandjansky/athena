/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BPhysMetadataBase.cxx
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
// - w.w., 2017-01-22: Added use of BPhysMetaDataTool.
// - w.w., 2017-05-27: Removed use of BPhysMetaDataTool and
//                     IOVDbMetaDataTool.
//
// Store JO metadata in the output file.
//
// It uses a FileMetaData object to store job option information
// as metadata in a specific branch whose name needs to prefixed by
// the derivation format name.
//
// This is a base class.  Inherit from it to add the job options you want
// to store.  For a usage example, see
//   Bmumu_metadata.h / Bmumu_metadata.cxx
// and
//   BPHY8.py .
//
// Job options provided by the base class:
// - DerivationName       -- assign the name of the derivation format
// - MetadataFolderName   -- assign the name of the metadata folder,
//                           should start with the derivation format name,
//                           defaults to DerivationName if not set.
//                           
//============================================================================
//

#include "DerivationFrameworkBPhys/BPhysMetadataBase.h"
#include "xAODMetaData/FileMetaData.h"
#include "xAODCore/AuxInfoBase.h"

namespace DerivationFramework {

  //--------------------------------------------------------------------------
  BPhysMetadataBase::BPhysMetadataBase(const std::string& t,
				       const std::string& n,
				       const IInterface*  p)
    : AthAlgTool(t,n,p),
      m_outputMetaStore("StoreGateSvc/MetaDataStore", n) {
    
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare derivation format name
    declareProperty("DerivationName", m_derivationName = "_NOSUCHFORMAT_");

    // Declare metadata folder name (should start with derivation name)
    declareProperty("MetadataFolderName", m_mdFolderName = "_NONE_");

    // Prefix would typically be the derivation format name
    declareProperty("Prefix", m_prefix = "");
    
  }
  //--------------------------------------------------------------------------
  StatusCode BPhysMetadataBase::initialize() {
  
    ATH_MSG_DEBUG("BPhysMetaDataBase::initialize() -- begin");

    // handle general prefix
    if ( m_prefix == "" ) {
      if ( m_derivationName == "_NOSUCHFORMAT_" ) {
	m_prefix = name() +"_";
      } else {
	m_prefix = m_derivationName + "_";
      }
    }
      
    CHECK( saveMetaDataBPhys() );
    
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
#define SET_VALUES_IMP( TYPE, MAP )					\
  for (auto const &ent : MAP) {						\
    fm->auxdata< TYPE >( m_prefix + ent.first ) = ent.second;		\
  }
  
  StatusCode BPhysMetadataBase::saveMetaDataBPhys() const {
    
    ATH_MSG_DEBUG("BPhysMetaDataBase::saveMetaDataBPhys() -- begin");

    std::string mdFolderKey = buildFolderName() + "_MetaData";
    // protection
    if ( m_outputMetaStore->contains< xAOD::FileMetaData >( mdFolderKey ) ) {
      ATH_MSG_WARNING("saveMetaDataBPhys2: "
		      "xAOD::FileMetaData already in output: "
		      << mdFolderKey
		      << " -- BPhys metadata will NOT be saved!");
    } else {
      // create a FileMetaData object
      auto fm    = std::make_unique< xAOD::FileMetaData >();
      auto fmAux = std::make_unique< xAOD::AuxInfoBase >();
      fm->setStore( fmAux.get() );
      
      // fill it
      fm->auxdata< std::string >(m_prefix+"DerivationName"    ) =
	m_derivationName;
      fm->auxdata< std::string >(m_prefix+"MetaDataFolderName") = 
	m_mdFolderName;

      // fill it with contents of maps
      SET_VALUES_IMP( int                     , m_propInt     );
      SET_VALUES_IMP( double                  , m_propDouble  );
      SET_VALUES_IMP( bool                    , m_propBool    );
      SET_VALUES_IMP( std::string             , m_propString  );
      SET_VALUES_IMP( std::vector<int>        , m_propVInt    );
      SET_VALUES_IMP( std::vector<double>     , m_propVDouble );
      SET_VALUES_IMP( std::vector<bool>       , m_propVBool   );
      SET_VALUES_IMP( std::vector<std::string>, m_propVString );
      
      // record it
      ATH_CHECK( m_outputMetaStore->record( std::move(fm), mdFolderKey ) );
      ATH_CHECK( m_outputMetaStore->record( std::move(fmAux),
					    mdFolderKey+"Aux." ) );
    }

    return StatusCode::SUCCESS;
  }
#undef SET_VALUES_IMP
  //--------------------------------------------------------------------------
  std::string BPhysMetadataBase::buildFolderName(std::string fname) const {

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
    return fname;
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
