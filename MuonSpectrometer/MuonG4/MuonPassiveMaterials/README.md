MuonPassiveMaterials
---

This package hosts the AGDD primary number xml files for the description of the passive materials present in the muon spectrometer and its close surrounding. Those passive materials are important for the simulation of muon tracks but also for cavern background and radiation studies.

Previously the AGDD primary numbers were contained in an xml block inside the [amdb files](https://twiki.cern.ch/twiki/bin/view/Atlas/AmdbSimrecFiles). However, there was no version control system. Now, they are contained in this package which also allows to study the passive materials in a standalone GeoModel environment outside of athena (such as the [agdd standalone package](https://gitlab.cern.ch/GeoModelATLAS/agdd)) without needing the full amdb files.

Content
---

`passiveMat_r.08.02.xml`: Run 2 detector geometry, based on AGDD xml block of `amdb_simrec.r.08.01`, fixes of JFSH and JTSH, fixed floor position, update HFTruck description

