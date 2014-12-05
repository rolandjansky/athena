// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  
     \class JetCaloCellQualityTool
     \brief Calculates calorimeter based variables for jet quality
     using CaloCell contained in jet object directly.
 
     This class is identical (same properties) as JetCaloQualityTool except it uses CaloCell based calculations instead of the cluster moments based calculations.

     *IMPORANT* : cell based calculations often differ from cluster based calculations (because of how cluster
     moments are calculated/stored). 
     
     This tool therefore give different jet attribute names to these calculations.
         Ex : "NegativeE" -> "NegativeEFromCells" 
     The suffix can be changed by the CellSuffix property (default to "FromCells").
        
     
     This class is available only in Athena.

     \author P-A Delsart 
     \date (run 2 implementation) November , 2014
*/
#ifndef JETREC_JETCALOCELLQUALITYTOOL_H
#define JETREC_JETCALOCELLQUALITYTOOL_H

#include "JetMomentTools/JetCaloQualityTool.h"

#include "JetUtils/JetCaloCellQualityUtils.h"

class JetCaloCellQualityTool : public JetCaloQualityTool {
  ASG_TOOL_CLASS0(JetCaloCellQualityTool);
public:
  JetCaloCellQualityTool(const std::string & name);
  virtual StatusCode initialize();

 protected:

  /// This objects holds a list of cell-based calculators
  jet::JetCaloCellCalculations m_cellCalculators; 

  /// Suffix added to the name of the jet attribute
  std::string m_attSuffix;

  // parameters for Quality cuts
  int m_LArQualityCut;
  int m_TileQualityCut;

};
#endif
