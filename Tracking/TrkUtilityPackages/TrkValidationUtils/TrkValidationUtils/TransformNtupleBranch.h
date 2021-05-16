/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_TRANSFORMNTUPLEBRANCH_H
#define TRK_TRANSFORMNTUPLEBRANCH_H

#include <string>
class TTree;

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkValidationUtils/MatrixNtupleBranch.h"
#include "TrkValidationUtils/VectorNtupleBranch.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Point3D.h"

namespace Trk {

  /**
     This class provides a simple interface to write Transforms to a root tree
     
     Usage: 
     
     TTree* myTree = new TTree("data","data");
     TransformNtupleBranch branch;
     branch.initForWrite(*myTree,"");
     
     for(int i=0;i<n;++i){
       branch.fill( );
       myTree->Fill();
     }
  */

  struct TransformNtupleBranch {

    /** initialize class for writing */
    void initForWrite(TTree& tree, const std::string& prefix = "" );

    /** initialize class for reading */
    void initForRead(TTree& tree, const std::string& prefix = "" );
    
    /** fill a transform */
    void fill( const Amg::Transform3D& transform );

    /** fill a transform */
    void fill( const HepGeom::Transform3D& transform );

    /** data */
    MatrixNtupleBranch m_rotation;
    VectorNtupleBranch m_translation;
  };   

}

#endif
