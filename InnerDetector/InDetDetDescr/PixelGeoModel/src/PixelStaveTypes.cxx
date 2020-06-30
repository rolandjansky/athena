/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelStaveTypes.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"

#include "RDBAccessSvc/IRDBRecordset.h"

#include <iostream>
#include <algorithm>

// Class to store information from PixelStaveTypes and allow easy retrieval.


const PixelStaveTypes::Datum PixelStaveTypes::s_defaultDatum;

PixelStaveTypes::Datum::Datum(int fluidType_in, int biStaveType_in)
: fluidType(fluidType_in),
  biStaveType(biStaveType_in)
{}      

PixelStaveTypes::Key::Key(int layer_in, int phiModule_in)
  : layer(layer_in),
    phiModule(phiModule_in)
{}      

bool
PixelStaveTypes::Key::operator<(const PixelStaveTypes::Key &rhs) const
{
  return 
    ((layer < rhs.layer) ||
     (layer == rhs.layer && phiModule < rhs.phiModule));
}


PixelStaveTypes::PixelStaveTypes(const IGeometryDBSvc * db, IRDBRecordset_ptr table)
{
  for (unsigned int i = 0; i < db->getTableSize(table); i++) {
    int layer = db->getInt(table,"LAYER",i);
    int phiModule = db->getInt(table,"SECTOR",i);
    int fluidType = db->getInt(table,"FLUIDTYPE",i);
    int biStaveType = db->getInt(table,"BISTAVETYPE",i);
    
    m_dataLookup[Key(layer,phiModule)] = Datum(fluidType,biStaveType);
    m_maxSector[layer] = std::max(phiModule, m_maxSector[layer]); // Store the max sector for each layer. 
  }
}

int
PixelStaveTypes::getFluidType(int layer, int phiModule) const
{
  return getData(layer, phiModule).fluidType;
}

int
PixelStaveTypes::getBiStaveType(int layer, int phiModule) const
{
  return getData(layer, phiModule).biStaveType;
}


const PixelStaveTypes::Datum &
PixelStaveTypes::getData(int layer, int phiModule) const
{
  int layerTmp = layer;

  // If no entries for layer, use layer 0. 
  std::map<int,int>::const_iterator iterMaxSector;
  iterMaxSector =  m_maxSector.find(layerTmp);
  if (iterMaxSector == m_maxSector.end() && layerTmp) {
    layerTmp = 0; 
    iterMaxSector =  m_maxSector.find(layerTmp);
  }

  if (iterMaxSector != m_maxSector.end()) {
    int maxSector = iterMaxSector->second;
    // if phiModule is greater than max Sector in table then assume pattern repeats.
    // NB This is not the case as the pattern is not very regular and there is an entery
    // for each layer and phi sector.
    int phiModuleTmp = phiModule % (maxSector+1);

    MapType::const_iterator iter;
    iter = m_dataLookup.find(Key(layerTmp, phiModuleTmp));
    
    if (iter != m_dataLookup.end()) return iter->second;
  }

  std::cout << "ERROR: PixelStaveTypes cannot find type for layer,phiModule: " << layer << ", " << phiModule << std::endl;
  return s_defaultDatum;
}

