/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARONLINEID_BASE_H
#define LARONLINEID_BASE_H

#include "CLIDSvc/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"
#include "LArIdentifier/LArOnlID_Exception.h"
#include "string.h"
#include <vector>
#include <algorithm>

#include <iostream>


class IdentifierHash;
class Range;


  /**
   * @brief Helper for the Liquid Argon Calorimeter cell identifiers. 
   * This class provides a hardware description of a calorimeter cell 
   * @author Arnaud Lucotte 
   * 
   * LArCalorimeter fields:
   * ----------------------
   * Part:         4 = LArOnline 
   * barrel_ec :   0 = barrel 
   *               1 = end-cap
   * pos_neg :     0 = negative eta side (C side)
   *               1 = positive eta side (A side)
   * feedthrough :  
   *               [0,31]                = EM BARREL crates
   *               00,01,04,07,08,11,12,13,
   *               14,17,18,19,20,23,24  = EMEC Standard crates
   *               02,09,15,21           = EMEC Special crates
   *               03,10,16,22           = HEC crates 
   *                                      (note: slots 1&2 are EMEC slots)
   *               06                    = FCAL crate     
   *               25-27                 = sFCAL (is reported as FCAL)
   * slot :    
   *               1 to 14               = EM BARREL crates
   *               1 to 13               = Endcap Standard crates
   *               1 to 15               = Endcap Special crates
   *               5,6,7,8,9,10          = HEC in HEC crates
   *               1,2                   = EMEC in HEC crates
   *               1 to 15               = FCAL crates
   *               1 to 10               = sFCAL crates
   * channel: 
   *              0 to 127              
   *  
   * In the following, four contexts are defined:
   * - cryostatEndContext
   * - feedthroughContext
   * - febContext 
   * - channelContext
   *
   * Throw a LArOnlID_Exception if one parameter is out of range 
   *
   *
   * Access Methods :
   * ----------------
   * There are three types of methods:
   * - recommended methods 
   * - methods that can still be used but with special care
   * - methods that are now obsolete
   *
   * RECOMMENDED methods :
   *    bool  isEMBchannel            (const HWIdentifier Id) const;
   *    bool  isEMECchannel           (const HWIdentifier Id) const;
   *    bool  isHECchannel            (const HWIdentifier Id) const;
   *    bool  isFCALchannel           (const HWIdentifier Id) const;
   *    bool  isEMECIW                (const HWIdentifier Id) const;
   *    bool  isEMECOW                (const HWIdentifier Id) const;
   *    bool  isEndcapOnline          (const HWIdentifier Id) const;
   *    bool  isEmEndcapStandardOnline(const HWIdentifier Id) const;
   *    bool  isEmEndcapSpecialOnline (const HWIdentifier Id) const;
   *    bool  isEMBPS                 (const HWIdentifier Id) const;
   *    bool  isEMECPS                (const HWIdentifier Id) const;
   *    bool  isPS                    (const HWIdentifier Id) const;
   *
   * USE-with-CARE methods (still used for historical reasons):
   *    bool  isHecOnline     (const HWIdentifier chId) const; 
   *    bool  isHecOnlineFebId(const HWIdentifier febId)const; 
   *    bool  isEMECinHECchannel(const HWIdentifier Id) const; 
   *
   * OBSOLETE methods :
   *    bool  isEmBarrelOnline(const HWIdentifier chId) const; -> use isEMBchannel
   *    bool  isEmEndcapOnline(const HWIdentifier chId) const; -> use isEMECchannel
   *    bool  isFcalOnline    (const HWIdentifier chId) const; -> use isFCALchannel
   *
   *    int   channelInSlotMax         (const HWIdentifier Id) const;  
   *
   * The following methods are now disabled:
   *    bool  isEMB_BACK               (const HWIdentifier id) const;
   *    bool  isEMB_MIDDLE             (const HWIdentifier id) const;
   *    bool  isEMB_FRONT              (const HWIdentifier id) const;
   *===========================================================================
   */
class LArOnlineID_Base : public AtlasDetectorID
{
 public:        

  typedef Identifier::size_type  size_type;
  
  /** 
   * @brief Default constructor
   */
  LArOnlineID_Base(void);    
  /** 
   * @brief Default destructor
   */
  ~LArOnlineID_Base(void);
  
  /**
   * @brief Create a feedthrough identifier from fields 
   */
  HWIdentifier feedthrough_Id( int barrel_ec, int pos_neg, int feedthrough ) const;
  
  /**
   * @brief Create a feedthrough identifier from hash_id 
   */
  HWIdentifier feedthrough_Id(IdentifierHash feedthroughHash) const;

  /**
   * @brief Create hash id from feedthrough identifiers 
   */
  IdentifierHash feedthrough_Hash(HWIdentifier feedthroughId) const;

  /**
   * @brief Return an iterator pointing to Feedthrough identifiers collection
   */
  std::vector<HWIdentifier>::const_iterator feedthrough_begin(void) const;
  std::vector<HWIdentifier>::const_iterator feedthrough_end  (void) const;


  /**
   * @brief Create feb_Id from fields 
   */
  HWIdentifier feb_Id(int barrel_ec,
		      int pos_neg,
		      int feedthrough, 
		      int slot ) const;
  /**
   * @brief create feb identifier from hash id 
   */
  HWIdentifier feb_Id(IdentifierHash febHash) const;	

  /**
   * @brief Create feb hash identifiers from feb identifiers 
   */
  IdentifierHash feb_Hash(HWIdentifier febId) const;

  /**
   * @brief Returns an iterator pointing to a feb identifier collection
   */
  std::vector<HWIdentifier>::const_iterator feb_begin(void) const;
  std::vector<HWIdentifier>::const_iterator feb_end  (void) const;



  /**
   * @brief create channel identifier from fields 
   */
  HWIdentifier channel_Id(int barrel_ec,
			  int pos_neg,
			  int feedthrough, 
			  int slot,
			  int channel ) const;
  /**
   * @brief Create channel identifiers from hash id 
   */
  HWIdentifier channel_Id(IdentifierHash channelHash) const;	

  /**
  * @brief Create channel_hash from channel_Id 
  */
  IdentifierHash channel_Hash(HWIdentifier channelId) const;

  /**
   * @brief Returns an iterator pointing to a channel identifier collection
   */
  std::vector<HWIdentifier>::const_iterator channel_begin(void) const;
  std::vector<HWIdentifier>::const_iterator channel_end  (void) const;
  

  /**
   * @brief Define a channel identifier from a feedthrough identifier
   */
  HWIdentifier feedthrough_Id(const HWIdentifier Id) const ;
  /**
   * @brief Define a feb identifier from a channel identifier
   */
  HWIdentifier feb_Id(const HWIdentifier channelId) const ;
  /**
   * @brief Define a feb identifier from a feedthrough identifier
   */
  HWIdentifier feb_Id(const HWIdentifier feedthroughId, int slot) const ;
  /**
   * @brief Define a channel identifier from a feedthrough identifier
   */
  HWIdentifier channel_Id(const HWIdentifier feedthroughId, int slot, int channel ) const;
  /**
   * @brief Define a channel identifier from a feb identifier
   */
  HWIdentifier channel_Id(const HWIdentifier febId, int channel ) const;


  /**
   * @brief Define feedthrough hash tables max size 
   */
  size_type  feedthroughHashMax (void) const;
  /**
   *  @brief define feb hash tables max size 
   */
  size_type  febHashMax (void) const;
  /**
   *  @brief Define channel hash tables max size 
   */
  size_type  channelHashMax (void) const;


  /** 
   * @returns true if the Id belongs to a subsystem 
   */
  /* recommended methods */
  bool  isEMBchannel             (const HWIdentifier id) const;
  bool  isEMECchannel            (const HWIdentifier id) const;
  bool  isHECchannel             (const HWIdentifier id) const;
  bool  isFCALchannel            (const HWIdentifier id) const;
  bool  isEMECIW                 (const HWIdentifier id) const;
  bool  isEMECOW                 (const HWIdentifier id) const;
  bool  isEMBPS                  (const HWIdentifier id) const;
  bool  isEMECPS                 (const HWIdentifier id) const;
  bool  isPS                     (const HWIdentifier id) const;
  bool  isNotWarmCableConnected  (const HWIdentifier id) const;
  bool  isCalibration            (const HWIdentifier id) const;

  /* To be used with care */
  bool	isHecOnline              (const HWIdentifier id) const;
  bool	isHecOnlineFebId         (const HWIdentifier febId) const;
  bool	isEMECinHECchannel       (const HWIdentifier id) const;

  /* obsolete */
  bool	isEmBarrelOnline         (const HWIdentifier id) const;
  bool	isEmEndcapOnline         (const HWIdentifier id) const;
  bool  isEndcapOnline           (const HWIdentifier id) const;
  bool	isFcalOnline             (const HWIdentifier id) const;
  bool	isEmEndcapStandardOnline (const HWIdentifier id) const;
  bool	isEmEndcapSpecialOnline  (const HWIdentifier id) const;

  /**
   * @brief Return the Maximum channel number of a given feb slot
   */
  int   channelInSlotMax         (const HWIdentifier Id) const;

  /**
   * @brief Return a string corresponding to a feedthrough name given an identifier
   */
  std::string feedthrough_name(const HWIdentifier id ) const;

  /**
   * @brief Return a string corresponding to a feedthrough name given an identifier
   */
  std::string channel_name(const HWIdentifier id ) const;
    
  /**
   * @brief Return the position barrel or endcap of a hardware cell identifier: <br>
   * barrel_ec  = [0,1] : Barrel / EndCap
   */
  int barrel_ec(const HWIdentifier id)const;  

  /**
   * @brief Return the side of a hardware cell identifier  <br>
   * pos_neg = [0,1]  positive-side or negative-side Barrel/Endcap
   */
  int pos_neg(const HWIdentifier id)const; 

  /** 
   * @brief Return the feedthrough of a hardware cell identifier : <br>
   * feedthrough = [0,31]  Barrel - A/C side or H/I half barrel   <br> 
   * feedthrough = [1,25]  Endcap-Standard EMEC 1.6<|eta|<2.4 - A/C side  <br>
   * feedthrough = [1,25]  Endcap-Special EMEC 1.5<|eta|<1.6 and 2.4<|eta|<3.2  <br>
   * feedthrough = [1,25]  HEC A/C side
   */
  int feedthrough(const HWIdentifier id)const; 

  /**
   * @brief Return the slot number of a hardware cell identifier: <br>
   * slot  =      [1,15]   Slot-ID in top part of the crate (higher phi)<br>
   * Note that a crate is = Slot(1-15)L + Slot(1-15)R 
   */
  int slot(const HWIdentifier id)const;  

  /**
   * @brief Return the channel number of a hardware cell identifier <BR>
   * channel = [0,127] in all FEB 
   */
  int channel(const HWIdentifier id)const;  

  /**
   * @brief Return whether the channel is in slar or not
   * is_ slar = [0,1] in all FEB
   */
  int is_slar(const HWIdentifier id)const;


  /**
   * @brief Returns false if the identifier is not a LAr-online id or any of the sub-fields is out of range. <BR>
   */
  bool isValidId(const HWIdentifier id) const;

  /**
   * @brief initialization from the identifier dictionary
   */
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);


  /** 
   * @brief Define context for cryostat (obsolete)
   */
  IdContext    cryostatEndContext(void) const;
  /** 
   * @brief Define context for feedthroughs
   */
  IdContext    feedthroughContext(void) const;
  /** 
   * @brief Define context for feb
   */
  IdContext    febContext(void) const;
  /** 
   * @brief Define context for channel
   */
  IdContext    channelContext(void) const;

protected:    
/*==== */
     
  enum {NOT_VALID_HASH = 256000};
  bool m_this_is_slar;

  IdentifierHash feb_Hash_binary_search(HWIdentifier channelId) const;

  IdentifierHash channel_Hash_binary_search(HWIdentifier channelId) const;

  // Check methods
  void  channel_Id_checks(int barrel_ec, 
			  int pos_neg,
			  int feedthrough,
			  int slot,
			  int channel ) const;

  void  channel_Id_checks(const HWIdentifier feedthroughId,
			  int slot,
			  int channel ) const;

  void  channel_Id_checks(const HWIdentifier febId,
			  int channel ) const;

  void  feb_Id_checks(int barrel_ec, int pos_neg, int feedthrough, int slot ) const;

  void  feedthrough_Id_checks(int barrel_ec, int pos_neg, int feedthrough ) const; 

  /** create expanded HWIdentifier from HWIdentifier (return == 0 for OK) */
  int  get_expanded_id  (const HWIdentifier& id, 
			 ExpandedIdentifier& exp_id, 
			 const IdContext* context) const;
  int  get_expanded_calib_id  (const HWIdentifier& id, 
			       ExpandedIdentifier& exp_id, 
			       const IdContext* context) const;
  int  initLevelsFromDict(const std::string& group_name) ;
  int  init_hashes(void) ;
  size_type m_laronlineRegion_index;   // LArOnline_region
  size_type m_lar_index;               // LAr
  size_type m_laronline_index;         // 4
  size_type m_bec_index;               // [0 Barrel, 1 Endcap]
  size_type m_side_index;              // [-1 negative-side, 1 positive-side]
  size_type m_feedthrough_index;       // [0,31] or [0,24]
  size_type m_slot_index;              // [1,15] 
  size_type m_channel_in_slot_index;   // [0,127]
  size_type m_slar_index;              // [0,1]

  const IdDictDictionary*       m_dict;
  MultiRange                    m_full_laronline_range;
  MultiRange                    m_full_feedthrough_range;
  MultiRange                    m_full_feb_range;
  MultiRange                    m_full_channel_range;
  size_type                     m_feedthroughHashMax;
  size_type                     m_febHashMax;
  size_type                     m_channelHashMax;

  std::vector<HWIdentifier>     m_feedthrough_vec;
  std::vector<HWIdentifier>     m_feb_vec;
  std::vector<HWIdentifier>     m_channel_vec;

  /** Field Implementation */
  IdDictFieldImplementation     m_lar_impl;
  IdDictFieldImplementation     m_laronline_impl;
  IdDictFieldImplementation     m_bec_impl;
  IdDictFieldImplementation     m_side_impl;
  IdDictFieldImplementation     m_feedthrough_impl;
  IdDictFieldImplementation     m_slot_impl;
  IdDictFieldImplementation     m_channel_in_slot_impl;
  IdDictFieldImplementation     m_bec_ft_impl;
  IdDictFieldImplementation     m_bec_slot_impl;
  IdDictFieldImplementation     m_slar_impl;

  /** 
   * @brief Define the Hash identifier for channels and febs
   */
  class HashCalc
  {
  public:
    HashCalc() : m_hash(0) {}
    IdentifierHash m_hash;
  };
  class HashCalcFeb
  {
  public:
    HashCalcFeb() : m_hash(0) {}
    IdentifierHash m_hash;
    std::vector<int> m_slot_values; // need to save slot values for enumerated slot field
  };
  std::vector<HashCalcFeb>      m_feb_hash_calcs;
  std::vector<HashCalc>         m_chan_hash_calcs;
  
};

static const int s_lar_online_field_value = 4 ;
static const int s_lar_onlineCalib_field_value = 5 ;


//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
//CLASS_DEF( LArOnlineID , 158698068 , 1 )


#endif // LARONLINE_ID_BASE_H



