// this file is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPFLOW_VERSIONS_FLOWELEMENTAUXCONTAINER_V1_H
#define XAODPFLOW_VERSIONS_FLOWELEMENTAUXCONTAINER_V1_H


// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "AthLinks/ElementLink.h"
#include "xAODPFlow/FlowElement.h"

namespace xAOD {


  class FlowElementAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    FlowElementAuxContainer_v1();
    ~FlowElementAuxContainer_v1() = default;

  private:
    /** Charge of FlowElement */
    std::vector<float> charge;

    std::vector<FlowElement_v1::signal_t> signalType;

    std::vector<FlowElement_v1::vertex_t> vertexType;

    /** 4-vector of FlowElement */
    std::vector<float> pt;
    std::vector<float> eta;
    std::vector<float> phi;
    std::vector<float> m;

    /** Vectors of links to underlying objects*/

    std::vector<std::vector<ElementLink<IParticleContainer> > > chargedObjectLinks;
    std::vector<std::vector<float > >  chargedObjectWeights;

    std::vector<std::vector<ElementLink<IParticleContainer> > > otherObjectLinks;
    std::vector<std::vector<float > >  otherObjectWeights;


  }; // class FlowElementAuxContainer_v1

} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::FlowElementAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODPFLOW_VERSIONS_FlowElementAUXCONTAINER_V1_H
