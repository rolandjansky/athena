#ifndef TrigEgammaHypo_TrigL2ElectronViewsMergeTool_h
#define TrigEgammaHypo_TrigL2ElectronViewsMergeTool_h


#include "AthViews/IViewsMergerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "AthViews/View.h"
#include "AthContainers/ConstDataVector.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"


class TrigL2ElectronViewsMergerTool : virtual public IViewsMergerTool, virtual public AthAlgTool {
public:
  TrigL2ElectronViewsMergerTool( const std::string& type, 
				 const std::string& name, 
				 const IInterface* parent );
  StatusCode initialize() override;
  StatusCode merge( const EventContext& context, const std::vector<SG::View*>& ) const override;

private:
  SG::ReadHandleKey< xAOD::TrigElectronContainer > m_inViewElectronsKey {this, "InViewElectrons", "L2ElectronContainerInView", "Input"};
  SG::WriteHandleKey< ConstDataVector<xAOD::TrigElectronContainer> > m_mergedElectronsKey {this, "MergedElectrons", "L2ElectronContainerMerged", "Output"};
};


#endif
