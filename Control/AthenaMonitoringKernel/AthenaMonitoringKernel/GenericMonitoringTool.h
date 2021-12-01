/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_GenericMonitoringTool_h
#define AthenaMonitoringKernel_GenericMonitoringTool_h

#include <functional>
#include <vector>
#include <mutex>
#include <memory>

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "GaudiKernel/ITHistSvc.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

/* Here, by forward declaring these two classes, which appear as parameters and values
   in GenericMonitoringTool functions only as pointers (not as the objects themselves),
   we can avoid including those class headers here, moving them to the implementation
   file instead. This prevents external dependancy of the public-facing tool on the
   internals of AthenaMonitoringKernel. */
namespace Monitored {
  /** Forward declaration of HistogramFiller for pointers in GenericMonitoringTool functions */
  class HistogramFiller;
  /** Forward declaration of IMonitoredVariable for pointers in GenericMonitoringTool functions */
  class IMonitoredVariable;
}

/**
 * @brief Generic monitoring tool for athena components
 *
 * The tool can be used standalone or attached to an algorithm, tool or service.
 * The variables to be monitored need to be exposed via the Monitored framework.
 * The histograms to be created are configured via the `Histograms` list property
 * and the format is defined via Monitored::HistogramDef.

 * Configuration from python should be done using GenericMonitoringTool.defineHistogram.
 * For the list of supported histogram types see Monitored::HistogramFillerFactory.
 *
 * @see @ref MonAPI
 */
namespace Monitored {
  class Group;
}
class GenericMonitoringTool : public extends<AthAlgTool, IIncidentListener> {
public:
  using extends::extends;

  virtual ~GenericMonitoringTool() override;
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;
  virtual StatusCode stop() override;
  void handle( const Incident& ) override;

  /// feed the fillers
  void invokeFillers(const std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>& monitoredVariables) const;  
  
  /// Book histograms
  StatusCode book();
  /// Overrride configured booking path
  void setPath( const std::string& newPath ) { m_histoPath = newPath; }

  virtual const ServiceHandle<ITHistSvc>& histogramService() const { return m_histSvc; }
  virtual uint32_t runNumber();
  virtual uint32_t lumiBlock();

private:
  void invokeFillersDebug(const std::shared_ptr<Monitored::HistogramFiller>& filler,
                          const std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>& monitoredVariables) const;
  
  /// THistSvc (do NOT fix the service type (only the name) to allow for a different implementation online
  ServiceHandle<ITHistSvc> m_histSvc { this, "THistSvc", "THistSvc", "Histogramming svc" };
  Gaudi::Property<std::string> m_histoPath { this, "HistPath", {}, "Directory for histograms [name of parent if not set]" };
  Gaudi::Property<std::vector<std::string> > m_histograms { this, "Histograms", {},  "Definitions of histograms", "OrderedSet<std::string>"};
  Gaudi::Property<bool> m_explicitBooking { this, "ExplicitBooking", false, "Do not create histograms automatically in initialize but wait until the method book is called." };
  Gaudi::Property<bool> m_failOnEmpty { this, "FailOnEmpty", true, "Fail in initialize() if no histograms defined" };
  BooleanProperty m_useCache { this, "UseCache", true, "Cache filler lookups" };
  BooleanProperty m_registerHandler { this, "RegisterHandler", true, "Use incident handler to make 'always book' plots (else only check once)" };

  std::vector<std::shared_ptr<Monitored::HistogramFiller>> m_fillers; //!< plain list of fillers
  std::vector<std::shared_ptr<Monitored::HistogramFiller>> m_alwaysCreateFillers; //!< fillers that need touching, usually empty
  mutable std::map<std::vector<std::string>,std::unique_ptr<std::vector<std::shared_ptr<Monitored::HistogramFiller>>>,std::less<>> m_fillerCacheMap ATLAS_THREAD_SAFE; //!< lookup map to speed up filler searches
  mutable std::mutex m_cacheMutex;

};

/**
 * Helper class to declare an empty monitoring ToolHandle
 *
 * This can be used in case an empty monitoring tool needs to be declared in the constructor:
 *    declareProperty("MonTool", m_monTool=VoidMonitoringTool(this), "Monitoring tool");
 *
 * It is however preferred to instead use the initializer syntax in the header file:
 *    ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
 */
class VoidMonitoringTool : public ToolHandle<GenericMonitoringTool> {
public:
  VoidMonitoringTool(const IInterface* parent) :
    ToolHandle<GenericMonitoringTool>("", parent) {}
};

#endif /* AthenaMonitoringKernel_GenericMonitoringTool_h */
