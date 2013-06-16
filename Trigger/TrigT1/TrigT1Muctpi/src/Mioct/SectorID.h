// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SectorID.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_SECTORID_H
#define TRIGT1MUCTPI_SECTORID_H

// STL include(s):
#include <iostream>

// Local include(s):
#include "../Common/Detector.h"

namespace LVL1MUCTPI {

   //
   // Declare a comparison operator. This is needed to be able to index
   // an std::map with SectorID objects...
   //
   class SectorID;
   bool operator< ( const SectorID& id1, const SectorID& id2 );

   /**********************************************************************
    *
    *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $ 
    *
    *    @short The Identifier of a Sector.
    *
    *           The SectorID contains all data to unambigously  identify 
    *           a sector.  This data  contains the  detector  Type,  the 
    *           sector  number, and the  rapidity region. Several access
    *           functions allow to  read back these  data. SectorIDs can  
    *           be set  only in  their constructor. Two constructors are   
    *           available.  The  first   takes   values  as  defined  in   
    *           Detector.h, the secondtakes  numerical  values  as  used  
    *           in the  Lvl1MuCTPIInput class.  It  is not  possible  to   
    *           change  the values after construction.
    *
    *      @see Detector.h
    *      @see SectorConstants.h
    *      @see Lvl1MuCTPIInput
    *   @author $Author: krasznaa $
    *  @version $Revision: 362102 $
    *
    ********************************************************************
    */
   class SectorID {

      // Declare the comparison operator as a friend of the class:
      friend bool operator< ( const SectorID&, const SectorID& );

   public: 
      /**
       * The default constructor - no values are set - they are expected
       * to be set by the set method instead
       * @see SectorID::set()
       */
      SectorID();
      /**
       * This constructor should only be used by the XML multiplicity calculator.
       * The sectors are identified by a single integer number in the XML files
       * with the following encoding:
       *
       *   0 --  31 :  BARREL sectors, NEGATIVE side
       *  32 --  79 :  ENDCAP sectors, NEGAVIVE side
       *  80 -- 103 : FORWARD sectors, NEGATIVE side
       * 104 -- 135 :  BARREL sectors, POSITIVE side
       * 136 -- 183 :  ENDCAP sectors, POSITIVE side
       * 184 -- 207 : FORWARD sectors, POSITIVE side
       */
      SectorID( int code );
      /**
       * This constructor takes the information to initialize the 
       * SectorID from three different arguments
       * @param theDetector as defined in Detector.h
       * @param theRegion   as defined in Detector.h
       * @param theNumber   the sector number. Valid ranges depend
       *                    on the Detector Type and are defined in SectorConstants.h
       */
      SectorID( Detector theDetector, Hemisphere theRegion, unsigned int theNumber );
      /**
       * This constructor takes the information to initialize the 
       * SectorID from three different numerical arguments
       * @param theDetector  system ID as used in Lvl1MuCTPIInput
       * @param theRegion    subsystem ID as used in Lvl1MuCTPIInput
       * @param theNumber    is the sector number. Valid ranges depend
       *                     on the Detector Type and are defined in SectorConstants.h
       * @see Lvl1MuCTPIInput
       */
      SectorID( unsigned int theDetector, unsigned int theRegion,
                unsigned int theNumber );
      /**
       * This method takes the information to initialize the 
       * SectorID from three different numerical arguments
       * and sets the data members
       * @param theDetector  system ID as used in Lvl1MuCTPIInput
       * @param theRegion    subsystem ID as used in Lvl1MuCTPIInput
       * @param theNumber    is the sector number. Valid ranges depend
       *                     on the Detector Type and are defined in SectorConstants.h
       * @see Lvl1MuCTPIInput
       */
      void setVal( unsigned int theDetector, unsigned int theRegion,
                   unsigned int theNumber );
      /**
       * This method takes the information to initialize the 
       * SectorID from three different numerical arguments
       * and returns this object by value, e.g. for use as an
       * argument in fuction calls
       * @param theDetector  system ID as used in Lvl1MuCTPIInput
       * @param theRegion    subsystem ID as used in Lvl1MuCTPIInput
       * @param theNumber    is the sector number. Valid ranges depend
       *                     on the Detector Type and are defined in SectorConstants.h
       * @return this SectorID object by value
       * @see Lvl1MuCTPIInput
       */
      SectorID ret( unsigned int theDetector, unsigned int theRegion,
                    unsigned int theNumber );
      /**
       * This method returns the Detector type as a number, as used by
       * the Lvl1MuCTPIInput class
       * @return the detector type
       * @see Lvl1MuCTPIInput
       */
      unsigned int getDetectorType() const;
      /**
       * This method returns the Rapidity region as a number, as used by 
       * the Lvl1MuCTPIInput class
       * @return the rapiduty region
       * @see Lvl1MuCTPIInput
       */
      unsigned int getRapidityRegion() const;
      /**
       * This method returns the Sector number
       * @return the sector number
       */
      unsigned int getSectorNumber() const { return m_sectorNumber; }
      /**
       * Provide a comparison operator
       */
      bool operator== ( const SectorID& ) const;
      /**
       * Provide a assignment operator
       */
      SectorID& operator= ( const SectorID& );
      /**
       * This operator is used when the sector ID has to be stored as a single
       * integer number. (When reading the multiplicity handling from an XML
       * file.) Compare this with the constructor accepting a single integer.
       */
      operator int() const;
      /**
       * overload the << operator to have easy printout
       */
      friend std::ostream& operator<< ( std::ostream&, const SectorID& );
      /**
       * The destructor of class SectorID - nothing to destruct
       */
      ~SectorID();

   private:
      Detector      m_detectorType;
      Hemisphere    m_rapidityRegion;
      unsigned int  m_sectorNumber;

   }; // class SectorID

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_SECTORID_H
