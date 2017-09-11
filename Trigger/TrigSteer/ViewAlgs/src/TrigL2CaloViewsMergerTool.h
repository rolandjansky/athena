#ifndef TrigEgammaHypo_TrigL2CaloViewsMergeTool_h
#define TrigEgammaHypo_TrigL2CaloViewsMergeTool_h


#include "AthViews/IViewsMergerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "AthViews/View.h"
#include "AthContainers/ConstDataVector.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

class TrigL2CaloViewsMergerTool : virtual public IViewsMergerTool, virtual public AthAlgTool {
public:
  TrigL2CaloViewsMergerTool( const std::string& type, 
				 const std::string& name, 
				 const IInterface* parent );
  StatusCode initialize() override;
  StatusCode merge( const EventContext& context, const std::vector<SG::View*>& ) const override;

private:
  SG::ReadHandleKey< xAOD::TrigEMClusterContainer > m_inViewClustersKey {this, "InViewClusters", "L2CaloClustersInView", "Input"};
  SG::WriteHandleKey< ConstDataVector<xAOD::TrigEMClusterContainer> > m_mergedClustersKey {this, "MergedClusters", "L2CaloClustersMerged", "Output"};
};


#endif
