// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthMetaDataTool.h 676522 2015-06-18 22:17:03Z cranshaw $
#ifndef XAODMETADATACNV_XAODTRUTHMETADATATOOLTOOL_H
#define XAODMETADATACNV_XAODTRUTHMETADATATOOLTOOL_H

// System include(s):
#include <string>
#include <memory>

// Infrastructure include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AthenaPoolKernel/IMetaDataTool.h"

namespace xAODMaker {

   /// Tool propagating xAOD::TruthMetaDataContainer information
   ///
   /// This Athena-only tool can be used to copy input file 
   /// xAOD::TruthMetaDataContainer payload, taken as is, into the output.
   ///
   /// The class uses asg::AsgMetadataTool as a base class for convenience,
   /// but it's not a dual-use tool. (Hence the header is hidden from the
   /// outside world.)
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 676522 $
   /// $Date: 2015-06-19 00:17:03 +0200 (Fri, 19 Jun 2015) $
   ///
   class xAODTruthMetaDataTool : public asg::AsgMetadataTool,
                                   public virtual ::IMetaDataTool {

      /// Declare the correct constructor for Athena
      ASG_TOOL_CLASS( xAODTruthMetaDataTool, IMetaDataTool )

   public:
      /// Regular AsgTool constructor
      xAODTruthMetaDataTool( const std::string& name =
                               "xAODTruthMetaDataTool" );

      /// Function initialising the tool
      virtual StatusCode initialize();

   protected:
      /// @name Functions called by the AsgMetadataTool base class
      /// @{

      /// Function collecting the metadata from a new input file
      virtual StatusCode beginInputFile();

      /// @}

   }; // class xAODTruthMetaDataTool

} // namespace xAODMaker

#endif // XAODMETADATACNV_XAODTRUTHMETADATATOOLTOOL_H
