/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/ServiceVolume.h"

#include "InDetServMatGeoModel/ServicesLayer.h"

#include "GeoModelKernel/Units.h"
#include <iostream>

void ServiceVolume::dump( bool dumpMaterial) const
{
  using namespace std;
  if (shape() == Cylinder) cout << "Cylinder ";
  else if (shape() == Disk) cout << "Disk";
  
  cout << "Radial extent " << m_rMin << "," << m_rMax 
       << " Z extent "     << m_zMin << "," << m_zMax;

  cout << " routes " << layers().size() << " layers" << endl;

  if ( dumpMaterial && !materials().empty()) {
    for (std::vector<ServiceMaterial>::const_iterator i=materials().begin(); i!=materials().end(); ++i) {
      cout << "   service material " << i->name() << " has the following components" << endl;
      for ( ServiceMaterial::EntryIter ient= i->components().begin(); ient!=i->components().end(); ++ient) {
	cout << ient->name << " linear? " << ient->linear 
	     << " number " << ient->number 
	     << " l/w " << ient->weight 
	     << " total l/w " << ient->weight*ient->number << endl;
      }
    }
    cout << endl;
  }
}

void ServiceVolume::addEosServices( const ServicesLayer* /* l */)
{
  //FIXME: add it back!!!
  /*
  // Here we treat the fixed mass end of stave material as linear material, with mass per mm equal
  // to the EOS mass of one stave (this should be set in the weight table), "length" inside the volume
  // equal to the number of staves in the layer.
  if (l->type() == DetType::Pixel) {
    m_materials.push_back( ServiceMaterial( "PixelEOS", l->nStaves()));
  }
  else {
    m_materials.push_back( ServiceMaterial( "StripEOS", l->nStaves()));
  }
  */
}
