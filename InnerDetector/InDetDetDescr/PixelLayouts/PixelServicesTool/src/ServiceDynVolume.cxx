/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelServicesTool/ServiceDynVolume.h"

#include "PixelServicesTool/ServicesDynLayer.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include <iostream>

void ServiceDynVolume::dump( bool dumpMaterial) const
{
  using namespace std;
  if (shape() == Cylinder) cout << "Cylinder ";
  else if (shape() == Disk) cout << "Disk";
  
  cout << "Radial extent " << m_rMin << "," << m_rMax 
       << " Z extent "     << m_zMin << "," << m_zMax;

  cout << " routes " << layers().size() << " layers" << endl;

  if ( dumpMaterial && !materials().empty()) {
    for (std::vector<ServiceDynMaterial>::const_iterator i=materials().begin(); i!=materials().end(); ++i) {
      cout << "   service material " << i->name() << " has the following components" << endl;
      for ( ServiceDynMaterial::EntryIter ient= i->components().begin(); ient!=i->components().end(); ient++) {
	cout << ient->name << " linear? " << ient->linear 
	     << " number " << ient->number 
	     << " l/w " << ient->weight 
	     << " total l/w " << ient->weight*ient->number << endl;
      }
    }
    cout << endl;
  }
}

void ServiceDynVolume::addEosServices( const ServicesDynLayer* /* l */)
{
  //FIXME: add it back!!!
  /*
  // Here we treat the fixed mass end of stave material as linear material, with mass per mm equal
  // to the EOS mass of one stave (this should be set in the weight table), "length" inside the volume
  // equal to the number of staves in the layer.
  if (l->type() == DetTypeDyn::Pixel) {
    m_materials.push_back( ServiceDynMaterial( "PixelEOS", l->nStaves()));
  }
  else {
    m_materials.push_back( ServiceDynMaterial( "StripEOS", l->nStaves()));
  }
  */
}
