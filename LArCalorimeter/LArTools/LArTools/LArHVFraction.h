// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHVTOOLS_LARHVFRACTION_H
#define LARHVTOOLS_LARHVFRACTION_H

#include "LArElecCalib/ILArHVCorrTool.h"

/////////////////////////////////////////////////////////////////////////
/// \class LArHVFraction
/// \brief Retrieves the energy of cells affected by HV issues
///
/// 
/// This class provides utilities to select cells affected by HV issues and
/// to compute their number and amount of energy from a given list of cells.
///
/// The class makes use of ILArHVCorrTool and implements this selection :
///      isAffected = |hvcorr - 1| > threshold  
/// where hvcorr=ILArHVCorrTool::Scale( cell ) and threshold is the parameter given to the ctor.
///
class LArHVFraction {  
 public:
  LArHVFraction(ILArHVCorrTool *tool, double threshold=0.002) : m_tool(tool), m_threshold(threshold) {}
  
  struct Result {
    double energySum;
    size_t numCells;
  };



  bool isHVAffected(const CaloCell* cell) const {
    if(cell->caloDDE()->is_tile()) return false;

    Identifier cellID=cell->ID();          
    //retrieve offline correction from DB:
    float hvcorr = m_tool->Scale(cellID);
    
    //Correction should be between (0 and 2)
    if (!(hvcorr>0. && hvcorr<100.)) return false;

    return (fabs(hvcorr-1.)>m_threshold) ;
  }

  
  /// Returns the energy & num of cells affected by HV issues.
  /// Inputs parameter are *any* type of iterator over cells.
  template<class CELLITERATOR>
  Result findHVaffectedCells(CELLITERATOR it, CELLITERATOR itEnd) {

    double energyHVaff=0;
    size_t numCellsHVaff=0;

    for( ;it!=itEnd; it++) {
      const CaloCell* thisCell = *it;

      if( isHVAffected(thisCell) ){
        energyHVaff+=fabs(thisCell->e());
        numCellsHVaff++;
      }//end of non nominal HV area

    }
    return Result {energyHVaff, numCellsHVaff};
  }
  
 protected:
  ILArHVCorrTool *m_tool;
  double m_threshold;
};

#endif
