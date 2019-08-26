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



private:    
/*==== */
     
  // Test of Em EC special / standard
  bool  isLArCalibOnline(const HWIdentifier id) const;

  // Check methods
  int  init_hashes(void) ;
  int  init_H8Hashes(void) ;
  int  init_H6Hashes(void) ;

};


//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( LArOnlineID , 158698068 , 1 )

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



#endif // LARONLINE_ID_H



