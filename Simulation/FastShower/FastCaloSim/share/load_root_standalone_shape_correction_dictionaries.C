// This root script creates a shared library that can be used to write lateral shape correction objects into .root files
// * copy/symlink load_root_standalone_shape_correction_dictionaries.C to a run directory
// * symlink the FastCaloSim header and src directories to the run directory
// * execute ".L load_root_standalone_shape_correction_dictionaries.C+" on the root command prompt
//   or
//   execute "gROOT->LoadMacro("load_root_standalone_shape_correction_dictionaries.C+");" in a root script
// * Create shape correction objects
// * Write all shape correction objects into the subdirectory "ShapeCorrection" of a .root file

#include "FastCaloSim/TLateralShapeCorrection.h"
#include "FastCaloSim/TSplineReweight.h"
#include "FastCaloSim/TCellReweight.h"
#include "src/TLateralShapeCorrection.cxx"
#include "src/TSplineReweight.cxx"
#include "src/TCellReweight.cxx"

#ifdef __CINT__
#pragma link C++ class TLateralShapeCorrectionBase+;
#pragma link C++ class TLateralShapeCorrection+;
#pragma link C++ class TSplineReweight+;
#pragma link C++ class TEtaSplineReweight+;
#pragma link C++ class TPhiSplineReweight+;
#pragma link C++ class TRSplineReweight+;
#pragma link C++ class TMetricEtaSplineReweight+;
#pragma link C++ class TMetricPhiSplineReweight+;
#pragma link C++ class TMetricRSplineReweight+;
#pragma link C++ class THitCellReweight+;
#endif
