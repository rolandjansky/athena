/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_PVTOOLS_H
#define DERIVATIONFRAMEWORK_PVTOOLS_H
#include "xAODBPhys/BPhysHelper.h"
#include <vector>

// Author: Adam Barton <abarton@SPAMMENOTTtttcern.ch>


namespace Trk {
  class V0Tools;
}
namespace Analysis{
  class PrimaryVertexRefitter;
}
namespace DerivationFramework {

  class BPhysPVTools {
       const Trk::V0Tools *m_v0Tools;
    
public:
  
       BPhysPVTools(Trk::V0Tools *v0Tools) : m_v0Tools(v0Tools) {};
         
       StatusCode FillCandExistingVertices(xAOD::VertexContainer* vtxContainer, const xAOD::VertexContainer* pvContainer, int DoVertexType);
       
       static void FillBPhysHelperNULL(xAOD::BPhysHelper &vtx, const xAOD::VertexContainer* PvContainer,
           xAOD::BPhysHelper::pv_type pvtype);
       
       StatusCode FillCandwithRefittedVertices(xAOD::VertexContainer* vtxContainer, const xAOD::VertexContainer* pvContainer,xAOD::VertexContainer* refPvContainer, Analysis::PrimaryVertexRefitter* , size_t in_PV_max, int DoVertexType);
       
       void DecorateWithNULL(xAOD::VertexContainer* vtxContainer,const xAOD::VertexContainer* pvContainer, int DoVertexType) const;
       
       void DecorateWithDummyVertex(xAOD::VertexContainer* vtxContainer, const xAOD::VertexContainer* pvContainer, const xAOD::Vertex* Dummy, int DoVertexType, bool SetOrignal) const;
       
       //Fills the BPhysHelper object with the standard parameters
       void FillBPhysHelper(xAOD::BPhysHelper &vtx, const xAOD::Vertex* refPV,const xAOD::VertexContainer* refPvContainer,
                    xAOD::BPhysHelper::pv_type pvtype, int) const;
    
       //Returns the index integer of the vertex with the lowest Z in relation to the given vertex
       size_t FindLowZIndex(const xAOD::BPhysHelper &Obj, const std::vector<const xAOD::Vertex*> &PVlist) const;
       //Returns the index integer of the vertex with the lowest A0 in relation to the given vertex
       size_t FindLowA0Index(const xAOD::BPhysHelper &Obj, const std::vector<const xAOD::Vertex*> &PVlist) const;
       
       static size_t FindHighPtIndex(const std::vector<const xAOD::Vertex*> &PVlist);
       
       //Static method call with DerivationFramework::BPhysDerHelpers::GetGoodPV
       //Returns a std::vector containing only PVs of type 1 and 3 - HighPt and Pileup
       static std::vector<const xAOD::Vertex*> GetGoodPV(const xAOD::VertexContainer* pvContainer);
    };

}


#endif //DERIVATIONFRAMEWORK_COMMON_H
