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
    class MuonCalibMdtTruthHit {
    public:
        MuonCalibMdtTruthHit();  //!< default constructor
        MuonCalibMdtTruthHit(MuonFixedId id, int barCode, double driftRadius, double positionAlongTube, double gpositionX,
                             double gpositionY, double gpositionZ,
                             double time);                        //<! constructor initializing all attributes of the MuonCalibMdtTruthHit
        MuonCalibMdtTruthHit(const MuonCalibMdtTruthHit &truth);  //!< copyconstructor
        ~MuonCalibMdtTruthHit(){};                                //!< destructor

        MuonCalibMdtTruthHit &operator=(const MuonCalibMdtTruthHit &rhs);  //!< assignment operator

        std::ostream &dump(std::ostream &stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibMdtTruthHit

        MuonFixedId identify() const { return m_ID; };                             //!< retrieve the MuonFixedId
        int barCode() const { return m_barCode; };                                 //!< retrieve the barcode
        double driftRadius() const { return m_driftRadius; };                      //!< retrieve the driftradius
        double positionAlongTube() const { return m_positionAlongTube; };          //!< retrieve the position along tube
        double gpositionX() const { return m_gpositionX; };                        //!< retrieve the global X position
        double gpositionY() const { return m_gpositionY; };                        //!< retrieve the global Y position
        double gpositionZ() const { return m_gpositionZ; };                        //!< retrieve the global Z position
        double time() const { return m_time; };                                    //!< retrieve the time
        void setIdentifier(MuonFixedId id) { m_ID = id; };                         //!< sets the MuonFixedId
        void setBarCode(int barCode) { m_barCode = barCode; };                     //!< sets the barcode
        void setDriftRadius(double driftRadius) { m_driftRadius = driftRadius; };  //!< sets the driftradius
        void setPositionAlongTube(double positionAlongTube) { m_positionAlongTube = positionAlongTube; };  //!< sets the position along tube
        void setgPositionX(double gpositionX) { m_gpositionX = gpositionX; };                              //!< sets the global X position
        void setgPositionY(double gpositionY) { m_gpositionY = gpositionY; };                              //!< sets the global Y position
        void setgPositionZ(double gpositionZ) { m_gpositionZ = gpositionZ; };                              //!< sets the global Z position
        void setTime(double time) { m_time = time; };                                                      //!< sets the time

    private:
        MuonFixedId m_ID;            //!< identifier of the truth hit, (a MuonFixedId, not an Identifier)
        int m_barCode;               //!< barcode
        double m_driftRadius;        //!< drift radius
        double m_positionAlongTube;  //!< position along tube
        double m_gpositionX;         //!< global x position of the truth hit
        double m_gpositionY;         //!< global Y position of the truth hit
        double m_gpositionZ;         //!< global Z position of the truth hit
        double m_time;               //!< time
    };

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibMdtTruthHit &truth);

#endif  // MuonCalibTruth_h
