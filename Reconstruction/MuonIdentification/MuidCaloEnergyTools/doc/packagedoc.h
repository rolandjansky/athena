/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuidCaloEnergyTools_page MuidCaloEnergyTools Package

Package providing estimate of the muon energy loss upstream the muon spectrometer. The estimation can be based either on a parametrization, on the calorimeter measurement, or, by default, a combination of the two (known as the "hybrid method"). In the case where the calorimeter measurement is used, muon (calorimeter and track) isolation issues are taken into account. Detailed information has been published: K. Nikolopoulos, D. Fassouliotis, C. Kourkoumelis, and A. Poppleton, "Event-by-Event Estimate of Muon Energy Loss in ATLAS", IEEE Trans. Nucl. Sci., vol. 54, no. 5, pt. 2, pp. 1792-1796, Oct. 2007 [also ATL-MUON-PUB-2007-002].


@author Alan.Poppleton@cern.ch
@author Konstantinos.Nikolopoulos@cern.ch

The package contains three AlgTools: 

- @ref Rec::MuidCaloEnergyTool : High-level tool for the estimation of the muon energy loss and its error. The estimation can be based either on a parameterization, on the calorimeter measurement or on a combination of the two. Uses the 2 tools below plus MuidTrackIsolation from the MuidCaloIsolationTools package.

- @ref Rec::MuidCaloEnergyMeas : Provides the information on the energy deposited in the calorimeter cells around the muon. A measurement and an isolation cone are defined. Energy deposits in the measurement cone are assumed to be due to muon energy loss, while the energy deposits in the isolation cone are used to estimate the reliability of the calorimeter measurement.

- @ref Rec::MuidCaloEnergyParam : Tool providing information on the parametrization of the muon energy loss in the material upstream of the muon spectrometer.



 @namespace Rec namespace for combined reconstruction tools and interfaces
 @namespace Trk namespace for general tracking tools and interfaces

*/
