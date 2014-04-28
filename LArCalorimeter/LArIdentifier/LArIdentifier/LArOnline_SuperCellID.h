/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARONLINE_SUPERCELLID_H
#define LARONLINE_SUPERCELLID_H

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
   *               1                     = for all crates for now
   * channel: 
   *              0 to 511              
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
class LArOnline_SuperCellID : public LArOnlineID_Base
{
 public:        

  typedef Identifier::size_type  size_type;
  
  /** 
   * @brief Default constructor
   */
  LArOnline_SuperCellID(void);    
  /** 
   * @brief Default destructor
   */
  ~LArOnline_SuperCellID(void);
  

  /**
   * @brief Define feedthrough hash tables max size 
   */
  //size_type  feedthroughHashMax (void) const;
  /**
   *  @brief define feb hash tables max size 
   */
  //size_type  febHashMax (void) const;
  /**
   *  @brief Define channel hash tables max size 
   */
  //size_type  channelHashMax (void) const;

  int  initialize_from_dictionary (const IdDictMgr& dict_mgr);

private:    
/*==== */
     
  enum {NOT_VALID_HASH = 256000};

  IdentifierHash feb_Hash_binary_search(HWIdentifier channelId) const;

  IdentifierHash channel_Hash_binary_search(HWIdentifier channelId) const;

  int  initLevelsFromDict(const std::string&) ;
  int  init_hashes(void) ;

};

static const int s_slar_online_field_value = 4 ;
//static const int s_lar_onlineCalib_field_value = 5 ;


//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
// Number must be changed
CLASS_DEF( LArOnline_SuperCellID , 115600394 , 1 )




//=====================
// Inline Definitions
//=====================
							   
inline IdentifierHash LArOnline_SuperCellID::channel_Hash_binary_search (HWIdentifier channelId) const
/*=========================================================================*/
{
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_channel_vec.begin(),m_channel_vec.end(),channelId);
  if ( it != m_channel_vec.end() ){
    return (it - m_channel_vec.begin());
  }
  return(0) ;
}



/* BOOLEAN */

#endif // LARONLINE_ID_H



