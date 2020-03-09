/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASGTOOLS_ASGMETADATATOOL_H
#define ASGTOOLS_ASGMETADATATOOL_H

// Local include(s):
#include "AsgTools/AsgTool.h"
#ifdef XAOD_STANDALONE
#   include "AsgTools/SgTEventMeta.h"
#   include "xAODRootAccess/TVirtualIncidentListener.h"
#else // XAOD_STANDALONE
#   include "GaudiKernel/IIncidentListener.h"
#endif // XAOD_STANDALONE

// Forward declaration(s):
class Incident;

namespace asg {

#ifdef XAOD_STANDALONE
   /// Typedef masquerading the standalone code as if it were Gaudi code
   typedef xAOD::TVirtualIncidentListener IIncidentListener;
   /// Typedef masquerading the standalone code as if it were Gaudi code
   typedef xAOD::TIncident Incident;
#endif // XAOD_STANDALONE

   /// Base class for dual-use tools that provide file metadata access
   ///
   /// Didn't want to make the AsgTool base class even heavier with a feature
   /// that only very few dual-use tools will actually use. So instead decided
   /// that tools that need metadata access, will need to inherit from this base
   /// class instead of AsgTool. Very much in sync with how the class hierarchy
   /// looks like in AthenaBaseComps.
   ///
   /// The class provides StoreGate-like access to the input and output metadata
   /// store in a uniform way across Athena and ROOT. In Athena it just relies
   /// on the two StoreGateSvc instances taking care of this, and in ROOT it
   /// uses a wrapper around xAOD::TEvent to achieve the same interface.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 771072 $
   /// $Date: 2016-08-31 16:50:22 +0200 (Wed, 31 Aug 2016) $
   ///
   class AsgMetadataTool : public AsgTool,
                           public virtual IIncidentListener {

   public:
      /// Normal ASG tool constructor with a name
      AsgMetadataTool( const std::string& name );
      /// Destructor
      ~AsgMetadataTool();

      /// @name Definition of the StoreGate-like object's definition
      /// @{

#ifdef XAOD_STANDALONE
      /// Type of the metadata store object in standalone mode
      typedef SgTEventMeta  MetaStore_t;
      /// Type of the metadata store pointer in standalone mode
      typedef SgTEventMeta* MetaStorePtr_t;
#else // XAOD_STANDALONE
      /// Type of the metadata store object in Athena
      typedef ServiceHandle< StoreGateSvc > MetaStore_t;
      /// Type of the metadata store pointer in standalone mode
      typedef const ServiceHandle< StoreGateSvc >& MetaStorePtr_t;
#endif // XAOD_STANDALONE

      /// @}

      /// @name Functions providing access to the input/output metadata
      /// @{

      /// Accessor for the input metadata store
      MetaStorePtr_t inputMetaStore() const;
      /// Accessor for the output metadata store
      MetaStorePtr_t outputMetaStore() const;

      /// @}

      /// Function initialising the tool in the correct way in Athena
      virtual StatusCode sysInitialize();


   protected:
      /// @name Callback functions helping in metadata reading/writing
      /// @{
      
      void setUseIncidents(const bool flag);

      /// Function receiving incidents from IncidentSvc/TEvent
      virtual void handle( const Incident& inc );

      /// Function called when a new input file is opened
      virtual StatusCode beginInputFile();

      /// Function called when the currently open input file got completely
      /// processed
      virtual StatusCode endInputFile();

      /// Function called when a new events is loaded
      virtual StatusCode beginEvent();

      /// Function called when the tool should write out its metadata
      virtual StatusCode metaDataStop();

      /// @}

   private:
#ifdef XAOD_STANDALONE
      /// Object accessing the input metadata store
      mutable MetaStore_t m_inputMetaStore;
      /// Object accessing the output metadata store
      mutable MetaStore_t m_outputMetaStore;
#else
      /// Object accessing the input metadata store
      MetaStore_t m_inputMetaStore;
      /// Object accessing the output metadata store
      MetaStore_t m_outputMetaStore;
#endif

      /// Flag helping to discover when the tool misses the opening of the first
      /// input file
      bool m_beginInputFileCalled;

      bool m_useIncidents;

   }; // class AsgMetadataTool

   inline void AsgMetadataTool::setUseIncidents(const bool flag) 
   {
      m_useIncidents = flag;
   }

} // namespace asg

#endif // ASGTOOLS_ASGMETADATATOOL_H
