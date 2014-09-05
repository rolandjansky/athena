#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser
import os, sys, string, math, shutil
import time, datetime, subprocess, ROOT

clean         = False
doRunRates    = False
doExampleCost = False
doCompRates   = True

def get_multi_ntuple_filenames(today, nDays):
    letters = [ 'A' , 'B' , 'C' , 'D' , 'E' , 'F' , 'G' , 'H' , 'I' , 'J' ]
    string1 = '/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_'
    string2 = '/15.6.X.Y/build/i686-slc5-gcc43-opt/p1hlt/TrigP1Test/runHLT_standaloneRTT_data_'
    string3 = '/TrigRateAL.root'
    nightly_ntuple_filenames = []
    day = today
    for i in range(7):
        this_days_ntuple_filenames = []
        for letter in letters:
            filename = string1 + str(day) + string2 + letter + string3
            if os.path.exists(filename) == False :
                print filename , 'does not exist' 
            else:
                this_days_ntuple_filenames.append(filename)
        nightly_ntuple_filenames.append(this_days_ntuple_filenames)
        day = day-1
        if day<0:
            day = 6
    return nightly_ntuple_filenames

################################################################################################################

# Parse options
p = OptionParser(usage="usage: -s <subject XML rate files> -r <reference XML rate files>  -o <output path>")
p.add_option( "-o", type="string", default = "",
              dest = "output_path",
              help = "Path to output directory." )

p.add_option( "-s", type="string", default = "",
              dest = "subjects",
              help = "Paths to subject xml files." )

p.add_option( "-r", type="string", default="",
              dest = "references",
              help = "Paths to reference xml files." )

(options, args) = p.parse_args()

reference_runs  = [ { 'run':'156682' , 'lb_beg':'450' , 'lb_end':'514' } ]

# Create output directory
# Get today's date
now = datetime.datetime.today()
output_base   = '/afs/cern.ch/user/t/trigcomm/www/rates_webpage/aidan/RTT/'
output_prefix = output_base + now.strftime('%Y_%m_%d')
if clean:
    if os.path.isdir(output_prefix):
        shutil.rmtree(output_prefix)
        print 'Removed ' , output_prefix
if not os.path.isdir(output_prefix):
    os.makedirs(output_prefix)
    print 'Made ' , output_prefix
dirname = output_prefix + '/subjects'
if not os.path.isdir(dirname):
    os.makedirs(dirname)
    print 'Made ' , dirname
dirname = output_prefix + '/references'
if not os.path.isdir(dirname):
    os.makedirs(dirname)
    print 'Made ' , dirname

# Make log file
log = open(output_prefix+'/log.html','w')
log.write('<html>\n<head>\n')
log.write('<title>Log</title>\n')
log.write('<link type="text/css" rel="stylesheet" media="all" href="style.css" />\n')
log.write('</head>\n<body>')
log.write('<h1>Log</h1>\n')
log.write('Made <tt>' + output_prefix + '</tt>\n')

cmd_log = ''

# Get nightlies
today = int(now.strftime("%w"))
nightly_ntuples = get_multi_ntuple_filenames(today, 3)

log.write('<p>Added the following nightly ntuples:</p>\n<ul>')
for ntuple in nightly_ntuples:
    for filename in ntuple:
        log.write('<li><tt>' + filename + '</tt></li>\n')
log.write('</ul>\n')

# Generate xml files from the nightlies
counter = 0
for ntuples in nightly_ntuples:
    counter = counter + 1
    if len(ntuples)==0:
        continue
    args = [ './Trigger/TrigCost/TrigCostRate/share/runRates.py' ]
    for filename in ntuples:
        args.append(filename)
    destination = output_prefix + '/subjects/' + str(counter)
    args.append('-o' + destination)
    args.append('--prescaleXML=output_xml_1e31.xml')
    args.append('--data')
    args.append('--pred-lumi=1e31')
    cmd_log = cmd_log + '> '
    for arg in args:
        cmd_log = cmd_log + ' ' + arg
    cmd_log = cmd_log + '<br />\n'
    if doRunRates:
        runRates = subprocess.Popen(args)
        runRates.wait()
        print 'Completed runRates.py on nightly ntuples.'
    
        # Clean up the unwanted HTML/JS files
        os.system('rm ' + destination + '/*.html')
        os.system('rm ' + destination + '/*.js')
        print 'Cleaned up unwanted HTML files.'
        print 'Finished with nightly ntuples.\n\n'
    else:
        print 'Skipped runRates.py on nightly ntuples.\n'

if doExampleCost:
    # Get reference xml files
    for reference_run in reference_runs:
        run    = reference_run['run']
        lb_beg = reference_run['lb_beg']
        lb_end = reference_run['lb_end']
        args = ['./Trigger/TrigCost/TrigCostPython/macros/exampleCost.py']
        args.append('--trp')
        args.append('--dorates')
        args.append('--run=' + run)
        args.append('--lb-beg=' + lb_beg)
        args.append('--lb-end=' + lb_end)
        args.append('--writexml=' + output_prefix + '/references/trp.xml')
        cmd_log = cmd_log + '> '
        for arg in args:
            cmd_log = cmd_log + ' ' + arg
        cmd_log = cmd_log + '<br />\n'
        exampleCost = subprocess.Popen(args)
        exampleCost.wait()
    
    print 'Completed exampleCost.py.\n\n'
else:
    print 'Skipped exampleCost.py.\n\n'

# Compare the nightly rate to reference and generate HTML file
args = []
args.append('./Trigger/TrigCost/TrigCostPython/macros/RTTCompRates.py')
labels_arg = '--labels='
ratios_arg = '--ratio-columns='
first = True
counter = 0
tracker = 0
for ntuples in nightly_ntuples:
    counter = counter + 1
    filename = output_prefix + '/subjects/' + str(counter) + '/' + 'TriggerCosts.xml'
    if os.path.exists(filename) == False :
        print 'Skipping ' + filename
    else:
        if tracker==0:
            args.append(filename)
            tracker = tracker + 1
            this_day = time.localtime(time.time() - 60*60*24*tracker)
            nightly_label = 'Nightly ' + str(counter) + ' ' + time.strftime('%Y%m%d', this_day)
            if first == True:
                labels_arg = labels_arg + nightly_label
                first = False
            else:
                labels_arg = labels_arg + ',' + nightly_label
for reference_run in reference_runs:
    args.append(output_prefix + '/references/trp_' + reference_run['run'] + '_' + reference_run['lb_beg'] + '_' + reference_run['lb_end'] + '.xml')
    labels_arg = labels_arg + ',Run ' + reference_run['run'] + ' ' + reference_run['lb_beg'] + '-' + reference_run['lb_end']
args.append('-o' + output_prefix)
args.append(labels_arg)
args.append('--ratio-columns=' + str(tracker+1))
args.append('--show-err')
args.append('--show-diff')

cmd_log = cmd_log + '> '
for arg in args:
    cmd_log = cmd_log + ' ' + arg
cmd_log = cmd_log + '<br />\n'
if doCompRates:
    compRates = subprocess.Popen(args)
    compRates.wait()
    print 'Completed compRates.py.\n\n'
else:
    print 'Skipped compRates.py.\n\n'

# Compare nightlies to each othrer
args = []
args.append('./Trigger/TrigCost/TrigCostPython/macros/RTTCompRates.py')
labels_arg = '--labels='
ratios_arg = '--ratio-columns='
first = True
counter = 0
tracker = 0
for ntuples in nightly_ntuples:
    counter = counter + 1
    filename = output_prefix + '/subjects/' + str(counter) + '/' + 'TriggerCosts.xml'
    if os.path.exists(filename) == False :
        print 'Skipping ' + filename
    else:
        args.append(filename)
        tracker = tracker + 1
        this_day = time.localtime(time.time() - 60*60*24*tracker)
        nightly_label = 'N' + str(counter) + ' ' + time.strftime('%Y%m%d', this_day)
        if first == True:
            labels_arg = labels_arg + nightly_label
            ratios_arg = ratios_arg + str(tracker)
            first = False
        else:
            labels_arg = labels_arg + ',' + nightly_label
            ratios_arg = ratios_arg + ',' + str(tracker)
args.append('-o' + output_prefix + '/nightlies')
args.append(labels_arg)
args.append('--ratio-columns=' + str(1))
args.append('--show-err')
args.append('--show-diff')

cmd_log = cmd_log + '> '
for arg in args:
    cmd_log = cmd_log + ' ' + arg
cmd_log = cmd_log + '<br />\n'
if doCompRates:
    compRates = subprocess.Popen(args)
    compRates.wait()
    print 'Completed compRates.py.\n\n'
else:
    print 'Skipped compRates.py.\n\n'

# Make index file
index = open(output_prefix+'/index.html','w')
log.write('Created index.html')
index.write('<html>\n<head>\n')
index.write('<title>RTT comparisons</title>\n')
index.write('<link type="text/css" rel="stylesheet" media="all" href="style.css" />')
index.write('</head>\n<body>\n')
index.write('<div id="container"\n')
index.write('  <h1>Contents</h1>\n')
index.write('  <ul>\n')
index.write('    <li><a href="compare_rate_L2.html">L1 rates comparison, nightlies vs reference</a></li>\n')
index.write('    <li><a href="compare_rate_L2.html">L2 rates comparison, nightlies vs reference</a></li>\n')
index.write('    <li><a href="compare_rate_EF.html">EF rates comparison, nightlies vs reference</a></li>\n')

index.write('    <li><a href="nightlies/compare_rate_L1.html">L1 rates comparison, nightlies vs nightlies</a></li>\n')
index.write('    <li><a href="nightlies/compare_rate_L2.html">L2 rates comparison, nightlies vs nightlies</a></li>\n')
index.write('    <li><a href="nightlies/compare_rate_EF.html">EF rates comparison, nightlies vs nightlies</a></li>\n')

index.write('    <li><a href="log.html">Log file</a></li>\n')
index.write('  </ul>\n')
index.write('</div>\n')
index.write('</body>\n')
index.write('</html>\n')
index.close() ;

# Make css file
css = open(output_prefix+'/style.css','w')
log.write('Created style.css')
css.write('pre{border:1px solid black ; padding : 10px;}')
css.write('div#container{border:1px solid black ; padding : 10px; width:800px ; margin:auto;}')

# Close log file
log.write('<h2>Command line log</h2>\n')
log.write('<pre>\n')
log.write(cmd_log)
log.write('</pre>')
log.close()

