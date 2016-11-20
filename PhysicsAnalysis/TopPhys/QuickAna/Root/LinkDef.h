/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <QuickAna/Global.h>

#include <QuickAna/Configuration.h>
#include <QuickAna/ELExample.h>
#include <QuickAna/QuickAna.h>
#include <QuickAna/ValidationEL.h>
#include <QuickAna/xAODTruth.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class ana::Configuration+;
#pragma link C++ class ana::ELExample+;
#pragma link C++ class ana::QuickAna+;
#pragma link C++ class ana::ValidationEL+;

#pragma link C++ function ana::getCTruth (StoreType&, const std::string&, bool);
#ifdef ROOTCORE
#pragma link C++ function ana::getCTruth (xAOD::TEvent&, const std::string&, bool);
#endif
#pragma link C++ function ana::getCTruth (asg::AsgTool&, const std::string&, bool allowNull);
#ifdef ROOTCORE
#pragma link C++ function ana::getCTruth (const std::string&, bool allowNull);
#endif

#endif
