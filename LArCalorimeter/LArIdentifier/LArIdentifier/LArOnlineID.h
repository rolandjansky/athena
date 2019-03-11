/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARONLINEID_H
#define LARONLINEID_H

#include "LArIdentifier/LArOnlineID_Base.h"
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
   * SPECIFIC usecases :
   *    bool  is_H8                    (const HWIdentifier id) const;
   *    bool  is_H8crack               (const HWIdentifier id) const;
   *    bool  is_H8low                 (const HWIdentifier id) const;
   *    bool  is_H8high                (const HWIdentifier id) const;
   *    bool  is_H6                    (const HWIdentifier id) const;
   *    bool  is_H6FT                  (const HWIdentifier feedthroughId) const;
   *    bool  is_H6fcal                (const HWIdentifier id) const;
   *    bool  is_H6hec                 (const HWIdentifier id) const;
   *    bool  is_H6emec                (const HWIdentifier id) const;
   *    bool  is_H8onlineOnly          (const HWIdentifier id) const;
   *
   *    int   channelInSlotMax         (const HWIdentifier Id) const;  
   *
   * The following methods are now disabled:
   *    bool  isEMB_BACK               (const HWIdentifier id) const;
   *    bool  isEMB_MIDDLE             (const HWIdentifier id) const;
   *    bool  isEMB_FRONT              (const HWIdentifier id) const;
   *===========================================================================
   */
class LArOnlineID : public LArOnlineID_Base
{
 public:        

  typedef Identifier::size_type  size_type;
  
  /** 
   * @brief Default constructor
   */
  LArOnlineID(void);    
  /** 
   * @brief Default destructor
   */
  ~LArOnlineID(void);
  
  int initialize_from_dictionary(const IdDictMgr&);
  
  /**
   * @brief Build calibration module identifier from fields
   */
  HWIdentifier calib_module_Id(int barrel_ec,
			       int pos_neg,
			       int feedthrough, 
			       int slot ) const;
  /**
   * @brief Create calibration module identifier from hash identifiers
   */
  HWIdentifier calib_module_Id(IdentifierHash calibModuleHash) const;	
  /**
   * @brief create calibration module_hash identifier from module Identifier
   */
  IdentifierHash calib_module_Hash(HWIdentifier calibModuleId) const;

  /**
   * @brief Return an iterator pointing to a calibFeb identifier 's collection
   */
  std::vector<HWIdentifier>::const_iterator calib_module_begin(void) const;
  std::vector<HWIdentifier>::const_iterator calib_module_end  (void) const;


  /**
   * @brief create calibration channel identifiers from fields 
   */
  HWIdentifier calib_channel_Id(int barrel_ec,
				int pos_neg,
				int feedthrough, 
				int slot,
				int channel ) const;
  /**
   * @brief create calibration channel identifiers from hash identifiers 
   */
  HWIdentifier calib_channel_Id(IdentifierHash calibChannelHash) const;	
  /**
   * @brief create hash calibration channel identifiers from calib channel identifiers
   */
  IdentifierHash calib_channel_Hash(HWIdentifier calibChannelId) const;
  /**
   * @brief Return an iterator pointing to a collection of calibration channel identifiers
   */
  std::vector<HWIdentifier>::const_iterator calib_channel_begin(void) const;
  std::vector<HWIdentifier>::const_iterator calib_channel_end  (void) const;


  /**
   * @brief Define a calibration module identifier from a channel identifier
   */
  HWIdentifier calib_module_Id(const HWIdentifier ChannelId) const ;
  /**
   * @brief Define a calibration module identifier from a feedthrough identifier
   */
  HWIdentifier calib_module_Id(const HWIdentifier FeedthroughId, int slot) const ;
  /**
   * @brief Define a calibration channel identifier from a feedthrough identifier
   */
  HWIdentifier calib_channel_Id(const HWIdentifier FeedthroughId, int slot, int channel ) const ;
  /**
   * @brief Define a calibration channel identifier from a feb identifier
   */
  HWIdentifier calib_channel_Id(const HWIdentifier febId, int ChannelId) const ;

  /**
   *  @brief Define calibration module hash tables max size 
   */
  size_type  calibModuleHashMax (void) const;

  /**
   *  @brief Define calibration channel hash tables max size 
   */
  size_type  calibChannelHashMax (void) const;


  /* Test beam disctionaries */
  bool  is_H8                    (const HWIdentifier channelId) const;
  bool  is_H8crack               (const HWIdentifier channelId) const;
  bool  is_H8low                 (const HWIdentifier channelId) const;
  bool  is_H8high                (const HWIdentifier channelId) const;
  bool  is_H6                    (const HWIdentifier channelId) const;
  bool  is_H6FT                  (const HWIdentifier feedthroughId) const;
  bool  is_H6fcal                (const HWIdentifier id) const;
  bool  is_H6hec                 (const HWIdentifier id) const;
  bool  is_H6emec                (const HWIdentifier id) const;
  bool  is_H8onlineOnly          (const HWIdentifier id) const;

  /*
   * Removed in AUG-2008 / AL
   bool  isEMB_BACK               (const HWIdentifier id) const;
   bool  isEMB_MIDDLE             (const HWIdentifier id) const;
   bool  isEMB_FRONT              (const HWIdentifier id) const;
  */


private:    
/*==== */
     
  enum {NOT_VALID_HASH = 256000};

  IdentifierHash feb_Hash_binary_search(HWIdentifier channelId) const;

  IdentifierHash channel_Hash_binary_search(HWIdentifier channelId) const;

  // Test of Em EC special / standard
  bool  isLArCalibOnline(const HWIdentifier id) const;

  // Check methods
  int  initLevelsFromDict(const std::string& group_name) ;
  int  init_hashes(void) ;
  int  init_H8Hashes(void) ;
  int  init_H6Hashes(void) ;
  int  init_calib_hashes(void) ;

  /* Calib */
  MultiRange                    m_full_calib_laronline_range;
  MultiRange                    m_full_calib_module_range;
  size_type                     m_calibModuleHashMax;
  size_type                     m_calibChannelHashMax;
  std::vector<HWIdentifier>     m_calib_module_vec;
  std::vector<HWIdentifier>     m_calib_channel_vec;

};

//static const int s_lar_online_field_value = 4 ;
//static const int s_lar_onlineCalib_field_value = 5 ;


//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( LArOnlineID , 158698068 , 1 )




//=====================
// Inline Definitions
//=====================

							   


/* CALIB inline Identifiers */
/*==========================*/

inline HWIdentifier LArOnlineID::calib_module_Id(int barrel_ec, int pos_neg, 
						 int feedthrough, int slot ) const 
/*=============================================================== */
{
  HWIdentifier result(0);  
  /*Pack fields independently */
  m_lar_impl.pack        (lar_field_value(), result);
  m_laronline_impl.pack  (5                , result);
  m_bec_impl.pack        (barrel_ec        , result);
  m_side_impl.pack       (pos_neg          , result);
  m_feedthrough_impl.pack(feedthrough      , result);
  m_slot_impl.pack       (slot             , result); 
  return result;
}

inline HWIdentifier LArOnlineID::calib_module_Id(const HWIdentifier feedthroughId , int slot) const
/*==================================================================================== */
{
  HWIdentifier result(feedthroughId);
  /* Pack fields independently */
  m_slot_impl.reset           (result);
  m_slot_impl.pack            (slot  , result);
  return(result);
}

inline HWIdentifier LArOnlineID::calib_module_Id(const HWIdentifier calibChannelId ) const
/*======================================================================= */
{
  HWIdentifier result(calibChannelId);
  m_channel_in_slot_impl.reset(result);
  return(result);
}

inline HWIdentifier LArOnlineID::calib_module_Id(IdentifierHash calibModuleHashId) const
/*=========================================================================== */
{
  return(m_calib_module_vec[calibModuleHashId]);
}

inline IdentifierHash LArOnlineID::calib_module_Hash (HWIdentifier calibModuleId) const
/*=============================================================================== */
{
    std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_calib_module_vec.begin(),m_calib_module_vec.end(),calibModuleId);
    if ( it != m_calib_module_vec.end() ){
	return (it - m_calib_module_vec.begin());
    }
    return (0);
}

inline HWIdentifier LArOnlineID::calib_channel_Id( int barrel_ec, int pos_neg, int feedthrough, 
						   int slot,      int channel ) const 
/*============================================================================== */
{  
  HWIdentifier result(0);
  /* Pack fields independently */
  m_lar_impl.pack            (lar_field_value()    , result);
  m_laronline_impl.pack      (5                    , result);
  m_bec_impl.pack            (barrel_ec            , result);
  m_side_impl.pack           (pos_neg              , result);
  m_feedthrough_impl.pack    (feedthrough          , result);
  m_slot_impl.pack           (slot                 , result);
  m_channel_in_slot_impl.pack(channel              , result);

  return result;
}

inline HWIdentifier LArOnlineID::calib_channel_Id(const HWIdentifier feedthroughId,int slot,int channel) const 
/*==================================================================================================== */
{  
  HWIdentifier result(feedthroughId);
  /* Pack fields independently */
  m_slot_impl.reset           (result);
  m_channel_in_slot_impl.reset(result);
  m_slot_impl.pack            (slot  , result);
  m_channel_in_slot_impl.pack (channel, result);

  return result;
}

inline HWIdentifier LArOnlineID::calib_channel_Id(const HWIdentifier febId, int channel) const 
/*======================================================================================= */
{  
  HWIdentifier result(febId);
  /* Pack fields independently */
  m_channel_in_slot_impl.reset(result);
  m_channel_in_slot_impl.pack (channel, result);

  return result;
}
inline HWIdentifier LArOnlineID::calib_channel_Id(IdentifierHash channelHashId) const
/*===================================================================*/
{
    return(m_calib_channel_vec[channelHashId]);
}

inline IdentifierHash LArOnlineID::calib_channel_Hash (HWIdentifier channelId) const
/*=========================================================================*/
{
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_calib_channel_vec.begin(),m_calib_channel_vec.end(),channelId);
  if ( it != m_calib_channel_vec.end() ){
    return (it - m_calib_channel_vec.begin());
  }
  return(0) ;
}


inline IdentifierHash LArOnlineID::channel_Hash_binary_search (HWIdentifier channelId) const
/*=========================================================================*/
{
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_channel_vec.begin(),m_channel_vec.end(),channelId);
  if ( it != m_channel_vec.end() ){
    return (it - m_channel_vec.begin());
  }
  return(0) ;
}



/* BOOLEAN */


/* TEST BEAM dictionaries */

inline bool LArOnlineID::is_H8( const HWIdentifier Id ) const
/*=======================================================================*/
{
  return( 
	 m_bec_impl.unpack(Id)== 0 && m_side_impl.unpack(Id)== 1 && 
	 m_feedthrough_impl.unpack(Id) < 2 
	 );
}
inline bool LArOnlineID::is_H8onlineOnly( const HWIdentifier Id ) const
/*=======================================================================*/
{
  return( 
	 m_bec_impl.unpack(Id)== 0 && m_side_impl.unpack(Id)==1 && 
	 ( m_feedthrough_impl.unpack(Id)==0 || m_feedthrough_impl.unpack(Id) == 1 )
	 && m_slot_impl.unpack(Id) == 14 && m_channel_in_slot_impl.unpack(Id) > 63  
	 );
}
inline bool LArOnlineID::is_H8crack( const HWIdentifier Id ) const
/*=======================================================================*/
{
  return( 
	 m_bec_impl.unpack(Id)== 0 && m_side_impl.unpack(Id)==1 && 
	 (
	  ( m_feedthrough_impl.unpack(Id)==0 
	    && ( m_slot_impl.unpack(Id) == 1 || m_slot_impl.unpack(Id)== 5 ||
		 m_slot_impl.unpack(Id) == 6 || m_slot_impl.unpack(Id)== 7 ||
		 m_slot_impl.unpack(Id) == 9 || m_slot_impl.unpack(Id)== 10||
		 m_slot_impl.unpack(Id) == 12|| m_slot_impl.unpack(Id)== 13 )
	    )
	   || 
	  ( m_feedthrough_impl.unpack(Id) ==1 
	    && ( m_slot_impl.unpack(Id) == 1 || m_slot_impl.unpack(Id)== 5 ||
		 m_slot_impl.unpack(Id) == 6 || m_slot_impl.unpack(Id)== 7 ||
		 m_slot_impl.unpack(Id) == 9 || m_slot_impl.unpack(Id)== 10||
		 m_slot_impl.unpack(Id) == 12 || m_slot_impl.unpack(Id)== 13 )
	    )
	  )
	 );
}

inline bool LArOnlineID::is_H8low( const HWIdentifier Id ) const
/*=======================================================================*/
{
  return( 
	 m_bec_impl.unpack(Id)== 0 && m_side_impl.unpack(Id)==1 && 
	 (
	  ( m_feedthrough_impl.unpack(Id)==0 
	    && ( m_slot_impl.unpack(Id) == 1 || m_slot_impl.unpack(Id)== 2 ||
		 m_slot_impl.unpack(Id) == 3 || m_slot_impl.unpack(Id)== 4 ||
		 m_slot_impl.unpack(Id) == 5 || m_slot_impl.unpack(Id)== 9 ||
		 m_slot_impl.unpack(Id) == 11|| m_slot_impl.unpack(Id)== 12 )
	    )
	   || 
	  ( m_feedthrough_impl.unpack(Id) ==1 
	    && ( m_slot_impl.unpack(Id) == 1 || m_slot_impl.unpack(Id)== 2 ||
		 m_slot_impl.unpack(Id) == 3 || m_slot_impl.unpack(Id)== 4 ||
		 m_slot_impl.unpack(Id) == 5 || m_slot_impl.unpack(Id)== 9 ||
		 m_slot_impl.unpack(Id) == 11 || m_slot_impl.unpack(Id)== 12 )
	    )
	  )
	 );
}

inline bool LArOnlineID::is_H8high( const HWIdentifier Id ) const
/*=======================================================================*/
{
  return( 
	 m_bec_impl.unpack(Id)== 0 && m_side_impl.unpack(Id)==1 && 
	 (
	  ( m_feedthrough_impl.unpack(Id) == 0 
	    && ( m_slot_impl.unpack(Id) == 1 || m_slot_impl.unpack(Id)== 6 ||
		 m_slot_impl.unpack(Id) == 7 || m_slot_impl.unpack(Id)== 8 ||
		 m_slot_impl.unpack(Id) == 10 || m_slot_impl.unpack(Id)== 13||
		 m_slot_impl.unpack(Id) == 14 )
	    )
	   || 
	  ( m_feedthrough_impl.unpack(Id) == 1 
	    && ( m_slot_impl.unpack(Id) == 1 || m_slot_impl.unpack(Id)== 6 ||
		 m_slot_impl.unpack(Id) == 7 || m_slot_impl.unpack(Id)== 8 ||
		 m_slot_impl.unpack(Id) == 10 || m_slot_impl.unpack(Id)== 13||
		 m_slot_impl.unpack(Id) == 14 )
	    )
	  )
	 );
}

inline bool LArOnlineID::is_H6( const HWIdentifier Id ) const
/*=======================================================================*/
{
  return( 
	 m_bec_impl.unpack(Id)==1 && m_side_impl.unpack(Id)==1 && 
	 (
	  (m_feedthrough_impl.unpack(Id) == 0 && (m_slot_impl.unpack(Id) == 5 ||
						 m_slot_impl.unpack(Id) == 7 || 
						 m_slot_impl.unpack(Id) == 9 || 
						 m_slot_impl.unpack(Id) == 11 ) )
	  ||
	  (m_feedthrough_impl.unpack(Id) == 1 && (m_slot_impl.unpack(Id) == 3 ||
						  m_slot_impl.unpack(Id) == 4 || 
						  m_slot_impl.unpack(Id) == 5 || 
						  m_slot_impl.unpack(Id) == 6 ) ) 
	  ) 
	 );
}

inline bool LArOnlineID::is_H6FT( const HWIdentifier Id ) const
/*=======================================================================*/
{
  return( 
	 m_bec_impl.unpack(Id)==1 && m_side_impl.unpack(Id)==1 && 
	 (
	  m_feedthrough_impl.unpack(Id) == 0 
	  ||
	  m_feedthrough_impl.unpack(Id) == 1
	  ) 
	 );
}
inline bool LArOnlineID::is_H6fcal( const HWIdentifier Id ) const
/*=======================================================================*/
{
  return( 
	 m_bec_impl.unpack(Id)==1 && m_side_impl.unpack(Id)==1 && 
	 (
	  m_feedthrough_impl.unpack(Id) == 1 && ( m_slot_impl.unpack(Id) == 4 || 
						  m_slot_impl.unpack(Id) == 5 || 
						  m_slot_impl.unpack(Id) == 6 ) 
	  ) 
	 );
}
inline bool LArOnlineID::is_H6hec( const HWIdentifier Id ) const
/*=======================================================================*/
{
  return( 
	 m_bec_impl.unpack(Id)==1 && m_side_impl.unpack(Id)==1 && 
	 (m_feedthrough_impl.unpack(Id) == 0 && (m_slot_impl.unpack(Id) == 5 ||
						 m_slot_impl.unpack(Id) == 7 || 
						 m_slot_impl.unpack(Id) == 9 || 
						 m_slot_impl.unpack(Id) == 11 )
	  ) 
	 );
}
inline bool LArOnlineID::is_H6emec( const HWIdentifier Id ) const
/*=======================================================================*/
{
  return( 
	 m_bec_impl.unpack(Id)==1 && m_side_impl.unpack(Id)==1 && 
	 (
	  m_feedthrough_impl.unpack(Id) == 1 && m_slot_impl.unpack(Id) == 3
	  ) 
	 );
}


/* Calib */
inline LArOnlineID::size_type LArOnlineID::calibModuleHashMax (void) const
/*=======================================================================*/
{
  return m_calibModuleHashMax;
}
inline LArOnlineID::size_type LArOnlineID::calibChannelHashMax (void) const
/*====================================================================*/
{
  return m_calibChannelHashMax;
}





/* Calib */
inline std::vector<HWIdentifier>::const_iterator LArOnlineID::calib_module_begin(void) const
/*====================================================================*/
{
  return(m_calib_module_vec.begin());
}
inline std::vector<HWIdentifier>::const_iterator LArOnlineID::calib_module_end(void) const
/*==================================================================*/
{
  return(m_calib_module_vec.end());
}

inline std::vector<HWIdentifier>::const_iterator LArOnlineID::calib_channel_begin(void) const
/*======================================================================*/
{
  return(m_calib_channel_vec.begin());
}
inline std::vector<HWIdentifier>::const_iterator LArOnlineID::calib_channel_end(void) const
/*======================================================================*/
{
  return(m_calib_channel_vec.end());
}


#endif // LARONLINE_ID_H



