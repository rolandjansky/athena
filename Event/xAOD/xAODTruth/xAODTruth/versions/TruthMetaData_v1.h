// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRUTH_VERSIONS_TRUTHMETADATA_V1_H
#define XAODTRUTH_VERSIONS_TRUTHMETADATA_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/ObjectType.h"

namespace xAOD {

   /// Class describing meta data for truth records
   ///
   ///
   /// @author Tobias Bisanz <tobias.bisanz@cern.ch>
   ///
   /// $Revision$
   /// $Date$
   ///
   class TruthMetaData_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TruthMetaData_v1();

      /// @name Simple truth meta data properties 
      /// @{

      uint32_t mcChannelNumber() const;
      void setMcChannelNumber( uint32_t value );

      const std::vector< std::string >& weightNames() const;
      void setWeightNames( const std::vector< std::string >& value );

      const std::string& lhefGenerator() const;
      void setLhefGenerator( const std::string& value );

      const std::string& generators() const;
      void setGenerators( const std::string& value );

      const std::string& evgenProcess() const;
      void setEvgenProcess( const std::string& value );

      const std::string& evgenTune() const;
      void setEvgenTune( const std::string& value );

      const std::string& hardPDF() const;
      void setHardPDF( const std::string& value );

      const std::string& softPDF() const;
      void setSoftPDF( const std::string& value );

      /// @}

   private:
   
   }; // class TruthMetaData_v1

} // namespace xAOD

// Declare the inheritance of the type to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthMetaData_v1, SG::AuxElement );

#endif // XAODTRUTH_VERSIONS_TRUTHMETADATA_V1_H
