// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**  
     @class JetCaloQualityTool
     Calculates calorimeter based variables for jet quality 
     @author Nikola Makovec
     @author P-A Delsart 
     @date (first implementation) October , 2009
     @date (run 2 implementation) February , 2014

     This tool calculate calorimeter quantities from a jet and saves them as attributes.
     It takes a list of string as a property 'Calculations' : the list of moments to calculate.<br>
     From this list it internally prepares a list of calculators from JetUtils/JetCaloQualityUtils.h<br>

     List of known calculations (see also the implementation of initialize()) :
         LArQuality, Timing, NegativeE, Centroid, N90Constituents, BchCorrCell, FracSamplingMax
     
     This class performs cluster-based calculation. For similar cell-based calculation, see JetCaloCellQualityUtils.h
     
*/
#ifndef JETREC_JETCALOQUALITYTOOL_H
#define JETREC_JETCALOQUALITYTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "JetInterface/IJetDecorator.h"
#include "JetUtils/JetCaloCalculations.h"
#include "AsgDataHandles/WriteDecorHandleKeyArray.h"

#include <vector>
#include <string>


class JetCaloQualityTool: public asg::AsgTool,
                          virtual public IJetDecorator {
  ASG_TOOL_CLASS1(JetCaloQualityTool,IJetDecorator)
  
public:
  JetCaloQualityTool(const std::string & name);

  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;
  
  virtual StatusCode initialize() override;

 protected:

  Gaudi::Property<std::vector<std::string> > m_calculationNames{this, "Calculations", {},
      "Name of calo quantities to compute and add as decorations"};
  Gaudi::Property<std::vector<double> > m_timingTimeCuts{this, "TimingCuts", {},
      "Time cuts for out-of-time calo quantities"};
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "",
      "SG key of input jet container"};

  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_writeDecorKeys{this, "OutputDecorKeys", {},
      "SG keys for output decorations (not to be configured manually!)"};

  /// This objects holds a list of cluster-based calculators  
  jet::JetCaloCalculations m_jetCalculations;

  // internal pointer to m_jetCalculations (this pointer is also used in the cell-based derived tool)
  jet::JetCaloCalculations * m_calcProcessor;

  bool m_doFracSamplingMax; // internal			     
};
#endif 

