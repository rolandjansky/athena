/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
 * @brief Basic monitoring tool for the HLT trigger.
 *
 * That tool works as follows. It is supposed to be attached to HLT algorithms (put to the list of AthenaMonTools of the algorithm.) 
 * During initialization it reads JO and also the list of monitoring variables exported by the algorithm (via HLT::IMoniotredAlgo interface).
 * Then for each declared histogram simple helper classes are created which span hisogram defined and the variable to be monitorred. (they are called HistogramFiller1D, HistogramFiller2D, ...).
 * The varaibles which are not mentioned in the configuration, this for which histograms are not defined, are just silently ignored.
 * Other way around is not the case. If there is request to create some histogram for variable not beeing exported by the algorithms the WARNING message is printed.
 *
 * During execution (in fillHists() method) the loop over all histogram filles is executed and internally varaibles are picked up and used to fill histograms.
 *
 * The JO is an list of histogram definitions of the following form:
 * PATH, HTYPE, NAME, TITLE, BINNING ...., OPTIONS

 * Example configuration strings:
 * ["/SHIFT, TH1D, name, title, xbins, xmin, xmax, opt"]
 * ["/EXPERT, TH2D, "name1,name2", title, xbins, xmin, xmax, ybins, ymin, ymax, opt"]
 * In last case the decision to create and fill 2D histograms happens at configuration time.
 * Name specifier can get additional parameter which is an alias. This alias will be used for naming actual histogram.
 * ex. name;alias for TH1* , name1,name2;alias for TH2*
 *
 *
 *
 * Following hisogram types are supported: TH1(F|I|D), TH2(F|I|D). 
 * Following paths are supported: EXPERT, SHIFT, DEBUG, RUNSTAT
 * Following options are suppored: kCanRebin, kCummulative
 * -# kCanRebin enables ROOT (http://root.cern.ch) internal functionality of autobinning of the histogram.
 * -# kCumulative does fill of all bins left to the bin into which the value falls, sort of distribuant
 * -# kLBN does make the histogram LBN aware (means many histograms)
 * -# kVec adds the content of the monitored varaible to the bins as if monitored varaible would be another histogram
 * -# kVecUO adds the content of the monitored varaible to the bins as if monitored varaible would be another histogram treating oth and last elements as under/overflows
 *
 * @author Tomasz Bold <Tomasz.Bold@cern.ch>
 * @author Wang Meng
 */

class GenericMonitoringTool : public AthAlgTool {
public:
	static const InterfaceID& interfaceID();

  GenericMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent);
  virtual ~GenericMonitoringTool();
  
  virtual StatusCode initialize();
  virtual std::vector<Monitored::HistogramFiller*> getHistogramsFillers(std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> monitoredVariables);
private:   
  ServiceHandle<ITHistSvc> m_histSvc;  
  std::vector<Monitored::HistogramFiller*> m_fillers;              //!< list of fillers
  std::string m_histogramsGroupName;                               //!< property (name of group to which histograms would be generated)
  std::vector<std::string> m_histograms;                           //!< property (list of histogram definitions)
};

#include "AthenaMonitoring/MonitoredScope.h"
#include "AthenaMonitoring/MonitoredScalar.h"
#include "AthenaMonitoring/MonitoredCollection.h"

#endif /* AthenaMonitoring_GenericMonitoringTool_h */