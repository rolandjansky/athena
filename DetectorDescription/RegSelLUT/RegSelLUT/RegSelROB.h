// emacs: this is -*- c++ -*-
//
//   @file    RegSelROB.h        
//
//                   
//  
//   Copyright (C) 2010 M.Sutton (sutt@cern.ch)    
//
//   $Id: RegSelROB.h, v0.0   Thu 14 Oct 2010 18:06:34 BST sutt $


#ifndef REGSELROB_H
#define REGSELROB_H

#include <iostream>
#include <set>
#include <vector>

#include "RegSelLUT/ZRObject.h"
#include "RegSelLUT/RegSelModule.h"


class RegSelROB : public ZRObject {

public:

  RegSelROB(const RegSelModule* m);

  virtual ~RegSelROB() { } 

  void addModule(const RegSelModule* m);

  double phiMin() const { return m_phiMin; } 
  double phiMax() const { return m_phiMax; } 

  uint32_t robID() const { return m_robID; }

  unsigned size() const { return m_modules.size(); } 

  double etaMin() const { return m_etaMin; }
  double etaMax() const { return m_etaMax; }

protected:

  double caletaMin();
  double caletaMax();

protected:

  uint32_t m_robID;

  double m_phiMin;
  double m_phiMax;

  double m_etaMin;
  double m_etaMax;

  std::set<const RegSelModule*> m_modules;
 
};

inline std::ostream& operator<<( std::ostream& s, const RegSelROB& r ) { 
  return s << "[ robID 0x" << std::hex << r.robID() << std::dec
	   << "\tr: " << r.rMin() << " " << r.rMax()
	   << "\tz: " << r.zMin() << " " << r.zMax()
	   << "\tphi: " << r.phiMin() << " " << r.phiMax() 
	   << "\teta: " << r.etaMin() << " " << r.etaMax() 
	   << "\tsize " << r.size() << " ]";
    
}



class RegSelROBList { 

public:

  RegSelROBList(const std::vector<RegSelModule>& modules);

  virtual ~RegSelROBList() { } 

};






#endif  // __REGSELROB_H 










