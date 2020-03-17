/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_ATHVIEWS_MINIMALVIEWALG_H
#define ATHVIEWS_ATHVIEWS_MINIMALVIEWALG_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthViews/View.h"
#include "GaudiKernel/IScheduler.h"

namespace AthViews {

class MinimalViewAlg : public AthAlgorithm
{
  public:

    // Constructor with parameters
    MinimalViewAlg( const std::string& name, ISvcLocator* pSvcLocator );

    // Destructor
    virtual ~MinimalViewAlg();

    // Athena algorithm hooks
    virtual StatusCode  initialize();
    virtual StatusCode  execute();
    virtual StatusCode  finalize();

  private:

    // Default constructor
    MinimalViewAlg();

    // Configurables
    ServiceHandle< IScheduler > m_scheduler{ this, "Scheduler", "AvalancheSchedulerSvc", "The Athena scheduler" };
    Gaudi::Property< std::string > m_viewNodeName{ this, "ViewNodeName", "", "Name of CF node to attach views to" };
};

} //> end namespace AthViews

#endif //> !ATHVIEWS_ATHVIEWS_MINIMALVIEWALG_H
