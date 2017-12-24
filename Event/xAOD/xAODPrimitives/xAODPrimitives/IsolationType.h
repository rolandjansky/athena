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

         ptvarcone20_TightTTVA_pt500 = 48,
         ptvarcone30_TightTTVA_pt500 = 50,
         ptvarcone40_TightTTVA_pt500 = 52,

         /// @}
         
         numIsolationTypes=53

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

              case ptvarcone20_TightTTVA_pt500:    return "ptvarcone20_TightTTVA_pt500";
              case ptvarcone30_TightTTVA_pt500:    return "ptvarcone30_TightTTVA_pt500";
              case ptvarcone40_TightTTVA_pt500:    return "ptvarcone40_TightTTVA_pt500";

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
