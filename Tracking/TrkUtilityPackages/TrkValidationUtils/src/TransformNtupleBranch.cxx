/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValidationUtils/TransformNtupleBranch.h"

#include <TTree.h>
#include <iostream>

namespace Trk {

  void TransformNtupleBranch::initForWrite(TTree& tree, std::string prefix ){    
    m_rotation.initForWrite(tree,"rot",3,3,prefix);
    m_translation.initForWrite(tree,"trans",3,prefix);
  }

  void TransformNtupleBranch::initForRead(TTree& tree, std::string prefix ) { 
    m_rotation.initForRead(tree,"rot",3,3,prefix);
    m_translation.initForRead(tree,"trans",3,prefix);
  }



  void TransformNtupleBranch::fill( const Amg::Transform3D& transform ) {
    Amg::Vector3D translation = transform.translation();
    Amg::RotationMatrix3D rotation = transform.rotation();

    m_rotation.fill(rotation);
    m_translation.fill(translation);
  }

  void TransformNtupleBranch::fill( const HepGeom::Transform3D& transform ) {
    m_rotation.fill(transform.getRotation());
    m_translation.fill(transform.getTranslation());
  }
}
