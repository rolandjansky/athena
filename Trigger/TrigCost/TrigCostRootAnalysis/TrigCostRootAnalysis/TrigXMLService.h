// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_TrigXMLService_H
#define TrigCostRootAnalysis_TrigXMLService_H

// STL include(s):
#include <map>

// ROOT include(s):
#include <Rtypes.h>

//Local include(s):
#include "Utility.h"

namespace TrigCostRootAnalysis {

  /**
   * @class TrigXMLService
   * Service class to read in ROS config XMLs and make the data availalbe to other classes.
   */
  class TrigXMLService {
  
   public:

    static TrigXMLService& trigXMLService(); //!< Use this method to get the singleton

    Int_t getPrescale( const std::string& _name );
    
   private:

    void parseMenuXML(); 
    void parsePrescaleXML();
    void writePrescaleXML();
    
    /**
     * Private constructor.
     */
    TrigXMLService();
    
    /**
     * Private destructor. Empty
     */
    ~TrigXMLService() {
    };
    
    /**
     * Private unimplemented copy constructor.
     */
    TrigXMLService(TrigXMLService const&);
    
    /**
     * Private unimplemented assignment constructor.
     */
    void operator=(TrigXMLService const&);

    std::string m_menuName; //!< The name of the loaded menu
    std::string m_prescalSetName; //!< The name of the loaded prescale set

    StringIntMap_t    m_chainCounter; //!< Holds chain counter ID number
    StringStringMap_t m_chainLowerLvl; //!< Holds string of seeding chains for HLT chain
    StringIntMap_t    m_chainPS; //!< Holds chain prescales
    StringIntMap_t    m_chainPT; //!< Holds chain passthrough
    StringIntMap_t    m_chainRerunPS; //!< Holds chain re-run prescales
    Bool_t            m_serviceEnabled; //!< True if the XML has been fully parsed

    StringIntMap_t    m_chainEvtPassed; //!< Events passed (TrigCostPython import)
    StringFloatMap_t  m_chainEvtPassedWeighted; //!< Events passed weighted (TrigCostPython import)
    StringFloatMap_t  m_chainRate; //!< Chain rate (TrigCostPython import)
    StringFloatMap_t  m_chainRateErr; //!< Error on chain rate (TrigCostPython import)
    StringFloatMap_t  m_chainEff; //!< Chain efficiency (TrigCostPython import)
    StringFloatMap_t  m_chainEffErr; //!< Error on chain efficiency (TrigCostPython import)
    StringFloatMap_t  m_chainPSEff; //!< Chain Prescale Efficiency (TrigCostPython import)
    StringFloatMap_t  m_chainPSEffErr; //!< Error on chain Prescale Efficiency (TrigCostPython import)

  }; //class TrigXMLService
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_TrigXMLService_H
