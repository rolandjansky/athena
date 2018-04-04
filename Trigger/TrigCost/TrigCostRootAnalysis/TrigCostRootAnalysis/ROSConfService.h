// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_ROSConfService_H
#define TrigCostRootAnalysis_ROSConfService_H

// STL include(s):
#include <map>

// ROOT include(s):
#include <Rtypes.h>

// Local include(s):
#include "Utility.h"

namespace TrigCostRootAnalysis {
  /**
   * @class ROSConfService
   * Service class to read in ROS config XMLs and make the data availalbe to other classes.
   */
  class ROSConfService {
  public:
    static ROSConfService& rosConfService(); //!< Use this method to get the singleton

    const std::string& getRobinNameFromId(UInt_t Id);
    const std::string& getRosNameFromFromRobinName(const std::string& robinName);
  private:
    void parseRosXml();

    StringStringMap_t m_RobinToRosMap;
    IntStringMap_t m_RobToRobinMap;
    Bool_t m_serviceEnabled;

    /**
     * Private constructor.
     */
    ROSConfService();

    /**
     * Private destructor. Empty
     */
    ~ROSConfService() {
    };

    /**
     * Private unimplemented copy constructor.
     */
    ROSConfService(ROSConfService const&);

    /**
     * Private unimplemented assignment constructor.
     */
    void operator = (ROSConfService const&);
  }; //class ROSConfService
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_ROSConfService_H
