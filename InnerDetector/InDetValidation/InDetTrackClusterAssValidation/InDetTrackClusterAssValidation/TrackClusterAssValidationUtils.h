/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TrackClusterAssValidationUtils_H
#define TrackClusterAssValidationUtils_H

namespace InDet {

  /////////////////////////////////////////////////////////////////////////////////
  //  Header file for class Barcode
  /////////////////////////////////////////////////////////////////////////////////

  class Barcode
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////    
    public:

      Barcode()  {};
      Barcode(const Barcode&);
      Barcode(int,int);
      Barcode(int,int,const HepMC::GenParticle*);
      ~Barcode() {};
      Barcode& operator = (const Barcode&);
      int barcode () const {return abs(m_barcharge);}
      int charge  () const {if(m_barcharge<0) return -1; if(m_barcharge>0) return 1; return 0;}
      int rapidity() const {return m_rapidity;}
      const HepMC::GenParticle* particle() const {return m_particle;}

    protected:

      int m_barcharge;
      int m_rapidity;
      const HepMC::GenParticle* m_particle;
    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////
  
  inline Barcode::Barcode(const Barcode& BC)
    {
      *this = BC;
    }
  
  inline Barcode& Barcode::operator = (const Barcode& BC) 
    {
      if(&BC!=this) {
	m_barcharge = BC.m_barcharge;
	m_rapidity  = BC.m_rapidity ;
	m_particle  = BC.m_particle ;
      }
      return(*this);
    }
  inline Barcode::Barcode (int bc,int rap)
    {
      m_barcharge = bc ;
      m_rapidity  = rap;
      m_particle  = 0  ;
    }
  inline Barcode::Barcode (int bc,int rap,const HepMC::GenParticle* pa)
    {
      m_barcharge = bc ;
      m_rapidity  = rap;
      m_particle  = pa ;
    }

}
#endif // TrackClusterAssValidationUtils_H
