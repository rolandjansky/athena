// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MioctSectorSet.h 701446 2015-10-19 15:19:09Z wengler $
#ifndef TRIGT1MUCTPI_MIOCTSECTORSET_H
#define TRIGT1MUCTPI_MIOCTSECTORSET_H

// STL include(s):
#include <vector>

// Local include(s):
#include "../Logging/MsgLogger.h"
#include "BarrelSector.h"
#include "EndcapSector.h"
#include "ForwardSector.h"
#include "MioctID.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class EventReader;

   /**
    ************************************************************************
    *
    *    $Date: 2015-10-19 17:19:09 +0200 (Mon, 19 Oct 2015) $
    *
    *    @short Helper class creating and containing the Sectors of one MIOCT module.
    *
    *           The only function of this class is to create and to group
    *           together the Sectors belonging to one MIOCT board.
    *           Exceptionally all data members of this class are public!
    *
    *      @see MioctID
    *      @see Sector
    *      @see BarrelSector
    *      @see EndcapSector
    *      @see ForwardSector
    *   @author $Author: krasznaa $
    *  @version $Revision: 701446 $
    *
    *
    ************************************************************************
    */
   class MioctSectorSet {

   public:
      /**
       * The Constructor creates a set of Sectors which belong to one
       * octant board MIOCT.
       * @param id describes the Octant board for which the Sectors
       *           have to be created.
       */
      MioctSectorSet( const MioctID& id, EventReader* reader );
      MioctSectorSet( const MioctSectorSet& set );
      MioctSectorSet  & operator = ( const MioctSectorSet  & ) = delete;
      /**
       * The four Barrel Sectors of the MIOCT module.
       */
      BarrelSector Barrel1, Barrel2, Barrel3, Barrel4;
      /**
       * The six Endcap Sectors of the MIOCT module.
       */
      EndcapSector Endcap1, Endcap2, Endcap3, Endcap4, Endcap5, Endcap6;
      /**
       * The three Forward Sectors of the MIOCT module.
       */
      ForwardSector Forward1, Forward2, Forward3;

      /**
       * method to get the number of Barrel Sectors
       */
      static unsigned int numberOfBarrelSectors() { return 4; }
      /**
       * method to get the number of Endcap Sectors
       */
      static unsigned int numberOfEndcapSectors() { return 6; }
      /**
       * method to get the number of Forward Sectors
       */
      static unsigned int numberOfForwardSectors() { return 3; }
      /**
       * This method returns a list of references to the bitfields
       * of all the sectors belonging to this set
       * @return List of references to the bitfields
       *         of all sector in this set
       */
      const std::vector< unsigned int* >* getBitFieldSet() const { return &m_bitFieldList; }

      /// Print which sectors belong to this set
      void report() const;

      /// Print the contents of the sectors
      void print() const;

   private:
      std::vector< unsigned int* > m_bitFieldList;
      mutable MsgLogger m_logger;
      const MioctID m_id;

   }; // class MioctSectorSet

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIOCTSECTORSET_H
