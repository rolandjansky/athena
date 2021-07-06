/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


//
/** 
 *  @file   BPhysMetaDataHelper.cxx
 *  @author Wolfgang Walkowiak <wolfgang.walkowiak@cern.ch>
 */

// Core include(s):
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/normalizedTypeinfoName.h"

#include <iostream>
#include <utility>


// Local include(s):
#include "xAODBPhys/BPhysMetaDataHelper.h"

namespace xAOD {
  //-----------------------------------------------------------------------
  BPhysMetaDataHelper::BPhysMetaDataHelper(const xAOD::FileMetaData* fm)
    : m_fm(fm), m_tmapOk(false) {
    
    assert(m_fm != 0); // sanity check: m_fm must not be null
  }
  //-----------------------------------------------------------------------
  const xAOD::FileMetaData* BPhysMetaDataHelper::metaObj() const {
    
    return m_fm;
  }
  //-----------------------------------------------------------------------
  void BPhysMetaDataHelper::setPrefix(std::string prefix) {

    m_prefix = std::move(prefix);
  }
  //-----------------------------------------------------------------------
  std::map<std::string, const std::type_info*>
  BPhysMetaDataHelper::varTypes() const {

    if ( ! m_tmapOk ) cacheVarTypes();

    return m_tmap;
  }
  //-----------------------------------------------------------------------
  //
  // Return complete metadata contents as TString
  //
  TString BPhysMetaDataHelper::metaDataToString(TString header) const {
  
    TString str = std::move(header);
    std::map<std::string, std::string> ms = valuesS();
    for ( auto it = ms.begin(); it != ms.end(); ++it ) {
      str += Form("%-30s :S : %s\n", it->first.c_str(), it->second.c_str());
    }
    std::map<std::string, int> mi = valuesI();
    for ( auto it = mi.begin(); it != mi.end(); ++it ) {
      str += Form("%-30s :I : %d\n", it->first.c_str(), it->second);
    }
    std::map<std::string, float> mf = valuesF();
    for ( auto it = mf.begin(); it != mf.end(); ++it ) {
      str += Form("%-30s :F : %f\n", it->first.c_str(), it->second);
    }
    std::map<std::string, double> md = valuesD();
    for ( auto it = md.begin(); it != md.end(); ++it ) {
      str += Form("%-30s :D : %f\n", it->first.c_str(), it->second);
    }
    std::map<std::string, bool> mb = valuesB();
    for ( auto it = mb.begin(); it != mb.end(); ++it ) {
      str += Form("%-30s :B : %s\n", it->first.c_str(),
                                  (it->second ? "True" : "False") );
    }
    std::map<std::string, std::vector<int> > mvi = valuesVI();
    for ( auto it = mvi.begin(); it != mvi.end(); ++it ) {
      TString strv;
      for (auto &ent : it->second) {
	strv += Form("%d,", ent);
      }
      strv.Remove(TString::kTrailing, ',');
      str += Form("%-30s :VI: [%s]\n", it->first.c_str(), strv.Data());
    }
    std::map<std::string, std::vector<float> > mvf = valuesVF();
    for ( auto it = mvf.begin(); it != mvf.end(); ++it ) {
      TString strv;
      for (auto &ent : it->second) {
	strv += Form("%f,", ent);
      }
      strv.Remove(TString::kTrailing, ',');
      str += Form("%-30s :VF: [%s]\n", it->first.c_str(), strv.Data());
    }
    std::map<std::string, std::vector<double> > mvd = valuesVD();
    for ( auto it = mvd.begin(); it != mvd.end(); ++it ) {
      TString strv;
      for (auto &ent : it->second) {
	strv += Form("%f,", ent);
      }
      strv.Remove(TString::kTrailing, ',');
      str += Form("%-30s :VD: [%s]\n", it->first.c_str(), strv.Data());
    }
    std::map<std::string, std::vector<bool> > mvb = valuesVB();
    for ( auto it = mvb.begin(); it != mvb.end(); ++it ) {
      TString strv;
      // vector<bool> needs special treatment
      for (auto &&ent : it->second) {
	strv += Form("%s,", ent ? "True" : "False");
      }
      strv.Remove(TString::kTrailing, ',');
      str += Form("%-30s :VB: [%s]\n", it->first.c_str(), strv.Data());
    }
    std::map<std::string, std::vector<std::string> > mvs = valuesVS();
    for ( auto it = mvs.begin(); it != mvs.end(); ++it ) {
      TString strv;
      for (auto &ent : it->second) {
	strv += Form("%s,", ent.c_str());;
      }
      strv.Remove(TString::kTrailing, ',');
      str += Form("%-30s :VS: [%s]\n", it->first.c_str(), strv.Data());
    }
    str.Remove(TString::kTrailing, '\n');
    
    return str;
  }
  //-----------------------------------------------------------------------
  void BPhysMetaDataHelper::cacheVarTypes() const {

    // Get the variable types
    const SG::auxid_set_t& auxids = m_fm->getAuxIDs();
    for ( SG::auxid_t auxid : auxids  ) {
      SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
      m_tmap[reg.getName(auxid)] = reg.getType(auxid);
    }
    m_tmapOk = true;
  }
  //-----------------------------------------------------------------------
  TString BPhysMetaDataHelper::varTypesToString(TString header) const {

    TString str = std::move(header);
    if ( !m_tmapOk ) cacheVarTypes();
    for (auto &ent : m_tmap) {
      str += Form("%-30s : %s\n", ent.first.c_str(),
		  SG::normalizedTypeinfoName( *ent.second ).c_str());
    }
    str.Remove(TString::kTrailing, '\n');
    return str;
  }
  //-----------------------------------------------------------------------
  //
  // Getter methods for different metadata types
  //
#define GET_VALUE_IMP( TYPE )						\
  bool BPhysMetaDataHelper::value(const std::string& name, TYPE & val)	\
    const {								\
    xAOD::FileMetaData::Accessor< TYPE > acc( m_prefix + name );	\
    if ( ! acc.isAvailable( *m_fm ) ) {					\
      return false;							\
    } else {								\
      val = acc( *m_fm );						\
      return true;							\
    }									\
  }

  GET_VALUE_IMP( int         )
  GET_VALUE_IMP( float       )
  GET_VALUE_IMP( double      )
  GET_VALUE_IMP( bool        )
  GET_VALUE_IMP( std::string )
  GET_VALUE_IMP( std::vector<int>         )
  GET_VALUE_IMP( std::vector<float>       )
  GET_VALUE_IMP( std::vector<double>      )
  GET_VALUE_IMP( std::vector<bool>        )
  GET_VALUE_IMP( std::vector<std::string> )

#undef GET_VALUE_IMP  
  //-----------------------------------------------------------------------
  //
  // Getter methods for maps of different metadata types
  //
#define GET_VALUES_IMP( SYMBOL, TYPE )					\
  std::map<std::string, TYPE> BPhysMetaDataHelper::values ## SYMBOL() const { \
    std::map<std::string, TYPE > metaMap;				\
    const SG::auxid_set_t& auxids = m_fm->getAuxIDs();			\
    for ( SG::auxid_t auxid : auxids  ) {				\
      SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();       \
      const std::type_info* ti = reg.getType( auxid );			\
      if ( ti != NULL && *ti == typeid( TYPE ) ) {			\
	const std::string name = reg.getName( auxid );			\
	const TYPE & value = m_fm->auxdata< TYPE >( name );		\
	metaMap[name] = value;						\
      }									\
    }									\
    return metaMap;							\
  }
  
  GET_VALUES_IMP( I , int         )
  GET_VALUES_IMP( F , float       )
  GET_VALUES_IMP( D , double      )
  GET_VALUES_IMP( B , bool        )
  GET_VALUES_IMP( S , std::string )
  GET_VALUES_IMP( VI, std::vector<int>         )
  GET_VALUES_IMP( VF, std::vector<float>       )
  GET_VALUES_IMP( VD, std::vector<double>      )
  GET_VALUES_IMP( VB, std::vector<bool>        )
  GET_VALUES_IMP( VS, std::vector<std::string> )
    
#undef GET_VALUES_IMP
  //-----------------------------------------------------------------------

} // namespace xAOD
