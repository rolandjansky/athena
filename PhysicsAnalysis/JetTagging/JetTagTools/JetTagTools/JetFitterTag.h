/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*- header for JetFitterTag
/******************************************************

   @class JetFitterTag - BTagging algorithm based on the 
                  "JetFitter" vertexing algorithm

   Adapted from JetFitterTag in 2012. 

   This is intended as an iteration on JetFitterCOMBNN: it should run fine 
   using JetFitterVariablesFactory and JetFitterNNTool, but can also 
   make use of the extensions in JetFitterCharmVariablesFactory, 
   and will augment this information with SVInfoPlus and IP3D tag info. 

   The base-tagger information is passed into any classifier that inherits
   from IJetFitterClassifierTool. 
   

   @author: 
   * Dan Guest (dguest AT cern.ch)
   * Giacinto Piacquadio (giacinto.piacquadio AT physik.uni-freiburg.de)
   * Christian Weiser (christian.weiser AT physik.uni-freiburg.de)

   --- ( University of FREIBURG and Yale ) ---

   (c) 2007 - ATLAS Detector Software

********************************************************/
 

#ifndef JETTAGTOOLS_JETFITTERCHARMTAG_H
#define JETTAGTOOLS_JETFITTERCHARMTAG_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"
#include <vector>


class StoreGateSvc;

//class Jet;

namespace Analysis { 

  class IJetFitterNtupleWriter;
  class IJetFitterClassifierTool;

  class JetFitterTag : public extends<AthAlgTool, ITagTool>
  {
    public:
      JetFitterTag(const std::string&,
			const std::string&,
			const IInterface*);
      
    /**
       Implementations of the methods defined in the abstract base class
    */
    virtual ~JetFitterTag();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual void finalizeHistos() override;
      

    virtual StatusCode tagJet(const xAOD::Vertex& priVtx,
                              const xAOD::Jet& jetToTag,
                              xAOD::BTagging& BTag,
                              const std::string &jetName) const override;
    
  private:      

    std::string    m_runModus;          

    /** for reference mode: */

    bool m_doForcedCalib;
    std::string m_ForcedCalibName;
    std::string m_ipinfo_tagger_name; 
    std::string m_secVxFinderName;
    std::string m_xAODBaseName;

    // skip light jets with heavy flavor in this cone
    double m_jetPtMinRef; // min cut on jet pT for reference

    std::vector<std::string> m_jetCollectionList; // 
    std::vector<std::string> m_hypothese; // hypotheses: b | c | u

    ToolHandle<IJetFitterNtupleWriter> m_ntupleWriter;
    ToolHandle<IJetFitterClassifierTool> m_classifier;
    

  }; // End class

} // End namespace 

#endif
