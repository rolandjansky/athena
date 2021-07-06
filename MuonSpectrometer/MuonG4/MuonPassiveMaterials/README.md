MuonPassiveMaterials
---

This package hosts the AGDD primary number xml files for the description of the passive materials present in the muon spectrometer and its close surrounding. Those passive materials are important for the simulation of muon tracks but also for cavern background and radiation studies.

Previously, the AGDD primary numbers were contained in an xml block inside the [amdb files](https://twiki.cern.ch/twiki/bin/view/Atlas/AmdbSimrecFiles). However, there was no version control system. Now, they are contained in this package which also allows to study the passive materials in a standalone GeoModel environment outside of athena (such as the [AGDD GeoModel plugin](https://gitlab.cern.ch/atlas/geomodelatlas/GeoModelATLAS)) without needing the amdb files.

Content
---

xml file | used in muon geometry | comments
--- | --- | ---
`passiveMat_r.08.02.xml` | `MuonSpectrometer-R.08.02` | Run 2 detector geometry, based on AGDD xml block of `amdb_simrec.r.08.01`, fixes of JFSH and JTSH, fixed floor position, updated HFTruck description, updated description of flexible chain in sectors 9, 11 and 15, updated description of ATLAS feet, use correct Boron fraction in Polyboron in JDSH_SmallWheelHubPoly
`passiveMat_r.09.02.xml` | `MuonSpectrometer-R.09.02` | Symmetric Run 3 detector geometry (symmetric NSW & BIS7/8), based on `passiveMat_r.08.02.xml`, added `JDSH_Shield_NSW` on both sides, removed Run 2 `JDSH_Shield`, use correct Boron fraction in Polyboron in NSW JD
`passiveMat_r.09.02.Asym.xml` | `MuonSpectrometer-R.09.02.Asym` | Asymmetric Run 3 detector geometry (NSW & BIS7/8 on Aside only), based on `passiveMat_r.08.02.xml`, added `JDSH_Shield_NSW` on Aside replacing Run 2 `JDSH_Shield`, use correct Boron fraction in Polyboron in NSW JD
`passiveMat_r.09.03.xml` | `MuonSpectrometer-R.09.03` and `MuonSpectrometer-R.10.01` | Symmetric Run 3 (and Run4) detector geometry (symmetric NSW), based on `passiveMat_r.09.02.xml`, removed clashes between NSW JD and calorimeter saddle
`passiveMat_r.09.03.Asym.xml` | `MuonSpectrometer-R.09.03.Asym` | Asymmetric Run 3 detector geometry (NSW & BIS7/8 on Aside only), based on `passiveMat_r.09.02.Asym.xml`, removed clashes between NSW JD and calorimeter saddle


