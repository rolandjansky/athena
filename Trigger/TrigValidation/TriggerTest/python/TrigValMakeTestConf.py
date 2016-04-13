# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


################################################################

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

def writeTestXML (testConf, testJobO, mailto, path="TriggerTest"):

# Write xml configuration for some test to temp file TriggerTest_TestConfiguration.xml.fragment
# this should then be added to TriggerTest_TestConfiguration.xml in the ATN block
# example:
#   <TEST name="ElectronSliceAthenaRDO" type="script" suite="tapm_val">
#      <options_atn>trigtest.pl --test ElectronSliceAthenaRDO --run ElectronSliceAthenaRDO --conf $ATN_PACKAGE/Testing/trigValidation.conf</options_atn>
#      <timelimit>5</timelimit>
#      <author>TAPM validation</author>
#      <expectations>
#         <errorMessage>FAILURE </errorMessage>
#         <returnValue>0</returnValue>
#      </expectations>
#   </TEST>

    import os

    if( not os.path.isdir( path )) :
       os.makedirs( path )
       
    fTestFile=path+"/"+"TriggerTest_TestConfiguration.xml.fragment"
    if( os.path.isfile( fTestFile ) ) :
        fTest = open( fTestFile,mode="a")
    else :
        fTest = open( fTestFile,mode="w")

    fTest.write("   <TEST name=\""+testJobO+"\" type=\"script\" suite=\"tapm_val\"> \n")
    fTest.write("      <options_atn>trigtest.pl --test "+testJobO+" --run "+testJobO+" --conf $ATN_PACKAGE/Testing/trigValidation.conf</options_atn> \n")
    fTest.write("      <timelimit>5</timelimit> \n")
    fTest.write("      <author>TAPM validation</author> \n")
    for address in mailto :
        if( address != "" ) :
            fTest.write("      <mailto> "+address+" </mailto> \n")
    fTest.write("      <expectations> \n")
    fTest.write("         <errorMessage>FAILURE </errorMessage> \n")    
    fTest.write("         <returnValue>0</returnValue> \n")
    fTest.write("      </expectations> \n")
    fTest.write("   </TEST> \n\n")
    fTest.close()

    print "xml fragment for test "+testJobO+" added to file "+fTestFile

################################################################
def addTrigTestConf(testConf, testId, testJobO, regtests, regtesttag,
                    post_command="", makeReferenceLink=False,useTmpReference=False) :
    from TrigValPrintRegtest import TrigValPrintRegtestSet
    fTest = open(testConf, mode = "a")
    fTest.write("test "+str(testId)+"\n")
    fTest.write("  name "+testJobO+"\n")
    fTest.write("  joboptions test"+testJobO+".py\n")
    fTest.write("  checklog\n")
    fTest.close()
    for x in regtests :
       post_command += TrigValPrintRegtestSet( testConf, x, regtesttag, makeReferenceLink,useTmpReference )

    fTest = open(testConf, mode = "a")
    if( post_command != "" ) :
       fTest.write("  post_command "+post_command+"\n")

    fTest.write("  pre_command export ERS_DEBUG_1='filter:!@default'\n")
    fTest.write("  pre_command export ERS_DEBUG_2='filter:!@default'\n")
    fTest.write("  pre_command export ERS_DEBUG_3='filter:!@default'\n")
    fTest.close()
    
    #print testConf+" is updated with test "+testJobO;
   

################################################################

def TrigValMakeTestConf(testConf, testId, testShId, testJobO, regtests,
                        regtesttag, mailto, path = "TriggerTest",post_command="",
                        makeReferenceLink=False,useTmpReference=False) :

    if( path == "" ) : path = "TriggerTest"

    # write xml fragment to be replaced in TriggerTest_TestConfiguration.xml
    writeTestXML(testConf, testJobO, mailto, path)

    addTrigTestConf(testConf, testId, testJobO, regtests, regtesttag, post_command,
                        makeReferenceLink,useTmpReference)


################################################################
