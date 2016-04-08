/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVxJetFitter_utilities_H
#define TrkVxJetFitter_utilities_H

namespace Trk {

  namespace {
    int numRow(int numVertex) {
      return numVertex+5;
    }

    Amg::MatrixX deleteRowFromSymMatrix(const Amg::MatrixX & orig,unsigned int rowToDelete) {
      unsigned int size=orig.rows();
      Amg::MatrixX modified(size-1,size-1);
      modified.setZero();
      for (unsigned int i=0;i<size;i++) {
	for (unsigned int j=0;j<size;j++) {
	  if (i<rowToDelete&&j<rowToDelete) {
	    modified(i,j)=orig(i,j);
	  } else if (i>rowToDelete&&j<rowToDelete) {
	    modified(i-1,j)=orig(i,j);
	  } else if (i<rowToDelete&&j>rowToDelete) {
	    modified(i,j-1)=orig(i,j);
	  } else if (i>rowToDelete&&j>rowToDelete) {
	    modified(i-1,j-1)=orig(i,j);
	  }
	}
      }
      return modified;
    }

    Amg::VectorX deleteRowFromVector(const Amg::VectorX & orig,unsigned int rowToDelete) {
      unsigned int size=orig.rows();
      Amg::VectorX modified(size-1);modified.setZero();
      for (unsigned int i=0;i<size;i++) {
	if (i<rowToDelete) {
	  modified(i)=orig(i);
	} else if (i>rowToDelete) {
	  modified(i-1)=orig(i);
	}
      }
      return modified;
    }
  }
}
#endif
