// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/JGTowerBase_ID.h
 * @author scott snyder <whopkins@uoregon.edu>
 * @date Oct, 2014
 * @brief Factor out code common between JTower_ID and GTower_ID.
 */
#ifndef JGTOWERBASE_ID_H
#define JGTOWERBASE_ID_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "IdDict/IdDictFieldImplementation.h"

#include "CaloIdentifier/CaloID_Exception.h"

#include "boost/range/iterator_range.hpp"
#include <vector>
#include <algorithm>

class IdentifierHash;
class IdDictRegion;

class JGTowerBase_ID : public AtlasDetectorID
{
public:        
	enum { NOT_VALID=999999 };

	typedef Identifier::size_type  size_type ;

	JGTowerBase_ID(void);    

	virtual ~JGTowerBase_ID();

	/** build a tower identifier */
	Identifier  tower_id   	(int pos_neg, int sampling, int region,
                                 int eta,       int phi ) const ;
	Identifier  tower_id   	(int pos_neg, int sampling, int region,
                                 int eta,       int phi, bool checks ) const ;
				 
	/** build a tower identifier */
	Identifier  tower_id   	(const Identifier regionId,
                                 int eta,       int phi )  const ;
	Identifier  tower_id   	(const Identifier regionId,
                                 int eta,       int phi, bool checks )  const ;
 
	/** build a region (of towers) identifier */
	Identifier region_id  ( int pos_neg, int sampling, int region ) const;
        Identifier region_id  ( int pos_neg, int sampling, int region, bool checks ) const;

	/** access to IdContext's which define which levels of fields are contained in the id */
	/** id for towers ("reduced" id) */
	IdContext    region_context   (void) const;

	/** access to IdContext's which define which levels of fields are contained in the id */
	/** id for towers ("normal" id) */
	IdContext    tower_context   (void) const;

	/** create compact id from hash id (return == 0 for OK)*/
	virtual int  get_id    (const IdentifierHash& hash_id, Identifier& id, const IdContext* context = 0 ) const;

	/** create hash id from compact id (return == 0 for OK)*/
	virtual int  get_hash  (const Identifier& id, IdentifierHash& hash_id, const IdContext* context = 0 ) const;

	IdentifierHash calo_region_hash(const Identifier id) const;

	/** create region id from hash id*/
	Identifier region_id	(IdentifierHash region_hash_id) const;

	Identifier region_id  ( const Identifier tower_Id ) const;

	/** create tower id from hash id*/
	Identifier tower_id	(IdentifierHash tower_hash_id) const;	

	/** create hash id from tower id*/
	IdentifierHash tower_hash (Identifier towerId) const;
	/** create hash id from tower id  -- method NOT optimised, please use tower_hash() above */
	IdentifierHash tower_hash_binary_search (Identifier towerId) const;

	/** initialization from the identifier dictionary*/
	virtual int  initialize_base_from_dictionary (const IdDictMgr& dict_mgr, const std::string& group_name, const std::string& t_pre);

	/**  tower hash table max size */
	size_type     tower_hash_max (void) const;
  
	/**  region hash table max size */
	size_type     calo_region_hash_max (void) const;


	/** Type for iterators over identifiers. */
	typedef std::vector<Identifier>::const_iterator id_iterator;
	/** Type for range over identifiers. */
	typedef boost::iterator_range<id_iterator> id_range;


	/** begin iterator over regions */
	id_iterator region_begin    () const;
	/** end iterator over regions */
	id_iterator region_end      () const;
	/** range over regions */
	id_range    reg_range    () const;

	/** begin iterator over towers */
	id_iterator tower_begin    () const;
	/** end iterator over towers */
	id_iterator tower_end      () const;
	/** range over towers */
	id_range    tower_range    () const;

	// /** Test wether given tower or layer is part of the Tile Calorimeter
	//     @warning does NOT take as input a REGION identifier   */
	// bool	has_tile	      (const Identifier id) const;
	// /** Test wether given tower or layer is part of the EM barrel
	//     @warning does NOT take as input a REGION identifier   
	//     @warning excluding 'barrel end'! */
	// bool	has_emb	      (const Identifier id) const;
	// /** Test wether given tower or layer is part of the EM barrel END
	//     @warning does NOT take as input a REGION identifier   */
	// bool	has_barrel_end (const Identifier id) const;
	// /** Test wether given tower or layer is part of the EM end-cap
	//     @warning does NOT take as input a REGION identifier   */
	// bool	has_emec	      (const Identifier id) const;
	// /** Test wether given tower or layer is part of the HEC
	//     @warning does NOT take as input a REGION identifier   */
	// bool	has_hec	      (const Identifier id) const;
	// /** Test wether given tower or layer is part of the FCAL
	//     @warning does NOT take as input a REGION identifier   */
	// bool	has_fcal	      (const Identifier id) const;

	/**
	 * return pos_neg according to : <br>
	 * 
	 * <pre>
	 * element           range              meaning
	 * -------           -----              -------
	 * DETZSIDE           +/-1             positive/negative
	 * 
	 * failure returns 0
	 * 
	 * </pre> 
	 */
	int         pos_neg       (const Identifier id)const;
  
	/**
	 * return sampling according to : <br>
	 * 
	 * <pre>
	 * element           range              meaning
	 * -------           -----              -------
	 * LVL1sampling         0                EM
	 * "                    1                Hadronic
	 * 
	 * failure returns 0
	 * 
	 * </pre> 
	 */
	int         sampling        (const Identifier id)const;  
  
	/**
	 * return region according to : <br>
	 * 
	 * <pre>
	 * GTowers
	 * element           range              meaning
	 * -------           -----              -------
	 * region           [0,1]            2 regions of different eta/phi granularity (FCAL and not FCAL)
	 * 
	 * JTowers
	 * element           range              meaning
	 * -------           -----              -------
	 * region           [0,3]            4 regions of different eta/phi granularity
	 * 
	 * failure returns 0
	 * 
	 * </pre> 
	 */
	int         region          (const Identifier id)const;  

	/**
	 * return eta according to : <br>
	 * 
	 * <pre>
	 * GTower
	 * element           range              meaning
	 * -------           -----              -------
	 * eta              [0,15]         region 0(|eta|<3.2), granularity = 0.2
	 * "                [0,2]          region 1(3.2<|eta|<4.9), granularity = 0.6 ***** NEEDS IMPROVEMENT *****
	 * 
	 * JTowers
	 * element           range              meaning
	 * -------           -----              -------
	 * eta              [0,24]         region 0(|eta|<2.5), granularity = 0.1
	 * "                [0,2]          region 1(2.5|eta|<3.1), granularity = 0.1
	 * "                [0]            region 2(3.1|eta|<3.2), granularity = 0.1
	 * "                [0,2]          region 3(3.2|eta|<4.9), granularity = 0.6
	 * 
	 * failure returns 0
	 * 
	 * </pre> 
	 */
	int         eta             (const Identifier id)const; 

	/**
	 * return phi according to : <br>
	 * 
	 * <pre>
	 * GTower
	 * element           range              meaning
	 * -------           -----              -------
	 * phi              [0,31]         region 0(|eta|<3.2), granularity = 0.2
	 * "                [0,15]         region 1(3.2<|eta|<4.9), granularity = 0.4
	 * 
	 * JTower
	 * element           range              meaning
	 * -------           -----              -------
	 * phi              [0,63]         region 0(|eta|<2.5), granularity = 0.1
	 * "                [0,31]         region 1(2.5<|eta|<3.1), granularity = 0.2
	 * "                [0,31]         region 1(3.1<|eta|<3.2), granularity = 0.2
	 * "                [0,15]         region 1(3.2<|eta|<4.9), granularity = 0.4
	 * 
	 * failure returns 0
	 * 
	 * </pre> 
	 */
	int         phi             (const Identifier id)const;

	/** min value of eta index (-999 == failure) 
	    @warning input = REGION ID !! */
	int eta_min(const Identifier regId) const;
	/** max value of eta index (-999 == failure) 
	    @warning input = REGION ID !! */
	int eta_max(const Identifier regId) const;
	/** min value of phi index (-999 == failure) 
	    @warning input = REGION ID !! */
	int phi_max(const Identifier regId) const;

	/**
	 * @brief Return the eta granularity of a region, or @c NOT_VALID.
	 * @param regHash REGION ID code.
	 */
	float etaGranularity(const Identifier regId) const;
   
	/**
	 * @brief Return the phi granularity of a region, or @c NOT_VALID.
	 * @param regHash REGION ID code.
	 */
	float phiGranularity(const Identifier regId) const;
   
	/**
	 * @brief Return the minimum eta of region, or @c NOT_VALID.
	 * @param regHash REGION ID code.
	 */
	float eta0(const Identifier regId) const;
    
	/**
	 * @brief Return the minimum phi of region, or @c NOT_VALID.
	 * @param regHash REGION ID code.
	 */
	float phi0(const Identifier regId) const;
   
	/** access to hashes for neighbours in phi -- towers only (no extended) 
	    return == 0 for neighbours found  */
	int          get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const;
	/** access to hashes for neighbours in phi -- towers only (no extended) 
	    return == 0 for neighbours found  */
	int          get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const;
	/** access to hashes for neighbours in eta -- towers only (no extended) 
	    return == 0 for neighbours found  */
	int          get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const;
	/** access to hashes for neighbours in eta -- towers only (no extended) 
	    return == 0 for neighbours found  */
	int          get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const;

	int fill_vec_of_dict_regions (const std::string& group_name = "");

	const std::vector<const IdDictRegion*>& dictRegions() const;

private:    

     
	enum {NOT_VALID_HASH = 64000};


	/** create expanded Identifier from Identifier (return == 0 for OK) */
	int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;
	void  tower_id_checks   	(int pos_neg, int sampling, int region,
		 int eta,       int phi ) const;
	void  tower_id_checks   	(const Identifier regionId,
		 int eta,       int phi )  const;
	void region_id_checks  ( int pos_neg, int sampling, int region ) const;

	int         initLevelsFromDict(const std::string& t_pre) ;

	int         init_hashes(void) ;

	int         init_neighbors(void) ;


	size_type                     m_jgtower_region_index;
	size_type                     m_CALO_INDEX;
	size_type                     m_DETZSIDE_INDEX;
	size_type                     m_SAMPLING_INDEX;
	size_type                     m_REGION_INDEX;
	size_type                     m_ETA_INDEX;
	size_type                     m_PHI_INDEX;

	const IdDictDictionary*       m_dict;

	MultiRange                    m_full_reg_range;
	MultiRange                    m_full_tower_range;
	size_type                     m_tower_hash_max;
	size_type                     m_calo_region_hash_max;
	std::vector<Identifier>       m_tower_vec;
	std::vector<Identifier>       m_region_vec;
	std::vector<unsigned short>   m_prev_phi_vec;
	std::vector<unsigned short>   m_next_phi_vec;
	std::vector<unsigned short>   m_prev_eta_vec;
	std::vector<unsigned short>   m_next_eta_vec;
  
	/// List of @c IdDictRegion objects.
	std::vector<const IdDictRegion*> m_vecOfDictRegions;

	/**
	   @brief small class holding the starting hash value, the min eta and the number of phi bins of each region 

	   used to CPU-optimize the conversion from an identifier to a hash index
	*/
	class HashCalc
	{
	public:
		HashCalc() :
			m_hash(0),
			m_etamin(0),
			m_nphi(0) {}
		IdentifierHash m_hash;
		size_type      m_etamin;
		size_type      m_nphi;
	};
	std::vector<HashCalc>         m_hash_calcs;



	IdDictFieldImplementation     m_calo_impl;
	IdDictFieldImplementation  	m_jgtower_impl;
	IdDictFieldImplementation  	m_sampling_impl;
	IdDictFieldImplementation  	m_region_impl;
	IdDictFieldImplementation  	m_eta_impl;
	IdDictFieldImplementation  	m_phi_impl;

	IdDictFieldImplementation  	m_pnz_reg_impl;

};

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( JGTowerBase_ID , 131336095, 1 )

inline Identifier JGTowerBase_ID::tower_id   ( int pos_neg, int sampling, int region,
                                               int eta,       int phi, bool checks ) const 
{  
	Identifier result(0);
	// Pack fields independently
	m_calo_impl.pack     (calo_field_value(),    result);
	m_jgtower_impl.pack     (pos_neg,             result);
	m_sampling_impl.pack (sampling,              result);
	m_region_impl.pack   (region,                result);
	m_eta_impl.pack      (eta,                   result);
	m_phi_impl.pack      (phi,                   result);

	// Do checks
	if(checks) {
		tower_id_checks( pos_neg, sampling, region, eta, phi );
	}

	return result;
}

inline Identifier JGTowerBase_ID::tower_id   ( int pos_neg, int sampling, int region,
                                               int eta,       int phi ) const
{
  return tower_id (pos_neg, sampling, region, eta, phi, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier JGTowerBase_ID::tower_id   ( const Identifier regionId,
                                               int eta,  int phi, bool checks ) const 
{
	Identifier result(regionId);

	// Reset the fields and then set the values
	m_eta_impl.reset     (result);
	m_phi_impl.reset     (result);
	m_eta_impl.pack      (eta, result);
	m_phi_impl.pack      (phi, result);

	// Do checks
	if(checks) {
		tower_id_checks( regionId, eta, phi );
	}

	return result;
}

inline Identifier JGTowerBase_ID::tower_id   ( const Identifier regionId,
                                               int eta,  int phi ) const
{
  return tower_id (regionId, eta, phi, do_checks());
}


//----------------------------------------------------------------------------
inline JGTowerBase_ID::size_type   JGTowerBase_ID::tower_hash_max (void) const
{
	return m_tower_hash_max;
}

//----------------------------------------------------------------------------
inline JGTowerBase_ID::size_type   JGTowerBase_ID::calo_region_hash_max (void) const
{
	return m_calo_region_hash_max;
}


//----------------------------------------------------------------------------
inline JGTowerBase_ID::id_iterator JGTowerBase_ID::region_begin    (void) const
{
	return(m_region_vec.begin());
}

//----------------------------------------------------------------------------
inline JGTowerBase_ID::id_iterator JGTowerBase_ID::region_end      (void) const
{
	return(m_region_vec.end());
}

//----------------------------------------------------------------------------
inline JGTowerBase_ID::id_range JGTowerBase_ID::reg_range    () const
{
	return id_range (region_begin(), region_end());
}

//----------------------------------------------------------------------------
inline JGTowerBase_ID::id_iterator JGTowerBase_ID::tower_begin    (void) const
{
	return(m_tower_vec.begin());
}

//----------------------------------------------------------------------------
inline JGTowerBase_ID::id_iterator JGTowerBase_ID::tower_end      (void) const
{
	return(m_tower_vec.end());
}

//----------------------------------------------------------------------------
inline JGTowerBase_ID::id_range JGTowerBase_ID::tower_range    () const
{
	return id_range (tower_begin(), tower_end());
}

//----------------------------------------------------------------------------
inline int JGTowerBase_ID::pos_neg(const Identifier id) const
{
	return (m_jgtower_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int JGTowerBase_ID::sampling(const Identifier id) const
{
	return (m_sampling_impl.unpack(id));
}
//----------------------------------------------------------------------------
inline int JGTowerBase_ID::region(const Identifier id) const
{
	return (m_region_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int JGTowerBase_ID::eta(const Identifier id) const
{
	return (m_eta_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int JGTowerBase_ID::phi(const Identifier id) const
{
	return (m_phi_impl.unpack(id));
}

// //----------------------------------------------------------------------------
// inline bool	JGTowerBase_ID::has_tile	(const Identifier id) const
// {
//   // Must first check if this is a normal tile id
//   if (AtlasDetectorID::is_tile(id)) {
//     return (true);
//   }
//   /* else { */
//   /* 	return (has_jgtower_trig_towers(id) && region(id) == 0 && sampling(id) == 1 && eta(id) < 15); */
//   /* } */
// }

// //----------------------------------------------------------------------------
// inline bool	JGTowerBase_ID::has_emb	(const Identifier id) const
// {
//   return true;//(has_jgtower_trig_towers(id) && region(id) == 0 && sampling(id) == 0 && eta(id) < 14);
// }

// //----------------------------------------------------------------------------
// inline bool	JGTowerBase_ID::has_barrel_end	(const Identifier id) const
// {
//   return true;//(has_jgtower_trig_towers(id) && region(id) == 0 && sampling(id) == 0 && eta(id) == 14);
// }

// //----------------------------------------------------------------------------
// inline bool	JGTowerBase_ID::has_emec	(const Identifier id) const
// {
//   return true;//(has_jgtower_trig_towers(id) && sampling(id) == 0 &&
//   //((region(id) == 0 &&  eta(id) > 14) || region(id) == 1 || region(id) == 2 ));
// }

// //----------------------------------------------------------------------------
// inline bool	JGTowerBase_ID::has_hec	(const Identifier id) const
// {
//   return true;//(has_jgtower_trig_towers(id) && sampling(id) == 1 &&
//   //((region(id) == 0 &&  eta(id) > 14) || region(id) == 1 || region(id) == 2 ));
// }

// //----------------------------------------------------------------------------
// inline bool	JGTowerBase_ID::has_fcal	(const Identifier id) const
// {
//   return true;//(has_jgtower_trig_towers(id) && region(id) == 3 );
// }

//----------------------------------------------------------------------------
inline Identifier JGTowerBase_ID::region_id    (IdentifierHash region_hash_id) const
{
	return(m_region_vec[region_hash_id]);
}
//----------------------------------------------------------------------------
inline Identifier JGTowerBase_ID::region_id   ( const Identifier tower_Id ) const 
{
	Identifier result(tower_Id);
	//  reset eta, phi, layer
	m_eta_impl.reset(result);
	m_phi_impl.reset(result);
	return (result);
}

//----------------------------------------------------------------------------
inline Identifier JGTowerBase_ID::region_id (int pos_neg, int sampling, int region,
                                             bool checks) const 
{
	Identifier result(0);
	// Pack fields independently
	m_calo_impl.pack     (calo_field_value(),    result);
	m_jgtower_impl.pack     (pos_neg,             result);
	m_sampling_impl.pack (sampling,              result);
	m_region_impl.pack   (region,                result);

	// Do checks
	if(checks) {
		region_id_checks( pos_neg, sampling, region);
	}

	return result;
}

inline Identifier JGTowerBase_ID::region_id (int pos_neg, int sampling, int region)const
{
  return region_id (pos_neg, sampling, region, do_checks());
}


//----------------------------------------------------------------------------
inline Identifier JGTowerBase_ID::tower_id    (IdentifierHash tower_hash_id) const
{
	return(m_tower_vec[tower_hash_id]);
}

//----------------------------------------------------------------------------
inline IdentifierHash JGTowerBase_ID::tower_hash (Identifier towerId) const
{
	const HashCalc& hc = m_hash_calcs[m_pnz_reg_impl.unpack(towerId)];
	return (hc.m_hash + (eta(towerId)-hc.m_etamin)*hc.m_nphi + phi(towerId));
}

//----------------------------------------------------------------------------
inline IdentifierHash JGTowerBase_ID::tower_hash_binary_search  (Identifier towerId) const
{
	std::vector<Identifier>::const_iterator it = std::lower_bound(m_tower_vec.begin(),m_tower_vec.end(),towerId);
	if ( it != m_tower_vec.end() ){
		return (it - m_tower_vec.begin());
	}
	return (0);
}

inline
const std::vector<const IdDictRegion*>& JGTowerBase_ID::dictRegions() const
{     return m_vecOfDictRegions;
}

#endif // CALOJGTOWER_ID_H
























