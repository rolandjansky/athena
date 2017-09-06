/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GOODCALOSELECTOR_H_
#define GOODCALOSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

  /**
  * @brief Select data events with good Tile and LAr calorimeters.
  *        MC simulation events are always accepted.
  */
  class GoodCaloSelector : public EventSelectorBase {
    public:
      GoodCaloSelector();
      virtual ~GoodCaloSelector(){};

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

#endif /* GoodCaloSelector */
