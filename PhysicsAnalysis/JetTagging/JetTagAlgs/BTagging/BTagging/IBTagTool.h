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

  /** \class IBTagTools.
    Interface class for the btagging tool(s). Don't mix it with the ITagJetTagTool interface class
    for the JetTagging classes.
       
    @author Andreas.Wildauer@cern.ch
    */
  
class IBTagTool : virtual public IAlgTool 
{
 public:
       DeclareInterfaceID( IBTagTool, 1, 0 );

       /** Virtual destructor */
       virtual ~IBTagTool() = default;

       virtual StatusCode tagJet(const xAOD::Jet*, xAOD::BTagging*, const std::string &jetName, const xAOD::Vertex* vtx = 0) const = 0;
       virtual StatusCode tagJet(const xAOD::JetContainer * jetContainer, xAOD::BTaggingContainer * btaggingContainer, const std::string &jetName) const = 0;
       virtual void finalizeHistos() = 0;

};
    
} // End namespace
#endif
