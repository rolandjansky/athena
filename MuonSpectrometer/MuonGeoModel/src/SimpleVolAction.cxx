/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 a GeoVolumeAction
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: SimpleVolAction.cxx,v 1.3 2006-04-10 17:12:17 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGeoModel/SimpleVolAction.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

namespace MuonGM {

SimpleVolAction::SimpleVolAction ()
    :GeoVolumeAction (GeoVolumeAction::TOP_DOWN)
{
}

SimpleVolAction::~SimpleVolAction ()
{
}

void SimpleVolAction::handleVPhysVol (const GeoVPhysVol *pv) 
{
    //// here comes my job
    //   std::cout<<" MY VOL Action is handlying a GeoVPhysVol "<<std::endl;
    const GeoLogVol* lv = pv->getLogVol();
    std::string name = lv->getName();
    const GeoMaterial* mat = lv->getMaterial();
    unsigned int nchild = pv->getNChildVols();

    std::cout<<" *** Tree Navigation *** in "<<name
             <<" made of "<<mat->getName()
             <<" N.of Children "<<nchild<<std::endl;
    
    for (unsigned int i=0 ; i < nchild; ++i)
    {
        std::cout<<" ... child n. "<<i<<" named "<<pv->getNameOfChildVol(i)<<std::endl;
    }    
}


} // namespace MuonGM
