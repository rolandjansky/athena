/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaUtils/ParameterDownWeighter.h"
#include <cfloat>

namespace ParameterDownWeighter {
  
  void getDownWeightedVM(EigenVectorAndMatrix& vm1, const EigenVectorAndMatrix& vm2, int indx) {

    const double factor=10e10;
    
    Amg::VectorX v1 = vm1.getVector();   
    Amg::MatrixX m1 = vm1.getMatrix(); 
    
    Amg::VectorX v2 = vm2.getVector();  
   
    if (indx != 999) {
      for (int i =0; i<5; i++)  {
	if ( i==indx ) {
	  v1[i] = v2[indx];
	  m1(i,indx)*=factor;
	} else {
	  m1(i,indx)*=std::sqrt(factor);
	  m1(indx,i)*=std::sqrt(factor);
	}
      } 	
    } 
  }
}
