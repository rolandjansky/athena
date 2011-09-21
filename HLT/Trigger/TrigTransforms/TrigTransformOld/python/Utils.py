# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: Hegoi Garitaonandia <hegoi.garitaonandia@cern.ch>
# Module containing some miscellanous utilities
#
# --2009-10-19  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - create_dir and exist_dir routines included

import subprocess
import logging
import time 
import os
import sys
import signal
import select

def get_year():
                year,a1,a2,a3,a4,a5,a6,a7,a8=time.localtime()
                return year

def create_recovered(basepath,runno):
        year=get_year()
        create_castor_dir(basepath)
        path=os.path.join(basepath,str(year))
        create_castor_dir(path)
        finalpath=os.path.join(path,str(runno))
        create_castor_dir(finalpath)
        return finalpath

def getRealJob(job):
        """hack"""
        return job.__dict__['_impl']

def setProcessedStatus(job):
        """dirty hack"""
        m=job.__dict__['_impl']
        m.__dict__['_data']['status']="reported"



def exist_dir(path):

        try:
                if path.find('/castor/')!=-1: cmd='nsls %s' % path
                else:                         cmd='ls %s' % path
        except:
                return False
                
        try:
                (ret,out,err)=runcmd(cmd.split(' '))
                if ret!=0: return False
                else:      return True
                
        except OSError:
                logging.warning('nsls not avaliable')



def create_dir(path):
        #check if dir already exist
        if exist_dir(path): return True
                
        try:
                cmd='rfmkdir -p '+path

                (ret,out,err)=runcmd(cmd.split(' '))
                if ret !=0:
                        logging.warning('create_dir: can not create %s' % path)
                        logging.warning(err)
                        return False

        except OSError:
                logging.warning('rfmkdir not avaliable')
                return False
        except:
                return False

        return True
        
def my_mkdir(newdir):
        """works the way a good mkdir should :)
                - already exists, silently complete
                - regular file in the way, raise an exception
                - parent directory(ies) does not exist, make them as well
        """
        if os.path.isdir(newdir):
                pass
        elif os.path.isfile(newdir):
                raise OSError("a file with the same name as the desired " \
                "dir, '%s', already exists." % newdir)
        else:
                head, tail = os.path.split(newdir)
                if head and not os.path.isdir(head):
                        my_mkdir(head)
                #print "_mkdir %s" % repr(newdir)
                if tail:
                        os.mkdir(newdir)

def killall(procname):
        (ret, out, err) = runcmd ( ['killall', '-9', procname] )

def kill(subproc):
        """Kills a child process following a softer-to-harder sequence.

        Stops a child process by killing it softly (with SIGHUP) and then, if that
        doesn't work, by doing it the hard-way (with SIGKILL)."""
        if subproc.returncode is not None:
                logging.warning('Trying to kill an already stopped subprocess. Ignored.')
                return subproc.returncode
        
        os.kill(subproc.pid, signal.SIGHUP)
        time.sleep(1) #wait a bit
        if subproc.returncode != (-1*signal.SIGHUP):
                logging.warning('Subprocess did not die with SIGHUP. Forcing...')
        
        os.kill(subproc.pid, signal.SIGKILL)
        time.sleep(0.1) #wait a bit
        if subproc.returncode != (-1*signal.SIGKILL):
                logging.error('Subprocess did not die with SIGKILL.')
        
        return subproc.returncode

def runcmd(*args, **kwargs):
        """Runs a command much like commands.getstatusoutput() would do.

        Runs a command and waits until it terminates, returning a tuple containing
        the status code, std output and standard error. This function has the same
        prototype of subprocess.Popen. It is just a replacements for
        commands.getstatusoutput()."""
        import select
        
        #for a in args:
        #        print a
        
        if 'stdout' not in kwargs.keys(): kwargs['stdout'] = subprocess.PIPE
        if 'stderr' not in kwargs.keys(): kwargs['stderr'] = subprocess.PIPE
        proc = subprocess.Popen(*args, **kwargs)
        
        try:
                (stdout, stderr) = proc.communicate()
        except select.error, e:
                logging.error('Process read interrupted (by user?): ' + e)
        return (proc.returncode, stdout, stderr)
