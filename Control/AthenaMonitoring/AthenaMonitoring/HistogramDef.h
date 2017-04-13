/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_HistogramDef_h
#define AthenaMonitoring_HistogramDef_h

#include <stdexcept>
#include <vector>
#include <string>
 
namespace Monitored {
  struct HistogramDefParseException: public std::runtime_error {
    HistogramDefParseException(std::string const& message)
      : std::runtime_error(message) {}
  };
  /**
   * @brief the internal class used to keep parsed Filler properties
   *
   */
  struct HistogramDef {
    std::vector<std::string> name;  //!< names of variables
    std::string alias;              //!< histogram name alias
    std::string type;               //!< histogram type
    std::string path;               //!< booking path
    std::string title;              //!< title of the histogram
    std::string opt;                //!< options
    
    int xbins{0};  //!< number of bins in X
    float xmin{0}; //!< left
    float xmax{0}; //!< right
    
    int ybins{0};  //!< number of bins in Y
    float ymin{0}; //!< bottom
    float ymax{0}; //!< top
    
    float zmin{0}; //!< in
    float zmax{0}; //!< out    
    
    bool  ok{false};    //!<  good declaration
    bool  ycut{false};  //!<  TProfile with cut on y
    bool  zcut{false};  //!<  TProfile2D with cut on z
    
    std::vector<std::string> labels; //!< bins labels
    
    static const HistogramDef parse(const std::string& jobOpts); //!< utility method to parse JO  
  };
}
  
#endif /* AthenaMonitoring_HistogramDef_h */