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
   * Provides the prescales loaded from XML for individual chains and will read and fill a map with
   * the enhanced bias weights for a run
   */
  class TrigXMLService {
  
   public:

    static TrigXMLService& trigXMLService(); //!< Use this method to get the singleton

    Double_t getPrescale( const std::string& _name );
    Float_t  getEventWeight(UInt_t _eventNumber);

    void parseEnhancedBiasXML();
    void exportEnhancedBiasXML(UInt_t _eventNumber, Float_t _weight, UInt_t _bunchGroup);
    void saveExportedEnhancedBiasXML();
    
   private:

    void parseMenuXML(); 
    void parsePrescaleXML(UInt_t _xmlID);
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
    StringDoubleMap_t m_chainPS; //!< Holds chain prescales
    StringIntMap_t    m_chainPT; //!< Holds chain passthrough
    StringDoubleMap_t m_chainRerunPS; //!< Holds chain re-run prescales
    Bool_t            m_serviceEnabled; //!< True if the XML has been fully parsed

    StringIntMap_t    m_chainEvtPassed; //!< Events passed (TrigCostPython import)
    StringFloatMap_t  m_chainEvtPassedWeighted; //!< Events passed weighted (TrigCostPython import)
    StringFloatMap_t  m_chainRate; //!< Chain rate (TrigCostPython import)
    StringFloatMap_t  m_chainRateErr; //!< Error on chain rate (TrigCostPython import)
    StringFloatMap_t  m_chainEff; //!< Chain efficiency (TrigCostPython import)
    StringFloatMap_t  m_chainEffErr; //!< Error on chain efficiency (TrigCostPython import)
    StringFloatMap_t  m_chainPSEff; //!< Chain Prescale Efficiency (TrigCostPython import)
    StringFloatMap_t  m_chainPSEffErr; //!< Error on chain Prescale Efficiency (TrigCostPython import)

    // For converting and writing EB rates
    IntIntMap_t                                     m_ebWeightingMapSave; //!< Map of event numbers to enhanced bias weight IDs populated on exporting to XML
    PairStringFloat_Float_Map_t                     m_ebWeightTypesCount; //!< Counter of how many events of each enhanced bias weight are in the file, statistics display only.
    std::vector< std::pair<std::string, Float_t> >  m_ebWeightTypes; //!< Vector linking the location of the vector (ID) to the vector contents (an ehanced bias weight value + BG)

    // For reading and using EB weights
    Bool_t        m_weightsServiceEnabled; //!< True when ready to supply EB weights
    IntIntMap_t   m_ebWeightingMap;        //!< Used to map EB weights to IDs
    IntFloatMap_t m_idToWeightMap;         //!< Map weight-ID to weight
    IntIntMap_t   m_idToBGMap;             //!< Map weight-ID to bunchgroup ID

  }; //class TrigXMLService
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_TrigXMLService_H
