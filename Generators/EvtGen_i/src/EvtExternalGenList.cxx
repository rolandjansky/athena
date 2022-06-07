
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

#include "EvtGen_i/EvtGenExternal/EvtExternalGenList.hh"

#include "EvtGen_i/EvtGenExternal/EvtExternalGenFactory.hh"
#include "EvtGen_i/EvtGenExternal/EvtPHOTOS.hh"
#include "EvtGen_i/EvtGenExternal/EvtPythia.hh"
#include "EvtGen_i/EvtGenExternal/EvtTauola.hh"

#include "EvtGen_i/EvtGenExternal/Belle2/EvtB0toKsKK.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtBCL.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtBGL.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtBSemiTauonic.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtBSemiTauonic2HDMType2.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtEtaFullDalitz.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtEtaPi0Dalitz.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtEtaPrimeDalitz.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtHQET3.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtLLSW.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtPHSPBMix.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtYmSToYnSpipiCLEOboost.h"

using namespace Belle2;

EvtExternalGenList::EvtExternalGenList( bool convertPythiaCodes,
                                        std::string pythiaXmlDir,
                                        std::string photonType,
                                        bool useEvtGenRandom )
{
    // Instantiate the external generator factory
    EvtExternalGenFactory* extFactory = EvtExternalGenFactory::getInstance();

    // Define the external generator "engines" here
    extFactory->definePhotosGenerator( photonType, useEvtGenRandom );

    if ( pythiaXmlDir.size() < 1 ) {
        // If we have no string defined, check the value of the
        // PYTHIA8DATA environment variable which should be set to the
        // xmldoc Pythia directory
        char* pythiaDataDir = getenv( "PYTHIA8DATA" );
        if ( pythiaDataDir != 0 ) {
            pythiaXmlDir = pythiaDataDir;
        }
    }

    extFactory->definePythiaGenerator( pythiaXmlDir, convertPythiaCodes,
                                       useEvtGenRandom );

    extFactory->defineTauolaGenerator( useEvtGenRandom );
}

EvtExternalGenList::~EvtExternalGenList()
{
}

EvtAbsRadCorr* EvtExternalGenList::getPhotosModel()
{
    // Define the Photos model, which uses the EvtPhotosEngine class.
    EvtPHOTOS* photosModel = new EvtPHOTOS();
    return photosModel;
}

std::list<EvtDecayBase*> EvtExternalGenList::getListOfModels()
{
    // Create the Pythia and Tauola models, which use their own engine classes.
    EvtPythia* pythiaModel = new EvtPythia();
    EvtTauola* tauolaModel = new EvtTauola();
  EvtB0toKsKK* evtB0toKsKK = new EvtB0toKsKK();
  EvtBCL* evtBCL = new EvtBCL();
  EvtBGL* evtBGL = new  EvtBGL();
  EvtBSemiTauonic*  evtBSemiTauonic  = new  EvtBSemiTauonic();
  EvtBSemiTauonic2HDMType2* evtBSemiTauonic2HDMType2  = new  EvtBSemiTauonic2HDMType2();
  EvtEtaFullDalitz* evtEtaFullDalitz  = new  EvtEtaFullDalitz();
  EvtEtaPi0Dalitz* evtEtaPi0Dalitz  = new  EvtEtaPi0Dalitz();
  EvtEtaPrimeDalitz* evtEtaPrimeDalitz  = new  EvtEtaPrimeDalitz();
  EvtHQET3*  evtHQET3  = new  EvtHQET3();
  EvtLLSW* evtLLSW  = new  EvtLLSW();
  EvtPHSPBMix* evtPHSPBMix  = new  EvtPHSPBMix();
  EvtYmSToYnSpipiCLEOboost* evtYmSToYnSpipiCLEOboost  = new EvtYmSToYnSpipiCLEOboost();

  std::list<EvtDecayBase*> extraModels;
  extraModels.push_back(pythiaModel);
  extraModels.push_back(tauolaModel);
  extraModels.push_back(evtB0toKsKK);
  extraModels.push_back(evtBCL);
  extraModels.push_back(evtBGL);
  extraModels.push_back(evtBSemiTauonic);
  extraModels.push_back(evtBSemiTauonic2HDMType2);
  extraModels.push_back(evtEtaFullDalitz);
  extraModels.push_back(evtEtaPi0Dalitz);
  extraModels.push_back(evtEtaPrimeDalitz);
  extraModels.push_back(evtHQET3);
  extraModels.push_back(evtLLSW);
  extraModels.push_back(evtPHSPBMix);
  extraModels.push_back(evtYmSToYnSpipiCLEOboost);


    //    std::list<EvtDecayBase*> extraModels;
    //    extraModels.push_back( pythiaModel );
    //    extraModels.push_back( tauolaModel );

    // Return the list of models
    return extraModels;
}
