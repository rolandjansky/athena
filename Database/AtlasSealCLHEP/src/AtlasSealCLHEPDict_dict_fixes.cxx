/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Generated at Mon Jul 11 17:45:44 2005. Do not modify it

#ifdef _WIN32
#pragma warning ( disable : 4786 )
#ifndef LCGDICT_STRING 
#include <string> // Included here since it is sensitive to private->public trick
#endif
#endif
#define private public
#define protected public
#include "../AtlasSealCLHEP/AtlasSealCLHEPDict.h"
#undef private
#undef protected
#include "ReflectionBuilder/ReflectionBuilder.h"
#include "ReflectionBuilder/CollectionProxy.h"
#include <typeinfo>
using namespace seal::reflect;

//------Dictionary for class Transform3D -------------------------------
class HepTransform3D_dict { 
  public:
  HepTransform3D_dict();
  static void* constructor_2212( void* );
  static void  destructor( void* o ) { ((HepGeom::Transform3D*)o)->~Transform3D(); }
};
HepTransform3D_dict::HepTransform3D_dict() {
  ClassBuilder _c_("HepTransform3D", "", typeid(HepGeom::Transform3D), sizeof(HepGeom::Transform3D),std::vector<const std::type_info*>(), false, NOCONTAINER, 0, PUBLIC);
  _c_.addField("xx", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->xx_)-64, PROTECTED );
  _c_.addField("xy", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->xy_)-64, PROTECTED );
  _c_.addField("xz", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->xz_)-64, PROTECTED );
  _c_.addField("dx", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->dx_)-64, PROTECTED );
  _c_.addField("yx", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->yx_)-64, PROTECTED );
  _c_.addField("yy", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->yy_)-64, PROTECTED );
  _c_.addField("yz", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->yz_)-64, PROTECTED );
  _c_.addField("dy", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->dy_)-64, PROTECTED );
  _c_.addField("zx", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->zx_)-64, PROTECTED );
  _c_.addField("zy", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->zy_)-64, PROTECTED );
  _c_.addField("zz", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->zz_)-64, PROTECTED );
  _c_.addField("dz", "double", "", (size_t)(&((HepGeom::Transform3D*)64)->dz_)-64, PROTECTED );
  _c_.addMethod("HepTransform3D", "", "HepTransform3D", constructor_2212, PUBLIC);
  _c_.addMethod("~HepTransform3D", "", destructor, PUBLIC);
  _c_.build();
}

//------Stub functions for class Transform3D -------------------------------
void* HepTransform3D_dict::constructor_2212( void* mem) {
  return ::new(mem) HepGeom::Transform3D();
}

//------Dictionary for class HepLorentzVector -------------------------------
class HepLorentzVector_dict { 
  public:
  HepLorentzVector_dict();
  static void* constructor_7008( void* );
  static void  destructor( void* o ) { ((CLHEP::HepLorentzVector*)o)->~HepLorentzVector(); }
};
HepLorentzVector_dict::HepLorentzVector_dict() {
  ClassBuilder _c_("HepLorentzVector", "", typeid(CLHEP::HepLorentzVector), sizeof(CLHEP::HepLorentzVector),std::vector<const std::type_info*>(), false, NOCONTAINER, 0, PUBLIC);
  _c_.addField("pp", "Hep3Vector", "", (size_t)(&((CLHEP::HepLorentzVector*)64)->pp)-64, PRIVATE );
  _c_.addField("ee", "double", "", (size_t)(&((CLHEP::HepLorentzVector*)64)->ee)-64, PRIVATE );
  _c_.addMethod("HepLorentzVector", "", "HepLorentzVector", constructor_7008, PUBLIC);
  _c_.addMethod("~HepLorentzVector", "", destructor, PUBLIC);
  _c_.build();
}

//------Stub functions for class HepLorentzVector -------------------------------
void* HepLorentzVector_dict::constructor_7008( void* mem) {
  return ::new(mem) CLHEP::HepLorentzVector();
}


//------Dictionary for class Hep3Vector -------------------------------
class Hep3Vector_dict { 
  public:
  Hep3Vector_dict();
  static void* constructor_x46( void* );
  static void  destructor( void* o ) { ((CLHEP::Hep3Vector*)o)->~Hep3Vector(); }
};
Hep3Vector_dict::Hep3Vector_dict() {
  ClassBuilder _c_("Hep3Vector", "", typeid(CLHEP::Hep3Vector), sizeof(CLHEP::Hep3Vector),std::vector<const std::type_info*>(), false, NOCONTAINER, 0, PUBLIC);
  _c_.addField("dx", "double", "", (size_t)(&((CLHEP::Hep3Vector*)64)->dx)-64, PROTECTED );
  _c_.addField("dy", "double", "", (size_t)(&((CLHEP::Hep3Vector*)64)->dy)-64, PROTECTED );
  _c_.addField("dz", "double", "", (size_t)(&((CLHEP::Hep3Vector*)64)->dz)-64, PROTECTED );
  _c_.addMethod("Hep3Vector", "", "CLHEP::Hep3Vector", constructor_x46, PUBLIC);
  _c_.addMethod("~Hep3Vector", "", destructor, PUBLIC);
  _c_.build();
}

//------Stub functions for class Hep3Vector -------------------------------

void* Hep3Vector_dict::constructor_x46( void* mem) {
  return ::new(mem) CLHEP::Hep3Vector();
}

//---------  Bootstrap for building classes ------

namespace {
  struct Dictionaries {
    Dictionaries() {
      HepTransform3D_dict();
      HepLorentzVector_dict();
      Hep3Vector_dict();
      Class::resolvePending();
    }
  };
  static Dictionaries instance;
}


//---------  Capabilities specification ------


// This file has been generated by lcgdict with the --capabilities option
static  const char* clnames[] = {
//--Begin AtlasSealCLHEPDictFix
    "LCGDict/HepTransform3D",
    "LCGDict/HepLorentzVector",
    "LCGDict/Hep3Vector"
//--End   AtlasSealCLHEPDict
//--Final End
};

extern "C" void SEAL_CAPABILITIES (const char**& names, int& n )
{ 
  names = clnames;
  n = sizeof(clnames)/sizeof(char*);
}

