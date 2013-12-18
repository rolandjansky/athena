/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBHOLE_E_H
#define MUONCALIBHOLE_E_H

#include "MuonCalibIdentifier/MuonFixedId.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib{
  /**
     @class MuonCalibHole_E
     class describing a Hole on track. A hole is the position/ID of a MDT where a Hit was expected, though not recorded.
  */
  class MuonCalibHole_E{
  public:
    MuonCalibHole_E();                                               //!< default constructor
    MuonCalibHole_E( const MuonFixedId& id, const Amg::Vector3D& pos);  //!< constructor initializing all members
    MuonCalibHole_E( const MuonCalibHole_E& hit ) ;                  //!< copyconstructor
    ~MuonCalibHole_E() {} ;                                          //!< destructor
 
    MuonCalibHole_E& operator=( const MuonCalibHole_E& rhs ) ;       //!< assignment operator

    const MuonFixedId& identify() const { return m_id ; } ;          //!< retrieve hole ID (MuonCalib::MuonFixedId)
    const Amg::Vector3D&  position() const { return m_pos ; } ;         //!< retrieve hole position

    void setId( const MuonFixedId& id ) { m_id = id ; } ;            //!< sets hole ID (MuonCalib::MuonFixedId)
    void setPosition( const Amg::Vector3D& pos ) { m_pos = pos ; } ;    //!< sets hole position

  private:
    MuonFixedId m_id;	 //!< hole ID (MuonCalib::MuonFixedId)		       
    Amg::Vector3D  m_pos;   //!< hole position
  } ;

}//namespace MuonCalib

#endif //MUONCALIBHOLE_E_H
