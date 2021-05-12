// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsolationType.h 718726 2016-01-20 12:10:22Z dzhang $
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

         /// Calorimeter isolation 
         ///
         /// E<sub>T</sub>-sum in a cone 
         ///
         etcone20 = 2,
         etcone30 = 4,
         etcone40 = 6,

         /// Track isolation 
         ///
         /// p<sub>T</sub>-sum of the tracks in a cone 
         ///
         ptcone20 = 12,
         ptcone30 = 14,
         ptcone40 = 16,
	 ptcone50 = 18,
         /// Topo-cluster E<sub>T</sub>-sum 
         ///
         /// E<sub>T</sub>-sum of the topo-clusters in a cone
         ///
         topoetcone20 = 22,
         topoetcone30 = 24,
         topoetcone40 = 26,

         /// Mini-Isolation http://arxiv.org/abs/1007.2221 
         /// @{

         ptvarcone20 = 32,
         ptvarcone30 = 34,
         ptvarcone40 = 36,

         /// @}

         /// Neutral eflow isolation
         /// @{
         
         neflowisol20 = 42,
         neflowisol30 = 44,
         neflowisol40 = 46,

         /// @}

         /// Mini-Isolation http://arxiv.org/abs/1007.2221 for high mu
         /// @{

         ptvarcone20_TightTTVA_pt500 = 52,
         ptvarcone30_TightTTVA_pt500 = 54,
         ptvarcone40_TightTTVA_pt500 = 56,
         ptvarcone20_TightTTVA_pt1000 = 62,
         ptvarcone30_TightTTVA_pt1000 = 64,
         ptvarcone40_TightTTVA_pt1000 = 66,

         /// @}

         /// Ptcone http://arxiv.org/abs/1007.2221 for high mu
         /// @{

         ptcone20_TightTTVA_pt500 = 72,
         ptcone30_TightTTVA_pt500 = 74,
         ptcone40_TightTTVA_pt500 = 76,
         ptcone20_TightTTVA_pt1000 = 82,
         ptcone30_TightTTVA_pt1000 = 84,
         ptcone40_TightTTVA_pt1000 = 86,

         /// @}

         /// Mini-Isolation http://arxiv.org/abs/1007.2221 for high mu
         /// @{

         ptvarcone20_TightTTVALooseCone_pt500 = 92,
         ptvarcone30_TightTTVALooseCone_pt500 = 94,
         ptvarcone40_TightTTVALooseCone_pt500 = 96,
         ptvarcone20_TightTTVALooseCone_pt1000 = 102,
         ptvarcone30_TightTTVALooseCone_pt1000 = 104,
         ptvarcone40_TightTTVALooseCone_pt1000 = 106,

         /// @}

         /// Ptcone http://arxiv.org/abs/1007.2221 for high mu
         /// @{

         ptcone20_TightTTVALooseCone_pt500 = 112,
         ptcone30_TightTTVALooseCone_pt500 = 114,
         ptcone40_TightTTVALooseCone_pt500 = 116,
         ptcone20_TightTTVALooseCone_pt1000 = 122,
         ptcone30_TightTTVALooseCone_pt1000 = 124,
         ptcone40_TightTTVALooseCone_pt1000 = 126,

         /// @}
         
         numIsolationTypes=127

      }; // enum IsolationType
      
      static inline const char* toString(IsolationType type)
      {
          switch (type)
          {
              case etcone20:    return "etcone20";
              case etcone30:    return "etcone30";
              case etcone40:    return "etcone40";
              
              case ptcone20:    return "ptcone20";
              case ptcone30:    return "ptcone30";
              case ptcone40:    return "ptcone40";
	      case ptcone50:    return "ptcone50";

              case ptvarcone20:    return "ptvarcone20";
              case ptvarcone30:    return "ptvarcone30";
              case ptvarcone40:    return "ptvarcone40";

              case ptcone20_TightTTVA_pt500:    return "ptcone20_TightTTVA_pt500";
              case ptcone30_TightTTVA_pt500:    return "ptcone30_TightTTVA_pt500";
              case ptcone40_TightTTVA_pt500:    return "ptcone40_TightTTVA_pt500";

              case ptcone20_TightTTVA_pt1000:    return "ptcone20_TightTTVA_pt1000";
              case ptcone30_TightTTVA_pt1000:    return "ptcone30_TightTTVA_pt1000";
              case ptcone40_TightTTVA_pt1000:    return "ptcone40_TightTTVA_pt1000";

              case ptvarcone20_TightTTVA_pt500:    return "ptvarcone20_TightTTVA_pt500";
              case ptvarcone30_TightTTVA_pt500:    return "ptvarcone30_TightTTVA_pt500";
              case ptvarcone40_TightTTVA_pt500:    return "ptvarcone40_TightTTVA_pt500";

              case ptvarcone20_TightTTVA_pt1000:    return "ptvarcone20_TightTTVA_pt1000";
              case ptvarcone30_TightTTVA_pt1000:    return "ptvarcone30_TightTTVA_pt1000";
              case ptvarcone40_TightTTVA_pt1000:    return "ptvarcone40_TightTTVA_pt1000";

              case ptcone20_TightTTVALooseCone_pt500:    return "ptcone20_TightTTVALooseCone_pt500";
              case ptcone30_TightTTVALooseCone_pt500:    return "ptcone30_TightTTVALooseCone_pt500";
              case ptcone40_TightTTVALooseCone_pt500:    return "ptcone40_TightTTVALooseCone_pt500";

              case ptcone20_TightTTVALooseCone_pt1000:    return "ptcone20_TightTTVALooseCone_pt1000";
              case ptcone30_TightTTVALooseCone_pt1000:    return "ptcone30_TightTTVALooseCone_pt1000";
              case ptcone40_TightTTVALooseCone_pt1000:    return "ptcone40_TightTTVALooseCone_pt1000";

              case ptvarcone20_TightTTVALooseCone_pt500:    return "ptvarcone20_TightTTVALooseCone_pt500";
              case ptvarcone30_TightTTVALooseCone_pt500:    return "ptvarcone30_TightTTVALooseCone_pt500";
              case ptvarcone40_TightTTVALooseCone_pt500:    return "ptvarcone40_TightTTVALooseCone_pt500";

              case ptvarcone20_TightTTVALooseCone_pt1000:    return "ptvarcone20_TightTTVALooseCone_pt1000";
              case ptvarcone30_TightTTVALooseCone_pt1000:    return "ptvarcone30_TightTTVALooseCone_pt1000";
              case ptvarcone40_TightTTVALooseCone_pt1000:    return "ptvarcone40_TightTTVALooseCone_pt1000";

              case topoetcone20:    return "topoetcone20";
              case topoetcone30:    return "topoetcone30";
              case topoetcone40:    return "topoetcone40";

              case neflowisol20:    return "neflowisol20";
              case neflowisol30:    return "neflowisol30";
              case neflowisol40:    return "neflowisol40";

              default:      return "[Unknown IsolationType]";
          }
      }

      struct ROOT6_NamespaceAutoloadHook{};
      
   } // namespace Iso

} // namespace xAOD

#endif // XAODPRIMITIVES_ISOLATIONTYPE_H
