/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SPLayer.cxx
// Richard Hawkings, started 13/4/04
// Methods for SPLayer class to represent a super layer of space points

#include <iostream>
#include "InDetAlignGenAlgs/SPLayer.h"

SPLayer::SPLayer(const int h1, const int h2, const int det, const int bec,
     const int layer, const int side, const std::string& name,
     const std::vector<TH1*>& histlist) :
  m_hbase(h1), m_hbase2(h2), m_dettype(det), m_bec(bec), m_layer(layer),
  m_side(side),m_nmod(0), m_nhit(0), m_name(name), m_histlist(histlist)
 { 
   std::cout << "In splayer constructer" << std::endl;
   if (m_histlist.size() != 10) std::cout << 
       "SPLayer Error: Unexpected histogram list size " << 
          m_histlist.size() << std::endl;
}

