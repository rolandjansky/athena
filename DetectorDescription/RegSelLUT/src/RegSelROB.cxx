//
//   @file    RegSelROB.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2010 M.Sutton (sutt@cern.ch)    
//
//   $Id: RegSelROB.cxx, v0.0   Thu 14 Oct 2010 18:08:00 BST sutt $

#include <cmath>
#include <map>

#include "RegSelLUT/RegSelROB.h"

RegSelROB::RegSelROB(const RegSelModule* m) { 
  m_robID = m->robID();
  m_modules.insert(m);

  m_phiMin = m->phiMin();
  m_phiMax = m->phiMax();
  
  m_rMax = m->rMax();
  m_rMin = m->rMin();
  
  m_zMax = m->zMax();
  m_zMin = m->zMin();

  m_etaMin = m->_etaMin();
  m_etaMax = m->_etaMax();
}


void RegSelROB::addModule(const RegSelModule* m) { 
  if ( m->robID()==m_robID ) { 
    m_modules.insert(m);

    if ( m->phiMin()<phiMin() ) m_phiMin = m->phiMin();
    if ( m->phiMax()>phiMax() ) m_phiMax = m->phiMax();

    if ( m->rMax()>rMax() ) m_rMax = m->rMax();
    if ( m->rMin()<rMin() ) m_rMin = m->rMin();

    if ( m->zMax()>zMax() ) m_zMax = m->zMax();
    if ( m->zMin()<zMin() ) m_zMin = m->zMin();

    if ( m->_etaMin()<etaMin() ) m_etaMin = m->_etaMin();
    if ( m->_etaMax()>etaMax() ) m_etaMax = m->_etaMax();
    
  }
}




RegSelROBList::RegSelROBList(const std::vector<RegSelModule>& modules) { 

  std::map<uint32_t, RegSelROB> robs;

  std::cout << "RegSelSiLUT::fillROBMap()" << std::endl;

  for ( std::vector<RegSelModule>::const_iterator mptr = modules.begin() ; mptr!=modules.end() ; mptr++ ) { 

    //    std::cout << *mptr << std::endl; 

    std::map<uint32_t, RegSelROB>::iterator  robitr = robs.find(mptr->robID());

    std::cout << *mptr << std::endl; 

    if ( robitr != robs.end() ) { 
      //  std::cout << "add to ROB" << mptr->robID() << std::endl;
      robitr->second.addModule(&(*mptr));
    }
    else { 
      //      std::cout << "add new ROB" << mptr->robID() << std::endl;
      robs.insert(  std::map<uint32_t, RegSelROB>::value_type(mptr->robID(),RegSelROB(&(*mptr))) );
    }
  }
  
  std::cout << "roblist size " << robs.size() << std::endl;

  std::map<uint32_t, RegSelROB>::iterator  robitr = robs.begin();

  for ( ; robitr!=robs.end() ; robitr++ ) { 
    std::cout << "RegSelLUT::fillROBMap() " << robitr->second << std::endl; 
  }
}




