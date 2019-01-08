/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1RoITools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1ROITOOLS_H
#define ILVL1L1ROITOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"
#include "xAODTrigL1Calo/JetElement.h"

class LVL1_ROI;
class EmTau_ROI;
class Jet_ROI;
class EnergySum_ROI;
class JetET_ROI;

namespace LVL1 
{
class TriggerTower;
class JetElement;

/**
Interface definition for L1RoITools
*/
  static const InterfaceID IID_IL1RoITools("LVL1::IL1RoITools", 1, 0);

  class IL1RoITools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void findROIs(const DataVector<TriggerTower>* tts, LVL1_ROI& rois, int slice = -1) = 0;
    virtual void findEmTauROIs(const DataVector<TriggerTower>* tts, std::vector<EmTau_ROI>& rois, int slice = -1) = 0;
    virtual void findJetROIs(const DataVector<TriggerTower>* tts, std::vector<Jet_ROI>& rois, int slice = -1) = 0;
    virtual void findJetROIs(const DataVector<JetElement>* jes, std::vector<Jet_ROI>& rois, int slice = -1) = 0;
    virtual void findEnergySumROI(const DataVector<TriggerTower>* tts, std::vector<EnergySum_ROI>& rois, int slice = -1) = 0;
    virtual void findEnergySumROI(const DataVector<xAOD::JetElement>* jes, std::vector<EnergySum_ROI>& rois, int slice = -1) = 0;
    virtual void findJetETROI(const std::vector<Jet_ROI>& jets, std::vector<JetET_ROI>& rois) = 0;
    virtual void findEmTauTTIDs(float roiEta, float roiPhi, std::vector<unsigned int>& coolIDs) = 0;
    virtual void findEmTauTTIDs(const EmTau_ROI& roi, std::vector<unsigned int>& coolIDs) = 0;
    virtual void findJetTTIDs(float roiEta, float roiPhi, std::vector<unsigned int>& coolIDs) = 0;
    virtual void findJetTTIDs(const Jet_ROI& roi, std::vector<unsigned int>& coolIDs) = 0;
      
  };

  inline const InterfaceID& LVL1::IL1RoITools::interfaceID()
    { 
      return IID_IL1RoITools;
    }

} // end of namespace

#endif 
