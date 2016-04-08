/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_VECTORNTUPLEBRANCH_H
#define TRK_VECTORNTUPLEBRANCH_H

#include <string>
class TTree;

#include "EventPrimitives/EventPrimitives.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Vector.h"

namespace Trk {

  /**
     This class provides a simple interface to write Vectors to a root tree
     The size of the vectors cannot exceed ROWMAX. The class requires all vectors
     to have the same size

     Usage: 

     TTree* myTree = new TTree("data","data");
     VectorNtupleBranch branch;
     branch.initForWrite(*myTree,"vec",3,"");

     for(int i=0;i<n;++i){
       branch.fill( );
       myTree->Fill();
     }
  */
  struct VectorNtupleBranch {
    VectorNtupleBranch() : m_nrows(-1) {}

    /** initialize class for writing */
    bool initForWrite(TTree& tree, std::string varname, int nrow, std::string prefix = ""  );

    /** initialize class for reading */
    bool initForRead(TTree& tree, std::string varname, int nrow, std::string prefix = "" );
    
    /** fill a vector */
    bool fill( const Amg::VectorX& matrix );

    /** fill a vector */
    bool fill( const CLHEP::HepVector& vec );

    /** fill a vector */
    bool fill( const CLHEP::Hep3Vector& vec );

    /** fill a vector */
    bool fill( const HepGeom::Point3D<double>& vec );

    /** fill a vector */
    bool fill( const HepGeom::Vector3D<double>& vec );

    /** maximum size of the vector */
    static const int ROWMAX = 10;
    int m_nrows;
    float  m_vector[ROWMAX]; 
  };   

}

#endif
