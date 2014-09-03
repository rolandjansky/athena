/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//PACKAGE: Trigger/TrigAlgorithms/TrigEgammaRec
//AUTHORS: Alessandro.Tricoli@cern.ch, Phillip.Urquijo@cern.ch, Cibran.Santamarina.Rios@cern.ch, Teresa.Fonseca.Martin@cern.ch
//AUTHORS (xAOD): Ryan.White@cern.ch

#ifndef TRIGEGAMMAREC_H
#define TRIGEGAMMAREC_H


// INCLUDE HEADER FILES:
#include "GaudiKernel/Algorithm.h"
#include "TrigInterfaces/FexAlgo.h"

// egammaRec 
#include "egammaRecEvent/egammaRec.h"
#include "egammaRecEvent/egammaRecContainer.h"

// xAOD
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaFwd.h"

//#include "xAODEgamma/Electron.h"
//#include "xAODEgamma/Photon.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaContainer.h"

#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IEMAmbiguityTool.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "egammaInterfaces/IEMShowerBuilder.h"
#include "egammaInterfaces/IEMBremCollectionBuilder.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"

#include "CaloUtils/CaloCellDetPos.h"
/** 
 * \class TrigEgammaRec
 * \brief A modification of the offline/Reconstruction/egammaBuilder algorithm for use on the Event Filter
 */

class TrigEgammaRec: public HLT::FexAlgo {

public:

  //Invokes base class constructor.
  TrigEgammaRec(const std::string& name, ISvcLocator* pSvcLocator);

  // destructor
  ~TrigEgammaRec();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute( const HLT::TriggerElement* inputTE, 
                             HLT::TriggerElement* outputTE );

  HLT::ErrorCode hltStart();
  HLT::ErrorCode hltEndRun();

private:

  // Timers
  TrigTimer *m_timerTotal;
  TrigTimer *m_timerTool1, *m_timerTool2, *m_timerTool3, *m_timerTool4, *m_timerTool5;

  // suffix to be added to the egamma container name in TDS as an alias
  std::string m_electronContainerAliasSuffix;
  std::string m_photonContainerAliasSuffix;

  // subalgorithm pointers cached in initialize:
  std::vector< ToolHandle<IegammaBaseTool> >   m_vec_builders; 
  ToolHandle<IEMTrackMatchBuilder> m_trackMatchBuilder;
  ToolHandle<IEMConversionBuilder> m_conversionBuilder;
  ToolHandle<IEMShowerBuilder> m_showerBuilder;  // trigger specific
  ToolHandle<IEMFourMomBuilder> m_fourMomBuilder; // trigger specific
  ToolHandle<IEMAmbiguityTool> m_ambiguityTool;

  // booleans to run specific parts of offline reconstruction
  bool m_doConversions;
  bool m_doTrackMatching;
  //CaloUtil
  CaloCellDetPos *m_caloCellDetPos;
  
  //needed for monitoring to work
  xAOD::ElectronContainer* m_electron_container;
  xAOD::PhotonContainer* m_photon_container;
 
};
#endif // TRIGEGAMMAREC_H
