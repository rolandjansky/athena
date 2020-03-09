/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLESINCONETOOLS_CALOCLUSTERSINCONETOOL_H
#define PARTICLESINCONETOOLS_CALOCLUSTERSINCONETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "ParticlesInConeTools/ICaloClustersInConeTool.h"
#include "IParticlesLookUpTable.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "AthenaKernel/CLASS_DEF.h"
 
namespace xAOD {

  class CaloClustersInConeTool: public AthAlgTool, virtual public ICaloClustersInConeTool {
  public:
    /** constructor */
    CaloClustersInConeTool(const std::string& type, const std::string& name, const IInterface* parent);

    /** destructor */
    virtual ~CaloClustersInConeTool(void); 
  
    /** initialize */
    virtual StatusCode initialize() override;

    /** finalize */
    virtual StatusCode finalize() override;

    /**ICaloClustersInConeTool interface */    
    virtual bool particlesInCone( float eta, float phi, float dr, std::vector< const CaloCluster*>& output ) const override;

    /**ICaloClustersInConeTool interface */    
    virtual bool particlesInCone( float eta, float phi, float dr, std::vector< ElementLink<CaloClusterContainer> >& output ) const override;

    typedef IParticlesLookUpTable<CaloCluster> LookUpTable;

  private:
    // init look-up table
    const LookUpTable* getTable() const;

    /** ID track collection name */
    SG::ReadHandleKey<CaloClusterContainer> m_caloClusterLocation {this,
	"CaloClusterLocation", "CaloCalTopoClusters"};
  };

}	// end of namespace


CLASS_DEF( xAOD::CaloClustersInConeTool::LookUpTable, 192366895, 0 )

#endif


