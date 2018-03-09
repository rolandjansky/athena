/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/DoubleArray.h"
#include "ISF_FastCaloSimEvent/IntArray.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunctionHistogram.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunctionRegression.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunctionRegressionTF.h"

#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"
#include "ISF_FastCaloSimEvent/TFCSParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSInitWithEkin.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationChain.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationBinnedChain.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationFloatSelectChain.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationPDGIDSelectChain.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationEbinChain.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationEkinSelectChain.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationEtaSelectChain.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationAbsEtaSelectChain.h"

#include "ISF_FastCaloSimEvent/TFCSEnergyParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSPCAEnergyParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSEnergyBinParametrization.h"

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"
#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitChain.h"
#include "ISF_FastCaloSimEvent/TFCSHistoLateralShapeParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitNumberFromE.h"
#include "ISF_FastCaloSimEvent/TFCSHitCellMapping.h"
#include "ISF_FastCaloSimEvent/TFCSHitCellMappingWiggleEMB.h"

#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

#ifdef __CINT__
#pragma link C++ class DoubleArray+;
#pragma link C++ class IntArray+;
#pragma link C++ class TFCS1DFunction+;
#pragma link C++ class TFCS1DFunctionHistogram+;
#pragma link C++ class TFCS1DFunctionRegression+;
#pragma link C++ class TFCS1DFunctionRegressionTF+;

#pragma link C++ class TFCSParametrizationBase+;
#pragma link C++ class TFCSParametrization+;
#pragma link C++ class TFCSInitWithEkin+;
#pragma link C++ class TFCSParametrizationChain+;
#pragma link C++ class TFCSParametrizationBinnedChain+;
#pragma link C++ class TFCSParametrizationFloatSelectChain+;
#pragma link C++ class TFCSParametrizationPDGIDSelectChain+;
#pragma link C++ class TFCSParametrizationEbinChain+;
#pragma link C++ class TFCSParametrizationEkinSelectChain+;
#pragma link C++ class TFCSParametrizationEtaSelectChain+;
#pragma link C++ class TFCSParametrizationAbsEtaSelectChain+;

#pragma link C++ class TFCSEnergyParametrization+;
#pragma link C++ class TFCSPCAEnergyParametrization+;
#pragma link C++ class TFCSEnergyBinParametrization+;

#pragma link C++ class TFCSLateralShapeParametrization+;
#pragma link C++ class TFCSLateralShapeParametrizationHitBase+;
#pragma link C++ class TFCSLateralShapeParametrizationHitChain+;
#pragma link C++ class TFCSHistoLateralShapeParametrization+;
#pragma link C++ class TFCSLateralShapeParametrizationHitNumberFromE+;
#pragma link C++ class TFCSHitCellMapping+;
#pragma link C++ class TFCSHitCellMappingWiggleEMB+;

#pragma link C++ class TFCSTruthState+;
#pragma link C++ class TFCSExtrapolationState+;
#pragma link C++ class TFCSSimulationState+;
#endif


