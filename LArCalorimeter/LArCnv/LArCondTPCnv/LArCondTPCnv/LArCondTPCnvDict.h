/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARCONDTPCNVDICT_H
#define LARCONDTPCNV_LARCONDTPCNVDICT_H 
/**
 * @file LArCondTPCnvDict.h
 *
 * @brief Header file for dictionary generation
 *
 * @author  <R.D.Schaffer@cern.ch>
 */

#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p1.h"
#include "LArCondTPCnv/LArCaliWaveSubset_p1.h"
#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p2.h"
#include "LArCondTPCnv/LArCaliWaveSubset_p2.h"
#include "LArCondTPCnv/LArPhysWaveSubsetCnv_p1.h"
#include "LArCondTPCnv/LArPhysWaveSubset_p1.h"
#include "LArCondTPCnv/LArAutoCorrSubsetCnv_p1.h"
#include "LArCondTPCnv/LArAutoCorrSubset_p1.h"
#include "LArCondTPCnv/LArOFCSubsetCnv_p1.h"
#include "LArCondTPCnv/LArOFCSubset_p1.h"
#include "LArCondTPCnv/LArPedestalMCCnv_p1.h"
#include "LArCondTPCnv/LArPedestalMC_p1.h"
#include "LArCondTPCnv/LArPedestalSubsetCnv_p1.h"
#include "LArCondTPCnv/LArPedestalSubset_p1.h"
#include "LArCondTPCnv/LArRampSubsetCnv_p1.h"
#include "LArCondTPCnv/LArRampSubset_p1.h"
#include "LArCondTPCnv/LArShapeSubsetCnv_p1.h"
#include "LArCondTPCnv/LArShapeSubsetCnv_p2.h"
#include "LArCondTPCnv/LArShapeSubset_p1.h"
#include "LArCondTPCnv/LArSingleFloatSubsetCnv_p1.h"
#include "LArCondTPCnv/LArSingleFloatSubset_p1.h"
#include "LArCondTPCnv/LArOFCBinSubsetCnv_p1.h"
#include "LArCondTPCnv/LArSingleIntSubset_p1.h"
#include "LArCondTPCnv/LArMphysOverMcalSubset_p1.h"
#include "LArCondTPCnv/LArMphysOverMcalSubsetCnv_p1.h"
#include "LArCondTPCnv/LArDSPThresholdsSubsetCnv_p1.h"
#include "LArCondTPCnv/LArPedestalSubsetCnv_p2.h"
#include "LArCondTPCnv/LArPedestalSubset_p2.h"


struct dummy {
    T_TPCnv<LArConditionsSubset<LArAutoCorrP1>, 
            LArAutoCorrSubset_p1 >                m_autocorr_p1;
    T_TPCnv<LArConditionsSubset<LArOFCP1>, 
            LArOFCSubset_p1 >                     m_ofc_p1;
    T_TPCnv<LArPedestalMC, 
            LArPedestalMC_p1 >                    m_pedestalmc_p1;
    T_TPCnv<LArConditionsSubset<LArPedestalP1>, 
            LArPedestalSubset_p1 >                m_pedestal_p1;
    T_TPCnv<LArConditionsSubset<LArRampP1>, 
            LArRampSubset_p1 >                    m_ramp_p1;

    T_TPCnv<LArConditionsSubset<LArShapeP1>, 
            LArShapeSubset_p1 >                   m_shape_p1;

    T_TPCnv<LArConditionsSubset<LArShapeP2>, 
            LArShapeSubset_p2 >                   m_shape_p2;

    T_TPCnv<LArConditionsSubset<LArMphysOverMcalP1>,
  	    LArMphysOverMcalSubset_p1 >           m_MphysOverMcal_p1;

    T_TPCnv<LArConditionsSubset<LArSingleFloatP>,
	    LArSingleFloatSubset_p1 >             m_singleFloat_p1;

    T_TPCnv<LArConditionsSubset<LArOFCBinP>,
	    LArSingleIntSubset_p1 >                  m_ofcbin_p1;

    T_TPCnv<LArConditionsSubset<LArCaliWaveVec>,	LArCaliWaveSubset_p1 >  m_caliwave_p1;
    T_TPCnv<LArConditionsSubset<LArCaliWaveVec>,	LArCaliWaveSubset_p2 >  m_caliwave_p2;
    T_TPCnv<LArConditionsSubset<LArPhysWave>,  		LArPhysWaveSubset_p1 >  m_physwave_p1;

    T_TPCnv<LArConditionsSubset<LArDSPThresholdsP>,  LArDSPThresholdsSubset_p1 >  m_dspthr_p1;




    T_TPCnv<LArConditionsSubset<LArPedestalP1>, 
            LArPedestalSubset_p2 >                m_pedestal_p2;


};

#endif // LARCONDTPCNV_LARCONDTPCNVDICT_H

