/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// UNFINISHED
// UNFINISHED
// UNFINISHED
// 
// 
// #include "FPTracker/SolenoidElement.h"
// #include "FPTracker/Particle.h"
// #include <sstream>
// #include <cmath>
// #include <iostream>
// 
// namespace FPTracker{
// 
//   SolenoidElement::SolenoidElement(double front,
// 				   double back,
// 				   const TransversePoint& beamAxis,
// 				   Side side,
// 				   double C,
// 				   double S,
// 				   double k
// 				   ):
// 
//     m_frontFace(front), m_rearFace(back),
//     m_signedLength(m_rearFace-m_frontFace),
//     m_absLength(std::fabs(m_signedLength)),
//     m_pos( beamAxis.x(), beamAxis.y(), front+0.5*m_signedLength),
//     m_label("Solenoid"), m_side(side),
//     m_t00(C*C),
//     m_t01(S.C/k),
//     m_t02(SC),
//     m_t03(S*S/k),
//     m_t10(-k*S*C),
//     m_t11(C*C),
//     m_t12(-k*S*S),
//     m_t13(S*C),
//     m_t20(-S*C),
//     m_t21(-S*S/k),
//     m_t22(C*2),
//     m_t23(S*C/k),
//     m_t30(k*S*S),
//     m_t31(-S*C),
//     m_t32(-k*S*C),
//     m_t33(C*C)
//   {
//   } 
// 
//   SolenoidElement::SolenoidElement(const IBeamElement::ConstPtr_t& p, const IBeamElement::ConstPtr_t& c):
//     m_frontFace(p->rearFace()), m_rearFace(c->frontFace()), m_signedLength(m_rearFace-m_frontFace),
//     m_absLength(std::fabs(m_signedLength)),
//     m_pos( p->position() ),
//     m_label("Solenoid"), m_side(c->side()){ 
//   }
// 
//   IBeamElement::ConstPtr_t SolenoidElement::clone() const{
//     IBeamElement::ConstPtr_t  pbe(new SolenoidElement(*this));
//     return pbe;
//   }
// 
//   double SolenoidElement::frontFace()  const {return m_frontFace;}
//   double SolenoidElement::rearFace()   const {return m_rearFace;}
//   std::string SolenoidElement::label() const {return m_label;}
//   double SolenoidElement::zsignedpos() const {return m_frontFace + /*0.5* */m_signedLength;}//Added dpelikan @ 110308
//   double SolenoidElement::zabspos()    const {return std::fabs(this->zsignedpos());}
//   Point SolenoidElement::position()    const {return m_pos;}
//   Side SolenoidElement::side()         const {return m_side;}
//   bool SolenoidElement::isEndElement() const {return false;}
// 
//   void SolenoidElement::track(IParticle& particle) const 
//   {
//     //Added dpelikan @ 110308
//     double TransLength=m_absLength-(std::abs(particle.position().z())-std::abs(m_frontFace));
//     //std::cout<<m_absLength<<"\t"<<TransLength<<std::endl;
//     
//     particle.setBeamCoordinateShift(this);
//     
// 
// 
//     //particle.position()    += m_absLength*particle.direction();
//     particle.position()    += TransLength*particle.direction();
//     particle.position()[2] = m_rearFace;
// 
//   }
// 
//   void SolenoidElement::calibrate(IParticle& particle) const 
//   {
//     return this->track(particle);
//   }
// 
//   std::string SolenoidElement::str() const {
// 
//     std::ostringstream ost;
//     ost <<" \n\n-- SolenoidElement  --\n"
// 	<< "frontFace  "  << m_frontFace <<'\n'
// 	<< "rearFace   "  << m_rearFace  <<'\n';
//     return ost.str();
//   }
// 
//   SolenoidElement::~SolenoidElement(){}
//   /*
//   double SolenoidElement::rearFace() const {return 0.;}
//   double SolenoidElement::frontFace() const {return 0.;}
//   std::string SolenoidElement::label() const {return "";}
//   double SolenoidElement::zsignedpos() const {return 0.;}
//   double SolenoidElement::zabspos()    const {return 0.;}
//   Point SolenoidElement::position()    const {return Point(0., 0., 0.);}
//   int SolenoidElement::side()          const {return 0;}
//   bool SolenoidElement::isEndElement() const {return false;}
//   void SolenoidElement::track(Particle& particle) const {}
// 
//   std::string SolenoidElement::str() const {return " ";}
//   */
// 
// }
// 
