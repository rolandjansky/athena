// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/CaloCell_Base_ID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2012
 * @brief Helper base class for offline cell identifiers
 */


#ifndef CALOIDENTIFIER_CALOCELL_BASE_ID_H
#define CALOIDENTIFIER_CALOCELL_BASE_ID_H


#include "AthenaKernel/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/LArNeighbours.h"
#include "CaloGeoHelpers/CaloSampling.h"


class CaloNeighbours;


/**
  * @class CaloCell_Base_ID
  * @brief Helper base class for offline cell identifiers
  *
  * This class provides an interface to generate offline identifiers
  * for the full calorimeter (LAr + Tile). <br>
  * It factors out code common to both
  * @c CaloCell_ID and @c CaloCell_SuperCell_ID.
  * 
  */  
class CaloCell_Base_ID : public AtlasDetectorID, public CaloSampling
{
public:
  typedef Identifier::size_type  size_type ;

  /** enumeration of sub calorimeters*/
  enum SUBCALO { LAREM = 0, LARHEC = 1, LARFCAL = 2, TILE = 3, LARMINIFCAL = 4, NSUBCALO = 5, NOT_VALID=CaloIDHelper::NOT_VALID /*999999*/ };

  /** enumeration of samplings (i.e.layers) separately for various sub calorimeters */
  //typedef CaloSampling::CaloSample CaloSample;
  // enum CaloSample {
  //   PreSamplerB=0, EMB1, EMB2, EMB3,              // LAr barrel
  //   PreSamplerE, EME1, EME2, EME3,                // LAr EM endcap
  //   HEC0, HEC1, HEC2, HEC3,                       // Hadronic end cap cal.
  //   TileBar0, TileBar1, TileBar2,                 // Tile barrel
  //   TileGap1, TileGap2, TileGap3,                 // Tile gap (ITC & scint)
  //   TileExt0, TileExt1, TileExt2,                 // Tile extended barrel
  //   FCAL0, FCAL1, FCAL2,                          // Forward EM endcap
  //   MINIFCAL0, MINIFCAL1, MINIFCAL2, MINIFCAL3,   // MiniFCAL 
  //   Unknown
  // };


  /** Constructor */
  CaloCell_Base_ID(const LArEM_Base_ID*    em_id, 
                   const LArHEC_Base_ID*   hec_id,
                   const LArFCAL_Base_ID*  fcal_id, 
                   const LArMiniFCAL_ID*   minifcal_id,
                   const Tile_Base_ID*     tile_id,
		   bool supercell);

  ~CaloCell_Base_ID();


  int GetSubCaloName( const std::string SubCaloName ) const;

  /// Initialization from the identifier dictionary
  virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr) override;

  /** Make a region ID from constituting fields and subCalo index;
      for (Mini)FCAL and Tiles, the last argument is not used  
      for Tiles, 'barec_or_posneg' corresponds to 'section', 'sampling_or_fcalmodule' corresponds to 'side'  */
  Identifier  region_id   	(const int subCalo, 
				 const int barec_or_posneg, 
                                 const int sampling_or_fcalmodule, 
                                 const int region_or_dummy ) const;
  Identifier  region_id   	(const int subCalo, 
				 const int barec_or_posneg, 
                                 const int sampling_or_fcalmodule, 
                                 const int region_or_dummy,
                                 bool checks) const;

  /** Make a cell (== channel) ID from constituting fields and subCalo index;
      for (Mini)FCAL,  'region_or_dummy' argument is not used 
      for Tiles, 'barec_or_posneg' corresponds to 'section', 'sampling_or_fcalmodule' corresponds to 'side', 'region_or_dummy' corresponds to 'module',
                 'eta' corresponds to 'tower' and 'phi' corresponds to 'sample'  */
  Identifier  cell_id   	(const int subCalo, 
				 const int barec_or_posneg, 
                                 const int sampling_or_fcalmodule,
                                 const int region_or_dummy,
				 const int eta,    
                                 const int phi ) const;
  Identifier  cell_id   	(const int subCalo, 
				 const int barec_or_posneg, 
                                 const int sampling_or_fcalmodule,
                                 const int region_or_dummy,
				 const int eta,    
                                 const int phi,
                                 bool checks) const;
				 
  /** Make a region ID from a cell ID   */     
  Identifier  region_id	(const Identifier cellId ) const;

  /** not available for  Tile
      depth is only used for miniFCAL */
  Identifier  cell_id( const Identifier regionId,
		       const int eta, const int phi,
                       int depth = 0) const ;
  Identifier  cell_id( const Identifier regionId,
		       const int eta, const int phi,
                       int depth,
                       bool checks) const ;
			     
  /** create region id from 'global'(==full calo) hash id*/
  Identifier region_id	(const IdentifierHash caloRegionHash) const;
  /** create region id from 'local'(==internal to sub calo) hash id and sub calorimeter */
  Identifier region_id	(const int subCalo, const IdentifierHash subCaloRegionHash) const; 

  /** create cell id from  'global' hash id*/
  Identifier cell_id	(const IdentifierHash caloCellHash) const;
  /** create cell id from 'local'(==internal to sub calo) hash id and sub calorimeter */
  Identifier cell_id	(const int subCalo, const IdentifierHash subCaloCellHash) const;

  /** create hash id from 'global' region id*/
  IdentifierHash calo_region_hash  (const Identifier regionId) const;
  /** create hash id from 'global' region id
      @return subCalo*/
  IdentifierHash subcalo_region_hash  (const Identifier regionId, int& subCalo) const;  // subCalo is an output argument

  /** create hash id from 'global' cell id */
  IdentifierHash calo_cell_hash (const Identifier cellId) const; 
  /** create hash id from 'global' cell id
      @return subCalo*/
  IdentifierHash subcalo_cell_hash (const Identifier cellId, int& subCalo) const;  // subCalo is an output argument

  /**  cell 'global' hash table max size  */
  size_type     calo_cell_hash_max (void) const;
  /**  cell 'global' region table max size  */
  size_type     calo_region_hash_max (void) const;

  /**  to loop on 'global' cell hashes of one sub-calorimeter alone  
       @param id
       @return caloCellMin, CaloCellMax*/
  void calo_cell_hash_range (const Identifier id, IdentifierHash& caloCellMin, IdentifierHash& caloCellMax) const;
  /**  to loop on 'global' cell hashes of one sub-calorimeter alone  
       @param subCalo
       @return caloCellMin, CaloCellMax*/
  void calo_cell_hash_range (const int subCalo,   IdentifierHash& caloCellMin, IdentifierHash& caloCellMax) const;
  /**  to loop on 'global' region hashes of one sub-calorimeter alone  
       @param id
       @return caloCellMin, CaloCellMax*/
  void calo_reg_hash_range (const Identifier id, IdentifierHash& caloCellMin, IdentifierHash& caloCellMax) const;
  /**  to loop on 'global' region hashes of one sub-calorimeter alone  
       @param subCalo
       @return caloCellMin, CaloCellMax*/
  void calo_reg_hash_range (const int subCalo,   IdentifierHash& caloCellMin, IdentifierHash& caloCellMax) const;

  /** to translate a cell 'global' caloHash into a subCaloHash  @return subCalo */
  IdentifierHash subcalo_cell_hash (const IdentifierHash caloHash, int& subCalo) const;
  /** to translate a region 'global' caloHash into a subCaloHash  @return subCalo */
  IdentifierHash subcalo_region_hash (const IdentifierHash caloHash, int& subCalo) const;

  /** to translate a cell subCaloHash with its subCalo index into a 'global' caloHash  @param subCalo, subCaloHash */
  IdentifierHash calo_cell_hash (const int subCalo, const IdentifierHash subCaloHash) const;
  /** to translate a region subCaloHash with its subCalo index into a 'global' caloHash  @param subCalo, subCaloHash */
  IdentifierHash calo_region_hash (const int subCalo, const IdentifierHash subCaloHash) const;

  /** Type for iterators over identifiers. */
  typedef std::vector<Identifier>::const_iterator id_iterator;
  /** Type for range over identifiers. */
  typedef boost::iterator_range<id_iterator> id_range;

  /** begin iterator over full set of Identifiers (LAr + Tiles) */
  id_iterator cell_begin    (void) const;
  /** end iterator over full set of Identifiers (LAr + Tiles) */
  id_iterator cell_end      (void) const;
  /** Range over full set of Identifiers (LAr + Tiles) */
  id_range cell_range      (void) const;

  /** begin iterator over subCalo sets of cell Identifiers @param subCalo */
  id_iterator cell_begin    (const int subCalo) const;
  /** end iterator over subCalo sets of cell Identifiers @param subCalo */
  id_iterator cell_end      (const int subCalo) const;
  /** Range over subCalo sets of cell Identifiers @param subCalo */
  id_range cell_range      (const int subCalo) const;

  /** begin iterator over set of region Identifiers (LAr + Tiles) */
  id_iterator reg_begin    (void) const;
  /** end iterator over set of region Identifiers (LAr + Tiles) */
  id_iterator reg_end      (void) const;
  /** Range over set of region Identifiers (LAr + Tiles) */
  id_range reg_range      (void) const;

  /** begin iterator over subCalo sets of region Identifiers  @param subCalo */
  id_iterator reg_begin    (const int subCalo) const;
  /** end iterator over subCalo sets of region Identifiers  @param subCalo */
  id_iterator reg_end      (const int subCalo) const;
  /** Range over subCalo sets of region Identifiers  @param subCalo */
  id_range reg_range      (const int subCalo) const;

  /** test if the id belongs to LArEM */
  bool	is_em   	(const Identifier id) const;
  /** test if the hash id belongs to LArEM */
  bool	is_em   	(const IdentifierHash caloHash) const;
  
  /** test if the id belongs to the EM barrel */
  bool	is_em_barrel   	(const Identifier id) const;
  
  /** test if the id belongs to the EM Endcap */
  bool	is_em_endcap	(const Identifier id) const;
  
  /** test if the id belongs to the EM Endcap inner wheel */
  bool	is_em_endcap_inner	(const Identifier id) const;
  
  /** test if the id belongs to the EM Endcap outer wheel */
  bool	is_em_endcap_outer	(const Identifier id) const;
  
  /** test if the id belongs to the HEC */
  bool	is_hec   	(const Identifier id) const;
  /** test if the hash id belongs to the HEC */
  bool	is_hec   	(const IdentifierHash caloHash) const;
  
  /** test if the id belongs to the FCAL - true also for MiniFCAL */
  bool	is_fcal   	(const Identifier id) const;
  /** test if the hash id belongs to the FCAL - true also for MiniFCAL */
  bool	is_fcal   	(const IdentifierHash caloHash) const;
  
  /** test if the id belongs to the MiniFCAL */
  bool	is_minifcal   	(const Identifier id) const;
  /** test if the hash id belongs to the FCAL */
  bool	is_minifcal   	(const IdentifierHash caloHash) const;
  
  /** test if the id belongs to the Tiles */
  bool	is_tile   	(const Identifier id) const;
  /** test if the hash id belongs to the Tiles */
  bool	is_tile   	(const IdentifierHash caloHash) const;
  bool  is_tile		(const ExpandedIdentifier& id) const;

  /** test if the id belongs to the Tiles barrel*/
  bool	is_tile_barrel                  (const Identifier id) const;
  /** test if the id belongs to the Tiles extended barrel */
  bool	is_tile_extbarrel               (const Identifier id) const;
  /** test if the id belongs to the Tiles gap */
  bool	is_tile_gap                     (const Identifier id) const;
  bool	is_tile_gapscin                 (const Identifier id) const;
  /** test if the id belongs to the Tiles negative side*/
  bool  is_tile_negative                (const Identifier id) const;
  /** test if the id belongs to the Tiles positive side*/
  bool  is_tile_positive                (const Identifier id) const;

  /** returns an int taken from SUBCALO enum and describing the subCalo to which the Id belongs. */
  int         sub_calo    (const Identifier         id)const;
  /** returns an int taken from SUBCALO enum and describing the subCalo to which the hash Id belongs. */
  int         sub_calo    (const IdentifierHash caloHash)const;

  /** returns an int taken from Sampling enum and describing the subCalo to which the Id belongs. */
  int         calo_sample    (const Identifier         id)const;
  /** returns an int taken from Sampling enum and describing the subCalo to which the hash Id belongs. */
  int         calo_sample    (const IdentifierHash caloHash)const;
			     
  
  /* field values (proposal)*/
  /*
  int         first_field     (const Identifier id)const;  // corresponds to 'barrel_ec' for LArEM, 'pos_neg' for LArHEC and LArFCAL, 'section' for Tile
  int         second_field    (const Identifier id)const;  // corresponds to 'sampling' for LArEM and LArHEC, 'module' for LArFCAL, 'side' for Tile
  int         third_field     (const Identifier id)const;  // corresponds to 'region' for LArEM and LArHEC, nothing for LArFCAL, 'module' for Tile
  int         fourth_field    (const Identifier id)const;  // corresponds to 'eta' for LAr, 'tower' for Tile
  int         fifth_field     (const Identifier id)const;  // corresponds to 'phi' for LAr, 'sample' for Tile
  */

  /** LAr field values (NOT_VALID == invalid request) @warning  corresponds to 'barrel_ec' for LArEM */
  int         pos_neg         (const Identifier id)const;        // corresponds to 'barrel_ec' for LArEM
  /** LAr field values (NOT_VALID == invalid request) @warning  corresponds 'module' for FCAL/MiniFCAL*/
  int         sampling        (const Identifier id)const;        // corresponds 'module' for FCAL/MiniFCAL
  /** LAr field values (NOT_VALID == invalid request) @warning  no region for FCAL, is 'depth' for MiniFCAL*/
  int         region          (const Identifier id)const;        // no region for FCAL, is 'depth' for MiniFCAL
  /** LAr field values (NOT_VALID == invalid request) */
  int         eta             (const Identifier id)const;       
  /** LAr field values (NOT_VALID == invalid request) */
  int         phi             (const Identifier id)const;       
  /** Tile field values (NOT_VALID == invalid request) */
  int         section         (const Identifier id)const;       
  /** Tile field values (NOT_VALID == invalid request) */
  int         side            (const Identifier id)const;
  /** Tile field values (NOT_VALID == invalid request) */
  int         module          (const Identifier id)const;      
  /** Tile field values (NOT_VALID == invalid request) */
  int         tower           (const Identifier id)const;       
  /** Tile field values (NOT_VALID == invalid request) */
  int         sample          (const Identifier id)const;       

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
  int phi_min(const Identifier regId) const;    // LAr only
  /** max value of phi index (-999 == failure) 
      @warning input = REGION ID !! */
  int phi_max(const Identifier regId) const;

  /** LAr eta granularity  (NOT_VALID == failure) 
      @warning input = REGION ID !! */
  float etaGranularity(const Identifier regId) const;
  /** LAr phi granularity  (NOT_VALID == failure) 
      @warning input = REGION ID !! */
  float phiGranularity(const Identifier regId) const;
  /** minimum LAr eta for this region
      @warning input = REGION ID !! */
  float eta0(const Identifier regId) const;
  /** minimum LAr phi for this region
      @warning input = REGION ID !! */
  float phi0(const Identifier regId) const;

    /** access to EM idHelper 
     */
    const LArEM_Base_ID*        em_idHelper() const 
    {     return m_emHelper;
      }
    /** access to HEC idHelper 
     */
    const LArHEC_Base_ID*       hec_idHelper() const
      {     return m_hecHelper;
      }

    /** access to FCAL idHelper 
     */
    const LArFCAL_Base_ID*      fcal_idHelper() const
      {     return m_fcalHelper;
      }

    /** access to MiniFCAL idHelper 
     */
    const LArMiniFCAL_ID*      minifcal_idHelper() const
      {     return m_minifcalHelper;
      }

    /** access to Tile idHelper 
     */
    const Tile_Base_ID*        tile_idHelper() const 
      {     return m_tileHelper;
      }
      

  /** access to hashes for neighbours      return == 0 for neighbours found  <br>
       @warning !!! input and output hash indices are all meant as CALO !!! <br>
      option = prevInPhi, nextInPhi, prevInEta, nextInEta, corners2D, prevInSamp, nextInSamp, upAndDown, prevSubDet, nextSubDet, corners3D, prevSuperCalo, nextSuperCalo<br>
      and the composite options <br>
      option = faces2D = prevInPhi|nextInPhi|prevInEta|nextInEta, <br>
                all2D = corners2D|faces2D, <br>
                all3D = all2D|prevInSamp|nextInSamp|upAndDown|prevSubDet|nextSubDet, <br>
		all3DwithCorners = all3D|corners3D, <br>
		super3D                = all3D|prevSuperCalo|nextSuperCalo<br>
               in 'nextInEta', 'nextInSamp', 'nextSubDet', and 'nextSuperCalo', next means 'away from the centre of Atlas' <br>
               in 'prevInEta', 'prevInSamp', 'prevSubDet', and 'prevSuperCalo', prev means 'towards   the centre of Atlas' <br> 
               'all2D', 'all3D', and 'all3DwithCorners' return neighbours within one
                sub detector, while 'super3D' returns neighbours also across
                detector boundaries. <br>*/ 
  int    get_neighbours(const IdentifierHash caloHash,
                        const LArNeighbours::neighbourOption& option,
                        std::vector<IdentifierHash>& neighbourList) const;



private:
  SUBCALO get_subcalo (Identifier id) const;

  const LArEM_Base_ID*        m_emHelper;
  const LArHEC_Base_ID*       m_hecHelper;
  const LArFCAL_Base_ID*      m_fcalHelper;
  const LArMiniFCAL_ID*       m_minifcalHelper;
  const Tile_Base_ID*         m_tileHelper;
  const CaloIDHelper*         m_helpers[NSUBCALO];

  size_type                     m_cell_hash_max;
  size_type                     m_region_hash_max;
  size_type                     m_cell_min[NSUBCALO];
  size_type                     m_cell_max[NSUBCALO];
  size_type                     m_reg_min[NSUBCALO];
  size_type                     m_reg_max[NSUBCALO];
  std::vector<Identifier>       m_cell_vec;
  std::vector<Identifier>       m_region_vec;

private:
  // Avoid coverity warnings.
  CaloCell_Base_ID (const CaloCell_Base_ID&);
  CaloCell_Base_ID& operator= (const CaloCell_Base_ID&);

  CaloNeighbours*               m_caloNeighbours;

  unsigned                      m_supercell;
};



#include "CaloIdentifier/CaloCell_Base_ID.icc"


CLASS_DEF( CaloCell_Base_ID , 257140327 , 1 )


#endif // not CALOIDENTIFIER_CALOCELL_BASE_ID_H
