/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPEDESTALCUMUL_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPEDESTALCUMUL_H

#include <string>
#include <vector>
#include <math.h>

/** 
 *  Intermediate Class to store pedestal data
 *  from the L1CaloPedestalMaker algorithm
 *
 *  Heavily inspired from the LArPedestal class
 *  
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class L1CaloPedestalCumul
{
 private:

  // Lower bound of window
  short m_min;
  // Upper bound of window
  short m_max;
  // Sum of components
  std::vector<double> m_sum;
  // Sum of squares
  std::vector<double> m_matrix;
  // Event counter
  int m_nped; 
 
 public:
  
  // Constructor
  L1CaloPedestalCumul();
  // Destructor
  ~L1CaloPedestalCumul();

  // Reset m_sum, m_matrix and m_nped
  void clear();

  // Set lower value
  void set_min(const short min) { m_min = min; };
  
  // Set upper value
  void set_max(const short max) { m_max = max; };

  // Get number of entries
  inline int get_nentries() const { return m_nped; }

  // Get sum
  double get_sum() const;
  double get_sum(const unsigned isample) const;

  // Get mean value
  double get_mean() const;
  double get_mean(const unsigned isample) const;

  // Get rms value
  double get_rms() const;
  double get_rms(const unsigned isample) const;

  // Get number of samples
  inline unsigned get_nsamples() const { return m_sum.size();}
  
  // Get lower value
  inline const short& get_min() const { return m_min; }

  // Get uper value
  inline const short& get_max() const { return m_max; }
  
  // Fill the m_sum and m_matrix vector 
  void add(const std::vector<int> samples);

};
 
#endif 

