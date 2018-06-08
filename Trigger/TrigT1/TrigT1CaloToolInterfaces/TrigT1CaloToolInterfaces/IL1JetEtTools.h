/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1JetEtTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1JETETTOOLS_H
#define ILVL1L1JETETTOOLS_H

#include "GaudiKernel/IAlgTool.h"

#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "TrigT1CaloEvent/JetROI.h"
#include "TrigT1CaloEvent/CMMJetHitsCollection.h"

namespace LVL1 
{

/**
Interface definition for L1JetEtTools
*/

  static const InterfaceID IID_IL1JetEtTools("LVL1::IL1JetEtTools", 1, 0);

  class IL1JetEtTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual unsigned int jetEtSum(const DataVector<JetROI>* rois) = 0;
    virtual unsigned int jetEtSum(const CMMJetHitsCollection* sums) = 0;
    virtual unsigned int jetEtSum(const std::vector<Jet_ROI>& rois) = 0;
    virtual unsigned int jetEtSum(const std::vector<unsigned int>& multiplicities,
                                  const std::vector<int>& weights) = 0;
    virtual unsigned int jetEtRoIWord(unsigned int jetEt) = 0;
    virtual void fillMultiplicities(const DataVector<JetROI>* rois, std::vector< unsigned int >& multiplicities) = 0;
    virtual void fillMultiplicities(const CMMJetHitsCollection* sums, std::vector< unsigned int >& multiplicities) = 0;
    virtual void fillMultiplicities(const std::vector< Jet_ROI >& rois, std::vector< unsigned int >& multiplicities) = 0;
    
  };

  inline const InterfaceID& LVL1::IL1JetEtTools::interfaceID()
    { 
      return IID_IL1JetEtTools;
    }

} // end of namespace

#endif 
