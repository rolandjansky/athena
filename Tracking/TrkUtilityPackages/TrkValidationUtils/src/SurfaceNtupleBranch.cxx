/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValidationUtils/SurfaceNtupleBranch.h"
#include "TrkSurfaces/Surface.h"
#include <TTree.h>

namespace Trk {

  void SurfaceNtupleBranch::initForWrite(TTree& tree, const std::string& prefix ){
    m_transform.initForWrite(tree,prefix);
    m_center.initForWrite(tree,"center",3,prefix);
    m_normal.initForWrite(tree,"normal",3,prefix);

    TString bname = prefix.c_str();
    bname += "id";
    tree.Branch(bname,&m_id);
  }

  void SurfaceNtupleBranch::initForRead(TTree& tree, const std::string& prefix ) { 
    m_transform.initForRead(tree,prefix);
    m_center.initForRead(tree,"center",3,prefix);
    m_normal.initForRead(tree,"normal",3,prefix);
    TString bname = prefix.c_str();
    bname += "_id";
    tree.SetBranchAddress(bname,&m_id);
  }

  void SurfaceNtupleBranch::fill( const Trk::Surface& surface ) {
    
    m_transform.fill(surface.transform());
    m_center.fill(surface.center());
    m_normal.fill(surface.normal());
    m_id = surface.associatedDetectorElementIdentifier().get_compact();
  }
}
