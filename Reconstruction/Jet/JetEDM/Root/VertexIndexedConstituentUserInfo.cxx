/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetEDM/VertexIndexedConstituentUserInfo.h"

namespace jet {

  VertexIndexedConstituentUserInfo::VertexIndexedConstituentUserInfo()  : IndexedConstituentUserInfo() {} 

  
  VertexIndexedConstituentUserInfo::VertexIndexedConstituentUserInfo(const xAOD::IParticle& par, Index idx, const LabelIndex* pli, const xAOD::Vertex* vtx) : IndexedConstituentUserInfo(par, idx, pli) , m_vertex(vtx) {}


}
