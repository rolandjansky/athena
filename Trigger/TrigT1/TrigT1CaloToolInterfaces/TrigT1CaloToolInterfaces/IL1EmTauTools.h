/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1EmTauTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1EMTAUTOOLS_H
#define ILVL1L1EMTAUTOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/TriggerTower.h"
//#include "TrigT1CaloUtils/CPAlgorithm.h"


namespace LVL1 
{
class CPAlgorithm;
class CPMTower;
class TriggerTower;

/**
Interface definition for L1EmTauTools
*/
  static const InterfaceID IID_IL1EmTauTools("LVL1::IL1EmTauTools", 1, 0);

  class IL1EmTauTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void findRoIs(const CPMTowerMap_t* towers, DataVector<CPAlgorithm>* rois, int slice = -1) = 0;
    virtual void findRoIs(const TriggerTowerMap_t* tts, DataVector<CPAlgorithm>* rois, int slice = -1) = 0;
    virtual void findRoIs(const DataVector<CPMTower>* cpmts, DataVector<CPAlgorithm>* rois, int slice = -1) = 0;
    virtual void findRoIs(const DataVector<TriggerTower>* tts, DataVector<CPAlgorithm>* rois, int slice = -1) = 0;
    virtual void mapTowers(const DataVector<TriggerTower>* tts, CPMTowerMap_t* towers) = 0;
    virtual void mapTowers(const DataVector<CPMTower>* cpmts, CPMTowerMap_t* towers) = 0;
    virtual void mapTowers(const TriggerTowerMap_t* tts, CPMTowerMap_t* towers) = 0;
    virtual CPAlgorithm findRoI(double RoIeta, double RoIphi, const CPMTowerMap_t* towers, int slice = -1) = 0;
    virtual void formSums(double RoIeta, double RoIphi, const CPMTowerMap_t* towers, int slice = -1) = 0;
    virtual void formSums(uint32_t roiWord, const CPMTowerMap_t* towers, int slice = -1) = 0;
    virtual int Core() const = 0;
    virtual int EMClus() const = 0;
    virtual int TauClus() const = 0;
    virtual int EMIsol() const = 0;
    virtual int HadIsol() const = 0;
    virtual int HadCore() const = 0;
    virtual bool isEtMax() const = 0;
    virtual unsigned int Hits() const = 0;
    virtual unsigned int RoIWord() const = 0;
      
  };

  inline const InterfaceID& LVL1::IL1EmTauTools::interfaceID()
    { 
      return IID_IL1EmTauTools;
    }

} // end of namespace

#endif 
