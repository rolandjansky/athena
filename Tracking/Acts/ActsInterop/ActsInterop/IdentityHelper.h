#pragma once

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

private:
  const InDetDD::SiDetectorElement* m_elem;
  
  const PixelID* getPixelIDHelper() const;
  const SCT_ID* getSCTIDHelper() const;

  
};
}

