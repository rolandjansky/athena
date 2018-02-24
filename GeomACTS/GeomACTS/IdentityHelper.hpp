// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef GeomACTS_IdentityHelper_h
#define GeomACTS_IdentityHelper_h

#ifndef ACTS_PARAMETER_DEFINITIONS_PLUGIN
#define ACTS_PARAMETER_DEFINITIONS_PLUGIN "ACTS/Utilities/detail/DefaultParameterDefinitions.hpp"
#endif

namespace InDetDD {
  class SiDetectorElement;
}

class PixelID;
class SCT_ID;

namespace Acts {
class IdentityHelper {

public:
  IdentityHelper(const InDetDD::SiDetectorElement* detElem);
  
  int bec() const;
  int layer_disk() const;
  int phi_module() const;
  int eta_module() const;
  int side() const;
  int phi_index() const;
  int eta_index() const;

private:
  const InDetDD::SiDetectorElement* m_elem;
  
  const PixelID* getPixelIDHelper() const;
  const SCT_ID* getSCTIDHelper() const;

  
};
}

#endif
