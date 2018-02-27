Short Description of the tests
-------------------------------

EgEfficiencyCorr_testEgEfficiencyCorrWithoutFile
-----------------------------------------------

This is the main test available to check directy that the tool
return the intended output (i.e what is in the root files)


Usage : 
```
 EgEfficiencyCorr_testEgEfficiencyCorrWithoutFile  --help
                         INFO    Options:
  -h [ --help ]                    Help screen
  -l [ --msgLevel ] arg (=3)       message level
  -f [ --file ] arg                scale factor file: Required a SF file or a 
                                   map file
  -r [ --runno ] arg               run number: Required
  -e [ --eta ] arg                 eta: Required
  -p [ --pt ] arg                  pt: Required
  -t [ --type ] arg                Simulation type: Required
  -c [ --correlation ] arg (=FULL) Correlation Model FULL 
                                   (default),SIMPLIFIED,TOTAL,COMBMCTOYS
  -k [ --keyreco ] arg             Reco working point Key
  -m [ --mapfile ] arg             Map file name: Required a SF file or a map 
                                   file
  -d [ --keyid ] arg               Identification working point Key
  -i [ --keyiso ] arg              Isolation working point Key
  -g [ --keytrigger ] arg          Trigger working point Key
```

For exampe :
```
EgEfficiencyCorr_testEgEfficiencyCorrWithoutFile -f ElectronEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v1/offline/efficiencySF.offline.LooseAndBLayerLLH_d0z0_v11.2015_2016.13TeV.rel20.7.25ns.v01.root -t FullSim -r 266904 -e -0.31 -p 79001 
```

Tests requiring a file
---------------------
Test With an xAOD File :

```
EgEfficiencyCorr_testEgEfficiencyCorr [xAOD file name] 
```

```
EgEfficiencyCorr_testEgChargeCorr [xAOD file name] [Correction File name]
```

EgEfficiencyCorr_mem_check
----------------------------
This just initializes the tools. Could be used for a memory leakage test

```
valgrind --tool=memcheck --leak-check=full --suppressions=$ROOTSYS/etc/valgrind-root.supp  --error-limit=no --track-origins=yes --smc-check=all --trace-children=yes  --track-fds=yes --num-callers=30 EgEfficiencyCorr_mem_check>valgrind.log 2>&1 & 
```
