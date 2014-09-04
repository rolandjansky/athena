/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_INCIDENTS_H
#define TRIGINTERFACES_INCIDENTS_H

/**
 * @file   Incidents.h
 * @brief  Incident classes used in HLT
 * @author Frank Winklmeier
 *
 * $Id: Incidents.h,v 1.1 2008-12-03 17:24:52 fwinkl Exp $
 */

// Gaudi includes
#include "GaudiKernel/Incident.h"

/**
 * @namespace HLT::Incidents
 * @brief HLT incident classes
 *
 * Every incident is derived from Gaudi's Incident class. Moreover,
 * the method type() is statically overloaded to return the type
 * of the incident (usually the same string as the class name).
 * This avoids the use of hardcoded strings in Incident sender and
 * listener. Typically, you would use them as follows:
 *
 * Sender:
 * \code
 * incSvc->fireIncident(Incidents::EventTimeout())
 * \endcode
 *
 * Listener:
 * \code
 * void MyAlgorithm::handle(const Incident& inc)
 * {
 *   if ( inc.type()==Incidents::EventTimeout::type() ) {...}
 * }
 * \endcode
 */

namespace HLT { namespace Incidents {

  /**
   * @class  EventTimeout
   * @brief  Event timeout incident
   *
   * This incident is fired by the PSC to indicate an event timeout
   * during online running. Only the steering should subscribe to
   * this incident since it requires a thread-safe incident handler.
   */
  class EventTimeout: public Incident {
  public:
    /**
     * Constructor
     * @param source    sender of the incident
     */
    EventTimeout(const std::string& source) :
      Incident(source, type()) {};

    /// Incident type
    static const std::string& type();
  };
  

  /**
   * @class  UpdateHLTPrescales
   * @brief  Incident to signal update of HLT prescales
   *
   * This incident is fired by the PSC to signal that the HLT prescales
   * have to be reloaded. It carries the "super lumi block" ID that is
   * incremented by the CTP for each update.
   */
  class UpdateHLTPrescales: public Incident {
  public:    
    /**
     * Constructor
     * @param super_lumi_block   super lumi block number
     * @param source             sender of the incident
     */
    UpdateHLTPrescales(unsigned int super_lumi_block,
                       const std::string& source) :
      Incident(source, type()), m_superLumiBlock(super_lumi_block) {};

    /// Incident type
    static const std::string& type();

    /// super lumi block number
    unsigned int super_lumi_block() const { return m_superLumiBlock; }

  private:
    unsigned int m_superLumiBlock;      //!< super lumi block ID
  };


  
  //<<<<<<<<<<< INLINE MEMBER FUNCTIONS

  inline
  const std::string& EventTimeout::type()
  {
    static const std::string _type("EventTimeout");
    return _type;
  }
  
  inline
  const std::string& UpdateHLTPrescales::type()
  {
    static const std::string _type("UpdateHLTPrescales");
    return _type;
  }

  //<<<<<<<<<<< INLINE MEMBER FUNCTIONS

  
}} // namespace HLT::Incidents

#endif
