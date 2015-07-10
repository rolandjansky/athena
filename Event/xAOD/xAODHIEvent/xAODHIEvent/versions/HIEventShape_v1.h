/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODHIEVENT_VERSIONS_HIEVENTSHAPE_V1_H
#define XAODHIEVENT_VERSIONS_HIEVENTSHAPE_V1_H

#include <set>
#include <vector>
#include <string>
#include <iosfwd>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"


/// Namespace holding all the xAOD classes/functions
namespace xAOD {

   class HIEventShape_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      HIEventShape_v1();

     // are floats enough  for storage?
     float Et() const;
     void setEt(float);

     float area() const;
     void setArea(float);

     float rho() const;
     void setRho(float);

     const std::vector<float>& Et_cos() const;
     void setEt_cos(const std::vector<float>& value);
     
     const std::vector<float>& Et_sin() const;
     void setEt_sin(const std::vector<float>& value);
     
     float etaMin() const;
     void setEtaMin(float);

     float etaMax() const;
     void setEtaMax(float);

     int layer() const;
     void setLayer(int);

     int nCells() const;
     void setNcells(int);

     void toPersistent();
     

     std::vector<float>& Et_cos() 
     {
       static Accessor< std::vector<float> > acc("Et_cos");
       return acc(*this); 
     }
     std::vector<float>& Et_sin() 
     {
       static Accessor< std::vector<float> > acc("Et_sin");
       return (std::vector<float>&) acc(*this); 
     }

   }; // class HIEventShape_v1

} // namespace xAOD
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::HIEventShape_v1, SG::AuxElement );

#endif // XAODEVENTINFO_VERSIONS_EVENTINFO_V1_H
