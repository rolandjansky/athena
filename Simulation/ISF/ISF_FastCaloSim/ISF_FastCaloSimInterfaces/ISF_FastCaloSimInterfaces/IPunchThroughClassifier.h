/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMINTERFACES_IPUNCHTHROUGHCLASSIFIER_H
#define ISF_FASTCALOSIMINTERFACES_IPUNCHTHROUGHCLASSIFIER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"


namespace ISF {

  /**
     @class IPunchThroughClassifier

      Interface for a tool which takes simulstate and particle
      and predicts whether it should result in a muon segment.

     @author thomas.michael.carter@cern.ch
   */

 class IPunchThroughClassifier : virtual public IAlgTool {
     public:

        /** Virtual destructor */
        virtual ~IPunchThroughClassifier(){}

        /// Creates the InterfaceID and interfaceID() method
        DeclareInterfaceID(IPunchThroughClassifier, 1, 0);

        /** calculates probability of punch through from ispf and simulstate */
        virtual double computePunchThroughProbability(const ISFParticle& isfp, const TFCSSimulationState& simulstate ) const = 0;

 };

} // end of namespace

#endif
