/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Mdt information in                   //
// Author: Zdenko van Kesteren                             //
// Date  : 13th january 2006                               //
//*********************************************************//

#ifndef MuonCalibRawMdtHit_h
#define MuonCalibRawMdtHit_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
    /**
       @class MuonCalibRawMdtHit
       Calib-EDM version of MdtPrepData class, containing uncalibrated information.
       - an identifier, stored in the MuonFixedId format (not an Identifier)
       - position in both local (station) and global coordinates
       - occupancy: value denoting how many times this raw hit is assigned to a MuonCalibSegment. When this value yields -1, the raw hit is
       not assinged to any MuonCalibSegment
       - tdc and adc counts
       - drift time, drift radius and its error
     */
    class MuonCalibRawMdtHit {
    public:
        MuonCalibRawMdtHit() :
            m_Lpos(0., 0., 0.),
            m_Gpos(0., 0., 0.),
            m_adc(0),
            m_tdc(0),
            m_occupancy(0),
            m_t(0.),
            m_r(0.),
            m_dr(0.){};  //!< default constructor
        MuonCalibRawMdtHit(const MuonFixedId &id, const Amg::Vector3D &loc, const Amg::Vector3D &glob,
                           int occ);                        //!< constructor initializing the identifier, the position(s) and occupancy.
        MuonCalibRawMdtHit(const MuonCalibRawMdtHit &hit);  //!< copyconstructor
        ~MuonCalibRawMdtHit(){};                            //!< destructor

        MuonCalibRawMdtHit &operator=(const MuonCalibRawMdtHit &rhs);  //!< assignment operator

        std::ostream &dump(std::ostream &stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibRawMdtHit

        const MuonFixedId &identify() const { return m_id; }            //!< retrieve the MuonFixedId
        const Amg::Vector3D &localPosition() const { return m_Lpos; }   //!< retrieve the position expressed in local (station) coordinates
        const Amg::Vector3D &globalPosition() const { return m_Gpos; }  //!< retrieve the position expressed in global coordinates
        int adcCount() const { return m_adc; }                          //!< retrieve adc counts
        int tdcCount() const { return m_tdc; }                          //!< retrieve tdc counts
        int occupancy() const { return m_occupancy; }                   //!< retrieve the occupancy (-1 == not assigned to any segment)
        double driftTime() const { return m_t; }                        //!< retrieve the drift time
        double driftRadius() const { return m_r; }                      //!< retrieve the drift radius
        double driftRadiusError() const { return m_dr; }                //!< retrieve the error on the drift radius

        void setId(MuonFixedId Id) { m_id = Id; }                          //!< sets the MuonFixedId
        void setLocalPosition(const Amg::Vector3D &loc) { m_Lpos = loc; }  //!< sets the position expressed in local (station) coordinates
        void setGlobalPosition(const Amg::Vector3D &glob) { m_Gpos = glob; }  //!< sets the position expressed in global coordinates
        void setAdc(int adc) { m_adc = adc; }                                 //!< sets adc counts
        void setTdc(int tdc) { m_tdc = tdc; }                                 //!< sets tdc counts
        void setOccupancy(int occ) { m_occupancy = occ; }                     //!< sets the occupancy
        void setDriftTime(double t) { m_t = t; }                              //!< sets the drift time
        void setDriftRadius(double r) { m_r = r; }                            //!< sets the drift radius
        void setDriftRadiusError(double dr) { m_dr = dr; }                    //!< sets the error on the drift radius

    private:
        MuonFixedId m_id;      //!< identifier of the raw hit, (is a MuonFixedId, not an Identifier)
        Amg::Vector3D m_Lpos;  //!< position of the hit in local (station) coordinates
        Amg::Vector3D m_Gpos;  //!< position of the hit in global coordinates
        int m_adc;             //!< ADC count
        int m_tdc;             //!< TDC count
        int m_occupancy;       //!< occupancy (-1 == not assigned to any segment)
        double m_t;            //!< drift time
        double m_r;            //!< drift radius
        double m_dr;           //!< error on the drift radius
    };

    inline MuonCalibRawMdtHit::MuonCalibRawMdtHit(const MuonFixedId &id, const Amg::Vector3D &loc, const Amg::Vector3D &glob, int occ) :
        m_id(id), m_Lpos(loc), m_Gpos(glob), m_adc(0), m_tdc(0), m_occupancy(occ), m_t(0.0), m_r(0.0), m_dr(0.0) {}

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibRawMdtHit &rawHit);

#endif  // MuonCalibRawMdtHit_h
