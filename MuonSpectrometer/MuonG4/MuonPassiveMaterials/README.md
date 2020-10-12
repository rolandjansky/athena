MuonPassiveMaterials
---

This package hosts the AGDD primary number xml files for the description of the passive materials present in the muon spectrometer and its close surrounding. Those passive materials are important for the simulation of muon tracks but also for cavern background and radiation studies.

Previously the AGDD primary numbers were contained in an xml block inside the [amdb files](https://twiki.cern.ch/twiki/bin/view/Atlas/AmdbSimrecFiles). However, there was no version control system. Now, they are contained in this package which also allows to study the passive materials in a standalone GeoModel environment outside of athena (such as the [agdd standalone package](https://gitlab.cern.ch/GeoModelATLAS/agdd)) without needing the full amdb files.

Content
---

`passiveMat_r.08.02.xml`: file used to create `MuonSpectrometer-R.08.02` layout: Run 2 detector geometry, based on AGDD xml block of `amdb_simrec.r.08.01`, fixes of JFSH and JTSH, fixed floor position, updated HFTruck description, updated description of flexible chain in sectors 9, 11 and 15, updated description of ATLAS feet

`passiveMat_r.09.02.xml`: file used to create `MuonSpectrometer-R.09.02` layout: Run 3 detector geometry (symmetric NSW & BIS7/8), based on `passiveMat_r.08.02.xml`, added `JDSH_Shield_NSW` on both sides, removed Run 2 `JDSH_Shield`

`passiveMat_r.09.02.Asym.xml`: file used to create `MuonSpectrometer-R.09.02.Asym` layout: Run 3 detector geometry (NSW & BIS7/8 on Aside only), based on `passiveMat_r.08.02.xml`, added `JDSH_Shield_NSW` on Aside replacing Run 2 `JDSH_Shield`

