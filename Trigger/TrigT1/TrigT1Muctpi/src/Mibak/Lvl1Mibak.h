// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Lvl1Mibak.h 701446 2015-10-19 15:19:09Z wengler $
#ifndef TRIGT1MUCTPI_LVL1MIBAK_H
#define TRIGT1MUCTPI_LVL1MIBAK_H

// STL include(s):
#include <list>
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"

// Local include(s):
#include "../Common/SectorConstants.h"
#include "../Common/MultiplicityCalcType.h"
#include "../Logging/MsgLogger.h"
#include "../Common/EventID.h"
#include "../Common/MioctL1TopoConverter.h"
#include "MibakStreamEvent.h"
#include "../Mioct/StrategyName.h"

// include  interface class to L1Topo
#include "TrigT1Interfaces/MuCTPIL1Topo.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class Configuration;
   class MioctModule;
   class MioctID;
   class PtMultiplicitySet;
   class MictpModule;
   class MirodModule;
   class OverlapLogic;
   class MultiplicityLogic;
   class EventReader;

   /**
    *  @short This class represents the MIBAK backplane of the MUCTPI Interface.
    *
    *         This class  is  a model of the    Lvl1 part of   the MIBAK
    *         backplane.  It  contains  the 16 MIOCT modules  which have
    *         corresponding    representations   in      software    and
    *         hardware. Differently  from the hardware implementation it
    *         is the Lvl1Mibak  which contains the  OverlapLogic used by
    *         all MioctBoards  in  order to resolve the  overlap between
    *         neighbouring  sectors. This choice  has  been made because
    *         there is no need to instantiate an overlap Logic for every
    *         octant board since all octant boards use the same strategy
    *         to  resolve   overlapping   muon    candidates.  Different
    *         Strategies can be   chosen   via a member   function.  The
    *         possible strategies are listed in StrategyName.h
    *
    *    @see MioctModule
    *    @see OverlapLogic
    *    @see OverlapStrategy
    *    @see StrategyName.h
    *    @see PtMultiplicitySet
    *
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 701446 $
    * $Date: 2015-10-19 17:19:09 +0200 (Mon, 19 Oct 2015) $
    */
   class Lvl1Mibak {

   public:
      /**
       * In the constructor the NullStrategy (no consideration of Overlap)
       * will be installed as a default for the OverlapLogic. The user has
       * to change the Strategy via a call to setOverlapStrategy() if he 
       * wants differnt strategies to be applied.
       * @see OverlapLogic
       * @see OverlapStrategy
       * @see NullStrategy
       * @see StrategyName.h
       */
      Lvl1Mibak( EventReader* reader );

      // disable copy/assignment operator as not used/implemented
      Lvl1Mibak( const Lvl1Mibak & ) = delete;
      Lvl1Mibak & operator = ( const Lvl1Mibak & ) = delete;

      ~Lvl1Mibak();

      /// set Configuration
      void setConfiguration( const Configuration &conf );

      /**
       * Read back a point to MioctModule specified by MioctID in the
       * Argument
       * @see MioctModule
       * @see MioctID
       */
      MioctModule* getMioctModule( MioctID id ) const;
      /**
       * This function determines the Pt multiplicity of the muon candidates
       * for each pt-threshold independently. Like in the hardware the
       * multiplicities satureate at a count of 7.
       * @return PtMultiplicitySet The pt-multiplicities for six different
       * pt-thresholds calculated over all muon trigger chambers.
       * @see PtMultiplicitySet
       * @see OverlapLogic
       * @see OverlapStrategy
       */
      PtMultiplicitySet  getPtMultiplicity() const;
      /**
       * This method fills the information obtained from the MIOCT Modules into
       * the MICTP
       */
      void setMictpData();
      /**
       * Get access to the MictpModule
       * @return Pointer to the MictpModule
       */
      MictpModule* getMictp() { return m_mictpModule; }
      /**
       * Get access to the MirodModule
       * @return Pointer to the MirodModule
       */
      MirodModule* getMirod() { return m_mirodModule; }
      /**
       * This method fills the information obtained from the MIOCT
       * the MICTP boards into the MIROD
       */
      void mirodProcessData();
      /**
       * This function return a pointer to an object allowing access to
       * the data of this event in Mibak format.
       * @return Pointer to MibakData object allowing access
       *         to the data of this event in Mibak format
       */
      const MibakStreamEvent* getMibakData();
      /**
       * This method just returns the pointer to the MibakStreamEvent, without
       * doing any processing
       * @return Pointer to MibakStreamEvent
       */
      const MibakStreamEvent* getMibakStreamEvent() { return &m_mibakStream; }

      LVL1::MuCTPIL1Topo getL1TopoCandidates(MioctL1TopoConverter & l1TopoConv ) const;

      /**
       * This function changes the current OverlapStrategy for the
       * OverlapLogic of the Lvl1Mibak.
       * @param newStrategy As listed in StrategyName.h
       * @see OverlapLogic
       * @see OverlapStrategy
       * @see StrategyName.h
       */
      void setOverlapStrategy( StrategyName newStrategy );
      /**
       * Return the name of the currently selected OverlapStrategy in
       * the Overlap Logic.
       * @see OverlapLogic
       * @see OverlapStrategy
       */
      std::string getOverlapStrategyName() const;
      /**
       * Initialize the LUT strategy from the given file.
       * @see LUTStrategy
       * @see LUTStructure
       */
      StatusCode initializeLUTOverlapStrategy( const std::string& lutXMLFile,
                                               bool flagMode, bool dumpLut,
					       const std::string& runPeriod );

      void setMultiplicityStrategy( MultiplicityCalcType type );
      void initializeXMLMultStrategy( const std::string& xmlFile );

      bool getZeroSuppression() const { return m_doZeroSuppression; }
      void setZeroSuppression( bool value ) { m_doZeroSuppression = value; }

      /// Is there at least one barrel candidate in the event?
      bool hasBarrelCandidate() const;
      /// Is there at least one endcap candidate in the event?
      bool hasEndcapCandidate() const;

   private:
      mutable MsgLogger         m_logger;
      std::list<MioctModule*>   m_lvl1MioctList;
      MictpModule*              m_mictpModule;
      MirodModule*              m_mirodModule;
      OverlapLogic*             m_lvl1OverlapLogic;
      MultiplicityLogic*        m_multiplicityLogic;
      EventID                   m_eventID;
      MibakStreamEvent          m_mibakStream;
      bool                      m_doZeroSuppression;
      EventReader*              m_eventReader;

   }; // class Lvl1Mibak

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_LVL1MIBAK_H
