// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: D3PDBunchCrossingTool.h 502485 2012-05-25 13:02:53Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_D3PDBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_D3PDBUNCHCROSSINGTOOL_H

// STL include(s):
#include <vector>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/RVar.h"

// Interface include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

// Local include(s):
#include "TrigBunchCrossingTool/D3PDBunchCrossingToolSA.h"

namespace Trig {

   /**
    *  @short Implementation reading the information from a D3PD file
    *
    *         This implementation is meant to be used in analyses that use
    *         Athena to analyze a D3PD file. It uses all the features
    *         provided by AthenaRootComps and friends for accessing the
    *         metadata from the input files.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 502485 $
    * $Date: 2012-05-25 15:02:53 +0200 (Fri, 25 May 2012) $
    */
   class D3PDBunchCrossingTool : public AthAlgTool,
                                 public virtual IBunchCrossingTool,
                                 public virtual IIncidentListener,
                                 public virtual D3PDBunchCrossingToolSA {

   public:
      /// Standard AlgTool constructor
      D3PDBunchCrossingTool( const std::string& type, const std::string& name,
                             const IInterface* parent );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();

      /// Function called when a registered incident happens
      virtual void handle( const Incident& inc );

   private:
      int m_maxBunchSpacing; ///< The maximum bunch spacing that the tool should consider
      int m_frontLength; ///< Length of the "front" of a bunch train
      int m_tailLength; ///< Length of the "tail" of a bunch train

      /// The current event's bunch configuration ID
      SG::RVar< unsigned int > m_configID;
      /// The currently loaded configuration ID
      unsigned int m_currentID;

      /// Name of the event tree in the job
      std::string m_tupleName;
      /// Names of the input files in the job
      std::vector< std::string > m_files;

   }; // class D3PDBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_D3PDBUNCHCROSSINGTOOL_H
