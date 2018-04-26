/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONTAGTOOL_H 
#define PHOTONTAGTOOL_H 

/*****************************************************************************
Name    : PhotonTagTool.h
Package : offline/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the Photon Tag objects - PhotonTagCollection.h. 
	  The Photon Tag fragment is built here
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/IEGammaAmbiguityTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationSelection/IsolationSelectionTool.h"
#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "ElectronPhotonShowerShapeFudgeTool/IElectronPhotonShowerShapeFudgeTool.h"
#include "ElectronPhotonFourMomentumCorrection/IEgammaCalibrationAndSmearingTool.h"
#include <map>

class PhotonTagCollection;
class IAsgPhotonIsEMSelector;


/** Interface ID for PhotonTagTool */  
static const InterfaceID IID_PhotonTagTool("PhotonTagTool", 1, 0);

class PhotonTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  PhotonTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_PhotonTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int& max);
  virtual StatusCode execute(TagFragmentCollection& photonTagCol, const int& max); 
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~PhotonTagTool( );

private:

  /** Properties */
  std::string m_containerName;
  double m_cut_Et;
  std::string m_photon_met_container_name;
  std::vector<float> m_etconeisocutvalues;
  std::vector<float> m_ptconeisocutvalues;

  /** the attribute names */
  std::vector<std::string> m_ptStr;
  std::vector<std::string> m_etaStr;
  std::vector<std::string> m_phiStr;
  std::vector<std::string> m_tightStr;
  std::vector<std::string> m_isoStr;

  /**photon shower shape fudge*/
  bool m_isFullsim;
  ToolHandle<IElectronPhotonShowerShapeFudgeTool> m_shower_shape_fudge;

  ToolHandle<IAsgPhotonIsEMSelector>      m_loose_cut_based; 
  ToolHandle<IAsgPhotonIsEMSelector>      m_tight_cut_based; 

  ToolHandle<IEGammaAmbiguityTool>        m_egamma_ambi_tool; 
  
  /** isolation correction tool */
  ToolHandle<CP::IIsolationCorrectionTool> m_isolation_correction_tool; 
  
  ToolHandle<CP::IIsolationSelectionTool> m_cone40_calo_isolation;//!
  ToolHandle<CP::IIsolationSelectionTool> m_cone40_isolation;//!
  ToolHandle<CP::IIsolationSelectionTool> m_cone20_isolation;//!
  ToolHandle<CP::IIsolationSelectionTool> m_fixedcut_tightcaloonly_isolation;//!
  ToolHandle<CP::IIsolationSelectionTool> m_fixedcut_loose_isolation;//!
  ToolHandle<CP::IIsolationSelectionTool> m_fixedcut_tight_isolation;//!

  /** calib tool */
  // ToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_EgammaCalibrationAndSmearingTool;

 };

#endif // PHOTONTAGTOOL_H


