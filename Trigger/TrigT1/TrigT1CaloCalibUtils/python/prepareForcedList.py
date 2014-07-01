# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
import sys
from optparse import OptionParser

from PlotCalibrationGains import *


print "Starting prepareForcedList"

parser = OptionParser()
  
parser.add_option("-f","--InputFile",action="store",type="string",dest="input_file_name",help="Name of input file")
parser.add_option("-o","--OutputFile",action="store",type="string",dest="output_file_name",help="Name of input file")
parser.add_option("-s","--StrategyString",action="store",type="string",dest="strategy",default=None,help="Strategy string")

(options, args) = parser.parse_args()

file_input = open(options.input_file_name)
file_output = open(options.output_file_name,'w')

geometry_convertor = L1CaloGeometryConvertor()
geometry_convertor.LoadReceiverPPMMap()

if options.strategy == None:
  print "taking default strategy string"
  strategy ="GainOneOvEmecFcalLowEta"
#  strategy ="GainOneOvEmbFcalHighEta" 
else:
  strategy=options.strategy

print "Using strategy string %s, check it makes sense!!!" % strategy

for line in file_input.readlines():
  line2=line.rstrip()
  parts = line2.split(' ')
  parts_stripped = [iii for iii in parts if not iii==''] 
#  print parts_stripped
  file_output.write("%s %.3f   # %s %s %s %s \n" % (geometry_convertor.getReceiverfromPPM(parts_stripped[2],strategy),float(parts_stripped[3]), \
           parts_stripped[0],parts_stripped[1],parts_stripped[2],parts_stripped[3]))
#  print geometry_convertor.getReceiverfromPPM(parts_stripped[2]), " ", parts_stripped[3], " # ", parts_stripped


file_input.close()
file_output.close()

print "Done!"
