// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEvent_v1.h 761796 2016-07-14 08:06:02Z krasznaa $
#ifndef XAODTRUTH_VERSIONS_TRUTHEVENT_V1_H
#define XAODTRUTH_VERSIONS_TRUTHEVENT_V1_H

// System include(s):
#include <utility>
#include <vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"
#include "xAODBase/ObjectType.h"

// Local include(s):
#include "xAODTruth/TruthEventBase.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

namespace xAOD {

   /// Class describing a signal truth event in the MC record
   ///
   /// @author Jovan Mitervski <Jovan.Mitrevski@cern.ch>
   /// @author Andy Buckley <Andy.Buckley@cern.ch>
   ///
   /// $Revision: 761796 $
   /// $Date: 2016-07-14 10:06:02 +0200 (Thu, 14 Jul 2016) $
   ///
   class TruthEvent_v1 : public TruthEventBase_v1 {

   public:
      /// Default constructor
      TruthEvent_v1();

      /// @name Simple event properties
      /// @{

      /// Const access to the weights vector
      /// @todo Need to add the map-like interface for the weights: very important!
      const std::vector< float >& weights() const;
      /// Set the event weights
      /// @todo Need to add named weight access: vector<string>
      void setWeights( const std::vector< float >& weights );

      /// Get the cross section
      float crossSection() const;
      /// Set the cross-section
      void setCrossSection( float value );

      /// Get the cross section error
      float crossSectionError() const;
      /// Set the cross-section error
      void setCrossSectionError( float value );

      /// Set the cross-section and its error
      void setCrossSection( float value, float error );

      /// @}

      /// @name Parton density info
      ///
      /// Optional due to particle gun & NLO events where a single PDF info
      /// doesn't work.
      ///
      /// @{

      /// Accessor enums for PDF info parameter lookup
      enum PdfParam {
         PDGID1 = 0, ///< [int]
         PDGID2 = 1, ///< [int]
         PDFID1 = 2, ///< [int]
         PDFID2 = 3, ///< [int]
         X1 = 4,     ///< [float]
         X2 = 5,     ///< [float]
         SCALE = 6,  ///< Not implemented!!!
         Q = 6,      ///< [float]
         PDF1 = 7,   ///< Not implemented!!!
         PDF2 = 8,   ///< Not implemented!!!
         XF1 = 7,    ///< [float]
         XF2 = 8     ///< [float]
      }; // enum PdfParam

      /// Read an integer PDF info parameter
      bool pdfInfoParameter( int& value, PdfParam parameter ) const;
      /// Read a floating point PDF info parameter
      bool pdfInfoParameter( float& value, PdfParam parameter ) const;

      /// Set an integer PDF info parameter
      bool setPdfInfoParameter( int value, PdfParam parameter );
      /// Set a floating point PDF info parameter
      bool setPdfInfoParameter( float value, PdfParam parameter );

      /// Helper struct holding a full set of PDF info values
      struct PdfInfo {

         /// Constructor to set (invalid) defaults
         PdfInfo();

         /// Check if all the variables in the object are valid
         bool valid() const;

         int pdgId1;
         int pdgId2;
         int pdfId1;
         int pdfId2;
         float x1;
         float x2;
         float Q;
         float xf1;
         float xf2;

      }; // struct PdfInfo

      /// Retrieve a full PdfInfo with a single call
      /// @note May have invalid values -- use valid() to check.
      PdfInfo pdfInfo() const;

      /// @}

      /// @name Heavy ion info
      /// @{

      /// Heavy ion parameter enum
      enum HIParam {
         NCOLLHARD = 0,                  ///< [int]
         NPARTPROJ = 1,                  ///< [int]
         NPARTTARG = 2,                  ///< [int]
         NCOLL = 3,                      ///< [int]
         SPECTATORNEUTRONS = 4,          ///< [int]
         SPECTATORPROTONS = 5,           ///< [int]
         NNWOUNDEDCOLLISIONS = 6,        ///< [int]
         NWOUNDEDNCOLLISIONS = 7,        ///< [int]
         NWOUNDEDNWOUNDEDCOLLISIONS = 8, ///< [int]
         IMPACTPARAMETER = 9,            ///< [float]
         EVENTPLANEANGLE = 10,           ///< [float]
         ECCENTRICITY = 11,              ///< [float]
         SIGMAINELNN = 12,               ///< [float]
         CENTRALITY = 13,                ///< [float]
      }; // enum HIParam

      /// Read an integer HI parameter
      bool heavyIonParameter( int& value, HIParam parameter ) const;
      /// Read a floating point HI parameter
      bool heavyIonParameter( float& value, HIParam parameter ) const;

      /// Set an integer HI parameter
      bool setHeavyIonParameter( int value, HIParam parameter );
      /// Set a floating point HI parameter
      bool setHeavyIonParameter( float value, HIParam parameter );

      /// @}

      /// @name Links to particles and vertices in the event
      /// @{

      /// Type of the links pointing at truth vertices
      typedef ElementLink< TruthVertexContainer > TruthVertexLink_t;

      /// Pointer to a vertex representing the primary beam interaction point
      ///
      /// The naming of the function is a bit misleading. The returned vertex
      /// can only be interpreted as an interaction *position*.
      ///
      const TruthVertex* signalProcessVertex() const;
      /// Link to the vertex representing the primary beam interaction point
      const TruthVertexLink_t& signalProcessVertexLink() const;
      /// Set pointer to a vertex representing the primary beam interaction point
      void setSignalProcessVertexLink( const TruthVertexLink_t& link );

      /// Type of the links pointing at truth particles
      typedef ElementLink< TruthParticleContainer > TruthParticleLink_t;

      /// Pair of pointers to the two incoming beam particles
      std::pair< const TruthParticle*, const TruthParticle* >
      beamParticles() const;

      /// Set incoming beam particles
      void setBeamParticleLinks( const TruthParticleLink_t& pcl1,
                                 const TruthParticleLink_t& pcl2 );

      /// Get the link to the first incoming beam particle
      const TruthParticleLink_t& beamParticle1Link() const;
      /// Set one incoming beam particle
      void setBeamParticle1Link( const TruthParticleLink_t& pcl1 );

      /// Get the link to the second incoming beam particle
      const TruthParticleLink_t& beamParticle2Link() const;
      /// Set one incoming beam particle
      void setBeamParticle2Link( const TruthParticleLink_t& pcl2 );

      /// @}

      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const;

      /// Function making sure that the object is ready for persistification
      void toPersistent();

   }; // class TruthEvent_v1

} // namespace xAOD

// Declare the base class of TruthEvent_v1 to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthEvent_v1, xAOD::TruthEventBase_v1 );

#include "AthContainers/DataVector.h"
DATAVECTOR_BASE( xAOD::TruthEvent_v1, xAOD::TruthEventBase_v1 );


#endif // XAODTRUTH_VERSIONS_TRUTHEVENT_V1_H
