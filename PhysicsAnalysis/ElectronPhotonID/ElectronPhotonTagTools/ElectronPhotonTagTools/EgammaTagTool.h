/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATAGTOOL_H 
#define EGAMMATAGTOOL_H 

/*****************************************************************************
Name    : EgammaTagTool.h
Package : offline/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the Egamma Tag objects - AnalysisTag.h. 
	  The Egamma Analysis Tag fragment is built here
*****************************************************************************/
#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
//#include "egammaEvent/ElectronContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMissingET/MissingETContainer.h"
//#include "egammaEvent/PhotonContainer.h"
//#include "MissingETEvent/MissingET.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include <vector>

/** Interface ID for EgammaTagTool*/  
static const InterfaceID IID_EgammaTagTool("EgammaTagTool", 1, 0);

class EgammaTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  EgammaTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_EgammaTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int& max);
  virtual StatusCode execute(TagFragmentCollection& egammaTagCol, const int& max);
  virtual StatusCode finalize();


private:
  bool ZeeSelection (const xAOD::ElectronContainer* eleColl,
                     const DataHandle<EventInfo>& eventInfo);
  bool WenuSelection (const xAOD::ElectronContainer* eleColl,
                      const xAOD::MissingETContainer* metTopo,
                      const DataHandle<EventInfo>& eventInfo);
  void dumpEventDetails(const xAOD::Electron*,
                        const DataHandle<EventInfo>& eventInfo);


protected:

   /** Standard destructor */
   virtual ~EgammaTagTool( );

private:
  /** Properties */
  std::string m_electronContainer;
  std::string m_photonContainer;
  std::string m_missingEtObject;
  std::string m_missingEtTerm;

  double m_electronPtCut;
  double m_photonPtCut;
  double m_massZLow;
  double m_massZHigh;

  bool m_flagZeeEvent;
  bool m_flagWenuEvent;

  double m_missingEtCut;
  double m_welectronPtCut; 
  
  double m_invMass;
 };

#endif // ELECTRONTAGTOOL_H


