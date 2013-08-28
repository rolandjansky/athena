/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store truth information in                     //
// Author: Zdenko van Kesteren                             //
// Date  : 2 December 2005                                 //
//*********************************************************//

#ifndef MuonCalibTruth_h
#define MuonCalibTruth_h
#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {
  /**
     @class MuonCalibTruth
     Calib-EDM version of TrackRecord, given at the MuonSpectrometer entrance. It contains the following:
     - position and momentum
     - kinetic energy
     - PDG code (13 for a muon) and the BarCode
  */

  class MuonCalibTruth {
    public:
    MuonCalibTruth() ; //!< default constructor
    MuonCalibTruth( const Amg::Vector3D& pos, const Amg::Vector3D& momentum, double kinEnergy, int code, int barCode ) ; //!< constructor initializing all attributes
    MuonCalibTruth( const MuonCalibTruth& truth ); //!< copy constructor
    ~MuonCalibTruth( ) {}; //!< destructor

    MuonCalibTruth& operator=(const MuonCalibTruth& rhs);  //!< assignement operator

    void setPosition( const Amg::Vector3D& pos ) ; //!< sets position
    void setMomentum( const Amg::Vector3D& mom ); //!< sets momentum
    void setKinEnergy( double kinEnergy ) ;     //!< sets kinetic energy 
    void setPDGCode( int code ) ;               //!< sets PDG code
    void setBarCode( int barCode ) ;            //!< sets barcode
    
    const Amg::Vector3D& position() const;         //!< retrieve position
    const Amg::Vector3D& momentum() const;        //!< retrieve momentum
    double kinEnergy() const;                   //!< retrieve kinetic energy 
    int PDGCode() const;                        //!< retrieve PDG code
    int barCode() const;                        //!< retrieve barcode

    std::ostream& dump( std::ostream& stream) const; //!< dump method to be used in operator<<() of the MuonCalibTruth for output
    private:
    Amg::Vector3D  m_position;  //!< position
    Amg::Vector3D m_momentum;  //!< momentum
    double      m_kinEnergy; //!< kinetic energy
    int         m_PDGCode;   //!< PDG code
    int         m_barCode;   //!< bar code
  } ;

  inline MuonCalibTruth::MuonCalibTruth()
    : m_position(0.,0.,0.), m_momentum(0.,0.,0.), m_kinEnergy(0.), m_PDGCode(0), m_barCode(0) {
  } 

  inline MuonCalibTruth::MuonCalibTruth( const Amg::Vector3D& pos, const Amg::Vector3D& momentum, double kinEnergy, int code, int barCode ) 
    :  m_position(pos), m_momentum(momentum), m_kinEnergy(kinEnergy), m_PDGCode(code), m_barCode(barCode) {
  }

  inline void MuonCalibTruth::setPosition( const Amg::Vector3D& pos ) {
    m_position = pos ;
  }

  inline void MuonCalibTruth::setMomentum( const Amg::Vector3D& mom ) {
    m_momentum = mom ;
  }

  inline void MuonCalibTruth::setKinEnergy( double kinEnergy ) {
    m_kinEnergy = kinEnergy ;
  }

 inline void MuonCalibTruth::setPDGCode( int code ) {
   m_PDGCode = code;
 }

 inline void MuonCalibTruth::setBarCode( int barCode ){
   m_barCode = barCode ;
 }


 inline const Amg::Vector3D& MuonCalibTruth::position() const {
   return m_position ;
 }

 inline const Amg::Vector3D& MuonCalibTruth::momentum() const {
   return m_momentum ;
 }

 inline double MuonCalibTruth::kinEnergy() const {
   return m_kinEnergy ;
 }
 
 inline int MuonCalibTruth::PDGCode() const {
   return m_PDGCode ;
 } 

 inline int MuonCalibTruth::barCode() const {
   return m_barCode ;
 } 



}// namespace MuonCalib

std::ostream& operator << (std::ostream& stream, const MuonCalib::MuonCalibTruth& truht);

#endif //MuonCalibTruth_h
