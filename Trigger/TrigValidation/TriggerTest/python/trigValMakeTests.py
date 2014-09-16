#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import getopt, sys
##################################################
#     DEFAULTS 

output=""
defaultPath="TriggerTest"
testConfName="trigValidation.conf"
defaultEmail=""
defaultHNEmail="hn-atlas-trig-validation-nightly-reports@cern.ch"
import os
if( not os.path.isdir(defaultPath )) :
   os.makedirs( defaultPath )



####################################################

from TrigValMakeTestConf import TrigValMakeTestConf

def warning() :
   answer = raw_input("This scripts overwrites files in TriggerTest directory. Are you sure you want to proceed (y/n)?")
   if( answer.lower() != "y" and answer.lower() !=  "yes" ) :
      import sys
      sys.exit("Nothing is touched")

def detectorSet(icounter,  testBase=120, config="Modern", output="" ) :

   if( output != "" ) : output = "to"+output

   itest=testBase
   icounter+= 1
   TrigValMakeTestConf(testConfName, icounter, itest, "CaloAthena"+config+"RDO"+output, 
                              ['Full' ],"testCalo",  [defaultEmail, "Denis.Damazio@cern.ch"], "",  )
#                              ['T2CaloEgamma_eGamma','T2CaloTau_Tau', 'T2TauFinal', 'TauHypo' ],"testCalo",  [defaultEmail])
   icounter+= 1
   itest +=1
   TrigValMakeTestConf(testConfName, icounter, itest, "IDAthena"+config+"RDO"+output, 
                              ['Full'], "testID", [defaultEmail, "Denis.Damazio@cern.ch"], "" )
   icounter+= 1
   itest +=1
   TrigValMakeTestConf(testConfName, icounter,itest , "MuonAthena"+config+"RDO"+output, 
                              ['Muon'], "", [defaultEmail], "" )

def sliceSet(icounter, testBase=150, config="Modern", iinput="", output="", path="TriggerTest") :

   if( iinput == "" ) : iinput = "RDO"
   if( output != "" ) : output = "to"+output
   if( iinput == "BS" or  iinput == "BStmp" ) : config = "Standalone"+config


   icounter+= 1
   itest=testBase+1
   TrigValMakeTestConf(testConfName, icounter, itest, "ElectronSliceAthena"+config+iinput+output, 
                              ['Full'], "testElectronSlice" , 
                              [defaultEmail, "Denis.Damazio@cern.ch","Teresa.Fonseca.Martin@cern.ch"], path )
   icounter+= 1
   itest +=1
   TrigValMakeTestConf(testConfName, icounter, itest, "PhotonSliceAthena"+config+iinput+output, 
                              ['Full'], "testPhotonSlice", 
                              [defaultEmail,"Valeria.Perez.Reale@cern.ch"], path)
   icounter+= 1
   itest +=1
   TrigValMakeTestConf(testConfName, icounter, itest, "MuonSliceAthena"+config+iinput+output, 
                              ['Full'], "testMuonSlice",
                              [defaultEmail,"alessandro.dimattia@roma1.infn.it"], path)
   icounter+= 1
   itest +=1
   TrigValMakeTestConf(testConfName, icounter, itest, "TauSliceAthena"+config+iinput+output, 
                              ['Full'], "testTauSlice",
                              [defaultEmail,"strom@physics.uoregon.edu"], path)
  
   icounter+= 1
   itest +=1
   TrigValMakeTestConf(testConfName, icounter, itest, "JetSliceAthena"+config+iinput+output, 
                              ['Full'], "testJetSlice", [defaultEmail,"cpotter@physics.mcgill.ca"], path)
  
   icounter+= 1
   itest +=1
   TrigValMakeTestConf(testConfName, icounter, itest, "METSliceAthena"+config+iinput+output, 
                       ['Full'], "testMETSlice",
                       [defaultEmail,"Allen.Mincer@cern.ch"], path)

   icounter+= 1
   itest +=1
   TrigValMakeTestConf(testConfName, icounter, itest, "BphysicsSliceAthena"+config+iinput+output, 
                           ['Full']  , "testBphysicsSlice",
                       [defaultEmail,"j.kirk@rl.ac.uk"], path)
  
   icounter+= 1
   itest +=1
   TrigValMakeTestConf(testConfName, icounter, itest, "BjetSliceAthena"+config+iinput+output, 
                              [    'Full'], "testBjetSlice",
                       [defaultEmail,"Andrea.Coccaro@cern.ch"], path)

   icounter+= 1
   itest +=1
   TrigValMakeTestConf(testConfName, icounter, itest, "LVL1CTPAthena"+config+iinput+output, 
                       [ 'LVL1CTP' ], "testLVL1CTP",
                       [ defaultEmail, "Wolfgang.Ehrenfeld@cern.ch"], path)

   if( iinput == "RDO" and  output == "" ) : #toESDAOD" and output !="toAOD" and output != "toESD" ) :
     icounter+= 1
     itest +=1
     TrigValMakeTestConf(testConfName, icounter, itest, "CosmicsSliceAthena"+config+iinput+output, 
                              ['Full'], "testCosmicsSlice", [defaultEmail,"jamie.boyd@cern.ch"], path)



def main():
  #warning()
  testConfigFullPath=defaultPath+"/"+testConfName

  try :
    testCF = open(testConfigFullPath,"w") # remove everything - we will prepare new tests
    testCF.write("# This file is created automatically . To recreate it use :\n")
    testCF.write("# python makeTests.py\n")
    testCF.write("# from Trigger/TrigValidation/TriggerTest/python directory \n")
  
    testCF.close()

    #########################################################
    #         PUT YOUR MODIFICATIONS BELOW THIS LINE
    #########################################################

    # by default NO AOD link from tmp directory is made.
  
    icounter = 1
    TrigValMakeTestConf(testConfName, icounter, 101, "AthenaModernRDO", 
                        ['Full'],"testAthenaRDO",  [defaultHNEmail], "",
                         makeReferenceLink=True )
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 201, "AthenaOldConfigRDO", 
                        ['Full'],"testAthenaRDO",  [defaultHNEmail], "",
                         makeReferenceLink=True )

    icounter += 1  
    TrigValMakeTestConf(testConfName, icounter, 102, "AthenaAllAlgsTrueModernRDO", 
                        ['Full'], "testAthenaRDO",  [defaultEmail], "",
                         useTmpReference=True )
    icounter += 1  
    TrigValMakeTestConf(testConfName, icounter, 202, "AthenaAllAlgsTrueOldConfigRDO", 
                        ['Full'], "testAthenaRDO",  [defaultEmail], "",
                         useTmpReference=True )
                              
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 103, "AthenaModernRDOtoBS", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "",
                        post_command="rm -f ../trigger.BS.data; ln -s $PWD/daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data ../trigger.BS.data",
                        useTmpReference=True)
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 203, "AthenaOldConfigRDOtoBS", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "",
                        post_command="rm -f ../trigger.BS.data; ln -s $PWD/daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data ../trigger.BS.data",
                        useTmpReference=True)

    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 104, "AthenaModernRDOtoESDAOD", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "TrigAnalysisTest",
                        post_command="rm -f ../trigger*root; rm -f ../output*xml; ln -s $PWD/ESD.pool.root ../trigger.ESD.pool.root;  ln -s $PWD/AOD.pool.root ../trigger.AOD.pool.root; \n  post_command ln -s $PWD/outputLVL1config.xml ../outputLVL1config.xml; \n  post_command ln -s $PWD/outputHLTconfig.xml ../outputHLTconfig.xml;", useTmpReference=True)
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 204, "AthenaOldConfigRDOtoESDAOD", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "TrigAnalysisTest",
                        post_command="rm -f ../trigger*root; rm -f ../output*xml; ln -s $PWD/ESD.pool.root ../trigger.ESD.pool.root;  ln -s $PWD/AOD.pool.root ../trigger.AOD.pool.root; \n  post_command ln -s $PWD/outputLVL1config.xml ../outputLVL1config.xml; \n  post_command ln -s $PWD/outputHLTconfig.xml ../outputHLTconfig.xml;", useTmpReference=True)

    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 105, "AthenaModernAOD_rerunHYPO", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "TrigAnalysisTest",
                         useTmpReference=True)
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 205, "AthenaOldConfigAOD_rerunHYPO", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "TrigAnalysisTest",
                         useTmpReference=True)

  
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 106, "AthenaStandaloneModernRDO", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "",
                        useTmpReference=True) 
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 206, "AthenaStandaloneOldConfigRDO", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "",
                        useTmpReference=True)

    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 107, "AthenaStandaloneModernBS", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "",
                        useTmpReference=True)
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 207, "AthenaStandaloneOldConfigBS", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "",
                        useTmpReference=True)

    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 108, "AthenaStandaloneModernBStmp", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "",
                        useTmpReference=True)
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 208, "AthenaStandaloneOldConfigBStmp", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "",
                        useTmpReference=True)


    detectorSet(icounter, 110, "Modern", "")
    icounter += 3
    detectorSet(icounter, 210, "OldConfig", "")    
    icounter += 3    
    sliceSet(icounter, 130, "Modern", "RDO", "")
    icounter += 9
    sliceSet(icounter, 230, "OldConfig", "RDO", "")
    #icounter += 9
    #sliceSet(icounter, 160, "RDO", "ESD","TrigAnalysisTest")
    #icounter += 9
    #sliceSet(icounter, 170, "RDO", "AOD","TrigAnalysisTest")
    icounter += 9
    sliceSet(icounter, 150, "Modern", "RDO", "ESDAOD","TrigAnalysisTest")
    icounter += 9
    sliceSet(icounter, 250, "OldConfig", "RDO", "ESDAOD","TrigAnalysisTest")
    icounter += 9
    sliceSet(icounter, 170, "Modern", "BStmp", "","")
    icounter += 9
    sliceSet(icounter, 270, "OldConfig", "BStmp", "","")
    
    icounter += 9
    TrigValMakeTestConf(testConfName, icounter, 301, "AthenaAllAlgsTrueModernRDO_memTrack", 
                        ['Full'], "testAthenaRDO",  [defaultEmail], "",
                         useTmpReference=True)
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 402, "AthenaAllAlgsTrueOldConfigRDO_memTrack", 
                        ['Full'], "testAthenaRDO",  [defaultEmail], "",
                         useTmpReference=True)
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 303, "AthenaModernRDO_memTrack", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "",
                         useTmpReference=True)
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 404, "AthenaOldConfigRDO_memTrack", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "",
                         useTmpReference=True)
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 305, "AthenaModernRDOtoESDAOD_memTrack", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "TrigAnalysisTest",
                        useTmpReference=True)
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 406, "AthenaOldconfigRDOtoESDAOD_memTrack", 
                        ['Full'],"testAthenaRDO",  [defaultEmail], "TrigAnalysisTest",
                        useTmpReference=True)
    
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 351, "ElectronSliceAthenaModernRDO_memTrack", 
                        ['Full'], "testElectronSlice" , 
                        [defaultEmail], "")
    icounter += 1
    TrigValMakeTestConf(testConfName, icounter, 452, "ElectronSliceAthenaOldConfigRDO_memTrack", 
                        ['Full'], "testElectronSlice" , 
                        [defaultEmail], "")


  
    import os
    os.rename( testConfName, testConfigFullPath)
    print "trigtest config file created as "+testConfigFullPath
    print " to put this in  release do : "
    print "     mv TriggerTest/*sh ../test"
    print "     mv "+defaultPath+"/"+testConfName+" ../Testing"
    print "     mv TrigAnalysisTest/*sh ../../TrigAnalysisTest/test"


  #############################################################
  
  except IOError: 
    print "TrigValMakeTests   ERROR: Can not open "+testConfigFullPath+". will do nothing"
  
  
if __name__ == "__main__":
    main()
