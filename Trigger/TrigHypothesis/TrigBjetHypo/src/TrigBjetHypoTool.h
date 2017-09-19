/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypoTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Lidija Zivkovic
// EMAIL:    Lidija.Zivkovic@ge.infn.it
// 
// ************************************************

#ifndef TRIGBJETHYPO_TRIGBJETHYPOTOOL_H
#define TRIGBJETHYPO_TRIGBJETHYPOTOOL_H 1

// This is in current hypo, not sure if needed
//#include "TrigInterfaces/HypoAlgo.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"

// Are these new?
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgTool.h" 


//Not sure if these are needed, thus commeneted
/* class TrigEFBjetContainer; */
/* class TrigTimerSvc; */
/* class TriggerElement; */


static const InterfaceID IID_TrigBjetHypoTool("TrigBjetHypoTool", 1, 0);


/**
 * @brief Hypo class for HLT b-jet selection.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 *
 * This is the base hypothesis class for the b-jet selection. 
 * Selection can be performed using different likelihood methods or a track-based chi2 method (CHI2), all implemented in the TrigBjetFex class.
 *
 * The different likelihood methods implemented are:
 * - significance of longitudinal impact parameter (IP1D);
 * - significance of transverese impact parameter (IP2D);
 * - 2D combination of the track-based methods (IP3D);
 * - invariant mass of tracks linked to the secondary vertex (MVTX);
 * - energy fraction of the secondary vertex (EVTX);
 * - number of tracks lined to the secondary vertex (NVTX);
 * - 3D combination of the vertex-based methods (SVTX);
 * - combination of the two likelihoods based on tracks and secondary vertex informations (COMB).
 *
 * Selection can be performed using these different taggers: CHI2, IP2D, IP3D or COMB.
 */


class TrigBjetHypoTool : virtual public ::AthAlgTool {

 public:

  /** @brief Constructor. */
  TrigBjetHypoTool (const std::string& type,
		const std::string& name,
		const IInterface* parent );
  /** @brief Destructor. */
  virtual ~TrigBjetHypoTool ();

  StatusCode initialize() override;
  StatusCode finalize() override;

  static const InterfaceID& interfaceID();

  TrigCompositeUtils::DecisionID decisionId() const {
    return m_id.numeric();
  }

 /**
   * @brief decides upon a single object
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   **/
  bool decide(  const xAOD::BTagging* bTag, const TrigRoiDescriptor* roiDescriptor )  const;  
  //  bool decide(  const xAOD::BTaggingContainer* trigBTaggingContainer, const TrigRoiDescriptor* roiDescriptor )  const;  


 private:

  HLT::Identifier m_id;

  //  std::string m_jetKey; // not sure if needed, with some changes new configuration may be established

  /** @brief DeclareProperty: if acceptAll flag is set to true, every event is taken. */ 
  bool m_acceptAll;
  /** @brief DeclareProperty: list of likelihood methods to be effectively used to perform the selection. */
  std::string m_methodTag; 
  /** @brief DeclareProperty: lower bound of the discriminant variable to be selected (if flag acceptAll is set to false) for MV2 tagger. */
  float m_xcutMV2c20;
  float m_xcutMV2c10;


  // Not sure if needed
  /** @brief DeclareProperty: string corresponding to the trigger level in which the algorithm is running. */
  std::string m_instance;

  /** @brief to check the beam spot flag status. */
  bool m_useBeamSpotFlag;

 /** @brief Overide the requirement that the BS is valid. */
  /** @brief Used to not apply the correction to the GSC chains */
  bool m_overRideBeamSpotValid;


  /** @brief DeclareProperty: to monitor method used to perform the cut. */
  //  float m_monitorMethod;
  ToolHandle<GenericMonitoringTool> m_monTool;
};

inline const InterfaceID& TrigBjetHypoTool::interfaceID()
{
   return IID_TrigBjetHypoTool;
}

#endif // !TRIGBJETHYPO_TRIGBJETHYPOTOOL_H

