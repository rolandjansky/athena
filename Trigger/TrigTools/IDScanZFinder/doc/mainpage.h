/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage
@author Nikos.Konstantinidis@cern.ch Erkcan.Ozcan@cern.ch
@section IntroIDScanZFinder Introduction

This package contains the AlgTool IDScanZfinder that determines the z-position of the primary pp interaction
before any track reconstruction. Full details about the algorithm used can be found in:

https://twiki.cern.ch/twiki/bin/view/Atlas/IDScanHowTo

Some details of the parameters and how they are set can be found in:

http://indico.cern.ch/getFile.py/access?contribId=1&resId=0&materialId=slides&confId=57832

@section IDScanZFinderProps IDScanZfinder properties:

   - PhiBinSize : phi size of histogram bins ( default 0.2 degrees );
   - UseOnlyPixels  : if true, only use the pixel SPs ( default false );
   - ROIphiWidth : phi size of RoI (must be the same as set in IDScanMain) ( default 0.2 radians );
   - MinZBinSize : the size of bins in the z-histogram is MinZBinSize + RoI_eta*ZBinSizeEtaCoeff ( default 0.2mm );
   - MaxZ : determines the limits of the z-histogram - reconstructed z can be up to +/- MaxZ ( default 200mm );
   - ZBinSizeEtaCoeff : the size of bins in the z-histogram is MinZBinSize + RoI_eta*ZBinSizeEtaCoeff ( default 0.1 );
   - DPhiDEta : to take into account that in the EC, the amount of bending as measured in DeltaPhi is dependent on the eta ( default -0.02 );
   - NeighborMultiplier : A multiplicative factor to apply to the computed number of neighbors considered while spacepoints are being paired, developed to account for severe beamspot displacements ( default 1.0 )
   - NumberOfPeaks : No. of possible z-positions to consider in track finding. ( default 1);
   - ChargeAware : in single particle RoIs, use the fact that the one track only bends to larger or smaller phi values when making hit pairs ( default false, turned true for e and mu slices by IDScan )
   - ZHistoPerPhi : if true, one z-histo per phi slice ( default false )
   - VrtxDistCut : minimum fractional distance between two output vertices ( default 0, set to 10% for e & mu slices by IDScan )
   - nVertexSeparation : minimum number of zbins that any two output vertices should by separated by ( default 0, set to 5 for e & mu slices by IDScan )
   - VrtxMixing : if any two output peaks are too close, use this value as a weight to average the two z-positions ( experimental feature, default 0 )
   - PreferCentralZ : If two peaks have the same height, report the one with lowest |z| first ( default false, set true for e & mu slices by IDScan )
   - TrustSPProvider : Use all the spacepoints returned by the OnlineSpacePointTool, without apply cuts based on RoI phi width ( default true )
   - FullScanMode : full-scan mode ( default false, used for full-scan slices )
   - TripletMode : integer to chose triplet mode; if 0 ( default ), do not use the triplets; if 1, triplet mode is only for confirmation of pairs; if 2, each triplet contributes to the zhistos ( slower, possibly more resistant to pileup )
   - TripletDZ, TripletDK, TripletDP : Cuts applied in confirming/creating triplets ( default values 25., 0.005, 0.05 )


@htmlinclude used_packages.html

@include requirements

*/
