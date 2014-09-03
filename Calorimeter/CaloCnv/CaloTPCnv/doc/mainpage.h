/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage CaloTPCnv Package

This package provides POOL persistent representation for Calo event data classes and the T/P converters. 

The event data are in CaloEvent package.  The transient data classes are CaloCell and CaloCluster. 

Calo event data classes are T/P separated, and the POOL Converters are in Calorimeter/CaloCnv/CaloAthenaPool package. 

@author RD Schaffer  <R.D.Schaffer@cern.ch>

@section CaloCell  ---- CaloCell persistency
The persistent CaloCellContainer is CaloCompactCellContainer in CaloEvent. The compatification is defined by CaloCompactCellTool in CaloTools package, and used by the POOL converter.  No persistent data or tool defined here for CaloCell in this package. 

@section CaloCluster   ---- CaloCluster  persistency
CaloCluster consists of Links to CaloCell and CaloShower (objects in event store)  and two split stores, CaloClusterMement and CaloSamplingData (contained within CaloCluster).  Each of these has persistent data structure:

CaloCellLinkContainer_p2

CaloClusterContainer_p3

CaloClusterMomentContainer_p2 and CaloClusterMomentStore_p1

CaloSamplingDataContainer_p1 and CaloSamplingData_p1

CaloShowerContainer_p2

@section CaloTower    ---- CaloTower persistency
CaloTowerContainer contains the towers and the tower segmentation. Only the Tower segmentation has the persistent form, the towers are recreated from cells on the fly using tower builder tools.  This means CaloCellContainer has to be present in the event data store, if CaloTowerContainer is needed.

CaloTowerContainer_p1 and CaloTowerSeg_p1

@section Converters  ---- T/P converter classes 

CaloCellLinkContainerCnv_p2

CaloClusterContainerCnv_p3

CaloClusterMomentContainerCnv_p1 CaloClusterMomentStoreCnv_p1

CaloShowerContainerCnv_p2

CaloTowerContainerCnv_p1 CaloTowerSegCnv_p1

*/
