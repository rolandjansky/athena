// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MinimalSumAssociation.h"
#include "munkres.h"


MinimalSumAssociation::MinimalSumAssociation() : asg::AsgMessaging("MinimalSumAssociation"){
  msg().setLevel(MSG::DEBUG);
}

IAssociationStrategy::index_assignment_t MinimalSumAssociation::associate(const std::vector<std::vector<double> >& matrix){
  IAssociationStrategy::index_assignment_t resultmap;
  int nrows =  matrix.size();
  int ncols = matrix.at(0).size();
  
  auto workmatrix = matrix;
  //if we have more columns than rows we pad the matrix to make it square
  if(nrows < ncols){
    for(int i = (ncols - nrows); i > 0;i--){
      workmatrix.push_back(std::vector<double>(ncols,0));
    }
  }

  munkres::vec_type costs;
  costs.reserve(ncols);
  munkres munk(workmatrix);
  
  bool debug = false;
  auto result = munk.run(costs,debug);
  
  //result is indexed by row
  //i.e. when matrix is matrxi[reco][trig]
  //it maps reco particle at index i to trig particle at index
  //result[i]
  for(int i = 0;i < nrows;++i){
    resultmap[i] = result[i];
  }

  return resultmap;
}
