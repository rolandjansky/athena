// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/HGTD_ID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief HGTD code derived from LArEM_ID
 */


#ifndef CALOIDENTIFIER_HGTD_ID_H
#define CALOIDENTIFIER_HGTD_ID_H


#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "CaloIdentifier/LArNeighbours.h"
#include "CaloIdentifier/CaloIDHelper.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "boost/range/iterator_range.hpp"


class IdDictRegion;
//class LArEM_region;



/**

 */
class HGTD_ID : public CaloIDHelper
{
public:
  typedef Identifier::size_type  size_type ;

  // Constructor
  HGTD_ID (void);
  ~HGTD_ID(void);

  /** Build a region identifier from an expanded identifier. */
  Identifier region_id (const Identifier channelId) const;

  /** Build a region identifier from an expanded identifier. */
  Identifier region_id (const ExpandedIdentifier& exp_id) const;

  /** build a region identifier for a connected channel  */
  Identifier  region_id   	(int barrel_ec, int sampling, int granularity ) const;
				 
  /** Build a cell identifier from an expanded identifier. */
  Identifier channel_id (const ExpandedIdentifier& exp_id) const;
				 
  /** build a cell identifier for a connected channel  */
  Identifier  channel_id   	(int barrel_ec, int sampling, int granularity, int x, int y )  const;
  /** Type for iterators over identifiers. */
  typedef std::vector<Identifier>::const_iterator id_iterator;
  /** Type for range over identifiers. */
  typedef boost::iterator_range<id_iterator> id_range;
  /** to disentangle between connected and disconnected channels 
      this method is CPU cheap */
  bool is_connected (const Identifier channelId) const;

  /** begin iterator over full set of HGTD Identifiers */
  id_iterator hgtd_begin    (void) const;
  /** end iterator over full set of HGTD Identifiers */
  id_iterator hgtd_end      (void) const;
  /** Range over full set of HGTD Identifiers. */
  id_range hgtd_range () const;

  /** test if the id belongs to the EM barrel -- valid for both connected and disconnected channels */
  bool	is_hgtd   	(const Identifier id) const;
  
  /**
   * return barrel_ec according to : <br>
   * 
   * <pre>
   * element           range              meaning
   * -------           -----              -------
   * 
   * barrel_ec        +/-1             positive/negative endcap A/C side 
   * 
   * </pre> 
   */
  int         barrel_ec       (const Identifier id)const;  

  /**
   * return sampling according to : <br>
   * 
   * element           range              meaning
   * -------           -----              -------
   * 
   * sampling         [0,3]                4 layers of HGTD in z direction
   * 
   * </pre> 
   */
  int         sampling        (const Identifier id)const;  
  

  /**
   * return granularity according to : <br>
   * 
   * <pre>
   * 
   * element           range              meaning
   * -------           -----              -------
   * 
   * granularity         0               coarse (Evgen to Hits, Hits to Digit)
   * "                   1               fine (Digit to RDO)
   * 
   * </pre> 
   */
  int         granularity          (const Identifier id)const;  
  
  /**
   * return eta according to : <br>
   * 
   * <pre>
   * Cells:
   * element           range              meaning
   * -------           -----              -------
   * 
   * x_index     [-12000,12000]            index in x (can be negative)
   *  -99999 if disconnected channel
   * </pre> 
   */
  int         x_index             (const Identifier id)const; 
  
  /**
   * return phi according to : <br>
   * 
   * <pre>
   * element           range              meaning
   * -------           -----              -------
   * y_index       [-12000,12000]       index in y (can be negative)
   *   -99999 if disconnected channel
   * </pre> 
   */
  int         y_index             (const Identifier id)const;
  
  /** initialization from the identifier dictionary*/
  int initialize_from_dictionary (const IdDictMgr& dict_mgr);

  virtual int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id) const;
  using CaloIDHelper::region_id;
  using CaloIDHelper::region_hash;
  using CaloIDHelper::channel_id;

private:
  enum {NOT_VALID_HASH = 256000};

  virtual int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext*) const;
  // Check methods
  void  channel_id_checks   	(int barrel_ec, int sampling, int granularity, int x,       int y ) const throw(LArID_Exception);

  int         initLevelsFromDict ();
  int         init_hashes(void);

private:
  bool                          m_two_sym_sides;

  MultiRange                    m_full_region_range;
  MultiRange                    m_full_hgtd_range;

  size_type                     m_HGTD_REGION_INDEX;
  size_type                     m_LAR_INDEX;
  size_type                     m_HGTD_INDEX;
  size_type                     m_BEC_INDEX;
  size_type                     m_SAMPLING_INDEX;
  size_type                     m_GRANULARITY_INDEX;
  size_type                     m_X_INDEX_INDEX;
  size_type                     m_Y_INDEX_INDEX;

  IdDictFieldImplementation     m_lar_impl;
  IdDictFieldImplementation  	m_hgtd_impl;
  IdDictFieldImplementation  	m_bec_impl;
  IdDictFieldImplementation  	m_sampling_impl;
  IdDictFieldImplementation  	m_granularity_impl;
  IdDictFieldImplementation  	m_x_index_impl;
  IdDictFieldImplementation  	m_y_index_impl;


  /**
     @brief small class holding the starting hash value, the min eta and the number of phi bins of each region 

     used to CPU-optimize the conversion from an identifier to a hash index
   */
/*    class HashCalc
    {
    public:
	HashCalc() :
	    m_hash(0),
	    m_etamin(0),
	    m_phimin(0),
	    m_nphi(0) {}
	IdentifierHash m_hash;
	size_type      m_etamin;
	size_type      m_phimin;
	size_type      m_nphi;
    };
  std::vector<HashCalc>         m_hash_calcs; */

  HGTD_ID (const HGTD_ID&);
  HGTD_ID& operator= (const HGTD_ID&);
};

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( HGTD_ID , 79264207 , 1 )

#include "CaloIdentifier/HGTD_ID.icc"


#endif // not CALOIDENTIFIER_LAREM_BASE_ID_H
