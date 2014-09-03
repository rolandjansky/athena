/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage CaloEvent Package

This package provides event data model classes for Calorimeter 
reconstruction.


@author David Rousseau rousseau@lal.in2p3.fr
@author many many others

@section CaloEventCaloCell CaloCell related classes

CaloCell is the input data class for all calorimeter reconstruction 
algorithms. CaloCells are stored in a CaloCellContainer. They can 
be compactified into a CaloCompactCell in a CaloCompactCellContainer.
 
@section CaloEventCluster Clustering related classes

The principal data classes for composite calorimeter reconstruction objects are
 CaloTower and  CaloCluster . Both classes provide the same basic features
in that they both have a four-vector representation and a list of  CaloCell s
attached. The differences are that  CaloTower s are defined on a regular
\f$ \Delta \eta \times \Delta \phi \f$ grid, while  CaloCluster s generally
represent signal (energy) blobs any where in the calorimeters, even across
sub-detector boundaries. 

 CaloTower and  CaloCluster are stored in the  CaloTowerContainer and
the  CaloClusterContainer , respectively. 

@section CaloEventOther Other classes




*/
