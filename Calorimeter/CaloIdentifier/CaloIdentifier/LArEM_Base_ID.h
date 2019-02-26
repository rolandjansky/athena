// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/LArEM_Base_ID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Factor out code common between LArEM_ID and LArEM_SuperCell_ID.
 */


#ifndef CALOIDENTIFIER_LAREM_BASE_ID_H
#define CALOIDENTIFIER_LAREM_BASE_ID_H


#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "CaloIdentifier/LArNeighbours.h"
#include "CaloIdentifier/CaloIDHelper.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "boost/range/iterator_range.hpp"


class IdDictRegion;
class LArEM_region;



/**
 * This class factors out code common between LArEM_ID and LArEM_SuperCell_ID.
 */
class LArEM_Base_ID : public CaloIDHelper
{
public:
  typedef Identifier::size_type  size_type ;

  /** Constructor.  If SUPERCELL is true, this helper is for supercells. */
  LArEM_Base_ID (const std::string& name, bool supercell);

  ~LArEM_Base_ID(void);

  /** Build a cell identifier from an expanded identifier. */
  Identifier region_id (const ExpandedIdentifier& exp_id) const;
				 
  /** Build a cell identifier from an expanded identifier. */
  Identifier channel_id (const ExpandedIdentifier& exp_id) const;
				 
  /** build a region identifier */
  Identifier  region_id   	(int barrel_ec, int sampling, int region ) const;
  Identifier  region_id   	(int barrel_ec, int sampling, int region, bool checks ) const;

  /** build a cell identifier */
  Identifier  channel_id   	(int barrel_ec, int sampling, int region,
				 int eta,       int phi )  const;
  Identifier  channel_id   	(int barrel_ec, int sampling, int region,
				 int eta,       int phi, bool checks )  const;

  /** allows to know in which region is a channel/cell
      -- valid for both kinds of channels */
  Identifier  region_id   	(const Identifier channelId) const ;

  /** allows to build a cell id starting from a region id (e.g. from descriptors) 
      -- valid for both kinds of channels */
  Identifier  channel_id   	(const Identifier regionId,
				 int eta,       int phi ) const;
  Identifier  channel_id   	(const Identifier regionId,
				 int eta,       int phi, bool checks ) const;

  /** create hash id from channel id  */
  IdentifierHash channel_hash (Identifier channelId) const;
  /** create hash id from channel id  -- method NOT optimised, please use channel_hash() above */
  IdentifierHash channel_hash_binary_search (Identifier channelId) const;

  /** Type for iterators over identifiers. */
  typedef std::vector<Identifier>::const_iterator id_iterator;
  /** Type for range over identifiers. */
  typedef boost::iterator_range<id_iterator> id_range;
  

  /** begin iterator over set of Region Identifiers */
  id_iterator reg_begin    (void) const;
  /** end iterator over set of Region Identifiers */
  id_iterator reg_end      (void) const;
  /** Range over set of Region Identifiers. */
  id_range reg_range () const;

  /** begin iterator over full set of EM Identifiers */
  id_iterator em_begin    (void) const;
  /** end iterator over full set of EM Identifiers */
  id_iterator em_end      (void) const;
  /** Range over full set of EM Identifiers. */
  id_range em_range () const;

  /** provide access to channel id vector, accessed via hash */
  const std::vector<Identifier>& channel_ids() const;

  /** test if the id belongs to the EM barrel */
  bool	is_em_barrel   	(const Identifier id) const;
  
  /** test if the id belongs to the EM Endcap */
  bool	is_em_endcap	(const Identifier id) const;
  
  /** test if the id belongs to the EM Endcap inner wheel */
  bool	is_em_endcap_inner	(const Identifier id) const;
  
  /** test if the id belongs to the EM Endcap outer wheel */
  bool	is_em_endcap_outer	(const Identifier id) const;




  /**
   * return barrel_ec according to : <br>
   * 
   * <pre>
   * element           range              meaning
   * -------           -----              -------
   * 
   * barrel_ec        +/-1             positive/negative barrel - A/C side or P/M half barrel
   * "                +/-2             positive/negative endcap outer wheel - A/C side 
   * "                +/-3             positive/negative endcap inner wheel - A/C side
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
   * sampling         0                both presamplers
   * "                [1,3]            barrel and endcap outer wheel 
   * "                [1,2]            endcap inner wheel
   * 
   * </pre> 
   */
  int         sampling        (const Identifier id)const;  
  

  /**
   * return region according to : <br>
   * 
   * <pre>
   * 
   * element           range              meaning
   * -------           -----              -------
   * 
   * region           0               both presamplers
   * "                [0,1]           barrel sampling 1 and 2 
   * "                0               barrel sampling 3
   * "
   * "                [0,5]           endcap outer wheel sampling 1 (cells)
   * "                0,[2,5]         endcap outer wheel sampling 1 (supercells)
   * "                0               endcap inner wheel sampling 1 (cells)
   * "                [0,1]           endcap inner wheel sampling 1 (supercells)
   * "                [0,1]           endcap outer wheel sampling 2
   * "                0               endcap inner wheel sampling 2 (cells)
   * "                [0,1]           endcap inner wheel sampling 2 (supercells)
   * "                0               endcap outer wheel sampling 3
   * 
   * </pre> 
   */
  int         region          (const Identifier id)const;  
  
  /**
   * return eta according to : <br>
   * 
   * <pre>
   * Cells:
   * element           range              meaning
   * -------           -----              -------
   * 
   * eta for barrel   [0,60]         presampler - 0< eta <1.52 - deta is approximately equal to 0.025
   * "                [0,447]        sampling 1 region 0  0   < eta < 1.4   - deta = 0.025/8 
   * "                [0,2]          sampling 1 region 1  1.4 < eta < 1.475 - deta = 0.025
   * "                [0,55]         sampling 2 region 0  0   < eta < 1.4   - deta = 0.025 
   * "                0              sampling 2 region 1  1.4 < eta < 1.475 - deta = 0.075
   * "                [0,26]         sampling 3 region 0  0   < eta < 1.35  - deta = 0.050 
   * "
   * 
   * eta for endcap   [0,11]      presampler  sampling 0 region 0  1.5   < eta < 1.8   - deta = 0.025
   * "                0           outer wheel sampling 1 region 0  1.375 < eta < 1.425 - deta = 0.05  
   * "                [0,2]       outer wheel sampling 1 region 1  1.425 < eta < 1.5   - deta = 0.025
   * "                [0,95]      outer wheel sampling 1 region 2  1.5   < eta < 1.8   - deta = 0.025/8
   * "                [0,47]      outer wheel sampling 1 region 3  1.8   < eta < 2.0   - deta = 0.025/6
   * "                [0,63]      outer wheel sampling 1 region 4  2.0   < eta < 2.4   - deta = 0.025/4 
   * "                [0,3]       outer wheel sampling 1 region 5  2.4   < eta < 2.5   - deta = 0.025
   * "                [0,6]       inner wheel sampling 1 region 0  2.5   < eta < 3.2   - deta = 0.1
   * "                0           outer wheel sampling 2 region 0  1.375 < eta < 1.425 - deta = 0.05
   * "                [0,42]      outer wheel sampling 2 region 1  1.425 < eta < 2.5   - deta = 0.025
   * "                [0,6]       inner wheel sampling 2 region 0  2.5   < eta < 3.2   - deta = 0.1
   * "                [0,19]      outer wheel sampling 3 region 0  1.5   < eta < 2.5   - deta = 0.05 
   * 
   * Supercells:
   * element           range              meaning
   * -------           -----              -------
   * 
   * eta for barrel   [0,15]         presampler - 0< eta <1.52 - deta is approximately equal to 0.1
   * "                [0,55]         sampling 1 region 0  0   < eta < 1.4   - deta = 0.025
   * "                [0,2]          sampling 1 region 1  1.4 < eta < 1.475 - deta = 0.025
   * "                [0,55]         sampling 2 region 0  0   < eta < 1.4   - deta = 0.025 
   * "                0              sampling 2 region 1  1.4 < eta < 1.475 - deta = 0.075
   * "                [0,13]         sampling 3 region 0  0   < eta < 1.35  - deta = 0.1
   * "
   * 
   * eta for endcap   [0,2]       presampler  sampling 0 region 0  1.5   < eta < 1.8   - deta = 0.1
   * "                0           outer wheel sampling 1 region 0  1.375 < eta < 1.425 - deta = 0.125
   * "                [0,11]      outer wheel sampling 1 region 2  1.5   < eta < 1.8   - deta = 0.025
   * "                [0,7]       outer wheel sampling 1 region 3  1.8   < eta < 2.0   - deta = 0.033
   * "                [0,15]      outer wheel sampling 1 region 4  2.0   < eta < 2.4   - deta = 0.025
   * "                0           outer wheel sampling 1 region 5  2.4   < eta < 2.5   - deta = 0.1
   * "                [0,2]       inner wheel sampling 1 region 0  2.5   < eta < 3.1   - deta = 0.2
   * "                0           inner wheel sampling 1 region 1  3.1   < eta < 3.2   - deta = 0.1
   * "                0           outer wheel sampling 2 region 0  1.375 < eta < 1.425 - deta = 0.05
   * "                [0,42]      outer wheel sampling 2 region 1  1.425 < eta < 2.5   - deta = 0.025
   * "                [0,2]       inner wheel sampling 2 region 0  2.5   < eta < 3.1   - deta = 0.2
   * "                0           inner wheel sampling 2 region 0  3.1   < eta < 3.2   - deta = 0.1
   * "                [0,9]       outer wheel sampling 3 region 0  1.5   < eta < 2.5   - deta = 0.1
   * 
   * </pre> 
   */
  int         eta             (const Identifier id)const; 
  
  /**
   * return phi according to : <br>
   * 
   * <pre>
   * element           range              meaning
   * -------           -----              -------
   * 
   * 
   * Cells:
   * phi for barrel   [0,63]         barrel presampler   - dphi = 0.1
   * "                [0,63]         sampling 1 region 0 - dphi = 0.1
   * "                [0,255]        sampling 1 region 1 - dphi = 0.025
   * "                [0,255]        sampling 2 region 0 - dphi = 0.025
   * "                [0,255]        sampling 2 region 1 - dphi = 0.025
   * "                [0,255]        sampling 3 region 1 - dphi = 0.025
   * 
   * 
   * 
   * phi for endcap   [0,63]      presampler  sampling 0 region 0       - dphi = 0.1
   * "                [0,63]      outer wheel sampling 1 regions [0,5]  - dphi = 0.1 
   * "                [0,63]      inner wheel sampling 1 regions 0      - dphi = 0.1 
   * "                [0,255]     outer wheel sampling 2 regions [0,1]  - dphi = 0.025
   * "                [0,63]      inner wheel sampling 2 region 0       - dphi = 0.1
   * "                [0,255]     outer wheel sampling 3 region 0       - dphi = 0.025
   * 
   * Supercells:
   * phi for barrel   [0,63]         barrel presampler   - dphi = 0.1
   * "                [0,63]         sampling 1 region 0 - dphi = 0.1
   * "                [0,63]         sampling 1 region 1 - dphi = 0.1
   * "                [0,63]         sampling 2 region 0 - dphi = 0.1
   * "                [0,63]         sampling 2 region 1 - dphi = 0.1
   * "                [0,63]         sampling 3 region 1 - dphi = 0.1
   * 
   * 
   * 
   * phi for endcap   [0,63]      presampler  sampling 0 region 0       - dphi = 0.1
   * "                [0,63]      outer wheel sampling 1 regions [0,5]  - dphi = 0.1 
   * "                [0,31]      inner wheel sampling 1 regions 0      - dphi = 0.2
   * "                [0,63]      outer wheel sampling 2 regions [0,1]  - dphi = 0.1
   * "                [0,31]      inner wheel sampling 2 region 0       - dphi = 0.2
   * "                [0,63]      outer wheel sampling 3 region 0       - dphi = 0.1
   * 
   * </pre> 
   */
  int         phi             (const Identifier id)const;
  

  /** Test if the identifier represents a supercell. */
  bool        is_supercell    (const Identifier id)const;


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
  int initialize_base_from_dictionary (const IdDictMgr& dict_mgr,
                                       const std::string& group_name);


  /** True if the + and - sides of the calorimeter are identical (true layout).
      False if they are mirror reflections (used in early MC). */
  bool twoSymSides() const;


  /** access to hashes for neighbours      return == 0 for neighbours found  <br>
      option = prevInPhi, nextInPhi, prevInEta, nextInEta, faces2D, corners2D, all2D, 
               prevInSamp, nextInSamp, upAndDown, prevSubDet, nextSubDet, all3D <br>

               in 'nextInEta', 'nextInSamp', 'nextSubDet', next means 'away from the centre of Atlas' <br>
               in 'prevInEta', 'prevInSamp', 'prevSubDet', prev means 'towards   the centre of Atlas' <br>
	       faces2D means prevInPhi + nextInPhi + prevInEta + nextInEta <br>
	       corners2D means corners in the same sampling, 
	                       not only touching cells but all corner cells forming a square (in case of change in eta granularity ) <br>
	       all2D means faces2D + corners2D <br>
	       upAndDown means prevInSamp + nextInSamp <br>
	       prevSubDet and nextSubDet mean across barrel/EC boundary <br>
	       all3D means all2D + upAndDown + prevSubDet + nextSubDet <br>  */
  int get_neighbours(const IdentifierHash id,
                     const LArNeighbours::neighbourOption& option,
                     std::vector<IdentifierHash>& neighbourList) const;


  using CaloIDHelper::region_id;
  using CaloIDHelper::region_hash;
  using CaloIDHelper::channel_id;
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
  enum {NOT_VALID_HASH = 256000};

  // Check methods
  void  region_id_checks   	(int barrel_ec, int sampling, int region ) const;
  void  channel_id_checks   	(int barrel_ec, int sampling, int region,
				 int eta,       int phi ) const;
  void  channel_id_checks   	(const Identifier regionId,
				 int eta,       int phi ) const;

  /** create expanded Identifier from Identifier (return == 0 for OK) */
  virtual int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;

  int phi_min_init(const Identifier regId) const;

  int         initLevelsFromDict (const std::string& group_name);
  int         init_hashes(void) ;

  int   get_prevInPhi(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
		      int& neighbourIndex, IdentifierHash* neighbList) const;

  int   get_nextInPhi(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
		      int& neighbourIndex, IdentifierHash* neighbList) const;

  int   get_prevInEta(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const float& gPhi, const unsigned int& minHash, 
		      int& neighbourIndex, IdentifierHash* neighbList, unsigned int& nBiggerCell) const;

  int   get_nextInEta(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const float& gPhi, const unsigned int& maxHash, 
		      int& neighbourIndex, IdentifierHash* neighbList, unsigned int& nBiggerCell) const;

  int   get_prevInSamp(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
		       const double& gEta, const float& gPhi, const double& absEta,
		       int& neighbourIndex, IdentifierHash* neighbList) const;
  
  int   get_nextInSamp(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
		       const double& gEta, const float& gPhi, const double& absEta,
		       int& neighbourIndex, IdentifierHash* neighbList) const;

  int   get_prevInSubdet(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
			 const double& gEta, const float& gPhi, const double& absEta,
			 int& neighbourIndex, IdentifierHash* neighbList) const;

  int   get_nextInSubdet(const LArEM_region* emRegion, const unsigned int& index, const short int& nPhi, const unsigned int& minHash, 
			 const double& gEta, const float& gPhi, const double& absEta,
			 int& neighbourIndex, IdentifierHash* neighbList) const;



  int         init_neighbors(void) ;


private:
  unsigned                      m_slar;

  bool                          m_two_sym_sides;

  MultiRange                    m_full_region_range;
  MultiRange                    m_full_em_range;

  size_type                     m_em_region_index;
  size_type                     m_LAR_INDEX;
  size_type                     m_EM_INDEX;
  size_type                     m_BEC_INDEX;
  size_type                     m_SAMPLING_INDEX;
  size_type                     m_REGION_INDEX;
  size_type                     m_ETA_INDEX;
  size_type                     m_PHI_INDEX;
  size_type                     m_SLAR_INDEX;

  IdDictFieldImplementation     m_lar_impl;
  IdDictFieldImplementation  	m_em_impl;
  IdDictFieldImplementation  	m_bec_impl;
  IdDictFieldImplementation  	m_sampling_impl;
  IdDictFieldImplementation  	m_region_impl;
  IdDictFieldImplementation  	m_eta_impl;
  IdDictFieldImplementation  	m_phi_impl;

  IdDictFieldImplementation  	m_slar_impl;

  std::vector<int>              m_vecOfPhiMin;

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

  IdDictFieldImplementation  	m_bec_reg_impl;

  std::vector<LArEM_region*>    m_vecOfRegions;
  std::vector<unsigned>         m_cells;          // stores starting hash number of each region, regions are ordered by hash so this array is ordered as well

  LArEM_Base_ID (const LArEM_Base_ID&);
  LArEM_Base_ID& operator= (const LArEM_Base_ID&);
};


#include "CaloIdentifier/LArEM_Base_ID.icc"


#endif // not CALOIDENTIFIER_LAREM_BASE_ID_H
