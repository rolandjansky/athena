/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRIMARYVERTEXSELECTOR_H_
#define PRIMARYVERTEXSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

  /**
  * @brief Select data events with good Primary vertices
  *        following:
  * https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/InDetTrackingPerformanceGuidelines#Vertexing
  * 
  */
  class PrimaryVertexSelector : public EventSelectorBase {
    public:
      PrimaryVertexSelector();
      virtual ~PrimaryVertexSelector(){};

      /**
      * @brief MC simulation passes, data is checked against the GRL.
      *
      * @param event The event in question
      * @return True for MC simulation, or if the data event is on the GRL.
      */
      virtual bool apply(const top::Event& event) const override;

      /**
      * @brief The name of the tool.
      *
      * @return The name of the tool for humans to see.
      */
      std::string name() const override;

  };
}

#endif /* PrimaryVertexSelector */
