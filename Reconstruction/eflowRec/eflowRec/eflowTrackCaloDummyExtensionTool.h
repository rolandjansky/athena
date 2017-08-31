#ifndef EFLOWTRACKCALODUMMYEXTENSIONTOOL_H_
#define EFLOWTRACKCALODUMMYEXTENSIONTOOL_H_

/********************************************************************

 NAME:     eflowTrackCaloDummyExtensionTool.h
 PACKAGE:  offline/Reconstruction/eflowRec

 AUTHORS:  T.J. Khoo
 CREATED:  6/6/2016

 ********************************************************************/

#include "eflowRec/eflowTrackExtrapolatorBaseAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <map>
#include "TGraphErrors.h"

class eflowTrackCaloPoints;

static const InterfaceID IID_eflowTrackCaloDummyExtensionTool("eflowTrackCaloDummyExtensionTool", 1, 0);

class eflowTrackCaloDummyExtensionTool: virtual public eflowTrackExtrapolatorBaseAlgTool, public AthAlgTool {

public:
  eflowTrackCaloDummyExtensionTool(const std::string& type, const std::string& name,
                             const IInterface* parent);
  ~eflowTrackCaloDummyExtensionTool();

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() override;
  virtual  std::unique_ptr<eflowTrackCaloPoints> execute(const xAOD::TrackParticle* track) const override;
  virtual StatusCode finalize() override;

private:
  void  FillEstimationMap(const char * filename, const char * graphname, eflowCalo::LAYER layer);
  Trk::CurvilinearParameters* CorrectLayerPosition(eflowCalo::LAYER layer, const xAOD::TrackParticle* track) const;

  bool                                        m_doMapEstimation;
  std::string                                 m_mapEstimationPath;
  std::string                                 m_parameterGraphName;
  std::map<eflowCalo::LAYER, TGraphErrors*>   m_mapEstimation;

};

inline const InterfaceID& eflowTrackCaloDummyExtensionTool::interfaceID() {
  return IID_eflowTrackCaloDummyExtensionTool;
}

#endif /* EFLOWTRACKCALODUMMYEXTENSIONTOOL_H_ */
