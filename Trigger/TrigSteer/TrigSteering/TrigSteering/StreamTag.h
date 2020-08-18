/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @class  : StreamTag
 *
 * @brief  proerties of a DataFlow stream
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: StreamTag.h,v 1.6 2007-08-08 00:10:04 eifert Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_STREAMTAG_H
#define TRIGSTEERING_STREAMTAG_H

#include <vector>

#include "TrigConfHLTData/HLTStreamTag.h"



namespace HLT {

  class IScalerSvc;
  class IScaler;

  /**
     @class StreamTag
     This class represents one DataFlow streamTag

     @author Till Eifert     <Till.Eifert@cern.ch>
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
  */
  class StreamTag
  {

  public:

    StreamTag( const TrigConf::HLTStreamTag* configStream,
               HLT::IScalerSvc* scalerSvc ); //!< constructor for normal running

    ~StreamTag() = default; //!< destructor

    bool reset(); //!< Is called for every new event, resets everything to default.

    const TrigConf::HLTStreamTag* getConfigStream() const { return m_configStream; } //!< get underlying ConfigStream


    const std::string& getStream() const { return m_configStream->stream(); } //!< getter for stream name
    const std::string& getType() const { return m_configStream->type(); }     //!< getter for stream type
    bool getObeyLB()             const { return m_configStream->obeyLB(); }   //!< getter for obey LuminosityBlock
    bool isPrescaled()     const { return m_prescaled;     }                  //!< is chain prescaled ?
    float prescaleFactor()  const { return m_configStream->prescale(); }        //!< get prescale factor

    void setPrescaleState(); //!< set prescale state of this streamTag, similar to the overall prescale state of a chain !


  private:

    HLT::IScaler* m_preScaler; //!< engines to determine prescale/passThrough status

    const TrigConf::HLTStreamTag* m_configStream; //!< underlying Config StreamTag
    bool m_prescaled;                             //!< Flagged as "prescaled" for this event ?
    bool m_state;                                  //!< can we run this ? (call setPrescaleState())
  };


} // end of namespace


#endif
