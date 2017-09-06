/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetConversionFinder_page InDetConversionFinder

@section InDetConversionFinder_introduction Introduction:
InDetConversionFinder is a package which contains algorithm to reconstruct conversions. The only job now is to call a vertex finder tool - InDetConversionFinderTool. (see InnerDetector/InDetRecTools/InDetConversionFinderTools)
All units are in mm and MeV.

@section InDetConversionFinder_howitworks How it works:
The algorithm InDetConversion.cxx is a prototype usecase of the InDetConversionFinderTools package. This tool reconstructs conversions in the InnerDetector. The performance of the conversion reconstruction depends stronlgy on the BackTracking performace and the choosen cuts.

@section InDetConversionFinder_packagecontent Package Contents:
InDetConversionFinder contains the following files/classes:
-ConversionFinder.cxx

@section InDetConversionFinder_jobOptions The jobOptions file:
Remark: the algorithm is configured via automatic generated python scripts (genConf) <br>
All values shown are <b>default</b> values, i.e. if nothing is specified in the jobOptions
the algorithm will use this default value.
<ul>
<li>Specifiy the name of the track input container:<br>
    <b>'TracksName' : 'Tracks' </b><br>
<li>Specify the name of the output container:<br>
    <b>'InDetConversionOutputName' : 'InDetConversion' </b></li>
<li>Specify the name of IVertexFinder:<br>
    <b>''VertexFinderTool' : PublicToolHandle('InDet::InDetConversionFinderTools') </b></li>
</ul>


\namespace Vtx
A namespace for all vertexing packages and related stuff.
*/