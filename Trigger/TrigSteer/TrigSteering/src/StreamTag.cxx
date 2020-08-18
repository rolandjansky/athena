/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Streamtag
 *
 * @brief  proerties of a DataFlow stream
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: StreamTag.cxx,v 1.5 2007-08-08 00:10:04 eifert Exp $
 **********************************************************************************/


#include "TrigSteering/StreamTag.h"

#include "TrigSteering/Scaler.h"
#include "TrigSteering/ScalerSvc.h"

#include <sstream>
#include <iostream>

using namespace HLT;



StreamTag::StreamTag(const TrigConf::HLTStreamTag* configStream,
                     HLT::IScalerSvc* scalerSvc )
  : m_configStream( configStream ),
    m_prescaled( false ),
    m_state(false)
{
  // For now use default -- at some point StreamTag should get "ScalerType" attributes.
  if (scalerSvc) {
    m_preScaler = scalerSvc->get("");
    if (m_preScaler) m_state = true;
  }
}


bool StreamTag::reset()
{
  m_prescaled = false;
  return true;
}


void StreamTag::setPrescaleState()
{
  // kill this stream tag of prescale is set to 0 == infinity
  if ( prescaleFactor() < 0.1  && prescaleFactor() > -0.1 ) {
    m_prescaled = true;
    return;
  }

  // prescale == 1 -> do nothing
  // prescale == N -> reject (N-1) events out of N
  if (m_state && prescaleFactor() > 1.)
    m_prescaled = ! m_preScaler->decision( prescaleFactor() );
}
