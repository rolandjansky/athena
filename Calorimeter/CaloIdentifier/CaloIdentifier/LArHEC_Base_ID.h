// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/LArHEC_Base_ID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Factor out code common between LArHEC_ID and LArHEC_SuperCell_ID.
 */


#ifndef CALOIDENTIFIER_LARHEC_BASE_ID_H
#define CALOIDENTIFIER_LARHEC_BASE_ID_H


#include "AthenaKernel/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"

#include "CaloIdentifier/CaloIDHelper.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArNeighbours.h"
#include "boost/range/iterator_range.hpp"

#include <vector>
#include <algorithm>


class Range;
class LArHEC_region;


/**  
 *
 * This class factors out code common between LArEM_ID and LArEM_SuperCell_ID.
 */
class LArHEC_Base_ID : public CaloIDHelper
{
public:        

  /** Constructor.  If SUPERCELL is true, this helper is for supercells. */
  LArHEC_Base_ID (const std::string& name, bool supercell);

  ~LArHEC_Base_ID();

  /** region identifier for a channel from ExpandedIdentifier*/
  Identifier  region_id (const ExpandedIdentifier& exp_id ) const ;

  /** channel identifier for a channel from ExpandedIdentifier*/
  Identifier  channel_id (const ExpandedIdentifier& exp_id ) const ;

  /** build a region identifier for a channel  */
  Identifier  region_id   	(int pos_neg, int sampling, int region ) const ;
  Identifier  region_id   	(int pos_neg, int sampling, int region, bool checks) const;

  /** build a cell identifier for a channel  */
  Identifier  channel_id   	(int pos_neg, int sampling, int region,
				 int eta,     int phi ) const;
  Identifier  channel_id   	(int pos_neg, int sampling, int region,
				 int eta,     int phi, bool checks) const;
				 
  /** build a cell identifier for a channel  <br>
      eta counting includes regions     */
  Identifier  channel_id  ( int pos_neg, int sampling,
			    int eta,     int phi ) const;

  /** For a specific sector , eta, phi_sector 
   * <pre>
   * 
   * sector       [0,31]
   * 
   * phi_sector   [0,1]            Outer part, dphi=0.1
   * "            [0]              Inner part, dphi=0.2
   * 
   * </pre>
   */
  Identifier  channel_id  ( int pos_neg, int sampling, int sector, int region, 
			    int eta,     int phi_sector ) const;

  /** allows to know in which region is a channel/cell
      -- valid for both kinds of channels */
  Identifier  region_id   	(const Identifier channelId) const ;

  /** allows to build a channel id starting from a region id (e.g. from descriptors) 
      -- valid for both kinds of channels */
  Identifier  channel_id   	(const Identifier regionId,
				 int eta,       int phi ) const ;
  Identifier  channel_id   	(const Identifier regionId,
				 int eta,       int phi, bool checks) const;

  /** Test if the identifier represents a supercell. */
  bool        is_supercell    (const Identifier id)const;
				 
  /** create hash id from channel id  */
  IdentifierHash channel_hash (Identifier channelId) const;
  /** create hash id from channel id  -- method NOT optimised, please use channel_hash() above */
  IdentifierHash channel_hash_binary_search (Identifier channelId) const;

  /** begin iterator over set of region Identifiers */
  id_iterator reg_begin    (void) const;
  /** end iterator over set of region Identifiers */
  id_iterator reg_end      (void) const;
  /** Range over set of Region Identifiers. */
  id_range reg_range () const;

  /** begin iterator over full set of Hec Identifiers for channels */
  id_iterator hec_begin    (void) const;
  /** end iterator over full set of Hec Identifiers for channels */
  id_iterator hec_end      (void) const;
  /** Range over full set of HEC Identifiers. */
  id_range hec_range () const;

  /** provide access to channel id vector, accessed via hash */
  const std::vector<Identifier>& channel_ids() const;
  /** provide access to region id vector, accessed via hash */
  const std::vector<Identifier>& region_ids() const;


  /** return pos_neg -2 (C side) or 2 (A side)*/
  int         pos_neg       (const Identifier id)const;  
  
  /** return sampling [0,3] (only 0 for supercells) */
  int         sampling        (const Identifier id)const;  

  /** return region [0,1] */
  int         region          (const Identifier id)const;  

  /** return eta [0,9] outer part [0,3] inner part */
  int         eta             (const Identifier id)const; 

  /** return phi[0,63] outer part [0,31] inner part */
  int         phi             (const Identifier id)const;

  /** min value of eta index (-999 == failure) 
      @warning input = REGION ID !! */
  int eta_min(const Identifier regId) const;
  /** max value of eta index (-999 == failure) 
      @warning input = REGION ID !! */
  int eta_max(const Identifier regId) const;
  /** min value of phi index (-999 == failure) 
      @warning input = REGION ID !! */
  int phi_min(const Identifier regId) const;
  /** max value of phi index (-999 == failure) 
      @warning input = REGION ID !! */
  int phi_max(const Identifier regId) const;
  

  /** initialization from the identifier dictionary*/
  int  initialize_base_from_dictionary (const IdDictMgr& dict_mgr,
                                        const std::string& group_name);

  /** True if the + and - sides of the calorimeter are identical (true layout).
      False if they are mirror reflections (used in early MC). */
  bool twoSymSides() const;


  /** access to hashes for neighbours      return == 0 for neighbours found  <br>
      option = prevInPhi, nextInPhi, prevInEta, nextInEta, faces 2D, corners2D, all2D, prevInSamp, nextInSamp, upAndDown, all3D <br>

               in 'nextInEta' and 'nextInSamp', next means 'away from the centre of Atlas' <br>
               in 'prevInEta' and 'prevInSamp', prev means 'towards   the centre of Atlas' <br>
	       faces2D means prevInPhi + nextInPhi + prevInEta + nextInEta <br>
	       corners2D means corners in the same sampling, 
	                       not only touching cells but all corner cells forming a square (in case of change in eta granularity ) <br>
	       all2D means faces2D + corners2D <br>
	       upAndDown means prevInSamp + nextInSamp <br>
	       all3D means all2D + upAndDown <br> */
  int    get_neighbours(const IdentifierHash id,
                        const LArNeighbours::neighbourOption& option,
                        std::vector<IdentifierHash>& neighbourList) const;


  using CaloIDHelper::channel_id;
  using CaloIDHelper::region_id;
  // Inherited from CaloIDHelper:
  // 
  //Identifier channel_id (IdentifierHash hashId) const;
  //Identifier region_id (IdentifierHash hashId) const;
  //IdentifierHash region_hash (Identifier regionId) const;
  //size_type channel_hash_max() const;
  //size_type region_hash_max() const;
  //IdContext channel_context() const;
  //IdContext region_context() const;
  //const std::vector<const IdDictRegion*>& dictRegions() const;
  //float etaGranularity(const IdentifierHash regHash) const;
  //float phiGranularity(const IdentifierHash regHash) const;
  //float eta0(const IdentifierHash regHash) const;
  //float phi0(const IdentifierHash regHash) const;
  //virtual int get_id (const IdentifierHash& hash_id,
  //                    Identifier& id,
  //                    const IdContext* context = 0 ) const;
  //virtual int  get_hash  (const Identifier& id,
  //                        IdentifierHash& hash_id,
  //                        const IdContext* context = 0 ) const;


private:    

  enum {NOT_VALID_HASH = 64000};

  int phi_min_init(const Identifier regId) const;

  // Check methods

  void  region_id_checks   	(int pos_neg, int sampling, int region ) const;
  void  channel_id_checks   	(int pos_neg, int sampling, int region,
				 int eta,       int phi ) const;
  void  channel_id_checks   	(const Identifier regionId,
				 int eta,       int phi ) const;

  /** create expanded Identifier from Identifier (return == 0 for OK) */
  virtual int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;

  int         initLevelsFromDict (const std::string& group_name) ;

  int         init_hashes(void) ;

  int   get_prevInPhi(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
		      int& neighbourIndex, IdentifierHash* neighbList) const;
  
  int   get_nextInPhi(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
		      int& neighbourIndex, IdentifierHash* neighbList) const;

  int   get_prevInEta(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const float& gPhi, const unsigned int& minHash, 
		      int& neighbourIndex, IdentifierHash* neighbList, unsigned int& nBiggerCell) const;

  int   get_nextInEta(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const float& gPhi, const unsigned int& maxHash, 
		      int& neighbourIndex, IdentifierHash* neighbList, unsigned int& nBiggerCell) const;

  int   get_prevInSamp(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
		       const double& absEta,
		       int& neighbourIndex, IdentifierHash* neighbList) const;

  int   get_nextInSamp(const LArHEC_region* hecRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
		       const double& absEta,
		       int& neighbourIndex, IdentifierHash* neighbList) const;


  int         init_neighbors(void) ;


  unsigned                      m_slar;

  size_type                     m_hec_region_index;
  size_type                     m_LAR_INDEX;
  size_type                     m_HEC_INDEX;
  size_type                     m_POSNEG_INDEX;
  size_type                     m_SAMPLING_INDEX;
  size_type                     m_REGION_INDEX;
  size_type                     m_ETA_INDEX;
  size_type                     m_PHI_INDEX;
  size_type                     m_SLAR_INDEX;

  MultiRange                    m_full_channel_range;
  MultiRange                    m_full_region_range;

  bool                          m_two_sym_sides;

  std::vector<short int>        m_vecOfPhiMin;

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
	    m_phimin(0),
	    m_nphi(0) {}
	IdentifierHash m_hash;
	size_type      m_etamin;
	size_type      m_phimin;
	size_type      m_nphi;
    };
  std::vector<HashCalc>         m_hash_calcs;


  IdDictFieldImplementation     m_lar_impl;
  IdDictFieldImplementation  	m_hec_impl;
  IdDictFieldImplementation  	m_pn_impl;
  IdDictFieldImplementation  	m_sampling_impl;
  IdDictFieldImplementation  	m_region_impl;
  IdDictFieldImplementation  	m_eta_impl;
  IdDictFieldImplementation  	m_phi_impl;
  IdDictFieldImplementation  	m_slar_impl;

  IdDictFieldImplementation  	m_pn_reg_impl;

  std::vector<LArHEC_region*>   m_vecOfRegions;
  std::vector<short int>        m_vecOfCellInfo;
};


#include "CaloIdentifier/LArHEC_Base_ID.icc"

#endif // LARHEC_BASE_ID_H
