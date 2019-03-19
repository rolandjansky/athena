/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         JEPRoIDecoder.h  -  description
                            -------------------
   begin                : Fri Apr 19 2002
   email                : moyse@ph.qmw.ac.uk
***************************************************************************/

#ifndef TRIGT1INTERFACES_JEPROIDECODER_H
#define TRIGT1INTERFACES_JEPROIDECODER_H

#include <vector>

#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/RoIDecoder.h"

namespace LVL1 {

  /**
   *  A level 1 calorimeter trigger conversion service:
   *  returns the Coordinate represented by a RoI word
   *
   *  @author E.Moyse
   * \todo this should probably be a static class.
   */
  class JEPRoIDecoder : public RoIDecoder {

  public:
    JEPRoIDecoder();
    virtual ~JEPRoIDecoder();
    
    TrigT1CaloDefs::RoIType roiType( unsigned int word ) const;
    
    /** Identify Jet RoI Version */
    unsigned int jetRoIVersion( unsigned int word ) const;

    /** Return eta/phi coordinate object */
    virtual CoordinateRange coordinate( const unsigned int roiWord ) const override;
    /** Return hardware coordinates */
    unsigned int crate( const unsigned int word ) const;
    unsigned int module( const unsigned int word ) const;
    unsigned int row( const unsigned int word ) const;
    unsigned int column( const unsigned int word ) const;
    /** returns a vector containing the numbers of threshold passed i.e. if the vector contains 1,3,5
        it means that this RoI passed thresholds 1,3 and 5.*/
    const std::vector< unsigned int > thresholdsPassed( const unsigned int word ) const;
    const std::vector<unsigned int> fwdThresholdsPassed( const unsigned int word ) const;
    
    /** Return ET values from Run 2 Jet RoIs */
    unsigned int etLarge( const unsigned int word ) const;
    unsigned int etSmall( const unsigned int word ) const;
    
    /** returns the (signed) Ex energy projection. If the RoIWord looks invalid, then zero will be returned. */
    int energyX( unsigned int energyRoIWord0 ) const;
    /** returns the (signed) Ex energy projection. If the RoIWord looks invalid, then zero will be returned. */
    int energyY( unsigned int energyRoIWord1 ) const;
    /** returns the (signed) Ex energy projection. If the RoIWord looks invalid, then zero will be returned. */
    int energyT( unsigned int energyRoIWord2 ) const;
    /** returns overflow flag energy RoIWord */
    bool energyOverflow( unsigned int energyRoIWord ) const;
    /** returns a vector containing the numbers of threshold passed
    i.e. if the vector contains 1,3,5 it means that this RoI passed thresholds 1,3 and 5.*/
    std::vector<unsigned int> mEtSigThresholdsPassed( const unsigned int energyRoIWord0 ) const;
    /** returns a vector containing the numbers of threshold passed
    i.e. if the vector contains 1,3,5 it means that this RoI passed thresholds 1,3 and 5.*/
    std::vector<unsigned int> etSumThresholdsPassed( const unsigned int energyRoIWord1 ) const;
    /** returns a vector containing the numbers of threshold passed
    i.e. if the vector contains 1,3,5 it means that this RoI passed thresholds 1,3 and 5.*/
    std::vector<unsigned int> etMissThresholdsPassed( const unsigned int energyRoIWord2 ) const;
    /** returns true if thresh is passed*/
    bool mEtSigThresholdPassed( const unsigned int energyRoIWord0, const unsigned int thresh ) const;
    /** returns true if thresh is passed*/
    bool sumEtThresholdPassed( const unsigned int energyRoIWord1, const unsigned int thresh ) const;
    /** returns true if thresh is passed*/
    bool etMissThresholdPassed( const unsigned int energyRoIWord2, const unsigned int thresh ) const;


  private:
    unsigned int mEtSigThresholdsFlags( const unsigned int energyRoIWord0 ) const;
    unsigned int sumEtThresholdsFlags( const unsigned int energyRoIWord1 ) const;
    unsigned int missEtThresholdsFlags( const unsigned int energyRoIWord1 ) const;

    /** No descriptions */
    int decodeEnergyComponent( const unsigned int energyRoIWord ) const;

    /** return a CoordinateRange for the JEMs coving -2.4<eta<2.4 */
    CoordinateRange midJEMCoordinate(const unsigned int jem, const unsigned int roiWord) const;
    /** returns a CoordinateRange for the end JEMs, i.e. 2.4<eta<4.9 */
    CoordinateRange leftEndJEMCoordinate(const unsigned int jem, const unsigned int roiWord) const;
    /** returns a CoordinateRange for the end JEMs, i.e. 2.4<eta<4.9 */
    CoordinateRange rightEndJEMCoordinate(const unsigned int jem, const unsigned int roiWord) const;
    /** set phi coords for Jet RoI */
    void setPhiCoords( const unsigned int jem, const unsigned int roiWord,
                       double& phiMin, double& phiMax ) const;
  }; // class JEPRoIDecoder

} // namespace LVL1

#endif // TRIGT1INTERFACES_JEPROIDECODER_H
