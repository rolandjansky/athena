/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define CY_GEOM_CPP
#include "JetUtils/cyl_geom.h"

namespace JetGeom {
void testHullLine(point_list_t &hull, point_t p){
  // helper function for findConvexHull
  point_list_t::iterator it_prev = hull.end();it_prev--;
  point_list_t::iterator it_pprev = it_prev; it_pprev--;
  line_t l(*it_pprev, *it_prev);
  //   std::cout << " hull : "<< hull.size()<< "  p.phi ="<<p.second  <<std::endl;
  //   std::cout << "      "<< (*it_prev).first<< " " << (*it_prev).second << std::endl;
  //   std::cout << "      "<< (*it_pprev).first << "  "<<(*it_pprev).second << std::endl;
  //   std::cout << "      -> "<< p.first << "  "<< p.second << std::endl;
  if ( ! l.is_left(p) && (hull.size()>2)){
    //std::cout << "  is right !"  << std::endl;
    hull.pop_back(); // remove last
    testHullLine(hull, p);
  }else{
    //std::cout << "  is left !"  << std::endl;
    hull.push_back(p);
  }
}
void listToSet(point_list_t &inList, point_set_t &outSet){
  point_list_t::iterator it = inList.begin();
  point_list_t::iterator itE = inList.end();
  point_set_t::iterator s_it = outSet.begin();
  for(; it!=itE; ++it){
    s_it = outSet.insert(s_it, *it);
  }  
}

}
