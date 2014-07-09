/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARONLDBPREP_LARONLDBPREPDICT_H
#define LARONLDBPREP_LARONLDBPREPDICT_H

#include "LArRawConditions/LArConditionsSubset.h"

#include "LArOnlDbPrep/LArOnlCalibDataContainer.h"
#include "LArOnlDbPrep/LArOnlEnergyDataContainer.h"
#include "LArOnlDbPrep/ILArOnlDbPrepTool.h"
#include "LArOnlDbPrep/LArOnlDbPrepTool.h"


namespace LArOnlDbPrepDict 
{

    LArConditionsSubset<LArOnlCalibData>        m_subset1;

    LArConditionsSubset<LArOnlCalibData>::CorrectionPair        m_pair1;

    LArConditionsSubset<LArOnlCalibData>::ConstCorrectionVecIt        m_pairit11;

    LArConditionsContainer<LArOnlCalibData>::ConstCorrectionIt        m_pairit111;

    LArConditionsContainer<LArOnlCalibData>::ConditionsMap        m_map11;

    LArConditionsSubset<LArOnlCalibData>::FebPair        m_pair101;



    LArConditionsSubset<LArOnlEnergyData>        m_subset2;

    LArConditionsSubset<LArOnlEnergyData>::CorrectionPair        m_pair2;

    LArConditionsSubset<LArOnlEnergyData>::ConstCorrectionVecIt        m_pairit12;

    LArConditionsContainer<LArOnlEnergyData>::ConstCorrectionIt        m_pairit112;

    LArConditionsContainer<LArOnlEnergyData>::ConditionsMap        m_map12;

    LArConditionsSubset<LArOnlEnergyData>::FebPair        m_pair102;
 
    ILArOnlDbPrepTool* m_itool                          ;
    LArOnlDbPrepTool* m_tool                          ;

}


#endif
