/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  TopoCoreSimResult.h
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/29/12.
//

#ifndef __L1TopoCoreSimulation__TopoCoreSimResult__
#define __L1TopoCoreSimulation__TopoCoreSimResult__

#include <iostream>
#include "L1TopoCoreSimulation/StatusCode.h"
#include "L1TopoCoreSimulation/GlobalDecision.h"
#include "L1TopoCoreSimulation/DataArray.h"
#include <vector>

namespace TCS { class TopoCoreSimResult; }

std::ostream& operator<<(std::ostream&, const TCS::TopoCoreSimResult &);

namespace TCS {

   class Connector;

   class TopoCoreSimResult {
   public:
      TopoCoreSimResult();
      ~TopoCoreSimResult();

      const GlobalDecision& globalDecision() const { return m_globalDecision; }

      const DataArray* output(const std::string & connName) const;

      StatusCode setConfiguration(const std::vector<TCS::Connector*>&);

      StatusCode collectResult();

      StatusCode reset();

   private:
      friend std::ostream& ::operator<<(std::ostream&, const TCS::TopoCoreSimResult &);

      GlobalDecision m_globalDecision;

      // flags if the decision field is up to date
      bool m_valid;

      // map to all decision algorithms for configuration and decision
      std::vector<TCS::Connector*> m_connectors;

   };

} // end of namespace TCS


#endif /* defined(__L1TopoCoreSimulation__TopoCoreSimResult__) */
