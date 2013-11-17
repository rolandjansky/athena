// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGOBJECTMATCHING_TRIGMATCHTOOL_H
#define TRIGOBJECTMATCHING_TRIGMATCHTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"

// Local include(s):
#include "TrigMatchToolCore.h"

class Incident;

static const InterfaceID IID_TrigMatchTool("TrigMatchTool", 1, 0);

/**
 * @brief The TrigMatchTool is an athena tool used to match reconstructed
 *        objects to trigger objects and chains.  Please see TrigMatchToolCore
 *        for more information.
 */
class TrigMatchTool : public AthAlgTool,
                      public TrigMatchToolCore,
                      virtual public IIncidentListener {

public:
   TrigMatchTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent );

   static const InterfaceID& interfaceID() {
      return IID_TrigMatchTool;
   }

   virtual StatusCode initialize();

private:
   // for printing warnings
   virtual void warning( const std::string& w ) {
      ATH_MSG_ERROR( w );
      return;
   }

   // status functions for determining what information we have
   // access to
   virtual bool changedDecisionAware() { return true; };

   // handle incidents (end event, begin event, etc.)
   virtual void handle( const Incident& inc );

   // grab the configured chain names via the trig decision tool
   virtual void assertConfiguredChainNames();

   // grab the feature containers from the navigation
   // via the trig decision tool
   virtual Trig::FeatureContainer
   getFeatureContainer( const std::string &chainName, const int condition ) {
      return m_trigDecisionTool->features( chainName, condition );
   }

   // use the tdt to get access the the name of the chain that
   // seeded the current chain
   std::string lowerChainName( const std::string& chainName );
    
   // note that we access the trig decision tool normally
   // in athena
   ToolHandle< Trig::TrigDecisionTool > m_trigDecisionTool;

}; // end TrigMatchTool declaration

#endif // TRIGOBJECTMATCHING_TRIGMATCHTOOL_H
