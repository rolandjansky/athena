/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuctpiSim.cxx 726107 2016-02-25 11:04:42Z wengler $

// STL include(s):
#include <cassert>
#include <iomanip>
#include <sstream>
#include <ostream>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// TrigT1 include(s):
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"

// Local include(s):
#include "MuctpiSim.h"
#include "Configuration.h"
#include "BitOp.h"
#include "../Mioct/EventReader.h"
#include "../Mioct/EventReaderFactory.h"
#include "../Mioct/SectorLogicSource.h"
#include "../Mioct/SectorLogicReader.h"
#include "MuctpiBitMasks.h"

/*******************************************************************
 * $Date: 2016-02-25 12:04:42 +0100 (Thu, 25 Feb 2016) $
 *
 * Implementation of class MuctpiSim
 * @author  $Author: krasznaa $
 * @version $Revision: 726107 $
 ******************************************************************/

namespace LVL1MUCTPI {

   // constructor of class MuctpiSim
   MuctpiSim::MuctpiSim()
      : m_logger( "MuctpiSim" ) {

      // make a dummy input object used to initialise
      m_muctpiInput = new LVL1MUONIF::Lvl1MuCTPIInput();

      // install the SectorLogic Reader as EventReader (it is the
      // default one - otherwise controlled via environment variable
      m_theReader = EventReaderFactory::getEventReader();
      if( ! m_theReader ) {
         REPORT_FATAL_MSG( "no valid Reader -> can't continue" );
         assert( 0 );
      }

      // Now take take the Lvl1MuCTPIInput object and declare it to
      // be the EventSource
      m_theSource = new SectorLogicSource();
      m_theSource->setSectorLogicSource( m_muctpiInput );

      // Now give this source to the Event Reader, so it knows where to
      // read the events from
      m_theReader->setEventSource( *m_theSource );
      REPORT_MSG(DEBUG, m_theReader->getInfoString() );
      // now create the Lvl1Mibak object, which in turn will create
      // the Octant modules and sectors, which will register themselves
      // with the Reader
      m_mibak = new Lvl1Mibak( m_theReader );
      m_mibak->setZeroSuppression( true );
      REPORT_MSG(DEBUG, m_theReader->getInfoString() );
   }

   // process method of class MuctpiSim
  void MuctpiSim::processData( const LVL1MUONIF::Lvl1MuCTPIInput* currentInput, int bcidOffset ) {

      // Print the input to the simulation:
      REPORT_VERBOSE_MSG( "Start processing event with input from sector logics:\n" );
      REPORT_VERBOSE_MSG( *currentInput );

      // set the input source to the current input object passed
      // as an argument - first get the EventSource, then change
      // what it is pointing to
      SectorLogicReader* sl_Reader = dynamic_cast< SectorLogicReader* >( m_theReader );
      if( ! sl_Reader ) {
         REPORT_FATAL_MSG( "The EventReader is apparently not a SectorLogicReader. How come?" );
         assert( 0 );
      }
      sl_Reader->getSource().setSectorLogicSource( currentInput );
      sl_Reader->getSource().setBcidOffset( bcidOffset );

      // read the event currently held in the input object
      m_theReader->putNextEvent();

      // set the MICTP data - has to be done first
      m_mibak->setMictpData();

      // do the processing in the Mirod
      m_mibak->mirodProcessData();

      return;
   }


  LVL1::MuCTPIL1Topo  MuctpiSim::getL1TopoData() { 
    
    // get the L1Topo candidates
    LVL1::MuCTPIL1Topo l1topo = m_mibak->getL1TopoCandidates(m_L1TopoConv); 
    std::vector<LVL1::MuCTPIL1TopoCandidate> l1topoCandVec = l1topo.getCandidates();

    // now get the output formatted for LVL2
    std::list<unsigned int> lvl2OutCand = getRoIBCandidates();

    // loop and match
    for (std::list< unsigned int >::iterator it_l2Cand = lvl2OutCand.begin(); 
	 it_l2Cand != lvl2OutCand.end(); ++it_l2Cand) {

	
      std::string system;
      std::string hemisphere = "-";
      unsigned int sectorNumber = 0;
      unsigned int regionOfInterest = 0;
      //offset for differnt counting scheme in MuCTPI geometry file for L1Topo
      unsigned int offset =0;
      unsigned int sectorWord = ( * it_l2Cand);
      
      // get the address values out of the bit mask
      const unsigned int sectorAddress =
	BitOp::getValue( &sectorWord, Lvl2SectorAddressMask );
      const unsigned int sectorRoIOvl = BitOp::getValue( &sectorWord, Lvl2RoIOvlMask );
      
      // get the Hemisphere
      if ( BitOp::getValue( &sectorAddress, EvReSecAddressSubSysMask ) == 1 ) {
	hemisphere = "+";
      }
      
      
      // sector address and RoI and Overlap must be taken apart agin
      // to extract the readable information
      // first find out if this is Barrel, endcap, forward, get sector
      // number, RoI and Overlap information
      if ( BitOp::getValue( &sectorAddress, EvReSecAddressSystemMask ) == 0 ) {
	system = "B"; // Barrel
	if (hemisphere == "+") {
	  offset = 32;
	}
	sectorNumber = BitOp::getValue( &sectorAddress, EvReSecNumberBarrelMask );
	regionOfInterest = BitOp::getValue( &sectorRoIOvl, Lvl2BarrelRoIMask );
      } else if ( BitOp::getValue( &sectorAddress, EvReSecAddressSystemMask ) == 1 ) {
	if (hemisphere == "+") {
	  system = "FA";
	} else  {
	  system = "FC";
	}
	sectorNumber = BitOp::getValue( &sectorAddress, EvReSecNumberForwardMask );
	regionOfInterest = BitOp::getValue( &sectorRoIOvl, Lvl2ForwardRoIMask );
      } else if ( BitOp::getValue( &sectorAddress, EvReSecAddressSystemMask ) > 1 ) {
	if (hemisphere == "+") {
	  system = "EA";
	} else {
	  system = "EC";
	}
	sectorNumber = BitOp::getValue( &sectorAddress, EvReSecNumberEndcapMask );
	regionOfInterest = BitOp::getValue( &sectorRoIOvl, Lvl2EndcapRoIMask );
      }
      
      std::string secNumString = "";
      if (sectorNumber+offset < 10) {
	secNumString = "0" + std::to_string(sectorNumber + offset);
      } else {
	secNumString = std::to_string(sectorNumber + offset);
      }
      
      system = system + secNumString ;
      
      for (std::vector<LVL1::MuCTPIL1TopoCandidate>::iterator it_topoCand =  l1topoCandVec.begin(); 
	   it_topoCand != l1topoCandVec.end(); ++it_topoCand) {      
	
	
	//	std::cout << "TW:" << system << " | " << it_topoCand->getSectorName() << " | " << regionOfInterest << " | " << it_topoCand->getRoiID() << std::endl;
	
	if (system == it_topoCand->getSectorName() && regionOfInterest == it_topoCand->getRoiID() ) {
	  // match has been made, fill the roiWord into the candidate object
	  it_topoCand->setRoiWord(sectorWord);
	  // std::cout << "TW match:  Addr: " << system << " : " << hemisphere << system << " "
	  // 	    << std::setw( 2 ) << std::dec << sectorNumber
	  // 	    << " Pt/RoI: " << ptValue
	  // 	    << " " << std::setw( 2 ) << regionOfInterest
	  // 	    << " Ovl: " << overlap
	  // 	    << " pad/secOF: " << std::setw( 2 ) << padOverflow
	  // 	    << std::setw( 2 ) << sectorOverflow
	  // 	    << " First: " << candFirst
	  // 	    << " veto: " << veto << std::endl;
	  
	  // it_topoCand->print();
	  break;
	}
	
      }
    }

    l1topo.setCandidates(l1topoCandVec);
    return l1topo; 
    
}


   /**
    * The call is just forwarded to the MIBAK object, even though that's not the
    * hardware that implements this logic in real life...
    *
    * @returns <code>true</code> if any of the octant boards saw a barrel candidate,
    *          <code>false</code> otherwise
    */
   bool MuctpiSim::hasBarrelCandidate() const {

      return m_mibak->hasBarrelCandidate();
   }

   /**
    * The call is just forwarded to the MIBAK object, even though that's not the
    * hardware that implements this logic in real life...
    *
    * @returns <code>true</code> if any of the octant boards saw an endcap candidate,
    *          <code>false</code> otherwise
    */
   bool MuctpiSim::hasEndcapCandidate() const {

      return m_mibak->hasEndcapCandidate();
   }

   // print method for the CTP output word
   void MuctpiSim::printCTPOut( std::string format ) {

      unsigned int CTPword = this->getCTPData();
      std::ostringstream message;

      if( format == "hex" ) {
         message << std::hex << std::setw( 8 ) << CTPword;
      } else if ( format == "bin" ) {
         std::string ctpWordString;
         BitOp::printBin( CTPword, ctpWordString );
         message << ctpWordString;
      } else if ( format == "human" ) {
         message << " =============================== CTP word ========="
                 << "=========================" << std::endl;
         message << "   PtSum6: " << BitOp::getValue( &CTPword, MictpSum6Mask )
                 << "   PtSum5: " << BitOp::getValue( &CTPword, MictpSum5Mask )
                 << "   PtSum4: " << BitOp::getValue( &CTPword, MictpSum4Mask )
                 << "   PtSum3: " << BitOp::getValue( &CTPword, MictpSum3Mask )
                 << "   PtSum2: " << BitOp::getValue( &CTPword, MictpSum2Mask )
                 << "   PtSum1: " << BitOp::getValue( &CTPword, MictpSum1Mask )
                 << std::endl;
         message << " =================================================="
                 << "=========================";
      }

      REPORT_MSG(DEBUG, message.str() );

      return;
   }

   // print method for the Lvl2 Output list
   void MuctpiSim::printLvl2Out( std::string format ) {

      MirodLvl2Processor serviceLvl2( this->getRoIBData() );
      serviceLvl2.printEvent( format );
      return;
   }

   // print method for the DAQ output list
   void MuctpiSim::printDAQOut( std::string format ) {

      MirodDaqProcessor serviceDaq( this->getDAQData() );
      serviceDaq.printEvent( format );
      return;
   }

   //get the content of the MirodFiFos as an object
   MirodFiFoContent MuctpiSim::getMirodFiFoContent() {

      MirodFiFoContent mirodFiFos;

      mirodFiFos.monFiFo( *( this->getMirodModule()->getMirodExtractor()->getExtractorData() ) );
      mirodFiFos.lvl2FiFo( this->getRoIBData() );
      mirodFiFos.daqFiFo( this->getDAQData() );

      return mirodFiFos;
   }

   // set Configuration
   void MuctpiSim::setConfiguration ( const Configuration &conf ) {

      // set MIROD via MIBAK
      m_mibak->setConfiguration( conf );
      return;
   }

   StatusCode MuctpiSim::initializeLUTOverlapStrategy( const std::string& lutXMLFile,
                                                       bool flagMode, bool dumpLut,
						       const std::string& runPeriod ) {

      CHECK( m_mibak->initializeLUTOverlapStrategy( lutXMLFile, flagMode,
                                                    dumpLut, runPeriod ) );
      return StatusCode::SUCCESS;
   }

   //destructor of class MuctpiSim
   MuctpiSim::~MuctpiSim() {

      // the order of destruction matters, as the sectors destructing
      // themselves as part of the Lvl1Mibak destructor contact the
      // EventReader to unregister themselves first - for this the
      // EventReader must still be present
      delete m_mibak;
      delete m_theSource;
      delete m_theReader;
      delete m_muctpiInput;
   }

} // namespace LVL1MUCTPI
