/* emacs: this is -*- c++ -*- */
/**
 **     @file    TIDAChain.h
 **
 **     @brief   chain container, contains rois       
 **
 **     @author  mark sutton
 **     @date    Mon  1 Feb 2010 11:43:51 GMT 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDA_CHAIN_H
#define TIDA_CHAIN_H

#include <iostream>
#include <vector>
#include <string>

#include "TrigInDetAnalysis/TIDARoi.h"

#include "TObject.h"


namespace TIDA { 

class Chain : public TObject {

public:

  Chain() { } 
  Chain(const std::string& name) : m_name(name) { } 

  virtual ~Chain();

  // accessors and roi manipulation
  const std::string& name() const { return m_name; } 

  // all the following could be avoided by simply 
  // inheriting from an std::vector<TIDA::Roi> !! 

  /// add and roi by root dictionary class
  void addRoi(TIDA::Roi& roi)            { m_roi.push_back(roi); }
  
  /// add roi from TrigInDetAnalysis class
  void addRoi(const TIDARoiDescriptor& roi) { m_roi.push_back(TIDA::Roi(roi)); }
  
  /// get back roi vector
  const std::vector<TIDA::Roi>& rois() const { return m_roi; } 
  std::vector<TIDA::Roi>&       rois()       { return m_roi; } 

  /// number of rois
  unsigned size() const { return m_roi.size(); }

  // clear the event
  void clear() { m_roi.clear(); } 

  // return a refer to the last roi in the chain
  TIDA::Roi& back() { return m_roi.back(); }

  TIDA::Roi& operator[](int i) { return m_roi.at(i); }

private:

  std::string            m_name;
  std::vector<TIDA::Roi>  m_roi;

  ClassDef(TIDA::Chain,1)
  
};

}

inline std::ostream& operator<<( std::ostream& s, const TIDA::Chain& c ) { 
  s << "\tChain " << c.name() << "\trois: " << c.rois().size();
  for ( unsigned i=0 ; i<c.rois().size() ; i++ ) s << "\n\t" << c.rois()[i];
  return s;
}

#endif  // TIDA_CHAIN_H 










