/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthClassificationDefs.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVALEVENT_TRUTHCLASSIFICATIONDEFS_H
#define TRKVALEVENT_TRUTHCLASSIFICATIONDEFS_H

namespace Trk {
  namespace TruthClassification {

    /** @brief TruthClassification
        @author S. Fleischmann, W. Liebig */

    enum Classifiers {
      OutsideClassification = 0 //!< providing some well-defined error condition
    };

    /** @enum PrimaryClassifiers classification for type of ID track according to appearance in detector
      */
    enum PrimaryClassifiers {
      Primary=1,
      Secondary,
      Truncated,
      NumberOfPrimaryClassifiers
    };

    /** @enum HeavyFlavourClassifiers classification for type of ID track according to heavy flavour content
      */
    enum HeavyFlavourClassifiers {
      Prompt=1,
      HeavyFlavourDecay,
      NumberOfHeavyFlavourClassifiers
    };

    /** @enum TauClassifiers classification for type of ID track according to tau content
     */
    enum TauTruthClassifiers {
      NoTauAtAll=1,
      Tau1p,
      Tau3p,
      TauKaon,
      NumberOfTauTruthClassifiers
    };

    /** @struct Trk::TruthClassification::PrimaryClassifierStrings
        @brief Simple struct to access the names of the track classifiers
    */
    struct PrimaryClassifierStrings {
      PrimaryClassifierStrings()
      { 
        names.reserve(NumberOfPrimaryClassifiers);
        
        names.push_back("outofclassification");
        names.push_back("primary");
        names.push_back("secondary");
        names.push_back("truncated");
      }
     
      //! The member 
      std::vector<std::string> names;
      
    };                                    

    /** @struct Trk::TruthClassification::HeavyFlavourClassifierStrings
        @brief Simple struct to access the names of the hf classifiers
    */
    struct HeavyFlavourClassifierStrings {
      HeavyFlavourClassifierStrings()
      { 
        names.reserve(NumberOfHeavyFlavourClassifiers);
        
        names.push_back("outofclassification");
        names.push_back("prompt");
        names.push_back("heavyflavourdecay");
      }
     
      //! The member 
      std::vector<std::string> names;
      
    };                                    

    /** @struct Trk::TruthClassification::TauTruthClassifierStrings
        @brief Simple struct to access the names of the classifiers
    */
    struct TauTruthClassifierStrings {
      TauTruthClassifierStrings()
      { 
        names.reserve(NumberOfTauTruthClassifiers);
        
        names.push_back("outofclassification");
        names.push_back("noTauAtAll");
        names.push_back("tau1p");
        names.push_back("tau3p");
        names.push_back("tauKaon");
      }
     
      //! The member 
      std::vector<std::string> names;
      
    };                                    


  }
}
#endif
