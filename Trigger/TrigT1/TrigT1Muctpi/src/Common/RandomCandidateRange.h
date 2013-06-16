// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RandomCandidateRange.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_RANDOMCANDIDATERANGE_H
#define TRIGT1MUCTPI_RANDOMCANDIDATERANGE_H

namespace LVL1MUCTPI {

  /**
   *******************************************************************
   *
   *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $ 
   *
   *    @short Short description of RandomCandidateRange
   *
   *           Long description of RandomCandidateRange
   *
   *      @see 
   *   @author  Author: Thorsten Wengler
   *  @version $Revision: 362102 $
   *
   *******************************************************************
   */
  class RandomCandidateRange {

  public:
    /**
     * The constructor of RandomCandidateRange
     */
    RandomCandidateRange();
    /**
     * methods to get/set m_allowSubsystems Barrel (bin 001),
     * Endcap (bin 010), Forward (bin 100), or any combination
     */
    unsigned int allowSystems(){ return m_allowSystems; }
    void allowSystems( unsigned int allowSys ){ m_allowSystems = allowSys; }
    /**
     * methods to get/set m_allowHemispheres: Pos (bin 01), Neg (bin 10)
     * or both
     */
    unsigned int allowHemispheres(){ return m_allowHemispheres; }
    void allowHemispheres( unsigned int allowHem ){ m_allowHemispheres = allowHem; }
    /**
     * methods to get/set m_minBarrelSectorID
     */
    unsigned int minBarrelSectorID(){ return m_minBarrelSectorID; }
    void minBarrelSectorID( unsigned int minBarrelID ){ m_minBarrelSectorID = minBarrelID; }
    /**
     * methods to get/set m_maxBarrelSectorID
     */
    unsigned int maxBarrelSectorID(){ return m_maxBarrelSectorID; }
    void maxBarrelSectorID( unsigned int maxBarrelID ){ m_maxBarrelSectorID = maxBarrelID; }
    /**
     * methods to get/set m_minEndcapSectorID
     */
    unsigned int minEndcapSectorID(){ return m_minEndcapSectorID; }
    void minEndcapSectorID( unsigned int minEndcapID ){ m_minEndcapSectorID = minEndcapID; }
    /**
     * methods to get/set m_maxEndcapSectorID
     */
    unsigned int maxEndcapSectorID(){ return m_maxEndcapSectorID; }
    void maxEndcapSectorID( unsigned int maxEndcapID ){ m_maxEndcapSectorID = maxEndcapID; }
    /**
     * methods to get/set m_minForwardSectorID
     */
    unsigned int minForwardSectorID(){ return m_minForwardSectorID; }
    void minForwardSectorID( unsigned int minForwardID ){ m_minForwardSectorID = minForwardID; }
    /**
     * methods to get/set m_maxForwardSectorID
     */
    unsigned int maxForwardSectorID(){ return m_maxForwardSectorID; }
    void maxForwardSectorID( unsigned int maxForwardID ){ m_maxForwardSectorID = maxForwardID; }
    /**
     * method to get/set m_allow2Candidates
     */
    unsigned int allowForce2Cand(){ return m_allowForce2Cand; }
    void allowForce2Cand( unsigned int allowFor2Cand ){ m_allowForce2Cand = allowFor2Cand; }
    /**
     * methods to get/set m_minBCID
     */
    unsigned int minBCID(){ return m_minBCID; }
    void minBCID( unsigned int theMinBCID ){ m_minBCID = theMinBCID; }
    /**
     * methods to get/set m_maxBCID
     */
    unsigned int maxBCID(){ return m_maxBCID; }
    void maxBCID( unsigned int theMaxBCID ){ m_maxBCID = theMaxBCID; }
    /**
     * methods to get/set m_minBarrelRoI
     */
    unsigned int minBarrelRoI(){ return m_minBarrelRoI; }
    void minBarrelRoI( unsigned int theMinBarrelRoI ){ m_minBarrelRoI = theMinBarrelRoI; }
    /**
     * methods to get/set m_maxBarrelRoI
     */
    unsigned int maxBarrelRoI(){ return m_maxBarrelRoI; }
    void maxBarrelRoI( unsigned int theMaxBarrelRoI ){ m_maxBarrelRoI = theMaxBarrelRoI; }
    /**
     * methods to get/set m_minEndcapRoI
     */
    unsigned int minEndcapRoI(){ return m_minEndcapRoI; }
    void minEndcapRoI( unsigned int theMinEndcapRoI ){ m_minEndcapRoI = theMinEndcapRoI; }
    /**
     * methods to get/set m_maxEndcapRoI
     */
    unsigned int maxEndcapRoI(){ return m_maxEndcapRoI; }
    void maxEndcapRoI( unsigned int theMaxEndcapRoI ){ m_maxEndcapRoI = theMaxEndcapRoI; }
    /**
     * methods to get/set m_minForwardRoI
     */
    unsigned int minForwardRoI(){ return m_minForwardRoI; }
    void minForwardRoI( unsigned int theMinForwardRoI ){ m_minForwardRoI = theMinForwardRoI; }
    /**
     * methods to get/set m_maxForwardRoI
     */
    unsigned int maxForwardRoI(){ return m_maxForwardRoI; }
    void maxForwardRoI( unsigned int theMaxForwardRoI ){ m_maxForwardRoI = theMaxForwardRoI; }
    /**
     * methods to get/set m_minPt
     */
    unsigned int minPt(){ return m_minPt; }
    void minPt( unsigned int theMinPt ){ m_minPt = theMinPt; }
    /**
     * methods to get/set m_maxPt
     */
    unsigned int maxPt(){ return m_maxPt; }
    void maxPt( unsigned int theMaxPt ){ m_maxPt = theMaxPt; }
    /**
     * methods to get/set m_allowForceGT1InPad1Flag
     */
    unsigned int allowForceGT1InPad1Flag(){ return m_allowForceGT1InPad1Flag; }
    void allowForceGT1InPad1Flag( unsigned int allowForceGT1Pad1 ){ m_allowForceGT1InPad1Flag = allowForceGT1Pad1; }
    /**
     * methods to get/set m_allowForceGT1InPad2Flag
     */
    unsigned int allowForceGT1InPad2Flag(){ return m_allowForceGT1InPad2Flag; }
    void allowForceGT1InPad2Flag( unsigned int allowForceGT1Pad2 ){ m_allowForceGT1InPad2Flag = allowForceGT1Pad2; }
    /**
     * methods to get/set m_allowForce2CandidatesInSec
     */
    unsigned int allowForceGT2InSecFlag(){ return m_allowForceGT2InSecFlag; }
    void allowForceGT2InSecFlag( unsigned int allowForceGT2Sec ){ m_allowForceGT2InSecFlag = allowForceGT2Sec; }
    /**
     * The destructor of RandomCandidateRange
     */
    ~RandomCandidateRange();

  private:

    unsigned int m_allowSystems;
    unsigned int m_allowHemispheres;
    unsigned int m_minBarrelSectorID;
    unsigned int m_maxBarrelSectorID;
    unsigned int m_minEndcapSectorID;
    unsigned int m_maxEndcapSectorID;
    unsigned int m_minForwardSectorID;
    unsigned int m_maxForwardSectorID;
    unsigned int m_allowForce2Cand;
    unsigned int m_minBCID;
    unsigned int m_maxBCID;
    unsigned int m_minBarrelRoI;
    unsigned int m_maxBarrelRoI;
    unsigned int m_minEndcapRoI;
    unsigned int m_maxEndcapRoI;
    unsigned int m_minForwardRoI;
    unsigned int m_maxForwardRoI;
    unsigned int m_minPt;
    unsigned int m_maxPt;
    unsigned int m_allowForceGT1InPad1Flag;
    unsigned int m_allowForceGT1InPad2Flag;
    unsigned int m_allowForceGT2InSecFlag;

  }; // class RandomCandidateRange

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_RANDOMCANDIDATERANGE_H
