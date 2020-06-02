/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloAlignTool.h"
#include "CaloDetDescr/CaloDepthTool.h"
#include "CaloDetDescr/CaloDescriptors.h"
#include "CaloDetDescr/CaloSubdetNames.h"
#include "CaloDetDescr/ICaloCoordinateTool.h"
#include "CaloDetDescr/ICaloRecoMaterialTool.h"
#include "CaloDetDescr/ICaloRecoSimpleGeomTool.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"


struct CaloDetDescrDict
{
  CaloDetDescrManager::calo_descr_const_iterator m_rit;
  CaloDetDescrManager::calo_descr_range m_rrange;

  CaloDetDescrManager::calo_element_const_iterator m_eit;
  CaloDetDescrManager::calo_element_range m_ebrange;
};

namespace CaloDetDescrDictInstan
{
  bool ritdum (CaloDetDescrManager::calo_descr_const_iterator a,
               CaloDetDescrManager::calo_descr_const_iterator b)
  { return a==b || a!=b; }
  bool eitdum (CaloDetDescrManager::calo_element_const_iterator a,
               CaloDetDescrManager::calo_element_const_iterator b)
  { return a==b || a!=b; }
}
