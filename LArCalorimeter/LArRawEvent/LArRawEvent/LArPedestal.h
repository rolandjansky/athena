/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARPEDESTAL_H
#define LARPEDESTAL_H
/********************************************************************
 
 NAME:     LArPedestal.h
 PACKAGE:  offline/LArCalorimeter/LArRawEvent
 
 AUTHORS:  M. AHARROUCHE
 CREATED:  Jan. 12, 2004
 UPDATED:  Mar. 10, 2004 by Remi Lafaye
  
 PURPOSE:  Interchanges the data with LArPedestalMaker for 
           calculation of pedestal and rms.
            
 ********************************************************************/
// Include files
#include <string>
#include <vector>
#include <math.h>
#include <stdint.h>

class LArPedestal
{
 private:

  // Lower bound of window
  short m_min;
  // Upper bound of window
  short m_max;
  // Sum of components
  std::vector<uint32_t> m_sum;
  // Sum of squares
  std::vector<uint64_t> m_sumSquares;
  // Event counter
  uint32_t m_nped; 
 
 public:
  
  // Constructor
  LArPedestal();
  // Destructor
  ~LArPedestal();

  // Reset m_sum, m_matrix and m_nped
  void zero();

  // Set lower value
  void set_min(const short min);
  
  // Set upper value
  void set_max(const short max);

  // Get number of entries
  inline int get_nentries() const { return m_nped; }

  // Get sum
  double get_sum() const;
  double get_sum(const unsigned isample) const;

  // Get mean value
  double get_mean() const;
  double get_mean(const unsigned isample) const;

  double get_mean(const unsigned isample_min, const unsigned isample_max) const;

   // Get rms value
   double get_rms() const;
   double get_rms(const unsigned isample) const;
   double get_rms(const unsigned isample_min, const unsigned isample_max) const;


  // Get number of samples
  inline unsigned get_nsamples() const { return m_sum.size();}
  
  // Get lower value
  inline const short& get_min() const { return m_min; }

  // Get uper value
  inline const short& get_max() const { return m_max; }
  
  // Fill the m_sum and m_matrix vector 
  void add(const std::vector<short>& samples);

};
 
#endif 

