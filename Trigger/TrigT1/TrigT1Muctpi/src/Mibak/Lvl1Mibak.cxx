/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Lvl1Mibak.cxx 700318 2015-10-13 14:13:15Z wengler $

// STL include(s):
#include <cassert>
#include <sstream>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "Lvl1Mibak.h"
#include "../Mioct/MioctModule.h"
#include "../Mictp/MictpModule.h"
#include "../Mirod/MirodModule.h"
#include "../Common/Configuration.h"
#include "../Common/PtMultiplicitySet.h"
#include "../Mioct/EventReader.h"
#include "../Mioct/EventReaderFactory.h"
#include "../Mioct/OverlapLogic.h"
#include "../Mioct/MultiplicityLogic.h"


namespace LVL1MUCTPI {

   Lvl1Mibak::Lvl1Mibak( EventReader* reader )
      : m_logger( "Lvl1Mibak" ), m_lvl1MioctList(), m_lvl1OverlapLogic( 0 ),
        m_multiplicityLogic( 0 ), m_eventReader( reader ) {

      // Create the "logic objects". Note that the overlap logic uses the multiplicity
      // logic itself.
      m_multiplicityLogic = new MultiplicityLogic();
      m_lvl1OverlapLogic = new OverlapLogic( m_multiplicityLogic );

      // create a list of 16 MIOCT modules
      MioctModule* newMioct;
      REPORT_VERBOSE_MSG( "Constructor called" );

      for( unsigned int id = 0; id <= 7; ++id ) {
         // id mioct module for the negative rapidiy region...
         MioctID newID( id, Hemisphere( NEGATIVE ) );
         newMioct = new MioctModule( newID, m_lvl1OverlapLogic, m_eventReader );
         m_lvl1MioctList.push_back( newMioct );
         REPORT_VERBOSE_MSG( "MIOCT module with id " << id
                             << " in the NEGATIVE rapidity region created" );
      }
      for( unsigned int id = 0; id <= 7; ++id ) {
         // one mioct module for the positive rapidiy region...
         MioctID newID( id, Hemisphere( POSITIVE ) );
         newMioct = new MioctModule( newID, m_lvl1OverlapLogic, m_eventReader );
         m_lvl1MioctList.push_back( newMioct );
         REPORT_VERBOSE_MSG( "MIOCT module with id " << id
                             << " in the POSITIVE rapidity region created" );
      }

      // create the MICTP Module
      m_mictpModule = new MictpModule();
      REPORT_VERBOSE_MSG( "MICTP module created" );

      // create the MIROD Module
      m_mirodModule = new MirodModule();
      REPORT_VERBOSE_MSG( "MIROD module created" );

      // check if a valid Event reader is installed, If not, exit
      // with a fatal error here.
      if( ! m_eventReader->isValid() ) {
         REPORT_FATAL_MSG( "No valid EventReader installed!" );
         REPORT_FATAL_MSG( "The reason is : " << reader->getInfoString() );
         assert( 0 );
      }

      //initialise the zero suppression flag to true, which is the more common - will be set from the configuration
      m_doZeroSuppression = true;

   }

   Lvl1Mibak::~Lvl1Mibak() {

      REPORT_VERBOSE_MSG( "Destructor called" );
      std::list< MioctModule* >::iterator it;
      for( it = m_lvl1MioctList.begin(); it != m_lvl1MioctList.end(); it++ ) {
         delete *it;
      }
      delete m_mictpModule;
      delete m_mirodModule;
      delete m_lvl1OverlapLogic;
      delete m_multiplicityLogic;
   }

   MioctModule* Lvl1Mibak::getMioctModule( MioctID findId ) const {

      std::list< MioctModule* >::const_iterator it;
      for( it = m_lvl1MioctList.begin(); it != m_lvl1MioctList.end(); ++it ) {
         MioctID mioctID = (*it)->getMioctID();
         if( mioctID.getNumber() == findId.getNumber() &&
             mioctID.getRapidityRegion() == findId.getRapidityRegion() ) {
            return *it;
         }
      }

      m_logger << WARNING << "MioctID " << findId << " not known!"
               << MsgLogger::endmsg;
      return 0;
   }

   PtMultiplicitySet Lvl1Mibak::getPtMultiplicity() const {

      std::string preliminaryResult;
      std::list< MioctModule* >::const_iterator it;
      PtMultiplicitySet result;
      REPORT_VERBOSE_MSG( "getPtMultiplicity() called" );

      for( it = m_lvl1MioctList.begin(); it != m_lvl1MioctList.end(); ++it ) {
         result += ( ( *it )->calculateMultiplicity() );
         result.print( preliminaryResult );
         REPORT_VERBOSE_MSG( "Preliminary result : \n" << preliminaryResult );
      }

      return result;
   }

   void Lvl1Mibak::setMictpData() {

      // get access to the EventID singleton
      EventID eventID;
      // get the BCID and ECRC from the EventID singleton
      unsigned int BCID = eventID.BCID();
      unsigned int ECRC = eventID.getECRC();
      // get the PtSums
      PtMultiplicitySet ptSet = this->getPtMultiplicity();

      // fill the summed Pt information and the BCID into the MICTP
      m_mictpModule->setPtSums( &ptSet, BCID, ECRC );

      return;
   }

   LVL1::MuCTPIL1Topo Lvl1Mibak::getL1TopoCandidates(MioctL1TopoConverter & l1TopoConv) const {

      std::list< MioctModule* >::const_iterator it;
      LVL1::MuCTPIL1Topo result;
      REPORT_VERBOSE_MSG( "getL1TopoCandidates() called" );

      for( it = m_lvl1MioctList.begin(); it != m_lvl1MioctList.end(); ++it ) {
	result += ( ( *it )->getL1TopoCandidates(l1TopoConv) );
      }

      return result;
   }



   void Lvl1Mibak::mirodProcessData() {

      const MibakStreamEvent* theStream = this->getMibakData();
      m_mirodModule->processData( theStream );

      return;
   }


   const MibakStreamEvent* Lvl1Mibak::getMibakData() {

      // make sure the cointainers of the StreamEvent are empty --------
      m_mibakStream.highBits.clear();
      m_mibakStream.dataWords.clear();

      // fill in the MICTP data
      const MictpData* mictpData = m_mictpModule->getMictpOnMibak();
      const std::list<unsigned int>* mictpHighBits  = mictpData->getHighBits();
      const std::list<unsigned int>* mictpDataWords = mictpData->getDataWords();
      std::list<unsigned int>::const_iterator it_mictpHighBits  = mictpHighBits->begin();
      std::list<unsigned int>::const_iterator it_mictpDataWords = mictpDataWords->begin();
      for( ; it_mictpHighBits != mictpHighBits->end(); ++it_mictpHighBits ) {

         m_mibakStream.highBits.push_back( *it_mictpHighBits );
         m_mibakStream.dataWords.push_back( *it_mictpDataWords );

         ++it_mictpDataWords; // increase also the data word iterator
      }

      // Loop over the MIOCT Modules ------------------------------------
      std::list<MioctModule*>::const_iterator it_mioctModule;
      for( it_mioctModule = m_lvl1MioctList.begin();
           it_mioctModule != m_lvl1MioctList.end(); ++it_mioctModule ) {

         // get pointer to the data of this MIOCT
         MioctData* mioctData = ( (*it_mioctModule)->getMioctOnMibak() );

         // the Header of this MIOCT
         m_mibakStream.highBits.push_back( mioctData->getStartMarker() );
         m_mibakStream.dataWords.push_back( mioctData->getHeader() );

         // loop over the sectors of this MIOCT and do zero
         // suppression if requested
         const std::vector< unsigned int >* mioctSectorWords = mioctData->getSectorWords();
         const std::vector< unsigned int >* mioctSectorHighBits =
            mioctData->getSectorHighBits();
         if( mioctSectorWords->size() > SECTORS_IN_MIOCT ) {
            REPORT_FATAL_MSG( "Too many sectors transmitted by MIOCT!!!" );
            assert( 0 );
         }
         if( mioctSectorWords->size() != mioctSectorHighBits->size() ) {
            REPORT_FATAL_MSG( "Size of vectors in MioctData differ!!!" );
            assert( 0 );
         }
         std::vector< unsigned int >::const_iterator it_sectorWords =
            mioctSectorWords->begin();
         std::vector< unsigned int >::const_iterator it_sectorHighBits =
            mioctSectorHighBits->begin();
         unsigned int secCount = 0; // sector counter reset
         for( ; it_sectorWords != mioctSectorWords->end(); ++it_sectorWords ) {
            // The bit sequence 111 signalls that there is no candidate.
            // This variable is ony true, if at least one candidate's pt
            // value is different from 111
            bool notEmpty = ( ( *it_sectorWords ) & Pt1and2Mask ) ^ Pt1and2Mask;

            if ( notEmpty || !m_doZeroSuppression ) {
               ++secCount;
               m_mibakStream.highBits.push_back( *it_sectorHighBits );
               m_mibakStream.dataWords.push_back( *it_sectorWords );
            }

            ++it_sectorHighBits;
         } // end of sector loop

         // the Trailer of this MIOCT
         m_mibakStream.highBits.push_back( mioctData->getEndMarker() );
         // First fill the word count into the trailer word if zero
         // suppression was applied, than get the trailer pointer
         if ( m_doZeroSuppression ) {
            mioctData->setTrailerWordCount( secCount + 2 );
         }
         m_mibakStream.dataWords.push_back( mioctData->getTrailer() );

      } // end of MIOCT loop -------------------------------------------

      return &m_mibakStream;
   }

   void Lvl1Mibak::setOverlapStrategy( StrategyName newStrategy ) {

      REPORT_VERBOSE_MSG( "setOverlapStrategy() called" );
      m_lvl1OverlapLogic->changeStrategy( newStrategy );
      return;
   }

   std::string Lvl1Mibak::getOverlapStrategyName() const {

      REPORT_VERBOSE_MSG( "getOverlapStrategy() called" );
      return m_lvl1OverlapLogic->getCurrentStrategyName();
   }

   void Lvl1Mibak::setConfiguration( const Configuration &conf ) {

      // Zero Suppression Flag
      m_doZeroSuppression = conf.getDoZeroSuppression();

      // configure MIROD
      m_mirodModule->setConfiguration(conf);
      return;
   }

   StatusCode Lvl1Mibak::initializeLUTOverlapStrategy( const std::string& lutXMLFile,
                                                       bool flagMode, bool dumpLut,
						       const std::string& runPeriod ) {

     CHECK( m_lvl1OverlapLogic->initializeLUT( lutXMLFile, flagMode, dumpLut, runPeriod ) );
      return StatusCode::SUCCESS;
   }

   void Lvl1Mibak::setMultiplicityStrategy( MultiplicityCalcType type ) {

      REPORT_VERBOSE_MSG( "setMultiplicityStrategy() called" );
      m_multiplicityLogic->changeStrategy( type );
      return;
   }

   void Lvl1Mibak::initializeXMLMultStrategy( const std::string& xmlFile ) {

      m_multiplicityLogic->initializeXML( xmlFile );
      return;
   }

   /**
    * This function should actually not be here. Teh logic implemented here is
    * not happening in the MIBAK in the real hardware, but in a simple NIM logic.
    * However programatically it was much easier to put this code here...
    *
    * The function just checks if any of the octant boards received a barrel
    * candidate.
    *
    * @returns <code>true</code> if any of the octant boards saw a barrel candidate,
    *          <code>false</code> otherwise
    */
   bool Lvl1Mibak::hasBarrelCandidate() const {

      // Return true if any of the boards saw such a candidate:
      std::list< MioctModule* >::const_iterator itr = m_lvl1MioctList.begin();
      std::list< MioctModule* >::const_iterator end = m_lvl1MioctList.end();
      for( ; itr != end; ++itr ) {
         if( ( *itr )->hasBarrelCandidate() ) {
            return true;
         }
      }

      // If no board saw such a candidate, return false:
      return false;
   }

   /**
    * This function should actually not be here. Teh logic implemented here is
    * not happening in the MIBAK in the real hardware, but in a simple NIM logic.
    * However programatically it was much easier to put this code here...
    *
    * The function just checks if any of the octant boards received an endcap
    * candidate.
    *
    * @returns <code>true</code> if any of the octant boards saw an endcap candidate,
    *          <code>false</code> otherwise
    */
   bool Lvl1Mibak::hasEndcapCandidate() const {

      // Return true if any of the boards saw such a candidate:
      std::list< MioctModule* >::const_iterator itr = m_lvl1MioctList.begin();
      std::list< MioctModule* >::const_iterator end = m_lvl1MioctList.end();
      for( ; itr != end; ++itr ) {
         if( ( *itr )->hasEndcapCandidate() ) {
            return true;
         }
      }

      // If no board saw such a candidate, return false:
      return false;
   }

} // namespace LVL1MUCTPI
