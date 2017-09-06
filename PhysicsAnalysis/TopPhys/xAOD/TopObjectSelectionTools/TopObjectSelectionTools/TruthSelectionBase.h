/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHSELECTIONBASE_H_
#define TRUTHSELECTIONBASE_H_

#include "xAODTruth/TruthParticleContainer.h"

#include <memory>

  /**
  * @brief Base class for implementing a truth selection.
  *
  * If you want to build a new truth selection, you should inherit from this.
  */
  
namespace top {

  class TruthSelectionBase {
    public:
      TruthSelectionBase();

      virtual ~TruthSelectionBase(){}

      virtual bool passSelection(const xAOD::TruthParticle&) const = 0;

      virtual void print(std::ostream&) const = 0;
      
    protected:
      bool isBhadron(int pid) const;
      bool isChadron(int pid) const;

      TruthSelectionBase(const TruthSelectionBase& rhs);
      TruthSelectionBase(const TruthSelectionBase&& rhs);
      void operator=(const TruthSelectionBase& rhs);  
  };
}

std::ostream& operator<<(std::ostream& os, const top::TruthSelectionBase& selection);

#endif
