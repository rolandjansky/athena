/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TRT_PAI_Process_page TRT_PAI_Process Package

Ionization of TRT gas via the Photon Absorption Ionization model.

@author Originally implemented in age FORTRAN by Pavel Nevski.
        Adapted to C++ by T.Kittelmann/Mogens Dam.

@section TRT_PAI_Process_IntroductionTRT_PAI_Process Introduction

The main reference for this implementation is
- V.M.Grishin, V.K.Ermilova, and S.K.Kotelnikov, NIM A307 (1991) 273-278.

Other references to the PAI model are
- V.C.Ermilova, L.P.Kotenko, and G.I.Merzon, NIM145 (1977) 555-563.
- J.H.Cobb, W.W.M.Allison, and J.N.Bunch, NIM133 (1976) 315-323.
- W.W.M.Allison and J.H.Cobb, Ann.Rev.Nucl.Part.Sci (1980), 253-98.

@note Internally this package uses the original units of the fortran version.
      The external world sees CLHEP units.

@section TRT_PAI_Process_OverviewTRT_PAI_Process Class Overview
  The TRT_PAI_Process package contains of following classes:

  - TRT_PAI_Process:      main class for the PAI model.
  - TRT_PAI_element:      description of chemical elements
  - TRT_PAI_gasComponent: description of gas components (molecules)
  - TRT_PAI_gasMixture:   global description of gas mixture
  - TRT_PAI_effectiveGas: calculation of all variables for gasMixture in order
                          to perform PAI
  - TRT_PAI_gasdata.h:    tabulated values of photon absorption cross section



*/
