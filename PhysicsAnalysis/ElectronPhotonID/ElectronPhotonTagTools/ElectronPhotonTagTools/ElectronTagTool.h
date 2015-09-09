/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ELECTRONTAGTOOL_H 
#define ELECTRONTAGTOOL_H 

/*****************************************************************************
Name    : ElectronTagTool.h
Package : offline/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the Electron Tag objects - ElectronTagCollection.h. 
	  The Electron Tag fragment is built here
*****************************************************************************/

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonShowerShapeFudgeTool/IElectronPhotonShowerShapeFudgeTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "ElectronPhotonFourMomentumCorrection/IEgammaCalibrationAndSmearingTool.h"

#include <map>

class ElectronTagCollection;

namespace CP {
  class IIsolationSelectionTool;
}

/** Interface ID for ElectronTagTool*/  
static const InterfaceID IID_ElectronTagTool("ElectronTagTool", 1, 0);


class ElectronTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  ElectronTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_ElectronTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int& max);
  virtual StatusCode execute(TagFragmentCollection& electronTagCol, const int& max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~ElectronTagTool( );

private:

  /** Properties */
  std::vector<std::string> m_containerNames;
  double m_cut_Et;
  std::vector<float> m_etconeisocutvalues;
  std::vector<float> m_ptconeisocutvalues;

  /** the attribute names */
  std::vector<std::string> m_ptStr;
  std::vector<std::string> m_etaStr;
  std::vector<std::string> m_phiStr;
  std::vector<std::string> m_tightStr;
  std::vector<std::string> m_fwdStr;
  std::vector<std::string> m_isoStr;

  /** electron cut-based ID tool */
  ToolHandle<IAsgElectronIsEMSelector> m_loose_cut_based;
  ToolHandle<IAsgElectronIsEMSelector> m_medium_cut_based;
  ToolHandle<IAsgElectronIsEMSelector> m_tight_cut_based;

  /** electron likelihood ID tool */
  ToolHandle<IAsgElectronLikelihoodTool> m_veryloose_likelihood;
  ToolHandle<IAsgElectronLikelihoodTool> m_loose_likelihood;
  ToolHandle<IAsgElectronLikelihoodTool> m_medium_likelihood;
  ToolHandle<IAsgElectronLikelihoodTool> m_tight_likelihood;

  /** electron isolation tool */
  ToolHandle<CP::IIsolationSelectionTool> m_loose_trackonly_isolation;
  ToolHandle<CP::IIsolationSelectionTool> m_loose_isolation;
  ToolHandle<CP::IIsolationSelectionTool> m_tight_isolation;
  ToolHandle<CP::IIsolationSelectionTool> m_gradient_loose_isolation;
  ToolHandle<CP::IIsolationSelectionTool> m_gradient_isolation;

  /** calib tool */
  ToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_EgammaCalibrationAndSmearingTool;
  
};

#endif // ELECTRONTAGTOOL_H


