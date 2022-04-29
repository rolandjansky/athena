/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#include <variant>
namespace InDetDD {
  class SiDetectorElement;
}

class PixelID;
class SCT_ID;

class IdentityHelper {

public:
  IdentityHelper(const InDetDD::SiDetectorElement* detElem);
  
  int bec() const;
  int layer_disk() const;
  int phi_module() const;
  int eta_module() const;
  int side() const;

  int phi_module_max() const;
  int eta_module_max() const;

private:
  const InDetDD::SiDetectorElement* m_elem;
  std::variant<const PixelID*, const SCT_ID*> m_helper;
  const PixelID* getPixelIDHelper() const;
  const SCT_ID* getSCTIDHelper() const;

  
};
