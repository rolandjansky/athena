#include <iostream>
#include "ISF_FastCaloGpu/GeoLoadGpu.h"

__global__ void testHello() { printf( "Hello, I am from GPU thread %d\n", threadIdx.x ); }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

__global__ void testCell( CaloDetDescrElement_Gpu* cells, unsigned long index ) {
  CaloDetDescrElement_Gpu* cell   = &cells[index];
  int                  sample = cell->getSampling();
  float                eta    = cell->eta();
  float                phi    = cell->phi();

  long long hashid = cell->calo_hash();

  printf( " From GPU cell index %ld , hashid=%ld, eta=%f, phi=%f, sample=%d \n", index, hashid, eta, phi, sample );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

__global__ void testGeo( CaloDetDescrElement_Gpu* cells, GeoRegion* regions, unsigned int nregions, unsigned long ncells,
                         int r, int ir, int ip ) {

  int                neta  = regions[r].cell_grid_eta();
  int                nphi  = regions[r].cell_grid_phi();
  unsigned long long index = regions[r].cell_grid_g()[ir * nphi + ip];
  printf( " From GPU.., region %d, cell_grid[%d][%d]: [%d][%d] index=%lu \n", r, ir, ip, neta, nphi, index );

  CaloDetDescrElement_Gpu* c = &cells[index];

  long long hashid = c->calo_hash();
  long long id     = c->identify();
  int       sample = c->getSampling();
  float     eta    = c->eta();
  float     phi    = c->phi();

  printf( " From GPU.., region %d, cell_grid[%d][%d]: index %lu index, hashid=%ld,eta=%f, phi=%f, sample=%d , ID=%ld "
          "cell_ptr=%#015lx \n",
          r, ir, ip, index, hashid, eta, phi, sample, id, regions[r].all_cells() );

  CaloDetDescrElement_Gpu cc = ( regions[r].all_cells() )[index];
  printf( " GPU test region have cells: cell index %lu, eta=%f phi=%f size of cell*GPU=%lu\n", index, cc.eta(),
          cc.phi(), sizeof( CaloDetDescrElement_Gpu* ) );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

__global__ void testGeo_g( GeoGpu* geo, int r, int ir, int ip ) {

  printf("start testGeo_g");
  GeoRegion*           regions = ( *geo ).regions;
  CaloDetDescrElement_Gpu* cells   = geo->cells;

  int                neta  = regions[r].cell_grid_eta();
  int                nphi  = regions[r].cell_grid_phi();
  unsigned long long index = regions[r].cell_grid_g()[ir * nphi + ip];
  printf( " From GPU.., region %d, cell_grid[%d][%d]: [%d][%d] index=%ld \n", r, ir, ip, neta, nphi, index );

  CaloDetDescrElement_Gpu* c = &cells[index];

  long long hashid = c->calo_hash();
  long long id     = c->identify();
  int       sample = c->getSampling();
  float     eta    = c->eta();
  float     phi    = c->phi();

  printf( " From GPU.., region %d, cell_grid[%d][%d]: index %lu index, hashid=%ld,eta=%f, phi=%f, sample=%d , ID=%ld "
          "cell_ptr=%#015lx \n",
          r, ir, ip, index, hashid, eta, phi, sample, id, regions[r].all_cells() );

  CaloDetDescrElement_Gpu cc = ( regions[r].all_cells() )[index];
  printf( " GPU test region have cells: cell index %llu, eta=%f phi=%f size of cell*GPU=%lu\n", index, cc.eta(),
          cc.phi(), sizeof( CaloDetDescrElement_Gpu* ) );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool GeoLoadGpu::TestGeo() {
  testGeo<<<1, 1>>>( m_cells_d, m_regions_d, m_ncells, m_nregions, 14, 0, 32 );
  cudaDeviceSynchronize();
  cudaError_t err = cudaGetLastError();
  if ( err != cudaSuccess ) {
    std::cout << cudaGetErrorString( err ) << std::endl;
    return false;
  }

  testGeo_g<<<1, 1>>>( m_geo_d, 14, 0, 32 );
  cudaDeviceSynchronize();
  err = cudaGetLastError();
  if ( err != cudaSuccess ) {
    std::cout << cudaGetErrorString( err ) << std::endl;
    return false;
  }

  long long* c   = m_regions[14].cell_grid();
  int        np  = m_regions[14].cell_grid_phi();
  int        ne  = m_regions[14].cell_grid_eta();
  int        idx = c[0 * np + 32];
  Identifier_Gpu Id  = m_cellid_array[idx];
  std::cout << "From Host: Region[14]Grid[0][32]: index=" << idx << ", ID=" << Id
            << ", HashCPU=" << ( *m_cells )[Id]->calo_hash() << ", neta=" << ne << ",  nphi=" << np
            << ", eta=" << ( *m_cells )[Id]->eta() << std::endl;

  return true;
  // end test
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool GeoLoadGpu::SanityCheck() {
  // sanity check/test
  testHello<<<1, 1>>>();
  testCell<<<1, 1>>>( m_cells_d, 1872 );
  cudaDeviceSynchronize();

  Identifier_Gpu Id = m_cellid_array[2000];

  cudaError_t err = cudaGetLastError();
  if ( err != cudaSuccess ) {
    std::cout << cudaGetErrorString( err ) << std::endl;
    return false;
  }
  return true;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool GeoLoadGpu::LoadGpu_cu() {
  if ( !m_cells || m_ncells == 0 ) {
    std::cout << "Geometry is empty " << std::endl;
    return false;
  }

  GeoGpu geo_gpu_h;

  // Allocate Device memory for cells and copy cells as array
  // move cells on host to a array first
  if ( cudaSuccess != cudaMalloc( (void**)&m_cells_d, sizeof( CaloDetDescrElement_Gpu ) * m_ncells ) ) return false;

  CaloDetDescrElement_Gpu* cells_Host = (CaloDetDescrElement_Gpu*)malloc( m_ncells * sizeof( CaloDetDescrElement_Gpu ) );
  m_cellid_array                  = (Identifier_Gpu*)malloc( m_ncells * sizeof( Identifier_Gpu ) );

  // create an array of cell identities, they are in order of hashids.
  int ii = 0;
  for ( t_cellmap_Gpu::iterator ic = m_cells->begin(); ic != m_cells->end(); ++ic ) {
    cells_Host[ii]     = *( *ic ).second;
    Identifier_Gpu id      = ( ( *ic ).second )->identify();
    m_cellid_array[ii] = id;
    ii++;
  }

  if ( cudaSuccess !=
       cudaMemcpy( &m_cells_d[0], cells_Host, sizeof( CaloDetDescrElement_Gpu ) * m_ncells, cudaMemcpyHostToDevice ) )
    return false;

  free( cells_Host );

  if ( 0 ) {
    if ( !SanityCheck() ) { return false; }
  }

  Rg_Sample_Index* SampleIndex_g;
  if ( cudaSuccess != cudaMalloc( (void**)&SampleIndex_g, sizeof( Rg_Sample_Index ) * m_max_sample ) ) return false;

  // copy sample_index array  to gpu
  if ( cudaSuccess != cudaMemcpy( SampleIndex_g, m_sample_index_h, sizeof( Rg_Sample_Index ) * m_max_sample,
                                  cudaMemcpyHostToDevice ) ) {
    std::cout << "Error copy sample index " << std::endl;

    return false;
  }

  // each Region allocate a grid (long Long) gpu array
  //  copy array to GPU
  //  save to regions m_cell_g ;
  for ( unsigned int ir = 0; ir < m_nregions; ++ir ) {
    long long* ptr_g;
    if ( cudaSuccess != cudaMalloc( (void**)&ptr_g, sizeof( long long ) * m_regions[ir].cell_grid_eta() *
                                                        m_regions[ir].cell_grid_phi() ) )
      return false;
    if ( cudaSuccess != cudaMemcpy( ptr_g, m_regions[ir].cell_grid(),
                                    sizeof( long long ) * m_regions[ir].cell_grid_eta() * m_regions[ir].cell_grid_phi(),
                                    cudaMemcpyHostToDevice ) )
      return false;
    m_regions[ir].set_cell_grid_g( ptr_g );
    m_regions[ir].set_all_cells( m_cells_d ); // set this so all region instance know where the GPU cells are, before
  }

  // GPU allocate Regions data  and load them to GPU as array of regions

  if ( cudaSuccess != cudaMalloc( (void**)&m_regions_d, sizeof( GeoRegion ) * m_nregions ) ) return false;
  if ( cudaSuccess != cudaMemcpy( m_regions_d, m_regions, sizeof( GeoRegion ) * m_nregions, cudaMemcpyHostToDevice ) )
    return false;

  geo_gpu_h.cells        = m_cells_d;
  geo_gpu_h.ncells       = m_ncells;
  geo_gpu_h.nregions     = m_nregions;
  geo_gpu_h.regions      = m_regions_d;
  geo_gpu_h.max_sample   = m_max_sample;
  geo_gpu_h.sample_index = SampleIndex_g;

  // Now copy this to GPU and set the static member to this pointer
  cudaMalloc( (void**)&m_geo_d, sizeof( GeoGpu ) );
  cudaMemcpy( m_geo_d, &geo_gpu_h, sizeof( GeoGpu ), cudaMemcpyHostToDevice );

  // more test for region grids
  if ( 0 ) { return TestGeo(); }
  return true;
}
