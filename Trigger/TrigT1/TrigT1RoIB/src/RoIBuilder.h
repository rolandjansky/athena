// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RoIBuilder.h 500598 2012-05-14 15:39:58Z krasznaa $
#ifndef TRIGT1ROIB_ROIBUILDER_H
#define TRIGT1ROIB_ROIBUILDER_H

// STL include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

//! namespace for RoIBuilder related classes
namespace ROIB {

   /**
    *  @short RoIBuilder simulation algorithm
    *
    *         The RoIBuilder algorithm concatenates the different L1 RODs from
    *         calo, muon and CTP and fills the L1 RDO.
    *
    * @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>,
    * @author Attila Kraznahorkay Jr. <Attila.Krasznahorkay@cern.ch>
    * @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
    *
    * $Revision: 500598 $
    * $Date: 2012-05-14 17:39:58 +0200 (Mon, 14 May 2012) $
    */
   class RoIBuilder : public AthAlgorithm {

   public:
      /// Standard Gaudi algorithm constructor
      RoIBuilder( const std::string& name, ISvcLocator* pSvcLocator ) ;

      // standard algorithm methods:
      virtual StatusCode initialize();
      virtual StatusCode execute();
      virtual StatusCode finalize();

   private:
      // Properties:
      bool m_doCalo; //!< property, see @link RoIBuilder::RoIBuilder @endlink
      bool m_doMuon; //!< property, see @link RoIBuilder::RoIBuilder @endlink

      // String members containing locations of objects in SG:
      std::string m_ctpSLinkLocation;      //!< property, see @link RoIBuilder::RoIBuilder @endlink
      std::string m_caloEMTauLocation;     //!< property, see @link RoIBuilder::RoIBuilder @endlink
      std::string m_caloJetEnergyLocation; //!< property, see @link RoIBuilder::RoIBuilder @endlink
      std::string m_muctpiSLinkLocation;   //!< property, see @link RoIBuilder::RoIBuilder @endlink
      std::string m_roibRDOLocation;       //!< property, see @link RoIBuilder::RoIBuilder @endlink

   }; // class RoIBuilder

} // namespace ROIB

#endif // TRIGT1ROIB_ROIBUILDER_H
