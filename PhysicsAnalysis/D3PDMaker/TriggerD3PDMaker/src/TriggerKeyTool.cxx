/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/TriggerKeyTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2011
 * @brief Logic to fetch the current trigger key.
 */


#include "TriggerD3PDMaker/TriggerKeyTool.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfigSvc/DSConfigSvc.h"
#include "SGTools/crc64.h"
#include "AthenaKernel/errorcheck.h"
#include <sstream>

// Note: this tool is failing in 20.7+ when used on MC. To be fixed if still needed used...

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TriggerKeyTool::TriggerKeyTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : AthAlgTool (type, name, parent),
    m_trigConfigSvc ("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_dsSvc ("TrigConf::DSConfigSvc/DSConfigSvc", name)
{
  declareInterface<TriggerKeyTool> (this);

  declareProperty ("TrigConfigSvc", m_trigConfigSvc,
                   "Trigger configuration service instance.");
  declareProperty ("DSSvc", m_dsSvc,
                   "DS configuration instance "
                   "(sometimes used when reading MC).");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TriggerKeyTool::initialize()
{
  CHECK( AthAlgTool::initialize() );
  CHECK( m_trigConfigSvc.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the current list of trigger keys.
 *
 * Failure is indicated by first==-1.
 */
TriggerKeyTool::Keys TriggerKeyTool::getKeys() const
{
  Keys keys =
    std::make_pair( m_trigConfigSvc->masterKey(),
                    std::make_pair( m_trigConfigSvc->lvl1PrescaleKey(),
                                    m_trigConfigSvc->hltPrescaleKey() ) );

  // If the keys returned by the configuration service don't seem to make sense,
  // use something else as the SMK. (Needed mostly for MC test jobs.)
  if ( (keys.first == 0 && keys.second.first == 0 && keys.second.second == 0) ||
       keys.first < 0 || keys.second.first < 0 || keys.second.second < 0 )
  {
    // See if we are reading an AOD:
    TrigConf::DSConfigSvc* dsSvc = 0;
    if (m_dsSvc)
      dsSvc = dynamic_cast<TrigConf::DSConfigSvc*> (&*m_dsSvc);
     
    if( ! dsSvc ) {
      REPORT_MESSAGE( MSG::ERROR )
        << "The trigger configuration keys don't seem to make sense, "
        << "and we're not using TrigConf::DSConfigSvc...";
      keys.first = -1;
    }
    else {
      // Turn the configuration source name (probably an XML file in this case)
      // into an imaginary Super Master Key:
      keys =
        std::make_pair( ( SG::crc64( dsSvc->configurationSource() ) & 0xffff ),
                        std::make_pair( 0, 0 ) );
    }
  }
  return keys;
}


/**
 * @brief Return a string describing the trigger configuration
 * (excluding prescales).
 *
 * Failure is indicated by an empty string.
 */
std::string TriggerKeyTool::getString() const
{
  int smk = m_trigConfigSvc->masterKey();
  if (smk > 0) {
    std::ostringstream ss;
    ss << "SMK" << smk;
    return ss.str();
  }

  if (m_dsSvc) {
    TrigConf::DSConfigSvc* dsSvc =
      dynamic_cast<TrigConf::DSConfigSvc*> (&*m_dsSvc);
    if (dsSvc)
      return dsSvc->configurationSource();
  }

  return "";
}


} // namespace D3PD
