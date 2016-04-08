// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingReader.h 581810 2014-02-06 08:45:23Z krasznaa $
#ifndef XAODREADERALGS_BTAGGINGREADER_H
#define XAODREADERALGS_BTAGGINGREADER_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace xAODReader {

  /**
   *  @short Algorithm reading xAOD::BTagging
   *
   *         This algorithm is used to test the ability to read
   *         xAOD::BTagging objects from a file inside Athena.
   *
   * @author Johannes Erdmann <johannes.erdmann@cern.ch>
   *
   * $Revision: 581810 $
   * $Date: 2014-02-06 09:45:23 +0100 (Thu, 06 Feb 2014) $
   */
  class BTaggingReader : public AthAlgorithm {

  public:

    /// Regular Algorithm constructor
    BTaggingReader( const std::string& name, ISvcLocator* svcLoc );

    /// Function initialising the algorithm
    virtual StatusCode initialize();

    /// Function executing the algorithm
    virtual StatusCode execute();

  private:

    /// Name of the container to investigate
    std::string m_containerName;

  }; // class BTaggingReader

} // namespace xAODReader

#endif // XAODREADERALGS_BTAGGINGREADER_H
