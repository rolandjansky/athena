/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloTopoTowerContainer.cxx
PACKAGE:  offline/Calorimeter/CaloRec 

AUTHORS:  S. Rajagopalan
CREATED:  Oct 29, 2000

PURPOSE:  
          
Updated:  Jan 3, 2001    (HM)
          QA. 

Updated:  Feb 5, 2001    (HMa)
          Moved from LArClusterRec to CaloRec. 


********************************************************************/

#include "CaloEvent/CaloTopoTowerContainer.h"



//////////////////////////////
// Constructors/Desctructor //
//////////////////////////////

CaloTopoTowerContainer::CaloTopoTowerContainer()
  : m_celltoclustermap(nullptr),
    m_minimumCellEnergy(0),
    m_minimumClusterEnergy (0),
    m_useCellWeights (false),
    m_useNoiseTool (false),
    m_usePileUpNoise (false),
    m_noiseSigma (0),
    m_cellESignificanceThreshold (0),
    m_caloSelection (false)
{
}

CaloTopoTowerContainer::CaloTopoTowerContainer(const CaloTowerSeg& theSegmentation,
                                               bool noTowers /*= false*/)
  : CaloTowerContainer(theSegmentation, noTowers),
    m_celltoclustermap(nullptr),
    m_minimumCellEnergy(0),
    m_minimumClusterEnergy (0),
    m_useCellWeights (false),
    m_useNoiseTool (false),
    m_usePileUpNoise (false),
    m_noiseSigma (0),
    m_cellESignificanceThreshold (0),
    m_caloSelection (false)
{
}

CaloTopoTowerContainer::CaloTopoTowerContainer
  (const CaloTopoTowerContainer& other)
    : DataVector<INavigable4Momentum> (other),
      CaloTowerContainer           (other),
      m_clusters                   (other.m_clusters),
      m_towers                     (other.m_towers),
      m_cells                      (other.m_cells),
      m_celltoclustermap           (other.m_celltoclustermap),
      m_minimumCellEnergy          (other.m_minimumCellEnergy),
      m_minimumClusterEnergy       (other.m_minimumClusterEnergy),
      m_useCellWeights             (other.m_useCellWeights),
      m_useNoiseTool               (other.m_useNoiseTool),
      m_usePileUpNoise             (other.m_usePileUpNoise),
      m_noiseSigma                 (other.m_noiseSigma),
      m_cellESignificanceThreshold (other.m_cellESignificanceThreshold),
      m_caloIndices                (other.m_caloIndices),
      m_caloSelection              (other.m_caloSelection)
{
}

CaloTopoTowerContainer&
CaloTopoTowerContainer::operator= (const CaloTopoTowerContainer& other)
{
  if (this != &other) {
    *static_cast<CaloTowerContainer*>(this) = other;
    m_clusters                   = other.m_clusters;
    m_towers                     = other.m_towers;
    m_cells                      = other.m_cells;
    m_celltoclustermap           = other.m_celltoclustermap;
    m_minimumCellEnergy          = other.m_minimumCellEnergy;
    m_minimumClusterEnergy       = other.m_minimumClusterEnergy;
    m_useCellWeights             = other.m_useCellWeights;
    m_useNoiseTool               = other.m_useNoiseTool;
    m_usePileUpNoise             = other.m_usePileUpNoise;
    m_noiseSigma                 = other.m_noiseSigma;
    m_cellESignificanceThreshold = other.m_cellESignificanceThreshold;
    m_caloIndices                = other.m_caloIndices;
    m_caloSelection              = other.m_caloSelection;
  }
  return *this;
}

void CaloTopoTowerContainer::swap (CaloTopoTowerContainer& other)
{
  if (this != &other) {
    CaloTowerContainer::swap (other);
    std::swap (m_clusters,                  other.m_clusters);
    std::swap (m_towers,                    other.m_towers);
    std::swap (m_cells,                     other.m_cells);
    std::swap (m_celltoclustermap,          other.m_celltoclustermap);
    std::swap (m_minimumCellEnergy,         other.m_minimumCellEnergy);
    std::swap (m_minimumClusterEnergy,      other.m_minimumClusterEnergy);
    std::swap (m_useCellWeights,            other.m_useCellWeights);
    std::swap (m_useNoiseTool,              other.m_useNoiseTool);
    std::swap (m_usePileUpNoise,            other.m_usePileUpNoise);
    std::swap (m_noiseSigma,                other.m_noiseSigma);
    std::swap (m_cellESignificanceThreshold,other.m_cellESignificanceThreshold);
    std::swap (m_caloSelection,             other.m_caloSelection);
    m_caloIndices.swap                     (other.m_caloIndices);
  }
}

CaloTopoTowerContainer::~CaloTopoTowerContainer()
{ }

