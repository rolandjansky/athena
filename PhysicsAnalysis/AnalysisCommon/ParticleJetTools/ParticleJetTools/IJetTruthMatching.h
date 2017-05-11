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
#include <map>
#include <string>

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

            struct MatchInfo {
              // Predefined name to label the jets passing the matching.
              int jetLabel = 0;

              // Number of MC Events in the McEventCollection
              int NEventInCollection = 0;

              // Barcode of the matched parton (to be able to find the parton in the McEventColl)
              int barcode = 0;

              // pdg code of the parton/baryon the jet has been matched to (which was closest)
              int pdg = 0;

              // Keep track of distances to quarks.
              std::map<std::string, double> distanceToQuarks;

              // Position of the lowest lying B hadron vertex decay
              //Eigen::Vector3d BDecVtx;
              //int Bpdg

              double deltaRMinTo (const std::string& key) const
              {
                auto it = distanceToQuarks.find (key);
                if (it != distanceToQuarks.end())
                  return it->second;
                return 9999;
              }
            };

            /** Virtual destructor */
            virtual ~IJetTruthMatching(){};

            /** AlgTool interface methods */
            // static const InterfaceID& interfaceID() { return IID_IJetTruthMatching; };

            /** Method to truth tag a jet: */
            virtual bool matchJet(const xAOD::Jet& myJet,
                                  MatchInfo* info = nullptr) const = 0;
            //virtual bool matchJet(const Jet& myJet) = 0;

            /** print parameter settings of the truth match tool */
            virtual void printParameterSettings() const = 0;
    };

} // end of namespace

#endif // PARTICLEJETTOOLS_IJETTRUTHMATCHING_H

