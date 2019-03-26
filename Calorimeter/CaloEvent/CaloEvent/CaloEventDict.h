/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calo Rec Event
 -----------------------------------------
 ***************************************************************************/


#ifndef CALOEVENT_CALOEVENTDICT_H
# define CALOEVENT_CALOEVENTDICT_H

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloEvent/CaloShowerContainer.h"
#include "CaloEvent/CaloCompactCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloEvent/CaloTopoTowerContainer.h"
#include "CaloEvent/CaloTower.h"

#include "CaloEvent/CaloEnergyCluster.h"
#include "CaloEvent/CaloClusterBadChannelData.h"

#include "AthLinks/ElementLinkVector.h" 

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"

#include "CaloEvent/CaloClusterCellLinkContainer.h"

namespace CaloDict{
//NavTowerToken towerToken; 
//NavCellToken  cellToken; 

//
// struct temp { 
//  ElementLink<CaloCellContainer> m_link;
//  std::pair<const ElementLink<CaloCellContainer>*,double>  m_pair; 
// };
//
// change in default container (PL April 21, 2004)

  /** \brief dummy variables for dictionary generation 
   */
  struct temp {
    std::vector<CaloClusterBadChannelData>               m_vccbcd;
    ElementLinkVector<CaloCellContainer>                 m_elv;
    SG::ELVRef<CaloCellContainer>                        m_elvref; 
    DataLink<CaloCellContainer>                          m_dl;

    ElementLink<CaloCellContainer>                       m_link;
    std::pair<ElementLink<CaloCellContainer>,double>     m_pair;
    ElementLink<CaloClusterContainer>                    m_link2;
    std::pair<ElementLink<CaloClusterContainer>,double>  m_pair2;

    ElementLink<CaloCellLinkContainer>                   m_link3;
    std::pair<ElementLink<CaloCellLinkContainer>,double> m_pair3;

    ElementLink<CaloShowerContainer>                     m_link4;
    std::pair<ElementLink<CaloShowerContainer>,double>   m_pair4;

    std::pair<unsigned int,double>                       m_pair5;

    Navigable<CaloCellLinkContainer,double>              m_navCellLink;
    Navigable<CaloClusterContainer,double>               m_navCluster;

    Navigable<CaloCellContainer,double>::object_iter     m_navCellIter;

    DataModel_detail::ElementProxy<DataVector<CaloCell> >     m_elproxy_1;
    DataModel_detail::ElementProxy<DataVector<CaloShower> >   m_elproxy_2;
    DataModel_detail::ElementProxy<DataVector<CaloCellLink> > m_elproxy_3;
    DataModel_detail::ElementProxy<DataVector<CaloCluster> >  m_elproxy_4;
    DataModel_detail::ElementProxy<DataVector<CaloTower> >    m_elproxy_5;

    //    CaloCluster::moments_pair m_im_pair;
    //    CaloClusterMomentStore::moment_pair 


    std::vector<const CaloCell*> m_v_c_cc;
    std::vector<std::vector<const CaloCell*> > m_v_v_cc;

    ElementLink<CaloClusterCellLinkContainer>  m_link5;
    std::vector<ElementLink<CaloClusterCellLinkContainer> > m_linkVec5;

  private:
    // Don't try to make the default ctor for this --- ElementProxy doesn't
    // have a default ctor.
    temp();
  };

}

#endif 
