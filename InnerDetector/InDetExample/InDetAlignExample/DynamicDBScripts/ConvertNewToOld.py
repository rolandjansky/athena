#!/bin/python

import os
import sys

##########################################################
#             Inline Options Definition                  #
##########################################################

def optParsing():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("--inputDir",dest="inputDir", help="Point to the directory with the Alignment output",default="./")

    (config, sys.argv[1:]) = parser.parse_args(sys.argv[1:])

    return config

config = optParsing()
inputDir =config.inputDir


pwd     = os.environ['PWD']+"/"
Sifile = "%sSi.txt"%pwd
TRTfile = "%sTRT.txt"%pwd

os.system("cat %s/OutputSiGlobalFolder.txt %s/OutputSiAlignment.txt > %s"%(inputDir,inputDir,Sifile))
os.system("cat %s/OutputTRTGlobalFolder.txt %s/OutputTRTAlignment.txt > %s"%(inputDir,inputDir,TRTfile))

os.system("sed -i -e 's/AlignL1/Align/g' %s"%Sifile)
os.system("sed -i -e 's/AlignL2/Align/g' %s"%Sifile)
os.system("sed -i -e 's/AlignL3/Align/g' %s"%Sifile)

os.system("sed -i -e 's/AlignL1/Align/g' %s"%TRTfile)
os.system("sed -i -e 's/AlignL2/Align/g' %s"%TRTfile)

execCommandOptions = " -c 'FileInSi=\"%s\"; FileInTRT=\"%s\"' "%(Sifile,TRTfile)

print " <poolToTxt_converter> execCommandOptions = ", execCommandOptions 
os.system("athena.py poolToTxt_converter.py"+execCommandOptions)
