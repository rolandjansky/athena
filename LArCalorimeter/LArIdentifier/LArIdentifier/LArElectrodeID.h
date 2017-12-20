/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARIDENTIFIER_LARELECTRODEID_H
#define LARIDENTIFIER_LARELECTRODEID_H

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
   * @brief Helper for the Liquid Argon Calorimeter cell at the electrode level. This 
   * class provides a description of an electrode.  
   * 
   * @author Arnaud Lucotte
   * @version 00-00-00
   * @since AUG-25-2006
   *  This class provides an interface to generate HV line identifier 
   *  for the Calorimeter detector subsystems.
   *  Definition and the range of values for the elements of the 
   *  identifier:
   *   
   *  Electrodes for LAr detectors  
   *  -----------------------------
   *
   *  FIELD:         Designation:  VALUE:      DET: 
   *  ===============================================
   *  detector
   *                  EMB          0
   *                  EMBPS        1
   *                  EMEC         2
   *                  EMECPS       3
   *                  HEC          4
   *                  FCAL         5
   *                  EMBPUR       6
   *                  ECPUR        7
   *
   *  z-side       :  A,C          0,1        ALL
   * 
   *  module          00 to 15     0 to 15     EMB   
   *                  00 to 31     0 to 31     EMBPS
   *                  00 to 07     0 to 7      EMEC
   *                  00 to 31     0 to 31     EMECPS
   *                  00 to 31     0 to 31     HEC
   *                  00 to 15     0 to 31     FCAL
   *                  00 to 05     0 to 5      EMBPUR
   *                  00 to 10     1 to 10     ECPUR
   *
   *  hv_phi          -1,0         1,0         EMB
   *                  not used     0           EMBPS
   *                  1 to 8       1 to 8      EMEC(inner)
   *                  1 to 4       1 to 4      EMEC(outer)
   *                  not used     0           EMECPS
   *                  not used     0           HEC
   *                  not used     0           FCAL
   *                  not used     0           EMBPUR
   *                  not used     0           ECPUR
   *
   *
   *  hv_eta          
   *                  S1 to S7     1 to 7      EMB
   *                  S1 to S4     1 to 4      EMBPS
   *                  F0,B1 to B6  0 to 6      EMEC(outer) 
   *                  B7,B8        7,8         EMEC(inner)
   *                  S1 to S4     1 to 4      HEC
   *                  S1,S2,S3     1 to 3      FCAL
   *                  not used     0           EMBPUR
   *                  not used     0           ECPUR
   *
   *
   * gap         
   *                  H,L          1,0         EMB
   *                  H,L          1,0         EMBPS
   *                  H,L          1,0         EMEC
   *                  H,L          1,0         EMECPS
   *                  PAD1,EST1    0,1         HEC
   *                  PAD2,EST2    2,3         HEC
   *                  A,B,C,D      0,1,2,3     FCAL
   *                  N,P          0,1         EMBPUR
   *                  N,P          0,1         ECPUR
   *
   *  Electrode planes
   *                  0 to 63      0 to 63     EMB 
   *                  1 to 4       1 to 4      FCAL-S1
   *                  1 to 6       1 to 6      FCAL-S2
   *                  1 to 9       1 to 9      FCAL-S3
   *
   */
class LArElectrodeID : public AtlasDetectorID {
 public:
  

  typedef Identifier::size_type  size_type;
  

  /**
   * @brief Default constructor
   */
  LArElectrodeID(void);
  /**
   * @brief Default destructor
   */
  ~LArElectrodeID(void); 
  
  /**
  * @brief Create an Electrode identifier from fields 
  */
  HWIdentifier ElectrodeId( int det, int side, int mod, int phi, int eta, int gap, int elec ) const;
  /**
  * @brief Create an electrode identifier from an hash identifier
  */
  HWIdentifier ElectrodeId( IdentifierHash electrode_hash )const;
  /**
  * @brief Create an electrode hash identifier from an electrode identifier
  */
  IdentifierHash electrodeHash( HWIdentifier electrode_id ) const;

  /**
   * @brief Return an iterator pointing to a collection of electrode identifiers
   */
  std::vector<HWIdentifier>::const_iterator electrode_begin(void) const;
  std::vector<HWIdentifier>::const_iterator electrode_end(void) const; 

  /**
   * @brief Define  hash tables max size 
   */
  size_type  electrodeHashMax (void) const;

  /**
   * @brief Initialization from the identifier dictionary
   */
  virtual int initialize_from_dictionary (const IdDictMgr& dict_mgr);
  
  /**
   * @brief Return an integer corresponding to the detector
   * of an hardware electrode identifier 
   */
  int detector ( const HWIdentifier id) const;
  /**
   * @brief Return an integer corresponding to the side 
   * of an hardware electrode identifier 
   */
  int zside    ( const HWIdentifier id) const;
  /**
   * @brief Return an integer corresponding to the module
   * of an hardware electrode identifier 
   */
  int module   ( const HWIdentifier id) const;
  /**
   * @brief Return an integer corresponding to the high voltage phi sector
   * of an hardware electrode identifier 
   */
  int hv_phi   (const HWIdentifier id) const;
  /**
   * @brief Return an integer corresponding to the high volatge eta sector
   * of an hardware electrode identifier 
   */
  int hv_eta   (const HWIdentifier id) const;
  /**
   * @brief Return an integer corresponding to the cell gap 
   * of an hardware electrode identifier 
   */
  int gap      ( const HWIdentifier id) const;
  /**
   * @brief Return an integer corresponding to the electrode number
   * of an hardware electrode identifier 
   */
  int electrode( const HWIdentifier id) const;


  /**
   * @brief Return boolean yes if electrode is in EMB
   */
  bool isEMB(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in EMBPS
   */
  bool isEMBPS(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in EMEC
   */
  bool isEMEC(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in EMEC inner wheel
   */
  bool isEMECinWHEEL(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in EMEC outer wheel
   */
  bool isEMECoutWHEEL(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in EMEC PreSampler
   */
  bool isEMECPS(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in HEC
   */
  bool isHEC(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in FCAL
   */
  bool isFCAL( const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in FCAL wheel 1
   */
  bool isFCAL1(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in FCAL wheel 2
   */
  bool isFCAL2(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in FCAL wheel 3
   */
  bool isFCAL3(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in EMB PUR 
   */
  bool isEMBPUR(const HWIdentifier id) const;
  /**
   * @brief Return yes if electrode is in EC PUR
   */
  bool isECPUR(const HWIdentifier id) const;

  /**
   * @brief Define feedthrough Context 
   */
  IdContext    electrodeContext(void) const;
  
  
 private:
  
  
  enum {NOT_VALID_HASH = 256000};
  
  IdentifierHash electrode_hash_binary_search(HWIdentifier elecId) const;
  
  /* Check methods */
  void  electrodeId_checks(int det, int side, int mod, int phi, int eta, int hvgap, int elec )const;
  
  /* create expanded HWIdentifier from HWIdentifier (return == 0 for OK) */
  int  get_expanded_id (const HWIdentifier& id, ExpandedIdentifier& exp_id, 
			const IdContext* context) const;
  
  /* init() hashes */
  int  initLevelsFromDict(void) ;
  int  init_hashes(void);

  /* Member Data index */
  size_type m_larElectrodeRegion_index;
  size_type m_larelectrode_index;
  size_type m_atlas_index;     
  size_type m_configuration_index;     
  size_type m_detector_index;     
  size_type m_zside_index;
  size_type m_module_index;      
  size_type m_hvphi_index;      
  size_type m_hveta_index;      
  size_type m_hvgap_index;      
  size_type m_electrode_index;    
  
  const IdDictDictionary*       m_dict;
  MultiRange                    m_full_larelectrode_range;
  MultiRange                    m_full_electrode_range;
  size_type                     m_electrodeHashMax;
  std::vector<HWIdentifier>     m_electrode_vec;
  
  /** Field Implementation */
  IdDictFieldImplementation     m_atlas_impl;
  IdDictFieldImplementation     m_configuration_impl;
  IdDictFieldImplementation     m_detector_impl;
  IdDictFieldImplementation     m_zside_impl;
  IdDictFieldImplementation  	m_module_impl;
  IdDictFieldImplementation  	m_hvphi_impl;
  IdDictFieldImplementation  	m_hveta_impl;
  IdDictFieldImplementation  	m_hvgap_impl;
  IdDictFieldImplementation  	m_electrode_impl;

};
static const int s_larconfig_value = 1 ;

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( LArElectrodeID , 80757351 , 1 )




//=====================
// Inline Definitions
//=====================
     
     
/* Identifier Definition Inline */
inline HWIdentifier LArElectrodeID::ElectrodeId (int det, int zside, int mod, int phi, 
						 int eta, int gap, int elec ) const 
/*============================================================================== */
{
  HWIdentifier result(0);
  /*Pack fields independently */
  m_atlas_impl.pack       (lar_field_value(), result);
  m_configuration_impl.pack(s_larconfig_value, result);
  m_detector_impl.pack    (det    , result);
  m_zside_impl.pack       (zside  , result);
  m_module_impl.pack      (mod    , result);
  m_hvphi_impl.pack       (phi    , result);
  m_hveta_impl.pack       (eta    , result);
  m_hvgap_impl.pack       (gap    , result);
  m_electrode_impl.pack   (elec   , result);
  return result;
}

inline HWIdentifier LArElectrodeID::ElectrodeId(IdentifierHash electrodeHashId) const
/*===================================================================*/
{
    return(m_electrode_vec[electrodeHashId]);
}


inline IdentifierHash LArElectrodeID::electrodeHash (HWIdentifier hvId) const
/*=============================================================================== */
{
    std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_electrode_vec.begin(),m_electrode_vec.end(),hvId);
    if ( it != m_electrode_vec.end() ){
	return (it - m_electrode_vec.begin());
    }
    return (0);
}

inline LArElectrodeID::size_type LArElectrodeID::electrodeHashMax (void) const
/*====================================================================*/
{
  return m_electrodeHashMax;
}


inline std::vector<HWIdentifier>::const_iterator LArElectrodeID::electrode_begin(void) const
/*====================================================================*/
{
  return(m_electrode_vec.begin());
}

inline std::vector<HWIdentifier>::const_iterator LArElectrodeID::electrode_end(void) const
/*==================================================================*/
{
  return(m_electrode_vec.end());
}


inline int LArElectrodeID::detector(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_detector_impl.unpack(id));
}

inline int LArElectrodeID::zside(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_zside_impl.unpack(id));
}


inline int LArElectrodeID::module(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_module_impl.unpack(id));
}

inline int LArElectrodeID::hv_phi(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_hvphi_impl.unpack(id));
}

inline int LArElectrodeID::hv_eta(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_hveta_impl.unpack(id));
}

inline int LArElectrodeID::gap(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_hvgap_impl.unpack(id));
}

inline int LArElectrodeID::electrode(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_electrode_impl.unpack(id));
}


// ==========================
//   HEC
// ==========================
//
inline bool LArElectrodeID::isHEC( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  return( 
	 det == 4
	 );
}


// ==========================
//   FCAL 
// ==========================
//
inline bool LArElectrodeID::isFCAL( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  //int hveta= hv_eta(id);
  return( 
	 det == 5 
	 );
}
// ==========================
//   FCAL-S1 
// ==========================
//
inline bool LArElectrodeID::isFCAL1( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  int hveta = hv_eta(id);
  return( 
	 det == 5 && hveta == 1 
	 );
}

// ==========================
//   FCAL-S2 
// ==========================
//
inline bool LArElectrodeID::isFCAL2( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  int hveta = hv_eta(id);
  return( 
	 det == 5 && hveta == 2
	 );
}
// ==========================
//   FCAL-S3 
// ==========================
//
inline bool LArElectrodeID::isFCAL3( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  int hveta = hv_eta(id);
  return( 
	 det == 5 && hveta == 3
	 );
}

// ==========================
//   EMBPUR 
// ==========================
//
inline bool LArElectrodeID::isEMBPUR( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  return( 
	 det == 6
	 );
}

// ==========================
//   ECPUR 
// ==========================
//
inline bool LArElectrodeID::isECPUR( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  return( 
	 det == 7
	 );
}


// ==========================
//   EMEC 
// ==========================
//
inline bool LArElectrodeID::isEMEC( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  return( 
	 det == 2
	 );
}
inline bool LArElectrodeID::isEMECinWHEEL( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  int eta = hv_eta(id);
  return( 
	 det == 2 && eta >= 7
	 );
}
inline bool LArElectrodeID::isEMECoutWHEEL( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  int eta = hv_eta(id);
  return( 
	 det == 2 && eta < 7
	 );
}

// ==========================
//   EMECPS 
// ==========================
//
inline bool LArElectrodeID::isEMECPS( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  return( 
	 det == 3
	 );
}

// ==========================
//   EMB
// ==========================
//
inline bool LArElectrodeID::isEMB( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  return( 
	 det == 0
	 );
}


// ==========================
//   EMBPS
// ==========================
//
inline bool LArElectrodeID::isEMBPS( const HWIdentifier id ) const
/*================================================================*/
{
  int det = detector(id);
  return( 
	 det == 1
	 );
}





#endif //LARELECTRODEID_H
