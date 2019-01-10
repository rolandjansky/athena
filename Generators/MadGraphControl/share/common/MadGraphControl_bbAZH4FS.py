# based on MC15JobOptions/aMcAtNloControl_bbA4FS.py, similarly we change the H->A
# and add the support to changing mA without a new gridpack

import subprocess
import os

if not hasattr(runArgs, "inputGenConfFile"):
    raise Exception("ERROR: Please specifiy an input gridpack (e.g. --inputGenConfFile=gridpack.tar.gz)")

if not hasattr(runArgs, "randomSeed"):
    raise Exception("ERROR: Please supply a random seed (e.g. --randomSeed=5)")

try:
    nLHEvents
except:
    nLHEvents = evgenConfig.minevents

print 'aMcAtNloControl_bbA4FS nLHEvents ',nLHEvents 

try:
    component
except:
    if "ybyt" in jo_name:
        component = "bbH4FS_ybyt"
    elif "yb2" in jo_name:
        component = "bbH4FS_yb2"
    else:
        component = "bbH4FS_yb2"
        print "WARNING: component (bbH4FS_yb2 or bbH4FS_ybyt) not specified, set to yb2"

print 'aMcAtNloControl_bbA4FS component ', component
    
conf = os.path.basename(runArgs.inputGenConfFile)
if conf.endswith(".tar.gz"):
    gridpack = conf[:-7]
elif conf.endswith(".tgz"):
    gridpack = conf[:-4]
 

   
process_folder = "{}/{}/".format(gridpack, component)
print "my process_folder"
print process_folder

# check whether gridpack has expected folder
if not os.path.exists(process_folder+"bin/generate_events"):
    raise Exception("ERROR: Unexpected gridpack content.")

# configure number of events and seed
with open(process_folder+"Cards/run_card.dat", "r") as template_runcard_f:
    template_runcard_lines = template_runcard_f.readlines()
new_runcard_f = open(process_folder+"Cards/run_card.dat", "w")
for line in template_runcard_lines:
    if " = nevents " in line:
        new_runcard_f.write(" {} = nevents ! Number of unweighted events requested\n".format(int(nLHEvents))) # produce more LHE because of subchannel filter
    elif " = iseed " in line:
        new_runcard_f.write(" {} = iseed ! rnd seed (0=assigned automatically=default))\n".format(runArgs.randomSeed))
    else:
        new_runcard_f.write(line)
new_runcard_f.close()

# configure mass of A (H so far)
with open(process_folder+"Cards/param_card.dat", "r") as template_paramcard_f:
    template_paramcard_lines = template_paramcard_f.readlines()
new_paramcard_f = open(process_folder+"Cards/param_card.dat", "w")
for line in template_paramcard_lines:
    if "# MH" in line:
        new_paramcard_f.write("   25 {} # MH\n".format(mA)) # mass of A
    else:
        new_paramcard_f.write(line)
new_paramcard_f.close()

        
# generate LHE
print 'aMcAtNloControl_bbA4FS generate '
exitcode = subprocess.call(["./bin/generate_events", "--parton", "--nocompile", "--only_generation", "-f"], cwd=process_folder, stdin=subprocess.PIPE)
if exitcode != 0:
    raise Exception("LHE generation returned statuscode {}".format(exitcode))

 
# prepare LHE for showering
unpack = subprocess.call(["gzip", "-d", "events.lhe.gz"], cwd=process_folder+"Events/run_02/")
print 'aMcAtNloControl_bbA4FS unpack', unpack
if unpack != 0:
    raise Exception("Couldn't unzip events.lhe.gz")

#####
# change the H(25) to an A(36)
HtoA = subprocess.call(['sed', '-i ', 's/25  1/36  1/g', process_folder+"Events/run_02/events.lhe"]) 
print 'aMcAtNloControl_bbA4FS HtoA ',HtoA
if HtoA != 0:
    raise Exception("Error while replacing H to A")
#####

rename = subprocess.call(["mv", process_folder+"Events/run_02/events.lhe", gridpack+"._00001.events"])
print 'aMcAtNloControl_bbA4FS rename', rename
if rename != 0:
    raise Exception("Error while renaming events.lhe")

correctLHE = subprocess.call(["sed", "-i", r"s/<LesHouchesEvents version=\"1.0\">/<LesHouchesEvents version=\"3.0\">/g", gridpack+"._00001.events"])
if correctLHE != 0:
    raise Exception("Error while modifying lhe file")

evgenConfig.inputfilecheck = gridpack
runArgs.inputGeneratorFile= gridpack+"._00001.events"
