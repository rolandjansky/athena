/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_MAKE_SCALING_H
#define GEO_MODEL_XML_MAKE_SCALING_H
//
//    Create and return a HepRotation3d. Caller must delete it.
//

namespace xercesc_3_1 {class DOMElement;}
namespace HepGeom {class Scale3D;}
class GmxUtil;


class MakeScaling {
public:
    MakeScaling();
    HepGeom::Scale3D getTransform(const xercesc_3_1::DOMElement *scaling, GmxUtil &gmxUtil);
};
#endif // MAKE_SCALING_H