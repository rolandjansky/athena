/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

*/

#ifndef RD53B_CHIPMAP_H
#define RD53B_CHIPMAP_H

#include <string>
#include <vector>

class ChipMap_RD53B {
  
public: 
  //// This is the chip map constructor:
  //   - cols: number of columns in the chip
  //   - rows: number of rows in the chip
  //   - use50x50: boolean to chose the pixel dimension, this triggers the needed quarter-core dimension
  ChipMap_RD53B(int cols, int rows, bool use50x50=true):
          m_cols(cols), m_rows(rows),
          m_use50x50(use50x50)
          {
            // the definition of the cols_core and rows_core depends on the pitch size
            // such that the core area is the same
            // In the RD53B manual you have:
            // - 8 x 2 (cols x rows) quarter cores for 50x50 pixels
            // - 4 x 4 (cols x rows) quarter cores for 100x25 pixels
            if (m_use50x50) {
             m_cols_core = 8;
             m_rows_core = 2;             
            } else {
              m_cols_core = 4;
              m_rows_core = 4;
            }
            
            m_ccols = m_cols/m_cols_core;
            m_crows = m_rows/m_rows_core;            
            m_fired_pixels =  std::vector < std::vector < bool > >(m_cols, std::vector < bool > (m_rows, false) );
            m_tots =  std::vector < std::vector < int > >(m_cols, std::vector < int > (m_rows, 0) );
            m_fired_ccol_crows =  std::vector < std::vector < bool > >(m_ccols, std::vector < bool > (m_crows, false) );
            m_hits_per_core = std::vector<int>(m_ccols*m_crows, 0);
            m_fired_crows_per_ccol = std::vector<int>(m_ccols, 0);            
            
          };

  ~ChipMap_RD53B() {};
  
  void fillChipMap(int eta, int phi, int tot);
  void fillCores();
  bool isFilled() {return m_is_filled;}
  int getFiredCores() {return m_nfired_cores;}
  int getFiredPixels() {return m_nfired_pixels;}
  int getCcols() {return m_ccols;}
  int getCrows() {return m_crows;}
  int getCcolsCore() {return m_cols_core;}
  int getCrowsCore() {return m_rows_core;}  
  int getHitsAtCore(int core) {return m_hits_per_core.at(core);}
  std::vector<int> getHitsPerCore() {return m_hits_per_core;}
  std::vector<int> getCrowsPerCcol() {return m_fired_crows_per_ccol;}
  std::vector < std::vector < bool > > getCrowsCcol() {return m_fired_ccol_crows;}
  int getCoreIndex(int eta, int phi);
  int getCcol(int eta);
  int getCrow(int phi);
  int getCore(int ccol, int crow);
  int getTotalChannels();
  int getCoreBits();
  int getToTBits(int bits_for_tot=4);
  int getToTBitsCore(int core, int bits_for_tot=4);
  float getBitTreeLength(bool do_compression = true);
  float getBitTree(int myccol, int mycrow, bool do_compression = true);
  std::string getBitTreeString(int myccol, int mycrow, bool do_compression=true);
  std::string getToTBitsString(int myccol, int mycrow, std::vector<int>& tots);
  
private:
  int m_cols;
  int m_rows;
  int m_cols_core;
  int m_rows_core;
  int m_n_colsbtwchips;
  int m_n_rowsbtwchips;
  bool m_use50x50;
  int m_ccols;
  int m_crows;
  int m_nfired_cores = 0;
  int m_nfired_pixels = 0;
  bool m_is_filled = false;
  
  std::vector<int>               m_hits_per_core;  
  std::vector<int>               m_fired_crows_per_ccol;  
  std::vector<std::vector<bool>> m_fired_ccol_crows;
  std::vector<std::vector<bool>> m_fired_pixels;
  std::vector<std::vector<int>>  m_tots;
  
  std::vector< std::tuple < int, int, int, int> > doSplit(std::vector< std::tuple < int, int, int, int> > extremes, 
                                                          std::string& new_result, bool do_compression, bool do_rightLeft = true);
  std::string isHalfFired(int min_eta, int max_eta,
                          int min_phi, int max_phi);
  
};


#endif // RD53B_CHIPMAP_H
