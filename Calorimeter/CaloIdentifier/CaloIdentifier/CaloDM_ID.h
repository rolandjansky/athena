/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALODM_ID_H
#define CALODM_ID_H

#include "CLIDSvc/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "IdDict/IdDictFieldImplementation.h"

#include "CaloIdentifier/CaloID_Exception.h"

#include <vector>
#include <algorithm>


/**
*
* @class CaloDM_ID
* @brief Helper class for Calo Dead Material offline identifiers
*  
*  This class provides an interface to decode and generate offline identifiers
*  for the Calo Dead Materials.  <p>
* 
* Definition and range of values for the elements of the
* identifier are (ATL-SOFT-2001-004): <p>
* <pre>
* element           range              meaning
* -------           -----              -------
* 
* DETZSIDE            +/-4         positive/negative LAr
*                     +/-5         positive/negative Tile
*
* LAr:
* ====
* DeadMAT             [1,3]        1=outside accordion ....
*                                  2=internal "cracks"
*                                  3=beam test
*
* 
* sampling         [0,3]     type 1 and type 2
*                    ?       type 3      
* 
* region           [0,5]     type 1, sampling 0
*                  [0,7]     type 1, sampling 1     
*                  [0,5]     type 1, sampling 2     
*                  [0,2]     type 1, sampling 3     
*                  [0,3]     type 2, sampling [0,3]     
*                    ?       type 3
* 
* eta              [0,49]    values and ranges depend on the DM type, the sampling, the region etc ....         
*
* phi              [0,63]         all except below
* "                [0,31]    type 2, sampling [0,3], region 2     
* "                  0       type 1, sampling 3, region 2        
*
* Tile
* ====
* DeadMAT          [1,3]     1=dead material around sensitive material
*                            2=dead material in internal "cracks" (if any)
*                            3=beam test
* 
* sampling         [0,3]     type 1
*                    0       type 2,3      
* 
* region           [0,1]     type 1, sampling 0
*                  [0,2]     type 1, sampling 1     
*                  [0,2]     type 1, sampling 2     
*                    0       type 1, sampling 3     
*                    0       type 2,3   
* 
* eta              [0,9]     type 1, sampling 0, region 0
*                  [0,4]     type 1, sampling 0, region 1
*                  [0,3]     type 1, sampling 1, region 0
*                  [0,2]     type 1, sampling 1, region 1
*                  [0,3]     type 1, sampling 1, region 2
*                  [0,7]     type 1, sampling 2, region 0
*                  [0,4]     type 1, sampling 2, region 1
*                  [0,1]     type 1, sampling 2, region 2
*                  [0,16]    type 1, sampling 3, region 0
*                    0       type 2,3   
*
* phi              [0,63]    in all regions
*
*
*
* region _id        DETZSIDE/DMTYPE/SAMPLING/REGION
*
*
* zone_id           Region_ID+ETA+PHI
*
* </pre>
* @author started by Fairouz Malek
* @author maintained by Fabienne Ledroit
*/
class IdentifierHash;

class CaloDM_ID : public AtlasDetectorID
{
public:        

  typedef Identifier::size_type  size_type ;


  CaloDM_ID(void);    

  virtual ~CaloDM_ID();

  /** build a region identifier valid for both LAr and Tiles  */
  Identifier region_id  ( int pos_neg_z, int dmat, int sampling, int region )  const;

  /** build a region identifier valid for both LAr and Tiles  */
  Identifier region_id  (const Identifier& zoneId ) const;

  /** build a zone identifier valid for both LAr and Tiles  */
  Identifier  zone_id   	(int pos_neg_z, int dat, int sampling, int region,
				 int eta,       int phi )  const;
				 
  /** build a zone identifier valid for both LAr and Tiles  */
  Identifier  zone_id   	(const Identifier& regionId,
    				 int eta,       int phi )  const ;

  /** to disentangle between LAr and Tile dead material */
  bool is_lar  (const Identifier& zoneId) const;
  /** to disentangle between LAr and Tile dead material */
  bool is_tile (const Identifier& zoneId) const;
				 
  /** create a lar region id from hash id*/
  Identifier lar_region_id	(IdentifierHash lar_region_hash_id) const;
  /** create a tile region id from hash id*/
  Identifier tile_region_id	(IdentifierHash tile_region_hash_id) const;

  /** create a lar zone id from hash id*/
  Identifier lar_zone_id	(IdentifierHash lar_zone_hash_id) const;	
  /** create a tile zone id from hash id*/
  Identifier tile_zone_id	(IdentifierHash tile_zone_hash_id) const;	

  /** create a lar hash id from region id*/
  IdentifierHash lar_region_hash  (Identifier LArRegionId) const;
  /** create a tile hash id from region id*/
  IdentifierHash tile_region_hash  (Identifier TileRegionId) const;

  /** create a lar hash id from zone id*/
  IdentifierHash lar_zone_hash (Identifier LArZoneId) const;
  /** create a tile hash id from zone id*/
  IdentifierHash tile_zone_hash (Identifier TileZoneId) const;
 

  /**  lar region hash table max size */
   size_type     lar_region_hash_max (void) const;
  /**  lar zone hash table max size */
   size_type     lar_zone_hash_max (void) const;
  /**  tile region hash table max size */
   size_type     tile_region_hash_max (void) const;
  /**  tile zone hash table max size */
   size_type     tile_zone_hash_max (void) const;

  /** begin iterator over lar regions */
  std::vector<Identifier>::const_iterator lar_region_begin    (void) const;
  /** end iterator over lar regions */
  std::vector<Identifier>::const_iterator lar_region_end      (void) const;
  /** begin iterator over tile regions */
  std::vector<Identifier>::const_iterator tile_region_begin    (void) const;
  /** end iterator over tile regions */
  std::vector<Identifier>::const_iterator tile_region_end      (void) const;

 /** begin iterator over full set of LAr identifiers */
  std::vector<Identifier>::const_iterator lar_zone_begin    (void) const;
 /** end iterator over full set of LAr identifiers */
  std::vector<Identifier>::const_iterator lar_zone_end      (void) const;
 /** begin iterator over full set of Tile identifiers */
  std::vector<Identifier>::const_iterator tile_zone_begin    (void) const;
 /** end iterator over full set of Tile identifiers */
  std::vector<Identifier>::const_iterator tile_zone_end      (void) const;

 
  /**
   * return pos_neg_z according to : <br>
   * 
   * <pre>
   * element           range              meaning
   * -------           -----              -------
   * DETZSIDE           +/-4             positive/negative LAr
   *                    +/-5             positive/negative Tile
   * 
   * failure returns 0
   * 
   * </pre> 
   */
  int         pos_neg_z       (const Identifier& id)const;  

   /**
   * return DMtype according to : <br>
   * 
   * <pre>
   * element           range             
   * -------           -----             
   * DeadMat             [1,3]       LAr
   *                     [1,3]       Tile           
   * 
   * failure returns 0
   * 
   * </pre> 
   */
  int         dmat             (const Identifier& id)const;

  /**
   * return sampling according to : <br>
   * 
   * <pre>
   * element           range              meaning
   * -------           -----              -------
   * sampling         [0,3]           type 1 and type 2 (LAr only)
   *                    ?                type 3      
   * 
   * failure returns 0
   * 
   * </pre> 
   */
  int         sampling        (const Identifier& id)const;  

  /**
   * return region according to : <br>
   * 
   * <pre>
   * element           range              meaning
   * -------           -----              -------
   * lar region         0            type 1, sampling 0
   *                  [0,7]          type 1, sampling 1     
   *                  [0,5]          type 1, sampling 2     
   *                  [0,2]          type 1, sampling 3     
   *                  [0,3]          type 2, sampling [0,3]     
   *                    ?            type 3
   * 
   * 
   * failure returns 0
   * 
   * </pre> 
   */
  int         region          (const Identifier& id)const;  

  /**
   * return eta  <br>
   * 
   */
  int         eta             (const Identifier& id)const; 

  /**
   * return phi <br>
   * 
   */
  int         phi             (const Identifier& id)const;

  

  /** min value of eta index (-999 == failure) 
      @warning input = REGION ID !! */
  int eta_min(const Identifier& id) const;
  /** max value of eta index (-999 == failure) 
      @warning input = REGION ID !! */
  int eta_max(const Identifier& id) const;
  /** min value of phi index (-999 == failure) 
      @warning input = REGION ID !! */
  int phi_min(const Identifier& id) const;
  /** max value of phi index (-999 == failure) 
      @warning input = REGION ID !! */
  int phi_max(const Identifier& id) const;
  

  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);

  /** access to IdContext's which define which levels of fields are contained in a region id */
  IdContext    region_context   (void) const;

  /** access to IdContext's which define which levels of fields are contained in a zone id */
  IdContext    zone_context   (void) const;


private:    

     
  enum {NOT_VALID_HASH = 128000};


  IdentifierHash lar_zone_hash_binary_search (Identifier zoneId) const;
  IdentifierHash tile_zone_hash_binary_search (Identifier zoneId) const;

  /** create expanded Identifier from Identifier (return == 0 for OK) */
  int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;


  void lar_region_id_checks  ( int pos_neg_z, int dmat, int sampling, int region ) const;

  void tile_region_id_checks  ( int pos_neg_z, int dmat, int sampling, int region ) const;

  void lar_zone_id_checks   	(int pos_neg_z, int dat, int sampling, int region,
				 int eta,       int phi ) const;
  void tile_zone_id_checks   	(int pos_neg_z, int dat, int sampling, int region,
				 int eta,       int phi ) const;
  void  zone_id_checks   	(const Identifier& regionId,
    				 int eta,       int phi )  const;



  int         initLevelsFromDict(void) ;

  int         init_lar_hashes(void) ;
  int         init_tile_hashes(void) ;


  size_type                     m_calodm_region_index;
  size_type                     m_CALO_INDEX;
  size_type                     m_DETZSIDE_INDEX;
  size_type                     m_DMAT_INDEX;
  size_type                     m_SAMPLING_INDEX;
  size_type                     m_REGION_INDEX;
  size_type                     m_ETA_INDEX;
  size_type                     m_PHI_INDEX;
 
  const IdDictDictionary*       m_dict;

  MultiRange                    m_full_lar_region_range;
  MultiRange                    m_full_lar_zone_range;
  size_type                     m_lar_zone_hash_max;
  size_type                     m_lar_region_hash_max;
  std::vector<Identifier>       m_lar_zone_vec;
  std::vector<Identifier>       m_lar_region_vec;

  MultiRange                    m_full_tile_region_range;
  MultiRange                    m_full_tile_zone_range;
  size_type                     m_tile_zone_hash_max;
  size_type                     m_tile_region_hash_max;
  std::vector<Identifier>       m_tile_zone_vec;
  std::vector<Identifier>       m_tile_region_vec;

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

  std::vector<HashCalc>         m_lar_hash_calcs;
  std::vector<HashCalc>         m_tile_hash_calcs;



  IdDictFieldImplementation     m_calo_impl;
  IdDictFieldImplementation  	m_calodm_impl;
  IdDictFieldImplementation  	m_dmat_impl;
  IdDictFieldImplementation  	m_sampling_impl;
  IdDictFieldImplementation  	m_region_impl;
  IdDictFieldImplementation  	m_eta_impl;
  IdDictFieldImplementation  	m_phi_impl;
 

  IdDictFieldImplementation  	m_pnz_reg_impl;

};

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( CaloDM_ID , 167756483 , 1 )

     
//----------------------------------------------------------------------------
inline Identifier CaloDM_ID::region_id (int pos_neg_z, int dmat, int sampling, int region)  const
{
    Identifier result(0);
    // Pack fields independently
    m_calo_impl.pack     (calo_field_value(),             result);
    m_calodm_impl.pack   (pos_neg_z,                      result);
    m_dmat_impl.pack     (dmat,                           result);
    m_sampling_impl.pack (sampling,                       result);
    m_region_impl.pack   (region,                         result);

    // Do checks
    if(m_do_checks) {
      if(abs(pos_neg_z) == 4) {
	lar_region_id_checks( pos_neg_z, dmat, sampling, region);
      } else
      if(abs(pos_neg_z) == 5) {
	tile_region_id_checks( pos_neg_z, dmat, sampling, region);
      }
      else
	{
	  CaloID_Exception except("wrong value for pos_neg_z, only +-4 and +-5 allowed" , 99);
	  throw except ;   
	}
    }

    return result;
}

//----------------------------------------------------------------------------
inline Identifier 
CaloDM_ID::region_id   ( const Identifier& zoneId ) const 
{
  Identifier result(zoneId);
  //  reset eta/phi
  m_eta_impl.reset(result);
  m_phi_impl.reset(result);
  return (result);
}

//----------------------------------------------------------------------------
inline Identifier CaloDM_ID::zone_id   ( int pos_neg_z, int dmat, int sampling, int region,
					 int eta,       int phi )  const
{  
    Identifier result(0);
    // Pack fields independently
    m_calo_impl.pack     (calo_field_value(),             result);
    m_calodm_impl.pack   (pos_neg_z,                      result);
    m_dmat_impl.pack     (dmat,                           result);
    m_sampling_impl.pack (sampling,                       result);
    m_region_impl.pack   (region,                         result);
    m_eta_impl.pack      (eta,                            result);
    m_phi_impl.pack      (phi,                            result);

    // Do checks
    if(m_do_checks) {
      if(abs(pos_neg_z) == 4) {
	lar_zone_id_checks( pos_neg_z, dmat, sampling, region, eta, phi );
      } 
      else if(abs(pos_neg_z) == 5) {
	tile_zone_id_checks( pos_neg_z, dmat, sampling, region, eta, phi );
      }
      else {
	CaloID_Exception except("wrong value for pos_neg_z, only +-4 and +-5 allowed" , 99);
	throw except ;   
      }
    }

    return result;
}
 
//----------------------------------------------------------------------------
inline Identifier CaloDM_ID::zone_id   ( const Identifier& regionId,
					 int eta,  int phi ) const 
{
    Identifier result(regionId);

    // Reset the fields and then set the values
    m_eta_impl.reset     (result);
    m_phi_impl.reset     (result);
    m_eta_impl.pack      (eta, result);
    m_phi_impl.pack      (phi, result);

    // Do checks
    if(m_do_checks) {
	zone_id_checks( regionId, eta, phi );
    }

    return result;
}

//----------------------------------------------------------------------------
inline bool CaloDM_ID::is_lar(const Identifier& zoneId) const
{
    return ( abs(m_calodm_impl.unpack(zoneId)) == 4 );
}

//----------------------------------------------------------------------------
inline bool CaloDM_ID::is_tile(const Identifier& zoneId) const
{
    return ( abs(m_calodm_impl.unpack(zoneId)) == 5 );
}

//----------------------------------------------------------------------------
inline Identifier CaloDM_ID::lar_region_id    (IdentifierHash lar_region_hash_id) const
{
    return(m_lar_region_vec[lar_region_hash_id]);
}

//----------------------------------------------------------------------------
inline Identifier CaloDM_ID::tile_region_id    (IdentifierHash tile_region_hash_id) const
{
    return(m_tile_region_vec[tile_region_hash_id]);
}

//----------------------------------------------------------------------------
inline Identifier CaloDM_ID::lar_zone_id    (IdentifierHash lar_zone_hash_id) const
{
    return(m_lar_zone_vec[lar_zone_hash_id]);
}

//----------------------------------------------------------------------------
inline Identifier CaloDM_ID::tile_zone_id    (IdentifierHash tile_zone_hash_id) const
{
    return(m_tile_zone_vec[tile_zone_hash_id]);
}

//----------------------------------------------------------------------------
inline IdentifierHash CaloDM_ID::lar_region_hash (Identifier LArRegionId) const
{
    std::vector<Identifier>::const_iterator it = std::lower_bound(m_lar_region_vec.begin(),m_lar_region_vec.end(),LArRegionId);
    if ( it != m_lar_region_vec.end() ){
	return (it - m_lar_region_vec.begin());
    }
    return (0);
}

//----------------------------------------------------------------------------
inline IdentifierHash CaloDM_ID::tile_region_hash (Identifier TileRegionId) const
{
    std::vector<Identifier>::const_iterator it = std::lower_bound(m_tile_region_vec.begin(),m_tile_region_vec.end(),TileRegionId);
    if ( it != m_tile_region_vec.end() ){
	return (it - m_tile_region_vec.begin());
    }
    return (0);
}

//----------------------------------------------------------------------------
inline IdentifierHash CaloDM_ID::lar_zone_hash (Identifier LArZoneId) const
{
    const HashCalc& hc = m_lar_hash_calcs[m_pnz_reg_impl.unpack(LArZoneId)];
    return (hc.m_hash + (eta(LArZoneId)-hc.m_etamin)*hc.m_nphi + phi(LArZoneId));
}

//----------------------------------------------------------------------------
inline IdentifierHash CaloDM_ID::tile_zone_hash (Identifier TileZoneId) const
{
    const HashCalc& hc = m_tile_hash_calcs[m_pnz_reg_impl.unpack(TileZoneId)];
    return (hc.m_hash + (eta(TileZoneId)-hc.m_etamin)*hc.m_nphi + phi(TileZoneId));
}

//----------------------------------------------------------------------------
inline CaloDM_ID::size_type   CaloDM_ID::lar_region_hash_max (void) const
{
  return m_lar_region_hash_max;
}

//----------------------------------------------------------------------------
inline CaloDM_ID::size_type   CaloDM_ID::lar_zone_hash_max (void) const
{
  return m_lar_zone_hash_max;
}

//----------------------------------------------------------------------------
inline CaloDM_ID::size_type   CaloDM_ID::tile_region_hash_max (void) const
{
  return m_tile_region_hash_max;
}

//----------------------------------------------------------------------------
inline CaloDM_ID::size_type   CaloDM_ID::tile_zone_hash_max (void) const
{
  return m_tile_zone_hash_max;
}

//----------------------------------------------------------------------------
inline std::vector<Identifier>::const_iterator CaloDM_ID::lar_region_begin    (void) const
{
  return(m_lar_region_vec.begin());
}

//----------------------------------------------------------------------------
inline std::vector<Identifier>::const_iterator CaloDM_ID::lar_region_end      (void) const
{
  return(m_lar_region_vec.end());
}

//----------------------------------------------------------------------------
inline std::vector<Identifier>::const_iterator CaloDM_ID::lar_zone_begin    (void) const
{
  return(m_lar_zone_vec.begin());
}

//----------------------------------------------------------------------------
inline std::vector<Identifier>::const_iterator CaloDM_ID::lar_zone_end      (void) const
{
  return(m_lar_zone_vec.end());
}

//----------------------------------------------------------------------------
inline std::vector<Identifier>::const_iterator CaloDM_ID::tile_region_begin    (void) const
{
  return(m_tile_region_vec.begin());
}

//----------------------------------------------------------------------------
inline std::vector<Identifier>::const_iterator CaloDM_ID::tile_region_end      (void) const
{
  return(m_tile_region_vec.end());
}


//----------------------------------------------------------------------------
inline std::vector<Identifier>::const_iterator CaloDM_ID::tile_zone_begin    (void) const
{
  return(m_tile_zone_vec.begin());
}

//----------------------------------------------------------------------------
inline std::vector<Identifier>::const_iterator CaloDM_ID::tile_zone_end      (void) const
{
  return(m_tile_zone_vec.end());
}

//----------------------------------------------------------------------------
inline int CaloDM_ID::pos_neg_z(const Identifier& id) const
{
  return (m_calodm_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int CaloDM_ID::sampling(const Identifier& id) const
{
  return (m_sampling_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int CaloDM_ID::region(const Identifier& id) const
{
  return (m_region_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int CaloDM_ID::eta(const Identifier& id) const
{
  return (m_eta_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int CaloDM_ID::phi(const Identifier& id) const
{
  return (m_phi_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int CaloDM_ID::dmat(const Identifier& id) const
{
  return (m_dmat_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline IdentifierHash CaloDM_ID::lar_zone_hash_binary_search  (Identifier zoneId) const
{
    std::vector<Identifier>::const_iterator it = std::lower_bound(m_lar_zone_vec.begin(),m_lar_zone_vec.end(),zoneId);
    if ( it != m_lar_zone_vec.end() ){
        return (it - m_lar_zone_vec.begin());
    }
    return (0);
}

//----------------------------------------------------------------------------
inline IdentifierHash CaloDM_ID::tile_zone_hash_binary_search  (Identifier zoneId) const
{
    std::vector<Identifier>::const_iterator it = std::lower_bound(m_tile_zone_vec.begin(),m_tile_zone_vec.end(),zoneId);
    if ( it != m_tile_zone_vec.end() ){
        return (it - m_tile_zone_vec.begin());
    }
    return (0);
}

#endif // CALODM_ID_H
