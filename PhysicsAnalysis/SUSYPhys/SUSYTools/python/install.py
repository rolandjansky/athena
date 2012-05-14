#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,subprocess

## Common packages required by SUSYTools
lpkgs = [
'svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/D3PDTools/RootCore/tags/RootCore-00-00-15 RootCore',
'svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/AnalysisCommon/ObjectSelectorCore/tags/ObjectSelectorCore-00-00-09 ObjectSelectorCore',
'svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/JetMissingEtID/JetSelectorTools/tags/JetSelectorTools-00-00-07 JetSelectorTools',
'svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections/tags/MuonEfficiencyCorrections-00-03-15 MuonEfficiencyCorrections',
'svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections/tags/MuonMomentumCorrections-00-03-02 MuonMomentumCorrections',
'svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/egamma/egammaAnalysis/egammaAnalysisUtils/tags/egammaAnalysisUtils-00-02-43 egammaAnalysisUtils', 
#Wait for PROOF usage 'svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/CalibrationDataInterface/tags/CalibrationDataInterface-00-00-09/cmt svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/CalibrationDataInterface/tags/CalibrationDataInterface-00-00-09/CalibrationDataInterface svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/CalibrationDataInterface/tags/CalibrationDataInterface-00-00-09/Root svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/CalibrationDataInterface/tags/CalibrationDataInterface-00-00-09/src CalibrationDataInterface',
'svn co svn+ssh://svn.cern.ch/reps/atlasoff/DataQuality/GoodRunsLists/tags/GoodRunsLists-00-00-94 GoodRunsLists',
'svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/AnalysisCommon/PileupReweighting/tags/PileupReweighting-00-00-16 PileupReweighting',
]

## Extra packages
# - Fake lepton background
#lpkgs += ['']

def doc():
    return """
Short instructions
==================

If you have not done so:
cd RootCore
./configure
source scripts/setup.sh
cd ..

Then every time you start a new session you should:
source RootCore/scripts/setup.sh

To compile:
$ROOTCOREDIR/scripts/find_packages.sh
$ROOTCOREDIR/scripts/compile.sh

Further informations on RootCore can be found on:
https://twiki.cern.ch/twiki/bin/view/AtlasProtected/RootCore
"""

## Check-out
def checkout(mylpkgs):
    ret = 0
    for pkg in mylpkgs:
        print 'checking-out',pkg.split()[len(pkg.split())-1]
        ret = subprocess.call(pkg, shell=True)
        if ret == 0: continue
        print pkg,'returned with code',ret
        break
    return

def main():
    checkout(lpkgs)
    ## Print short instructions
    print doc()
    return

if __name__ == "__main__":
    main()
