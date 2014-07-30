// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CutFlowMetadataTool.h 339578 2011-01-11 11:18:56Z krasznaa $
#ifndef EVENTCOMMOND3PDMAKER_CUTFLOWMETADATATOOL_H
#define EVENTCOMMOND3PDMAKER_CUTFLOWMETADATATOOL_H

// STL include(s):
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Eventbookkeeper configuration include(s):
#include "AthenaKernel/ICutFlowSvc.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/IMetadataTool.h"
#include "D3PDMakerInterfaces/ID3PDSvc.h"

namespace D3PD {

   /**
    *  @short Tool for dumping eventbookkeeper information into D3PDs
    *
    *         The original implementation was written for the SUSYD3PDMaker package,
    *         which now lives in a much more general implementation in this package.
    *
    *         The tool takes the transient TTree created by the CutFlowSvc, and saves
    *         it using the full D3PDMaker machinery. It means that the output format
    *         still depends on the D3PDSvc used by this tool.
    *
    * @author Jet Goodson <jetgoodson@gmail.com>
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 339578 $
    * $Date: 2011-01-11 12:18:56 +0100 (Tue, 11 Jan 2011) $
    */
   class CutFlowMetadataTool : public AthAlgTool,
                               public IMetadataTool {

   public:
      /// Regular AlgTool constructor
      CutFlowMetadataTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// AlgTool initialization function
      virtual StatusCode initialize();
      /// Gaudi function describing which interfaces the tool implements
      virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

      /// Function writing the configuration information to the D3PD
      virtual StatusCode writeMetadata( ID3PD* d3pd );
      
   private:    
      /// Directory in which to store the cut flow
      std::string m_metaDir;

      /// Handle for the cutflow service
      ServiceHandle< ICutFlowSvc > m_cutFlowSvc;
      
      /// Handle for the D3PDSvc:
      ServiceHandle< ID3PDSvc > m_d3pdSvc;

      /// The created D3PD metadata tree:
      ID3PD* m_d3pd;

      /// Name of the transient cut flow tree
      std::string m_cutFlowKey;

   }; // class CutFlowMetadataTool

} // namespace D3PD

#endif // EVENTCOMMOND3PDMAKER_CUTFLOWMETADATATOOL_H
