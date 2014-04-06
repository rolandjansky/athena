/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
// TopInputMapTrigger - Template class to hold map of ElementLink<Container> and TopOverlapTrigger

#ifndef TOP_INPUT_MAP_TRIGGER_H
#define TOP_INPUT_MAP_TRIGGER_H

#include <vector>
#include <string>
#include "DataModel/ElementLink.h"
#include "TopInputs/TopOverlapTrigger.h"
#include "SGTools/CLASS_DEF.h"

namespace TopRec {

  template<class Container>
  class TopInputMapTrigger : public std::map< ElementLink<Container>, TopOverlapTrigger >
  {
   public:
    TopInputMapTrigger();
    //TopInputMapTrigger(std::string);
    virtual ~TopInputMapTrigger() {;}
  };
  
  template<class Container>
  TopInputMapTrigger<Container>::TopInputMapTrigger() 
    : std::map< ElementLink<Container>, TopOverlapTrigger > () {;}
  
  //template<class Container>
  //TopInputMapTrigger<Container>::TopInputMapTrigger(std::string) 
    //: std::map< ElementLink<Container>, TopOverlapTrigger > () {;}
  
} // end namespace

#endif // TOP_INPUT_MAP_TRIGGER_H
