// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RootReaderD3PDSvc.h 452707 2011-08-08 11:45:42Z krasznaa $
#ifndef D3PDMAKERREADER_ROOTREADERD3PDSVC_H
#define D3PDMAKERREADER_ROOTREADERD3PDSVC_H

// STL include(s):
#include <map>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthService.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ID3PDSvc.h"

namespace D3PD {

   /**
    *  @short A very simple D3PDSvc which produces RootReaderD3PD objects
    *
    *         This service can be used to create RootReaderD3PD objects, and
    *         generate C++ D3PDReader classes using D3PD::ReaderAlg. It should
    *         not be used with D3PD::MakerAlg, as the created RootReaderD3PD
    *         objects do not implement the full D3PD::ID3PD interface. (The job
    *         will fail...)
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 452707 $
    * $Date: 2011-08-08 13:45:42 +0200 (Mon, 08 Aug 2011) $
    */
   class RootReaderD3PDSvc : public AthService,
                             public ID3PDSvc {

   public:
      /// Standard Service constructor
      RootReaderD3PDSvc( const std::string& name, ISvcLocator* svcloc );

      /// Standatd Gaudi initialization function
      virtual StatusCode initialize();
      /// Standard Gaudi finalization function
      virtual StatusCode finalize();

      /// Function implementing the ID3PDSvc interface
      virtual StatusCode make( const std::string& name, ID3PD*& d3pd );

      /// Gaudi framework function declaring the implemented interfaces
      virtual StatusCode queryInterface( const InterfaceID& riid,
                                         void** ppvIf );

   private:
      /// Internal list of all the created D3PD objects
      std::vector< ID3PD* > m_d3pds;

      /// D3PDReader version to use
      int m_version;

   }; // class RootReaderD3PDSvc

} // namespace D3PD

#endif // D3PDMAKERREADER_ROOTREADERD3PDSVC_H
