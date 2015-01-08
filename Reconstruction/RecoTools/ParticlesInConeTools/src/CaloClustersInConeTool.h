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
#include "GaudiKernel/IIncidentListener.h"
 
class IIncidentSvc;
 
namespace xAOD {

  class CaloClustersInConeTool: public AthAlgTool, virtual public ICaloClustersInConeTool, virtual public IIncidentListener {
  public:
    /** constructor */
    CaloClustersInConeTool(const std::string& type, const std::string& name, const IInterface* parent);

    /** destructor */
    ~CaloClustersInConeTool(void); 
  
    /** initialize */
    StatusCode initialize();

    /** finalize */
    StatusCode finalize();

    /**ICaloClustersInConeTool interface */    
    bool particlesInCone( float eta, float phi, float dr, std::vector< const CaloCluster*>& output );

    /**ICaloClustersInConeTool interface */    
    bool particlesInCone( float eta, float phi, float dr, std::vector< ElementLink<CaloClusterContainer> >& output );

    /** incident to clear cache at end of the event */
    void handle(const Incident& inc);

  private:
    // init look-up table
    bool initLookUp();

    /** retrieve id track particles */
    const CaloClusterContainer* retrieveCaloClusterContainer() const; 

    /** ID track collection name */
    std::string m_caloClusterLocation;
    
    /** look-up table */
    IParticlesLookUpTable<CaloCluster> m_lookUpTable;

    /** incident service */
    ServiceHandle< IIncidentSvc >      m_incidentSvc;
  };

}	// end of namespace

#endif


