/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODHIEVENT_VERSIONS_HIEVENTSHAPEAUXCONTAINER_V1_H
#define XAODHIEVENT_VERSIONS_HIEVENTSHAPEAUXCONTAINER_V1_H


#include <vector>
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   class HIEventShapeAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      HIEventShapeAuxContainer_v1();

   private:

     /// Energy density information
     std::vector<float> Et;
     std::vector<float> area;
     std::vector<float> rho;
     std::vector<int> nCells;

     /// Harmonic modulation
     std::vector<std::vector<float> > Et_cos; 
     std::vector<std::vector<float> > Et_sin; 

     /// Information about slice
     std::vector<float> etaMin;
     std::vector<float> etaMax;
     std::vector<int> layer;

   }; // class HIEventShapeAuxContainer_v1

} // namespace xAOD
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::HIEventShapeAuxContainer_v1, xAOD::AuxContainerBase );
#endif // XAODEVENTINFO_VERSIONS_EVENTINFOAUXCONTAINER_V1_H
