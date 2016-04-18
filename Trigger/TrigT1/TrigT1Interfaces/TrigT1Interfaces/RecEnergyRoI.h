// Dear emacs, this is -*- c++ -*-
// $Id: RecEnergyRoI.h 707787 2015-11-13 15:09:29Z fwinkl $
/***************************************************************************
                         RecEnergyRoI.h  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   copyright            : (C) 2001 by moyse
   email                : moyse@heppch.ph.qmw.ac.uk
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TRIGT1INTERFACES_RECENERGYROI_H
#define TRIGT1INTERFACES_RECENERGYROI_H

// STL include(s):
#include <map>

// Gaudi/Athena include(s):
#include "DataModel/DataVector.h"

// Local include(s):
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

// Forward declaration(s):
namespace TrigConf {
   class TriggerThreshold;
}

namespace LVL1 {

   /** This class defines the reconstructed EnergySum ROI. It is generated from the
       Slink output of TrigT1Calo

       @author Edward Moyse
   */
   class RecEnergyRoI {

   public:
      // constructor
      RecEnergyRoI() {}
      // constructor
      RecEnergyRoI( unsigned int RoIWord0, unsigned int RoIWord1, unsigned int RoIWord2,
                    const std::vector< TrigConf::TriggerThreshold* >* caloThresholds );
      // destructor
      ~RecEnergyRoI();

      //  does excatly what the constructor should do
      void construct( unsigned int RoIWord0, unsigned int RoIWord1, unsigned int RoIWord2,
                      const std::vector< TrigConf::TriggerThreshold* >* caloThresholds );

      /** returns roi word 0. See  ATL-DAQ-99-015 for
          further explanation.*/
      unsigned int roiWord0() const;

      /** returns roi word 1. See  ATL-DAQ-99-015 for
          further explanation.*/
      unsigned int roiWord1() const;

      /** returns roi word 2. See  ATL-DAQ-99-015 for
          further explanation.*/
      unsigned int roiWord2() const;

      /** returns TRUE if threshold number <em>threshold_number</em> has been passed
          by this ROI. */
      bool passedEtMissThreshold( int thresholdNumber ) const;

      /** returns TRUE if threshold number <em>threshold_number</em> has been passed
          by this ROI. */
      bool passedSumEtThreshold( int thresholdNumber ) const;

      /** returns TRUE if threshold number <em>threshold_number</em> has been passed
          by this ROI. */
      bool passedMEtSigThreshold( int thresholdNumber ) const;

      /** returns a vector of thresholds passed. */
      std::vector< unsigned int >* etMissThresholdsPassed() const;

      /** returns a vector of thresholds passed. */
      std::vector< unsigned int >* sumEtThresholdsPassed() const;

      /** returns a vector of thresholds passed. */
      std::vector< unsigned int >* mEtSigThresholdsPassed() const;

      /** returns the value of the trigger threshold for the threshold passed.
          The trigger threshold is also sometimes called the cluster threshold.*/
      unsigned int etMissTriggerThreshold( const unsigned int thresh ) const;

      /** returns the value of the trigger threshold for the threshold passed.
          The trigger threshold is also sometimes called the cluster threshold.*/
      unsigned int sumEtTriggerThreshold( const unsigned int thresh ) const;

      /** returns the value of the trigger threshold for the threshold passed.
          The trigger threshold is also sometimes called the cluster threshold.*/
      unsigned int mEtSigTriggerThreshold( const unsigned int thresh ) const;

      /** returns true if thresh is a valid EtMiss threshold number */
      bool isValidEtMissThreshold( const unsigned int thresh ) const;

      /** returns true if thresh is a valid Sum Et threshold number */
      bool isValidSumEtThreshold( const unsigned int thresh ) const;

      /** returns true if thresh is a valid Sum Et threshold number */
      bool isValidMEtSigThreshold( const unsigned int thresh ) const;

      /** returns the (signed) Ex energy projection.
          If the RoIWord looks invalid, then zero will be returned. */
      int energyX() const;
      /** returns the Ex overflow bit */
      bool overflowX() const;
      /** returns the (signed) Ey energy projection.
          If the RoIWord looks invalid, then zero will be returned. */
      int energyY() const;
      /** returns the Ey overflow bit */
      bool overflowY() const;
      /** returns the total ET.
          If the RoIWord looks invalid, then zero will be returned. */
      int energyT() const;
      /** returns the ETsum overflow bit */
      bool overflowT() const;

   private:
      /** this is the actual format of the data sent from
          the LVL1 hardware. See  ATL-DAQ-99-015 for
          further explanation. */
      unsigned long int m_roiWord0 { 0 };
      /** this is the actual format of the data sent from
          the LVL1 hardware. See  ATL-DAQ-99-015 for
          further explanation. */
      unsigned long int m_roiWord1 { 0 };
      /** this is the actual format of the data sent from
          the LVL1 hardware. See  ATL-DAQ-99-015 for
          further explanation. */
      unsigned long int m_roiWord2 { 0 };

      std::map< int, unsigned int > m_etMissThresholdValue;
      std::map< int, unsigned int > m_sumEtThresholdValue;
      std::map< int, unsigned int > m_mEtSigThresholdValue;
    
   }; // class RecEnergyRoI

} // namespace LVL1

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( DataVector< LVL1::RecEnergyRoI >, 6262, 1 )
CLASS_DEF( LVL1::RecEnergyRoI, 6263, 1 )

#endif // TRIGT1INTERFACES_RECENERGYROI_H
