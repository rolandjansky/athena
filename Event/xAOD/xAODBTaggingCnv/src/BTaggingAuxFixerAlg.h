// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAuxFixerAlg.h 802614 2017-04-10 13:30:46Z krasznaa $
#ifndef XAODBTAGGINGCNV_BTAGGINGAUXFIXERALG_H
#define XAODBTAGGINGCNV_BTAGGINGAUXFIXERALG_H

// System include(s):
#include <vector>
#include <string>
#include <map>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace xAODMaker {

   /// Algorithm fixing the auxiliary variables in xAOD::BTaggingAuxContainer
   ///
   /// This algorithm can be used to fix corrupt dynamic auxiliary variables
   /// in AODs produced with release 21.0.X. To be used from inside of AODFix.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 802614 $
   /// $Date: 2017-04-10 15:30:46 +0200 (Mon, 10 Apr 2017) $
   ///
   class BTaggingAuxFixerAlg : public AthAlgorithm {

   public:
      /// Regular algorithm constructor
      BTaggingAuxFixerAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      StatusCode initialize() override;
      /// Function executing the algorithm
      StatusCode execute() override;

   private:
      /// @name Algorithm properties
      /// @{

      /// The names of the containers to massage
      std::vector< std::string > m_containers;
      /// The TrackParticle link variables to consider
      std::vector< std::string > m_tpLinkNames;
      /// The Vertex link variables to consider
      std::vector< std::string > m_vxLinkNames;

      /// @}

      /// Helper variable used to iterate over the TrackParticle variables
      std::vector< std::pair< std::string, SG::auxid_t > > m_tpLinkNameIDs;
      /// Helper variable used to iterate over the Vertex variables
      std::vector< std::pair< std::string, SG::auxid_t > > m_vxLinkNameIDs;

   }; // class BTaggingAuxFixerAlg

} // namespace xAODMaker

#endif // XAODBTAGGINGCNV_BTAGGINGAUXFIXERALG_H
