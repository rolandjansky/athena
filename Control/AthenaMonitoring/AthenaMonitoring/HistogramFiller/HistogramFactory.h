/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_HistogramFiller_HistogramFactory_h
#define AthenaMonitoring_HistogramFiller_HistogramFactory_h

#include "TH1.h"
#include "TEfficiency.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "AthenaMonitoring/HistogramDef.h"
#include "AthenaMonitoring/HistogramFiller/HistogramException.h"

namespace Monitored {
  /**
   * @brief Bridge between ROOT framework and monitoring code
   * 
   * Main purpose of this class is to properly register and retrieve histograms for specific definitions.
   */
  class HistogramFactory {
  public:
    /**
     * @brief Defualt constructor
     * 
     * @param histSvc ROOT framework histogramming service
     * @param groupName Name of the group to which produced histograms will belong
     */
    HistogramFactory(const ServiceHandle<ITHistSvc>& histSvc, std::string groupName);
    /**
     * @brief Virtual destructor
     */
    virtual ~HistogramFactory() {}

    /**
     * @brief Book and register ROOT object for given definition
     * 
     * Based on the histogram type, method will return proper kind of the ROOT object
     * 
     * @param def Histogram definition 
     * @return ROOT object handler
     */
    TNamed* create(const HistogramDef& def);
  private:
    /**
     * @brief Create and register histogram
     * 
     * If histogram already exists under that name, re-use it
     * 
     * @tparam H Actual type of histogram to be created
     * @tparam HBASE Histogram base class (TH[1,2,3])
     * @tparam Types Types of the remaining arguments for TH constructor (except name, title)
     * @param def Histogram definition 
     * @param hargs Arguments that defines specific type of the histogram
     * @return Histogram handler
     */
    template<class H, class HBASE, typename... Types> 
    HBASE* create(const HistogramDef& def, Types&&... hargs);
    /**
     * @brief Helper for generic 'create' method for 1D histograms
     * 
     * @tparam H Actual type of histogram to be created
     * @param def Histogram definition 
     * @return Histogram handler
     */
    template<class H> 
    TH1* create1D(const HistogramDef& def);
    /**
     * @brief Helper for generic 'create' method for 1DProfile histograms
     * 
     * @tparam H Actual type of histogram to be created
     * @param def Histogram definition 
     * @return Histogram handler
     */
    template<class H> 
    TH1* create1DProfile(const HistogramDef& def);
    /**
     * @brief Helper for generic 'create' method for 2D histograms
     * 
     * @tparam H Actual type of histogram to be created
     * @param def Histogram definition 
     * @return Histogram handler
     */
    template<class H> 
    TH2* create2D(const HistogramDef& def);
    /**
     * @brief Helper for generic 'create' method for 2DProfile histograms
     * 
     * @tparam H Actual type of histogram to be created
     * @param def Histogram definition 
     * @return Histogram handler
     */
    template<class H> 
    TH2* create2DProfile(const HistogramDef& def);
    /**
     * @brief Create and register efficiency graph
     * 
     * If graph already exists under that name, re-use it
     * 
     * @param def Histogram definition 
     * @return Efficiency graph handler
     */
    TEfficiency* createEfficiency(const HistogramDef& def);

    /**
     * @brief Setup various histogram options
     * 
     * Available options: kCanRebin, Sumw2
     * 
     * @param hist Histogram handler 
     * @param opt Histogram options (from histogram definition)
     */
    static void setOpts(TH1* hist, const std::string& opt);
    /**
     * @brief Setup labels for histogram axes
     * 
     * @param hist Histogram handler 
     * @param labels Histogram labels (from histogram definition)
     */
    static void setLabels(TH1* hist, const std::vector<std::string>& labels);

    /**
     * @brief Invent path name
     *
     * If def path contains any of: EXPERT, SHIFT, DEBUG, RUNSTAT, EXPRES this is online 
     * convention this becomes the first element of the path followed by the group name.
     * Else if the def.path is DEFAULT then only the group name is used if the path yet 
     * different is concatenated with the group name.
     * 
     * @param def Histogram definition 
     * @return Efficiency graph handler
    */
    std::string getFullName(const HistogramDef& def);

    ServiceHandle<ITHistSvc> m_histSvc;
    std::string m_groupName; //!< defines location of histograms
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFactory_h */