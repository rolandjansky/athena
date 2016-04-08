# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from exc2string2       import exc2string2
#from Borg              import Borg
from Tools2            import isValidReleaseNumber, fixSBit
from stat              import ST_ATIME, ST_CTIME
from RTTpath           import RTTpath
from os.path           import join, isdir, isfile, islink, isabs
from popen2            import Popen4
from RTTSException     import RTTCodingError, RTTTimeOut
import os, sys, shutil, time, string, types
from ShellCommand import ShellCommand

#----------------------------------------------------
import logging
logger = logging.getLogger('rtt')
smallLogger = logging.getLogger('rtt.standalone')

#----------------------------------------------------

def timed_cmd(c,secs=-1, execute=1):
    def isTimedOut(start,secs):
        if secs == -1: return False
        return (time()-start)>secs
    from time import time
    from popen2 import Popen4

    po   = Popen4(c)
    proc = po.pid
    out  = []
    start = time()
    while po.poll() == -1:
        if isTimedOut(start, secs): 
            m = 'Command timed out(%d secs)!\n%s' % (secs, c)
            raise RTTTimeOut(m)
        out.extend(po.fromchild.readlines())
    del po
    return out

#----------------------------------------------------

def pathIsOwnedByMe(path):
    if os.path.exists(path):
        return os.getuid() == os.stat(path).st_uid
    return False
        
#----------------------------------------------------

def changePerm(path,perm):
    if pathIsOwnedByMe(path):
        os.chmod(path,fixSBit(path, perm))
    return os.stat(path).st_mode

#----------------------------------------------------

def mkdirIfNew(directory,perm=02775):
    """Creates, with given permissions, the directory 'directory'
(only if it does not already exist)
This function originally used os.makedirs, but was rewritten
as makedirs was not setting the permission perm.
"""


    perm = fixSBit(directory, perm)
    if pathIsOwnedByMe(directory):
        os.chmod(directory, perm)
        return
    else:
        directory = os.path.normpath(directory)
        head = str(directory)
        start = head
        if not isabs(head):
            m = 'Cannot extend paths: start point must be absolute: %s' % head
            logger.critical(m)
            raise RTTCodingError(m)
        tails = []
        ncount = 0
        while head != '/':
            ncount += 1
            if not  ncount % 100:
                logger.warning('Making directories for path %s, sub dir count huge %d' % (start, ncount))
            (head, tail) = os.path.split(head)
            tails.append(tail)
        tails.reverse()
        for tail in tails:
            head = os.path.join(head, tail)
            if not os.path.exists(head):
                try:
                    os.mkdir(head,perm)
                    changePerm(head,perm)
                except Exception,e :
                    msg  = 'Unable to make directory'
                    msg += '%s\n%s\nTraceBack:\n%s' %(str(head),
                                                      str(e),
                                                      exc2string2())
                    logger.error(msg)
                    raise RTTCodingError(msg)
            
# ----------------------------------------------------

def rmTreeErrorHandler(function, path, excinfo):
    m = 'shutils.rmtree error. Traceback:\n%s\npath: %s\n' % (exc2string2(), path)
    logger.error(m)

    if os.path.isdir(path):
        dirlist = os.listdir(path)
        dirlist = [os.path.join(path, f) for f in dirlist]
        for f in dirlist:
            try:
                os.remove(f)
            except:
                m = 'Could not remove %s' % f
                logger.error(m)
        
#----------------------------------------------------

def emptyDirMkDir(top,perm=02775):
    perm = fixSBit(top, perm)

    if not os.path.exists(top):
        mkdirIfNew(top,perm)
        return

    # if the dire exists,  remove all subdirectories  and files            
    [shutil.rmtree(os.path.join(top,d), False, rmTreeErrorHandler) for d in os.listdir(top) if os.path.isdir(os.path.join(top,d))]
    # remove all files
    [os.remove(os.path.join(top,f)) for f in os.listdir(top) if os.path.isfile(os.path.join(top,f))]
    


# ----------------------------------------------------
    
def rmdirMkdir(top,perm=02775):
    perm = fixSBit(top, perm)

    max = 3
    ok  = False
    while max:
        if  not os.path.exists(top):
            ok = True
            break
        max -= 1
        shutil.rmtree(top, False, rmTreeErrorHandler)
        
    if not ok:
        msg  = 'Unable to delete the directory tree below %s ' % str(top)
        logger.error(msg)
        raise RTTCodingError(msg)
        
    mkdirIfNew(top,perm)
    logger.info('Made directory %s' % top)

#takes the jobOptions+fileName and returns just the fileName
def fileNameTrim(name,jobOptions):
    """Takes the combined string jobOptions+fileName and returns just the fileName string"""
    name = name.replace(jobOptions+"_","")
    name = name.replace(jobOptions+".","")
    return name

#----------------------------------------------------

def now():
    """Returns a formatted string representing the present instant. Format example: 4May_1330"""
    return time.strftime('%d%b_%H:%M', time.localtime())
#----------------------------------------------------

def nowSecs():
    """Returns a formatted string representing the present instant. Format example: 4may_1330"""
    return time.strftime('%d %b %H:%M:%S', time.localtime())
    
#----------------------------------------------------

def isDateStampedToday(fileOrDir, midnight='00:00'):
    """Returns true if the passed file or directory is date stamped today"""
    from datetime import datetime

    # first check out "now" date
    todaysTime = time.localtime()
    year,month,day,hour,min = todaysTime[0], todaysTime[1], todaysTime[2], todaysTime[3], todaysTime[4]
    now_date = datetime(year,month,day,hour,min)

    # check out the file or dir object now
    stats = time.localtime(os.lstat(fileOrDir)[ST_CTIME])
    fyear,fmonth,fday,fhour,fmin = stats[0], stats[1], stats[2], stats[3], stats[4]
    file_or_dir_date = datetime(fyear, fmonth, fday, fhour, fmin)

    diff_date = now_date - file_or_dir_date
    if diff_date.days >= 1: return False

    midn_hour = int(midnight.split(':')[0].strip())
    midn_mins = int(midnight.split(':')[1].strip())
    
    def toSecs(h, m):
        return (h*3600) + (m*60)

    midnight_in_secs = toSecs(midn_hour, midn_mins)
    now_in_secs      = toSecs(hour, min)
    file_in_secs     = toSecs(fhour,fmin)

    if midnight_in_secs > now_in_secs and midnight_in_secs > file_in_secs:
        return now_in_secs > file_in_secs
    if midnight_in_secs < now_in_secs and midnight_in_secs < file_in_secs:
        return now_in_secs > file_in_secs
    return file_in_secs > now_in_secs

#----------------------------------------------------

# taken from the python cookbook
def unique(s):
    """ return a list of the elements in s in arbitrary order,
    but without duplicates"""

    # get the special case of empty s out of the way rapidly
    n = len(s)
    if n==0: return []

    # try using a dict - fast, usaually works
    u={}
    try:
        for x in s: u[x]=1
    except TypeError:
        del u # move to next method
    else:
        return u.keys()

    # can't hash elements: sort, and remove identical
    # elements in a single pass

    try:
        t = list(s)
        t.sort()
    except TypeError:
        del t # move to next method
    else:
        assert n>0
        last = t[0]
        lasti = i= 1
        while i<n:
            if t[i] != last:
                t[lasti] = last= t[i]
                lasti += 1
            i += 1

        return t[:lasti]

    # only brute force remains

    u = []
    for x in s:
        if x not in u:
            u.append(x)
    return u

#----------------------------------------------------

def sendMail(to,subject,message):
    """A simple mailer"""
    command = 'mail ' + str(to) + ' -s ' + '"' + str(subject) + '"'
    os.popen(command,'w').write(message)

#----------------------------------------------------

def machineNameToIPaddress(machineName):
    def legalIPformat(ipCandidate):
        toks = ipCandidate.split('.')
        if len(toks)!=4: return False
        try:
            [int(t) for t in toks]
            return True
        except:
            return False
        
    po = Popen4('host %s' % machineName)
    out = ''
    while po.poll() == -1:
        out += po.fromchild.read()
    del po
    if not out or out.find('not found')!=-1:
        return 'http://localhost:8001'
    ip = (out.split()[-1]).strip()
    if not legalIPformat(ip): return 'http://localhost:8001'
    return 'http://%s:8001' % ip

# ----------------------------------------------------------------------
