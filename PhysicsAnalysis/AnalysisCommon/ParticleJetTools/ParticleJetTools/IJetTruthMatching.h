/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IJetTruthMatching.h
//   Header file for class IJetTruthMatching
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Andreas.Wildauer@cern.ch
///////////////////////////////////////////////////////////////////


#ifndef PARTICLEJETTOOLS_IJETTRUTHMATCHING_H
#define PARTICLEJETTOOLS_IJETTRUTHMATCHING_H

// Gaudi
#include "AsgTools/IAsgTool.h"
#include "xAODJet/Jet.h"

class Jet;

namespace Analysis
{

    // static const InterfaceID IID_IJetTruthMatching("IJetTruthMatching", 1, 0);

    /** \class IJetTruthMatching
      Interface class for the truthmatching AlgTool, it inherits from IAlgTool
      Detailed information about private members and member functions can be found in the actual implementation class
      JetTrackTruthMatching and JetConeTruthMatching who inherit from this one.

      @author Andreas.Wildauer@cern.ch
      */

    class IJetTruthMatching : virtual public asg::IAsgTool {
        ASG_TOOL_INTERFACE(IJetTruthMatching)
        public:

            /** Virtual destructor */
            virtual ~IJetTruthMatching(){};

            /** AlgTool interface methods */
            // static const InterfaceID& interfaceID() { return IID_IJetTruthMatching; };

            /** Method to truth tag a jet: */
            virtual bool matchJet(const xAOD::Jet& myJet) = 0;
            //virtual bool matchJet(const Jet& myJet) = 0;

            /** print parameter settings of the truth match tool */
            virtual void m_printParameterSettings() = 0;

            /** Return the name to label the jets passing the matching: */
            virtual const int& jetLabel() const = 0;

    };

} // end of namespace

#endif // PARTICLEJETTOOLS_IJETTRUTHMATCHING_H

