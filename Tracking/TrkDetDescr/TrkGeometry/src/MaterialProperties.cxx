/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialProperties.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/MaterialProperties.h"
#include <iostream>

Trk::MaterialProperties::MaterialProperties():
  m_material(),
  m_dInX0(0.),
  m_dInL0(0.),  
  m_zOaTrTd(0.)   
{}

Trk::MaterialProperties::MaterialProperties(
			             float path,
                         float Xo,
                         float Lo,
                         float averageA,
                         float averageZ,
                         float averageRho,
                         float dEdX) :
  m_material(Xo, Lo, averageA, averageZ, averageRho, dEdX),
  m_dInX0( Xo*Xo > 10e-10 ? path/Xo : 0.),
  m_dInL0( Lo*Lo > 10e-10 ? path/Lo : 0.),
  m_zOaTrTd(averageA * averageA > 10e-10 ? averageZ/averageA*averageRho*path : 0.)   
{}

// TODO add constructor with element composition

Trk::MaterialProperties::MaterialProperties(const Trk::Material& material, float path) :
  m_material(material),
  m_dInX0( material.X0*material.X0 > 10e-10 ? path/material.X0 : 0.),
  m_dInL0( material.L0*material.L0 > 10e-10 ? path/material.L0 : 0.),
  m_zOaTrTd( material.A*material.A > 10e-10  ? path*material.Z/material.A*material.rho : 0.)
{}

Trk::MaterialProperties::MaterialProperties(const Trk::MaterialProperties& mprop)   
   
= default;


Trk::MaterialProperties* Trk::MaterialProperties::clone() const 
{ return new Trk::MaterialProperties(*this); }


Trk::MaterialProperties& Trk::MaterialProperties::operator =( const Trk::MaterialProperties& mprop)
{
  if (this!= &mprop){
      m_material = mprop.m_material;
      m_dInX0    = mprop.m_dInX0;
      m_dInL0    = mprop.m_dInL0;  
      m_zOaTrTd  = mprop.m_zOaTrTd;  
  }
  return(*this);
}

Trk::MaterialProperties& Trk::MaterialProperties::operator *=( float scale )
{
  // assuming rescaling of the material thickness 
  m_dInX0       *= scale;
  m_dInL0       *= scale;
  m_zOaTrTd     *= scale;
  
  return(*this);
}

void Trk::MaterialProperties::addMaterial(const Trk::Material& mat, float dInX0){
  
      //!< @todo update ?
      // averaging factors based on thickness
      float fnew = dInX0*mat.X0/(m_dInX0*m_material.X0+dInX0*mat.X0);
      float fold = 1.-fnew;

      // updated material thickness
      m_dInX0 += dInX0;
      
      // updated material
      m_material = Trk::Material(1./(fnew/mat.X0+fold/m_material.X0),
				 1./(fnew/mat.L0+fold/m_material.L0),
				 fnew*mat.A+fold*m_material.A,
				 fnew*mat.Z+fold*m_material.Z,
				 fnew*mat.rho+fold*m_material.rho);

      // updated derived members
      m_dInL0 = m_dInX0*m_material.X0/m_material.L0;
      m_zOaTrTd = m_material.A > 0 ? m_dInX0*m_material.X0*m_material.Z/m_material.A*m_material.rho : 0;      
}

void Trk::MaterialProperties::setMaterial(const Trk::Material& mat, float thickness) {
    // just overwrite what you have
    m_material = mat;
    m_dInX0    = thickness/mat.X0;
    m_dInL0    = thickness/mat.L0;
    m_zOaTrTd   = mat.Z/mat.A*mat.rho*thickness;
}

void Trk::MaterialProperties::setDeDx(float dEdX)
{
    // set the DE/DX value
    m_material.dEdX = dEdX;
}

MsgStream& Trk::operator << ( MsgStream& sl, const Trk::MaterialProperties& mprop)
{
    sl << "Trk::MaterialProperties: "                                                 << endmsg;
    sl << "   - thickness/X0                          = " << mprop.thicknessInX0()    << endmsg; 
    sl << "   - thickness                       [mm]  = " << mprop.thickness()        << endmsg;
    sl << "   - radiation length X0             [mm]  = " << mprop.x0()               << endmsg;
    sl << "   - nuclear interaction length L0   [mm]  = " << mprop.l0()               << endmsg;
    sl << "   - average material Z/A*rho [gram/mm^3]  = " << mprop.zOverAtimesRho()   << endmsg;
    /*  interface not finalized
    if (mprop.material().composition){
        sl << "   - material composition from " << mprop.material().composition->size() << " elements " << std::endl;
        sl << "       listing them (prob. ordereded ) : " << std::endl;
        for ( auto& eIter : (*mprop.material().composition) )
            sl << "         -> Z : " << eIter.element() << "( fraction : "  <<  eIter.fraction() << " )" << std::endl;
    }
    */
    return sl;
}

std::ostream& Trk::operator << ( std::ostream& sl, const MaterialProperties& mprop)
{ 
    sl << "Trk::MaterialProperties: "                                                 << std::endl;
    sl << "   - thickness/X0                          = " << mprop.thicknessInX0()    << std::endl; 
    sl << "   - thickness                       [mm]  = " << mprop.thickness()        << std::endl;
    sl << "   - radiation length X0             [mm]  = " << mprop.x0()               << std::endl;
    sl << "   - nuclear interaction length L0   [mm]  = " << mprop.l0()               << std::endl;
    sl << "   - average material Z/A*rho [gram/mm^3]  = " << mprop.zOverAtimesRho()   << std::endl;
    /*  interface not finalized
    if (mprop.material().composition){
        sl << "   - material composition from " << mprop.material().composition->size() << " elements " << std::endl;
        sl << "       listing them (prob. ordereded ) : " << std::endl;
        for ( auto& eIter : (*mprop.material().composition) )
            sl << "         -> Z : " << eIter.element() << "( fraction : "  <<  eIter.fraction() << " )" << std::endl;
    }    
    */
    return sl;
}
