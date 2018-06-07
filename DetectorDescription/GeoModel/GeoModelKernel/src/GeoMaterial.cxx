/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD878BB0006.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD878BB0006.cm

//## begin module%3CD878BB0006.cp preserve=no
//## end module%3CD878BB0006.cp

//## Module: GeoMaterial%3CD878BB0006; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoMaterial.cxx

//## begin module%3CD878BB0006.additionalIncludes preserve=no
//## end module%3CD878BB0006.additionalIncludes

//## begin module%3CD878BB0006.includes preserve=yes

#include "CLHEP/Units/PhysicalConstants.h"
#include <stdexcept>
#include <cmath>
#include <numeric>
#include <cfloat>
#include <iostream>
#include <algorithm>
#include <iterator>

//## end module%3CD878BB0006.includes

// GeoMaterial
#include "GeoModelKernel/GeoMaterial.h"
//## begin module%3CD878BB0006.additionalDeclarations preserve=yes

// These constants are the ionization potentials, indexed by atomic     
// number.  They have been obtained from      
//     
// http://physics.nist.gov/PhysRefData/XrayMassCoef/tab1.html     
//     
// Which is the reference of the particle data book. Approximate      
// semiempirical formulae are not accurate enough, so we take the     
// tabular values. These are in electron-volts.     


const double
  GeoMaterial::s_ionizationPotential[93] = {
  0.0 *
    CLHEP::eV,
  19.2 *
    CLHEP::eV,
  41.8 *
    CLHEP::eV,
  40.0 *
    CLHEP::eV,
  63.7 *
    CLHEP::eV,
  76.0 *
    CLHEP::eV,
  78.0 *
    CLHEP::eV,
  82.0 *
    CLHEP::eV,
  95.0 *
    CLHEP::eV,
  115.0 *
    CLHEP::eV,
  137.0 *
    CLHEP::eV,
  149.0 *
    CLHEP::eV,
  156.0 *
    CLHEP::eV,
  166.0 *
    CLHEP::eV,
  173.0 *
    CLHEP::eV,
  173.0 *
    CLHEP::eV,
  180.0 *
    CLHEP::eV,
  174.0 *
    CLHEP::eV,
  188.0 *
    CLHEP::eV,
  190.0 *
    CLHEP::eV,
  191.0 *
    CLHEP::eV,
  216.0 *
    CLHEP::eV,
  233.0 *
    CLHEP::eV,
  245.0 *
    CLHEP::eV,
  257.0 *
    CLHEP::eV,
  272.0 *
    CLHEP::eV,
  286.0 *
    CLHEP::eV,
  297.0 *
    CLHEP::eV,
  311.0 *
    CLHEP::eV,
  322.0 *
    CLHEP::eV,
  330.0 *
    CLHEP::eV,
  334.0 *
    CLHEP::eV,
  350.0 *
    CLHEP::eV,
  347.0 *
    CLHEP::eV,
  348.0 *
    CLHEP::eV,
  343.0 *
    CLHEP::eV,
  352.0 *
    CLHEP::eV,
  363.0 *
    CLHEP::eV,
  366.0 *
    CLHEP::eV,
  379.0 *
    CLHEP::eV,
  393.0 *
    CLHEP::eV,
  417.0 *
    CLHEP::eV,
  424.0 *
    CLHEP::eV,
  428.0 *
    CLHEP::eV,
  441.0 *
    CLHEP::eV,
  449.0 *
    CLHEP::eV,
  470.0 *
    CLHEP::eV,
  470.0 *
    CLHEP::eV,
  469.0 *
    CLHEP::eV,
  488.0 *
    CLHEP::eV,
  488.0 *
    CLHEP::eV,
  487.0 *
    CLHEP::eV,
  485.0 *
    CLHEP::eV,
  491.0 *
    CLHEP::eV,
  482.0 *
    CLHEP::eV,
  488.0 *
    CLHEP::eV,
  491.0 *
    CLHEP::eV,
  501.0 *
    CLHEP::eV,
  523.0 *
    CLHEP::eV,
  535.0 *
    CLHEP::eV,
  546.0 *
    CLHEP::eV,
  560.0 *
    CLHEP::eV,
  574.0 *
    CLHEP::eV,
  580.0 *
    CLHEP::eV,
  591.0 *
    CLHEP::eV,
  614.0 *
    CLHEP::eV,
  628.0 *
    CLHEP::eV,
  650.0 *
    CLHEP::eV,
  658.0 *
    CLHEP::eV,
  674.0 *
    CLHEP::eV,
  684.0 *
    CLHEP::eV,
  694.0 *
    CLHEP::eV,
  705.0 *
    CLHEP::eV,
  718.0 *
    CLHEP::eV,
  727.0 *
    CLHEP::eV,
  736.0 *
    CLHEP::eV,
  746.0 *
    CLHEP::eV,
  757.0 *
    CLHEP::eV,
  790.0 *
    CLHEP::eV,
  790.0 *
    CLHEP::eV,
  800.0 *
    CLHEP::eV,
  810.0 *
    CLHEP::eV,
  823.0 *
    CLHEP::eV,
  823.0 *
    CLHEP::eV,
  830.0 *
    CLHEP::eV,
  825.0 *
    CLHEP::eV,
  794.0 *
    CLHEP::eV,
  827.0 *
    CLHEP::eV,
  826.0 *
    CLHEP::eV,
  841.0 *
    CLHEP::eV,
  847.0 *
    CLHEP::eV,
  878.0 *
    CLHEP::eV,
  890.0 *
  CLHEP::eV
};


//## end module%3CD878BB0006.additionalDeclarations


// Class GeoMaterial 

//## begin GeoMaterial::s_lastID%3CD8833A01EB.attr preserve=no  private: static unsigned int {U} 0
unsigned int GeoMaterial::s_lastID = 0;
//## end GeoMaterial::s_lastID%3CD8833A01EB.attr

GeoMaterial::GeoMaterial (const std::string &Name, double Density):
  //## begin GeoMaterial::GeoMaterial%3CDA10C001E4.hasinit preserve=no
  m_name(Name),
  m_density(Density),
  m_iD(s_lastID++),
  m_radLength(0),
  m_intLength(0),
  m_dedDxConst(0),
  m_deDxI0(0),
  m_locked(false)
  //## end GeoMaterial::GeoMaterial%3CDA10C001E4.hasinit
  //## begin GeoMaterial::GeoMaterial%3CDA10C001E4.initialization preserve=yes
  //## end GeoMaterial::GeoMaterial%3CDA10C001E4.initialization
{
  //## begin GeoMaterial::GeoMaterial%3CDA10C001E4.body preserve=yes
  //## end GeoMaterial::GeoMaterial%3CDA10C001E4.body
}


GeoMaterial::~GeoMaterial()
{
  //## begin GeoMaterial::~GeoMaterial%3CD878BB0006_dest.body preserve=yes
  for (size_t i = 0; i < m_element.size (); i++)
    {
      m_element[i]->unref ();
    }
  //## end GeoMaterial::~GeoMaterial%3CD878BB0006_dest.body
}



//## Other Operations (implementation)
void GeoMaterial::add (GeoElement* element, double fraction)
{
  //## begin GeoMaterial::add%3CDA0E1D02A2.body preserve=yes
  // You can only add materials until you call "lock"...     
  if (!m_locked)
    {
      std::vector <GeoElement *>::iterator e = std::find(m_element.begin(),m_element.end(),element);
      if (e==m_element.end()) {
	m_element.push_back (element);
	m_fraction.push_back (fraction);
	element->ref ();
      }
      else {
	int n = e-m_element.begin();
	m_fraction[n]+=fraction;
      }
    }
  else
    {
      throw std::out_of_range ("Element added after material locked");
    }
  //## end GeoMaterial::add%3CDA0E1D02A2.body
}

void GeoMaterial::add (GeoMaterial* material, double fraction)
{
  //## begin GeoMaterial::add%3CDA0E2003E7.body preserve=yes
  if (!m_locked)
    {
      for (size_t e = 0; e < material->getNumElements (); e++)
	{
	  add(material->m_element[e],fraction * material->m_fraction[e]);
	}
    }
  else
    {
      throw std::out_of_range ("Material added after material locked");
    }
  //## end GeoMaterial::add%3CDA0E2003E7.body
}

void GeoMaterial::lock ()
{
  //## begin GeoMaterial::lock%3CDA0E250362.body preserve=yes
  if(m_locked) return;

  m_locked = true;

  // -------------------------------------------//     
  // Now compute some quantities:               //     
  // Source of these calculations is:           //     
  //                                            //     
  // For DeDx constants and X0:  PDG.           //     
  // For Lambda0:                G4.            //     
  //                                            //     
  // For energy loss                            //     
  //                                            //     
  const double C0 = 0.00307 * CLHEP::cm3 / CLHEP::gram;	//     
  //                                            //     
  // For nuclear absorption length.             //     
  const double lambda0 = 35 * CLHEP::gram / CLHEP::cm2;	//     
  //                                            //     
  //--------------------------------------------//     

  if (getNumElements () == 0)
    {
      throw std::out_of_range ("Attempt to lock a material with no elements");
      return;
    }


  //--------------------------------------------//     
  //                                            //     
  // -------------------------------------------//     

  double dEDxConstant = 0, dEDxI0 = 0, NILinv = 0.0, radInv = 0.0;

  { // ===============Renormalization================================
    double wSum=std::accumulate(m_fraction.begin(),m_fraction.end(),0.0);
    if (fabs(wSum-1.0)>FLT_EPSILON) { 
      std::cerr << "Warning in material " 
		<< m_name 
		<< ". Mass fractions sum to "      
	        << wSum << "; renormalizing to 1.0" << std::endl;
    }
    double inv_wSum = 1. / wSum;
    for (size_t e=0;e<getNumElements();e++) {m_fraction[e]*=inv_wSum;}
  } // ==============================================================

  const double inv_lambda0 = 1. / lambda0;
  for (size_t e = 0; e < getNumElements (); e++)
    {
      double w = getFraction (e);	// Weight fraction.     
      double Z = m_element[e]->getZ ();	// Atomic number     
      double A = m_element[e]->getA ();	// Atomic mass.     
      double N = m_element[e]->getN ();	// Number of nucleons.     
      double dovera = m_density ? m_density / A : 0; // don't crash if both are 0
      double n = m_fraction[e] * CLHEP::Avogadro * dovera;	// Number density.
      int iZ = (int) (m_element[e]->getZ () + 0.5) - 1;	// Atomic number index     

      dEDxConstant += w * C0 * dovera * Z;
      // Make sure we don't overflow the table:
      // the `Ether' special `element' has Z set to 500.
      if (iZ >= 0 && iZ < (std::end(s_ionizationPotential)-std::begin(s_ionizationPotential)))
        dEDxI0 += w * s_ionizationPotential[iZ];
      NILinv += n * pow (N, 2.0 / 3.0) * CLHEP::amu * inv_lambda0;

      double nAtomsPerVolume = A ? CLHEP::Avogadro*m_density*m_fraction[e]/A : 0.;
      radInv += (nAtomsPerVolume*m_element[e]->getRadTsai());
    }
  m_dedDxConst = dEDxConstant;
  m_deDxI0    = dEDxI0 ;
  m_intLength = NILinv ? 1.0 / NILinv : 0;
  m_radLength = radInv ? 1.0 / radInv : 0;
  //## end GeoMaterial::lock%3CDA0E250362.body
}

double GeoMaterial::getDeDxConstant () const
{
  //## begin GeoMaterial::getDeDxConstant%3CDA0E3F026F.body preserve=yes
  if (!m_locked)
    throw std::out_of_range ("Material accessed before lock");
  return m_dedDxConst;
  //## end GeoMaterial::getDeDxConstant%3CDA0E3F026F.body
}

double GeoMaterial::getDeDxI0 () const
{
  //## begin GeoMaterial::getDeDxI0%3CDA0E4A021B.body preserve=yes
  if (!m_locked)
    throw std::out_of_range ("Material accessed before lock");
  return m_deDxI0;
  //## end GeoMaterial::getDeDxI0%3CDA0E4A021B.body
}

double GeoMaterial::getDeDxMin () const
{
  //## begin GeoMaterial::getDeDxMin%3CDA0E5801D5.body preserve=yes

  //------------------------------------------------------------//     
  static const double ConstToMin = 11.528;	//     
  //------------------------------------------------------------//     

  if (!m_locked)
    throw std::out_of_range ("Material accessed before lock");

  // -----------------------------------------------------------//     
  // See:  Paul Avery's notes on track fitting, CBX 92-39.      //     
  // Good for typical materials                                 //     
  // -----------------------------------------------------------//     

  return m_dedDxConst * ConstToMin;


  //## end GeoMaterial::getDeDxMin%3CDA0E5801D5.body
}

double GeoMaterial::getRadLength () const
{
  //## begin GeoMaterial::getRadLength%3CDAF57E03B0.body preserve=yes
  if (!m_locked)
    throw std::out_of_range ("Material accessed before lock");
  return m_radLength;
  //## end GeoMaterial::getRadLength%3CDAF57E03B0.body
}

double GeoMaterial::getIntLength () const
{
  //## begin GeoMaterial::getIntLength%3CDAF58501C6.body preserve=yes
  if (!m_locked)
    throw std::out_of_range ("Material accessed before lock");
  return m_intLength;

  //## end GeoMaterial::getIntLength%3CDAF58501C6.body
}

unsigned int GeoMaterial::getNumElements () const
{
  //## begin GeoMaterial::getNumElements%3CDA103F00DA.body preserve=yes
  if (!m_locked)
    throw std::out_of_range ("Material accessed before lock");
  return m_element.size ();
  //## end GeoMaterial::getNumElements%3CDA103F00DA.body
}

const GeoElement* GeoMaterial::getElement (unsigned int i) const
{
  //## begin GeoMaterial::getElement%3CDA104F0304.body preserve=yes
  if (!m_locked)
    throw std::out_of_range ("Material accessed before lock");
  return m_element[i];
  //## end GeoMaterial::getElement%3CDA104F0304.body
}

double GeoMaterial::getFraction (int i) const
{
  //## begin GeoMaterial::getFraction%3CDB2237001D.body preserve=yes
  if (!m_locked)
    throw std::out_of_range ("Material accessed before lock");
  return m_fraction[i];
  //## end GeoMaterial::getFraction%3CDB2237001D.body
}

// Additional Declarations
  //## begin GeoMaterial%3CD878BB0006.declarations preserve=yes
  //## end GeoMaterial%3CD878BB0006.declarations

//## begin module%3CD878BB0006.epilog preserve=yes
//## end module%3CD878BB0006.epilog
