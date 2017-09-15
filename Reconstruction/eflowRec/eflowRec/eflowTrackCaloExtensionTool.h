/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTRACKCALOEXTENSIONTOOL_H_
#define EFLOWTRACKCALOEXTENSIONTOOL_H_

/********************************************************************

 NAME:     eflowTrackCaloExtensionTool.h
 PACKAGE:  offline/Reconstruction/eflowRec

 AUTHORS:  T.Guttenberger
 CREATED:  19th September, 2014

 ********************************************************************/

#include "eflowRec/eflowTrackExtrapolatorBaseAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
namespace Trk {
//class IParticleCaloExtensionTool;
class TrackParametersIdHelper;
}
class eflowTrackCaloPoints;

static const InterfaceID IID_eflowTrackCaloExtensionTool("eflowTrackCaloExtensionTool", 1, 0);

class eflowTrackCaloExtensionTool: virtual public eflowTrackExtrapolatorBaseAlgTool, public AthAlgTool {

public:
  eflowTrackCaloExtensionTool(const std::string& type, const std::string& name,
                             const IInterface* parent);
  ~eflowTrackCaloExtensionTool();

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() override;
  virtual std::unique_ptr<eflowTrackCaloPoints> execute(const xAOD::TrackParticle* track) const override;
  virtual StatusCode finalize() override;

private:
  eflowCalo::LAYER getLayer(const Trk::CurvilinearParameters* clParameters) const;

  ToolHandle<Trk::IParticleCaloExtensionTool> m_theTrackExtrapolatorTool;

  std::unique_ptr<Trk::TrackParametersIdHelper> m_trackParametersIdHelper;

};

inline const InterfaceID& eflowTrackCaloExtensionTool::interfaceID() {
  return IID_eflowTrackCaloExtensionTool;
}

#endif /* EFLOWTRACKCALOEXTENSIONTOOL_H_ */
