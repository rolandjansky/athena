// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPRIMITIVES_ISOLATIONCORRECTION_H
#define XAODPRIMITIVES_ISOLATIONCORRECTION_H
#include <bitset>
#include <vector>

namespace xAOD {

  namespace Iso {

    /// Enumeration for different ways of correcting isolation in xAOD files
    ///
    enum IsolationCaloCorrection {
      /// no correction
      noneCaloCorrection = 0,

      /// core for muons
      coreMuon = 1,
         
      /// core 5x7 cells
      core57cells = 2,

      /// core energy (in dR<0.1).     
      coreCone = 3,

      /// egamma ptcorrection
      ptCorrection = 4,

      /// fully corrected
      pileupCorrection = 5,

      /// core energy (super cluster).     
      coreConeSC = 6,

      /// unknown
      numIsolationCaloCorrections=7

    }; // enum IsolationCaloCorrection

    static inline const char* toString(IsolationCaloCorrection corr)
    {
        switch (corr)
        {
            case noneCaloCorrection: return "none";
            case coreMuon:           return "coreMuon";
            case core57cells:        return "core57cells";
            case coreCone:           return "coreCone";
            case coreConeSC:         return "coreConeSC";
            case ptCorrection:       return "pt";
	    case pileupCorrection:   return "pileup";
            case numIsolationCaloCorrections:
            default:                 return "[Unknown IsolationCaloCorrection]";
        }
    }

    enum IsolationTrackCorrection {
      /// no correction
      noneTrackCorrection = 0,

      /// tracks pointer 
      coreTrackPtr = 1,

      /// tracks in cone 
      coreTrackCone = 2,

      /// pT of the track (needed by trigger)
      coreTrackPt = 3,

      /// unknown
      numIsolationTrackCorrections=4

    }; // enum IsolationTrackCorrection

    static inline const char* toString(IsolationTrackCorrection corr)
    {
        switch (corr)
        {
            case noneTrackCorrection: return "none";
            case coreTrackPtr:        return "coreTrackPtr";
            case coreTrackCone:       return "coreTrackCone";
            case coreTrackPt:         return "coreTrackPt";
            default:                  return "[Unknown IsolationTrackCorrection]";
        }
    }

    enum IsolationCorrectionParameter {
      /// energy stored for this correction
      coreEnergy = 0,

      /// area used to calculate this correction
      coreArea = 1,

      /// total
      NumCorrParameters = 2

    };
    
    static inline const char* toString(IsolationCorrectionParameter corr)
    {
        switch (corr)
        {
            case coreEnergy: return "Energy";
            case coreArea:   return "Area";
            default:         return "[Unknown IsolationCorrectionParameter]";
        }
    }

    /// typedef of the bit word used to encode the corrections used to calculate the isolation
    typedef std::bitset<32> IsolationCaloCorrectionBitset;
    typedef std::bitset<32> IsolationTrackCorrectionBitset;

  }
}    
    
#endif
