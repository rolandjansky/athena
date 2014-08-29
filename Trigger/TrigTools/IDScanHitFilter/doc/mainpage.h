/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage IDScanHitFilter Package
@author Nikos.Konstantinidis@cern.ch Dmitry.Emeliyanov@cern.ch
@section IntroIDScanHitFilter Introduction

This package contains the AlgTool that does the main part of pattern recognition of IDScan. For a full discription of all the 
pattern recognition steps of IDScan, please look at 

https://twiki.cern.ch/twiki/bin/view/Atlas/IDScanHowTo

@section IDScanHitFilterProps IDScanHitFilter Properties:

   - PrintDiagnosticMessages : extra diagnostic printing to std::cout ( default false )
   - phiHalfWidth : RoI size in phi ( default 0.1 rads )
   - etaHalfWidth : RoI size in eta ( default 0.1 rads )
   - PhiBinSize : Size of the phi bins in the (phi,eta) histogram used at the main step of pattern recognition. The phi bin size is linked to how low-pT tracks can be reconstructed ( default 2.4 degrees ) 
   - EtaBinSize : Size of the eta bins in the (phi,eta) histogram used at the main step of pattern recognition ( default 0.005 )
   - LayerThreshold : Min. number of SPs in different Si layers in order to accept a group of SPs as a possible track candidate ( default 3.5 )
   - EnhanceLayer0 : By setting this parameter to TRUE you have a better chance of finding the Pixel Layer0 spacepoint of a track ( default true )
   - RemoveDuplicates : Apply a basic removal of duplicate tracks ( default true )
   - CloneRemovalCut : Between tracks sharing 3 (default) spacepoints or more, only the longest one is kept ( default 3 )
   - UsePtSign : Used to find clone tracks more efficiently ( default true )

   - LooseTightBoundary : Number of SPs in a group below which we use the loose set of cuts for the groupCleaner ( default 20.5 )
   - deltadzdrCut_loose : The difference in d_z/d_rho between hits 12 and 23 in a triplet, used to decide if the triplet comes from a track ( default 0.04 )
   - deltadzdrCut_tight : As above, but tighter, for when origianl group of hits is large, i.e. containing hits from several tracks ( default 0.01 )
   - etaCutInner_loose : The difference in eta between two hits (1-2), used to decide if they come from the same track ( default 0.008 )
   - etaCutInner_tight : As above, but tighter, for when origianl group of hits is large ( default 0.004 )
   - etaCutOuter_loose : The difference in eta between two hits (2-3), used to decide if they come from the same track ( default 0.02 )
   - etaCutOuter_tight : As above, but tighter, for when origianl group of hits is large ( default 0.01 )
   - d0Cut_loose : The track d0 calculated from a triplet of hits, used to decide if a triplet could come from a reasonable track ( default 4.0 )
   - d0Cut_tight : As above, but tighter, for when origianl group of hits is large ( default 1.0 )
   - dPhidRBins_loose : Number of 1/pT bins in the (1/pT,phi0) histogram used in the final step of pattern recognition ( default 200 )
   - dPhidRBins_tight : As above, but tighter, for when origianl group of hits is large ( default 250 )
   - phi0Bins_loose : Number of phi0 bins in the (1/pT,phi0) histogram used in the final step of pattern recognition ( default 16000 )
   - phi0Bins_tight : As above, but tighter, for when origianl group of hits is large ( default 20000 )


@htmlinclude used_packages.html

@include requirements

*/
