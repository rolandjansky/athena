/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calorimeter LVL1 TT  description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloTTDescrRegion.h,v 1.5 2008-12-13 07:25:15 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef CALODETDESCR_CALOTTDESCRREGION_H
#define CALODETDESCR_CALOTTDESCRREGION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "Identifier/Identifiable.h"
#include "Identifier/Identifier.h"
#include "CaloTTDetDescr/CaloTTDescriptor.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
*
*  @brief This class provides an interface to the geometrical description of the Calorimeter Trigger Towers

*  Each instance of a CaloTTDescrRegion corresponds to the description of a Trigger Tower. <p>
*  It is an interface, the real geometrical information is held by the CaloTTDescriptors.
*
*  Information available: <br>
*  ---------------------- <br>
*<br>
*    Identifier: Each CaloTTDescrRegion has an Identifier which
*    uniquely identifies to which region it corresponds, i.e. which
*    positive/negative half, sampling, and region number (see
*    class Calo_LVL1ID in package CaloIdentifier for more info.). <br>
*<br>
*    Print: A general print method is available. <p>
*
*/
class CaloTTDescrRegion : public Identifiable
{
public:
    
    typedef CaloTTDescriptor		descriptor_type;

    /** default constructor */
    CaloTTDescrRegion(void);
    /** constructor */
    CaloTTDescrRegion(const Identifier& id, 
		      const descriptor_type* descriptor);
    
    /**  */
    virtual Identifier		identify	() const;
    
    const descriptor_type* 	descriptor	() const;

    /** set coordinates of the Det Descr region -- pseudo spherical system; eta is signed, rho is unsigned */
    void set_spherical ( double     eta, 
			 double     phi, 
			 double     rho );

    /** set size of the Det Descr region -- pseudo spherical system; eta is signed, rho is unsigned */
    void set_spherical_size ( double     deta, 
			      double     dphi,  
			      double     drho  );

    /** set coordinates of the Det Descr region -- pseudo cylindrical system; eta is signed, z as well (same sign) */
    void set_cylindric ( double     eta, 
			 double     phi, 
			 double     z );

    /** set size of the Det Descr region -- pseudo cylindrical system; eta is signed, z as well (same sign) */
    void set_cylindric_size ( double     deta, 
			      double     dphi,  
			      double     dz  );

    /** set coordinates of the Det Descr region -- cartesian system; all signed */
    void set_cartesian ( double     x, 
			 double     y, 
			 double     z );

    /** set size of the Det Descr region -- cartesian system; all signed */
    void set_cartesian_size ( double     dx, 
			      double     dy,
			      double     dz );


    /** return coordinateof the Det Descr Region: eta (signed) */
    double eta ()    const ;
    /** return coordinateof the Det Descr Region: phi */
    double phi ()    const ;
    /** return coordinateof the Det Descr Region: rho (unsigned) */
    double rho ()    const ;
    /** return size in eta (unsigned)*/
    double dphi ()    const ;
    /** return size in phi (unsigned)*/
    double deta ()    const ;
    /** return size in rho (unsigned)*/
    double drho ()    const ;

    /** return coordinateof the Det Descr Region: x (signed) */
    double x()       const ;
    /** return coordinateof the Det Descr Region: y (signed) */
    double y()       const ;
    /** return coordinateof the Det Descr Region: z (signed) */
    double z()       const ;
    /** return size in x (unsigned)*/
    double dx()       const ;
    /** return size in y (unsigned)*/
    double dy()       const ;
    /** return size in z (unsigned)*/
    double dz()       const ;

    void 			print		() const;

private:
    
    Identifier			m_id;
    
    const descriptor_type* 	m_descriptor;
    
    // spherical coordinates :
    double      m_eta       ;
    double      m_phi       ;
    double      m_rho       ;
    double      m_deta      ;
    double      m_dphi      ;
    double      m_drho      ;

    // cartesian coordinates :
    double      m_x         ;
    double      m_y         ;
    double      m_z         ;
    double      m_dx        ;
    double      m_dy        ;
    double      m_dz        ;

    
};



inline
CaloTTDescrRegion::CaloTTDescrRegion(void)
    :
  m_descriptor(0),
  m_eta(0.),
  m_phi(0.),
  m_rho(0.),
  m_deta(0.),
  m_dphi(0.),
  m_drho(0.),
  m_x(0.),
  m_y(0.),
  m_z(0.),
  m_dx(0.),
  m_dy(0.),
  m_dz(0.)

{}

inline
CaloTTDescrRegion::CaloTTDescrRegion(const Identifier& id, 
	     const descriptor_type* descriptor)
    : m_id(id),
      m_descriptor(descriptor),
      m_eta(0.),
      m_phi(0.),
      m_rho(0.),
      m_deta(0.),
      m_dphi(0.),
      m_drho(0.),
      m_x(0.),
      m_y(0.),
      m_z(0.),
      m_dx(0.),
      m_dy(0.),
      m_dz(0.)
{}

inline const CaloTTDescrRegion::descriptor_type* 	
CaloTTDescrRegion::descriptor	() const
{
    return m_descriptor;
}


inline double CaloTTDescrRegion::eta() const
{ return m_eta;}
inline double CaloTTDescrRegion::phi() const
{ return m_phi;}
inline double CaloTTDescrRegion::rho() const
{ return m_rho;}
inline double CaloTTDescrRegion::deta() const
{ return m_deta;}
inline double CaloTTDescrRegion::dphi() const
{ return m_dphi;}
inline double CaloTTDescrRegion::drho() const
{ return m_drho;}

inline double CaloTTDescrRegion::x() const
{ return m_x;}
inline double CaloTTDescrRegion::y() const
{ return m_y;}
inline double CaloTTDescrRegion::z() const
{ return m_z;}
inline double CaloTTDescrRegion::dx() const
{ return m_dx;}
inline double CaloTTDescrRegion::dy() const
{ return m_dy;}
inline double CaloTTDescrRegion::dz() const
{ return m_dz;}

#endif // CALODETDESCR_CALOTTDESCRREGION_H
