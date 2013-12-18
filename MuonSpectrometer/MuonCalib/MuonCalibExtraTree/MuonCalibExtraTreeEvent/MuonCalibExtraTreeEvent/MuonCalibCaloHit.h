/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBCALOHIT_H
#define MUONCALIBCALOHIT_H

#include "GeoPrimitives/GeoPrimitives.h"


namespace MuonCalib{

  /**
     Simplified class designed to store information of a MBTS hit. It has :
     - a position
     - a Identifier (int)
     - time and charge
     
     @author Niels van Eldik
  */
  class MuonCalibCaloHit{
  public:
    MuonCalibCaloHit();                                                                               //!< default constructor
    MuonCalibCaloHit( const int& id, const Amg::Vector3D& pos, double time, double charge );
    ~MuonCalibCaloHit() {} ;                                                                          //!< destructor

    const int& identify() const { return m_id; }              //!< return the identifier of the Hit
    const Amg::Vector3D&  position() const { return m_pos; }     //!< return the (global) position of the MuonCalibHit
    double       time() const { return m_time; }        //!< return the time 
    double       charge() const { return m_charge; }    //!< return the charge

  private:
    int         m_id;	       //!< Identifier 
    Amg::Vector3D  m_pos;         //!< Global position
    double      m_time;        //!< time
    double      m_charge;      //!< charge
  } ;

}//namespace MuonCalib

#endif 
