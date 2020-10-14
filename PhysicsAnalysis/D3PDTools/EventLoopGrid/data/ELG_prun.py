# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os.path
import subprocess
import re
import shlex

def ELG_prun(sample) :

    try:
        from pandatools import PandaToolsPkgInfo  # noqa: F401
    except ImportError:
        print ("prun needs additional setup, try:")
        print ("    lsetup panda")
        return 99

    cmd = ["prun"]

    #These are options that can be set by the user
    opts = ['destSE',
            'site',
            'cloud',
            'rootVer',
            'cmtConfig',
            'excludedSite',
            'nGBPerJob',
            'memory',
            'maxCpuCount',
            'nFiles',
            'nFilesPerJob',
            'nJobs',
            'maxFileSize',
            'maxNFilesPerJob',
            'addNthFieldOfInDSToLFN',
            'cpuTimePerEvent',
            'maxWalltime',
            'voms',
            'workingGroup',
            'tmpDir']

    #These are options that can be set by the user
    switches = ['useChirpServer',
                'express',
                'noSubmit',
                'skipScout',
                'disableAutoRetry',
                'useNewCode',
                'official',
                'mergeOutput',
                'useRootCore',
                'useAthenaPackages',
                'useContElementBoundary']

    for opt in opts :
        arg = sample.getMetaDouble('nc_' + opt, -1)        
        if abs(arg + 1) > 1e-6 :
            cmd += ["--" + opt + "=" + str(int(round(arg)))]
        else :
            arg = sample.getMetaString('nc_' + opt)
            if len(arg) :
                cmd += ["--" + opt + "=" + arg]
    
    for switch in switches :
        arg = sample.getMetaDouble('nc_' + switch, 0)
        if arg != 0 :
            cmd += ["--" + switch]
        else :
            arg = sample.getMetaString('nc_' + switch)
            if len(arg) :
                if arg != "False" and arg != "false" and arg != "FALSE" :
                    cmd += ["--" + switch]

    #These options should normally not be touched by the user
    internalOpts = ['exec',
                    'inDS',
                    'outDS',
                    'outputs',
                    'writeInputToTxt',
                    'match']                    

    for opt in internalOpts :
        cmd += ["--" + opt + "=" + sample.getMetaString('nc_' + opt)]

    if sample.getMetaDouble('nc_mergeOutput', 1) == 0 or sample.getMetaString('nc_mergeOutput').upper() == 'FALSE' :   
        #don't set merge script 
        pass
    else :
        cmd += ["--mergeScript=" + sample.getMetaString('nc_mergeScript')]

    if len(sample.getMetaString('nc_EventLoop_SubmitFlags')) :
        cmd += shlex.split (sample.getMetaString('nc_EventLoop_SubmitFlags'))

    if sample.getMetaDouble('nc_showCmd', 0) != 0 :
        print (cmd)

    if not os.path.isfile('jobcontents.tgz') : 
        import copy
        dummycmd = copy.deepcopy(cmd)
        dummycmd += ["--outTarBall=jobcontents.tgz"]
        dummycmd += ["--extFile=jobdef.root,runjob.sh"]
        dummycmd += ["--noSubmit"]

        try:
            out = subprocess.check_output(dummycmd, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e: 
            print ("Command:")
            print (e.cmd)
            print ("failed with return code " , e.returncode)
            print ("output was:")
            print (e.output)
            return 1

    cmd += ["--inTarBall=jobcontents.tgz"]

    out = ""
    try:
        out = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e: 
        print ("Command:")
        print (e.cmd)
        print ("failed with return code ", e.returncode)
        print ("output was:")
        print (e.output)
        return 2

    jediTaskID = 0
    try:
        line = re.findall(r'TaskID=\d+', str(out))[0]
        jediTaskID = int(re.findall(r'\d+', line)[0])
    except IndexError:
        print (out)
        return 3

    return jediTaskID
