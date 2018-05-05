

print ('Hello world makeRoot.py')
import logging
logging.basicConfig(level=logging.INFO)
log = logging.getLogger('makeRoot')
log.info('################################################################################')
log.info('#')
log.info('#                          Hello World from makeRulesFast.py')
log.info('#')
log.info('################################################################################')
import sys
cmd = ' '.join(sys.argv)
log.info("Executing command: %s" % cmd)

# Parse command-line arguments
import optparse
preamble='usage: %prog [options]'
p = optparse.OptionParser(usage=preamble, add_help_option=False, version='%prog')
p.add_option('-r','--run',      dest='run',     type='string', default=[],          help='run no: e.g., 339849  [default=%default]')
p.add_option('-o','--root',     dest='root',    type='string', default='test.root', help='output ROOT file                   [default=%default]')
p.add_option('-i','--db_path',    dest='db_path',    type='string', default="/eos/atlas/atlascerngroupdisk/tdaq-mon/coca/2017/TRP-Rates", help='input path                   [default=%default]')
p.add_option('-d','--dir',      dest='dir',     type='string', default='.',         help='output directory                   [default=%default]')
p.add_option('-?','--usage',    dest='help',        action='store_true', default=False, help='show this message and exit         [default=%default]')
p.add_option('-h','--help',     dest='help',        action='store_true', default=False, help='show this message and exit         [default=%default]')
p.add_option('-f','--full',     dest='full',        action='store_true', default=False, help='call createNtuple().C with automatic arguements         [default=%default]')


# opts is from above, args are the arguments without a flag
(opts, args) = p.parse_args()

# Help
if opts.help:
    p.print_help()
    sys.exit(1)

log.info('Using %s database to process run=%s' % ('TRP', opts.run))

log.info('Loading packages')
from ROOT import TFile, TTree, TIter
from TrigCostXmon import LumiBlockCollection
import os
import subprocess


input_path = os.path.join(opts.db_path, "TriggerRates_ATLAS_%s.root" % opts.run)
output_path = os.path.join(opts.dir, "rules_%s.root" % opts.run)
rates_path = os.path.join(opts.dir, "rates_trp_%s.root" % opts.run)

if opts.full:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    subcall = "-q -n -l -b '%s(%s)'" % (os.path.join(dirname, "createNtupleFast.C"), opts.run)
    text_file = open("runcard_%s.xmon" % opts.run, "w")
    text_file.write("INPUT %s\n" % input_path)
    text_file.write("RULES %s\n" % output_path)
    text_file.write("OUTPUT %s\n" % rates_path)
    text_file.close()

    log.info("Going to call ::: %s" % subcall)
    popen = subprocess.Popen(["root",subcall])
    popen.wait()

file_trp = TFile(input_path)
lumi_tree = file_trp.Get("Lu_Rate_Luminosity")
l1_tree = file_trp.Get("L1_Rate")
hlt_tree = file_trp.Get("HLT_Rate")

log.info ("Entires in TRP recorded Luminosity: %d" % lumi_tree.GetEntriesFast())
log.info ("Entires in TRP recorded L1 Rates: %d" % l1_tree.GetEntriesFast())
log.info ("Entires in TRP recorded HLT Rates: %d" % hlt_tree.GetEntriesFast())

lumi_branches = lumi_tree.GetListOfBranches();
for branch in lumi_branches:
    if ("TimeStamp" in branch.GetName()) : log.info ("Found TimeStamp branch in lumi")

l1_branches = l1_tree.GetListOfBranches();
for branch in l1_branches:
    if ("TimeStamp" in branch.GetName()) : log.info ("Found TimeStamp branch in LVL1")
    if ("LumiBlock" in branch.GetName()) : log.info ("Found LumiBlock branch in LVL1")

hlt_branches = hlt_tree.GetListOfBranches();
for branch in hlt_branches:
    if ("TimeStamp" in branch.GetName()) : log.info ("Found TimeStamp branch in HLT")
    if ("LumiBlock" in branch.GetName()) : log.info ("Found LumiBlock branch in HLT")

lumicol = LumiBlockCollection(0)

log.info ("Initializaing Rate Collection")
lumicol.InitializeRateCollection()

log.info (">>>>> Now adding lumi timestamp info <<<<<")
count = 0
for e in lumi_tree:
    if (count % 100 == 0) : log.info("Now processing entry == %d" % count)
    lumicol.AddLumiInfo (e.ATLAS_PREFERRED_Inst_CalibLumi_Val, e.ATLAS_PREFERRED_Inst_Mu, e.TimeStamp)
    count += 1
count = 0
log.info (">>>>> Now adding LVL1 timestamp info <<<<<")
for e in l1_tree:
    if (count % 100 == 0) : log.info("Now processing entry == %d" % count)
    lumicol.AddL1TimeInfo (e.TimeStamp)
    count += 1
count = 0
log.info (">>>>> Now adding HLT timestamp info <<<<<")
for e in hlt_tree:
    if (count % 100 == 0) : log.info("Now processing entry == %d" % count)
    lumicol.AddHLTTimeInfo (e.TimeStamp)
    count += 1

lumicol.ProcessLumiBlock()
lumicol.WriteOutRules(output_path)



print("I'm done now.")


#run_file = open("runcode_%s.xmon.sh" % opts.run, "w")
#run_file.write("#! /bin/bash")
#run_file.write(subcall)
#run_file.close()
#exec("chmod +x runcode_%s.xmon.sh" % opts.run)


#if opts.full:
    #dirname, filename = os.path.split(os.path.abspath(__file__))
    #subcall = """-l -n %s("%s","%s")""" % (os.path.join(dirname, "createNtupleFast.C"), opts.run, opts.dir)
    #print ("Executing following command now: ", subcall)
    #subprocess.call(['root',subcall])
