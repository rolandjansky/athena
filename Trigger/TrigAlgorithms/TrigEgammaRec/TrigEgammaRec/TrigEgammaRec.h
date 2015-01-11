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
#include "LumiBlockComps/ILuminosityTool.h" 

#include "xAODPrimitives/IsolationType.h"
#include "RecoToolInterfaces/IsolationCommon.h"

namespace xAOD {
    class ITrackIsolationTool;
    class ICaloCellIsolationTool;
    class ICaloTopoClusterIsolationTool;
}
/** 
 *
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
  TrigTimer *m_timerIsoTool1, *m_timerIsoTool2, *m_timerIsoTool3;
  TrigTimer *m_timerPIDTool1, *m_timerPIDTool2, *m_timerPIDTool3;
  // suffix to be added to the egamma container name in TDS as an alias
  std::string m_electronContainerAliasSuffix;
  std::string m_photonContainerAliasSuffix;

  ToolHandle<IEMTrackMatchBuilder> m_trackMatchBuilder;
  ToolHandle<IEMConversionBuilder> m_conversionBuilder;
  ToolHandle<IEMShowerBuilder> m_showerBuilder;  // trigger specific
  ToolHandle<IEMFourMomBuilder> m_fourMomBuilder; // trigger specific
  ToolHandle<IEMAmbiguityTool> m_ambiguityTool;

  /** @brief Tool for decorating electron PID isEM and LH */
  ToolHandle<IegammaBaseTool> m_electronPIDBuilder;
  
  /** @brief Tool for decorating electron PID Calo LH */
  ToolHandle<IegammaBaseTool> m_electronCaloPIDBuilder;

  /** @brief Tool for decorating photon PID isEM for tight */
  ToolHandle<IegammaBaseTool> m_photonPIDBuilder;
 
  /** @brief Tool for tracking isolation calculation */
  ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool;
  
   /** @brief Tool for cell isolation calculation */
  ToolHandle<xAOD::ICaloCellIsolationTool> m_caloCellIsolationTool;
  
  /** @brief Tool for topo isolation calculation */
  ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_topoIsolationTool;
  
  /** Luminosity Tool */
  ToolHandle<ILuminosityTool>  m_lumiTool; 
  // booleans to run specific parts of offline reconstruction
  bool m_doConversions;
  bool m_doTrackMatching;
  bool m_doTrackIsolation;
  bool m_doCaloCellIsolation;
  bool m_doTopoIsolation;
  bool m_useBremAssoc;
  //CaloUtil
  CaloCellDetPos *m_caloCellDetPos;
  
  //needed for monitoring to work
  xAOD::ElectronContainer* m_electron_container;
  xAOD::PhotonContainer* m_photon_container;

  // Methods to dump reconstruction info for debugging 
  void PrintElectron(xAOD::Electron *); 
  void PrintPhoton(xAOD::Photon *); 

  //Now for isolation
  /** @brief Isolation types (for the alg. properties, only vector<vector<double>> available */
  std::vector<std::vector<double> > m_egisoInts;
  struct IsoHelp {
      std::vector<std::string> isoNames;
      std::vector<xAOD::Iso::IsolationType> isoTypes;
      std::vector<SG::AuxElement::Decorator<float>*> isoDeco;
  };
  struct CaloIsoHelp {
      IsoHelp help;
      xAOD::CaloCorrection CorrList;
  };
  std::map<std::string,CaloIsoHelp> m_egCaloIso;
  struct TrackIsoHelp {
      IsoHelp help;
      xAOD::TrackCorrection CorrList;
  };
  std::map<std::string,TrackIsoHelp> m_egTrackIso;

  // Get a name from the input integer corresponding to the enums
  std::pair<std::string,std::string> decodeEnum(int);

  //Monitoring vectors
  std::vector<float> m_lhval;
};
#endif // TRIGEGAMMAREC_H
