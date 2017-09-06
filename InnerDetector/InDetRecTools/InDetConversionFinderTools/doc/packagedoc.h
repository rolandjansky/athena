/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetConversionFinderTools_page InDetConversionFinderTool

@section InDetConversionFinderTools_introduction Introduction:
InDetConversionFinderTool is a package which contains an AlgTool to reconstruct gamma conversions in an event. It inherits from IVertexFinder interface (InnerDetector/InDetRecTools/InDetRecToolInterfaces) and can be called by an algorithm using this interface. To fit the decay vertex of found conversions a common IVertexFitter interface is used. The default vertex fitter is Billoir FullFitter.
All units are in mm and MeV.

@section InDetConversionFinderTools_howitworks How it works:
The input of this tool is a collection of tracks (see TrkTrack/TrackCollection.h), it rejects the tracks comming from the primary vertex by applying of \f$ d_0 \f$ and \f$ z_0 \f$ cuts. From remaining tracks all possible positive and negative track pairs are build. For each pair a distance cut and a momentum fraction cut are applyed. These cuts are useful to reconstruct symmetric photon conversions. The cuts for (very) asymmetric conversions still have to be implemented. The distance is the 3d distance between the first hits in detector of both tracks. If both hits are on the same surface, the global position of the hits is taken to calculate the distance. If not, the track with the first measurement closest to 000 is chosen and the distance of the closest approach of another track to this measurement is calculated. The momentum fraction is defined as a track momentum devided by a sum of momenta of both tracks. In case of symmetric conversions this value should be (exponential?) distributed around 0,5. All cuts can be changed/set via jobOptions. The remaining track pairs are fitted. On the reconstructed vertices the \f$ \chi^2 \f$ and an invariant vertex mass cut are applyed. If the VxCandidate passes all cuts, it is stored and all info is written in a VxContainer (see VxVertex/VxContainer.h).<br>

@section InDetConversionFinderTools_jobOptions The jobOptions file:
The algorithm is configured automaticly via genConf
<ul>
<li>Specifiy the name of the vertex fitter:<br>
<b>'VertexFitterTool' : PublicToolHandle('Trk::FullVertexFitter'),</b><br>
<li>Reconstruction cuts:<br>
    <ul>
      <li><b> 'maxDist' : 50.0 </b></li>
      <li><b> 'minD0' : 0.1 </b> </li>
      <li><b> 'minZ0' : 0.1 </b> </li>
      <li><b> 'minPt' : 500.0 </b> </li>
      <li><b> 'chi2cut' : 10.0 </b> </li>
      <li><b> 'invMasscut' : 500.0 </b> </li>
       <li><b>'min_mom_frac' : 0.3 </b> </li>
      <li><b>'max_mom_frac' : 0.7 </b> </li>
    </ul>



\namespace Vtx
A namespace for all vertexing packages and related stuff.
*/