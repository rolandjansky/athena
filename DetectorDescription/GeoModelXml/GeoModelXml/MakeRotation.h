/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_MAKE_ROTATION_H
#define GEO_MODEL_XML_MAKE_ROTATION_H
//
//    Create and return a HepRotation3d. Caller must delete it.
//

namespace xercesc_3_1 {class DOMElement;}
namespace HepGeom {class Rotate3D;}
class GmxUtil;


class MakeRotation {
public:
    MakeRotation();
    HepGeom::Rotate3D getTransform(const xercesc_3_1::DOMElement *rotation, GmxUtil &gmxUtil);
};
#endif // MAKE_ROTATION_H