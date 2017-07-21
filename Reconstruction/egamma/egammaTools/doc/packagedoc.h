/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page egammaTools_page egammaTools Package

This package contains electron and photon reconstruction and identification tools. These tools are called by main algorithms located in the egammaRec package.

Following tools are used by egammaBuilder for the reconstruction of isolated high-pT electrons and photons (THE LIST IS OUT OF DATE) :

egammaSwTool : creation of the cluster

EMClusterTool : define cluster size depending on type of object 

EMClusterErrorsTool : associate error on cluster quantities

EMShowerBuilder : Calculation of the EM shower shapes. 

EMIsolationBuilder : It calls specialized tools located in egammaCaloTools package. When called from an analysis alg or tool it applies a special treatement to retrieve the original shower object to keep the "calorimetric isolation" variables which cannot be recalculated (due to lack of cells)

EMTrackMatchBuilder : performs matching between cluster and a track

EMPIDBuilder : performs identification of the object. It calls specialized tools located in egammaPID package

EMConversionBuilder : retrieve and match conversion objects to an egamma one

EMBremsstrahlungBuilder : retrieve bremsstrahlung. It calls specialized tools BremsstrahlungHit and BremsstrahlungHelix

EMTrkRefitter : retrieve bremsstrahlung. It calls specialized tools like DNA and GSF.

EMConversionRefitter : Refits the conversion tracks with dedicated e+- track fitters (like GSF and DNA) and refits the vertex (rebuilds the vertex from the first measurement in case of single-track conversions). Does not change the original tracks/vertices, but supplies the results in an egDetail.

EMFourMomBuilder : build the 4-momentum of the candidates

Following tools are called by softebuilder for the reconstruction of low-pT and non-isoalted electrons :

EMClusterBuilderFromTrackSETool : take a track, extrapolate it to the EM and build a cluster

egammaSelectClusterSETool : selection based on cluster

egammaSelectSETool : pre and post-selection based on shower shape

EMSofteCombinedTool :calculate variables which combine cluster and track

Tools EMShowerBuilder, EMPIDBuilder, EMFourMomBuilder, EMConversionBuilder and EMBremsstrahlungBuilder are also called by softeBuilder.


@author H. Ma <hma@bnl.gov>
@author A. Poppleton <Alan.Poppleton@cern.ch>
@author S. Rajagopalan <srinir@bnl.gov>
@author M. Wielers <Monika.Wielers@cern.ch>
@author A. Khodinov <khodinov@bnl.gov>
@author F. Derue <derue@lpnhe.in2p3.fr>
@author T. Koffas  <Thomas.Koffas@cern.ch>
*/
