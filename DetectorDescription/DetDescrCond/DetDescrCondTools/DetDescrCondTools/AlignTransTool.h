/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDTOOLS_ALIGNTRANSTOOL_H
#define DETDESCRCONDTOOLS_ALIGNTRANSTOOL_H
// AlignTransTool.h
// an AlgTool to manage the AlignableTransforms in the conditions store
// Richard Hawkings, started 25/9/03
// 
// This AlgTool provides the following methods
//   getTransform(ident,key)  - get the AlignableTransform corresponding to
//                              the given Identifier and key
//   reset(key)               - reset the sorted state for the object key
//                              must be called from any IOV callback function
//                              telling the user the key object has been 
//                              modified

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/DataHandle.h"
#include "DetDescrCondTools/AlignTransKey.h"
#include "CLHEP/Geometry/Transform3D.h"

////class HepGeom::Transform3D;
class Identifier;
class AlignableTransform;

class AlignTransTool: public AthAlgTool {
 public:
  AlignTransTool(const std::string& type, const std::string& name,
		 const IInterface* parent);
  ~AlignTransTool();

  static const InterfaceID& interfaceID();
  StatusCode initialize();
  StatusCode finalize();
  const HepGeom::Transform3D 
    getTransform(const Identifier& ident, const std::string& key) const;
  StatusCode reset(const std::string& key);

 private:
  const AlignableTransform* getptr(const std::string) const;
  StoreGateSvc* p_condstore;
  typedef std::vector<AlignTransKey>::iterator key_itr;
  mutable std::vector<AlignTransKey> m_keyvec;
  mutable std::string m_lastkey;
};

#endif // DETDESCRCONDTOOLS_ALIGNTRANSTOOL_H


