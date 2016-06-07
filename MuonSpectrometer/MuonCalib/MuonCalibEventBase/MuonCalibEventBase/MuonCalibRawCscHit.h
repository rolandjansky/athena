/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Csc information in                   //
// Author: Zdenko van Kesteren                             //
// Date  : 21st june 2006                                  //
//*********************************************************//

#ifndef MuonCalibRawCscHit_h
#define MuonCalibRawCscHit_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

  /**
     @class MuonCalibRawCscHit
     Calib-EDM version of CscPrepData class, containing uncalibrated information. 
     - an identifier, stored in the MuonFixedId format (not an Identifier)
     - position in global coordinates
     - occupancy: value denoting how many times this raw hit is
       assigned to a MuonCalibSegment. A value of -1 indicates the
       raw hit is not assinged to any MuonCalibSegment
     - charge and time
     - strip width
   */

  class MuonCalibRawCscHit {
    public: 
    
    MuonCalibRawCscHit()
     : m_Gpos(0.,0.,0.), m_occupancy(0), m_t(0.), m_width(0.), m_charge(0.)
    {};//!< default constructor

    MuonCalibRawCscHit( const MuonFixedId &id, const Amg::Vector3D &glob, int occ, float t, float width, float charge );//!< constructor setting all attributes
    MuonCalibRawCscHit( const MuonCalibRawCscHit &hit ); //!< copy constructor
    ~MuonCalibRawCscHit() {}; //!< destructor

    MuonCalibRawCscHit& operator=( const MuonCalibRawCscHit &rhs ); //!< assingment operator
    std::ostream& dump( std::ostream &stream) const; //!< dump to be used for operator<<() to dump the MuonCalibRawCscHit

    const MuonFixedId &identify()         const { return m_id;}        //!< retrieve the MuonFixedId 
    const Amg::Vector3D &globalPosition() const { return m_Gpos;}      //!< retrieve the position expressed in global coordinates
    int                occupancy()        const { return m_occupancy;} //!< retrieve the occupancy (-1 == not assigned to any segment)
    float              t()                const { return m_t;}         //!< retrieve the time
    float              width()            const { return m_width;}     //!< retrieve the strip width
    float              charge()           const { return m_charge;}    //!< retrieve the charge

    void               setId(MuonFixedId Id) { m_id = Id;}                        //!< sets the MuonFixedId 
    void               setGlobalPosition(const Amg::Vector3D &glob) { m_Gpos = glob;} //!< sets the position expressed in global coordinates
    void               setOccupancy(int occ) { m_occupancy = occ;}                //!< sets the occupancy
    void               setT(float t) { m_t = t;}                                  //!< sets the time
    void               setWidth(float width) { m_width = width;}                  //!< sets the strip width
    void               setCharge(float charge) { m_charge = charge;}              //!< sets the charge

    private:
    MuonFixedId m_id;        //!< identifier of the raw hit, (a MuonFixedId, not an Identifier) 
    Amg::Vector3D m_Gpos;    //!< position of the hit in global coordinates
    int         m_occupancy; //!< occupancy (-1 == not assigned to any segment)
    float       m_t;         //!< time
    float       m_width;     //!< strip width
    float       m_charge;    //!< charge

  };
  inline MuonCalibRawCscHit::MuonCalibRawCscHit(const MuonFixedId &id, const Amg::Vector3D &glob, int occ, float t, float width, float charge )
    : m_id(id), m_Gpos(glob), m_occupancy(occ), m_t(t), m_width(width), m_charge(charge) {
  }
  
} // namespace MuonCalib

std::ostream& operator << (std::ostream &stream, const MuonCalib::MuonCalibRawCscHit &rawHit);

#endif // MuonCalibRawCscHit_h
