/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML__STREAMTOSERVERTOOL_H
#define JIVEXML__STREAMTOSERVERTOOL_H

//Interfaces
#include "JiveXML/IStreamTool.h"
#include "JiveXML/IServerSvc.h"
#include "JiveXML/EventStream.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

namespace JiveXML {
  
  /**
   * Stream event to an IServerSvc, which typically is a network server run from
   * and controlled by the Athena application.  A stream name is attached to the
   * events by doing so.
   *
   * @author Sebastian Boeser
   */
  class StreamToServerTool : virtual public IStreamTool, public AthAlgTool{

    public:
      
      /// Constructor
      StreamToServerTool( const std::string&, const std::string&, const IInterface*);

      /// Initialize
      virtual StatusCode initialize();
      
      /// Finalize
      virtual StatusCode finalize();

      /// Stream one event
      virtual StatusCode StreamEvent( const unsigned long EventNumber, const unsigned int RunNumber, const std::ostringstream* const EventBuffer ) ;

    protected:

      /// Handle to the server that this tools shall stream events to
      ServiceHandle<IServerSvc> m_ServerSvc;

    private:

      /** Stream name under which these events shall appear */
      std::string m_StreamName;

  };

} //Namespace

#endif
