/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypoTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Carlo Varni
// EMAIL:    carlo.varni@cern.ch
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
#include "BeamSpotConditionsData/BeamSpotData.h"

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
  template<typename T> 
    StatusCode retrieveTool( const std::string&,PublicToolHandle< T >& );

 private:
  HLT::Identifier m_id;

  /** @brief DeclareProperty: if acceptAll flag is set to true, every event is taken. */ 
  Gaudi::Property< bool > m_acceptAll {this,"AcceptAll",false,"if acceptAll flag is set to true, every event is taken"};
  /** @brief DeclareProperty: list of likelihood methods to be effectively used to perform the selection. */
  Gaudi::Property< std::string > m_methodTag {this,"MethodTag","","list of likelihood methods to be effectively used to perform the selection"};
  /** @brief DeclareProperty: lower bound of the discriminant variable to be selected (if flag acceptAll is set to false) for MV2 tagger. */
  Gaudi::Property< float > m_bTaggingCut {this,"BTaggingCut",-20,"lower bound of the discriminant variable to be selected for b-tagging"};

  // Not sure if needed
  /** @brief to check the beam spot flag status. */
  Gaudi::Property< bool > m_useBeamSpotFlag {this,"UseBeamSpotFlag",false,"check the beam spot flag status"};
 /** @brief Overide the requirement that the BS is valid. */
  /** @brief Used to not apply the correction to the GSC chains */
  Gaudi::Property< bool > m_overRideBeamSpotValid {this,"OverrideBeamSpotValid",false,"Overide the requirement that the BS is valid"};

  /** @brief DeclareProperty: to monitor method used to perform the cut. */
  //  float m_monitorMethod;
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
};

inline const InterfaceID& TrigBjetHypoTool::interfaceID()
{
   return IID_TrigBjetHypoTool;
}

#endif // !TRIGBJETHYPO_TRIGBJETHYPOTOOL_H

