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
   * slot :    
   *               1 to 14               = EM BARREL crates
   *               1 to 13               = Endcap Standard crates
   *               1 to 15               = Endcap Special crates
   *               5,6,7,8,9,10          = HEC in HEC crates
   *               1,2                   = EMEC in HEC crates
   *               1 to 15               = FCAL crates
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
			  int channel ) const throw(LArOnlID_Exception);

  void  channel_Id_checks(const HWIdentifier feedthroughId,
			  int slot,
			  int channel ) const throw(LArOnlID_Exception);

  void  channel_Id_checks(const HWIdentifier febId,
			  int channel ) const throw(LArOnlID_Exception);

  void  feb_Id_checks(int barrel_ec, int pos_neg, int feedthrough, int slot ) const throw(LArOnlID_Exception); 

  void  feedthrough_Id_checks(int barrel_ec, int pos_neg, int feedthrough ) const throw(LArOnlID_Exception); 

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




//=====================
// Inline Definitions
//=====================

							   


inline HWIdentifier LArOnlineID_Base::feedthrough_Id (int barrel_ec, int pos_neg, int feedthrough ) const 
{
  HWIdentifier result(0);

  /*Pack fields independently */
  m_lar_impl.pack        (lar_field_value(), result);
  m_laronline_impl.pack  (s_lar_online_field_value, result);
  m_bec_impl.pack        (barrel_ec        , result);
  m_side_impl.pack       (pos_neg          , result);
  m_feedthrough_impl.pack(feedthrough      , result);
  if ( m_this_is_slar )
	m_slar_impl.pack       (1                , result);

  /* Do checks */
  if(m_do_checks) 
    {
      feedthrough_Id_checks ( barrel_ec, pos_neg, feedthrough );
    }
  return result;
}

inline HWIdentifier LArOnlineID_Base::feedthrough_Id(IdentifierHash feedthroughHashId) const
/*=============================================================================== */
{
  return(m_feedthrough_vec[feedthroughHashId]);
}

inline IdentifierHash LArOnlineID_Base::feedthrough_Hash (HWIdentifier feedthroughId) const
/*=============================================================================== */
{
    std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_feedthrough_vec.begin(),m_feedthrough_vec.end(),feedthroughId);
    if ( it != m_feedthrough_vec.end() ){
	return (it - m_feedthrough_vec.begin());
    }
    return (0);
}

inline HWIdentifier LArOnlineID_Base::feedthrough_Id(const HWIdentifier Id) const
/*============================================================================== */
{
  HWIdentifier result(Id);
  m_slot_impl.reset(result);
  m_channel_in_slot_impl.reset(result);
  return(result);
}

inline std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::feedthrough_begin(void) const
/*====================================================================*/
{
  return(m_feedthrough_vec.begin());
}

inline std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::feedthrough_end(void) const
/*==================================================================*/
{
  return(m_feedthrough_vec.end());
}


/* FEB id */
/*========*/

inline HWIdentifier LArOnlineID_Base::feb_Id(int barrel_ec, int pos_neg, 
					int feedthrough, int slot ) const 
/*==================================================================== */
{
  HWIdentifier result(0);

  /*Pack fields independently */
  m_lar_impl.pack        (lar_field_value(), result);
  m_laronline_impl.pack  (s_lar_online_field_value, result);
  m_bec_impl.pack        (barrel_ec        , result);
  m_side_impl.pack       (pos_neg          , result);
  m_feedthrough_impl.pack(feedthrough      , result);
  m_slot_impl.pack       (slot             , result);
  if ( m_this_is_slar )
    m_slar_impl.pack       (1                , result);

  /* Do checks */
  if(m_do_checks) {
    feb_Id_checks ( barrel_ec, pos_neg, feedthrough, slot );
  }
  return result;
}

inline HWIdentifier LArOnlineID_Base::feb_Id(const HWIdentifier feedthroughId , int slot) const
/*==================================================================================== */
{
  HWIdentifier result(feedthroughId);
  /* Pack fields independently */
  m_slot_impl.reset           (result);
  m_slot_impl.pack            (slot  , result);
  if ( m_this_is_slar )
     m_slar_impl.pack            (1     , result);

  return(result);
}

inline HWIdentifier LArOnlineID_Base::feb_Id(const HWIdentifier channelId ) const
/*======================================================================= */
{
  HWIdentifier result(channelId);
  m_channel_in_slot_impl.reset(result);
  return(result);
}

inline HWIdentifier LArOnlineID_Base::feb_Id(IdentifierHash febHashId) const
/*================================================================== */
{
  return(m_feb_vec[febHashId]);
}

inline IdentifierHash LArOnlineID_Base::feb_Hash (HWIdentifier febId) const
/*=============================================================================== */
{
    // Get the hash caculator for the febs
    const HashCalcFeb& hc = m_feb_hash_calcs[m_bec_ft_impl.unpack(febId)];
    // Two cases: 
    //   1) slot values are enumerated and we must look for a matching
    //      value to obtain the index
    //   2) slot values are a continuous range, then the slot index is
    //      sufficient for the hash calculation
    if (hc.m_slot_values.size()) {
        // find matching value
        int slotValue = slot(febId);
        for (int i = 0; (unsigned int)i < hc.m_slot_values.size(); ++i) {
            if (slotValue == hc.m_slot_values[i]) return (hc.m_hash + i);
        }
        std::cout << "LArOnlineID_Base::feb_Hash - ***** ERROR: could not match slot value for has calculation " << std::endl;
    }
    size_type slotIndex = m_slot_impl.unpackToIndex(febId);
    return (hc.m_hash + slotIndex);
}

inline IdentifierHash LArOnlineID_Base::feb_Hash_binary_search (HWIdentifier febId) const
/*=============================================================================== */
{
    std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_feb_vec.begin(),m_feb_vec.end(),febId);
    if ( it != m_feb_vec.end() ){
	return (it - m_feb_vec.begin());
    }
    return (0);
}

inline HWIdentifier LArOnlineID_Base::channel_Id( int barrel_ec, int pos_neg, int feedthrough, 
					     int slot,      int channel ) const 
/*============================================================================== */
{  
  HWIdentifier result(0);
  /* Pack fields independently */
  m_lar_impl.pack            (lar_field_value()    , result);
  m_laronline_impl.pack      (s_lar_online_field_value , result);
  m_bec_impl.pack            (barrel_ec            , result);
  m_side_impl.pack           (pos_neg              , result);
  m_feedthrough_impl.pack    (feedthrough          , result);
  m_slot_impl.pack           (slot                 , result);
  m_channel_in_slot_impl.pack(channel              , result);
  if ( m_this_is_slar )
     m_slar_impl.pack           (1                    , result);

  /* Do checks */
  if(m_do_checks) {
    channel_Id_checks( barrel_ec, pos_neg, feedthrough, slot, channel );
  }
  return result;
}


inline HWIdentifier LArOnlineID_Base::channel_Id(IdentifierHash channelHashId) const
/*===================================================================*/
{
    return(m_channel_vec[channelHashId]);
}


inline HWIdentifier LArOnlineID_Base::channel_Id(const HWIdentifier feedthroughId,int slot,int channel) const 
/*==================================================================================================== */
{  
  HWIdentifier result(feedthroughId);
  /* Pack fields independently */
  m_slot_impl.reset           (result);
  m_channel_in_slot_impl.reset(result);
  m_slot_impl.pack            (slot  , result);
  m_channel_in_slot_impl.pack (channel, result);
  if ( m_this_is_slar )
    m_slar_impl.pack            (1  , result);

  /* Do checks */
  if(m_do_checks) {
      channel_Id_checks( feedthroughId, slot, channel );
  }
  return result;
}

inline HWIdentifier LArOnlineID_Base::channel_Id(const HWIdentifier febId, int channel) const 
/*======================================================================================= */
{  
  HWIdentifier result(febId);
  /* Pack fields independently */
  m_channel_in_slot_impl.reset(result);
  m_channel_in_slot_impl.pack (channel, result);
  if ( m_this_is_slar )
	m_slar_impl.pack (1, result);

  /* Do checks */
  if(m_do_checks) {
    channel_Id_checks( febId, channel );
  }
  return result;
}

//----------------------------------------------------------------------------
inline IdentifierHash LArOnlineID_Base::channel_Hash (HWIdentifier channelId) const
{
    const HashCalc& hc = m_chan_hash_calcs[m_bec_slot_impl.unpack(channelId)];
    return (hc.m_hash + channel(channelId));
}

inline IdentifierHash LArOnlineID_Base::channel_Hash_binary_search (HWIdentifier channelId) const
/*=========================================================================*/
{
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_channel_vec.begin(),m_channel_vec.end(),channelId);
  if ( it != m_channel_vec.end() ){
    return (it - m_channel_vec.begin());
  }
  return(0) ;
}

/* BOOLEAN */

/* 
 *=============================================================
 *=============================================================
 *                   Recommended methods 
 *=============================================================
 *=============================================================
 */

inline bool LArOnlineID_Base::isEMBchannel(const HWIdentifier id) const
/*========================================================*/
{
    return (barrel_ec(id)==0);
}
inline bool LArOnlineID_Base::isFCALchannel(const HWIdentifier id) const
/*========================================================*/
{
  return ( barrel_ec(id)==1 && feedthrough(id)==6 );
}
inline bool LArOnlineID_Base::isHECchannel(const HWIdentifier id) const
/*========================================================*/
{
  int ft = feedthrough(id);
  return ( barrel_ec(id)==1 
	   && 
	   ( ft==3 || ft==10 || ft==16 || ft==22 )
	   &&
	   slot(id) > 2 );
}
inline bool LArOnlineID_Base::isEMECIW(const HWIdentifier id) const {
  /*======================================================*/
  // 
  int bec= barrel_ec(id);
  int ft = feedthrough(id);
  int sl = slot(id);
  return (bec==1 && sl<3 && (ft==3  || ft==10 || 
			     ft==16 || ft==22)); 
}
inline bool LArOnlineID_Base::isEMECOW(const HWIdentifier id) const {
  /*======================================================*/
  // 
  int bec= barrel_ec(id);
  int ft = feedthrough(id);
  return (bec==1 && 
	  (ft==0 || ft==1 ||
	   ft==2 || ft==4 ||
	   ft==5 || ft==7 ||
	   ft==8 || ft==9 ||
	   ft==11|| ft==12||
	   ft==13|| ft==14||
	   ft==15|| ft==17||
	   ft==18|| ft==19||
	   ft==20|| ft==21||
	   ft==23|| ft==24 )
	  );
}
inline bool LArOnlineID_Base::isEMECchannel(const HWIdentifier id) const
/*========================================================*/
{/* redefinition with isEMECIW and isEMECOW */
  return (isEMECOW(id) || isEMECIW(id));
}


inline bool LArOnlineID_Base::isEMBPS(const HWIdentifier id) const
/*=================================================================*/
{
  int bec= barrel_ec(id);
  int sl = slot(id);
  return ( 
	  bec == 0 && sl == 1 
	  );
}

inline bool LArOnlineID_Base::isEMECPS(const HWIdentifier id) const
/*=================================================================*/
{/* redefinition (bug fix) */
  return (isEMECOW(id) && slot(id)==1);
}

inline bool LArOnlineID_Base::isPS(const HWIdentifier id) const
/*=================================================================*/
{/* redefinition with isEMBPS and isEMECPS */
  return ( isEMBPS(id) || isEMECPS(id));
}


inline bool LArOnlineID_Base::isNotWarmCableConnected(const HWIdentifier id) const
/*========================================================*/
{
  int ft = feedthrough(id);
  int sl = slot(id);
  int ch = channel(id);
  return ( 
	  (
	   isEMBchannel(id) &&
	   ( (-1 < ft  && ft < 32) &&
	     ( sl == 14 ) &&
	     ( 63 < ch && ch < 128 )
	     )
	   ) 
	  ||
	  (
	   isEMECchannel(id) && sl == 1 &&
	   (ft == 2 || ft == 9 || ft == 15 || ft == 21 ) &&
	   (63 < ch && ch < 128) 
	   )
	  );
}


/* 
 *=============================================================
 *=============================================================
 *                   Obsolete methods 
 *=============================================================
 *=============================================================
 */

inline bool LArOnlineID_Base::isEmBarrelOnline(const HWIdentifier id) const
/*========================================================*/
{/* redefined to isEMBchannel() */
  return (isEMBchannel(id));
}

inline bool LArOnlineID_Base::isEndcapOnline(const HWIdentifier id) const
/*========================================================*/
{
  return (barrel_ec(id)==1);
}

inline bool LArOnlineID_Base::isFcalOnline(const HWIdentifier id) const
/*========================================================*/
{/* redefinition (was redundant with isFCALchannel) */
  return isFCALchannel(id);
}
inline bool LArOnlineID_Base::isEmEndcapOnline(const HWIdentifier id) const
/*========================================================*/
{/* redefinition */
 return isEMECchannel(id);
}


/* 
 *=============================================================
 *=============================================================
 *                  Specific UseCases
 *=============================================================
 *=============================================================
 */

inline bool LArOnlineID_Base::isEmEndcapStandardOnline(const HWIdentifier id) const
/*======================================================================*/
{
  int ft = feedthrough(id);
  return ( barrel_ec(id)==1 && 
	   (ft==0 ||
	    ft==1 ||
	    ft==4 ||
	    ft==5 ||
	    ft==7 ||
	    ft==8 ||
	    ft==11 ||
	    ft==12 ||
	    ft==13 ||
	    ft==14 ||
	    ft==17 ||
	    ft==18 ||
	    ft==19 ||
	    ft==20 ||
	    ft==23 ||
	    ft==24)
	   );
}
inline bool LArOnlineID_Base::isEmEndcapSpecialOnline(const HWIdentifier id) const
/*======================================================================*/
{
  int ft = feedthrough(id);
  return ( barrel_ec(id)==1 && 
	   (ft==2 ||
	    ft==9 ||
	    ft==15 ||
	    ft==21 )
	   );
}

inline bool LArOnlineID_Base::isCalibration(const HWIdentifier id) const
/*========================================================*/
{
  int bec= barrel_ec(id);
  int ft = feedthrough(id);
  int sl = slot(id);
  return (  (bec==0 && sl==15) 
	    ||
	    ( bec==1 && sl==15 &&
	      (ft==0 ||
	       ft==1 ||
	       ft==4 ||
	       ft==5 ||
	       ft==7 ||
	       ft==8 ||
	       ft==11 ||
	       ft==12 ||
	       ft==13 ||
	       ft==14 ||
	       ft==17 ||
	       ft==18 ||
	       ft==19 ||
	       ft==20 ||
	       ft==23 ||
	       ft==24 )
	      )
	    ||
	    ( barrel_ec(id)==1 && 
	      (sl==3 || sl==4 || sl==12) &&
	      (ft==3 ||
	       ft==10 ||
	       ft==16 ||
	       ft==22 )
	      )
	    ||
	    ( barrel_ec(id)==1 && sl==16 )
	    );
}


/*
 *================================================================
 *================================================================
 * The following methods should NOT be used for new applications:
 *================================================================
 *================================================================
 */

inline bool LArOnlineID_Base::isHecOnline(const HWIdentifier id) const
/*========================================================*/
{
  /* Must be used with CARE !
   * This method returns true for all slots/channel in HEC crates,  
   * thus including the EMEC slot/channels that are present 
   * in slot 1 and slot 2 of HEC crates ! 
   * To access specifically EMEC in HEC slot/channel, you then 
   * have to use isEMECinHECchannel()
   */
  int ft = feedthrough(id);
  return ( 
	  //(slot(id)!=1 && slot(id)!= 2) &&
	  barrel_ec(id)==1 && 
	  (ft==3 || 
	   ft==10 || 
	   ft==16 || 
	   ft==22 )
	  );
}

inline bool LArOnlineID_Base::isHecOnlineFebId(const HWIdentifier febId) const
/*========================================================*/
{
  /* Must be used with CARE !
   * This method returns true for all slots/channel in HEC crates,  
   * thus including the EMEC slot/channels that are present 
   * in slot 1 and slot 2 of HEC crates ! 
   * To access specifically EMEC in HEC slot/channel, you then 
   * have to use isEMECinHECchannel()
   */
  int ft = feedthrough(febId);
  return ( barrel_ec(febId)==1 && 
	   (ft==3 || 
	    ft==10 || 
	    ft==16 || 
	    ft==22 )
	   );
}

inline bool LArOnlineID_Base::isEMECinHECchannel(const HWIdentifier id) const
/*=================================================================*/
{/* re-definition with isEMECIW */
  return isEMECIW(id);
}









inline LArOnlineID_Base::size_type LArOnlineID_Base::feedthroughHashMax (void) const
     /*=======================================================================*/
{
  return m_feedthroughHashMax;
}
inline LArOnlineID_Base::size_type LArOnlineID_Base::febHashMax (void) const
/*=======================================================================*/
{
  return m_febHashMax;
}
inline LArOnlineID_Base::size_type LArOnlineID_Base::channelHashMax (void) const
/*====================================================================*/
{
  return m_channelHashMax;
}




inline std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::feb_begin(void) const
/*====================================================================*/
{
  return(m_feb_vec.begin());
}
inline std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::feb_end(void) const
/*==================================================================*/
{
  return(m_feb_vec.end());
}

inline std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::channel_begin(void) const
/*======================================================================*/
{
  return(m_channel_vec.begin());
}
inline std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::channel_end(void) const
/*======================================================================*/
{
  return(m_channel_vec.end());
}

inline int LArOnlineID_Base::barrel_ec(const HWIdentifier id)const
/*=========================================================*/
{
  return (m_bec_impl.unpack(id));
}

inline int LArOnlineID_Base::feedthrough(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_feedthrough_impl.unpack(id));
}

inline int LArOnlineID_Base::pos_neg(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_side_impl.unpack(id));
}


inline int LArOnlineID_Base::slot(const HWIdentifier id)const
/*=====================================================*/
{
  return (m_slot_impl.unpack(id));
}

inline int LArOnlineID_Base::channel(const HWIdentifier id)const
/*=====================================================*/
{
  return (m_channel_in_slot_impl.unpack(id));
}

inline int LArOnlineID_Base::is_slar(const HWIdentifier id)const
/*=====================================================*/
{
  return (m_slar_impl.unpack(id));
}


#endif // LARONLINE_ID_BASE_H



