// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MioctModule.h 678659 2015-06-26 14:54:31Z wengler $
#ifndef TRIGT1MUCTPI_MIOCTMODULE_H
#define TRIGT1MUCTPI_MIOCTMODULE_H

// Local include(s):
#include "../Common/PtMultiplicitySet.h"
#include "MioctID.h"
#include "MioctData.h"
#include "MioctSectorSet.h"
#include "../Logging/MsgLogger.h"
#include "../Common/MioctL1TopoConverter.h"

// include  interface class to L1Topo
#include "TrigT1Interfaces/MuCTPIL1Topo.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class OverlapLogic;
   class EventReader;

   /**
    *  @short Contains all relevant entities of an octant board MIOCT.
    *
    *         The MioctModule groups  together all entities relevant for
    *         an onctant board MIOCT. It's  only service is to calculate
    *         the Pt-Multiplicity for the data which is currently stored
    *         in the Sectors of the MioctModule.
    *
    *    @see MiocID
    *    @see OverlapLogic
    *    @see OverlapStrategy
    *    @see PtMultiplicitySet
    *    @see Detector.h
    *
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 678659 $
    * $Date: 2015-06-26 16:54:31 +0200 (Fri, 26 Jun 2015) $
    */
   class MioctModule {

   public:
      /**
       * The constructor instantiates all entities relevant for an
       * octant board. There is one major difference to the real
       * world: There is only one Instantiation of the OverlapLogic
       * in the Lvl1Mibak class. The octant boards only contain a
       * pointer to that logic. This has been chosen because it does
       * not make sense to simulate the system with different overlap
       * logics implemented in different octant boards at the same
       * time. There is of course still the possibility to use different
       * strategies to resolve the overlap in the OverlapLogic.
       * @param theID Identfieer of the Mioctodule
       * @param theOverlapLogic Pointer to the OverlapLogic which is used
       *                        to resolve overlapping muon candidates during forming the
       *                        Pt-multiplicity over the octant board.
       * @param reader The event reader isntance for this simulation
       * @see MioctID
       * @see OverlapLogic
       * @see OverlapStrategy
       */
      MioctModule( const MioctID& theID, const OverlapLogic* theOverlapLogic,
                   EventReader* reader );
      /**
       * Return the MioctSectorSet object of this MioctModule
       * @return The MioctSectorSet object of this octant board.
       * @see MioctSectorSet
       */
      const MioctSectorSet* getMioctSectorSet() const { return &m_mioctSectors; }
      /**
       * Return the MioctID object of this MioctModule
       * @return The MioctID object of this octant board.
       * @see MioctID
       */
      MioctID getMioctID() const { return m_ID; }
      /**
       * This service determines the actual pt-multiplicity of the
       * data contained in the Sectors of the MioctModule. It uses the
       * OverlapLogic given to the constructor to resolve the overlap
       * between neighbouring sectors.
       * @return For the six different Pt-thresholds
       *         the multiplicity of muon candidates is returned.
       * @see PtMultiplicitySet
       */
      PtMultiplicitySet calculateMultiplicity() const;
      /**
       * Return the L1Topo outputs for this MIOCT 
       * This must be called after calculateMultiplicity() to make sure the
       * flags are properly set in sector data
       */
      LVL1::MuCTPIL1Topo getL1TopoCandidates(MioctL1TopoConverter & l1TopoConv) const;
      /**
       * This method returns a pointer to the SectorSet conatained
       * in this MIOCT
       * @return Pointer to the set of Sectors belonging to this MIOCT
       */
      const MioctSectorSet* getSectorSet() const { return &m_mioctSectors; }
      /**
       * This service groups the data stored in this MIOCT into an object
       * which allows easy access to the data in the format found on the
       * MIBAK backplane.
       * @return data of this MIOCT module with access
       *         function to retrive the data in MIBAK format
       */
      MioctData* getMioctOnMibak();

      /// Is there at least one barrel candidate in the octant?
      bool hasBarrelCandidate() const;
      /// Is there at least one endcap candidate in the octant?
      bool hasEndcapCandidate() const;

   private:
      mutable MsgLogger   m_logger;
      MioctID             m_ID;
      MioctSectorSet      m_mioctSectors;
      const OverlapLogic* m_lvl1OverlapLogic;
      MioctData           m_mioctData;

   }; // class MioctModule

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIOCTMODULE_H
