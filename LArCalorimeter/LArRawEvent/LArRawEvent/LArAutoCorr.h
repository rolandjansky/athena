/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARAUTOCORR_H
#define LARAUTOCORR_H
/********************************************************************
 
 NAME:     LArAutoCorr.h
 PACKAGE:  offline/LArCalorimeter/LArRawEvent
 
 AUTHORS:  M. AHARROUCHE
 CREATED:  Dec. 16, 2003
  
 PURPOSE:  Intermediate object used to handle data
           to be used for calculation of autocorrelation
           elements.
            
 ********************************************************************/
// Include files
#include <string>
#include <vector>
#include <math.h>


class LArAutoCorr
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

  // Autocorrelation matrix
  std::vector<double>  m_cov;
  std::vector<double>  m_cov_temp;
 
  // Counter of events
  int m_nped; 
  
 public:
  
  // Constructor
  LArAutoCorr()
    {
      m_min = -1;
      m_max = -1;
      m_nped = 0;
    }
 
  // Destructor
  ~LArAutoCorr(){}

  // Reset m_sum, m_matrix and m_nped
  void correl_zero();

  // Set lower value
  void set_min(const short min);
  
  // Set upper value
  void set_max(const short max);

  // Get number of entries
  int get_nentries() const;
 
  // Get mean value
  double get_mean() const;

  // Get rms value
  double get_rms() const;
 
  // Get lower value
  const short & get_min()const;

  // Get uper value
  const short & get_max() const;
  
  // Fill the m_sum and m_matrix vector 
  void add(const std::vector<short>& samples, size_t maxnsamples);

  // Compute the autocorrelation elements
  //MGV implement switch m_normalize to normalize
  const std::vector<double> & get_cov(int normalize, int phys) ;


};

#endif 

