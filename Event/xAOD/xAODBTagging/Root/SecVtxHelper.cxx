/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODBTagging/SecVtxHelper.h"


  // get variable mass
  float xAOD::SecVtxHelper::VertexMass(const xAOD::Vertex* vtx){
    static const  SG::AuxElement::Accessor<float> a("mass");
    if(!a.isAvailable(*vtx)){
      std::cout << "ERROR: Failed to get mass" << std::endl;
      return 0;
    }
    return a(*vtx);
  }
  // set variable mass
  void xAOD::SecVtxHelper::setVertexMass(xAOD::Vertex * vtx, float val){
    static const SG::AuxElement::Accessor<float> a("mass");
    a(*vtx)=val;        

  }
  // get variable energyFraction
  float xAOD::SecVtxHelper::EnergyFraction(const xAOD::Vertex* vtx){
    static const SG::AuxElement::Accessor<float> a("energyFraction");
    if(!a.isAvailable(*vtx)){
      std::cout << "ERROR: Failed to get energyFraction" << std::endl;
      return 0;
    }
    return a(*vtx);
  }    
  // set variable energyFraction
  void xAOD::SecVtxHelper::setEnergyFraction(xAOD::Vertex * vtx, float val){ 
    static const SG::AuxElement::Accessor<float> a("energyFraction");
    a(*vtx)=val;

  }   
 //
  int xAOD::SecVtxHelper::VtxNtrk(const xAOD::Vertex* vtx){
    static const SG::AuxElement::Accessor<int> a("ntrk");
    if(!a.isAvailable(*vtx)){
      std::cout << "ERROR: Failed to get ntrk" << std::endl;
      return 0;
    }
    return a(*vtx);
  }
  //
  void xAOD::SecVtxHelper::setVtxNtrk(xAOD::Vertex * vtx, int val){
    static const SG::AuxElement::Accessor<int> a("ntrk");
    a(*vtx)=val;
  }
  //
  float xAOD::SecVtxHelper::Vtxpt(const xAOD::Vertex* vtx){
    static const SG::AuxElement::Accessor<float> a("pt");
    if(!a.isAvailable(*vtx)){
      std::cout << "ERROR: Failed to get pt" << std::endl;
      return 0;
    }
    return a(*vtx);
  }
  // 
  void xAOD::SecVtxHelper::setVtxpt(xAOD::Vertex * vtx, float val){
    static const SG::AuxElement::Accessor<float> a("pt");
    a(*vtx)=val;
  }
  //
  float xAOD::SecVtxHelper::Vtxeta(const xAOD::Vertex* vtx){
    static const SG::AuxElement::Accessor<float> a("eta");
    if(!a.isAvailable(*vtx)){
      std::cout << "ERROR: Failed to get eta" << std::endl;
      return 0;
    }
    return a(*vtx);
  }
  //
  void xAOD::SecVtxHelper::setVtxeta(xAOD::Vertex * vtx, float val){
    static const SG::AuxElement::Accessor<float> a("eta");
    a(*vtx)=val;
  }
  //
  float xAOD::SecVtxHelper::Vtxphi(const xAOD::Vertex* vtx){
    static const SG::AuxElement::Accessor<float> a("phi");
    if(!a.isAvailable(*vtx)){
      std::cout << "ERROR: Failed to get phi" << std::endl;
      return 0;
    }
    return a(*vtx);
  }
  //
  void xAOD::SecVtxHelper::setVtxphi(xAOD::Vertex * vtx, float val){
    static const SG::AuxElement::Accessor<float> a("phi");
    a(*vtx)=val;
  }
  //
  float xAOD::SecVtxHelper::VtxnormDist(const xAOD::Vertex* vtx){
    static const SG::AuxElement::Accessor<float> a("normDist");
    if(!a.isAvailable(*vtx)){
      std::cout << "ERROR: Failed to get normDist" << std::endl;
      return 0;
    }
    return a(*vtx);
  }
  //
  void xAOD::SecVtxHelper::setVtxnormDist(xAOD::Vertex * vtx, float val){
    static const SG::AuxElement::Accessor<float> a("normDist");
    a(*vtx)=val;
  }



























