/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_GenericMonitoringTool_h
#define AthenaMonitoring_GenericMonitoringTool_h

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

#include "AthenaMonitoring/IMonitoredVariable.h"
#include "AthenaMonitoring/HistogramDef.h"
#include "AthenaMonitoring/HistogramFiller.h"

/**
 * @brief Generic monitoring tool for athena components
 *
 * The tool can be used standalone or attached to an algorithm, tool or service.
 * The variables to be monitored need to be exposed via the Monitored framework.
 * The histograms to be created are configured via the `Histograms` list property.
 * Each list entry has the form:
 *
 *   "PATH, HTYPE, NAME, TITLE, BINNING, [LABELS], OPTIONS"
 *
 * Example configuration strings:
 * - `"SHIFT, TH1D, name, title, xbins, xmin, xmax, opt"`
 * - `"EXPERT, TH2D, "name1,name2", title, xbins, xmin, xmax, ybins, ymin, ymax, opt"`
 *
 * As an option an alias can be provided for the naming of the actual histogram:
 *   `"name;alias"` or `"name1,name2;alias"`
 * For the configuration from python the helper GenericMonitoringTool.defineHistogram should be used.
 *
 * The following histogram types are supported:
 * - TH1[F,D,I]
 * - TH2[F,D,I]
 * - TProfile[2D]
 *
 * The following top-level paths are supported:
 * - EXPERT, SHIFT, DEBUG, RUNSTAT, EXPRESS
 *
 * The following options are suppored:
 * - `kCanRebin` enables ROOT's internal functionality of autobinning the histogram
 * - `kCumulative` does fill of all bins left to the bin into which the value falls
 * - `kLBN` makes the histogram lumiblock aware
 * - `kVec` adds the content of the monitored variable to the histogram bins
 * - `kVecUO` same as kVec but treat 0th(last) element as underflow(overflow)
 *
 * Optionally, a colon-separated list of bin labels ("bin1:bin2:bin3:") can be provided (at least one
 * colon is required). In case of a 2D histogram the labels are assigned consecutively to the x-axis 
 * and then y-axis bins.
 *
 * @author Tomasz Bold
 * @author Piotr Sarna
 */

class GenericMonitoringTool : public AthAlgTool {
public:

  GenericMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent);
  virtual StatusCode initialize() override;

  /// Retrieve the histogram fillers
  std::vector<Monitored::HistogramFiller*> getHistogramsFillers(std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> monitoredVariables);
  /// Book histograms
  StatusCode book();
  /// Overrride configured booking path
  void setPath( const std::string& newPath );

private:   
  ServiceHandle<ITHistSvc> m_histSvc       { this, "THistSvc", "THistSvc/THistSvc", "Histogramming svc" };  
  Gaudi::Property<std::string> m_histoPath { this, "HistPath", {}, "Directory for histograms [name of parent if not set]" };
  Gaudi::Property<std::vector<std::string> > m_histograms    { this, "Histograms", {},  "Definitions of histograms"};
  Gaudi::Property<bool> m_explicitBooking  { this, "ExplicitBooking", false, "Do not create histograms automatically in initialize but wait until the method book is called." };

  std::vector<Monitored::HistogramFiller*> m_fillers;      //!< list of fillers
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

#endif /* AthenaMonitoring_GenericMonitoringTool_h */
