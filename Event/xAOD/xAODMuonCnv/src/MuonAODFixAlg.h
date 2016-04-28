// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonAODFixAlg.h 297747 2013-10-28 15:14:24Z krasznaa $
#ifndef XAODCREATORALGS_MUONAODFIXALG_H
#define XAODCREATORALGS_MUONAODFIXALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// xAOD include
#include "xAODMuon/MuonContainer.h"



namespace xAOD {

   /**
    *  @short Algorithm to fix AOD muons
    *
    * @author Edward Moyse <Edward.Moyse@cern.ch>
    *
    */
   class MuonAODFixAlg : public AthAlgorithm {

   public:
      /// Regular algorithm constructor
      MuonAODFixAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

      void fixMuon(xAOD::Muon& muon) const;
      
   private:

      /// The key for the output xAOD::CaloClusterContainer
      std::string m_containerName;
   }; // class MuonAODFixAlg

} // namespace xAODMaker

#endif 
