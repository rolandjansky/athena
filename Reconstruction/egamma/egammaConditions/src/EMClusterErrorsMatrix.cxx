/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaConditions/EMClusterErrorsMatrix.h"
#include <iostream>
#include <utility>

EMClusterErrorsMatrix::EMClusterErrorsMatrix()
  :  EMAPMatrix<EMClusterErrorsEntry>()
{
}


EMClusterErrorsMatrix::EMClusterErrorsMatrix(const std::vector<EMAPMatrixAxis> &axes, 
					     const std::string& textDescription)
  :  EMAPMatrix<EMClusterErrorsEntry>(axes, textDescription)
{
}


  // Acces Operators
StatusCode EMClusterErrorsMatrix::setError(const std::vector<double> &x, const EMClusterErrorsEntry& apEntry)
{
  return setBinContent(x, apEntry);
}

void EMClusterErrorsMatrix::printMatrix() const
{
  std::cout << "***** Printing EMClusterErrorsMatrix: " << getTextDescription() << " *******"<< std::endl;
  std::cout << "  Matrix has " << m_dimensions << " dimensions" << std::endl;
  for(unsigned i = 0; i < m_dimensions; i++) {
    std::cout << "  Dim " << i << " is binning in " << m_axis.at(i).getName() << " with bin boundaries \n    ";
    std::vector<double> axisBinning = m_axis.at(i).getBinningInformation();
    for (double j : axisBinning) {
      std::cout << j << "  ";
    }
    std::cout << std::endl;
  }
  
  std::cout << "  Mapping of extra dimensions in matrix (m_base): ";
  for (unsigned int i : m_base) {
    std::cout << i << "  ";
  }
  
  std::cout << "\n  Matrix data:\n    "; 
  
  const unsigned int matrixSize = m_matrix.size();
  
  // push back the number of floats to store per matrix entry
  const unsigned int vectSize = (matrixSize) ? m_matrix.at(0).size() : 0;
  
  //std::cerr << "matrixSize = " << matrixSize << ", vectSize = " << vectSize << std::endl;
  
  for (unsigned int i=0; i<matrixSize; i++) {
    const EMClusterErrorsEntry vect = m_matrix.at(i);
    if (vect.size() != vectSize) {
      std::cerr << "The input EMClusterErrorsMatrix doesn't have entries with all having the same number of floats" << std::endl; // shoudl this go to a message stream instead? The macros didn't work.
      return;
    }
    for (unsigned int j=0; j<vectSize; j++) {
      std::cout << vect.at(j) << "  ";
    }
    std::cout << "\n    ";
  }
  std::cout << std::endl;
}

