// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//
#ifndef STANDALONEANALYSISALGORITHMS_XAODWRITERALG_H
#define STANDALONEANALYSISALGORITHMS_XAODWRITERALG_H

// System include(s):
#include <string>
#include <vector>
#include <typeinfo>

// Core include(s):
#include "AnaAlgorithm/AnaAlgorithm.h"
#include "SystematicsHandles/SysListHandle.h"
#include "xAODRootAccess/TEvent.h"

namespace CP {

   /// Algorithm writing an xAOD output file
   ///
   /// This algorithm is meant to be used to write mini-xAOD output files from
   /// analysis jobs. Similar to how such a file could be written using an
   /// Athena job.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class xAODWriterAlg final : public EL::AnaAlgorithm {

   public:
      /// Algorithm constructor
      xAODWriterAlg( const std::string& name, ISvcLocator* svcLoc );

      /// @name Function(s) inherited from @c EL::AnaAlgorithm
      /// @{

      /// Function initialising the algorithm
      StatusCode initialize() override;

      /// Function executing the algorithm
      StatusCode execute() override;

      /// Function finalising the algorithm
      StatusCode finalize() override;

      /// @}

   private:
      /// Function setting up the algorithm while processing the first event
      StatusCode setup();

      /// @name Algorithm properties
      /// @{

      /// Name of the output stream to write to
      std::string m_outputStreamName;
      /// Item list to write to the output file
      std::vector< std::string > m_itemList;

      /// (Starter) Basket size for the created branches
      int m_basketSize;
      /// Split level for the created branches
      int m_splitLevel;

      /// @}

      /// Helper struct
      struct Item {
         /// Name of the written object
         std::string name;
         /// Type of the written object
         const std::type_info* type;
         /// Type name of the written object
         std::string typeName;
      }; // struct Item

      /// Object to write the output file with
      xAOD::TEvent m_event;

      /// Internal flag
      bool m_itemListInitialized = false;
      /// Item list being written after the first event
      std::vector< Item > m_writtenItemList;

      /// The systematic list to consider during execution
      SysListHandle m_systematicsList{ this };

   }; // class xAODWriterAlg

} // namespace CP

#endif // STANDALONEANALYSISALGORITHMS_XAODWRITERALG_H
