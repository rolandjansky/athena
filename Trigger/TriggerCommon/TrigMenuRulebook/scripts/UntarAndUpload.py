#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, os, re, time
from subprocess import Popen, PIPE, STDOUT
from os import environ
from collections import defaultdict

def execCmd(cmd):
    import shlex
    (stdoutdata, stderrdata) = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT).communicate()
    return (stdoutdata, stderrdata)


def source(script, update=1):
    if not os.path.exists(script): return
    data = Popen(". %s; env" % script, stdout=PIPE, shell=True).communicate()[0]
    env = dict(((line.split("=", 1)+[''])[0:2] for line in data.splitlines()))
    if update:
        #environ.update(env)
        environ = env


def upload(filename, smk, pstype, logfile=None):
    """
    Uploads file to database and returns psk
    """
    print "\n\n==> Uploading prescale set: %s" % filename
    if logfile: print >>logfile, "\n\n==> Uploading prescale set: %s" % filename
    tt_options="-onl -db %s" % environ['TRIGGERDB']
    upload_cmd = "java -Xms256m -Xmx1400m -jar %s %s -smk %i -psup %s" % (environ['TRIGGERTOOL'], tt_options, smk, filename)
    print upload_cmd
    if logfile: print >>logfile, upload_cmd
    (out,err) = execCmd(upload_cmd)

    if logfile:
        print >>logfile, out

    # output should contain
    # INFO: LVL1/HLT Prescale set saved with id: 6878.
    m = re.match(".*^INFO:.*Prescale set saved with id: (?P<key>[0-9]*).",out,re.S|re.M)
    if m:
        return m.groupdict()['key']
    else:
        print "ERROR!!\n:\n",out,
        from exceptions import RuntimeError
        raise RuntimeError("Prescale set was not uploaded.")
    

def setDefaultEnv(**kwargs):
    for k,v in kwargs.items():
        if not k in environ: environ[k] = v

def main():
    if len(sys.argv)!=3:
        print "You did not provide enough arguments, expected:"
        print "%s <prescales.tgz> <smk>" % sys.argv[0]
        sys.exit(1)

    # check for tarfile
    tarfile = sys.argv[1]
    if not os.path.exists(tarfile):
        print "file %s does not exist" % tarfile
        sys.exit(1)

    # get the smk from the commandline
    try:
        smk = int(sys.argv[2])
    except ValueError:
        print "specified smk %s is not an integer" % sys.argv[2]
        sys.exit(1)

    # dir with xml files
    extractDir=tarfile[:tarfile.find('.')]

    # extract tar
    execCmd('tar -zxvf %s' % tarfile)

    # open new log file
    t=time.gmtime()
    logfile="%s_upload_%02i%02i%02i%02i%02i%02i.log"  % (extractDir, t.tm_mday, t.tm_mon, t.tm_year%100, t.tm_hour, t.tm_min, t.tm_sec)
    print "output will go to %s" % logfile
    FH = open(logfile,"w")

    # source setup file to set online environment
    source('/det/tdaq/scripts/setup_trigger_tool_autoupload')

    # if not found, set defaults
    atP1 = not os.path.exists("/afs")

    if atP1:
        setDefaultEnv(
            TRIGGERTOOL='/det/tdaq/hlt/trigconf/TriggerTool/TriggerTool-new.jar',
            TRIGGERDB='TRIGGERDB',
            CORAL_AUTH_PATH='/atlas/db/.adm/coral/',
            CORAL_DBLOOKUP_PATH='/atlas/db/.adm/coral/'
            )
    else:
        setDefaultEnv(
            TRIGGERTOOL='/afs/cern.ch/user/a/attrgcnf/TriggerTool/TriggerTool.jar',
            TRIGGERDB='TRIGGERDBREPR',
            CORAL_AUTH_PATH='/afs/cern.ch/atlas/software/builds/AtlasCore/17.1.6/InstallArea/XML/AtlasAuthentication/',
            CORAL_DBLOOKUP_PATH='/afs/cern.ch/atlas/software/builds/AtlasCore/17.1.6/InstallArea/XML/AtlasAuthentication/'
            )

    s  = "Prescales in: %s\n" % (extractDir+"/")
    s += "Triggertool : TRIGGERTOOL=%s\n" % environ['TRIGGERTOOL']
    s += "Trigger DB  : TRIGGERDB=%s\n" % environ['TRIGGERDB']
    s += "DBLookup    : CORAL_DBLOOKUP_PATH=%s\n" % environ['CORAL_DBLOOKUP_PATH']
    s += "DBAuth      : CORAL_AUTH_PATH=%s\n" % environ['CORAL_AUTH_PATH']

    print s
    if FH: print >>FH, s

    p_phys = re.compile("RuleBook_(?P<level>LV1|HLT)PS_Physics_\w*_(?P<lummin>[\d\.]*)_(?P<lummax>[\d\.]*)e\d\d_(?P<bunches>\d*)b.xml")
    p_standby = re.compile("RuleBook_(?P<level>LV1|HLT)PS_(?P<type>\w*).xml")

    # upload prescales and extract keys
    keytable = {}
    for (dirname, subdirs, files) in os.walk(extractDir):
        if dirname != extractDir: continue
        for fn in sorted(files):
            if "VdM" in fn:
                fn1 = fn.split("VdM")[0]+"VdM.xml"
                os.system("cp "+dirname+"/"+fn+" "+dirname+"/"+fn1)
                fn = fn1
            m = p_phys.match(fn)
            if m:
                mr = m.groupdict()
                psk = upload(extractDir+"/"+fn, smk, 'Physics', FH)
                if not 'Physics' in keytable:
                    keytable['Physics'] = defaultdict(dict)
                keytable['Physics'][(mr['lummin'],mr['lummax'])][mr['level']] = psk
            else:
                m = p_standby.match(fn)
                if m:
                    mr = m.groupdict()
                    psk = upload(extractDir+"/"+fn, smk, mr['type'], FH)
                    if not mr['type'] in keytable:
                        keytable[mr['type']] = {}
                    keytable[mr['type']][mr['level']] = psk


    if FH:
        FH.close()


    print """
Active since %s<br>
*SMK:* %i <br>
*Release:* 17.1.<br>
*Bunchgroup:* <br>
""" % (time.strftime('%a %b %d %Y %H:%m'), smk)

    for uptype in ['Physics', 'Standby', 'Cosmic', 'VdM']:
        if uptype not in keytable.keys(): continue
        #if len(keytable[uptype])==0: continue
        print "__%s__" % uptype
        if uptype == "Physics":
            print "| *L1* | *HLT* | *Lumi Range* | *Comment* |"
            for (lumimin,lumimax),kd in sorted(keytable['Physics'].items(), reverse=True):
                l1key  = kd['LV1'] if ('LV1' in kd) else 0
                hltkey = kd['HLT'] if ('HLT' in kd) else 0
                print "| %s | %s | %s - %s |  |" % ( l1key, hltkey, lumimin, lumimax)
            print ""
        else:
            print "| *L1* | *HLT* | *Type* |"
            kd = keytable[uptype]
            l1key = kd['LV1'] if ('LV1' in kd) else 0
            hltkey = kd['HLT'] if ('HLT' in kd) else 0
            print "| %s | %s | %s |" % (l1key, hltkey, uptype)




if __name__=="__main__":
    main()
