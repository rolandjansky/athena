/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileDetDescrManager_h
#define TileDetDescrManager_h 1

#include "GeoModelKernel/GeoVDetectorManager.h"
#include "Identifier/IdentifierHash.h"
#include "TileDetDescr/TileDetDescrRegion.h"
#include "CaloIdentifier/CaloCell_ID.h"
     
#include <vector>
#include <map>
#include <cmath>

#include "boost/shared_ptr.hpp"

class TileDddbManager;
class TileDetDescrRegion;
class TileDetDescriptor;
class TileID;
class TileHWID;
class CaloDetDescriptor;
class CaloDetDescrElement;
class TileCellDim;
class MsgStream;

typedef boost::shared_ptr<TileDddbManager> TileDddbManager_ptr;

class TileDetDescrManager : public GeoVDetectorManager
{
 public:
  // Temporary solution for cell volumes 
  // these numbers are normally overwritten by numbers from GeoModel in releases after 12.0.0
  static const double vBarrelCells[23];
  static const double vExtendedCells[12];
  static const double vItcGapCells[6];

  // TYPEDEFS
  typedef std::vector <TileDetDescrRegion*>		tile_region_vec;
  typedef tile_region_vec::size_type		        tile_region_vec_size;
  typedef tile_region_vec::const_iterator		tile_region_const_iterator;

  typedef std::vector <TileDetDescriptor*>		tile_descr_vec;
  typedef tile_descr_vec::size_type		        tile_descr_vec_size;
  typedef tile_descr_vec::const_iterator		tile_descr_const_iterator;

  typedef std::vector <CaloDetDescriptor*>		calo_descr_vec;
  typedef calo_descr_vec::size_type		        calo_descr_vec_size;
  typedef calo_descr_vec::const_iterator		calo_descr_const_iterator;

  typedef std::vector <CaloDetDescrElement*>		calo_element_vec;
  typedef calo_element_vec::size_type		        calo_element_vec_size;
  typedef calo_element_vec::const_iterator		calo_element_const_iterator;

  typedef std::map <unsigned int, TileDetDescrRegion*,  std::less<unsigned int> >  tile_region_map;
  typedef std::map <unsigned int, CaloDetDescriptor*,   std::less<int> >           scalo_descr_map;
  typedef std::map <unsigned int, CaloDetDescriptor*,   std::less<unsigned int> >   calo_descr_map;
  typedef std::map <unsigned int, CaloDetDescrElement*, std::less<unsigned int> > calo_element_map;
  typedef std::map <unsigned int, TileCellDim*, std::less<unsigned int> >             cell_dim_map;

  // --------------- TYPEDEFS

  TileDetDescrManager(TileDddbManager_ptr dbManager);
  ~TileDetDescrManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  virtual PVConstLink getTreeTop(unsigned int i) const;

  void addTreeTop(PVLink);      // Add a Tree top:

  // ----------------  Access to raw geometry:

  // Access to TileDddbManager object
  TileDddbManager* getDbManager() const;
  // Release Db manager
  void releaseDbManager();

  // -------------- Readout geometry ---------------------

  //  Access to Tile Regions
  TileDetDescrRegion* find_tile_region(const Identifier& region_id) const    // Region find()
    { tile_region_map::const_iterator it = m_tile_region_map.find(region_id.get_identifier32().get_compact());
      if (it != m_tile_region_map.end() ) return (*it).second; else return NULL;
    }
  
  tile_region_const_iterator tile_region_begin() const                       // Region begin()
    { return m_tile_region_vec.begin(); }

  tile_region_const_iterator tile_region_end() const                         // Region end()
    { return m_tile_region_vec.end(); }

  tile_region_vec_size tile_region_size() const                              // Region size()
    { return m_tile_region_vec.size(); }


  //  Access to Tile Descriptors
  tile_descr_const_iterator tile_descriptors_begin() const                   // Descriptor begin()
    { return m_tile_descr_vec.begin(); }

  tile_descr_const_iterator tile_descriptors_end() const                     // Descriptor end()
    { return m_tile_descr_vec.end(); }

  tile_descr_vec_size tile_descriptors_size() const                          // Descriptor size()
    { return m_tile_descr_vec.size(); }


  //  Access to Calo Descriptors
  calo_descr_const_iterator calo_descriptors_begin() const                   // Calo descr begin()
    { return m_calo_descr_vec.begin(); }

  calo_descr_const_iterator calo_descriptors_end() const                     // Calo descr end()
    { return m_calo_descr_vec.end(); }

  calo_descr_vec_size calo_descriptors_size() const                          // Calo descr size()
    { return m_calo_descr_vec.size(); }

  CaloDetDescriptor* get_calo_descriptor(CaloCell_ID::CaloSample sample, int side) const //
    { scalo_descr_map::const_iterator it = m_calo_descr_map.find((int)sample*side);
      if (it != m_calo_descr_map.end() ) return (*it).second; else return NULL;
    }


  //  Access to Tile Modules
  calo_descr_const_iterator tile_module_begin() const                        // Module begin()
    { return m_tile_module_vec.begin(); }

  calo_descr_const_iterator tile_module_end() const                          // Module end()
    { return m_tile_module_vec.end(); }

  calo_descr_vec_size tile_module_size() const                               // Module size()
    { return m_tile_module_vec.size(); }

  CaloDetDescriptor* get_module_element(const IdentifierHash module_hash) const   // GetElement (Hash)
    { return m_tile_module_vec[(unsigned int)module_hash]; }

  CaloDetDescriptor* get_module_element(const Identifier& module_id) const        // GetElement (ID)
    { calo_descr_map::const_iterator it = m_tile_module_map.find(module_id.get_identifier32().get_compact()); 
      if (it != m_tile_module_map.end() ) return (*it).second; else return NULL;
    }


  //  Access to Tile Cells
  calo_element_const_iterator tile_cell_begin() const                        // Cell begin()
    { return m_tile_cell_vec.begin(); }

  calo_element_const_iterator tile_cell_end() const                          // Cell end()
    { return m_tile_cell_vec.end(); }

  calo_element_vec_size tile_cell_size() const                               // Cell size()
    { return m_tile_cell_vec.size(); }

  CaloDetDescrElement* get_cell_element(unsigned int cell_hash) const             // GetCell (CellHash)
    { return m_tile_cell_vec[cell_hash]; }

  CaloDetDescrElement* get_cell_element(const IdentifierHash cell_hash) const     // GetCell (Hash)
    { return m_tile_cell_vec[(unsigned int)cell_hash]; }

  CaloDetDescrElement* get_cell_element(const Identifier& cell_id) const          // GetCell (ID)
    { calo_element_map::const_iterator it = m_tile_cell_map.find(cell_id.get_identifier32().get_compact());
      if (it != m_tile_cell_map.end() ) return (*it).second; else return NULL; 
    }


  // Access TileCellDim-s
  TileCellDim* get_cell_dim(const Identifier& cell_id) const;

  //  Access to IdHelpers
  const TileID* get_id() const { return m_tile_id;}  
  const TileHWID* get_hwid() const { return m_tile_hwid;}

  // Print
  void print() const;
  
  // Create elements  
  void create_elements();
  void create_elements(bool checks);

  //  Insertion
  void add(TileDetDescrRegion* region);
  void add(TileDetDescriptor* descriptor);
  void add_calodescr(CaloDetDescriptor* descriptor);
  void add_module(IdentifierHash idhash, CaloDetDescriptor* module);
  void add_cell(CaloDetDescrElement* cell);
  void add_cellDim(int section, int side, int tower, int sample, TileCellDim* cellDim);

  void set_helper(const TileID* id){ m_tile_id = id; }
  void set_helper(const CaloCell_ID* id){ m_cell_id = id; }
  void set_helper(const TileHWID* id){ m_tile_hwid = id; }

 private:  
  const TileDetDescrManager & operator=(const TileDetDescrManager &right);
  TileDetDescrManager(const TileDetDescrManager &right);

  // Delete regions and descriptors
  void clear();

  // ----------- Data members -------------

  // Tree Tops
  std::vector<PVLink> m_treeTops;

  // DB Manager
  TileDddbManager_ptr m_dbManager;

  bool m_elements_created;

  const TileID*       m_tile_id;
  const CaloCell_ID*  m_cell_id;
  const TileHWID*     m_tile_hwid;

  tile_region_vec 	m_tile_region_vec;
  tile_region_map 	m_tile_region_map;
  tile_descr_vec 	m_tile_descr_vec;

  // calo descriptors : 1 map for CaloSample*sign and one unsorted vector
  calo_descr_vec 	m_calo_descr_vec;
  scalo_descr_map 	m_calo_descr_map;

  // modules : 1 map for compact id, 1 vector for hashed
  calo_descr_vec 	m_tile_module_vec;
  calo_descr_map 	m_tile_module_map;

  // cells : 1 map for compact id, 1 vector for hashed
  calo_element_vec 	m_tile_cell_vec;
  calo_element_map 	m_tile_cell_map;

  cell_dim_map          m_cell_dim_map;

  bool                  m_verbose;

  MsgStream * m_log;

  //  z-shift operator-function definitions:
  inline double shiftEta(double eta, double Rcen, double zshift) {
    return eta + zshift * tanh(eta) * tan(2 * atan(exp(-eta))) / Rcen;
  }
};

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h" 
CLASS_DEF(TileDetDescrManager, 2941, 1)
#endif


#endif

