/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 Athena processor which will dump the trigger counts either to an XML file or standard out.
 @author: R. Moore <rwmoore@ualberta.ca>
 */

#ifndef TRIGVALALGS_TRIGCOUNTDUMPER_H
#define TRIGVALALGS_TRIGCOUNTDUMPER_H

// std stuff
#include <string>
#include <map>
#include <iostream>

// Xerces C++ Includes
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOM.hpp"

// Core ATLAS includes
#include "AthenaBaseComps/AthAlgorithm.h"

// Trigger includes
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

namespace trigcount {
  
  /**
   * @class TriggerCount
   * Helper class which will store the data for a single trigger count
   */
  class TriggerCount {
  public:
    /** Constructs the count for a given trigger.
     @param name name of the trigger being counted
     @param prescale prescale value for the trigger
     */
    TriggerCount(const std::string &name="",float prescale=1.0,float passthrough=0.);
    /** Increments the raw and actual counts for this trigger.
     Every call of this method will increment the raw counts for this
     trigger. The actual count will only be increased if the argument
     'actual' is set to true.
     @param actual flag to indicate that the actual count should be increased
    */
    void addEvent(bool actual);
    /// Returns the raw trigger count
    uint32_t raw(void) const;
    /// Returns the actual trigger count
    uint32_t actual(void) const;
    /// Returns the prescale factor
    float prescale(void) const;
    /// Returns the pass-through factor
    float passthrough(void) const;
    /** Returns the projected count based on the raw and prescale values.
     This method will project an actual count number based on the number of
     raw counts and the given prescale factor.
     @return projected number of trigger counts
    */
    float projected(void) const;
    /** Fills an XML element structure with the count data.
     This method will create an XML element structure using the given XML
     document and then returns the top element of the structure. It is then up
     to the calling code to attach this structure to the document in a suitable
     location.
     @param doc pointer to the Xerces DOM document
     @return pointer to the top level element of the XML representation
     */
    xercesc::DOMElement *xml(xercesc::DOMDocument *doc) const;
    /// Outputs the count to a stream
    std::iostream &operator<<(std::iostream &ostr) const;
    /** Returns the name of the trigger.
     @return name of the trigger being counted
    */
    const std::string &name(void) const;
    /** Returns the name of the lower chain name.
      @return name of the lower chain name of this trigger
    */
    const std::string &lower(void) const;
    /** Sets the name of the lower chain name.
     @param lower the name of the lower chain name of this trigger
    */
    void setLower(const std::string &lower);
  private:
    /// Name of the trigger
    std::string m_name;
    /// Actual counts for this trigger
    uint32_t m_count;
    /// Raw counts for this trigger
    uint32_t m_raw;
    /// Prescale factor for this trigger
    float m_prescale;
    /// Passthrough factor for this trigger
    float m_passthrough;
    /// Lower chain name this trigger
    std::string m_lower;
  };
  
  inline TriggerCount::TriggerCount(const std::string &name,float prescale,float passthrough) : 
    m_name(name), m_count(0), m_raw(0), m_prescale(prescale), m_passthrough(passthrough), m_lower("") {
  }
  
  inline void TriggerCount::addEvent(bool actual) {
    // Increase the raw event count
    ++m_raw;
    // Only increase the actual event count if the flag is set
    if(actual) ++m_count;
  }
  
  inline float TriggerCount::projected(void) const {
    if(m_prescale!=0.) {
      return static_cast<float>(m_raw)/m_prescale+static_cast<float>(m_raw)*m_passthrough
      -static_cast<float>(m_raw)*(m_passthrough/m_prescale);
    } else {
      return static_cast<float>(m_raw)*m_passthrough;
    }
  }

  inline uint32_t TriggerCount::raw(void) const {
    return m_raw;
  }
  
  inline uint32_t TriggerCount::actual(void) const {
    return m_count;
  }
  
  inline float TriggerCount::prescale(void) const {
    return m_prescale;
  }
  
  inline float TriggerCount::passthrough(void) const {
    return m_passthrough;
  }
  
  inline const std::string &TriggerCount::name(void) const {
    return m_name;
  }
  
  inline const std::string &TriggerCount::lower(void) const {
    return m_lower;
  }
  
  inline void TriggerCount::setLower(const std::string &lower) {
    m_lower=lower;
  }
  

  class TriggerMenu : public std::map<std::string,TriggerCount> {
  public:
    /** Constructs a new trigger menu class to contain counts.
     @param smk super master key of the trigger list
     */
    TriggerMenu(uint32_t smk=0);
    /// Copy constructor
    /** Adds an event with the given number of interactions to those the menu has seen.
     This increments the number of events which have been triggered by this menu by
     one and increases the number of interactions by the number of interactions in
     the event.
     @param nint number of interactions in the event, defaults to 1
     */
    void addEvent(uint32_t nint=1);
    /** Adds a new trigger count to the menu.
     @param tcount trigger count to add
    */
    void addTrigger(const TriggerCount &tcount);
    /** Returns the average number of interactions per event.
     This calculates the average number of interactions in events which were triggered
     by this menu.
     @return the average number of interactions per event.
     */
    float mu(void) const;
    /** Fills an XML element structure with the menu data.
     This method will create an XML element structure using the given XML
     document and then returns the top element of the structure. It is then up
     to the calling code to attach this structure to the document in a suitable
     location.
     @param doc pointer to the Xerces DOM document
     @return pointer to the top level element of the XML representation
    */
    xercesc::DOMElement *xml(xercesc::DOMDocument *doc) const;
    /// Outputs the menu to a stream
    std::iostream &operator<<(std::iostream &ostr) const;
  private:
    /// Super master key associated with this trigger list
    uint32_t m_smk;
    /// Number of events which have been run with this trigger menu
    uint32_t m_count;
    /// Number of interactions in this menu's events: used to calculate mu
    uint32_t m_ninteractions;
  };
  
  inline TriggerMenu::TriggerMenu(uint32_t smk) : 
    std::map<std::string,TriggerCount>(), m_smk(smk), m_count(0), m_ninteractions(0) {
  }
  
  inline void TriggerMenu::addEvent(uint32_t nint) {
    m_count++;             // Increment the number of events
    m_ninteractions+=nint; // Add the number of interactions
  }

  inline void TriggerMenu::addTrigger(const TriggerCount &tcount) {
    (*this)[tcount.name()]=tcount;
  }

  inline float TriggerMenu::mu(void) const {
    return static_cast<float>(m_ninteractions)/static_cast<float>(m_count);
  }
  
} // end namespace


/**
 * @class TrigCountDumper
 * Simple Athena algorithm which will dump the trigger counts into suitable formats.
 *
 * @author Roger Moore     <rwmoore@ualberta.ca>     - University of Alberta, Canada
 */
class TrigCountDumper : public AthAlgorithm {
 public:
  /** Standard algorithm constructor.
  */
  TrigCountDumper(const std::string &name, ISvcLocator *pSvcLocator);
  /** Standard algorithm destructor.
  */
  ~TrigCountDumper(void);
  /** Initializes the count dump algoritm from the job options.
      This method sets up the configuration options of the dump algorithm, such as the
      type of output to generate and the file to store it in etc.
   */
  StatusCode initialize(void);
  /** Called by Athena once per event to execute the dumper on the current event.
      This method is called once per event and collects the trigger counts for that single
      event. These are then stored within the class instance.
  */
  StatusCode execute();
  /** Called by Athena once all the events have been processed to output the stored counts.
      This method is executed at the end of a run and will write out the stored counts using the
      configured format to the configured location.
   */
  StatusCode finalize();
 private:
  /// Handle to access the trigger decision tool
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;
  /// The trigger configuration service to get the information from
  ServiceHandle< TrigConf::ITrigConfigSvc > m_configSvc;
  /// Handle used (sometimes) when reading MC files
  ServiceHandle< TrigConf::ITrigConfigSvc > m_dsSvc;
  /// Type definition for the menu map
  typedef std::map<uint32_t,trigcount::TriggerMenu> MenuMap_t;
  /// Map of super master keys to trigger menus
  MenuMap_t m_menu;
  /// Name of output file
  std::string m_ofname;
  /// Name of event info
  std::string m_eventInfoName;
  /// Release name
  std::string m_release;
  /// Dataset name
  std::string m_dataset;
  /// Dataset labels
  std::vector<std::string> m_labels;
};

inline TrigCountDumper::~TrigCountDumper(void) {
}

#endif
