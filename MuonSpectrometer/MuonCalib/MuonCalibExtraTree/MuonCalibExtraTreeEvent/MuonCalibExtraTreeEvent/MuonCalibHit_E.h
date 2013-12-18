/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBHIT_E_H
#define MUONCALIBHIT_E_H

#include "MuonCalibIdentifier/MuonFixedId.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib{
  /**
     @class MuonCalibHit_E
     Simplified class designed to store information of a MDT hit. It has :
     - a position
     - a MuonCalib::MuonFixedId identifier
     - driftradius and its error
     
     @author Zdenko.Van.Kesteren
  */
  class MuonCalibHit_E{
  public:
    MuonCalibHit_E();                                                                               //!< default constructor
    MuonCalibHit_E( const MuonFixedId& id, const Amg::Vector3D& pos);                                  //!< constructor initializing its ID and position
    MuonCalibHit_E( const MuonFixedId& id, const Amg::Vector3D& pos, double driftRadius, double error);//!< constructor initializing all members
    MuonCalibHit_E( const MuonFixedId& id, const Amg::Vector3D& pos, double driftRadius, double error,
		    double resi, double pull, int measType);
    ~MuonCalibHit_E() {} ;                                                                          //!< destructor


    const MuonFixedId& identify() const { return m_id ; }                    //!< return the MuonCalib::MuonFixedId of the MuonCalibHit
    const Amg::Vector3D&  position() const { return m_pos ; }                   //!< return the (global) position of the MuonCalibHit
    double       driftRadius() const { return m_driftRadius ; }        //!< return the driftradius of the MuonCalibHit 
    double       error() const { return m_error ; }                    //!< return the error on the driftradius of the MuonCalibHit 
    double       residual() const { return m_resi ; }                  //!< return the residual
    double       pull() const { return m_pull ; }                      //!< return the pull
    int          type() const { return m_measType ; }                  //!< return the type of measurement

    void setId( const MuonFixedId& id ) { m_id = id ; }                        //!< sets the MuonCalib::MuonFixedId of the MuonCalibHit
    void setPosition( const Amg::Vector3D& pos ) { m_pos = pos ; }                //!< sets the (global) position of the MuonCalibHit
    void setDriftRadius( double driftRadius ) { m_driftRadius = driftRadius ; }//!< sets the driftradius of the MuonCalibHit 
    void setError( double error ) { m_error = error ; }                        //!< sets the error on the driftradius of the MuonCalibHit 
    void setResidual( double res ) { m_resi = res ; }                        //!< sets the error on the driftradius of the MuonCalibHit 
    void setPull( double pull ) { m_pull = pull ; }                        //!< sets the error on the driftradius of the MuonCalibHit 
    void setType( int type ) { m_measType = type ; }                        //!< sets the error on the driftradius of the MuonCalibHit 

  private:
    MuonFixedId m_id;	       //!< Identifier of the (athena-independent) type MuonCalib::MuonFixedId		       
    Amg::Vector3D  m_pos;         //!< Global position
    double      m_driftRadius; //!< driftRadius
    double      m_error;       //!< error on the driftRadius
    float       m_resi;        //!< residual
    float       m_pull;        //!< pull including track error
    int         m_measType;    //!< error on the driftRadius
  } ;

}//namespace MuonCalib

#endif //MUONCALIBHIT_E_H
