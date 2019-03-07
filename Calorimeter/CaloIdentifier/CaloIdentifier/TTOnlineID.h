/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOIDENTIFIER_TTONLINEID_H
#define CALOIDENTIFIER_TTONLINEID_H

#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/MsgStream.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"
#include "CaloIdentifier/CaloID_Exception.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>

//using namespace std;

class IdentifierHash;
class Range;

/**
*
* @class TTOnlineID
* @brief Helper class for Calo TT online identifiers
*  
*  This class provides an interface to decode and generate online identifiers
*  for the Calorimeter Trigger Towers.  <p>
* 
*/
class TTOnlineID : public AtlasDetectorID
{
 public:

  typedef Identifier::size_type  size_type;

  TTOnlineID(void);
  ~TTOnlineID(void);


  /* build towerId HWIdentifier */
  /*=========================== */
  /** (1) create towerId from fields */
  HWIdentifier crateId( int crate ) const;
  HWIdentifier crateId( int crate, bool checks ) const;
  /** (2) create towerId from hash */
  HWIdentifier crateId( IdentifierHash crate_hash ) const;
  /** (3) create towerId from hash */
  IdentifierHash crateHash( HWIdentifier crate_id ) const;
  /** (4) iterator over towerId */
  std::vector<HWIdentifier>::const_iterator crate_begin(void) const;
  std::vector<HWIdentifier>::const_iterator crate_end(void) const;

  /* build moduleId HWIdentifier */
  /*=========================== */
  /** (1) create towerId from fields */
  HWIdentifier moduleId( int crate, int module) const;
  HWIdentifier moduleId( int crate, int module, bool checks) const;
  /** (2) create towerId from hash */
  HWIdentifier moduleId( IdentifierHash crate_hash ) const;
  /** (3) create towerId from hash */
  IdentifierHash moduleHash( HWIdentifier crate_id ) const;
  /** (4) iterator over towerId */
  std::vector<HWIdentifier>::const_iterator module_begin(void) const;
  std::vector<HWIdentifier>::const_iterator module_end(void) const;

  /* build submoduleId HWIdentifier */
  /*=========================== */
  /** (1) create towerId from fields */
  HWIdentifier submoduleId( int crate, int module, int submodule) const;
  HWIdentifier submoduleId( int crate, int module, int submodule, bool checks) const;
  /** (2) create towerId from hash */
  HWIdentifier submoduleId( IdentifierHash crate_hash ) const;
  /** (3) create towerId from hash */
  IdentifierHash submoduleHash( HWIdentifier crate_id ) const;
  /** (4) iterator over towerId */
  std::vector<HWIdentifier>::const_iterator submodule_begin(void) const;
  std::vector<HWIdentifier>::const_iterator submodule_end(void) const;

  /* build moduleId HWIdentifier */
  /*=========================== */
  /** (1) create towerId from fields */
  HWIdentifier channelId( int crate, int module, int submodule, int channel) const;
  HWIdentifier channelId( int crate, int module, int submodule, int channel, bool checks) const;
  /** (2) create towerId from hash */
  HWIdentifier channelId( IdentifierHash crate_hash ) const;
  /** (3) create towerId from hash */
  IdentifierHash channelHash( HWIdentifier crate_id ) const;
  /** (4) iterator over towerId */
  std::vector<HWIdentifier>::const_iterator channel_begin(void) const;
  std::vector<HWIdentifier>::const_iterator channel_end(void) const;



  /*  hash tables max size */
  /*====================== */
  size_type  crateHashMax (void) const;
  size_type  moduleHashMax (void) const;
  size_type  submoduleHashMax (void) const;
  size_type  channelHashMax (void) const;


  /** Initialization from the identifier dictionary*/
  virtual int initialize_from_dictionary (const IdDictMgr& dict_mgr);


  // HWIdentifier Fields for TTOnlineID
  // ========================================
  int crate( const HWIdentifier id )const;
  int module( const HWIdentifier id )const;
  int submodule( const HWIdentifier id )const;
  int channel( const HWIdentifier id )const;


  /** context for feedthroughs, feb  & channels */

  IdContext    crateContext(void) const;
  IdContext    moduleContext(void) const;
  IdContext    submoduleContext(void) const;
  IdContext    channelContext(void) const;


private:

  enum {NOT_VALID_HASH = 256000};

  IdentifierHash channel_hash_binary_search(HWIdentifier channelId) const;

  // Check methods
  void  channel_Id_checks(int crate, int module, int submodule, int channel ) const;
  void  channel_Id_checks(const HWIdentifier crateId, int module, int submodule, int channel ) const;
  void  channel_Id_checks(const HWIdentifier moduleId, int submodule, int channel ) const;
  void  channel_Id_checks(const HWIdentifier submoduleId, int channel ) const;

  void  submodule_Id_checks(int crate, int module, int submodule ) const;
  void  module_Id_checks(int crate, int module ) const;
  void  crate_Id_checks( int crate ) const;

  /** create expanded HWIdentifier from HWIdentifier (return == 0 for OK) */
  int  get_expanded_id (const HWIdentifier& id, ExpandedIdentifier& exp_id,
			const IdContext* context) const;

  /** return the value of the detzside field (initialized in initialize_from_dictionary())*/
  int detzside_field_value() const;

  /** init() hashes */
  int  initLevelsFromDict(void) ;
  int  init_hashes(void);

  /** Member Data index */
	size_type m_calo_index;
	size_type m_detzside_index;
	size_type m_l1online_regions_index;
	size_type m_crate_index;
	size_type m_module_index;
	size_type m_submodule_index;
	size_type m_channel_index;

  const IdDictDictionary*       m_dict;
  MultiRange                    m_full_crate_range;
  MultiRange                    m_full_module_range;
  MultiRange                    m_full_submodule_range;
  MultiRange                    m_full_channel_range;

  size_type                     m_crateHashMax;
  size_type                     m_moduleHashMax;
  size_type                     m_submoduleHashMax;
  size_type                     m_channelHashMax;

  std::vector<HWIdentifier>     m_crate_vec;
  std::vector<HWIdentifier>     m_module_vec;
  std::vector<HWIdentifier>     m_submodule_vec;
  std::vector<HWIdentifier>     m_channel_vec;

  int m_iDetZSideFieldValue;

  /**
     @brief small class holding the starting hash value, the min eta and the number of phi bins of each region <br>
     used to CPU-optimize the conversion from an identifier to a hash index
   */
  class HashCalc
  {
  public:
    HashCalc() :
    m_hash(0),
      m_channelmin(0) {}
    IdentifierHash m_hash;
    size_type      m_channelmin;
  };
  std::vector<HashCalc>         m_hash_calcs;


  /** Field Implementation */
  IdDictFieldImplementation   m_calo_impl;
  IdDictFieldImplementation   m_calo_detzside_impl;
  IdDictFieldImplementation   m_crate_impl;
  IdDictFieldImplementation   m_module_impl;
  IdDictFieldImplementation   m_submodule_impl;
  IdDictFieldImplementation   m_channel_impl;
};


//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( TTOnlineID , 38321944 , 1 )


//=====================
// Inline Definitions
//=====================

/* Crates */

//==========================================================================================================
inline HWIdentifier TTOnlineID::crateId( int crate, bool checks ) const
//==========================================================================================================
{
  HWIdentifier result(0);
  m_calo_impl.pack                (10   , result);
  m_calo_detzside_impl.pack       (0   , result);
  m_crate_impl.pack               (crate                , result);

  /* Do checks */
  if(checks) {
    crate_Id_checks( crate );
  }
  return result;
}
inline HWIdentifier TTOnlineID::crateId( int crate ) const
{
  return crateId (crate, do_checks());
}

//============================================================================== */
inline HWIdentifier TTOnlineID::crateId(IdentifierHash cratehash_id) const
//===============================================================================*/
{
    return(m_crate_vec[cratehash_id]);
}

//=============================================================================
inline IdentifierHash TTOnlineID::crateHash  (HWIdentifier crate_id) const{
//=============================================================================
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_crate_vec.begin(),m_crate_vec.end(),crate_id);
  if ( it != m_crate_vec.end() ){
    return (it - m_crate_vec.begin());
  }
  return (0);
}

//=============================================================================
inline std::vector<HWIdentifier>::const_iterator TTOnlineID::crate_begin(void) const {
//=============================================================================
	return(m_crate_vec.begin());
}

//=============================================================================
inline std::vector<HWIdentifier>::const_iterator TTOnlineID::crate_end(void) const {
//=============================================================================
	return(m_crate_vec.end());
}

/* Modules */

//==========================================================================================================
inline HWIdentifier TTOnlineID::moduleId( int crate, int module, bool checks ) const
//==========================================================================================================
{
  HWIdentifier result(0);
  m_calo_impl.pack                (10   , result);
  m_calo_detzside_impl.pack       (0   , result);
  m_crate_impl.pack               (crate                , result);
  m_module_impl.pack              (module               , result);

  /* Do checks */
  if(checks) {
    module_Id_checks( crate, module );
  }
  return result;
}

inline HWIdentifier TTOnlineID::moduleId( int crate, int module ) const
{
  return moduleId (crate, module, do_checks());
}

//============================================================================== */
inline HWIdentifier TTOnlineID::moduleId(IdentifierHash modulehash_id) const
//===============================================================================*/
{
    return(m_module_vec[modulehash_id]);
}

//=============================================================================
inline IdentifierHash TTOnlineID::moduleHash  (HWIdentifier module_id) const{
//=============================================================================
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_module_vec.begin(),m_module_vec.end(),module_id);
  if ( it != m_module_vec.end() ){
    return (it - m_module_vec.begin());
  }
  return (0);
}

//=============================================================================
inline std::vector<HWIdentifier>::const_iterator TTOnlineID::module_begin(void) const {
//=============================================================================
	return(m_module_vec.begin());
}

//=============================================================================
inline std::vector<HWIdentifier>::const_iterator TTOnlineID::module_end(void) const {
//=============================================================================
	return(m_module_vec.end());
}


/* SubModules */

//==========================================================================================================
inline HWIdentifier TTOnlineID::submoduleId( int crate, int module, int submodule, bool checks ) const
//==========================================================================================================
{
  HWIdentifier result(0);
  m_calo_impl.pack                (10   , result);
  m_calo_detzside_impl.pack       (0   , result);
  m_crate_impl.pack               (crate                , result);
  m_module_impl.pack              (module               , result);
  m_submodule_impl.pack           (submodule            , result);

  /* Do checks */
  if(checks) {
    submodule_Id_checks( crate, module, submodule );
  }
  return result;
}
inline HWIdentifier TTOnlineID::submoduleId( int crate, int module, int submodule ) const
{
  return submoduleId (crate, module, submodule, do_checks());
}


//============================================================================== */
inline HWIdentifier TTOnlineID::submoduleId(IdentifierHash submodulehash_id) const
//===============================================================================*/
{
    return(m_submodule_vec[submodulehash_id]);
}

//=============================================================================
inline IdentifierHash TTOnlineID::submoduleHash  (HWIdentifier submodule_id) const{
//=============================================================================
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_submodule_vec.begin(),m_submodule_vec.end(),submodule_id);
  if ( it != m_submodule_vec.end() ){
    return (it - m_submodule_vec.begin());
  }
  return (0);
}

//=============================================================================
inline std::vector<HWIdentifier>::const_iterator TTOnlineID::submodule_begin(void) const {
//=============================================================================
	return(m_submodule_vec.begin());
}

//=============================================================================
inline std::vector<HWIdentifier>::const_iterator TTOnlineID::submodule_end(void) const {
//=============================================================================
	return(m_submodule_vec.end());
}


/* Channels */

//==========================================================================================================
inline HWIdentifier TTOnlineID::channelId( int crate, int module, int submodule, int channel, bool checks ) const
//==========================================================================================================
{
  HWIdentifier result(0);
  m_calo_impl.pack                (10   , result);
  m_calo_detzside_impl.pack       (0   , result);
  m_crate_impl.pack               (crate                , result);
  m_module_impl.pack              (module               , result);
  m_submodule_impl.pack           (submodule            , result);
  m_channel_impl.pack             (channel              , result);

  /* Do checks */
  if(checks) {
    channel_Id_checks( crate, module, submodule, channel );
  }
  return result;
}

inline HWIdentifier TTOnlineID::channelId( int crate, int module, int submodule, int channel ) const
{
  return channelId (crate, module, submodule, channel, do_checks());
}

//============================================================================== */
inline HWIdentifier TTOnlineID::channelId(IdentifierHash channelhash_id) const
//===============================================================================*/
{
    return(m_channel_vec[channelhash_id]);
}

//=============================================================================
inline IdentifierHash TTOnlineID::channelHash  (HWIdentifier channel_id) const{
//=============================================================================
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_channel_vec.begin(),m_channel_vec.end(),channel_id);
  if ( it != m_channel_vec.end() ){
    return (it - m_channel_vec.begin());
  }
  return (0);
}

//=============================================================================
inline std::vector<HWIdentifier>::const_iterator TTOnlineID::channel_begin(void) const {
//=============================================================================
	return(m_channel_vec.begin());
}

//=============================================================================
inline std::vector<HWIdentifier>::const_iterator TTOnlineID::channel_end(void) const {
//=============================================================================
	return(m_channel_vec.end());
}


/* Hash indices */

inline TTOnlineID::size_type TTOnlineID::crateHashMax (void) const
/*====================================================================*/
{
  return m_crateHashMax;
}

inline TTOnlineID::size_type TTOnlineID::moduleHashMax (void) const
/*====================================================================*/
{
  return m_moduleHashMax;
}

inline TTOnlineID::size_type TTOnlineID::submoduleHashMax (void) const
/*====================================================================*/
{
  return m_submoduleHashMax;
}

inline TTOnlineID::size_type TTOnlineID::channelHashMax (void) const
/*====================================================================*/
{
  return m_channelHashMax;
}

inline int TTOnlineID::crate(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_crate_impl.unpack(id));
}

inline int TTOnlineID::module(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_module_impl.unpack(id));
}

inline int TTOnlineID::submodule(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_submodule_impl.unpack(id));
}

inline int TTOnlineID::channel(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_channel_impl.unpack(id));
}

inline int TTOnlineID::detzside_field_value() const
{
  return m_iDetZSideFieldValue;
}

#endif // TTOnlineID_H
