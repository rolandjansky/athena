// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGERALGS_MUCTPI_RDOTOROIBRESULT_H
#define ANALYSISTRIGGERALGS_MUCTPI_RDOTOROIBRESULT_H

// STL include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

/**
 *  @short Algorithm fixing the muon RoI information in the data
 *
 *         This algorithm can create a fixed ROIB::RoIBResult object from the
 *         incorrect object in the data, plus the MuCTPI_RDO readout data.
 *
 * @author Attila Krasznahorkay Jr.
 *
 * $Revision: 275499 $
 * $Date: 2010-01-27 19:25:00 +0100 (Wed, 27 Jan 2010) $
 */
class MuCTPI_RDOToRoIBResult : public AthAlgorithm {

public:
   /// Regular algorithm constructor
   MuCTPI_RDOToRoIBResult( const std::string& name, ISvcLocator* pSvcLocator );

   /// Regular algorithm execure function
   virtual StatusCode execute() override;

private:
   /// Extract the BCID stored in the multiplicity word
   static uint32_t multiplicityBCID( uint32_t multi_word );
   /// Transform a data word into an RoI word
   static uint32_t toRoIWord( uint32_t data_word );
   /// Extract the BCID stored in the data word
   static uint32_t roiBCID( uint32_t data_word );
   /// Extract whether the candidate was accepted to be sent to LVL2
   static bool roiAccepted( uint32_t data_word );

   std::string m_muctpiInputKey; ///< Key of the MuCTPI_RDO object
   std::string m_roibInputKey; ///< Key of the old (incorrect) RoIBResult object
   std::string m_roibOutputKey; ///< Key of the new (correct) RoIBResult object

}; // class MuCTPI_RDOToRoIBResult

#endif // ANALYSISTRIGGERALGS_MUCTPI_RDOTOROIBRESULT_H
