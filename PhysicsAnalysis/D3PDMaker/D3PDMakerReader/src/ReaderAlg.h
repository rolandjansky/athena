// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ReaderAlg.h 348546 2011-03-01 15:09:56Z krasznaa $
#ifndef D3PDMAKERREADER_READERALG_H
#define D3PDMAKERREADER_READERALG_H

// STL include(s):
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ID3PDSvc.h"
#include "D3PDMakerInterfaces/IObjFillerTool.h"

namespace D3PD {

   // Forward declaration(s):
   class IReaderD3PD;

   /**
    *  @short Algorithm used for generating D3PDReader sources
    *
    *         This algorithm is used for generating D3PDReader source files. The exact
    *         source type depends on the D3PDSvc given to the algorithm, it's not
    *         tied to the C++ code generation implementation.
    *
    *         The algorithm is practically a simplified version of D3PD::MakerAlg, as
    *         it would've been less elegant to use that algorithm for the code generation.
    *         (But not impossible.)
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 348546 $
    * $Date: 2011-03-01 16:09:56 +0100 (Tue, 01 Mar 2011) $
    */
   class ReaderAlg : public AthAlgorithm {

   public:
      /// Standard algorithm constructor
      ReaderAlg( const std::string& name, ISvcLocator* svcloc );

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

      /// Property: The tuple name.
      std::string m_tuplePath;

      std::string m_prefix; ///< Common variable name prefix

      std::string m_dir; ///< Directory where the sources should be put
      std::string m_classname; ///< Name of the generated class

      /// The created tuple. Note: we don't take ownership.
      IReaderD3PD* m_d3pd;
      /// Flag that we've called book().
      bool m_booked;

   }; // class ReaderAlg

} // namespace D3PD

#endif // D3PDMAKERREADER_READERALG_H
