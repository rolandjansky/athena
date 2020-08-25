// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleCompressorTool.h 789457 2016-12-13 12:00:43Z krasznaa $
#ifndef XAODTRACKINGCNV_TRACKPARTICLECOMPRESSORTOOL_H
#define XAODTRACKINGCNV_TRACKPARTICLECOMPRESSORTOOL_H

// System include(s):
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// CxxUtils include(s):
#include "CxxUtils/FloatCompressor.h"

// Local include(s):
#include "xAODTrackingCnv/ITrackParticleCompressorTool.h"

namespace xAODMaker {

   /// Implementation of the tool compressing track particles
   ///
   /// This tool is used by the POOL converter code to "compress" some of the
   /// float properties of track particles before writing them to disk. To make
   /// the xAODs smaller.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 789457 $
   /// $Date: 2016-12-13 13:00:43 +0100 (Tue, 13 Dec 2016) $
   ///
   class TrackParticleCompressorTool : public AthAlgTool,
         public virtual ITrackParticleCompressorTool {

   public:
      /// Regular AlgTool constructor
      TrackParticleCompressorTool( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent );

      /// Function initialising the tool
      StatusCode initialize() override;

      /// The function doing the heavy lifting
      StatusCode compress( xAOD::TrackParticle& tp ) const override;

   private:
      /// The number of mantissa bits to keep for off-diagonal covariance matrix
      /// elements
      int m_offDiagCovMatrixBits;
      /// The number of mantissa bits to keep for the diagonal covariance matrix
      /// elements
      int m_diagCovMatrixBits;
      // Flag to control compression scheme of off-diagonal elements
      bool m_useOffDiagCompr;

      /// Helper object for compressing the on-diagonal covariance matrix
      /// elements
      std::unique_ptr< CxxUtils::FloatCompressor > m_diagCovMatrixCompressor;

   }; // class TrackParticleCompressorTool

} // namespace xAODMaker

#endif // XAODTRACKINGCNV_TRACKPARTICLECOMPRESSORTOOL_H
