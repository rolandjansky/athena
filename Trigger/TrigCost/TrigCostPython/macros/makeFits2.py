#!/usr/bin/env python

print ('Hello world makeFits2.py')
import logging
logging.basicConfig(level=logging.INFO)
log = logging.getLogger('makeFits2')
log.info('################################################################################')
log.info('#')
log.info('#                          Hello World from makeFits2.py')
log.info('#')
log.info('################################################################################')
import sys
cmd = ' '.join(sys.argv)
log.info("Executing command: %s" % cmd)

# Parse command-line arguments
import optparse
preamble='usage: %prog [options]'
p = optparse.OptionParser(usage=preamble, add_help_option=False, version='%prog')
p.add_option('-i','--input',     dest='input',     type='string', default="",               help='input ntuple      [default=%default]')
p.add_option('-r','--runno',     dest='runno',     type='string', default="999999",               help='run number [default=%default]')
p.add_option('-o','--output',    dest='output',    type='string', default="xmon_fits.json", help='output directory  [default=%default]')
p.add_option('-p','--ps',        dest='ps',        type='int', default=1, help='read prescale  [default=%default]')
p.add_option('-c','--cut',        dest='cut',        type='float', default=2.0, help='acceptance scale [default=%default]')
p.add_option('-t','--triggers',  dest='triggers',  type='string', default="L1_XE60",               help='triggers          [default=%default]')
p.add_option('-?','--usage',     dest='help',      action='store_true', default=False, help='show this message and exit         [default=%default]')
p.add_option('-h','--help',      dest='help',      action='store_true', default=False, help='show this message and exit         [default=%default]')

# opts is from above, args are the arguments without a flag
(opts, args) = p.parse_args()

# Help
if opts.help:
    p.print_help()
    sys.exit(1)

log.info('Loading packages')
from ROOT import gROOT, TFile, TTree, TIter
from TrigCostPython.TrigCostFitter import TriggerFitObject 
import os
import subprocess
log.info('Starting')

input_path = os.path.join(".", opts.input)
file_ntp = TFile(input_path)
t1 = file_ntp.Get("ntp")


l1_fit_list = []
hlt_fit_list = []
trigger_list = opts.triggers.split(",")
for trig_name in trigger_list:
    if "HLT_" in trig_name: hlt_fit_list.append (TriggerFitObject(trig_name, "HLT", opts.runno))
    else:                   l1_fit_list.append  (TriggerFitObject(trig_name, "L1",  opts.runno))

F = open("filtered_fits_%s.json" % (opts.runno),"w")
os.makedirs("plots")
F.write(""""HLT_triggers": [\n""")
for p in hlt_fit_list:
    p.Read(t1, ps_value=opts.ps)
    p.GenerateGraph()
    p.FitFirst()
    p.FitGaus(scale=opts.cut)
    p.FitFiltered()
    p.Write(F)
F.write("""],\n""")
F.write(""""L1_triggers": [\n""")
for p in l1_fit_list:
    p.Read(t1, ps_value=opts.ps)
    p.GenerateGraph()
    p.FitFirst()
    p.FitGaus(scale=opts.cut)
    p.FitFiltered()
    p.Write(F)
F.write("""],\n""")
