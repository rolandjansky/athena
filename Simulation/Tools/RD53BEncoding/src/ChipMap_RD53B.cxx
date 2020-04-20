#include "./ChipMap_RD53B.h"

#include <math.h>
#include <bitset>
#include <tuple>
// #include <iostream> 

void ChipMap_RD53B::fillChipMap(int eta, int phi, int tot) {
  eta = correctEta(eta);
  if (eta<0) return;
  phi = correctPhi(phi);
  if (phi<0) return;
  
  m_fired_pixels.at(eta).at(phi) = true;
  int mytot = tot/16.+1;
  m_tots.at(eta).at(phi) = mytot>15 ? 15 : mytot;
  
  // TESTING
//   m_tots.at(eta).at(phi) = tot;

  m_nfired_pixels++;
}

int ChipMap_RD53B::getCoreIndex(int eta, int phi) {
  // check if eta and phi are valids, if not return a not valid core
  eta = correctEta(eta);
  if (eta<0) return -1;
  phi = correctPhi(phi);
  if (phi<0) return -1;
  
  int myccol = getCcol(eta);
  int mycrow = getCrow(phi);
  
  return getCore(myccol, mycrow);  
}


int ChipMap_RD53B::getCcol(int eta) {
  return int(floor(eta/float(m_cols_core)));
}

int ChipMap_RD53B::getCrow(int phi) {
  return int(floor(phi/float(m_rows_core)));
}

int ChipMap_RD53B::getCore(int ccol, int crow) {
  return (crow*m_ccols+ccol);
}

int ChipMap_RD53B::getTotalChannels() {
  return m_cols*m_rows; 
}

int ChipMap_RD53B::correctEta(int eta) {
  if (eta < (m_cols+m_n_colsbtwchips) and eta > (m_cols-1)) return -1;
  if (eta > (m_cols-1)) return eta-(m_cols+m_n_colsbtwchips);    
  return eta;
}

int ChipMap_RD53B::correctPhi(int phi) {
  if (phi < (m_rows+m_n_rowsbtwchips) and phi > (m_rows-1)) return -1;    
  if (phi > (m_rows-1)) return phi-(m_rows+m_n_rowsbtwchips);
  return phi;
}

void ChipMap_RD53B::fillCores() {
  if (m_is_filled) return;  
  for (unsigned int eta = 0; eta < m_fired_pixels.size(); eta++) {
    for (unsigned int phi = 0; phi < m_fired_pixels.at(eta).size(); phi++) {
      if (not m_fired_pixels.at(eta).at(phi)) continue;
      int myccol = getCcol(eta);
      int mycrow = getCrow(phi);

      if (not m_fired_ccol_crows.at(myccol).at(mycrow)) {
        m_fired_crows_per_ccol.at(myccol)+=1;
        m_nfired_cores+=1;        
      }
      m_fired_ccol_crows.at(myccol).at(mycrow) = true;
      m_hits_per_core.at(getCore(myccol, mycrow))+=1;
    }
  }
  m_is_filled = true;    
}

int ChipMap_RD53B::getCoreBits() {
  int bits = 0;
  for (int myccol = 0; myccol<m_ccols; myccol++) {
    bool isfirst_ccol = true;
    int previous_row = 0;
    for (int mycrow = 0; mycrow<m_crows; mycrow++) {      
      if (not m_fired_ccol_crows.at(myccol).at(mycrow)) continue;
      if (isfirst_ccol) {
        // add the 6 bits for the ccol
        bits += 6;
        isfirst_ccol = false;
      }      
      // add the 8 bits for the crow if current crow is not neighbor of previous crow
      if (previous_row!=(mycrow-1))
        bits += 8;      
      // add 2 bits for islast and isneighbor bits
      bits +=2;      
      // update the previous_row to the current
      previous_row = mycrow;
    }
  }
  return bits;
}

int ChipMap_RD53B::getToTBits(int bits_for_tot) {
  int tot = 0;
  for (auto& hits : m_hits_per_core)
    tot+=bits_for_tot*hits;
  return tot;  
}

int ChipMap_RD53B::getToTBitsCore(int core, int bits_for_tot) {
  return (bits_for_tot*m_hits_per_core.at(core));
//   //TESTING
//   return 5;
}

std::string ChipMap_RD53B::isHalfFired(int min_eta, int max_eta,
                                 int min_phi, int max_phi) {
  for (int eta = min_eta; eta<= max_eta; eta++) {
    for (int phi = min_phi; phi<= max_phi; phi++) {
      if (m_fired_pixels.at(eta).at(phi)) return "1";
    }
  }
  return "0";
}

float ChipMap_RD53B::getBitTreeLength(bool do_compression) {
  float result = 0.;
  for (int myccol = 0; myccol<m_ccols; myccol++) {
    for (int mycrow = 0; mycrow<m_crows; mycrow++) {
      result+=getBitTree(myccol, mycrow, do_compression);
    }
  }
  return result;
}

float ChipMap_RD53B::getBitTree(int myccol, int mycrow, bool do_compression) {    
    
    float result = 0.;
    
    if (not m_fired_ccol_crows.at(myccol).at(mycrow))
      return result;
    
//     //TESTING
//     return 5.;
    
    int min_eta = myccol*m_cols_core; 
    int max_eta = (myccol+1)*m_cols_core-1;
    
    int min_phi = mycrow*m_rows_core; 
    int max_phi = (mycrow+1)*m_rows_core-1;
    
    int loop = 0;
    
    std::vector< std::tuple < int, int, int, int> > good_extremes = {std::make_tuple(min_eta,max_eta, min_phi, max_phi)};
    
    // the bit tree works in 4 steps 
    // 1) split right/left 
    // 2) split top/botton
    // 3) split right/left 
    // 4) split right/left --> this last step provides the 2-bit maps of all the hit pairs
    // for 25x100 it is as follows:
    // 1) split top/botton
    // 2) split right/left 
    // 3) split top/botton
    // 4) split right/left --> this last step provides the 2-bit maps of all the hit pairs
    
    
    if (m_use50x50) {
      do {
        loop++;
        // this returns directly the correct fired halves or quarters
        std::string new_result = "";
        good_extremes = (loop!=2) ? doSplit(good_extremes, new_result, do_compression, true) : doSplit(good_extremes, new_result, do_compression, false);    
        result+=float(new_result.length());
      } while (loop<4);
    } else {
      do {
        loop++;
        // this returns directly the correct fired halves or quarters
        std::string new_result = "";
        good_extremes = (loop%2==0) ? doSplit(good_extremes, new_result, do_compression, true) : doSplit(good_extremes, new_result, do_compression, false);    
        result+=float(new_result.length());
      } while (loop<4);
    }
    return result;
}
  
std::string ChipMap_RD53B::getBitTreeString(int myccol, int mycrow, bool do_compression) {
    
    std::string result = "";
    
    if (not m_fired_ccol_crows.at(myccol).at(mycrow))
      return result;
    
    int min_eta = myccol*m_cols_core; 
    int max_eta = (myccol+1)*m_cols_core-1;
    
    int min_phi = mycrow*m_rows_core; 
    int max_phi = (mycrow+1)*m_rows_core-1;
    
    int loop = 0;
    
    std::vector< std::tuple < int, int, int, int> > good_extremes = {std::make_tuple(min_eta,max_eta, min_phi, max_phi)};
    
    // the bit tree works in 4 steps 
    // 1) split right/left 
    // 2) split top/botton
    // 3) split right/left 
    // 4) split right/left --> this last step provides the 2-bit maps of all the hit pairs
    
    do {
    loop++;
    // this returns directly the correct fired halves or quarters
    std::string new_result = "";
    good_extremes = (loop!=2) ? doSplit(good_extremes, new_result, do_compression, true) : doSplit(good_extremes, new_result, do_compression, false);   
    result+=new_result;
    } while (loop<4);
    
    return result;
}

std::string ChipMap_RD53B::getToTBitsString(int myccol, int mycrow, std::vector<int>& tots) {
    std::string result = "";
    
    if (not m_fired_ccol_crows.at(myccol).at(mycrow))
      return result;
    
    int min_eta = myccol*m_cols_core; 
    int max_eta = (myccol+1)*m_cols_core-1;
    
    int min_phi = mycrow*m_rows_core; 
    int max_phi = (mycrow+1)*m_rows_core-1;
    
    for (int eta=min_eta; eta<=max_eta; eta++) {
      for (int phi=min_phi; phi<=max_phi; phi++) {
        tots.push_back(m_tots.at(eta).at(phi));
        if (m_tots.at(eta).at(phi)==0) continue;
        std::bitset<4> tot;
        tot |= m_tots.at(eta).at(phi);
        result += tot.to_string();
      }      
    }    
    return result;
}

  
std::vector< std::tuple < int, int, int, int> > ChipMap_RD53B::doSplit(std::vector< std::tuple < int, int, int, int> > extremes, 
                                                                 std::string& new_result, bool do_compression, bool do_leftRight) {
    
    std::vector< std::tuple < int, int, int, int> > new_extremes;
    for (auto & extreme: extremes) {
      int current_eta_min = std::get<0>(extreme);
      int current_eta_max = std::get<1>(extreme);
      int average_eta = (current_eta_max+current_eta_min+1)*0.5;
      
      int current_phi_min = std::get<2>(extreme);
      int current_phi_max = std::get<3>(extreme);
      int average_phi = (current_phi_max+current_phi_min+1)*0.5;
      
      int new_eta_min = current_eta_min;
      int new_eta_max = average_eta-1;      
      int new_phi_min = current_phi_min;
      int new_phi_max = current_phi_max;
      
      // change the extremes if you split top-bottom instead of right-left
      if (not do_leftRight) {
        new_eta_max = current_eta_max;
        new_phi_min = average_phi;
      }        
      
      std::string temp_result = "";
      if (isHalfFired(new_eta_min, new_eta_max, new_phi_min, new_phi_max) == "1") {
        new_extremes.push_back(std::make_tuple(new_eta_min, new_eta_max, new_phi_min, new_phi_max));
        temp_result+="1";
      } else temp_result+="0";  
      
      new_eta_min = average_eta;
      new_eta_max = current_eta_max;      
      new_phi_min = current_phi_min;
      new_phi_max = current_phi_max;
      
      // change the extremes if you split top-bottom instead of right-left
      if (not do_leftRight) {
        new_eta_min = current_eta_min;
        new_phi_max = average_phi-1;
      }
      
      if (isHalfFired(new_eta_min, new_eta_max, new_phi_min, new_phi_max) == "1") {
        new_extremes.push_back(std::make_tuple(new_eta_min, new_eta_max, new_phi_min, new_phi_max));
        temp_result+="1";
      } else temp_result+="0";  
      
      if (do_compression and temp_result=="01") temp_result="0";
      
      new_result+=temp_result;        
    }
    return new_extremes;
}
