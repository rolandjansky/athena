/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Rpc information in                   //
// Author: Zdenko van Kesteren                             //
// Date  : 21st june 2006                                  //
//*********************************************************//

#ifndef MuonCalibRawRpcHit_h
#define MuonCalibRawRpcHit_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

  /**
     @class MuonCalibRawRpcHit
     Calib-EDM version of RpcPrepData class, containing uncalibrated information. 
     - an identifier, stored in the MuonFixedId format (not an Identifier)
     - position in global coordinates
     - occupancy: value denoting how many times this raw hit is assigned to a MuonCalibSegment. When this value yields -1, the raw hit is not assinged to any MuonCalibSegment 
     - time
     - strip length and width
   */
  class MuonCalibRawRpcHit {
    public:
    MuonCalibRawRpcHit() : m_occupancy(0), m_t(0.0), m_width(0.0), m_length(0.0) {}; //!< default constructor
    MuonCalibRawRpcHit( const MuonFixedId& id, const Amg::Vector3D& glob, int occ, float t, float width ); //!< constructor setting almost all attributes
    MuonCalibRawRpcHit( const MuonFixedId& id, const Amg::Vector3D& glob, int occ, float t, float width, float length ); //!< constructor setting all attributes
    MuonCalibRawRpcHit( const MuonCalibRawRpcHit& hit ) ; //!< copy constructor
    virtual ~MuonCalibRawRpcHit() {}; //!< destructor
  
    MuonCalibRawRpcHit& operator=( const MuonCalibRawRpcHit& rhs ) ; //!< assingment operator

    std::ostream& dump( std::ostream& stream) const; //!< dump to be used for operator<<() to dump the MuonCalibRawRpcHit


    const MuonFixedId& identify()         const { return m_id  ;}       //!< retrieve the MuonFixedId 
    const Amg::Vector3D&  globalPosition()   const { return m_Gpos ;}      //!< retrieve the position expressed in global coordinates
    int                occupancy()        const { return m_occupancy ;} //!< retrieve the occupancy (-1 == not assigned to any segment)
    float              t()                const { return m_t ;}         //!< retrieve the time
    float              width()            const { return m_width ;}     //!< retrieve the strip width
    float              length()           const { return m_length ;}    //!< retrieve the strip length

    void               setId(MuonFixedId Id) { m_id = Id ;}                        //!< sets the MuonFixedId 
    void               setGlobalPosition(const Amg::Vector3D& glob) { m_Gpos = glob;} //!< sets the position expressed in global coordinates
    void               setOccupancy(int occ) { m_occupancy = occ ;}                //!< sets the occupancy
    void               setT(float t) { m_t = t ;}                                  //!< sets the time
    void               setWidth(float width) { m_width = width ;}                  //!< sets the strip width
    void               setLength(float length) { m_length = length ;}              //!< sets the strip length

    private:
    MuonFixedId m_id;        //!< identifier of the raw hit, (is a MuonFixedId, not an Identifier) 
    Amg::Vector3D  m_Gpos;      //!< position of the hit in global coordinates
    int         m_occupancy; //!< occupancy (-1 == not assigned to any segment)
    float       m_t;         //!< time
    float       m_width;     //!< strip width
    float       m_length;    //!< strip length

  };
  inline MuonCalibRawRpcHit::MuonCalibRawRpcHit(const MuonFixedId& id, const Amg::Vector3D& glob, int occ, float t, float width, float length )
    : m_id(id), m_Gpos(glob), m_occupancy(occ), m_t(t), m_width(width), m_length(length) {
  }

  inline MuonCalibRawRpcHit::MuonCalibRawRpcHit(const MuonFixedId& id, const Amg::Vector3D& glob, int occ, float t, float width )
    : m_id(id), m_Gpos(glob), m_occupancy(occ), m_t(t), m_width(width), m_length(0.) {
  }
  
} // namespace MuonCalib

std::ostream& operator << (std::ostream& stream, const MuonCalib::MuonCalibRawRpcHit& rawHit);

#endif // MuonCalibRawRpcHit_h
