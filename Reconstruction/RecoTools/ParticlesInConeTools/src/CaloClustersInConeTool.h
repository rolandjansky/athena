/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLESINCONETOOLS_CALOCLUSTERSINCONETOOL_H
#define PARTICLESINCONETOOLS_CALOCLUSTERSINCONETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ParticlesInConeTools/ICaloClustersInConeTool.h"
#include "IParticlesLookUpTable.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "SGTools/CLASS_DEF.h"
 
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

    /** retrieve id track particles */
    const CaloClusterContainer* retrieveCaloClusterContainer() const; 

    /** ID track collection name */
    std::string m_caloClusterLocation;
  };

}	// end of namespace


CLASS_DEF( xAOD::CaloClustersInConeTool::LookUpTable, 192366895, 0 )

#endif


