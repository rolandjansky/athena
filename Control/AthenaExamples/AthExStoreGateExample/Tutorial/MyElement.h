/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _MyElement_
#define _MyElement_

//sample data class for the Read/Write example
//Notice that is does not inherit from Gaudi ContainedObject

namespace SGTutorial {

  class MyElement {
  
  public: 
    MyElement() {set(0,0);}
    MyElement(float t, int i){set(t,i);}

    void set(float t, int i) { m_time = t; m_channelID = i; }
  
    float time() const { return m_time; }
    int   id() const { return m_channelID; }
    
    bool operator==(const MyElement& rhs) const {
      return m_time == rhs.m_time && m_channelID == rhs.m_channelID;
    }
  
  private:
    float m_time;
    int m_channelID;

  };

#include <iostream>
  template <class OST>
  inline OST& operator<<(OST& ost, const MyElement& rhs) {
    ost << "Time: " << rhs.time() << "  ID: " << rhs.id(); 
    return ost;
  }
  
}

#endif






