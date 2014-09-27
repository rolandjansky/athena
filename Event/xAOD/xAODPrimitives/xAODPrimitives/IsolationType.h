// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsolationType.h 618834 2014-09-27 16:40:39Z christos $
#ifndef XAODPRIMITIVES_ISOLATIONTYPE_H
#define XAODPRIMITIVES_ISOLATIONTYPE_H

namespace xAOD {

   /// Namespace holding the IsolationType enumeration
   ///
   /// The only reason for having this namespace is to make the user code
   /// a little easier to read, with the users requesting isolation values
   /// with enumerations values like <code>xAOD::Iso::etcone20</code>
   /// instead of just <code>xAOD::etcone20</code>.
   ///
   namespace Iso {

      /// Overall enumeration for isolation types in xAOD files
      ///
      /// This enumeration holds all the isolation types that are used more
      /// ore less in the same way across different reconstruction domains.
      ///
      enum IsolationType {

         /// Calorimeter isolation with R=0.10
         ///
         /// E<sub>T</sub>-sum in a cone with a half-opening angle of 0.1
         ///
         etcone10 = 0,

         /// Calorimeter isolation with R=0.15
         ///
         /// E<sub>T</sub>-sum in a cone with a half-opening angle of 0.15
         ///
         etcone15 = 1,

         /// Calorimeter isolation with R=0.20
         ///
         /// E<sub>T</sub>-sum in a cone with a half-opening angle of 0.2
         ///
         etcone20 = 2,

         /// Calorimeter isolation with R=0.25
         ///
         /// E<sub>T</sub>-sum in a cone with a half-opening angle of 0.25
         ///
         etcone25 = 3,

         /// Calorimeter isolation with R=0.30
         ///
         /// E<sub>T</sub>-sum in a cone with a half-opening angle of 0.3
         ///
         etcone30 = 4,

         /// Calorimeter isolation with R=0.35
         ///
         /// E<sub>T</sub>-sum in a cone with a half-opening angle of 0.35
         ///
         etcone35 = 5,

         /// Calorimeter isolation with R=0.40
         ///
         /// E<sub>T</sub>-sum in a cone with a half-opening angle of 0.4
         ///
         etcone40 = 6,

         /// Calorimeter isolation with R=0.45
         ///
         /// E<sub>T</sub>-sum in a cone with a half-opening angle of 0.45
         ///
         etcone45 = 7,

         /// Track isolation with R=0.10
         ///
         /// p<sub>T</sub>-sum of the tracks in a cone with a half-opening 
         /// angle of 0.1
         ///
         ptcone10 = 10,

         /// Track isolation with R=0.15
         ///
         /// p<sub>T</sub>-sum of the tracks in a cone with a half-opening 
         /// angle of 0.15
         ///
         ptcone15 = 11,

         /// Track isolation with R=0.20
         ///
         /// p<sub>T</sub>-sum of the tracks in a cone with a half-opening 
         /// angle of 0.2
         ///
         ptcone20 = 12,

         /// Track isolation with R=0.25
         ///
         /// p<sub>T</sub>-sum of the tracks in a cone with a half-opening 
         /// angle of 0.25
         ///
         ptcone25 = 13,

         /// Track isolation with R=0.30
         ///
         /// p<sub>T</sub>-sum of the tracks in a cone with a half-opening 
         /// angle of 0.3
         ///
         ptcone30 = 14,

         /// Track isolation with R=0.35
         ///
         /// p<sub>T</sub>-sum of the tracks in a cone with a half-opening 
         /// angle of 0.35
         ///
         ptcone35 = 15,

         /// Track isolation with R=0.40
         ///
         /// p<sub>T</sub>-sum of the tracks in a cone with a half-opening 
         /// angle of 0.4
         ///
         ptcone40 = 16,

         /// Track isolation with R=0.45
         ///
         /// p<sub>T</sub>-sum of the tracks in a cone with a half-opening 
         /// angle of 0.45
         ///
         ptcone45 = 17,

         /// Number of tracks with R=0.1
         ///
         /// Number of charged tracks in a cone with a half-opening angle
         /// of 0.1
         ///
         nucone10 = 20,

         /// Number of tracks with R=0.15
         ///
         /// Number of charged tracks in a cone with a half-opening angle
         /// of 0.15
         ///
         nucone15 = 21,

         /// Number of tracks with R=0.2
         ///
         /// Number of charged tracks in a cone with a half-opening angle
         /// of 0.2
         ///
         nucone20 = 22,

         /// Number of tracks with R=0.25
         ///
         /// Number of charged tracks in a cone with a half-opening angle
         /// of 0.25
         ///
         nucone25 = 23,

         /// Number of tracks with R=0.3
         ///
         /// Number of charged tracks in a cone with a half-opening angle
         /// of 0.3
         ///
         nucone30 = 24,

         /// Number of tracks with R=0.35
         ///
         /// Number of charged tracks in a cone with a half-opening angle
         /// of 0.35
         ///
         nucone35 = 25,

         /// Number of tracks with R=0.4
         ///
         /// Number of charged tracks in a cone with a half-opening angle
         /// of 0.4
         ///
         nucone40 = 26,

         /// Number of tracks with R=0.45
         ///
         /// Number of charged tracks in a cone with a half-opening angle
         /// of 0.45
         ///
         nucone45 = 27,

         /// Hadronic calorimeter isolation with R=0.10
         ///
         /// E<sub>T</sub>-sum, removing the electromagnetic component, in a
         /// cone with a half-opening angle of 0.1
         ///
         etconeNoEm10 = 30,

         /// Hadronic calorimeter isolation with R=0.15
         ///
         /// E<sub>T</sub>-sum, removing the electromagnetic component, in a
         /// cone with a half-opening angle of 0.15
         ///
         etconeNoEm15 = 31,

         /// Hadronic calorimeter isolation with R=0.20
         ///
         /// E<sub>T</sub>-sum, removing the electromagnetic component, in a
         /// cone with a half-opening angle of 0.2
         ///
         etconeNoEm20 = 32,

         /// Hadronic calorimeter isolation with R=0.25
         ///
         /// E<sub>T</sub>-sum, removing the electromagnetic component, in a
         /// cone with a half-opening angle of 0.25
         ///
         etconeNoEm25 = 33,

         /// Hadronic calorimeter isolation with R=0.30
         ///
         /// E<sub>T</sub>-sum, removing the electromagnetic component, in a
         /// cone with a half-opening angle of 0.3
         ///
         etconeNoEm30 = 34,

         /// Hadronic calorimeter isolation with R=0.35
         ///
         /// E<sub>T</sub>-sum, removing the electromagnetic component, in a
         /// cone with a half-opening angle of 0.35
         ///
         etconeNoEm35 = 35,

         /// Hadronic calorimeter isolation with R=0.40
         ///
         /// E<sub>T</sub>-sum, removing the electromagnetic component, in a
         /// cone with a half-opening angle of 0.4
         ///
         etconeNoEm40 = 36,

         /// Hadronic calorimeter isolation with R=0.45
         ///
         /// E<sub>T</sub>-sum, removing the electromagnetic component, in a
         /// cone with a half-opening angle of 0.45
         ///
         etconeNoEm45 = 37,

         /// p<sub>T</sub> corrected E<sub>T</sub> with R=0.10
         ///
         /// p<sub>T</sub> corrected E<sub>T</sub>-sum in a cone with a
         /// half-opening angle of 0.1
         ///
         etcone10_ptcorrected = 40,

         /// p<sub>T</sub> corrected E<sub>T</sub> with R=0.15
         ///
         /// p<sub>T</sub> corrected E<sub>T</sub>-sum in a cone with a
         /// half-opening angle of 0.15
         ///
         etcone15_ptcorrected = 41,

         /// p<sub>T</sub> corrected E<sub>T</sub> with R=0.20
         ///
         /// p<sub>T</sub> corrected E<sub>T</sub>-sum in a cone with a
         /// half-opening angle of 0.2
         ///
         etcone20_ptcorrected = 42,

         /// p<sub>T</sub> corrected E<sub>T</sub> with R=0.25
         ///
         /// p<sub>T</sub> corrected E<sub>T</sub>-sum in a cone with a
         /// half-opening angle of 0.25
         ///
         etcone25_ptcorrected = 43,

         /// p<sub>T</sub> corrected E<sub>T</sub> with R=0.30
         ///
         /// p<sub>T</sub> corrected E<sub>T</sub>-sum in a cone with a
         /// half-opening angle of 0.3
         ///
         etcone30_ptcorrected = 44,

         /// p<sub>T</sub> corrected E<sub>T</sub> with R=0.35
         ///
         /// p<sub>T</sub> corrected E<sub>T</sub>-sum in a cone with a
         /// half-opening angle of 0.35
         ///
         etcone35_ptcorrected = 45,

         /// p<sub>T</sub> corrected E<sub>T</sub> with R=0.40
         ///
         /// p<sub>T</sub> corrected E<sub>T</sub>-sum in a cone with a
         /// half-opening angle of 0.4
         ///
         etcone40_ptcorrected = 46,

         /// p<sub>T</sub> corrected E<sub>T</sub> with R=0.45
         ///
         /// p<sub>T</sub> corrected E<sub>T</sub>-sum in a cone with a
         /// half-opening angle of 0.45
         ///
         etcone45_ptcorrected = 47,

         /// p<sub>T</sub> and ED corrected E<sub>T</sub> with R=0.10
         ///
         /// Corrected E<sub>T</sub>-sum in a cone with a half-opening angle
         /// of 0.10
         ///
         etcone10_corrected = 50,

         /// p<sub>T</sub> and ED corrected E<sub>T</sub> with R=0.15
         ///
         /// Corrected E<sub>T</sub>-sum in a cone with a half-opening angle
         /// of 0.15
         ///
         etcone15_corrected = 51,

         /// p<sub>T</sub> and ED corrected E<sub>T</sub> with R=0.20
         ///
         /// Corrected E<sub>T</sub>-sum in a cone with a half-opening angle
         /// of 0.20
         ///
         etcone20_corrected = 52,

         /// p<sub>T</sub> and ED corrected E<sub>T</sub> with R=0.25
         ///
         /// Corrected E<sub>T</sub>-sum in a cone with a half-opening angle
         /// of 0.25
         ///
         etcone25_corrected = 53,

         /// p<sub>T</sub> and ED corrected E<sub>T</sub> with R=0.30
         ///
         /// Corrected E<sub>T</sub>-sum in a cone with a half-opening angle
         /// of 0.30
         ///
         etcone30_corrected = 54,

         /// p<sub>T</sub> and ED corrected E<sub>T</sub> with R=0.35
         ///
         /// Corrected E<sub>T</sub>-sum in a cone with a half-opening angle
         /// of 0.35
         ///
         etcone35_corrected = 55,

         /// p<sub>T</sub> and ED corrected E<sub>T</sub> with R=0.40
         ///
         /// Corrected E<sub>T</sub>-sum in a cone with a half-opening angle
         /// of 0.40
         ///
         etcone40_corrected = 56,

         /// p<sub>T</sub> and ED corrected E<sub>T</sub> with R=0.45
         ///
         /// Corrected E<sub>T</sub>-sum in a cone with a half-opening angle
         /// of 0.45
         ///
         etcone45_corrected = 57,

         /// Topo-cluster E<sub>T</sub>-sum for R=0.10
         ///
         /// E<sub>T</sub>-sum of the topo-clusters in a cone with a
         /// half-opening angle of 0.10
         ///
         topoetcone10 = 60,

         /// Topo-cluster E<sub>T</sub>-sum for R=0.15
         ///
         /// E<sub>T</sub>-sum of the topo-clusters in a cone with a
         /// half-opening angle of 0.15
         ///
         topoetcone15 = 61,

         /// Topo-cluster E<sub>T</sub>-sum for R=0.20
         ///
         /// E<sub>T</sub>-sum of the topo-clusters in a cone with a
         /// half-opening angle of 0.20
         ///
         topoetcone20 = 62,

         /// Topo-cluster E<sub>T</sub>-sum for R=0.25
         ///
         /// E<sub>T</sub>-sum of the topo-clusters in a cone with a
         /// half-opening angle of 0.25
         ///
         topoetcone25 = 63,

         /// Topo-cluster E<sub>T</sub>-sum for R=0.30
         ///
         /// E<sub>T</sub>-sum of the topo-clusters in a cone with a
         /// half-opening angle of 0.30
         ///
         topoetcone30 = 64,

         /// Topo-cluster E<sub>T</sub>-sum for R=0.35
         ///
         /// E<sub>T</sub>-sum of the topo-clusters in a cone with a
         /// half-opening angle of 0.35
         ///
         topoetcone35 = 65,

         /// Topo-cluster E<sub>T</sub>-sum for R=0.40
         ///
         /// E<sub>T</sub>-sum of the topo-clusters in a cone with a
         /// half-opening angle of 0.40
         ///
         topoetcone40 = 66,

         /// Topo-cluster E<sub>T</sub>-sum for R=0.45
         ///
         /// E<sub>T</sub>-sum of the topo-clusters in a cone with a
         /// half-opening angle of 0.45
         ///
         topoetcone45 = 67,

         /// p<sub>T</sub>-corrected topo-cluster E<sub>T</sub>-sum for R=0.10
         ///
         /// p<sub>T</sub>-corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.10
         ///
         topoetcone10_ptcorrected = 70,

         /// p<sub>T</sub>-corrected topo-cluster E<sub>T</sub>-sum for R=0.15
         ///
         /// p<sub>T</sub>-corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.15
         ///
         topoetcone15_ptcorrected = 71,

         /// p<sub>T</sub>-corrected topo-cluster E<sub>T</sub>-sum for R=0.20
         ///
         /// p<sub>T</sub>-corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.20
         ///
         topoetcone20_ptcorrected = 72,

         /// p<sub>T</sub>-corrected topo-cluster E<sub>T</sub>-sum for R=0.25
         ///
         /// p<sub>T</sub>-corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.25
         ///
         topoetcone25_ptcorrected = 73,

         /// p<sub>T</sub>-corrected topo-cluster E<sub>T</sub>-sum for R=0.30
         ///
         /// p<sub>T</sub>-corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.30
         ///
         topoetcone30_ptcorrected = 74,

         /// p<sub>T</sub>-corrected topo-cluster E<sub>T</sub>-sum for R=0.35
         ///
         /// p<sub>T</sub>-corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.35
         ///
         topoetcone35_ptcorrected = 75,

         /// p<sub>T</sub>-corrected topo-cluster E<sub>T</sub>-sum for R=0.40
         ///
         /// p<sub>T</sub>-corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.40
         ///
         topoetcone40_ptcorrected = 76,

         /// p<sub>T</sub>-corrected topo-cluster E<sub>T</sub>-sum for R=0.45
         ///
         /// p<sub>T</sub>-corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.45
         ///
         topoetcone45_ptcorrected = 77,

         /// Fully corrected topo-cluster E<sub>T</sub>-sum for R=0.10
         ///
         /// Fully corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.10
         ///
         topoetcone10_corrected = 80,

         /// Fully corrected topo-cluster E<sub>T</sub>-sum for R=0.15
         ///
         /// Fully corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.15
         ///
         topoetcone15_corrected = 81,

         /// Fully corrected topo-cluster E<sub>T</sub>-sum for R=0.20
         ///
         /// Fully corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.20
         ///
         topoetcone20_corrected = 82,

         /// Fully corrected topo-cluster E<sub>T</sub>-sum for R=0.25
         ///
         /// Fully corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.25
         ///
         topoetcone25_corrected = 83,

         /// Fully corrected topo-cluster E<sub>T</sub>-sum for R=0.30
         ///
         /// Fully corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.30
         ///
         topoetcone30_corrected = 84,

         /// Fully corrected topo-cluster E<sub>T</sub>-sum for R=0.35
         ///
         /// Fully corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.35
         ///
         topoetcone35_corrected = 85,

         /// Fully corrected topo-cluster E<sub>T</sub>-sum for R=0.40
         ///
         /// Fully corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.30
         ///
         topoetcone40_corrected = 86,

         /// Fully corrected topo-cluster E<sub>T</sub>-sum for R=0.45
         ///
         /// Fully corrected E<sub>T</sub>-sum of the topo-clusters in
         /// a cone with a half-opening angle of 0.45
         ///
         topoetcone45_corrected = 87,
	 
	 /// @name Topo-cluster E<sub>T</sub>-sum with 5x7 core subracted.     
	 /// @{

	 topoetcone10_core57cells = 90,
	 topoetcone15_core57cells = 91,
	 topoetcone20_core57cells = 92,
	 topoetcone25_core57cells = 93,
	 topoetcone30_core57cells = 94,
	 topoetcone35_core57cells = 95,
	 topoetcone40_core57cells = 96,
	 topoetcone45_core57cells = 97,

	 /// @}
	 
	 /// @name Ptcorrected Topo-cluster E<sub>T</sub>-sum with 5x7 core subracted.     
	 /// @{

	 topoetcone10_core57cells_ptcorrected = 100,
	 topoetcone15_core57cells_ptcorrected = 101,
	 topoetcone20_core57cells_ptcorrected = 102,
	 topoetcone25_core57cells_ptcorrected = 103,
	 topoetcone30_core57cells_ptcorrected = 104,
	 topoetcone35_core57cells_ptcorrected = 105,
	 topoetcone40_core57cells_ptcorrected = 106,
	 topoetcone45_core57cells_ptcorrected = 107,
	 
	 /// @}

	 /// @name Fully Corrected Topo-cluster E<sub>T</sub>-sum with 5x7 core subracted.     
	 /// @{

	 topoetcone10_core57cells_corrected = 110,
	 topoetcone15_core57cells_corrected = 110,
	 topoetcone20_core57cells_corrected = 112,
	 topoetcone25_core57cells_corrected = 112,
	 topoetcone30_core57cells_corrected = 114,
	 topoetcone35_core57cells_corrected = 114,
	 topoetcone40_core57cells_corrected = 116,
	 topoetcone45_core57cells_corrected = 117
	 
	 /// @}

      }; // enum IsolationType


     /// Enumeration for different ways of calculating isolation in xAOD files
     ///
     enum IsolationFlavour {
       /// Calorimeter isolation 
       etcone = 0,

       /// Track isolation
       ptcone = 1,

       /// Number of tracks
       nucone = 2,

       /// Hadronic calorimeter isolation
       etconeNoEm = 3,

       /// p<sub>T</sub> corrected E<sub>T</sub> 
       etcone_ptcorrected = 4,

       /// p<sub>T</sub> and ED corrected E<sub>T</sub>
       etcone_corrected = 5,

       /// Topo-cluster E<sub>T</sub>-sum 
       topoetcone = 6,

       /// p<sub>T</sub>-corrected topo-cluster E<sub>T</sub>-sum 
       topoetcone_ptcorrected = 7,

       /// Fully corrected topo-cluster E<sub>T</sub>-sum
       topoetcone_corrected = 8,

       /// Topo-cluster E<sub>T</sub>-sum with 5x7 core subtracted
       topoetcone_core57cells = 9,

       /// ptcorrected Topo-cluster E<sub>T</sub>-sum with 5x7 core subtracted
       topoetcone_core57cells_ptcorrected = 10,
       
       ///corrected Topo-cluster E<sub>T</sub>-sum with 5x7 core subtracted
       topoetcone_core57cells_corrected = 11

     }; // enum IsolationFlavour

     /// convert Isolation Type into cone size
     inline float coneSize( IsolationType type ){
       return 0.1 + (type%10)*0.05;
     }

     /// convert Isolation Type into cone size
     inline IsolationFlavour isolationFlavour( IsolationType type ){
       return static_cast<IsolationFlavour>(type/10);
     }

     

   } // namespace Iso

} // namespace xAOD

#endif // XAODPRIMITIVES_ISOLATIONTYPE_H
