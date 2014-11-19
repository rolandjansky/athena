/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARFEBENERGY_H
#define LARFEBENERGY_H

/**
   @class LArFebEnergy
   @brief Holds partial energy sums Ex,Ey,Ez for each Feb

   The LArFebEnergy is a simple base class which holds the partial energy sums Ex, 
   Ey, Ez which are calculated for each FEB. The data members can be set and 
   accessed through public methods.
   
   @author Ignacio Aracena
   
*/

class LArFebEnergy 
{
  
 public:
  
  /** @brief main constructor with all data members set to zero */
  LArFebEnergy();
    
  /** @brief constructor; initialize all data members in constructor */
  LArFebEnergy( unsigned int febid, double ex, double ey, double ez, double sume );      

  /** @brief constructor for a given FebId */
  LArFebEnergy( unsigned int febid );
  
  /** @brief get FebId (data member) */
  unsigned int getFebId() const{ return m_febId; }

  /** @brief get FebEx (data member) */
  double getFebEx() const{ return m_ex; }

  /** @brief get FebEy (data member) */
  double getFebEy() const{ return m_ey; }

  /** @brief get FebEz (data member) */
  double getFebEz() const{ return m_ez; }
  
  /** @brief get FebSumE (data member) */
  double getFebSumE() const{ return m_sumE; }
  
  /** @brief set FebEx (data member) */
  void setFebEx( const double enex ) { m_ex = enex; }

  /** @brief set FebEy (data member) */
  void setFebEy( const double eney ) { m_ey = eney; }

  /** @brief set FebEz (data member) */
  void setFebEz( const double enez ) { m_ez = enez; }

  /** @brief set FebSumE (data member) */
  void setFebSumE( const double sume ) { m_sumE = sume; }

 private:

  /** @brief Feb identifier */
  unsigned int m_febId;

  /** @brief Feb Ex */
  double m_ex;

  /** @brief Feb Ey */
  double m_ey;

  /** @brief Feb Ez */
  double m_ez;

  /** @brief Feb SumE */
  double m_sumE;

};

inline LArFebEnergy::LArFebEnergy()
  : m_febId(0) , m_ex(0.) , m_ey(0.), m_ez(0.), m_sumE(0.){} 

inline LArFebEnergy::LArFebEnergy( unsigned int febid, double ex, double ey, double ez, double sume ) 
  : m_febId(febid), m_ex(ex), m_ey(ey), m_ez(ez), m_sumE(sume){}

inline LArFebEnergy::LArFebEnergy( unsigned int febid ) 
  : m_febId(febid) , m_ex(0.) , m_ey(0.), m_ez(0.), m_sumE(0.) {}

#endif //LARFEBENERGY_H
