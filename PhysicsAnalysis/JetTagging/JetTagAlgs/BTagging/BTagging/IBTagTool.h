/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* **************************************************************************
                           IBTagTool.h  -  Description
                             -------------------
    begin   : 21.01.05
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
              
   ***************************************************************************/

#ifndef BTAGGING_IBTAGTOOL_H
#define BTAGGING_IBTAGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/Jet.h"
#include "xAODBTagging/BTagging.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODTracking/VertexContainer.h"

namespace Analysis
{

static const InterfaceID IID_IBTagTool("IBTagTool", 1, 0);
  
  /** \class IBTagTools.
    Interface class for the btagging tool(s). Don't mix it with the ITagJetTagTool interface class
    for the JetTagging classes.
       
    @author Andreas.Wildauer@cern.ch
    */
  
class IBTagTool : virtual public IAlgTool 
{
 public:

       /** Virtual destructor */
       virtual ~IBTagTool(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IBTagTool; };

       virtual StatusCode initialize() = 0;
       virtual StatusCode tagJet(const xAOD::Jet*, xAOD::BTagging*, const xAOD::Vertex* vtx = 0) = 0;
       virtual StatusCode tagJet(const xAOD::JetContainer * jetContainer, xAOD::BTaggingContainer * btaggingContainer) = 0;
       virtual StatusCode finalize() = 0;
       virtual void finalizeHistos() = 0;

};
    
} // End namespace
#endif
