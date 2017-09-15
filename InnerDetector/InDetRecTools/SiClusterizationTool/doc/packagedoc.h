/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SiClusterizationTool_page SiClusterizationTool Package
@author Tommaso.Lari@cern.ch

@section SiClusterizationTool_siClusterizationTool Introduction

This package contains the AlgTools which process 
Silicon Detector Raw Data Objects and creates 
InDetPrepRawData (or clusters).

@section SiClusterizationTool_SiClusterizationToolOverview Overview

These are the classes of the package:

SimplePixelClusteringTool
-------------------
    This is one of the pixel clustering algorithms, 
inherits from PixelClusterToolBase.
    As in MergedPixelsTools, all the pixel cells with a side 
in common are clustered together. 
Depending on the order the Pixel RDos are read, however, two 
pixels with a side in common may belong to different clusters. 
In practice this is a rare occurrence.
    Method clusterize groups the pixels cells of a module in 
clusters. It is called by InDetPrepRawDataFormation PixelClustering
algorithm, once per module. 
   The method makeCluster computes the position associated to 
the cluster, and creates the PixelCluster object. The ClusterMaker
class is called to compute cluster error positions and compute 
global position coordinates, including Lorentz correction.

MergedPixelsTool
----------------
   This is very similar to the SimpleClusteringTool described 
above. However, at the end of clustering algorithm the 
possibility that two clusters may have a pixel side in common
is checked and if this occurs the two clusters are merging, 
removing the dependence of the results of the clustering 
algorithm on the order the Pixel RDOs are read out. This is 
the default clustering tool. 

PixelClusteringToolBase
-----------------------
   Base class for pixel clustering algorithms. Properties which 
can be set from job option are:

GlobalPosAlg: [Default ClusterMakerTool] Algorithm used to 
   compute global position coordinates and cluster position 
   errors, called from clusterMaker method of PixelClustering
   algorithms.
PositionStrategy:
   0 - "digital" position (does not make use of charge information)
   1 - charge interpolation for ATLAS collision data used, 
       except for clusters including ganged pixel cells
  10 - Use special tunings for CTB for charge iterpolation
  20 - Use special tunings for SR1 cosmics data for charge interpolation
ErrorStrategy:
   0 - the error associated to a cluster is the cluster width, divided 
       by the square root of 12. This is VERY conservative, unless one 
       has good reason to think the cluster is anomalous, for example 
       a cluster with a very large size produced by a "delta ray" 
       (energetic secondary ionization electron) 
   1 - The error is the pixel pitch divided by the square root of 12.
       Still slightlyl conservative for most cluster.
   2 - The errror is parametrized as a function of pseudorapidity and 
       cluster size. This is the default.
  10 - A special parametrization for CTB is used, with the error tuned
       for each module and cluster size.

PixelGangedAmbiguitiesFinder
----------------------------
  If two clusters share the same electronics channel because of 
a ganged pixel cell, they are put into the "ambiguities map" 
which flag pairs of clusters connected by the ganged ambiguity.

SCT_Clusterization
------------------
  For each module, adjacent strips are clustered togheter.
Inherits from ISCT_ClusteringTool.
  Strips which are masked as "bad" (noisy or dead) by the 
SCT_ChannelStatusAlg class are removed. This may require 
to split a cluster if surviving channel are no longer 
consecutive; to this purpose the SCT_ReClustering class
is called.
  The errors which are associated to clusters are steered 
by the jobOption flag ErrorStrategy:
  0 - uses the cluster width, divided by the square root of 12.
This is very conservative, but it may be the best option for 
clusters which have an anomalous number of strips (for example, 
large clusters created by a delta-ray or hadronic interaction)
  1 - Uses a parametrization depending on the cluster size (number 
of strips) based on resolution studies. Present parametrization
(implemented in ClusterMakerTool) is 
   1.05*(strip pitch)/sqrt(12.) for one-strip clusters
   0.54*(strip pitch)/sqrt(12.) for two-strip clusters
   cluster width/sqrt(12.) for three-strip or larger clusters

ClusterMakerTool
----------------
  It computes the global coordinates of a (pixel or strip) cluster, 
including Lorentz angle correction, and the error to be associated 
to the cluster, according to the ErrorStrategy flag.
*/
