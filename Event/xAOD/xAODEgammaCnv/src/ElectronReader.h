// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronReader.h 581810 2014-02-06 08:45:23Z krasznaa $
#ifndef XAODREADERALGS_ELECTRONREADER_H
#define XAODREADERALGS_ELECTRONREADER_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace xAODReader {

   /**
    *  @short Algorithm reading xAOD::CaloElectrons
    *
    *         This algorithm is used to test the ability to read
    *         xAOD::Electron objects from a file inside Athena.
    *
    * @author Christos Anastopoulos <Christos.Anastopoulos@cern.ch>
    *
    * $Revision: 581810 $
    * $Date: 2014-02-06 09:45:23 +0100 (Thu, 06 Feb 2014) $
    */
   class ElectronReader : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      ElectronReader( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// Name of the container to investigate
      std::string m_containerName;
     std::string m_containerFrwdName;
   }; // class ElectronReader

} // namespace xAODReader

#endif // XAODREADERALGS_ELECTRONREADER_H
