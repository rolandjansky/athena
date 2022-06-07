
/***********************************************************************
* Copyright 1998-2022 CERN for the benefit of the EvtGen authors       *
*                                                                      *
* This file is part of EvtGen.                                         *
*                                                                      *
* EvtGen is free software: you can redistribute it and/or modify       *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation, either version 3 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
* EvtGen is distributed in the hope that it will be useful,            *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU General Public License for more details.                         *
*                                                                      *
* You should have received a copy of the GNU General Public License    *
* along with EvtGen.  If not, see <https://www.gnu.org/licenses/>.     *
***********************************************************************/

#include "EvtGen_i/EvtGenExternal/EvtExternalGenFactory.hh"

#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtReport.hh"

#include "EvtGen_i/EvtGenExternal/EvtPythiaEngine.hh"

#include "EvtGen_i/EvtGenExternal/EvtPhotosEngine.hh"

#include "EvtGen_i/EvtGenExternal/EvtTauolaEngine.hh"

#include <iostream>
using std::endl;

EvtExternalGenFactory::EvtExternalGenFactory()
{
    m_extGenMap.clear();
}

EvtExternalGenFactory::~EvtExternalGenFactory()
{
    ExtGenMap::iterator iter;
    for ( iter = m_extGenMap.begin(); iter != m_extGenMap.end(); ++iter ) {
        EvtAbsExternalGen* theGenerator = iter->second;
        delete theGenerator;
    }

    m_extGenMap.clear();
}

EvtExternalGenFactory* EvtExternalGenFactory::getInstance()
{
    static EvtExternalGenFactory* theFactory = 0;

    if ( theFactory == 0 ) {
        theFactory = new EvtExternalGenFactory();
    }

    return theFactory;
}

void EvtExternalGenFactory::definePythiaGenerator( std::string xmlDir,
                                                   bool convertPhysCodes,
                                                   bool useEvtGenRandom )
{
    int genId = EvtExternalGenFactory::PythiaGenId;

    EvtGenReport( EVTGEN_INFO, "EvtGen" )
        << "Defining EvtPythiaEngine: data tables defined in " << xmlDir << endl;
    if ( convertPhysCodes == true ) {
        EvtGenReport( EVTGEN_INFO, "EvtGen" )
            << "Pythia 6 codes in decay files will be converted to Pythia 8 codes"
            << endl;
    } else {
        EvtGenReport( EVTGEN_INFO, "EvtGen" )
            << "Pythia 8 codes need to be used in decay files" << endl;
    }

    if ( useEvtGenRandom == true ) {
        EvtGenReport( EVTGEN_INFO, "EvtGen" )
            << "Using EvtGen random engine for Pythia 8 as well" << endl;
    }

    EvtAbsExternalGen* pythiaGenerator =
        new EvtPythiaEngine( xmlDir, convertPhysCodes, useEvtGenRandom );
    m_extGenMap[genId] = pythiaGenerator;
}

void EvtExternalGenFactory::definePhotosGenerator( std::string photonType,
                                                   bool useEvtGenRandom )
{
    int genId = EvtExternalGenFactory::PhotosGenId;

    EvtGenReport( EVTGEN_INFO, "EvtGen" )
        << "Defining EvtPhotosEngine using photonType = " << photonType << endl;

    EvtAbsExternalGen* photosGenerator = new EvtPhotosEngine( photonType,
                                                              useEvtGenRandom );
    m_extGenMap[genId] = photosGenerator;
}


void EvtExternalGenFactory::defineTauolaGenerator( bool useEvtGenRandom )
{
    int genId = EvtExternalGenFactory::TauolaGenId;

    EvtGenReport( EVTGEN_INFO, "EvtGen" ) << "Defining EvtTauolaEngine." << endl;

    EvtAbsExternalGen* tauolaGenerator = new EvtTauolaEngine( useEvtGenRandom );
    m_extGenMap[genId] = tauolaGenerator;
}

EvtAbsExternalGen* EvtExternalGenFactory::getGenerator( int genId )
{
    EvtAbsExternalGen* theGenerator( 0 );

    ExtGenMap::iterator iter;

    if ( ( iter = m_extGenMap.find( genId ) ) != m_extGenMap.end() ) {
        // Retrieve the external generator engine
        theGenerator = iter->second;

    } else {
        EvtGenReport( EVTGEN_INFO, "EvtGen" )
            << "EvtAbsExternalGen::getGenerator: could not find generator for genId = "
            << genId << endl;
    }

    return theGenerator;
}

void EvtExternalGenFactory::initialiseAllGenerators()
{
    ExtGenMap::iterator iter;
    for ( iter = m_extGenMap.begin(); iter != m_extGenMap.end(); ++iter ) {
        EvtAbsExternalGen* theGenerator = iter->second;
        if ( theGenerator != 0 ) {
            theGenerator->initialise();
        }
    }
}
