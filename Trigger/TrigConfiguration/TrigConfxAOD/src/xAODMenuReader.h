// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMenuReader.h 612011 2014-08-15 13:43:57Z krasznaa $
#ifndef TRIGCONFXAOD_XAODMENUREADER_H
#define TRIGCONFXAOD_XAODMENUREADER_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Trigger include(s):
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfInterfaces/ITrigConfigTool.h"

namespace TrigConf {

   /**
    *  @short Algorithm for testing the xAOD menu reading in Athena
    *
    *         This algorithm is here to allow us to test reading the
    *         trigger configuration metadata in Athena through all the
    *         standard ways that we provide to the users.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 612011 $
    * $Date: 2014-08-15 15:43:57 +0200 (Fri, 15 Aug 2014) $
    */
   class xAODMenuReader : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      xAODMenuReader( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();

      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// Connection to the trigger configuration service
      ServiceHandle< TrigConf::ITrigConfigSvc > m_configSvc;
      /// Connection to the trigger configuration tool
      ToolHandle< TrigConf::ITrigConfigTool > m_configTool;

      /// An internal counter
      int m_eventCounter;

   }; // class xAODMenuReader

} // namespace TrigConf

#endif // TRIGCONFXAOD_XAODMENUREADER_H
