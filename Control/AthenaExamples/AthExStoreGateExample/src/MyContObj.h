/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXSTOREGATEEXAMPLE_MYCONTOBJ_H
#define ATHEXSTOREGATEEXAMPLE_MYCONTOBJ_H

//sample data class for the Read/Write example
//Notice that is does not inherit from Gaudi ContainedObject

class MyContObj {
  
public: 
  MyContObj(float t, int i){set(t,i);}
  MyContObj(){set(0,0);}
  ~MyContObj(){}

  void set(float t, int i) { m_time = t; m_channelID = i; }
  
  float time() const { return m_time; }
  int   id() const { return m_channelID; }
  
 private:
  float m_time;
  int m_channelID;

};

#endif
