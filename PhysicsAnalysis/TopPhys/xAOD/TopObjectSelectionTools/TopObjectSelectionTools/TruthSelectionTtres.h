/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHSELECTIONTTRES_H_
#define TRUTHSELECTIONTTRES_H_

#include "TopObjectSelectionTools/TruthSelectionBase.h"


  /**
  * @brief An example class implementing a truth selection for tt resonances.
  *
  */

namespace top {

  class TruthSelectionTtres : public top::TruthSelectionBase {
    public:

      TruthSelectionTtres();
      /**
      * @brief Doesn't do anything itself.
      */
      virtual ~TruthSelectionTtres(){}

      /**
      * @brief The cuts (per object) are implemented in here.
      *
      * Not that I've marked this as final, to stop people going crazy with
      * inheritance.  Why not just inherit from TruthSelectionBase instead?
      *
      * @param mc The particle to cut on.
      * @return True if you want to keep this particle, false otherwise.
      */
      bool passSelection(const xAOD::TruthParticle& mc) const override final;

      ///Print the cuts to the ostream.
      void print(std::ostream& os) const override final;

    private:
      TruthSelectionTtres(const TruthSelectionTtres& rhs);
      TruthSelectionTtres(const TruthSelectionTtres&& rhs);
      void operator=(const TruthSelectionTtres& rhs);
  };

}

#endif
