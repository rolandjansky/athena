/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store truth information in                     //
// Author: Zdenko van Kesteren                             //
// Date  : 2 December 2005                                 //
//*********************************************************//

#ifndef MuonCalibMdtTruthHit_h
#define MuonCalibMdtTruthHit_h

#include <iostream>

#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
  /**
     @class MuonCalibMdtTruthHit
     Class with truth information of a MDT hit, containing:
     - an identifier, stored in the MuonFixedId format (not an Identifier)
     - barcode
     - driftradius and position along the tube
  */
  class MuonCalibMdtTruthHit{
    public:
    MuonCalibMdtTruthHit(); //!< default constructor
    MuonCalibMdtTruthHit( MuonFixedId id, int barCode, double driftRadius, double positionAlongTube ); //<! constructor initializing all attributes of the MuonCalibMdtTruthHit
    MuonCalibMdtTruthHit( const MuonCalibMdtTruthHit& truth ); //!< copyconstructor
    ~MuonCalibMdtTruthHit() {} ; //!< destructor

    MuonCalibMdtTruthHit& operator=( const MuonCalibMdtTruthHit& rhs ); //!< assingment operator
    
    std::ostream& dump( std::ostream& stream ) const;   //!< dump to be used for operator<<() to dump the MuonCalibMdtTruthHit
  
    MuonFixedId identify() const {return m_ID ; } ;                     //!< retrive the MuonFixedId
    int barCode() const { return m_barCode ; } ;                        //!< retrieve the barcode
    double driftRadius() const { return m_driftRadius ; } ;             //!< retrieve the driftradius
    double positionAlongTube() const { return m_positionAlongTube ; } ; //!< retrieve the position along tube
    
    void setIdentifier( MuonFixedId id ) { m_ID = id ; } ;                                                //!< sets the MuonFixedId
    void setBarCode( int barCode ) { m_barCode = barCode ; } ;                                            //!< sets the barcode
    void setDriftRadius( double driftRadius ) { m_driftRadius = driftRadius ; } ;                         //!< sets the driftradius
    void setPositionAlongTube( double positionAlongTube ) { m_positionAlongTube = positionAlongTube ; } ; //!< sets the position along tube

    private:
    MuonFixedId m_ID;              //!< identifier of the truth hit, (is a MuonFixedId, not an Identifier) 
    int     m_barCode;             //!< barcode 
    double  m_driftRadius;         //!< drift radius
    double  m_positionAlongTube;   //!< position along tube
  } ;

}// namespace MuonCalib

std::ostream& operator << ( std::ostream& stream, const MuonCalib::MuonCalibMdtTruthHit& truth );

#endif //MuonCalibTruth_h
