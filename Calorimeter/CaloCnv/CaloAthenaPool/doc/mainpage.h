/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage CaloAthenaPool Package

This package provides POOL persistency event data classes.

The event data are in CaloEvent package.  The data classes are 
CaloCell and CaloCluster. 

Calo event data classes are T/P separated, and the persistent data are in 
Calorimeter/CaloCnv/CaloTPCnv package. 

@author Walter Lampl <walter.lampl@cern.ch>
@author RD Schaffer  <R.D.Schaffer@cern.ch>

@section CaloCell  ---- CaloCell persistency
The persistent CaloCell is CaloCompactCell in CaloEvent. The compatification is defined by CaloCompactCellTool in CaloTools package, and used by the POOL converter.

@section CaloCluster   ---- CaloCluster  persistency
CaloCluster consists of Links to CaloCell and CaloShower (objects in event store)  and two split stores, CaloClusterMement and CaloSamplingData (contained within CaloCluster).  Each of these has persistent data structure. 

@section CaloTower    ---- CaloTower persistency
CaloTowerContainer contains the towers and the tower segmentation. Only the Tower segmentation has the persistent form, the towers are recreated from cells on the fly using tower builder tools.  This means CaloCellContainer has to be present in the event data store, if CaloTowerContainer is needed.

@section Converters  ---- POOL Converters 

CaloCellContainerCnv

CaloCellLinkContainerCnv

CaloClusterContainerCnv

CaloShowerContainerCnv

CaloTowerContainerCnv

*/
