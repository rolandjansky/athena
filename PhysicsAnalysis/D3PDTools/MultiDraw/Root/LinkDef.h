/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <MultiDraw/AlgCFlow.h>
#include <MultiDraw/AlgHist.h>
#include <MultiDraw/Formula.h>
#include <MultiDraw/FormulaSvc.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace MD;
#pragma link C++ class MD::AlgCFlow+;
#pragma link C++ class MD::AlgHist+;
#pragma link C++ class MD::Formula+;
#pragma link C++ class MD::FormulaSvc+;

#pragma link C++ function MD::formulas (EL::IWorker*);
#pragma link C++ function MD::useFormulas (EL::Job&);

#endif
