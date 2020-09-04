/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENT_MUONPARAMDEFS_H
#define MUONEVENT_MUONPARAMDEFS_H
/*****************************************************************************
Name    : MuonParamDefs.h
Package : offline/Reconstruction/MuonIdentification/muonEvent
Author  : Ketevi A. Assamagan
Created : September 2005

DESCRIPTION:

This file defines enums for Muon parameters

*****************************************************************************/

#include <vector>
#include <string>

namespace MuonParameters { 

  /** do not change the order of these enums.  You can add one, but
      changing the order is bad. 
      The authors "highpt", "lowPt" and "mediumPt" should not be used - 
      they are kept for backward compatibility ONLY, to read old data 
  */
  enum Author {
     unknown=0, 
     highPt=1, 
     lowPt=2, 
     mediumPt=3,
     MuonboySP=4, 
     Muonboy=5, 
     STACO=6, 
     MuTag=7, 
     MOORE=10,
     MuidSA=11,
     MuidCo=12,
     MuGirl=13,
     CaloMuonId=14,
     MuGirlLowBeta=15,
     CaloTag=16,
     CaloLikelihood=17,
     MuTagIMO=18,
     MuonCombinedRefit=19,
     ExtrapolateMuonToIP=20,
     NumberOfMuonAuthors=21 // increase this guy when adding
  };

     
  /** @struct AuthorNames

      @brief Simple struct to access the strings of the author definitions 
             for output reasons.

             Use in the following way:
             MuonParameters::AuthorNames authorNames;
             std::string thisMuon = authorNames.author[muon.author()];
       
      @author Wolfgang.Liebig@cern.ch
  */
  struct AuthorNames {
    /**Constructor*/
    AuthorNames()
    { 
      author.reserve(NumberOfMuonAuthors);
 
      author.push_back("undefined");      // 0
      author.push_back("unknown");
      author.push_back("unknown");
      author.push_back("unknown");
      author.push_back("Muonboy (SP)");
      author.push_back("Muonboy");        // 5
      author.push_back("STACO");
      author.push_back("MuTag");
      author.push_back("unknown");
      author.push_back("unknown");
      author.push_back("Moore");          // 10
      author.push_back("Muid Standalone");
      author.push_back("Muid Combined");
      author.push_back("MuGirl");
      author.push_back("CaloMuonID");
      author.push_back("MuGirlLowBeta");  // 15
      author.push_back("CaloTag");
      author.push_back("CaloLikelihood");
      author.push_back("MuTagIMO");
      author.push_back("MuonCombinedRefit");
      author.push_back("ExtrapolateMuonToIP");  // 20
    }
      
    /**The member*/ 
    std::vector<std::string> author;
    
  };                                    



  /** Enum for parameter indexes */
  enum ParamDef {
    /** isolation enums - common parameters */
    etcone10 = 0,//<!  Isolation Et in a cone of 0.1
    etcone20 = 1,//<!  Isolation Et in a cone of 0.2 
    etcone30 = 2,//<!  Isolation Et in a cone of 0.3
    etcone40 = 3,//<!  Isolation Et in a cone of 0.4
    nucone10 = 8,//<!  Number of Tracks in a cone of 0.1
    nucone20 = 9,//<! Number of Tracks in a cone of 0.2 
    nucone30 = 10,//<! Number of Tracks in a cone of 0.3
    nucone40 = 11,//<! Number of Tracks in a cone of 0.4
    ptcone10 = 16,//<! summed Pt of tracks in a cone of 0.1
    ptcone20 = 17,//<! summed Pt of tracks in a cone of 0.2
    ptcone30 = 18,//<! summed Pt of tracks in a cone of 0.3
    ptcone40 = 19,//<! summed Pt of tracks in a cone of 0.4
//
    etconeNoEm10 = 20,//<!  Isolation Et removing electromagnetic component in a cone of 0.1
    etconeNoEm20 = 21,//<!  Isolation Et removing electromagnetic component in a cone of 0.2
    etconeNoEm30 = 22,//<!  Isolation Et removing electromagnetic component in a cone of 0.3
    etconeNoEm40 = 23,//<!  Isolation Et removing electromagnetic component in a cone of 0.4


    /** Discriminators and further variables */
    spectrometerFieldIntegral  = 50, //<! B-field integral in MS
    scatteringCurvatureSignificance = 51, //<! Scattering angle significance: curvature 
    scatteringNeighbourSignificance = 52, //<! Scattering angle significance: neighbours
    momentumBalanceSignificance = 53, //<! momentum balance significance

    /** MuTag parameters */
    segmentDeltaEta = 100, 
    segmentDeltaPhi = 101,
    segmentChi2OverDoF = 102,
    /** MuGirl parameter */
    t0        = 198,
    beta      = 199,
    annBarrel = 200,
    annEndCap = 201,
    /** common MuGirl and MuTag parameters */
    innAngle  = 202,
    midAngle  = 203
  };

}

#endif // MUONIDEVENT_MUONPARAMDEFS_H





