/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWHistGlobalSettings                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWHISTGLOBALSETTINGS_H
#define LWHISTGLOBALSETTINGS_H

//Trade-off setting between CPU and Memory
//Set to 16 to disable (eliminating certain code paths, values 9..15 are reasonable settings.
#define FLEXBINCHUNK_CONVERTALLTHRESHOLD 12
//NB: Don't change this without reconsidering what pools we want available!

#endif
