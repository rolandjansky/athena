/* emacs: this is -*- c++ -*- */
/**
 **   @file    RegSelCondData.h        
 **                   
 **            generic wrapper class for RegionSelector (or any)  
 **            class, so we can keep the actual classes pure, 
 **            and free from the predations of athena code
 **
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   $Id: RegSelCondData.h, v0.0   Tue  4 Feb 2020 15:25:00 CET sutt $
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef  REGSELCONDDATA_H
#define  REGSELCONDDATA_H

#include <iostream>
#include <memory>


template<typename T>
class RegSelCondData {

public:

  RegSelCondData( std::unique_ptr<T> t ) : m_payload(std::move(t)) { } 
						
  virtual ~RegSelCondData() { } 
  
  const T* payload() const { return m_payload.get(); } /// could return the actual payload reference, but have everything using pointers 
 
protected:

  std::unique_ptr<T>  m_payload;

};


template<typename T> 
inline std::ostream& operator<<( std::ostream& s, const RegSelCondData<T>& r ) { 
  return s << *(r->payload());
}


#endif  // REGSELCONDDATA_H 

