/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DETELEMHASH_H
#define LUCID_DETELEMHASH_H

class Identifier;
class IdHelper;
class IdContext;

// based on the Muon DetEkemHash class

class LUCID_DetElemHash 
{
 public:
    
  LUCID_DetElemHash();
  LUCID_DetElemHash(const LUCID_DetElemHash& other);
  ~LUCID_DetElemHash();
    
  typedef Identifier ID;
  
  int max() const; 

 private:

  const IdHelper* m_helper;
  IdContext* m_context;
  int m_max;
  

};


#endif
