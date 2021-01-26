/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         RecMuonRoI.cxx  -  description
                            -------------------
   begin                : Tue Feb 25 2003
   email                : Thorsten.Wengler
***************************************************************************/


// STL include(s):
#include <iomanip>
#include <sstream>

// Trigger config includes:
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"

// Local include(s):
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"

using namespace std;
using namespace TrigConf;

namespace LVL1 {

   RecMuonRoI::RecMuonRoI( unsigned int roIWord, const RecMuonRoiSvc* theRecRPCRoiSvc,
                           const RecMuonRoiSvc* theRecTGCRoiSvc,
                           const std::vector< TriggerThreshold* >* theMuonConfig ) {

      this->construct( roIWord, theRecRPCRoiSvc, theRecTGCRoiSvc, theMuonConfig );
   }
   RecMuonRoI::RecMuonRoI( unsigned int roIWord, const ITrigT1MuonRecRoiTool* theRecRPCRoiTool,
                           const ITrigT1MuonRecRoiTool* theRecTGCRoiTool,
                           const std::vector< TriggerThreshold* >* theMuonConfig ) {

      this->construct( roIWord, theRecRPCRoiTool, theRecTGCRoiTool, theMuonConfig );
   }

   void RecMuonRoI::construct( unsigned int roIWord, const RecMuonRoiSvc* theRecRPCRoiSvc,
                               const RecMuonRoiSvc* theRecTGCRoiSvc,
                               const std::vector< TriggerThreshold* >* theMuonConfig ) {

      m_roiWord = roIWord;
      m_firstCandidateFlag = false;
      m_sectorOflFlag = false;
      m_padOflFlag = false;
      m_eta = 0;
      m_phi = 0;

      //  first extract the parts holding the sector address and
      //  and the RoI/Overlap from the 32 bit word
      unsigned int sectorAddress = this->getBitMaskValue( &m_roiWord, SectorAddressMask );
      unsigned int sectorRoIOvl = this->getBitMaskValue( &m_roiWord, RoIOvlMask );

      // the first candidate flag
      if ( this->getBitMaskValue( &m_roiWord, FirsCandMask ) == 1 ) {
         m_firstCandidateFlag = true;
      }

      // the sector overflow flag
      if ( this->getBitMaskValue( &m_roiWord, SectOflMask ) == 1 ) {
         m_sectorOflFlag = true;
      }

      // the pad overflow flag
      if ( this->getBitMaskValue( &m_roiWord, PadOflMask ) == 1 ) {
         m_padOflFlag = true;
      }

      // the pt threshold number
      m_thresholdNumber = this->getBitMaskValue( &m_roiWord, ThresNumMask );

      // the RoI and sector ID encoding is system dependent - need 3 cases

      unsigned int temp_sysID = this->getBitMaskValue( &sectorAddress, SysIDMask );
      m_sysID = 0;                             // The default is the barrel
      if( temp_sysID & 0x2 ) m_sysID = 1;      // If the higher bit is set, it's from the endcap region
      else if( temp_sysID & 0x1 ) m_sysID = 2; // if the lower bit is set, it's from the forward region

      m_subSysID = this->getBitMaskValue( &sectorAddress, SubSysIDMask );
      if ( m_sysID == 0 ) {                 // Barrel
         m_sectorID  = this->getBitMaskValue( &sectorAddress, SectNumBarrelMask );
         m_roiNumber = this->getBitMaskValue( &sectorRoIOvl, BarrelRoIMask );
         m_overlap   = this->getBitMaskValue( &sectorRoIOvl, BarrelOvlMask );
      } else if ( m_sysID == 2 ) {         // Forward
         m_sectorID = this->getBitMaskValue( &sectorAddress, SectNumForwardMask );
         m_roiNumber = this->getBitMaskValue( &sectorRoIOvl, ForwardRoIMask );
         m_overlap = 0;
      } else if ( m_sysID == 1 ) {          // Endcap
         m_sectorID = this->getBitMaskValue( &sectorAddress, SectNumEndcapMask );
         m_roiNumber = this->getBitMaskValue( &sectorRoIOvl, EndcapRoIMask );
         m_overlap = this->getBitMaskValue( &sectorRoIOvl, EndcapOvlMask );
      }

      // set RecMuonRoiSvc pointer
      const RecMuonRoiSvc* l_recMuonRoiSvc;

      if( m_sysID == 0 ) {   // RPC
         l_recMuonRoiSvc = theRecRPCRoiSvc;
      } else {               // TGC
         l_recMuonRoiSvc = theRecTGCRoiSvc;
      }

      if( l_recMuonRoiSvc ) {
         // set eta and phi values
         l_recMuonRoiSvc->reconstruct( roIWord );
         m_eta = l_recMuonRoiSvc->eta();
         m_phi = l_recMuonRoiSvc->phi();
      }

      // Get the threshold value
      // Don't bother casting TriggerThresholdValue to MuonThresholdValue  as the latter
      // currently only adds a printout function
      m_thresholdValue = 0;
      for( std::vector< TriggerThreshold* >::const_iterator it = theMuonConfig->begin();
           it != theMuonConfig->end(); ++it ) {
         if( ( *it )->type() == L1DataDef::muonType() &&
             ( *it )->thresholdNumber() + 1 == static_cast< int >( m_thresholdNumber ) ) {

            TriggerThresholdValue* ttv = ( *it )->triggerThresholdValue( 0, 0 );
            m_thresholdValue = static_cast< unsigned int >( ttv->ptcut() );
         }
      }

      return;
   }
   void RecMuonRoI::construct( unsigned int roIWord, const ITrigT1MuonRecRoiTool* theRecRPCRoiTool,
                               const ITrigT1MuonRecRoiTool* theRecTGCRoiTool,
                               const std::vector< TriggerThreshold* >* theMuonConfig ) {

      m_roiWord = roIWord;
      m_firstCandidateFlag = false;
      m_sectorOflFlag = false;
      m_padOflFlag = false;
      m_eta = 0;
      m_phi = 0;

      //  first extract the parts holding the sector address and
      //  and the RoI/Overlap from the 32 bit word
      unsigned int sectorAddress = this->getBitMaskValue( &m_roiWord, SectorAddressMask );
      unsigned int sectorRoIOvl = this->getBitMaskValue( &m_roiWord, RoIOvlMask );

      // the first candidate flag
      if ( this->getBitMaskValue( &m_roiWord, FirsCandMask ) == 1 ) {
         m_firstCandidateFlag = true;
      }

      // the sector overflow flag
      if ( this->getBitMaskValue( &m_roiWord, SectOflMask ) == 1 ) {
         m_sectorOflFlag = true;
      }

      // the pad overflow flag
      if ( this->getBitMaskValue( &m_roiWord, PadOflMask ) == 1 ) {
         m_padOflFlag = true;
      }

      // the pt threshold number
      m_thresholdNumber = this->getBitMaskValue( &m_roiWord, ThresNumMask );

      // the RoI and sector ID encoding is system dependent - need 3 cases

      unsigned int temp_sysID = this->getBitMaskValue( &sectorAddress, SysIDMask );
      m_sysID = 0;                             // The default is the barrel
      if( temp_sysID & 0x2 ) m_sysID = 1;      // If the higher bit is set, it's from the endcap region
      else if( temp_sysID & 0x1 ) m_sysID = 2; // if the lower bit is set, it's from the forward region

      m_subSysID = this->getBitMaskValue( &sectorAddress, SubSysIDMask );
      if ( m_sysID == 0 ) {                 // Barrel
         m_sectorID  = this->getBitMaskValue( &sectorAddress, SectNumBarrelMask );
         m_roiNumber = this->getBitMaskValue( &sectorRoIOvl, BarrelRoIMask );
         m_overlap   = this->getBitMaskValue( &sectorRoIOvl, BarrelOvlMask );
      } else if ( m_sysID == 2 ) {         // Forward
         m_sectorID = this->getBitMaskValue( &sectorAddress, SectNumForwardMask );
         m_roiNumber = this->getBitMaskValue( &sectorRoIOvl, ForwardRoIMask );
         m_overlap = 0;
      } else if ( m_sysID == 1 ) {          // Endcap
         m_sectorID = this->getBitMaskValue( &sectorAddress, SectNumEndcapMask );
         m_roiNumber = this->getBitMaskValue( &sectorRoIOvl, EndcapRoIMask );
         m_overlap = this->getBitMaskValue( &sectorRoIOvl, EndcapOvlMask );
      }

      if( m_sysID == 0 ) {   // RPC
        if( theRecRPCRoiTool ) {
           // set eta and phi values
           LVL1::TrigT1MuonRecRoiData roiData = theRecRPCRoiTool->roiData( roIWord );
           m_eta = roiData.eta();
           m_phi = roiData.phi();
        }
      } else {               // TGC
        if( theRecTGCRoiTool ) {
           // set eta and phi values
           LVL1::TrigT1MuonRecRoiData roiData = theRecTGCRoiTool->roiData( roIWord );
           m_eta = roiData.eta();
           m_phi = roiData.phi();
        }
      }



      // Get the threshold value
      // Don't bother casting TriggerThresholdValue to MuonThresholdValue  as the latter
      // currently only adds a printout function
      m_thresholdValue = 0;
      for( std::vector< TriggerThreshold* >::const_iterator it = theMuonConfig->begin();
           it != theMuonConfig->end(); ++it ) {
         if( ( *it )->type() == L1DataDef::muonType() &&
             ( *it )->thresholdNumber() + 1 == static_cast< int >( m_thresholdNumber ) ) {

            TriggerThresholdValue* ttv = ( *it )->triggerThresholdValue( 0, 0 );
            m_thresholdValue = static_cast< unsigned int >( ttv->ptcut() );
         }
      }

      return;
   }

   RecMuonRoI::ChargeSign RecMuonRoI::candidateCharge() const {

      // If it's a barrel muon then we don't know its sign:
      if( sysID() == 0 ) return UNDEFINED;

      if( m_roiWord & TGCCandidateSignMask ) {
         return POSITIVE;
      } else {
         return NEGATIVE;
      }
   }

   bool RecMuonRoI::candidateVetoed() const {

      return ( m_roiWord & CandidateVetoMask );
   }

   /** a helper function to extract the value corresponding to a
       bit mask from a 32 bit unsigned int*/
   unsigned int RecMuonRoI::getBitMaskValue( const unsigned int * uintValue,
                                             const unsigned int mask ) {

      unsigned int result;
      unsigned int maskcopy;
      // make a copy of the mask, because a mask is a mask and
      // one should stay a mask (i.e. should be something constant!)
      maskcopy = mask;
      result = *uintValue & mask;
      if ( mask != 0 ) {
         while ( ( maskcopy & 0x00000001 ) == 0 ) {
            maskcopy = maskcopy >> 1;
            result = result >> 1;
         }
      }
      return result;
   }

   string RecMuonRoI::getDebugString() {

      ostringstream out;
      string system;
      string hemisphere = "-";

      if ( this->sysID() == 0 ) {system = "B";}
      if ( this->sysID() == 1 ) {system = "F";}
      if ( this->sysID() > 1 ) {system = "E";}

      if ( this->subsysID() == 1 ) {hemisphere = "+";}

      out << " Addr: " << hemisphere << system << " "
          << setw( 2 ) << ios::dec << this-> sectorID()
          << " Pt/RoI: " << this->getThresholdNumber()
          << " " << setw( 2 ) << this->getRoINumber() << std::setfill( ' ' )
          << " Ovl: " << this->getOverlap()
          << " pad/secOF: " << setw( 2 ) << this->padOverflow()
          << setw( 2 ) << this->sectorOverflow()
          << " First: " << this->firstCandidate();

      //      out << ends;

      return out.str();
   }

} // namespace LVL1
