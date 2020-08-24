/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMISSINGETTAGTOOL_H 
#define JETMISSINGETTAGTOOL_H 

/*****************************************************************************
Name    : JetMissingEtTagTool.h
Package : offline/PhysicsAnalysis/JetMissingEtID/JetMissingEtTagTools
Purpose : build the JetMissingEt Tag objects - ParticleJetTagCollection.h and MissingEtTag.h 
	  The JetMissingEt Tag fragments are built here
*****************************************************************************/

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "xAODJet/JetContainer.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"

#include <map>

// forward declaration
class IJetCalibrationTool;
class IJetUpdateJvt;
class IMETMaker;

class JetMetTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  JetMetTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { 
    static const InterfaceID IID_JetMetTagTool("JetMetTagTool", 1, 0);

    return IID_JetMetTagTool; };

  /** Overriding initialize, finalize */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap);
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& missingEtTag);
  virtual StatusCode execute(TagFragmentCollection& jetTagCol, const int max);
  virtual StatusCode finalize();
  
  //   Standard destructor
  virtual ~JetMetTagTool( );

private:
  
  /**Calibrate and record a shallow copy of a given jet container */
  const xAOD::JetContainer* calibrateAndRecordShallowCopyJetCollection(const xAOD::JetContainer * jetContainer);

  /**recompute MET*/
  StatusCode recomputeMissingEt();


  /** Jet calibration tool handle */
  ToolHandle<IJetCalibrationTool>        m_jetCalibrationTool;
  ToolHandle<IJetUpdateJvt>              m_jetJVTUpdateTool;
  ToolHandle<IMETMaker>                  m_metmaker;
  ToolHandle<IBTaggingSelectionTool>     m_FixedCutBEff_60,m_FixedCutBEff_70,m_FixedCutBEff_85,
                                         m_FlatBEff_60,m_FlatBEff_70,m_FlatBEff_77;

  /** Properties */
  std::string m_containerName;
  std::string m_jetCalibcontainerName;
  std::string m_jetCalibcontainerName_skim;
  std::string m_ElectronsContainer_skim;
  std::string m_PhotonsContainer_skim;
  std::string m_TausContainer_skim;
  std::string m_MuonsContainer_skim;
  std::string m_METCoreName;
  std::string m_METMapName;
  std::string m_metContainerName;
  std::string m_metRefFinalName;
  std::string m_metRefJetName;
  std::string m_metMuonsName;
  std::string m_metSoftClusName;
  std::string m_metRefTauName;
  std::string m_metRefEleName;
  std::string m_metRefGammaName;
  std::string m_metPVSoftTrkName;
  std::string m_metFinalTrkName;
  
  double m_jetPtCut;
  double m_jetPtCut_skim;
  bool   m_useEMScale; //Emergency fix for HI (W.L, 22.11.2010)
  bool   m_isSimulation;
  bool   m_doJVT;

  /** the attribute names */
  std::vector<std::string> m_ptStr;
  std::vector<std::string> m_etaStr;
  std::vector<std::string> m_phiStr;
  std::vector<std::string> m_pidStr;
  std::vector<std::string> m_eStr;


 };

#endif // JETMISSINGETTAGTOOL_H



