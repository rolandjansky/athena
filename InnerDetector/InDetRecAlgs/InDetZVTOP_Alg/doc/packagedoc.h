/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetZVTOP_Alg_page ZVTOP_Algs Package

contains: <br>
- ConversionFinder.cxx <br>

@author Tatjana Lenz <tatjana.lenz@cern.ch>

@section InDetZVTOP_Alg_ZVTOP_AlgsIntro Introduction

ZVTOP_Algs is a package which contains algorithm to reconstruct vertices. The only job is to call a vertex finder tool - InDetZVTOPVxFinder (see InnerDetector/InDetRecTools/InDetZVTOPVxFinder). The topological vertex finder has been developed by David J. Jackson, Nucl.Instrum.Meth.A388: 247-253, 1997 and now implemented to ATHENA and adjusted  to ATLAS requirements. It reconstructs a set of vertices & associated subset of the charged tracks. The association accords to a vertex probability function which is based on the trajectories and position resolution of the tracks.



*/
