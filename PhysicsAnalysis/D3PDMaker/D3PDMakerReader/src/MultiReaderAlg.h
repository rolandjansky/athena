// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MultiReaderAlg.h 348546 2011-03-01 15:09:56Z krasznaa $
#ifndef D3PDMAKERREADER_MULTIREADERALG_H
#define D3PDMAKERREADER_MULTIREADERALG_H

// STL include(s):
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ID3PDSvc.h"
#include "D3PDMakerInterfaces/IObjFillerTool.h"
#include "D3PDMakerInterfaces/IMetadataTool.h"

namespace D3PD {

   // Forward declaration(s):
   class IReaderD3PD;

   /**
    *  @short Algorithm used for generating D3PDReader sources
    *
    *         This algorithm is a bit more flexible than D3PD::ReaderAlg in that it can
    *         receive multiple D3PDObject-s as input, and can generate the D3PDReader
    *         source files for each of them in the finalization of the job. This
    *         algorithm is practically a full replacement for D3PD::MakerAlg, so it
    *         can replace that even in larger D3PDMaker jobs.
    *
    *         Use the D3PDMakerReader.MultiReaderAlg configurable for setting up this
    *         algorithm in an Athena job.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 348546 $
    * $Date: 2011-03-01 16:09:56 +0100 (Tue, 01 Mar 2011) $
    */
   class MultiReaderAlg : public AthAlgorithm {

   public:
      /// Standard algorithm constructor
      MultiReaderAlg( const std::string& name, ISvcLocator* svcloc );

      /// Standard Gaudi @c initialize method.
      virtual StatusCode initialize();
      /// Standard Gaudi @c finalize method.
      virtual StatusCode finalize();
      /// Standard Gaudi @c execute method.
      virtual StatusCode execute();

   private:
      /// Property: The D3PD creation service.
      ServiceHandle< ID3PDSvc > m_d3pdSvc;
      /// Property: List of object filler tools to run.
      ToolHandleArray< IObjFillerTool > m_tools;
      /// Property: List of metadata tools to run.
      ToolHandleArray< IMetadataTool > m_metadataTools;

      std::string m_tuplePath; ///< The D3PD base name

      std::vector< std::string > m_prefixes; ///< Variable name prefixes
      std::vector< std::string > m_classnames; ///< Reader class names

      std::string m_dir; ///< Directory where the sources should be put

      /// The created tuples. Note: we don't take ownership.
      std::vector< IReaderD3PD* > m_d3pds;
      /// Flag that we've called book().
      bool m_booked;

   }; // class MultiReaderAlg

} // namespace D3PD

#endif // D3PDMAKERREADER_MULTIREADERALG_H
