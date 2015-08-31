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
  GeoMaterial::_ionizationPotential[93] = {
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

//## begin GeoMaterial::_lastID%3CD8833A01EB.attr preserve=no  private: static unsigned int {U} 0
unsigned int GeoMaterial::_lastID = 0;
//## end GeoMaterial::_lastID%3CD8833A01EB.attr

GeoMaterial::GeoMaterial (const std::string &Name, double Density):
  //## begin GeoMaterial::GeoMaterial%3CDA10C001E4.hasinit preserve=no
  name(Name),
  density(Density),
  iD(_lastID++),
  _radLength(0),
  _intLength(0),
  _deDxConst(0),
  _deDxI0(0),
  _locked(false)
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
  for (size_t i = 0; i < _element.size (); i++)
    {
      _element[i]->unref ();
    }
  //## end GeoMaterial::~GeoMaterial%3CD878BB0006_dest.body
}



//## Other Operations (implementation)
void GeoMaterial::add (GeoElement* element, double fraction)
{
  //## begin GeoMaterial::add%3CDA0E1D02A2.body preserve=yes
  // You can only add materials until you call "lock"...     
  if (!_locked)
    {
      std::vector <GeoElement *>::iterator e = std::find(_element.begin(),_element.end(),element);
      if (e==_element.end()) {
	_element.push_back (element);
	_fraction.push_back (fraction);
	element->ref ();
      }
      else {
	int n = e-_element.begin();
	_fraction[n]+=fraction;
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
  if (!_locked)
    {
      for (size_t e = 0; e < material->getNumElements (); e++)
	{
	  add(material->_element[e],fraction * material->_fraction[e]);
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
  if(_locked) return;

  _locked = true;

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
    double wSum=std::accumulate(_fraction.begin(),_fraction.end(),0.0);
    if (fabs(wSum-1.0)>FLT_EPSILON) { 
      std::cerr << "Warning in material " 
		<< name 
		<< ". Mass fractions sum to "      
	        << wSum << "; renormalizing to 1.0" << std::endl;
    }
    double inv_wSum = 1. / wSum;
    for (size_t e=0;e<getNumElements();e++) {_fraction[e]*=inv_wSum;}
  } // ==============================================================

  const double inv_lambda0 = 1. / lambda0;
  for (size_t e = 0; e < getNumElements (); e++)
    {
      double w = getFraction (e);	// Weight fraction.     
      double Z = _element[e]->getZ ();	// Atomic number     
      double A = _element[e]->getA ();	// Atomic mass.     
      double N = _element[e]->getN ();	// Number of nucleons.     
      double dovera = density ? density / A : 0; // don't crash if both are 0
      double n = _fraction[e] * CLHEP::Avogadro * dovera;	// Number density.
      int iZ = (int) (_element[e]->getZ () + 0.5) - 1;	// Atomic number index     

      dEDxConstant += w * C0 * dovera * Z;
      dEDxI0 += w * _ionizationPotential[iZ];
      NILinv += n * pow (N, 2.0 / 3.0) * CLHEP::amu * inv_lambda0;

      double nAtomsPerVolume = A ? CLHEP::Avogadro*density*_fraction[e]/A : 0.;
      radInv += (nAtomsPerVolume*_element[e]->getRadTsai());
    }
  _deDxConst = dEDxConstant;
  _deDxI0    = dEDxI0 ;
  _intLength = NILinv ? 1.0 / NILinv : 0;
  _radLength = radInv ? 1.0 / radInv : 0;
  //## end GeoMaterial::lock%3CDA0E250362.body
}

double GeoMaterial::getDeDxConstant () const
{
  //## begin GeoMaterial::getDeDxConstant%3CDA0E3F026F.body preserve=yes
  if (!_locked)
    throw std::out_of_range ("Material accessed before lock");
  return _deDxConst;
  //## end GeoMaterial::getDeDxConstant%3CDA0E3F026F.body
}

double GeoMaterial::getDeDxI0 () const
{
  //## begin GeoMaterial::getDeDxI0%3CDA0E4A021B.body preserve=yes
  if (!_locked)
    throw std::out_of_range ("Material accessed before lock");
  return _deDxI0;
  //## end GeoMaterial::getDeDxI0%3CDA0E4A021B.body
}

double GeoMaterial::getDeDxMin () const
{
  //## begin GeoMaterial::getDeDxMin%3CDA0E5801D5.body preserve=yes

  //------------------------------------------------------------//     
  static const double ConstToMin = 11.528;	//     
  //------------------------------------------------------------//     

  if (!_locked)
    throw std::out_of_range ("Material accessed before lock");

  // -----------------------------------------------------------//     
  // See:  Paul Avery's notes on track fitting, CBX 92-39.      //     
  // Good for typical materials                                 //     
  // -----------------------------------------------------------//     

  return _deDxConst * ConstToMin;


  //## end GeoMaterial::getDeDxMin%3CDA0E5801D5.body
}

double GeoMaterial::getRadLength () const
{
  //## begin GeoMaterial::getRadLength%3CDAF57E03B0.body preserve=yes
  if (!_locked)
    throw std::out_of_range ("Material accessed before lock");
  return _radLength;
  //## end GeoMaterial::getRadLength%3CDAF57E03B0.body
}

double GeoMaterial::getIntLength () const
{
  //## begin GeoMaterial::getIntLength%3CDAF58501C6.body preserve=yes
  if (!_locked)
    throw std::out_of_range ("Material accessed before lock");
  return _intLength;

  //## end GeoMaterial::getIntLength%3CDAF58501C6.body
}

unsigned int GeoMaterial::getNumElements () const
{
  //## begin GeoMaterial::getNumElements%3CDA103F00DA.body preserve=yes
  if (!_locked)
    throw std::out_of_range ("Material accessed before lock");
  return _element.size ();
  //## end GeoMaterial::getNumElements%3CDA103F00DA.body
}

const GeoElement* GeoMaterial::getElement (unsigned int i) const
{
  //## begin GeoMaterial::getElement%3CDA104F0304.body preserve=yes
  if (!_locked)
    throw std::out_of_range ("Material accessed before lock");
  return _element[i];
  //## end GeoMaterial::getElement%3CDA104F0304.body
}

double GeoMaterial::getFraction (int i) const
{
  //## begin GeoMaterial::getFraction%3CDB2237001D.body preserve=yes
  if (!_locked)
    throw std::out_of_range ("Material accessed before lock");
  return _fraction[i];
  //## end GeoMaterial::getFraction%3CDB2237001D.body
}

// Additional Declarations
  //## begin GeoMaterial%3CD878BB0006.declarations preserve=yes
  //## end GeoMaterial%3CD878BB0006.declarations

//## begin module%3CD878BB0006.epilog preserve=yes
//## end module%3CD878BB0006.epilog
