/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          RecMuonRoI.h  -  description
                             -------------------
    begin                : Tue Feb 25 2003
    email                : Thorsten Wengler
 ***************************************************************************/

#ifndef TRIGT1INTERFACES_RECMUONROI_H
#define TRIGT1INTERFACES_RECMUONROI_H

// STL include(s):
#include <string>
#include <vector>

// Gaudi/Athena include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "TrigT1Interfaces/iRecCoordRoI.h"

// Forward declaration(s):
namespace TrigConf {
  class TriggerThreshold;
}

namespace LVL1 {

   // Forward declaration(s):
   class RecMuonRoiSvc;
   class ITrigT1MuonRecRoiTool;

   /** Bit masks used to decipher the 32 bit words of a muon candidate*/
   enum BitMask {
      FirsCandMask                 = 0x00400000,
      SectorAddressMask            = 0x003fc000,
      SysIDMask                    = 0x000000c0,
      SubSysIDMask                 = 0x00000001,
      SectNumBarrelMask            = 0x0000003e,
      SectNumEndcapMask            = 0x0000007e,
      SectNumForwardMask           = 0x0000003e,
      ThresNumMask                 = 0x00003800,
      RoIOvlMask                   = 0x000007fc,
      BarrelRoIMask                = 0x0000001f,
      BarrelOvlMask                = 0x00000180,
      EndcapRoIMask                = 0x000000ff,
      EndcapOvlMask                = 0x00000100,
      ForwardRoIMask               = 0x0000003f,
      PadOflMask                   = 0x00000002,
      SectOflMask                  = 0x00000001,
      TGCCandidateSignMask         = 0x08000000,
      CandidateVetoMask            = 0x10000000
   };

   /** This class defines the reconstructed Muon ROI. 
       It is generated from the Slink output of TrigT1Muctpi


       @author Thorsten Wengler
   */
   class RecMuonRoI : public iRecCoordRoI {

   public:
      /// Charge sign of the muon candidate
      enum ChargeSign {
         NEGATIVE = 0, ///< Negative (mu minus) candidate in the TGC
         POSITIVE = 1, ///< Positive (mu plus) candidate in the TGC
         UNDEFINED = 100 ///< Candidate coming from the RPC
      }; // enum CandidateSign

      // constructor
      RecMuonRoI() {}
      // constructor
      RecMuonRoI( unsigned int roIWord, const RecMuonRoiSvc* theRecRPCRoiSvc,
                  const RecMuonRoiSvc* theRecTGCRoiSvc,
                  const std::vector< TrigConf::TriggerThreshold* >* theMuonConfig );
      RecMuonRoI( unsigned int roIWord, const ITrigT1MuonRecRoiTool* theRecRPCRoiTool,
                  const ITrigT1MuonRecRoiTool* theRecTGCRoiTool,
                  const std::vector< TrigConf::TriggerThreshold* >* theMuonConfig );

      // does exactly like the constructor
      void construct( unsigned int roIWord, const RecMuonRoiSvc* theRecRPCRoiSvc,
                      const RecMuonRoiSvc* theRecTGCRoiSvc,
                      const std::vector< TrigConf::TriggerThreshold* >* theMuonConfig );
      void construct( unsigned int roIWord, const ITrigT1MuonRecRoiTool* theRecRPCRoiTool,
                      const ITrigT1MuonRecRoiTool* theRecTGCRoiTool,
                      const std::vector< TrigConf::TriggerThreshold* >* theMuonConfig );

      /** returns roi word*/
      virtual unsigned int roiWord() const { return m_roiWord; }

      /** returns system ID ROI (Barrel=0, Endcap=1, Forward=2) */
      unsigned int sysID() const { return m_sysID; }

      /** returns sub-system ID ROI (0=-z,1=+z) */
      unsigned int subsysID() const { return m_subSysID; }

      /** returns sector ID ROI */
      unsigned int sectorID() const { return m_sectorID; }

      /** returns phi coord of ROI */
      virtual double phi() const { return m_phi; }

      /** returns eta coord of ROI */
      virtual double eta() const { return m_eta; }

      /** returns the Threshold Number (1 to 6)  associated with this RecRoI */
      unsigned int getThresholdNumber() const { return  m_thresholdNumber; }

      /** returns the Threshold Value (in GeV)  associated with this RecRoI */
      unsigned int getThresholdValue() const { return m_thresholdValue; }

      /** return RoI number in hardware numbering scheme*/
      unsigned int getRoINumber() const { return m_roiNumber; }

      /** return Overlap in hardware numbering scheme*/
      unsigned int getOverlap() const { return m_overlap; }

      /** returns true if this was the highest pt candidate in this sector*/
      bool firstCandidate() const { return m_firstCandidateFlag; }

      /** returns true if there were > 2 candidates in this sector*/
      bool sectorOverflow() const { return m_sectorOflFlag; }

      /** returns true if there were > 1 candidates in this pad*/
      bool padOverflow() const { return m_padOflFlag; }

      /** Returns the change sign of the candidate */
      ChargeSign candidateCharge() const;

      /** Returns true if the candidate was vetoed in the multiplicity sum */
      bool candidateVetoed() const;

      std::string getDebugString();

   private:
      /** a helper function to extract the value corresponding to a
          bit mask from a 32 bit unsigned int*/
      unsigned int getBitMaskValue( const unsigned int * uintValue, const unsigned int mask );

      /** this is the actual format of the data sent from
          the LVL1 hardware. See  ATL-DAQ-99-015 for
          further explanation. */
      unsigned int m_roiWord { 0 };

      /** flag indicating if this was the highest pt candidate in this
          sector - extracted from m_roiWord in the constructor */
      bool m_firstCandidateFlag { false };

      /** flag indicating if there were more than 2 candidates in this
          sector - extracted from m_roiWord in the constructor */
      bool m_sectorOflFlag { false };

      /** flag indicating if there was more than 1 candidate in this
          pad - extracted from m_roiWord in the constructor */
      bool m_padOflFlag { false };

      /** the pt threshold number associated with this RoI -
          extracted from m_roiWord in the constructor*/
      unsigned int m_thresholdNumber { 0 };

      /** The value of the threshold in GeV. Extracted from
          supplied vector of TrigConf::TriggerThresholds */
      unsigned int m_thresholdValue { 0 };

      /** the RoI number of this RoI -
          extracted from m_roiWord in the constructor*/
      unsigned int m_roiNumber { 0 };

      /** the overlap information for this RoI
          extracted from m_roiWord in the constructor*/
      unsigned int m_overlap { 0 };

      /** the system ID for this RoI (Barrel=0, Endcap=1, Forward=2)
          extracted from m_roiWord in the constructor*/
      unsigned int m_sysID { 0 };

      /** the subSystem ID for this RoI (0=-z,1=+z)
          extracted from m_roiWord in the constructor*/
      unsigned int m_subSysID { 0 };

      /** the sector number for this RoI
          extracted from m_roiWord in the constructor*/
      unsigned int m_sectorID { 0 };

      /** eta value */
      double m_eta { 0 };

      /** phi value */
      double m_phi { 0 };

   }; // class RecMuonRoI

} // namespace LVL1

/* CLID for RecMuonRoI */
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( DataVector< LVL1::RecMuonRoI >, 247859471, 1 )
CLASS_DEF( LVL1::RecMuonRoI, 6080, 0 )

#endif // TRIGT1INTERFACES_RECMUONROI_H
