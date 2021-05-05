/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_MATRIXNTUPLEBRANCH_H
#define TRK_MATRIXNTUPLEBRANCH_H

#include <string>
class TTree;

#include "EventPrimitives/EventPrimitives.h"
#include "CLHEP/Geometry/Transform3D.h"

namespace Trk {

  /**
     This class provides a simple interface to write Matricis to a root tree
     The size of the Matrix cannot exceed COLMAX x ROWMAX. The class requires all matrices
     to have the same size
     
     Usage: 
     
     TTree* myTree = new TTree("data","data");
     MatrixNtupleBranch branch;
     branch.initForWrite(*myTree,"vec",3,"");
     
     for(int i=0;i<n;++i){
       branch.fill( );
       myTree->Fill();
     }
  */

  struct MatrixNtupleBranch {
    MatrixNtupleBranch() : m_nrows(-1),m_ncols(-1) {}

    /** initialize class for writing */
    bool initForWrite(TTree& tree, const std::string& varname, int ncol, int nrow, const std::string& prefix = ""  );

    /** initialize class for reading */
    bool initForRead(TTree& tree, const std::string& varname, int ncol, int nrow, const std::string& prefix = "" );
    
    /** fill a vector */
    bool fill( const Amg::MatrixX& matrix );

    /** fill a vector */
    bool fill( const HepGeom::Rotate3D& matrix );

    /** data */
    static const int COLMAX = 10;
    static const int ROWMAX = 10;
    int m_nrows;
    int m_ncols;
    float  m_matrix[COLMAX][ROWMAX]; 
  };   

}

#endif
