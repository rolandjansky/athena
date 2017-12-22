/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARIDENTIFIER_LARHVLINEID_H
#define LARIDENTIFIER_LARHVLINEID_H

#include "CLIDSvc/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"
#include "LArIdentifier/LArOnlID_Exception.h"
#include <vector>
#include <iostream>
#include <algorithm>

class IdentifierHash;
class Range;

/**
 * @brief Helper for the Liquid Argon Calorimeter High-Voltage identifiers.
 * This class provides a description of a HighVoltage cell. 
 * @author Arnaud Lucotte
 * @version 00-00-00
 * @since AUG-21-2006
 *
 *  This class provides an interface to generate HV line identifier 
 *  for the Calorimeter detector subsystems.
 *  Definition and the range of values for the elements of the 
 *  identifier:
 *   
 *  HV Line fields:
 *  ---------------
 *
 *  HV Partition :     1,2,3 
 *  HV CAN line  :     1,2 
 *  HV CAN node  :     0 to 320 : note changed on 07/05/2008 to 0 to 325
 *  HV Line #    :     0 to 15  
 *
 * Major change in dictionary on 30-MAY-2008 to account for new maping 
 *  by Francesco
 *
 * ====================================================================
 */
class LArHVLineID : public AtlasDetectorID {
 public:
  
  typedef Identifier::size_type  size_type;
  
  /** only allowed constructor */
  LArHVLineID(void);
  ~LArHVLineID(void); 

  /**
   * @brief Create an HighVoltage line identifier from fields
   */
  HWIdentifier HVLineId( int part, int canline, int cannode, int line ) const;
  /**
   * @brief Create an HighVoltage line identifier from hash identifiers
   */
  HWIdentifier HVLineId( IdentifierHash hvline_hash )const;
  /**
   * @brief Create an HighVoltage line hash identifiers from identifiers
   */
  IdentifierHash hvlineHash( HWIdentifier hvline_id ) const;
  /**
   * @brief Return an iterator pointing to a collection of high voltage line identifier
   */
  std::vector<HWIdentifier>::const_iterator hvline_begin(void) const;
  /**
   * @brief Return an iterator pointing to a collection of high voltage line identifier
   */
  std::vector<HWIdentifier>::const_iterator hvline_end(void) const; 

  /** 
   * @brief Define hash tables max size 
   */
  size_type  hvlineHashMax (void) const;
  /**
   * @brief Initialization from the identifier dictionary
   */
  virtual int initialize_from_dictionary (const IdDictMgr& dict_mgr);
  

  /**
   * @brief Return the partition from an HighVoltage line identifier
   */
  int partition( const HWIdentifier id) const;
  /**
   * @brief Return the CAN line from an HighVoltage line identifier
   */
  int can_line ( const HWIdentifier id) const;
  /**
   * @brief Return the CAN node from an HighVoltage line identifier
   */
  int can_node ( const HWIdentifier id) const;
  /**
   * @brief Return the HV line number from an HighVoltage line identifier
   */
  int hv_line  ( const HWIdentifier id) const;




  /**
   * @brief 
   */
  /*
  bool isEMB(const HWIdentifier id) const;
  bool isEMBA(const HWIdentifier id) const;
  bool isEMBC(const HWIdentifier id) const;
  bool isEMBPS(const HWIdentifier id) const;
  bool isEMEC(const HWIdentifier id) const;
  bool isEMECA(const HWIdentifier id) const;
  bool isEMECC(const HWIdentifier id) const;
  bool isEMECPS(const HWIdentifier id) const;

  bool isHEC(const HWIdentifier id) const;
  bool isHECA(const HWIdentifier id) const;
  bool isHECC(const HWIdentifier id) const;
  bool isFCAL(const HWIdentifier id) const;
  bool isFCALA(const HWIdentifier id) const;
  bool isFCALC(const HWIdentifier id) const;
  bool isEMPUR(const HWIdentifier id) const;
  bool isNOTUSED(const HWIdentifier id) const;
  bool isNOTCONNECTED(const HWIdentifier id) const;
  bool isHVCONNECTED(const HWIdentifier id) const;
  */


  /* Test Beam configurations */
  /* Obsolete / Not to be implemented in the new framework */
  /*
  bool isH8EMB(const HWIdentifier id) const { return false; };
  bool isH8EMBPS(const HWIdentifier id) const { return false; };
  bool isH6EMEC(const HWIdentifier id) const { return false; };
  bool isH6HEC(const HWIdentifier id) const { return false; };
  bool isH6FCAL(const HWIdentifier id) const { return false; };
  bool isHV_H8(const HWIdentifier id) const { return false; };
  bool isHV_H6(const HWIdentifier id) const { return false; };
  */

  /* context for feedthroughs, feb  & channels */
  IdContext    hvlineContext(void) const;
  /* Now Obsolete */
  //IdContext    canlineContext(void) const;




 private:


  enum {NOT_VALID_HASH = 256000};

  IdentifierHash hvline_hash_binary_search(HWIdentifier hvId) const;

  /* Check methods */
  void  hvlineId_checks(int part, int canline, int cannode, int line )const;

  /* create expanded HWIdentifier from HWIdentifier (return == 0 for OK) */
  int  get_expanded_id (const HWIdentifier& id, ExpandedIdentifier& exp_id, 
			const IdContext* context) const;

  /* init() hashes */
  int  initLevelsFromDict(void) ;
  int  init_hashes(void);

  /* Member Data index */
  size_type m_larhvRegion_index;
  size_type m_atlas_index;     
  size_type m_configuration_index;     
  size_type m_partition_index;
  size_type m_canline_index;      
  size_type m_cannode_index;      
  size_type m_hvline_index;    

  
  const IdDictDictionary*       m_dict;
  MultiRange                    m_full_atlas_highvoltage_range;
  MultiRange                    m_full_hvline_range;
  MultiRange                    m_full_canline_range;
  size_type                     m_hvlineHashMax;
  std::vector<HWIdentifier>     m_hvline_vec;

  /** Field Implementation */
  IdDictFieldImplementation     m_atlas_impl;
  IdDictFieldImplementation     m_configuration_impl;
  IdDictFieldImplementation     m_partition_impl;
  IdDictFieldImplementation  	m_canline_impl;
  IdDictFieldImplementation  	m_cannode_impl;
  IdDictFieldImplementation  	m_hvline_impl;

};

static const int s_lar_atlas_value = 1 ;
static const int s_lar_testbeam_value = 2 ;


//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( LArHVLineID , 27863673 , 1 )




//=====================
// Inline Definitions
//=====================
     
     
/* Identifier Definition Inline */
inline HWIdentifier LArHVLineID::HVLineId (int part, int canline, int cannode, int line ) const 
/*============================================================================== */
{
  HWIdentifier result(0);
  /*Pack fields independently */
  m_atlas_impl.pack        (lar_field_value(), result);
  m_configuration_impl.pack(s_lar_atlas_value, result);
  m_partition_impl.pack    (part      , result);
  m_canline_impl.pack      (canline   , result);
  m_cannode_impl.pack      (cannode   , result);
  m_hvline_impl.pack       (line      , result);
  return result;
}

inline HWIdentifier LArHVLineID::HVLineId(IdentifierHash hvlineHashId) const
/*===================================================================*/
{
    return(m_hvline_vec[hvlineHashId]);
}


inline IdentifierHash LArHVLineID::hvlineHash (HWIdentifier hvId) const
/*=============================================================================== */
{
    std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_hvline_vec.begin(),m_hvline_vec.end(),hvId);
    if ( it != m_hvline_vec.end() ){
	return (it - m_hvline_vec.begin());
    }
    return (0);
}

inline LArHVLineID::size_type LArHVLineID::hvlineHashMax (void) const
/*====================================================================*/
{
  return m_hvlineHashMax;
}


inline std::vector<HWIdentifier>::const_iterator LArHVLineID::hvline_begin(void) const
/*====================================================================*/
{
  return(m_hvline_vec.begin());
}

inline std::vector<HWIdentifier>::const_iterator LArHVLineID::hvline_end(void) const
/*==================================================================*/
{
  return(m_hvline_vec.end());
}


inline int LArHVLineID::partition(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_partition_impl.unpack(id));
}

inline int LArHVLineID::can_line(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_canline_impl.unpack(id));
}


inline int LArHVLineID::can_node(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_cannode_impl.unpack(id));
}

inline int LArHVLineID::hv_line(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_hvline_impl.unpack(id));
}



// ==========================
//   HEC
// ==========================
//

/*

inline bool LArHVLineID::isHEC( const HWIdentifier id ) const
{
// Note: left unchanged with new MAY-2008 maping (14/05/2008)
int cannode = can_node(id);
return( (cannode >= 48 && cannode <= 79) ||
(cannode >= 148 && cannode <= 179) 
);
}
inline bool LArHVLineID::isHECA( const HWIdentifier id ) const
{
// Note: left unchanged with new MAY-2008 maping (14/05/2008)
int cannode = can_node(id);
return( 
cannode >= 48 && cannode <= 79
);
}
inline bool LArHVLineID::isHECC( const HWIdentifier id ) const
{
// Note: left unchanged with new MAY-2008 maping (14/05/2008)
int cannode = can_node(id);
return( 
cannode >= 148 && cannode <= 179 
);
}


// ==========================
//   FCAL 
// ==========================
//
inline bool LArHVLineID::isFCAL( const HWIdentifier id ) const
{
// Note: left unchanged with new MAY-2008 maping (14/05/2008)
int cannode = can_node(id);
return( (cannode >= 80 && cannode <= 93) ||
(cannode >= 180 && cannode <= 193) 
);
}
inline bool LArHVLineID::isFCALA( const HWIdentifier id ) const
{
// Note: left unchanged with new MAY-2008 maping (14/05/2008)
int cannode = can_node(id);
return( 
cannode >= 80 && cannode <= 93
);
}
inline bool LArHVLineID::isFCALC( const HWIdentifier id ) const
{
// Note: left unchanged with new MAY-2008 maping (14/05/2008)
int cannode = can_node(id);
return(
cannode >= 180 && cannode <= 193 
);
}

*/

// ==========================
//   EMEC 
// ==========================
//
/*
  inline bool LArHVLineID::isEMEC( const HWIdentifier id ) const
  {
  // Note: change in Tartarelli's new maping (15-MAY-2008)
  // 0-47 and 100-147 left unchanged 
  // added 320-322 HVcannodes to EMEC-A
  // added 324-325 HVcannodes to EMEC-C
  int cannode = can_node(id);
  return( (cannode >= 0 && cannode <= 47) ||
  (cannode >= 100 && cannode <= 147) ||
  (cannode >= 320 && cannode <= 322) ||
  (cannode >= 324 && cannode <= 325) 
  );
  }
  
  inline bool LArHVLineID::isEMECA( const HWIdentifier id ) const
  {
  // Note: change in Tartarelli's new maping (15-MAY-2008)
  // 0-47 unchanged
  // added 320-322 HVcannodes to EMEC-A
  int cannode = can_node(id);
  return( 
  (cannode >= 0 && cannode <= 47) ||
  (cannode >= 320 && cannode <= 322) 
  );
  }
  inline bool LArHVLineID::isEMECC( const HWIdentifier id ) const
  {
  // Note: change in Tartarelli's new maping (15-MAY-2008)
  // HVcannodes 100-147 left unchanged 
  // added 324-325 HVcannodes to EMEC-C
  int cannode = can_node(id);
  return( 
  (cannode >= 100 && cannode <= 147) ||
  (cannode >= 324 && cannode <= 325) 
  );
  }
  
  // ==========================
//   EMECPS 
// ==========================
//
inline bool LArHVLineID::isEMECPS( const HWIdentifier id ) const
{
// Note: left unchanged with new MAY-2008 maping (14/05/2008)
int cannode = can_node(id);
return( (cannode >= 312 && cannode <= 315) ||
(cannode >= 316 && cannode <= 319) 
);
}
*/

// ==========================
//   EMB
// ==========================
//

/*
inline bool LArHVLineID::isEMB( const HWIdentifier id ) const
{
  // Affected by new Tartarelli's maping (15-MAY-2008)
  // Added new HVcannodes 
  // 306-307 : EMBA
  // 308-309 : EMBC
  int cannode = can_node(id);
  return( 
	 (cannode >= 200 && cannode <= 231) ||
	 (cannode >= 232 && cannode <= 263) ||
	 cannode == 296 || cannode == 297 || cannode == 306 || cannode == 307 || 
	 cannode == 299 || cannode == 304 || cannode == 305 || cannode == 308 || cannode == 309 
	 );
}
inline bool LArHVLineID::isEMBA( const HWIdentifier id ) const
{
  // Affected by new Tartarelli's maping (15-MAY-2008)
  // Added new HVcannodes 
  // 306-307 : EMBA
   int cannode = can_node(id);
  return( 
	 (cannode >= 200 && cannode <= 231) || 
	 cannode == 296 || cannode == 297  ||
	 cannode == 306 || cannode == 307 
	 );
}

inline bool LArHVLineID::isEMBC( const HWIdentifier id ) const
{
  // Affected by new Tartarelli's maping (15-MAY-2008)
  // Added new HVcannodes 
  // 307-308 : EMBC
  int cannode = can_node(id);
  return( 
	 (cannode >= 232 && cannode <= 263) || 
	 cannode == 299 || cannode == 304 || cannode == 305 ||
	 cannode == 308 || cannode == 309
	 );
}


*/

// ==========================
//   EMBPS
// ==========================
//
/*
inline bool LArHVLineID::isEMBPS( const HWIdentifier id ) const
{
  // Unaffected by new Tartarelli's maping (15-MAY-2008)
  int cannode = can_node(id);
  return( (cannode >= 264 && cannode <= 279) ||
	  (cannode >= 280 && cannode <= 295) 
	  );
}
*/

// ==========================
//   EMPUR
// ==========================
//
/*
  inline bool LArHVLineID::isEMPUR( const HWIdentifier id ) const
  {
  // Changed EMBPUR and EMECPUR into the same block
  int cannode = can_node(id);
  return( cannode >= 300 && cannode <= 303 );
  }
*/
 /*
   inline bool LArHVLineID::isNOTCONNECTED( const HWIdentifier id ) const
   {
   // Added to offer other ids for new hv lines
   // Note : put by hand after having extended the fields of HV line 
   //        which allows for further change of the HV numbering
   //        (typically, new cannodes can be used)
   int cannode = can_node(id);
   int hvline  = hv_line(id);
   return( 
   // lines not used in the present maping (but present in the disctionary)
   (cannode == 323 || cannode == 326 || cannode == 327) ||
   (cannode == 310 || cannode == 311) 
   ||
   // disconnected hv lines 
   ((cannode >= 320 && cannode <= 327 ) && (hvline >= 8 && hvline <= 15))
   );
   }
 */

 /*
 //=============================================================
 inline bool LArHVLineID::isNOTUSED( const HWIdentifier id ) const
 {
 // Added to offer other ids for new hv lines
 // Note : put by hand after having extended the fields of HV line 
 //        which allows for further change of the HV numbering
 //        (typically, new cannodes can be used)
 int cannode = can_node(id);
 return( 
 (cannode >=  94 && cannode <=  99) ||
 (cannode >= 194 && cannode <= 199) 
 );
 }

 inline bool LArHVLineID::isHVCONNECTED( const HWIdentifier id ) const
 {
 // Added to offer other ids for new hv lines
 return( 
 !isNOTCONNECTED( id ) && !isNOTUSED( id )
 );
 }
 */



#endif //LARHVLINEID_H
