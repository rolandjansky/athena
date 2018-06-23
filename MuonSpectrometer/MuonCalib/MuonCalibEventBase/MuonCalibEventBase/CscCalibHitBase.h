/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Input Event Data Model
 * --------------------------------------
 *
 * Author       : Zdenko van Kesteren
 * Creation Date: 17 October 2005
 ***************************************************************************/

#ifndef MUONCALIBEVENT_CSCCALIBHIT_BASE_H
#define MUONCALIBEVENT_CSCCALIBHIT_BASE_H

// std
#include <iostream>

// other packages
#include "GeoPrimitives/GeoPrimitives.h"

// this package
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
  /** 
      @class CscCalibHitBase

     Athena-independent part of the CscCalibHit. A CscCalibHitBase
     contains the basic information of a CSC hit:

     - identifier, stored in the MuonFixedId format (not an Identifier)
     - strip width
     - number of strips used to cluster the hit
     - charge
     - the error
     - position in both local (station) and global coordinates
  */


  class CscCalibHitBase {
  public:
    CscCalibHitBase(); //!< default constructor
    CscCalibHitBase( int nStrips, double stripWidth, int charge, double error, const Amg::Vector3D &globalPos, const Amg::Vector3D &localPos ); //!< constructor setting almost all attributes of the hit
    ~CscCalibHitBase(); //!< destructor
    
    void setIdentifier( const MuonFixedId &id);        //!< sets the MuonFixedId 
    void setNStrips( int nStrips );                    //!< sets the number of strips used to cluster
    void setStripWidth( double stripWidth );           //!< sets the strip width
    void setCharge( int charge );                      //!< sets the charge
    void setError( double error );                     //!< sets the error on the hit
    void setGlobalPos( const Amg::Vector3D &globalPos ); //!< sets the position of the hits in global coordinates
    void setLocalPos( const Amg::Vector3D &localPos );   //!< sets the position of the hits in local (station) coordinates
    
    const MuonFixedId& identify() const;               //!< retrieve the MuonFixedId 
    int                nStrips() const;                //!< retrieve the number of strips used to cluster
    double             stripWidth() const;             //!< retrieve the strip width
    int                charge() const;                 //!< retrieve the charge
    double             error() const;                  //!< retrieve the error on the hit
    const Amg::Vector3D& globalPosition() const;       //!< retrieve the position of the hits in global coordinates
    const Amg::Vector3D& localPosition() const;        //!< retrieve the position of the hits in local (station) coordinates
    
    std::ostream& dump(std::ostream &stream) const;    //!< dump method used to implement operator<<() of the CscCalibHitBase
    
  private:
    MuonFixedId m_id;                                  //!< identifier of the hit (as a MuonFixedId, not Identifier)
    int         m_nStrips;                             //!< number of strips used to cluster       
    double      m_stripWidth;                          //!< width of the strip
    int         m_charge;                              //!< charge
    double      m_error;                               //!< error on the hit
    Amg::Vector3D m_globalPosition;                    //!< position of the hit expressed in global coordinates
    Amg::Vector3D m_localPosition;                     //!< position of the hit expressed in local (station) coordinates
  };
  
  inline void CscCalibHitBase::setIdentifier( const MuonFixedId &id) {
    m_id = id;
  }
  
  inline void CscCalibHitBase::setNStrips( int nStrips ) {
    m_nStrips = nStrips;
  }
  
  inline void CscCalibHitBase::setStripWidth( double stripWidth ) {
    m_stripWidth = stripWidth; 
  }

  inline void CscCalibHitBase::setCharge( int charge ) {
    m_charge = charge;
  }
  
  inline void CscCalibHitBase::setError( double error ) {
    m_error = error;
  }
  
  inline void CscCalibHitBase::setGlobalPos( const Amg::Vector3D &globalPos ) {
    m_globalPosition = globalPos;
  }
  
  inline void CscCalibHitBase::setLocalPos( const Amg::Vector3D &localPos ) {
    m_localPosition = localPos;
  }
  
  inline const MuonFixedId& CscCalibHitBase::identify() const {
    return m_id;
  }
  
  inline int CscCalibHitBase::nStrips() const {
    return m_nStrips; 
  }
  
  inline double CscCalibHitBase::stripWidth() const {
    return m_stripWidth;
  }
  
  inline int CscCalibHitBase::charge() const {
    return m_charge;
  }
  
  inline double CscCalibHitBase::error() const{
    return m_error;
  }
  
  inline const Amg::Vector3D& CscCalibHitBase::globalPosition() const {
    return m_globalPosition;
  }
  
  inline const Amg::Vector3D& CscCalibHitBase::localPosition() const {
    return m_localPosition;
  }
  
} // namespace MuonCalib

std::ostream& operator << (std::ostream& stream, const MuonCalib::CscCalibHitBase& hit);

#endif // MUONCALIBEVENT_CSCCALIBHITBASE_H
