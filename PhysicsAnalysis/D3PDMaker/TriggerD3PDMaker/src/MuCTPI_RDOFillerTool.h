// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuCTPI_RDOFillerTool.h 364692 2011-05-09 17:07:07Z krasznaa $
#ifndef TRIGGERD3PDMAKER_MUCTPI_RDOFILLERTOOL_H
#define TRIGGERD3PDMAKER_MUCTPI_RDOFILLERTOOL_H

// STL include(s):
#include <vector>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"

// Trigger include(s):
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool for MuCTPI_RDO objects
    *
    *         This filler tool can be used to save the properties of MuCTPI_RDO
    *         objects into the output D3PD. The MuCTPI_RDO object is the readout
    *         data of the Muon to CTP Interface, used in the LVL1 muon trigger.
    *         This data is only available on the ESD level.
    *
    *         The tool is able to "reconstruct" some of the information about the
    *         LVL1 muon candidates stored by the object. The most important information
    *         from these are the eta and phi positions of the candidates. The rest
    *         can actually be calculated from the dataWords themselves...
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 364692 $
    * $Date: 2011-05-09 19:07:07 +0200 (Mon, 09 May 2011) $
    */
   class MuCTPI_RDOFillerTool : public BlockFillerTool< MuCTPI_RDO > {

   public:
      /// Regular AlgTool constructor
      MuCTPI_RDOFillerTool( const std::string& type, const std::string& name,
                            const IInterface* parent );

      /// Standard AlgTool initialization function
      virtual StatusCode initialize();

      /// Function booking the variables in the output
      virtual StatusCode book();
      /// Function filling the variables for the output
      virtual StatusCode fill( const MuCTPI_RDO& rdo );

   private:
      /// Function creating an RoI word from the MuCTPI data words
      static uint32_t convertToRoIWord( uint32_t dataWord );
      /// Function extracting the 3-bit BCID from the data words
      static short getBCID( uint32_t dataWord );

      bool m_writeRawInfo; ///< Write the basic information from the MuCTPI_RDO
      bool m_writeReconstructedInfo; ///< Write the "reconstructed" info from the MuCTPI_RDO

      ServiceHandle< LVL1::RecMuonRoiSvc > m_rpcRoiSvc; ///< RPC rec. RoI service
      ServiceHandle< LVL1::RecMuonRoiSvc > m_tgcRoiSvc; ///< TGC rec. RoI service

      std::vector< uint32_t >* m_candidateMultiplicities; ///< Cand. multi. sent to the CTP
      int*                     m_nDataWords; ///< Number of muon candidates in readout window
      std::vector< uint32_t >* m_dataWords; ///< The 32-bit dataWords of the muon candidates

      std::vector< float >* m_dw_eta; ///< Eta position of the muon candidate
      std::vector< float >* m_dw_phi; ///< Phi position of the muon candidate
      std::vector< short >* m_dw_source; ///< Detector source of the muon candidate
      std::vector< short >* m_dw_hemisphere; ///< Hemisphere where the candidate was seen
      std::vector< short >* m_dw_bcid; ///< BCID in which the candidate was received
      std::vector< short >* m_dw_sectorID; ///< SectorID of the muon candidate
      std::vector< short >* m_dw_thrNumber; ///< Threshold number of the muon candidate
      std::vector< short >* m_dw_roi; ///< RoI (subsector) number of the muon candidate
      std::vector< short >* m_dw_overlapFlags; ///< Overlap flags of the muon candidate
      std::vector< short >* m_dw_firstCandidate; ///< Candidate had highest pT in sector
      std::vector< short >* m_dw_moreCandInRoI; ///< More candidates were in the RoI
      std::vector< short >* m_dw_moreCandInSector; ///< More than 2 candidates in the sector
      std::vector< short >* m_dw_charge; ///< Candidate sign (0=negative,1=positive,100=undefined)
      std::vector< short >* m_dw_vetoed; ///< Candidate vetoed in multiplicity sum

   }; // class MuCTPI_RDOFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_MUCTPI_RDOFILLERTOOL_H
