/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramDef_h
#define AthenaMonitoringKernel_HistogramDef_h

#include <string>
#include <vector>

namespace Monitored {
  /**
   * @brief the internal class used to keep parsed Filler properties
   */
  struct HistogramDef {
    std::vector<std::string> name; //!< names of monitored variables
    std::string alias; //!< unique alias for THistSvc
    std::string type; //!< class name
    std::string path; //!< booking path
    std::string title; //!< title of the histogram
    std::string opt; //!< options
    std::string tld{""}; //!< top level directory (below THistSvc stream)
    std::string convention; //!< path naming convention (e.g. OFFLINE)
    std::string weight; //!< name of weight variable
    std::string cutMask; //!< variable that defines whether event is accepted

    std::string xvar; //!< name of x variable
    int xbins; //!< number of y bins
    float xmin; //!< x axis minimum
    float xmax; //!< x axis maximum
    std::vector<std::string> xlabels; //!< labels for x axis
    std::vector<double> xarray; //!< array of x bin edges

    std::string yvar; //!< name of y variable
    int ybins; //!< number of y bins
    float ymin; //!< y axis minimum
    float ymax; //!< y axis maximum
    std::vector<std::string> ylabels; //!< labels for y axis
    std::vector<double> yarray; //!< array of y bin edges

    std::string zvar; //!< name of z variable
    int zbins; //!< number of z bins
    float zmin; //!< z axis minimum
    float zmax; //!< z axis maximum
    std::vector<std::string> zlabels; //!< labels for z axis

    bool ok{false}; //!< good declaration

    /**
     * @brief Parses histogram defintion from json data.
     *
     * @param histogramDefinition A string contains histogram definition to parse
     * @return HistogramDef Object that contains the parsed histogram definition
     */
    static const HistogramDef parse(const std::string& histogramDefinition);
  };
} // namespace Monitored

#endif /* AthenaMonitoringKernel_HistogramDef_h */
