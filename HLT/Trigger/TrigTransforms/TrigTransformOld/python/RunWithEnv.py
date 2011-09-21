#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: Alessandro DI Mattia <dimattia@mail.cern.ch>
# Module to run a command into a child python process
# after having setup a specific Atlas environment.

# --2010-12-06  Alessandro Di Mattia <dimattia@cern.ch>
#   - moved the configuration to AtlasSetup;
# --2011-02-14  Alessandro Di Mattia <dimattia@cern.ch>
#   - backward compatibility with release < 16 restored
# 2011-07-11 Graeme Stewart <graeme.andrew.stewart@cern.ch>
#  * Improved exception handling to print errors

import os, stat
import sys
import glob

from subprocess import *

def bad_arguments(*args):
        print "RunWithEnv accepts two arguments"
        print "                usage: RunWithEnv <AtlasConfiguration> <command>"
        print "                examples: RunWithEnv.py AtlasHLT,15.5.1 \'AtlCoolTrigger.py -r 131202\'"
        print "                          RunWithEnv.run(\"AtlasHLT,15.5.1\", \"AtlCoolTrigger.py -r 131202\")"
        print
        print "arguments given: ", args
        sys.exit(1)

def create_local_area_env(path=''):
        if path=='': return
        
        local_patch_area_file = file('local_patch_area.sh','w')
        
        local_patch_area_text = ['export CMTPATH=%s:$CMTPATH\n'%path,
                                 'export LD_LIBRARY_PATH=%s/InstallArea/$CMTCONFIG/lib:$LD_LIBRARY_PATH\n'%path,
                                 'export XMLPATH=%s/InstallArea/XML:$XMLPATH\n'%path,
                                 'export DATAPATH=%s/InstallArea/share:$DATAPATH\n'%path,
                                 'export PYTHONPATH=%s/InstallArea/python:$PYTHONPATH\n'%path
                                ]
        print >> local_patch_area_file, "".join(local_patch_area_text)
        local_patch_area_file.close()
        os.chmod('local_patch_area.sh',stat.S_IXUSR|stat.S_IRUSR)

def create_local_requirements():
        wdir = os.getcwd()
        requirements_file = file("requirements","w")
        file_text =         ['set   CMTSITE         \"CERN\"\n',
                        'set   CMTSITE         \"CERN\"\n',
                        'set   SITEROOT        \"/afs/cern.ch\"\n',
                        "macro ATLAS_DIST_AREA \"${SITEROOT}/atlas/software/dist\"\n",
                        "#\n",
                        "#--- Customization\n",
                        "#\n",
                        "apply_tag runtime\n",
                        "apply_tag slc5\n",
                        "apply_tag gcc43\n",
                        "####apply_tag slc4\n",
                        "apply_tag opt\n",
                        "####apply_tag gcc34\n",
                        "apply_tag 32\n",
                        "apply_tag oneTest\n",
                        "#\n",
                        "#--- group area\n",
                        "#\n",
                        "macro ATLAS_GROUP_AREA  \""+wdir+"\"\n",
                        "#\n",
                        "#--- work area\n",
                        "#\n",
                        "macro ATLAS_TEST_AREA   \""+wdir+"\"\n",
                        "#\n",
                        "#--- Atlas Login\n",
                        "#\n",
                        "use AtlasLogin AtlasLogin-* $(ATLAS_DIST_AREA)\n",
                        "#\n",
                        "\n",
                        "set CMTHOME ${HOME}/mycmtcontext\n"]
                        
        print >> requirements_file, "".join(file_text)
        requirements_file.close()

def relcycle(conf=''):
        for it in conf.split(','):
                if it.split('.')[0].isdigit(): return int(it.split('.')[0])
        return 999

def run(*args):
        
        #check arguments
        if len(args) < 2: bad_arguments(args)
        if len(args) > 3: bad_arguments(args)
                        
        conf = args[0]
        exef = args[1]
        loca = ''
        if len(args)==3: loca=args[2]
        
        #remove old requirements file
        if len(glob.glob("requirements")):
                remove = Popen(["rm","-rf","requirements"])
                try:
                    (stdout, stderr) = remove.communicate()
                except Exception, x:
                    print "Exception removing requirements: %s" % x
                    print stderr
                if remove.returncode != 0: 
                    print stderr
                    sys.exit(1)        

        #remove old local patch area file
        if len(glob.glob("local_patch_area.sh")):
                remove = Popen(["rm","-rf","local_patch_area.sh"])
                try:
                    (stdout, stderr) = remove.communicate()
                except Exception, x:
                    print "Exception removing local_patch_area.sh: %s" % x
                    print stderr
                if remove.returncode != 0: 
                    print stderr
                    sys.exit(1)        


        create_local_area_env(loca)

        cmtconf  = ""

        if relcycle(conf)<16:
                create_local_requirements()
                if not os.getenv('CMTROOT'): cmtconf="source /afs/cern.ch/sw/contrib/CMT/v1r20/mgr/setup.sh; "
                relconf  = "cmt config; source setup.sh -tag="+conf+"; "
        else:
                relconf = "export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup;source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh %s;" % conf

        if loca!='': relconf += 'source local_patch_area.sh; '
        infoconf = " echo \"CMTROOT=${CMTROOT}\"; echo \"CMTPATH=${CMTPATH}\"; echo \"PYTHONPATH=${PYTHONPATH}\"; echo \" ... running  "+exef+"\"; "

        if relcycle(conf)<16:
                command  = cmtconf+relconf+infoconf+exef
        else:
                command  = relconf+infoconf+exef


        #run command
        proc = Popen([command], shell=True,stdout=PIPE,stderr=PIPE)

        try:
            (stdout, stderr) = proc.communicate()
        except Exception, x:
            print "Exception raised executing %s: %s" % (command, x)
            return 1, stdout, stderr
        
        return proc.returncode, stdout, stderr


if __name__ == '__main__':        
        try:
            assert(len(sys.argv)>=3)
            assert(len(sys.argv)<=4)
            arg1 = sys.argv[1]
            arg2 = sys.argv[2]
            arg3 = ''
            if len(sys.argv)==4: arg3 = sys.argv[3]
        except AssertionError:
            bad_arguments(sys.argv[1:])

        try:
            (code, stdout, stderr) = run(arg1,arg2,arg3)
            print stdout
            if code!=0: sys.exit(1)
        except Exception, x:
            print "Unexpected exception in run() call: %s" % x
            print stderr
