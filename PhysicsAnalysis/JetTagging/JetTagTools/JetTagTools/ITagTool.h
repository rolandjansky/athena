/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    @class ITagTool

    Created   : 10.03.04
    @author Andreas Wildauer (CERN PH-ATC) andreas.wildauer@cern.ch
    @author2 Fredrik Akesson (CERN PH-ATC) fredrik.akesson@cern.ch
********************************************************/
#ifndef JETTAGTOOLS_ITAGTOOL_H
#define JETTAGTOOLS_ITAGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/Jet.h"
#include "xAODTracking/Vertex.h"
#include "xAODBTagging/BTagging.h"
//#include <string>

//namespace Trk { class VxCandidate; }
//class Jet;

namespace Analysis
{
  class ITagTool : virtual public IAlgTool
  {
    public:
      DeclareInterfaceID( ITagTool, 1, 0 );

      virtual ~ITagTool() = default;
      
      /** Method to return the name of the tagging tool */
//       virtual std::string tagToolName(void) = 0;
      

     /** The tagging method.
       *
       * TODO: The first argument is temporary ! The primary vertex should
       * be part of the JetTag IParticle interface implementation. The trouble with 
       * ElementLink and persistency has to be solved for that. Revisit ...
       *
       * TODO: The second argument will be removed as soon as we 
       * have the JetTag containing the necessary pointers to the constituent 
       * TrackParticles via navigation. But maybe this is not a very good idea since then
       * a JetTag always must be created even if the tagging does not work. Think ...
       */
      virtual StatusCode tagJet(const xAOD::Vertex& priVtx,
                                const xAOD::Jet&,
                                xAOD::BTagging&,
                                const std::string&) const = 0;
      

      /** finalize the histos: Normalize other histos and fill the integratedNegativeIP histos.
       This cannot be done in the finalize() of this algtool because the histo service
       writes the histos BEFORE AlgTool::finalize() methods are called.
       However, it is closed AFTER Algorithm::finalize() methods are called ...
       In the calling Algorithm::finalize() method the following method will be called.
       */
      virtual void finalizeHistos() = 0;

  }; // End class

} // End namespace 

#endif
