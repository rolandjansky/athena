/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileGeoCutBuilder.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileDddbManager.h"
#include "TileDetDescr/TileDetDescriptor.h"
#include "TileDetDescr/TileCellDim.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoIdentifierTag.h"

#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "GeoModelKernel/GeoXF.h"
#include "GeoModelKernel/GeoSerialTransformer.h"

#include "GaudiKernel/MsgStream.h"

#include <stdexcept>
#include <iostream>
#include <assert.h>

TileGeoCutBuilder::TileGeoCutBuilder(DataHandle<StoredMaterialManager> & /*matManager*/,
				     TileDddbManager * /*pDbManager*/,
                                     MsgStream * log)
  : m_log(log)
{
}

TileGeoCutBuilder::~TileGeoCutBuilder()
{
}

void TileGeoCutBuilder::MakeCut(GeoPhysVol*&   mother, 
			        int            number)
{ // overlap precision
  /*
  double rless =.150; // 150 [mkm]

  // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  // Obtain required materials - Air and Iron
  const GeoMaterial* matAir = theMaterialManager->getMaterial("std::Air");
  const GeoMaterial* matIron = theMaterialManager->getMaterial("std::Iron");
  */

  GeoTransform* tfTmp = new GeoTransform(HepGeom::Translate3D(0.,0.,0.));
  mother->add(tfTmp);

  (*m_log) << MSG::INFO <<"  MakeCut : number= "<<number<<endmsg;
}


// Checking geometry dimensions for all direction

void TileGeoCutBuilder::checking(std::string Name, bool print, int level,
                                 double X1, double X2, double Y1, double Y2, double Z) 
{
  double rless = .005; // 5 [mkm]
  std::string Step[8] = {" ",
                        "  ",
                        "   ",
                        "    ",
                        "     ",
                        "      ",
                        "       ",
                        "        "};
  if (print)
   {
     if(m_log->level()<=MSG::DEBUG)
       (*m_log) << MSG::DEBUG <<Step[level]<<Name<<"-"<<level
		<<" dX1,dX2= "<<X1<<","<<X2<<" dY1,dY2= "<<Y1<<","<<Y2<<",dZ= "<<Z
		<<endmsg;
   }
  if (X1 < rless && X2 < rless)
   { (*m_log) << MSG::WARNING <<" volume "<<Name<<" is empty, X1 or X2<0 "<<endmsg;
   }
  if (Y1 < rless && Y2 < rless)
   { (*m_log) << MSG::WARNING <<" volume "<<Name<<" is empty, Y1 or Y2<0 "<<endmsg;
   }
  if (Z < rless)
   { (*m_log) << MSG::WARNING <<" volume "<<Name<<" is empty, Z<0   "<<endmsg;
   } 
}

