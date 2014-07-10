/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODBTagging/SecVtxHelper.h"


  // get variable mass
  double xAOD::SecVtxHelper::VertexMass(const xAOD::Vertex* vtx){
    static SG::AuxElement::Accessor<double> a("mass");
    if(!a.isAvailable(*vtx)){
      std::cout << "ERROR: Failed to get mass" << std::endl;
      return 0;
    }
    return a(*vtx);
  }
  // set variable mass
  void xAOD::SecVtxHelper::setVertexMass(xAOD::Vertex * vtx, double val){
    static SG::AuxElement::Accessor<double> a("mass");
    a(*vtx)=val;        

  }
  // get variable energyFraction
  double xAOD::SecVtxHelper::EnergyFraction(const xAOD::Vertex* vtx){
    static SG::AuxElement::Accessor<double> a("energyFraction");
    if(!a.isAvailable(*vtx)){
      std::cout << "ERROR: Failed to get energyFraction" << std::endl;
      return 0;
    }
    return a(*vtx);
  }    
  // set variable energyFraction
  void xAOD::SecVtxHelper::setEnergyFraction(xAOD::Vertex * vtx, double val){ 
    static SG::AuxElement::Accessor<double> a("energyFraction");
    a(*vtx)=val;

  }   



