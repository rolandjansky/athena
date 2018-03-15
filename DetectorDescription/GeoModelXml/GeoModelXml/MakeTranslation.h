/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_MAKE_TRANSLATION_H
#define GEO_MODEL_XML_MAKE_TRANSLATION_H
//
//    Create and return a HepRotation3d. Caller must delete it.
//

namespace xercesc_3_1 {class DOMElement;}
namespace HepGeom {class Translate3D;}
class GmxUtil;


class MakeTranslation {
public:
    MakeTranslation();
    HepGeom::Translate3D getTransform(const xercesc_3_1::DOMElement *rotation, GmxUtil &gmxUtil);
};
#endif // MAKE_TRANSLATION_H