#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Author: Thor Taylor <pierrethor@gmail.com>
#Based on proceedure described at https://twiki.cern.ch/twiki/bin/viewauth/Atlas/FTKAthenaEmulationValidation

#USE: inupt arg is tower to be run


import sys, os, subprocess, time, glob, argparse


parser = argparse.ArgumentParser()
parser.add_argument("TowerID", help="Tower to be simulated")
args = parser.parse_args()

TowerID = args.TowerID

#make the jobs file to be sumbitted
#Currently based off a version of fullrununsplit_ttbarPU20_local_CommBank_valid.sh
#Will need to be modified if different pile-up levels are desired
fullrununsplit_ttbarPU20_local_CommBank_valid = """
#!/bin/bash

cat /etc/redhat-release

WRKDIR=$PWD

REGID=`expr {TOWERID} - 1`

# common commands
EOSCP="/afs/cern.ch/project/eos/installation/atlas/bin/eos.select cp"
EOSLS="/afs/cern.ch/project/eos/installation/atlas/bin/eos.select ls"

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

#echo "===== Environment setup ======"
#cd DIRECTORY/FTKDevValidation
#pwd
#ls
#source ./last_setup.sh
#cd $WRKDIR
# --SecondStageFit False --loadHWConf_path raw_8LbIbl.hw \
echo "====run==="
outfile="NTUP_FTKTMP_{TOWERID}.root"
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

#echo "Removing configuration files"
#rm -fv ${{pattfile[@]}} ${{fc8Lfile[@]}} ${{fc11Lfile[@]}} ${{connfile[@]}} ${{FTKINFILE}}

#echo "===== Save output ======"
#outputdir=DIRECTORY/FTKDevValidation/Run/Output
#mkdir -p $outputdir
#mv -v $outfile $outputdir
#tar --exclude="*.${{LSB_JOBID}}.${{LSB_JOBINDEX}}.out" -cvj -f $outputdir/ftksim_64Towers_jobid${{LSB_JOBINDEX}}.tbz *
"""

JobsFile = open("fullrununsplit_ttbarPU20_local_CommBank_valid.sh",'w')
JobsFile.write(fullrununsplit_ttbarPU20_local_CommBank_valid.format(TOWERID = TowerID))
JobsFile.close()

os.system("bash fullrununsplit_ttbarPU20_local_CommBank_valid.sh")

