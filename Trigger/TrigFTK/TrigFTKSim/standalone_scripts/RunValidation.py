#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Author: Thor Taylor <pierrethor@gmail.com>
#Based on proceedure described at https://twiki.cern.ch/twiki/bin/viewauth/Atlas/FTKAthenaEmulationValidation

#USE: No input arguments; Filepaths need to be edited in this script/ 
#Checks out the latest Dev version of the FTK simulation script and compares
#it to the reference version to validate it.
#Designed to run on lxplus
#Assumes CompareFTKSimDirectories.py is in TOPDIR


import sys, os, subprocess, time, glob

user = os.environ['USER']  #not currently used for anything
email = "" #the code will send an email to this address when it finishes running
loop_interval = 60 #seconds. Checks the lxbatch queue this often to see if jobs have finished running

#Location of Reference ntuples
REFERENCE = ""
#Refence ntuples form an earlier run of the simulation; 
#could be IBLPROD or an earlier, trusted, run of the DEV version.

#Working directory; Use full filepath
TOPDIR = ""
os.chdir(TOPDIR)

#update log of when this Validation has been run:
Log = open("ValidationLog.txt",'a')
Time = time.asctime()
Log.write("Validation run at %s\n" %(Time))

#------------Make bash script to setup envirnment and checout the latest Dev version---------


#clear out any old version.
subprocess.call("rm -rf FTKDevValidation/",shell=True)

#make a directory for this stuff to sit in
os.mkdir("FTKDevValidation")
os.chdir("FTKDevValidation")

WORKDIR = os.path.abspath(os.path.curdir)

#make the file to setup the right version of Athena (needs to be in a file so the bash jobs can access it)
AtlasSetupFile = open("last_setup.sh",'w')
AtlasSetupFile.write("""export AtlasSetup=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/AtlasSetup/V00-04-03/AtlasSetup
source $AtlasSetup/scripts/asetup.sh --CMTCONFIG=x86_64-slc6-gcc48-opt devval,rel_0,here,64""") 
#rel_0 grabs the latest Sunday's commit. 1 - 6 gets Mon to Sat, respectively.
AtlasSetupFile.close()

#Make & run bash script to setup envirnment and checout the latest Dev version
SetupLatestDev = '''#!/usr/bin/bash
echo "Sourcing Athena and checking out TrigFTK packages"

#setup Athena
source last_setup.sh

#Check out the latest version of the tools
cmt co Trigger/TrigFTK/TrigFTKSimTools
pkgco.py -A TrigFTKSim

#Compile them
make -C Trigger/TrigFTK/TrigFTKSimTools/cmt
cd Trigger/TrigFTK/TrigFTKSim/cmt/
cmt make
'''

DevSetupFile = open("DevSetupFile.sh",'w')
DevSetupFile.write(SetupLatestDev)
DevSetupFile.close()

os.system("bash DevSetupFile.sh")

#make a directory to run things from, and an output directory
os.chdir(WORKDIR)
os.mkdir("Run")
os.chdir("Run")
os.mkdir("Output")

#make the jobs file to be sumbitted
#Currently based off a version of fullrununsplit_ttbarPU20_local_CommBank_valid.sh
#Will need to be modified if different pile-up levels are desired
fullrununsplit_ttbarPU20_local_CommBank_valid = """
#!/bin/bash
#BSUB -J FTKC20{JOBNUMBERS}
#BSUB -q 1nh

cat /etc/redhat-release

WRKDIR=$PWD

REGID=$((LSB_JOBINDEX-1))

# common commands
EOSCP="eos cp"
EOSLS="eos ls"

#BANKDS=/afs/cern.ch/user/g/gvolpi/public/user.annovi.raw_8LcIbl123_15x16x36Ibl_8LextFrom12Lcache.ftk.2013_11_19_14_51_50.list
BANKDS=/eos/atlas/user/g/gvolpi/HWPrepBanks/PU40Bank
BASEDS=/afs/cern.ch/user/g/gvolpi/public/user.annovi.tomoya_phchang_multi_400MTracks_64reg.CONST_FTK.20140113cleanV3.list


for SUBID in {{0..3}}; do
    echo "REGID=$REGID, SUBID=$SUBID"
    echo "Get configuration files"

    pfnpath=$BANKDS/patterns_raw_8L_15x16x36Ibl_4M_reg${{REGID}}_sub${{SUBID}}.pcache.root
    pattfile[$SUBID]=$(basename $pfnpath)
    echo "$pfnpath --> ${{pattfile[$SUBID]}}"
    $EOSCP $pfnpath $PWD/${{pattfile[$SUBID]}}
    echo "pattfile$SUBID=${{pattfile[$SUBID]}}"
done

pfnpath=$(cat $BASEDS | grep -e raw_8L.*reg${{REGID}}\.gcon.bz2)
fc8Lfile=$(basename $pfnpath)
echo "$pfnpath --> ${{fc8Lfile}}"
$EOSCP $pfnpath $PWD/${{fc8Lfile}}
echo "fc8Lfile=${{fc8Lfile}}"

pfnpath=$(cat $BASEDS | grep -e raw_11L.*reg${{REGID}}\.gcon.bz2)
fc11Lfile=$(basename $pfnpath)
echo "$pfnpath --> ${{fc11Lfile}}"
$EOSCP $pfnpath $PWD/${{fc11Lfile}}
echo "fc11Lfile=${{fc11Lfile}}"

pfnpath=$(cat $BASEDS | grep -e raw_8L.*reg${{REGID}}\.conn)
connfile=$(basename $pfnpath)
echo "$pfnpath --> ${{connfile}}"
$EOSCP $pfnpath $PWD/${{connfile}}
echo "connfile=${{connfile}}"

echo "Copy input files"
INDIR=/eos/atlas/user/g/gvolpi/TDAQTDRData/group.trig-daq.mc12_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.NTUP_FTKIP.e2176_s1682_s1691_r4709_IBLwrapAllSimProfTest.140324114917
FTKINFILE=""
for ifile in {{1..4}}; do
  infile=$($EOSLS $INDIR | grep $(printf %05d $ifile).NTUP_FTKIP.root)
  echo "Copying $infile"
  $EOSCP $INDIR/$infile $PWD/$infile
  FTKINFILE="$FTKINFILE $infile"
done
echo "List of input files: $FTKINFILE"

echo "====ls===="
ls

echo "===== Environment setup ======"
cd {DIRECTORY}/FTKDevValidation
pwd
ls
source ./last_setup.sh
cd $WRKDIR
# --SecondStageFit False --loadHWConf_path raw_8LbIbl.hw \
echo "====run==="
outfile="NTUP_FTKTMP_${{LSB_JOBINDEX}}.root"
cmd="TrigFTKSM4Un_tf.py --maxEvents 400 --FTKSetupTag TDAQTDRv2 \
  --bankpatterns 1000000 --ssmap_path raw_60x32a64p2x72Ibl.ss \
  --bankregion ${{REGID}} --inputNTUP_FTKIPFile ${{FTKINFILE}} \
  --patternbank0path ${{pattfile[0]}} \
  --patternbank1path ${{pattfile[1]}} \
  --patternbank2path ${{pattfile[2]}} \
  --patternbank3path ${{pattfile[3]}} \
  --fitconstantspath ${{fc8Lfile}} \
  --fit711constantspath ${{fc11Lfile}} \
  --sectorpath ${{connfile}} \
  --CachedBank True --SaveRoads True \
  --outputNTUP_FTKTMPFile $outfile"
echo $cmd
$cmd || exit -1

echo "Removing configuration files"
rm -fv ${{pattfile[@]}} ${{fc8Lfile[@]}} ${{fc11Lfile[@]}} ${{connfile[@]}} ${{FTKINFILE}}

echo "===== Save output ======"
outputdir={DIRECTORY}/FTKDevValidation/Run/Output
mkdir -p $outputdir
mv -v $outfile $outputdir
tar --exclude="*.${{LSB_JOBID}}.${{LSB_JOBINDEX}}.out" -cvj -f $outputdir/ftksim_64Towers_jobid${{LSB_JOBINDEX}}.tbz *
"""
NeedNtuples = True
JobsToRun = "[1-64]"
#While loop reruns failed jobs until they all succeed
while NeedNtuples:
	JobsFile = open("fullrununsplit_ttbarPU20_local_CommBank_valid.sh",'w')
	JobsFile.write(fullrununsplit_ttbarPU20_local_CommBank_valid.format(JOBNUMBERS = JobsToRun, DIRECTORY = TOPDIR))
	JobsFile.close()
	
	#Submit the jobs:
	print "Submitting the jobs"
	os.system("bsub < fullrununsplit_ttbarPU20_local_CommBank_valid.sh") 
	
	
	#Wait until all the jobs have finished running
	#Assumes you don't have other jobs on lxbatch; this waits for ALL your jobs to finish
	p = subprocess.Popen("bjobs", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	stdout,stderr = p.communicate()
	bjobs_string = stdout.split('\n') + stderr.split('\n')
	
	Running = True
	while Running:
		#print bjobs_string
		if 'No unfinished job found' in bjobs_string:
			Running = False
		else:
			time.sleep(loop_interval)
			p = subprocess.Popen("bjobs", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
			stdout,stderr = p.communicate()
			bjobs_string = stdout.split('\n') + stderr.split('\n')
		
	
	#Check that the output files are all there
	Output_contents = os.listdir("Output")
	tbz_filename = "ftksim_64Towers_jobid"
	NTUP_filename = "NTUP_FTKTMP_"
	failed_jobs = False
	failed_job_numbers = []
	for i in range(1,65):
		if (tbz_filename + str(i) + ".tbz") not in Output_contents:
			failed_jobs = True
			failed_job_numbers.append(i)
		elif (NTUP_filename + str(i) + ".root") not in Output_contents:
			failed_jobs = True
			failed_job_numbers.append(i)
		
	
	if failed_jobs == True:
		print "These jobs failed:"
		print failed_job_numbers
		#os.system("""echo "Your job has failed :-(" | mail -s "Job failed" %s""" % (email))
		print "Will resumbit them"
		JobsToRun = str(failed_job_numbers)
	else:
		NeedNtuples = False
	

#Once all the ntuples have been created:
#Create the merge; maxEvents must match value in the sumbission script
os.chdir("Output")
Merge = """
echo "sourcing Athena"
source ../../last_setup.sh
echo "Merging the nTuples"
TrigFTKMergeReco_tf.py --maxEvents 400 --FTKSetupTag TDAQTDRv2 --MergeFromTowers True --inputNTUP_FTKTMPFile NTUP_FTKTMP_{1..64}.root --outputNTUP_FTKFile=NTUP_FTK.root
"""
MergeFile = open("Merge.sh",'w')
MergeFile.write(Merge)
MergeFile.close()
os.system("bash Merge.sh")

#Run the comparison script
#If you want to check subtowrs by default, this is what needs to be changed.
os.chdir(TOPDIR)
print "Running the comparison script"
RunComparison = """
source FTKDevValidation/last_setup.sh

python CompareFTKSimDirectories.py Ref {REF} Dev FTKDevValidation/Run/Output ValidationDev$(date +%Y%m%d).root
"""
RunComparisonFile = open("RunComparison.sh",'w')
RunComparisonFile.write(RunComparison.format(REF = REFERENCE))
RunComparisonFile.close()
os.system("bash RunComparison.sh")

#Announce the script has finished, and send an email:
print "Validation finished!"
os.system("""echo "The validation has finished running" | mail -s "Job finished" %s""" % (email))
