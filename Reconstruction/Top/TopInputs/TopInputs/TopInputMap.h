/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
// TopInputMap - Template class to hold map of ElementLink<Container> and bool

#ifndef TOP_INPUT_MAP_H
#define TOP_INPUT_MAP_H

#include <vector>
#include <string>
#include "DataModel/ElementLink.h"
#include "TopInputs/TopOverlap.h"
#include "SGTools/CLASS_DEF.h"

namespace TopRec {

  template<class Container>
  class TopInputMap : public std::map< ElementLink<Container>, TopOverlap >
  {
  public:
    TopInputMap();
    //TopInputMap(std::string);
    virtual ~TopInputMap() {;}
  };
  
  template<class Container>
  TopInputMap<Container>::TopInputMap() 
    : std::map< ElementLink<Container>, TopOverlap > () {;}
  
  //template<class Container>
  //TopInputMap<Container>::TopInputMap(std::string) 
    //: std::map< ElementLink<Container>, TopOverlap > () {;}

} // end namespace

#endif // TOP_INPUT_MAP_H
