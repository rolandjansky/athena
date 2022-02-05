# MuonPRDTest   
This package contains the muon subdetector validation code which can be executed during the simulation, digitization and reconstruction 
to write out the basic variables allowing to monitor the performance and plausibility of the respective transformation output. 
A guidance for the **CSC** and **TGC** implementation will be shown.   

Here are some important links for your reference:   
- The [Muon Software twiki](https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSoftware) collects all common information   
- The [Muon Software tutorials](https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSoftwareTutorials) of using and developing the ATLAS Muon Software   
- Some [Useful Tips](https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSWUsefulTips) for running Athena   
- Contacts:   
  - [Open issues on JIRA](https://its.cern.ch/jira/browse/ATLASMCP)   
  - Muon Software mailing list: [hn-atlas-muonSW](https://groups.cern.ch/group/hn-atlas-muonSW/default.aspx)   

## Getting the Code
### Setup
Setup the ATLAS and Git environment:   
```
setupATLAS   
lsetup git
```
### Sparse Checkout
Fork the repository, more details see the [ATLAS tutorial](https://atlassoftwaredocs.web.cern.ch/gittutorial/gitlab-fork/).
Athena is a large framework, very often you want to checkout a few packages to work on instead of a full checkout. 
To do the sparse checkout and add a specific package in Athena:
```
git atlas init-workdir https://:@gitlab.cern.ch:8443/atlas/athena.git
cd athena
git atlas addpkg MuonPRDTest
```
### Setup Athena
Athena setup: `asetup Project,Branch,Nightly,TestArea` 
```
mkdir ../build
cd ../build
asetup Athena,master,latest,here
```
### Compile
```
cmake ../athena/Projects/WorkDir/
make -j5
```
REMEMBER to source the new changes everytime after recompile:
```
source x86_64-centos7-gcc8-opt/setup.sh
```

## Retrieving the Basic Variables
The Containers, IdHelpers and MuonDetectorManager are the main packages used to retrieve the basic variables which define an object.

### Containers
Each container is a vector of objects. For CSC and TGC:
- The SimHitCollection and HitId are retrieved from the [MuonSimEvent](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonSimEvent) package. 
- The TGC SDO Identifiers and SimData are retrieved from the [MuonSimData.cxx](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonSimData/src/MuonSimData.cxx) 
in the [MuonSimData](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonSimData) package. 
And the CSC SDO Identifers and SimData are retrieved from [CscSimData.cxx](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonSimData/src/CscSimData.cxx).
- The Digits are retrieved from the [MuonDigitContainer](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonDigitContainer) package.
- The RDO are retrieved from the [MuonRDO](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonRDO) package.
- The PRD are retrieved from the [MuonPrepRawData](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData) package.

### IdHelpers
Each object has an Identifier which is an 64-bit integer, the bit fields describe the basic variables that defined the object. 
The IdHelpers are used to access the information in the Identifiers or prepare the Identifiers, 
the IdHelpers are defined in the [MuonIdHelpers](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonIdHelpers) package.   
The IdHelpers can be used to build an Identifiers with the given fields, for example:   
```
m_CscIdHelper->channelID(stname, steta, stphi, clayer, wlayer, measuresPhi, StripNumber, true, &isValid);
```

### MuonDetectorManager
The access to the readout geometry is done via the MuonDetectorManager class. 
The actual detector readout elements and the geometry information can be found in the package [MuonReadoutGeometry](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry).   
The detector manager also contains pointers to the IdHelpers used for packing/unpacking the Identifiers.   
The local and global positions of a detector element can be retrieved via the [CscReadoutElement](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry/src/CscReadoutElement.cxx)
and the [TgcReadoutElement](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry/src/TgcReadoutElement.cxx)
connected by the identifiers:
```
const MuonGM::CscReadoutElement* cscdet = MuonDetMgr->getCscReadoutElement(id);
const MuonGM::TgcReadoutElement* tgcdet = MuonDetMgr->getTgcReadoutElement(id);
```

## Athena Coding Conventions
Private and protected data members of classes must begin with "m_":
- Exception only for xAOD data classes
- no other variables should begin with "m_"
- Don't define variable names start with an "_" or have "__" in them   

All output should happen through an ATH_MSG_(DEBUG/VERBOSE/INFO/WARNING/ERROR/FATAL), more details see [Message Services](https://atlassoftwaredocs.web.cern.ch/ABtutorial/basic_messaging/).

## Cabling Services
The [TgcRawData](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonRDO/MuonRDO/TgcRawData.h) 
do not have a method returning an Identifier currently, but we can retrieve the Identifier by using the cabling services 
in the package [TGCcablingInterface](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonCablings/TGCcablingInterface/TGCcablingInterface).   
The Identifiers can be retrieved by calling TGC cabling services:
```
Identifier Id;
m_tgcCabling->getElementIDfromReadoutID(Id,rdo->subDetectorId(),rdo->rodId(),rdo->sswId(),rdo->slbId(),rdo->bitpos(),orFlag);
```

## Validation Algorithm
The [NSWPRDValAlg](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonValidation/MuonPRDTest/src/NSWPRDValAlg.cxx) scripts define the CSC and TGC validation with options:
- CSC: `m_doCSCHit`, `m_doCSCSDO`, `m_doCSCDigit`, `m_doCSCRDO` and `m_doCSCPRD`
- TGC: `m_doTGCHit`, `m_doTGCSDO`, `m_doTGCDigit`, `m_doTGCRDO` and `m_doTGCPRD`   

These job options can be included in the respective validation algorithm scripts in the [share](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonValidation/MuonPRDTest/share) folder
- [NSWPRDValAlg.sim.py](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonValidation/MuonPRDTest/share/NSWPRDValAlg.sim.py) - Simulation: Hit 
- [NSWPRDValAlg.digi.py](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonValidation/MuonPRDTest/share/NSWPRDValAlg.digi.py) - Digitization: Hit, SDO, Digit, RDO
- [NSWPRDValAlg.reco.py](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonValidation/MuonPRDTest/share/NSWPRDValAlg.reco.py) - Reconstruction: SDO, PRD

Please note that the job options of CSC have to include the `MuonGeometryFlags`, for example:
```
NSWPRDValAlg.doCSCHit= MuonGeometryFlags.hasCSC()
```    
These scripts can be used by specifying `--postInclude MuonPRDTest/NSWPRDValAlg.[sim/digi/reco].py` while running the 
respective scripts for simulation (Sim_tf.py), digitization (Digi_tf.py) and reconstruction (Reco_tf.py). 
The postInclude adds a validation algorithm which writes out an ntuple for the validation.    
More details of these tests in [MuonSWTestSamples twiki](https://twiki.cern.ch/twiki/bin/viewauth/Atlas/MuonSWTestSamples) 
and [MuonRecRTT/test](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonValidation/MuonRecValidation/MuonRecRTT/test) package.    

To run the simulation, digitization and reconstruction tests locally:    
1. Save your local changes and repeat the steps in [compile](#compile) section (don't forget to setup Athena first)
2. Specify the job options you would like to run in NSWPRDValAlg.[sim/digi/reco].py
3. `mkdir run`, `cd run` and then run the tests in the [MuonRecRTT/test](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonValidation/MuonRecValidation/MuonRecRTT/test) package.   

## DCube
DCube produces comparison plots of an ART job's output with a fixed reference, more details see [ART DCube](https://gitlab.cern.ch/art/dcube).   
The scripts which are used to create DCube histograms from the output ntuple files of NSWPRDValAlg are placed in the [scripts](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonValidation/MuonPRDTest/scripts) folder. 
The CSC and TGC histograms are defined in [DCubeHistograms.py](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonValidation/MuonPRDTest/scripts/DCubeHistograms.py), 
you can define which variables you would like to add or remove from the list of monitored variables here. 
Remember to modify the corresponding `dcube_config_*.xml` files in the [data](https://gitlab.cern.ch/atlas/athena/-/tree/master/MuonSpectrometer/MuonValidation/MuonPRDTest/data) folder.   

The CSC and TGC DCube histograms can be created by the [createDCubeSimHistograms.py](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonValidation/MuonPRDTest/scripts/createDCubeSimHistograms.py), 
[createDCubeDigitHistograms_withSel.py](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonValidation/MuonPRDTest/scripts/createDCubeDigitHistograms_withSel.py) 
and [createDCubeRecoHistograms_withSel.py](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonValidation/MuonPRDTest/scripts/createDCubeRecoHistograms_withSel.py).    
The CSC and TGC DCube histogram comparsions are implemented with eta and phi selections:   
- eta selections: None, positive, negative
- phi selections: None, phi sector    

The selections can be used by adding arguments, for example:
```
python createDCubeSimHistograms.py --CSCsel negative_2 --TGCsel None_None
```
for CSC Sim histograms with negative eta cut and second sector cut, and TGC Sim histograms with no eta and phi sector cut.   

Note: the CSC input should be turned off if using Run4 input file, controlled by the arguments `--doCSC`.   
The DCube histogram scripts are implemented in the [test_Run3_asymmetric_fullChain.sh](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonValidation/MuonRecValidation/MuonRecRTT/test/test_Run3_asymmetric_fullChain.sh) 
and [test_Run3_symmetric_fullChain.sh](https://gitlab.cern.ch/atlas/athena/-/blob/master/MuonSpectrometer/MuonValidation/MuonRecValidation/MuonRecRTT/test/test_Run3_symmetric_fullChain.sh) currently.   

## Websites
- The daily ART job's outputs are saved in this [website](https://atlas-art-data.web.cern.ch/atlas-art-data/grid-output/).    

## Authors
Author of the guidance of CSC and TGC implementation:
- Minlin Wu
- Contact: minlin.wu@cern.ch


