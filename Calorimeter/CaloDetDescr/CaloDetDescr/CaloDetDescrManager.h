// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloDetDescrManager.h
 *
 * @brief Definition of CaloDetDescrManager
 *
 * $Id: CaloDetDescrManager.h,v 1.32 2008-07-14 15:37:16 cbourdar Exp $
 */

#ifndef CALODETDESCR_CALODETDESCRMANAGER_H
#define CALODETDESCR_CALODETDESCRMANAGER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "boost/range/iterator_range.hpp"

class CaloCell_SuperCell_ID;
class CaloDetDescrElement;
class CaloDetDescriptor;
class CaloCellPacker_400_500_test;

class Identifier;
class IdentifierHash;
class CaloIdManager;

/** @class CaloDetDescrManager

   @brief This class provides the client interface for accessing the
   detector description information common to LAr and Tile. It is 
   used in the Calorimeter reconstruction (from CaloCell building down to combined 
   reconstruction)
   
   For subdetector specific functionality one should use special managers
   4 for LAr (in LArReadoutgeometry package) and one for Tile (in TileDetDescr package),
   essentially upstream of CaloCell building.

   MAJOR FUNCTIONALITIES :

   0) Access via instance() method :

   The CaloDetDescrManager used to be a singleton. It is now in StoreGate, but the
   instance() static method was kept for backward compatibility + because it is more 
   compact in client's code.


   1) 2 enums and 3 coordinate systems :

   For historical reasons, there are 2 ways to label a piece of the Calorimeter:
   the CaloCell_ID::CaloSample enum is by far the recommended one. 
   The methods CaloDetDescrManager::build_sample and CaloDetDescrManager::decode_sample 
   provide the translation.

   The CaloCell_ID::SUBCALO subCalo + bool barrel + int sampling_or_module combination
   is meaningless for Tiles, but used in many places in the LAr code, so was kept
   for backward compatibility. The boolean "barrel" is needed because there is an 
   emb/emec ambiguity (in the enum definition) and eta overlap: if true, will 
   search EMB/TILES, if false will search EMEC/HEC/FCAL.

   Also note the different coordinates :
      in EMB people generally use "eta, phi, radius",
      in EMEC/HEC people generally use "eta, phi, Z",
      in Tiles/FCAL people generally use cartesian coordinates.
   To avoid confusion, the coordinate system used is explicit in all methods signature.

   2) Access to  the "big" array of CaloDetDescrElement :

   These are stored internally according to the IdentifierHash caloCellHash.
   All navigation/convertion between the this complete hash and the
   subcalo (em/hec/fcal/tile) are provided by the CaloCell_ID helper. 


   3) Methods cellsInZone :

   Given a zone in eta/phi, a sub detector and a sampling
   (or module for FCAL), these methods return the list of CALO IdentifierHash.
   In case of failiure, the vector will be empty.
	
   WARNING : eta_min and eta_max can have NEGATIVE values and are
   not abs() (in contrast to CaloDescriptors, where eta() is
   always positive and has to be multiplied by sign() )
   
   WARNING: For the FCAL this involves a loop over all the Fcal cells, so this is a little bit
     slower if Fcal layers are required

   4) Given an eta/phi and a sampling or a depth, where-am-I ?


   These method return CaloDetDescrElement*  (to get, e.g., cell size),
   or return the entrance into the calorimeter (r or z, for barrel or endcap).

	WARNING : if eta/phi is outside the Calo range, or no Element or
	          descriptor is found for some reason, returns a NULL pointer 
		  or a radius=0 !

	WARNING : the methods get_barrel_element and get_ec_element
                  only work in the barrel/endcap regions.

    5) Methods is_in :

    Obviously, will return true if direction eta,phi crosses
    ANY of the corresponding descriptors 


    6) Methods get_entrance and get_surface (needed for TrackToCalo):

    Trk::Surfaces are not built by the CaloDetDescrManager to avoid 
    any dependency from Calorimeter onto Tracking : this is done in the
    Reconstruction/RecoTools/TrackToCalo package. But this class provides
    the surface parameters
    
    By convention, for each CaloSample the radius/z correspond to
    the entrance of the sampling, and are "flat" in the sense that there is no 
    eta dependance. This is valid for PS, Strips, HEC, FCAL and Tiles, but not 
    for EMB2,EMB3,EMEC2,EMEC3
    
    Clients who want a more precise (and eta dependant) entrance radius can
    overwrite the default one by the result of the methods 
    get_entrance_radius/get_entrance_z .
    
    Clients who want to add an offset, use the middle rather than entrace or use a 
    parametrised depth are invited to use the CaloDepthTool, which is in 
    Reconstruction/RecoTool/TrackToCalo

    7) Access to the CaloDetDescriptors :
    
    In LAr, there is 1 CaloDetDescriptor per region of homogeneus granularity. 
    Their primary goal if to speedup loops by grouping cells within regions, and 
    save spave by caching what they have in common. You may need to access them 
    to get, for example, the grannularity.
    
*/
class CaloDetDescrManager_Base
{
 public:
  /** @brief Default Constructor
   */
  CaloDetDescrManager_Base();
  /** @brief destructor
   */
  ~CaloDetDescrManager_Base();
  
  /** @brief initialization of the manager, used by the Converter when it creates the Manager
   */
  void initialize();
  /** @brief print out the contents
   */
  void print() const;

  /** @brief for backwards compatibility only
   */
  inline bool isInitialized() const {return true;}
  /** @brief get lar geometry label, distinguish between full atlas and TB
   */
  inline std::string lar_geometry() const {return m_lar_geometry;}
  /** @brief set lar geometry label, distinguish between full atlas and TB
   */
  inline void set_lar_geometry(std::string geom) { m_lar_geometry = geom;}
 
  /** @brief get calo cell ID helper
   */
  const CaloCell_Base_ID*   getCaloCell_ID() const;
  /** @brief get calo ID manager
   */
  const CaloIdManager* getCalo_Mgr() const;

  typedef std::vector <CaloDetDescrElement*> calo_element_vec;
  typedef calo_element_vec::size_type        calo_element_vec_size;
  typedef calo_element_vec::const_iterator   calo_element_const_iterator;
  typedef boost::iterator_range<calo_element_const_iterator> calo_element_range;

  /** @brief total number of elements
   */
  calo_element_vec_size       element_size()  const;
  /** @brief first element
   */
  calo_element_const_iterator element_begin() const;
  /** @brief end of element vector
   */
  calo_element_const_iterator element_end()   const;
  /** @brief Range over element vector
   */
  calo_element_range element_range()   const;
  /** @brief first element in subcalo
   */
  calo_element_const_iterator element_begin(CaloCell_ID::SUBCALO subCalo) const;
  /** @brief end of subcalo element list
   */
  calo_element_const_iterator element_end(CaloCell_ID::SUBCALO subCalo)   const;
  /** @brief Range over subcalo element list
   */
  calo_element_range element_range(CaloCell_ID::SUBCALO subCalo)   const;

  /** @brief get element by its identifier
      @param cellId [IN] element identifier
   */
  const CaloDetDescrElement* get_element(const Identifier& cellId) const;
  /** @brief get element by its identifier, non-const version.
      @param cellId [IN] element identifier
   */
  CaloDetDescrElement* get_element_nonconst(const Identifier& cellId);
  /** @brief get element by hash identifier
      @param caloCellHash [IN] hash identifier for the element
   */
  const CaloDetDescrElement* get_element(const IdentifierHash& caloCellHash) const;
  /** @brief get element by hash identifier, non-const version.
      @param caloCellHash [IN] hash identifier for the element
   */
  CaloDetDescrElement* get_element_nonconst(const IdentifierHash& caloCellHash);
  /** @brief get element by subcalo and hash identifier
      @param subCalo [IN] subsystem
      @param subCaloCellHash [IN] sub calo hash
   */
  const CaloDetDescrElement* get_element (CaloCell_ID::SUBCALO subCalo,
                                          const IdentifierHash& subCaloCellHash) const;
  /** @brief LAr only! get element by subcalo, sampling, barrel flag, eta, phi.  This is slower for FCAL
   */
  const CaloDetDescrElement* get_element (CaloCell_ID::SUBCALO subCalo,
                                          int sampling_or_module, 
                                          bool barrel,
                                          double eta, double phi) const;
  /** @brief LAr only! get element by sample, eta phi.  This is slower for FCAL
   */
  const CaloDetDescrElement* get_element (CaloCell_ID::CaloSample sample,
                                          double eta, 
                                          double phi) const;

  /** @brief Get element from raw quantities (to build real fixed size clusters)
   */
  const CaloDetDescrElement* get_element_raw (CaloCell_ID::CaloSample sample,
                                              double eta, 
                                              double phi) const;

  /** @brief the only client is CaloCellList class 
   */
  void cellsInZone(double eta_min, double eta_max, 
		   double phi_min, double phi_max,
		   std::vector<IdentifierHash> & cell_list) const;
    
  /** @brief the only client is CaloCellList class 
   */
  void cellsInZone(double eta_min, double eta_max, double phi_min,
		   double phi_max, CaloCell_ID::SUBCALO subCalo,
		   std::vector<IdentifierHash> & cell_list) const;
    
  /** @brief the only client is CaloCellList class 
   */
  void cellsInZone(double eta_min, double eta_max, double phi_min,
		   double phi_max, CaloCell_ID::CaloSample sample,
		   std::vector<IdentifierHash> & cell_list) const;
  
  /** @brief the only client is CaloCellList class 
   */
  void cellsInZone(double eta_min, double eta_max,double phi_min, 
		   double phi_max, CaloCell_ID::SUBCALO subCalo,
		   int sampling_or_module, 
		   std::vector<IdentifierHash> & cell_list) const;

  /** @brief the only client is CaloCellList class 
   */
  void cellsInZone(double eta, double phi, int ncell_eta,
		   int ncell_phi, CaloCell_ID::SUBCALO subCalo,
		   int sampling_or_module, bool barrel,
		   std::vector<IdentifierHash> & cell_list) const;
    
  /** @brief the only client is CaloCellList class 
   */
   void cellsInZone(double eta_min, double eta_max, double phi_min,
                     double phi_max, const CaloDetDescriptor* descr,
                     std::vector<IdentifierHash> & cell_list) const;

  /** @brief return true if direction eta,phi crosses ANY of the corresponding descriptors
      @param eta [IN] eta direction
      @param phi [IN] phi direction
      @param sample [IN] Calo Sample
      @param etadist [OUT] distance in eta to the neares edge
   */
  bool is_in (double eta, double phi, CaloCell_ID::CaloSample sample, double& etadist) const; 
  
  /** @brief translate between the 2 ways to label a sub-detector: 
   */
  void build_sample (CaloCell_ID::SUBCALO subCalo, bool barrel,
		     int sampling_or_module,
		     CaloCell_ID::CaloSample& sample) const;
  /** @brief translate between the 2 ways to label a sub-detector: 
   */
  void decode_sample (CaloCell_ID::SUBCALO& subCalo, bool& barrel,
		      int& sampling_or_module,
		      CaloCell_ID::CaloSample sample) const;   

  typedef std::vector <CaloDetDescriptor*>   calo_descr_vec;
  typedef calo_descr_vec::size_type	     calo_descr_size;
  typedef calo_descr_vec::const_iterator     calo_descr_const_iterator;
  typedef boost::iterator_range<calo_descr_const_iterator> calo_descr_range;
    
  /** @brief first descriptor in the vector
   */
  calo_descr_const_iterator	calo_descriptors_begin() const;
  /** @brief end of descriptors vector
   */
  calo_descr_const_iterator	calo_descriptors_end() const;
  /** @brief number of descriptors
   */
  calo_descr_size 		calo_descriptors_size() const;
  /** @brief Range over descriptors
   */
  calo_descr_range 		calo_descriptors_range() const;

  /** @brief get descriptor by region identifier
   */
  const CaloDetDescriptor* get_descriptor (const Identifier& regionId) const;
  /** @brief get descriptor by region identifier, non-const version.
   */
  CaloDetDescriptor* get_descriptor_nonconst (const Identifier& regionId);
  /** @brief get descriptor by subcalo, sampling, barrel flag, eta, phi
   */
  const CaloDetDescriptor* get_descriptor(CaloCell_ID::SUBCALO subCalo,
                                          int sampling_or_module, 
                                          bool barrel,
                                          double eta, 
                                          double phi) const;
  /** @brief get descriptor by sample, eta and phi
   */
  const CaloDetDescriptor* get_descriptor (CaloCell_ID::CaloSample sample,
                                           double eta, double phi) const;

  /** @brief get descriptor by sample, eta and phi raw
   */
  const CaloDetDescriptor* get_descriptor_raw (CaloCell_ID::CaloSample sample, 
                                               double eta, double phi) const;


  /** @brief first tile descriptor (they are in separate vector)
   */
  calo_descr_const_iterator   tile_descriptors_begin () const;
  /** @brief end of tile descriptors vector
   */
  calo_descr_const_iterator   tile_descriptors_end   () const;
  /** @brief number of tile descriptors
   */
  calo_descr_size             tile_descriptors_size  () const;
  /** @brief Range over tile descriptors
   */
  calo_descr_range            tile_descriptors_range  () const;

  /** @brief set calo Cell ID helper
   */
  void set_helper (const CaloCell_Base_ID* idHelper);
  /** @brief set calo ID Manager Helper
   */
  void set_helper (const CaloIdManager* idHelper);

  /** @brief add Calo DD element to the elements vector
   */
  void add(CaloDetDescrElement* element);
  /** @brief add new LAr descriptor to the descriptors vector
   */
  void add(CaloDetDescriptor* descr);
  /** @brief add tile descriptor to the tile descriptors vector
   */
  void add_tile(CaloDetDescriptor* descr);

private:
  /**
   * @brief Helper for CaloCompactCellTool_test.
   */
  friend class CaloCellPacker_400_500_test;
  CaloDetDescrElement* release_element (IdentifierHash hash);


  /** @brief Calo Cell ID helper
   */
  const CaloCell_Base_ID*    m_cell_id;
  /** @brief Calo ID Manager helper
   */
  const CaloIdManager*  m_calo_mgr;
  
  /** @brief vector of Calo DD elements
   */
  calo_element_vec 	m_element_vec;
  /** @brief vector of descriptors
   */
  calo_descr_vec 	m_descr_vec;

  /** @brief offsets for subcalo's elements 
   */
  std::vector <IdentifierHash> m_subCalo_min;
  /** @brief offsets for subcalo's elements 
   */
  std::vector <IdentifierHash> m_subCalo_max;
  /** @brief offsets for subcalo's elements 
   */
  std::vector <calo_element_const_iterator> m_subCalo_begin;
  /** @brief offsets for subcalo's elements 
   */
  std::vector <calo_element_const_iterator> m_subCalo_end;

  /** @brief vector of descriptors
   */
  calo_descr_vec      m_tile_descr_vec;

  /** @brief LAr geometry label (full atlas/TB) for backwards compatibility
   */
  std::string m_lar_geometry;

  /** @brief LArFCAl  private methode to get element
   */
  const CaloDetDescrElement* get_element_FCAL (const CaloDetDescriptor* reg,
                                               double eta, 
                                               double phi) const;

  /** @brief LArFCAl  private methode to get element from raw eta,phi
   */
  const CaloDetDescrElement* get_element_FCAL_raw (const CaloDetDescriptor* reg,
                                                   double eta,
                                                   double phi) const;


};


class CaloDetDescrManager
  : public CaloDetDescrManager_Base
{
public:
  /** @brief for backwards compatibility, provides access to the Calo DD manager 
      which is actually kept in DS
   */
  static const CaloDetDescrManager* instance();

  /** @brief get calo cell ID helper
   */
  const CaloCell_ID*   getCaloCell_ID() const;
  /** @brief set calo Cell ID helper
   */
  void set_helper (const CaloCell_ID* idHelper);
  void set_helper (const CaloIdManager* idHelper);

private:
  /** @brief static pointer - for backwards compatibility
   */
  static CaloDetDescrManager* s_instance;

};

CLASS_DEF( CaloDetDescrManager , 4548337 , 1 )


class CaloSuperCellDetDescrManager
  : public CaloDetDescrManager_Base
{
public:
  virtual ~CaloSuperCellDetDescrManager();

  /** @brief provides access to the Calo DD manager 
      which is actually kept in DS
   */
  static const CaloSuperCellDetDescrManager* instance();

  /** @brief get calo cell ID helper
   */
  const CaloCell_SuperCell_ID*   getCaloCell_ID() const;
  /** @brief set calo Cell ID helper
   */
  void set_helper (const CaloCell_SuperCell_ID* idHelper);
  void set_helper (const CaloIdManager* idHelper);

private:
  /** @brief static pointer 
   */
  static CaloSuperCellDetDescrManager* s_instance;

};


CLASS_DEF( CaloSuperCellDetDescrManager , 241807251 , 1 )


inline  const CaloDetDescrElement*			
CaloDetDescrManager_Base::get_element (const IdentifierHash& caloCellHash) const
{
  if ( caloCellHash < m_element_vec.size() ) 
    return m_element_vec[caloCellHash] ;
  else return 0 ;
}
                       
inline CaloDetDescrElement*			
CaloDetDescrManager_Base::get_element_nonconst (const IdentifierHash& caloCellHash)
{
  if ( caloCellHash < m_element_vec.size() ) 
    return m_element_vec[caloCellHash] ;
  else return 0 ;
}
                       

#endif // CALODETDESCR_CALODETDESCRMANAGER_H
