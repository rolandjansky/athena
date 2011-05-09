// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySum_ROI.h,v 1.5 2008-05-08 15:00:11 krasznaa Exp $
#ifndef ANALYSISTRIGGEREVENT_ENERGYSUM_ROI_H
#define ANALYSISTRIGGEREVENT_ENERGYSUM_ROI_H

// STL/System include(s):
#include <string>
#include <vector>
#include <inttypes.h>

/**
 *   @short Energy-Sum RoI class for analysis
 *
 *          Class giving information about the energy sum and missing Et at LVL1.
 *
 *  @author Tadashi Maeno, Alan Watson, Attila Krasznahorkay
 * @version $Revision: 1.5 $
 *    @date $Date: 2008-05-08 15:00:11 $
 */
class EnergySum_ROI {

public:
   /// Constructor used when creating the object from RoIBResult data
   EnergySum_ROI( uint32_t roiWord0, uint32_t roiWord1, uint32_t roiWord2,
                  float energyX, float energyY, float energyT, 
                  uint32_t thrPatSummET, uint32_t thrPatMissET, uint32_t thrPatMETSig, unsigned int overflows )
      : m_roiWord0( roiWord0 ), m_roiWord1( roiWord1 ), m_roiWord2( roiWord2 ),
        m_thrPatSummET( thrPatSummET ), m_thrPatMissET( thrPatMissET ), m_thrPatMETSig( thrPatMETSig ), m_thresholds( 0 ),
        m_energyX( energyX ), m_energyY( energyY ), m_energyT( energyT ), 
        m_overflows( overflows ) {}
   /// Default constructor (for persistency purposes)
   EnergySum_ROI()
      : m_roiWord0( 0 ), m_roiWord1( 0 ), m_roiWord2( 0 ), m_thrPatSummET( 0 ), m_thrPatMissET( 0 ), m_thrPatMETSig( 0 ),
        m_thresholds( 0 ), m_energyX( 0. ), m_energyY( 0. ), m_energyT( 0. ), m_overflows( 0 ) {}
   /// Destructor
   virtual ~EnergySum_ROI () {}

   /// Type storing the names of the passed thresholds
   typedef std::vector< std::string > thresholds_type;

   /// Return the first RoI word produced by the L1Calo hardware
   uint32_t getROIWord0() const { return m_roiWord0; }
   /// Return the second RoI word produced by the L1Calo hardware
   uint32_t getROIWord1() const { return m_roiWord1; }
   /// Return the third RoI word produced by the L1Calo hardware
   uint32_t getROIWord2() const { return m_roiWord2; }

   /// Return the total energy deposited in the X direction
   float getEnergyX() const { return m_energyX; }
   /// Return the total energy deposited in the Y direction
   float getEnergyY() const { return m_energyY; }
   /// Return the deposited total transverse energy
   float getEnergyT() const { return m_energyT; }
   /// Return the missing energy in the X direction
   float getExMiss() const { return -m_energyX; }
   /// Return the missing energy in the Y direction
   float getEyMiss() const { return -m_energyY; }
   /// Return the overflow flag for the E<sub>X</sub> calculation
   bool  getOverflowX() const { return ( m_overflows & 0x1 ); }
   /// Return the overflow flag for the E<sub>Y</sub> calculation
   bool  getOverflowY() const { return ( ( m_overflows >> 1 ) & 0x1 ); }
   /// Return the overflow flag for the E<sub>T</sub> calculation
   bool  getOverflowT() const { return ( ( m_overflows >> 2 ) & 0x1 ); }
   /// Return the overflow bit-pattern
   unsigned int getOverflows() const { return m_overflows; }

   /// Return the bit-pattern describing the passed sum-E<sub>T</sub> thresholds
   uint32_t getThrPatSummET() const { return m_thrPatSummET; }
   /// Return the bit-pattern describing the passed missing-E<sub>T</sub> thresholds
   uint32_t getThrPatMissET() const { return m_thrPatMissET; }
   /// Return the bit-pattern describing the passed missing-E<sub>T</sub> significance thresholds
   uint32_t getThrPatMETSig() const { return m_thrPatMETSig; }

   /// Return the names of the passed threshodlds
   const thresholds_type& getThresholds() const { return m_thresholds; }

   /// Add the name of a threshold that this RoI passed
   void addThreshold( thresholds_type::value_type thr ) { m_thresholds.push_back( thr ); }

private:
   uint32_t m_roiWord0; //!< First 32bit encoded ROI word
   uint32_t m_roiWord1; //!< Second 32bit encoded ROI word
   uint32_t m_roiWord2; //!< Third 32bit encoded ROI word

   uint32_t m_thrPatSummET; //!< threshold bit pattern for Summed E<sub>T</sub>
   uint32_t m_thrPatMissET; //!< threshold bit pattern for Missing E<sub>T</sub>
   uint32_t m_thrPatMETSig; //!< threshold bit pattern for Missing E<sub>T</sub> Significance

   thresholds_type m_thresholds; //!< Names of the passed thresholds

   float m_energyX; //!< Total energy deposited in the X direction
   float m_energyY; //!< Total energy deposited in the Y direction
   float m_energyT; //!< Total deposited transverse energy

   unsigned int m_overflows; //!< Overflow flags: bit 0 = Ex, bit 1 = Ey, bit 2 = ETsum

}; // class EnergySum_ROI

#endif // ANALYSISTRIGGEREVENT_ENERGYSUM_ROI_H
