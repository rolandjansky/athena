/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_TRACKCOUNTING_H
#define JETTAGTOOLS_TRACKCOUNTING_H
 
/******************************************************
    @class TrackCounting
    
    b-tagging AlgTool based on counting tracks with an 
    impact parameter significence exceeding a given
    threshold

    @author Thomas Goepfert <thomas.goepfert@cern.ch>
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"

#include <vector>

namespace Analysis { 

  class DummyTag : public AthAlgTool , virtual public ITagTool
  {
    public:
      DummyTag(const std::string&,const std::string&,const IInterface*);
      
      /** Implementations of the methods defined in the abstract base class */
      StatusCode initialize();
      StatusCode finalize();
      void finalizeHistos(){};   
      /** Implementation of ITagTool methods */
      virtual void setOrigin(const xAOD::Vertex* priVtx);
      /** called by BJetBuilder */
      StatusCode tagJet(xAOD::Jet& jetToTag, xAOD::BTagging * BTag, const std::string jetName);

    private:      

      /** write tagging info to info class */
      bool m_writeInfo;
      std::string    m_runModus; 

      // pointer to primary vertex (needed for implementation of setOrigin())
      const xAOD::Vertex* m_priVtx = 0;

  }; // End class
  
  inline void DummyTag::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx = priVtx; }

} // End namespace 

#endif
