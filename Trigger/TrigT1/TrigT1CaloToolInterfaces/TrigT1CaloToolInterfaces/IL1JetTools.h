/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1JetTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1JETTOOLS_H
#define ILVL1L1JETTOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigT1CaloEvent/JetInput.h"
#include "TrigT1CaloEvent/JetElement.h"
//#include "TrigT1CaloUtils/JetAlgorithm.h"

namespace LVL1 
{
class JetAlgorithm;

/**
Interface definition for L1JetTools
*/

  static const InterfaceID IID_IL1JetTools("LVL1::IL1JetTools", 1, 0);

  class IL1JetTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void findRoIs(const std::map<int, JetInput*>* elements, DataVector<JetAlgorithm>* rois) = 0;
    virtual void findRoIs(const DataVector<JetElement>* jes, DataVector<JetAlgorithm>* rois, int slice = -1) = 0;
    virtual void mapJetInputs(const DataVector<JetElement>* jes, std::map<int, JetInput*>* elements, int slice = -1) = 0;
    virtual JetAlgorithm findRoI(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements) = 0;
    virtual void formSums(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements) = 0;
    virtual void formSums(uint32_t roiWord, const std::map<int, JetInput*>* elements) = 0;
    virtual int ET4x4() const = 0;
    virtual int ET6x6() const = 0;
    virtual int ET8x8() const = 0;
    virtual bool isEtMax() const = 0;
    virtual unsigned int Hits() const = 0;
    virtual unsigned int RoIWord() const = 0;
    
  };

  inline const InterfaceID& LVL1::IL1JetTools::interfaceID()
    { 
      return IID_IL1JetTools;
    }

} // end of namespace

#endif 
