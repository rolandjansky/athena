/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOGPU_GeoLoadGpu_H
#define ISF_FASTCALOGPU_GeoLoadGpu_H

// This header can be use both gcc and nvcc host part

#include <map>
#include <vector>

#include "GeoGpu_structs.h"

typedef std::map<Identifier_Gpu, const CaloDetDescrElement_Gpu*> t_cellmap_Gpu;

class GeoLoadGpu {
public:
  GeoLoadGpu() = default;

  ~GeoLoadGpu() {
    delete m_cellid_array;
  }

  void                       set_ncells( unsigned long nc ) { m_ncells = nc; };
  void                       set_nregions( unsigned int nr ) { m_nregions = nr; };
  void                       set_cellmap( t_cellmap_Gpu* cm ) { m_cells = cm; };
  t_cellmap_Gpu* set_cellmap( ) { return m_cells ; };
  void                       set_regions( GeoRegion* r ) { m_regions = r; };
  void                       set_g_regions( GeoRegion* gr ) { m_regions_d = gr; };
  void                       set_cells_g( CaloDetDescrElement_Gpu* gc ) { m_cells_d = gc; };
  void                       set_max_sample( int s ) { m_max_sample = s; };
  void                       set_sample_index_h( Rg_Sample_Index* s ) { m_sample_index_h = s; };
  const CaloDetDescrElement_Gpu* index2cell( unsigned long index ) { return ( *m_cells )[m_cellid_array[index]]; };

  t_cellmap_Gpu*                 get_cellmap( ) { return m_cells; };
  bool LoadGpu() {
    return LoadGpu_cu();
  }
  bool LoadGpu_kk();
  bool LoadGpu_cu();

  void    set_geoPtr( GeoGpu* ptr ) { m_geo_d = ptr; }
  GeoGpu* get_geoPtr() const { return m_geo_d; }

  unsigned long get_ncells() const { return m_ncells; }

  // bool LoadGpu_Region(GeoRegion * ) ;

private:
  bool TestGeo();
  bool SanityCheck();

protected:
  unsigned long        m_ncells{0};         // number of cells
  unsigned int         m_nregions{0};       // number of regions
  t_cellmap_Gpu*           m_cells{0};          // from Geometry class
  GeoRegion*           m_regions{0};        // array of regions on host
  GeoRegion*           m_regions_d{0};      // array of region on GPU
  CaloDetDescrElement_Gpu* m_cells_d{0};        // Cells in GPU
  Identifier_Gpu*          m_cellid_array{0};   // cell id to Indentifier lookup table
  int                  m_max_sample{0};     // Max number of samples
  Rg_Sample_Index*     m_sample_index_h{0}; // index for flatout of  GeoLookup over sample

  GeoGpu* m_geo_d{0};

};
#endif
