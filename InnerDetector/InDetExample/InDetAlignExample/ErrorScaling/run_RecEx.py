#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#This is a python script, not an athena jobOptions file
# Ravi Shekhar <ravi.shekhar@duke.edu>

# This script runs multiple batch jobOptions files and combines at the end

import os, os.path
from subprocess import Popen

def parseOptions():
    from optparse import OptionParser

    usage = "usage: %prog [options] jobOptions.py jobOptions2.py ..."
    parser = OptionParser(usage=usage)
    parser.add_option('-n', dest='nSegments', help='number of batch segments', metavar="SEGMENTS")
    parser.add_option('-t', dest='itertag', help='iteration prefix (ex. iterC03)', metavar="TAG")
    parser.add_option('-q', dest='queue', help='queue used for job submission', default="atlasb1")
    parser.add_option('--estag', dest='esotag', help='error scaling override tag', metavar="TAG")
    (o,a) = parser.parse_args()

    return (o,a)

def submitjobs(prefix, nSegments, estag,queue, args):

    #setup directory structure
    if (os.path.exists(prefix)):
        if (os.path.isdir(prefix)):
            # directory exists, create directories in it
            for i in range(nSegments):
                try:
                    os.mkdir(prefix + ("/%02d" % i))
                except Exception as e:
                    print e
    else:
        try:
            os.mkdir(prefix)
            for i in range(nSegments):
                os.mkdir(prefix+ ("/%02d" % i))
        except Exception as e:
            print e
    #directories are now setup, lets launch jobs

    #we build a dictionary with 'definitions'. These are then substituted into
    vals = {}
    vals['releasetag'] = '20.1.5.8'
    vals['nSegments'] = nSegments
    vals['eso'] = estag #error scaling override
    vals['joboptions'] = ' '.join(args[0:])

    cwd = os.getcwd()
    pwd = cwd +'/'+ prefix +'/'

    for i in range(nSegments):
        # launch a shell command
        #a = Popen("cp -a datasets.py jobOptions.py  ReadInDet_jobOptions.py jobOptionsAlignmentSet.py mycool.db "
        #          + (" jobOptionsErrorTuning.py %s/%02d" % (prefix, i)), shell=True )

        #a = Popen("cp -a InDetRecAOD.root InDetRecESD.root "
        #                  + ("%s/%02d" % (prefix, i)), shell=True )  

        #print "ln -s datasets.py " + ("%s/%02d" % (prefix, i)) + "/datasets.py"
        a = Popen("ln -s " + cwd + "/datasets.py " + ("%s/%02d" % (prefix, i)) + "/datasets.py", shell=True)
        a = Popen("ln -s " + cwd + "/mycool.db " + ("%s/%02d" % (prefix, i)) + "/mycool.db", shell=True)
        #a = Popen("ln -s " + cwd + "/jobOptions.py " +  ("%s/%02d" % (prefix, i)) + "/jobOptions.py", shell=True)
        a = Popen("ln -s " + cwd + "/jobOptionsRAW.py " +  ("%s/%02d" % (prefix, i)) + "/jobOptionsRAW.py", shell=True)
        a = Popen("ln -s " + cwd + "/jobOptionsESD.py " +  ("%s/%02d" % (prefix, i)) + "/jobOptionsESD.py", shell=True)
        a = Popen("ln -s " + cwd + "/jobOptionsESD_Run2.py " +  ("%s/%02d" % (prefix, i)) + "/jobOptionsESD_Run2.py", shell=True)
        a = Popen("ln -s " + cwd + "/InDetMonitoringAlignment.py " +  ("%s/%02d" % (prefix, i)) + "/InDetMonitoringAlignment.py", shell=True)
        #a = Popen("ln -s " + cwd + "/ReadInDet_jobOptions.py " + ("%s/%02d" % (prefix, i)) + "/ReadInDet_jobOptions.py", shell=True)
        a = Popen("ln -s " + cwd + "/jobOptionsAlignmentSet.py " + ("%s/%02d" % (prefix, i)) + "/jobOptionsAlignmentSet.py", shell=True)
        a = Popen("ln -s " + cwd + "/jobOptionsErrorTuning.py " + ("%s/%02d" % (prefix, i)) + "/jobOptionsErrorTuning.py", shell=True)
        a = Popen("ln -s " + cwd + "/MisalignmentSet2.pool.root " + ("%s/%02d" % (prefix, i)) + "/MisalignmentSet2.pool.root", shell=True)
        
        #wait for command to finish, get the return code rc
        rc = os.waitpid(a.pid, 0)[1]

        fh = open('errscal.skel','r')
        s = fh.read()
        fh.close()
        vals['fullpwd'] = pwd + ("%02d" %  i)
        vals['iSegment'] = i


        jobOptionsVec = (vals['joboptions'].strip()).split()
        for ij in jobOptionsVec:
            print ij
            print len(ij.split('.'))
            if len(ij.split('.'))==1:
                jobOptionsVec.remove(ij)
            else:
                if ij.split('.')[1] != 'py':
                    jobOptionsVec.remove(ij)

        jobOptionsSt = ''

        for ij in jobOptionsVec:
            jobOptionsSt = jobOptionsSt + ij + ' '
        
        vals['joboptions'] = jobOptionsSt

        print 'The job options string is',vals['joboptions']
                
        s = s % vals # adjusts the template, substituting named variables
        fh = open(vals['fullpwd']+('/ersc%02d' % i), 'w')
        fh.write(s) # write the job back out as a job submission script in the appropriate dir
        fh.close()

        #have to chmod +x
        a = Popen("chmod +x " + vals['fullpwd']+('/ersc%02d' % i), shell=True )
        rc = os.waitpid(a.pid, 0)[1]

        # change dir, submit to batch, change back
        os.chdir(pwd + ("%02d" % i))
        command = "bsub -o batchLogFile -q %s ersc%02d " % (queue, i)
        print "Executing, %s" % command
        a = Popen(command, shell=True)
        rc = os.waitpid(a.pid, 0)[1]
        a = Popen("rm -rf done", shell=True)
        rc = os.waitpid(a.pid, 0)[1]
        os.chdir(cwd)

    completed = [False, ] * nSegments

    import time

    #yes polling is bad, but the interval is fairly high
    while ( not all(completed) ):
        for i in range(nSegments):
            if (completed[i] == False and os.path.exists(pwd + ("%02d/done" %  i))):
                completed[i] = True
            else:
                time.sleep(2.0) # slow down the afs access, bugs out sometimes
        time.sleep(30.0) # sleep 60s 

    a = Popen("hadd -T monitoring.root "+pwd+"/*/*.root" , shell=True)
    rc = os.waitpid(a.pid, 0)[1]
                
    

def main():
    (o, a ) = parseOptions()

    prefix = o.itertag
    nSegments = int(o.nSegments)
    estag = o.esotag
    queue = o.queue

#    print (prefix, nSegments, a)

    submitjobs(prefix, nSegments, estag,queue, a)


if (__name__ == '__main__'):
    main()
