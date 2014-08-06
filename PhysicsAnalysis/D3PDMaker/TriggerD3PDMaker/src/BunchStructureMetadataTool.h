// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchStructureMetadataTool.h 511909 2012-07-31 11:50:19Z krasznaa $
#ifndef TRIGGERD3PDMAKER_BUNCHSTRUCTUREMETADATATOOL_H
#define TRIGGERD3PDMAKER_BUNCHSTRUCTUREMETADATATOOL_H

// STL include(s):
#include <string>
#include <set>

// Gaudi/Athena include(s):
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Trigger include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingConfProvider.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/IMetadataTool.h"
#include "D3PDMakerInterfaces/ID3PDSvc.h"

namespace D3PD {

   /**
    *  @short Tool saving the bunch structure information into the D3PD
    *
    *         The design of this tool is pretty much the same as of
    *         D3PD::TrigConfMetadataTool. From the beginning of the job it
    *         listens to incidents from Trig::IBunchCrossingConfProvider, and
    *         whenever the bunch structure configuration changes, it saves the
    *         new configuration into a metadata tree.
    *
    *         The tool actually only has to do very little, most of the work
    *         is done by the Trig::IBunchCrossingConfProvider interface.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 511909 $
    * $Date: 2012-07-31 13:50:19 +0200 (Tue, 31 Jul 2012) $
    */
   class BunchStructureMetadataTool : public AthAlgTool,
                                      public IMetadataTool,
                                      public IIncidentListener {

   public:
      /// Regular AlgTool constructor
      BunchStructureMetadataTool( const std::string& type, const std::string& name,
                                  const IInterface* parent );

      /// AlgTool initialization function
      virtual StatusCode initialize();

      /// Function writing the bunch structure information to the D3PD
      virtual StatusCode writeMetadata( ID3PD* d3pd );

      /// Function called when a relevant incident is fired
      virtual void handle( const Incident& inc );

   private:
      /// Directory in which to store the configuration
      std::string m_configDir;

      ServiceHandle< IIncidentSvc > m_incidentSvc; ///< The incident service
      ServiceHandle< ID3PDSvc > m_d3pdSvc; ///< The D3PD service

      /// Provider of the bunch structure information
      ToolHandle< Trig::IBunchCrossingConfProvider > m_bcConfProv;

      /// Cache of which configurations have already been added to the output
      std::set< unsigned int > m_configKeyCache;

      //
      // The configuration variables:
      //
      ID3PD* m_configTree; ///< D3PD object holding the configuration description
      Trig::IBunchCrossingConfProvider::configid_type* m_confId;
      std::vector< int >*   m_filledBunches;
      std::vector< float >* m_bunchIntensitiesBeam1;
      std::vector< float >* m_bunchIntensitiesBeam2;
      std::vector< int >*   m_unpairedBunchesBeam1;
      std::vector< int >*   m_unpairedBunchesBeam2;
      std::vector< float >* m_unpairedBunchIntensitiesBeam1;
      std::vector< float >* m_unpairedBunchIntensitiesBeam2;

   }; // class BunchStructureMetadataTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_BUNCHSTRUCTUREMETADATATOOL_H
