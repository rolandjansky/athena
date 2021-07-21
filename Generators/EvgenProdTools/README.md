#  TestLHE 

* [Introduction](#introduction)
* [Using TestLHE within Athena](#using-testlhe-within-athena)
  * [LHE files](#lhe-files)
  * [Running TestLHE in Athena](#running-testlhe-in-athena)
    * [On-the-fly production](#on-the-fly-production)
    * [LHE only production](#lhe-only-production)
    * [Running with external inputs](#running-with-external-inputs)
* [Tests implemented in TestLHE](#tests-implemented-in-testlhe)
* [Some information for users](#some-information-for-users)



# Introduction 

TestLHE is a new tool for testing LHE files. When a problem is found on the LHE file a warning is thrown but the generation is not stopped.
The code of TestLHE does not interfere with other parts of MC production.

For any questions or suggestions on the TestLHE tool, please contact Danning Liu (danning.liu@cern.ch)



# Using TestLHE within Athena

TestLHE is tested for different JOs and different modes. TestLHE checks LHE files produced in OTF (on-the-fly) production, LHE only production 
and also in MC production using externally produced LHE input files. This tool performs checks before showering events.

TestLHE tool has a similar structure as TestHepMC, the difference is that TestLHE checks LHE files, while TestHepMC checks EVNT (HepMC) files.

TestLHE tool is written by C++, and all checks run in the init phase of the transform.



# LHE files 

LHE files are outputs of Matrix Element (ME) level generators. They are written in a standarised format. LHE files store information about 
generated process and detailed info about particles in each event. 

The standard format of LHE file is as below:

***

 **Initialization information**:

one line with process-number-independent information

NPRUP lines, one for each process IPR in the range 1 throught NPRUP (NPRUP is the number of processes in this run, with each process characterized by its beam identities, energies, PDF sets and weighting strategy )

> < init >
>
> 2212 2212 6.500000e+03  6.500000e+03  0  0  247000 247000 -4 1  +3.5275000e-01 +4.5550960e-04.  + 3.5275000e-01 1
>
> < generator name = 'MadGraph5_aMC@NLO'  version='2.8.1' > please cite 1405.0301 < /generator >
>
> < /init >  

***

**Event information, repeated as many times as there are events**

one line with common event information

 NUP lines, one for each particle I in the range through NUP  (NUP is the number of particles in the events, with each particle characterized by its identity, status, mother, colour, four-momentum, mass, proper lifetime and spin)

>< event >
>
>5   1 +3.5275000e-01 3.76156600e+02 7.55578600e-03 1.10113700e-01
>
>​     2 -1  0  0 502  0 +0.0000000000e+00 +0.0000000000e+00 +1.6137332936e+03 1.6137332936e+03 0.0000000000e+00 0.0000e+00 -1.0000e+00                            
>
>   -1 -1  0  0  0 501 -0.0000000000e+00 -0.0000000000e+00 -3.7008508053e+01 3.7008508053e+01 0.0000000000e+00 0.0000e+00 1.0000e+00                                          
>
>​    6 1  1  2 502  0 +6.1888789281e+01 -3.4443802267e+01 +6.2565208089e+02 6.5285019268e+02 1.7250000000e+02 0.0000e+00 -1.0000e+00                                       
>
>   -6 1  1  2  0 501 -6.1972574329e+01 -2.5387963415e+01 +4.4412118412e+02 4.8112890668e+02 1.7250000000e+02 0.0000e+00 1.0000e+00                                        
>
>   24 1  1  2  0  0 +8.3785047894e-02 +5.9831765681e+01 +5.0695152058e+02 5.1676270234e+02 8.0398999449e+01 0.0000e+00 -1.0000e+00
>
>< /event >




# Running TestLHE in Athena

## On-the-fly production
 LHE file is produced and showered in the same production job:

Gen_tf.py  ‘--jobConfig=421107’  ‘—maxEvents=10’  ‘--outputEVNTFile=tmp.EVNT.root’

## LHE-only production
only LHE file is produced in a production job:

Gen_tf.py  ‘--jobConfig=421005’  ‘—maxEvents=10’  ‘--outputTXTFile=tmp.TXT.tar.gz'


## Running with external inputs
the LHE files was produced externaly and in the production job it is showered: 

Gen_tf.py  '--jobConfig=600463'  '--inputGeneratorFile=mc15_14TeV.440103.PowhegV2_HHNLOFTcHHHp01d0.evgen.TXT.e0000'  '--outputEVNTFile=tmp.EVNT.root'



# Tests implemented in TestLHE

TestLHE tool will test event information, particle information and initialization information. 

First it checks the format of file (the number of integers and floats), then TestLHE tool tests energy balance, momentum balance, negative 
energy checks, looks for NaNs or infinite values, checks if the  PDGID is correct and performs checks for  tachyons. 
TestLHE tool also checks weights, i.e. looks for weight names duplications or  special characters in weight names, checks weights consistence. 

**This tool throws in output information messages or warnings like:**

> 22:40:58  TestLHE       INFO No Errors are detected when first reading LHE files                                                                                                                                               
>
> 22:41:10  TestLHE      WARNING Some Special Letters Or Symbols detected in weightgroupName



# Some information for users

LHE files contain event and particles information. TestLHE checks both format and content of the LHE file, as well as particle number 
consistence, weight consistence and energy/momentum conservation.

TestLHE tool only does not crash the job, but only provides information or warning messages.
Please pay attention to the warnings when preparing MC production including LHE files.

TestLHE tool is not executed if events.lhe file does not exists.

