# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Utility module for things not specific to DQ
"""

from __future__ import with_statement

from contextlib import contextmanager
from logging import getLogger; log = getLogger("DQUtils.general")

from time import time, mktime, strptime, strftime, gmtime
from commands import getoutput

def get_package_version(package_name, default_prefix="DataQuality"):
    """
    Returns package version as determined from "cmt show versions"
    """
    if "/" not in package_name and default_prefix:
        package_name = "/".join([default_prefix, package_name])
    output = getoutput("cmt show versions %s" % package_name).split("\n")
    output = map(str.strip, output)
    return " ".join(output[0].split()[1:])

@contextmanager
def kgrind_profile(filename="profile.kgrind"):
    """
    Generate kcachegrind profile information for a call. 
    
    Example use:
    
    with kgrind_profile() as p:
        p.run("main()")
    """
    from cProfile import Profile
    p = Profile()
    try:
        yield p
    finally:
        from .ext.lsprofcalltree import KCacheGrind
        with open(filename,"w") as fd:
            KCacheGrind(p).output(fd)

def daemonize(function, delay, error_args=None, **kwargs):
    try:
        while True:
            log.info("Sleeping for %i seconds.." % delay)
            sleep(delay)
            
            log.info("Current time: %s" % strftime("%d/%m/%Y %H:%M:%S"))
        
            function(**kwargs)
    except:
        if error_args is not None:
            import sys
            send_error_email(sys.exc_info(), **error_args)
        raise

def send_error_email(exception_info, from_, subject, body):
    import smtplib
    import traceback
    
    from email.mime.text import MIMEText
    from getpass import getuser
    from socket import gethostname

    exceptionType, exceptionValue, exceptionTraceback = exception_info

    user, host = getuser(), gethostname()

    code = exceptionTraceback.tb_frame.f_code.co_filename

    extra_info = ["Was running as user '%s' on machine '%s'" % (user, host),
                  #"Code located at: %s" % code
    ]

    msg = MIMEText("\n\n".join(body + extra_info +
                               traceback.format_tb(exceptionTraceback)))

    msg['Subject'] = subject
    msg['From'] = from_
    
    to = [
        "Peter Waller <peter.waller@cern.ch>",
        #"Peter Onyisi <peter.onyisi@cern.ch>"
    ]
    
    operations = ("Data Quality Operations "
                  "<hn-atlas-data-quality-operations@cern.ch>")
    
    msg['To'] = ", ".join(to)
    #msg['Cc'] = operations; to.append(operations)

    s = smtplib.SMTP()
    s.connect()
    s.sendmail(from_, to, msg.as_string())
    s.quit()

    print "Error email sent."
    
def all_equal(*inputs):
    """
    Returns true if all input arguments are equal (must be hashable)
    """
    return len(set(inputs)) == 1
    
def all_permutations(input_list):
    """
    Generate all permutations of `input_list`
    """
    if len(input_list) <= 1:
        yield input_list
    else:
        for perm in all_permutations(input_list[1:]):
            for i in range(len(perm)+1):
                yield perm[:i] + input_list[0:1] + perm[i:]

# Taken from:
# http://caolanmcmahon.com/flatten_for_python
def flattened(l):
    result = _flatten(l, lambda x: x)
    while type(result) == list and len(result) and callable(result[0]):
        if result[1] != []:
            yield result[1]
        result = result[0]([])
    yield result

def _flatten(l, fn, val=[]):
    if type(l) != list:
        return fn(l)
    if len(l) == 0:
        return fn(val)
    return [lambda x: _flatten(l[0], lambda y: _flatten(l[1:],fn,y), x), val]

@contextmanager
def silence(log_level=None, logger=None):
    """
    Turn down the logging verbosity temporarily
    """
    if log_level is None:
        log_level = log.WARNING
        
    if logger is None:
        logger = log.getLogger()
        
    orig_level = logger.level
    logger.setLevel(log_level)
    
    try:
        yield
    finally:
        logger.setLevel(orig_level)

@contextmanager
def timer(name):
    "A context manager which spits out how long the block took to execute"
    start = time()
    try:
        yield
    finally:
        end = time()
        log.debug("Took %.2f to %s" % (end - start, name))                

def interleave(*args):
    return [item for items in zip(*args) for item in items]

def date_to_nanounix(date_string):
    """
    Returns number of nanoseconds between unix epoch and date in the form
    'dd/mm/yyyy'
    """
    return int(mktime(strptime(date_string, "%d/%m/%Y")) * 1e9)
    
def nanounix_to_date(nanounix):
    """
    Returns a string representation of `nanounix` nanoseconds 
    since the unix epoch
    """
    sub_second = "%03i" % ((nanounix % int(1e9)) / 1e6)
    try:
        return strftime("%Y%m%d:%H%M%S.", gmtime(nanounix / 1e9)) + sub_second
    except ValueError:
        return "[BadTime: %s]" % int(nanounix)

def daemonize(function, delay, error_args=None, **kwargs):
    """
    Run a daemon which executes `function` every `delay` seconds.
    """
    try:
        while True:
            log.info("Sleeping for %i seconds.." % delay)
            sleep(delay)
            
            log.info("Current time: %s" % strftime("%d/%m/%Y %H:%M:%S"))
        
            function(**kwargs)
    except:
        if error_args is not None:
            import sys
            send_error_email(sys.exc_info(), **error_args)
        raise

def send_error_email(exception_info, from_, subject, body):
    """
    Send an error email containing `exception_info`
    TODO: Doesn't contain complete information
    """
    import smtplib
    import traceback
    
    from email.mime.text import MIMEText
    from getpass import getuser
    from socket import gethostname

    exceptionType, exceptionValue, exceptionTraceback = exception_info

    user, host = getuser(), gethostname()

    code = exceptionTraceback.tb_frame.f_code.co_filename

    extra_info = ["Was running as user '%s' on machine '%s'" % (user, host),
                  #"Code located at: %s" % code
    ]

    msg = MIMEText("\n\n".join(body + extra_info +
                               traceback.format_tb(exceptionTraceback)))

    msg['Subject'] = subject
    msg['From'] = from_
    
    to = [
        "Peter Waller <peter.waller@cern.ch>",
        #"Peter Onyisi <peter.onyisi@cern.ch>"
    ]
    
    operations = ("Data Quality Operations "
                  "<hn-atlas-data-quality-operations@cern.ch>")
    
    msg['To'] = ", ".join(to)
    #msg['Cc'] = operations; to.append(operations)

    s = smtplib.SMTP()
    s.connect()
    s.sendmail(from_, to, msg.as_string())
    s.quit()

    print "Error email sent."
