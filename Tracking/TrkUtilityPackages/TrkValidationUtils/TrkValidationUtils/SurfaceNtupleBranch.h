/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_SURFACENTUPLEBRANCH_H
#define TRK_SURFACENTUPLEBRANCH_H

#include <string>
class TTree;

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkValidationUtils/TransformNtupleBranch.h"
#include "TrkValidationUtils/VectorNtupleBranch.h"



namespace Trk {
  class Surface;

  /**
     This class provides a simple interface to write Surfaces to a root tree
     
     Usage: 
     
     TTree* myTree = new TTree("data","data");
     SurfaceNtupleBranch branch;
     branch.initForWrite(*myTree,"");
     
     for(int i=0;i<n;++i){
       branch.fill( );
       myTree->Fill();
     }
  */


  struct SurfaceNtupleBranch {

    /** initialize class for writing */
    void initForWrite(TTree& tree, const std::string& prefix = "" );

    /** initialize class for reading */
    void initForRead(TTree& tree, const std::string& prefix = "" );
    
    /** fill a vector */
    void fill( const Trk::Surface& surface );

    /** data */
    TransformNtupleBranch m_transform;
    VectorNtupleBranch    m_center;
    VectorNtupleBranch    m_normal;
    int                   m_id;
  };   

}

#endif
