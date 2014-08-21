/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMISSINGETTAGTOOL_H 
#define JETMISSINGETTAGTOOL_H 

/*****************************************************************************
Name    : JetMissingEtTagTool.h
Package : offline/PhysicsAnalysis/JetMissingEtID/JetMissingEtTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the JetMissingEt Tag objects - ParticleJetTagCollection.h and MissingEtTag.h 
	  The JetMissingEt Tag fragments are built here
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

class StoreGateSvc;
class ParticleJetTagCollection;

/** Interface ID for JetMissingEtTagTool*/  
static const InterfaceID IID_JetMissingEtTagTool("JetMissingEtTagTool", 1, 0);

class JetMissingEtTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  JetMissingEtTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_JetMissingEtTagTool; };

  /** Overriding initialize, finalize */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap);
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& missingEtTag);
  virtual StatusCode execute(TagFragmentCollection& jetTagCol, const int max);
  virtual StatusCode finalize();

  // not the most elegant -- should use the new JetSelectorTool ...
  /* typedef enum  { LooseMinusBad,  LooseBad, MediumBad,  TightBad  } BadJetCategory; */
  /* /\* static bool isBad(BadJetCategory criteria, double quality, double NegE, double emf,  *\/ */
  /* /\*   	    double hecf, double time,double fmax, double eta, double chf, double HecQ, double AvgLArQ ); *\/ */


protected:

   /** Standard destructor */
   virtual ~JetMissingEtTagTool( );

private:

  /** Properties */
  std::string m_containerName;
  std::string m_metContainerName;
  std::string m_metRefFinalName;
  // std::string m_metSoftJetName;
  // std::string m_metRefMuonName;
  std::string m_metMuonsName;
  std::string m_metSoftTermName;
  std::string m_metRefTauName;


  double m_jetPtCut;
  bool   m_useEMScale; //Emergency fix for HI (W.L, 22.11.2010)
  bool   m_isSimulation;

  /** the attribute names */
  std::vector<std::string> m_ptStr;
  std::vector<std::string> m_etaStr;
  std::vector<std::string> m_phiStr;
  std::vector<std::string> m_pidStr;

  /** Event Store */
  StoreGateSvc* m_storeGate;



 };

#endif // JETMISSINGETTAGTOOL_H



