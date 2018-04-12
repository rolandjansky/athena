// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEVENTFORMATCNV_EVENTFORMATSVC_H
#define XAODEVENTFORMATCNV_EVENTFORMATSVC_H

// System include(s):
#include <string>
#include <set>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "StoreGate/StoreGateSvc.h"

// xAOD include(s):
#include "xAODEventFormat/EventFormat.h"

// Local include(s):
#include "xAODEventFormatCnv/IEventFormatSvc.h"

namespace xAODMaker {

   /**
    *  @short Implementation of the IEventFormatSvc service interface
    *
    *         This class takes care of managing the xAOD::EventFormat
    *         objects of the xAOD files.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 721421 $
    * $Date: 2016-02-02 16:17:05 +0100 (Tue, 02 Feb 2016) $
    */
   class EventFormatSvc : public AthService,
                          public virtual IEventFormatSvc,
                          public virtual IIncidentListener {

   public:
      /// Regular Service constructor
      EventFormatSvc( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the service
      virtual StatusCode initialize();

      /// @name Implementation of the IEventFormatSvc interface
      /// @{

      /// Get the object describing the current input/output file's format
      const xAOD::EventFormat* eventFormat();

      /// @}

      /// Function handling the incoming incidents
      virtual void handle( const Incident& inc );

      /// Function communicating the implemented interfaces to Gaudi
      virtual StatusCode queryInterface( const InterfaceID& riid,
                                         void** ppvIf );

   private:
      /// Fill the internal object with metadata from the event
      int collectMetadata( xAOD::EventFormat& ef );

      /// StoreGate key of the EventFormat object
      std::string m_objectName;

      /// Connection to the CLID service
      ServiceHandle< IClassIDSvc > m_clidSvc;
      /// Connection to the event store
      ServiceHandle< StoreGateSvc > m_eventStore;
      /// Connection to the input metadata store
      ServiceHandle< StoreGateSvc > m_inputMetaStore;
      /// Connection to the metadata store
      ServiceHandle< StoreGateSvc > m_metaStore;
      /// Connection to the incident service
      ServiceHandle< IIncidentSvc > m_incidentSvc;
      /// We automatically generate event format information for type names
      /// that contain the string `xAOD'.  This property gives a list of
      /// additional type names that should have format information.
      std::vector< std::string > m_formatNames;

      /// CLIDs about which warnings have already been printed
      std::set< CLID > m_warnedCLIDs;

      /// The object that we write out in the end
      xAOD::EventFormat* m_ef;

      /// Helper variable detecting the first event in the processing
      bool m_firstEvent;

   }; // class EventFormatSvc

} // namespace xAODMaker

#endif // XAODEVENTFORMATCNV_EVENTFORMATSVC_H
