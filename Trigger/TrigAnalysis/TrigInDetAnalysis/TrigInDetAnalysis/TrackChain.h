// emacs: this is -*- c++ -*-
//
//   @file    TrackChain.h        
//
//            chain container, contains rois       
//   
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrackChain.h, v0.0   Mon  1 Feb 2010 11:43:51 GMT sutt $


#ifndef __TRACKCHAIN_H
#define __TRACKCHAIN_H

#include <iostream>
#include <vector>
#include <string>

#include "TrigInDetAnalysis/TrackRoi.h"

#include "TObject.h"



class TrackChain : public TObject {

public:

  TrackChain() { } 
  TrackChain(const std::string& name) : m_name(name) { } 

  virtual ~TrackChain();

  // accessors and roi manipulation
  const std::string& name() const { return m_name; } 

  // all the following could be avoided by simply 
  // inheriting from an std::vector<TrackRoi> !! 

  /// add and roi by root dictionary class
  void addRoi(TrackRoi& roi)            { m_roi.push_back(roi); }
  
  /// add roi from TrigInDetAnalysis class
  void addRoi(const TIDARoiDescriptor& roi) { m_roi.push_back(TrackRoi(roi)); }
  
  /// get back roi vector
  const std::vector<TrackRoi>& rois() const { return m_roi; } 
  std::vector<TrackRoi>&       rois()       { return m_roi; } 

  /// number of rois
  unsigned size() const { return m_roi.size(); }

  // clear the event
  void clear() { m_roi.clear(); } 

  // return a refer to the last roi in the chain
  TrackRoi& back() { return m_roi.back(); }

  TrackRoi& operator[](int i) { return m_roi.at(i); }

private:

  std::string            m_name;
  std::vector<TrackRoi>  m_roi;

  ClassDef(TrackChain,1)
  
};


inline std::ostream& operator<<( std::ostream& s, const TrackChain& c ) { 
  s << "\tChain " << c.name();
  for ( unsigned i=0 ; i<c.rois().size() ; i++ ) s << "\n\t" << c.rois()[i];
  return s;
}

#endif  // __TRACKCHAIN_H 










