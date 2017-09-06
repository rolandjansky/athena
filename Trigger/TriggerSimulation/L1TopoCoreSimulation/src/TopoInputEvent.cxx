//
//  TopoInputEvent.cxx
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/21/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#include "L1TopoCoreSimulation/TopoInputEvent.h"
#include "L1TopoCoreSimulation/ClusterTOB.h"
#include "L1TopoCoreSimulation/JetTOB.h"

using namespace std;

TCS::TopoInputEvent::TopoInputEvent() :
   m_clusters("InputClusters")
   , m_jets("InputJets")
{}

TCS::TopoInputEvent::~TopoInputEvent(){}

TCS::StatusCode
TCS::TopoInputEvent::add(const TCS::ClusterTOB & cluster) {
   m_clusters.push_back(cluster);
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::TopoInputEvent::add(const TCS::JetTOB & jet) {
   m_jets.push_back(jet);
   return StatusCode::SUCCESS;
}

// access to data
const TCS::InputTOBArray *
TCS::TopoInputEvent::inputTOBs(inputTOBType_t tobType) const {
   if(tobType == CLUSTER)
      return &m_clusters;
   
   if(tobType == JET)
      return &m_jets;
   
   return 0;
}


TCS::StatusCode
TCS::TopoInputEvent::clear() {
   // only need to clear the vectors since the objects themselves are collected on the ClusterTOB::heap and reset by the TopoSteering::reset

   m_clusters.clear();

   m_jets.clear();
   
   return StatusCode::SUCCESS;
}


std::ostream & operator<<(std::ostream &o, const TCS::TopoInputEvent &evt) {
   o << "Event:" << endl
   << "  #clusters: " << evt.clusters().size() << endl
   << "  #jets    : " << evt.jets().size() << endl;
   
   o << "Details:" << endl;
   o << "Cluster input vector (" << evt.clusters().name() << "):" << endl << evt.clusters() << endl;
   o << "Jet input vector (" << evt.jets().name() << "):" << endl << evt.jets();

   return o;
}
