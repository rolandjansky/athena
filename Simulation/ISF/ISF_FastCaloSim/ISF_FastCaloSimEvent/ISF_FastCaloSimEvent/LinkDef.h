/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/DoubleArray.h"
#include "ISF_FastCaloSimEvent/IntArray.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunctionHistogram.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunctionRegression.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunctionRegressionTF.h"
#include "ISF_FastCaloSimEvent/TFCSEnergyParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include "ISF_FastCaloSimEvent/TFCSParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"
#include "ISF_FastCaloSimEvent/TFCSPCAEnergyParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"

#ifdef __CINT__
#pragma link C++ class DoubleArray+;
#pragma link C++ class IntArray+;
#pragma link C++ class TFCS1DFunction+;
#pragma link C++ class TFCS1DFunctionHistogram+;
#pragma link C++ class TFCS1DFunctionRegression+;
#pragma link C++ class TFCS1DFunctionRegressionTF+;
#pragma link C++ class TFCSEnergyParametrization+;
#pragma link C++ class TFCSExtrapolationState+;
#pragma link C++ class TFCSParametrization+;
#pragma link C++ class TFCSParametrizationBase+;
#pragma link C++ class TFCSPCAEnergyParametrization+;
#pragma link C++ class TFCSSimulationState+;
#pragma link C++ class TFCSTruthState+;
#endif


