#!/usr/bin/python
# -*- coding:utf-8 -*-

################################################################################
#
# tools.py
#
# Author: Hide Oide (Hideyuki.Oide@cern.ch), Matthias Danninger (Matthias.Danninger@cern.ch)
#
# Date: April 19, 2016
#
# Description
# 
# Common tools
#
################################################################################


import os, sys, datetime, time
import smtplib

checker_log = ""


from email.MIMEText import MIMEText
from email.Utils import formatdate

def create_message(from_addr, to_addr, subject, body):
    msg = MIMEText(body)
    msg['Subject'] = subject
    msg['From'] = from_addr
    msg['To'] =  to_addr
    msg['Date'] = formatdate()
    return msg

def send(from_addr, to_addr, msg):
    s = smtplib.SMTP()
    s.connect()
    s.sendmail(from_addr, to_addr, msg.as_string())
    s.close()


#----------------------------------------------------------------------------------------------------
def write_log( message, case="INFO" ):
    global checker_log
    now = datetime.datetime.now()
    print "{0:10s}:: {1} - {2} - {3}".format( case, now.strftime("%Y-%m-%d-%H:%M:%S"), int(time.mktime(now.timetuple())), message )
    checker_log += "{0:10s}:: {1} - {2} - {3}\n".format( case, now.strftime("%Y-%m-%d-%H:%M:%S"), int(time.mktime(now.timetuple())), message )
    
#----------------------------------------------------------------------------------------------------
def write_log_info( message ):
    write_log( message, "INFO" )
    
#----------------------------------------------------------------------------------------------------
def write_log_warning( message ):
    write_log( message, "WARNING" )
    
#----------------------------------------------------------------------------------------------------
def write_log_error( message ):
    write_log( message, "ERROR" )
    
#----------------------------------------------------------------------------------------------------
def write_log_debug( message ):
    if( os.getenv("DEBUG") == "TRUE" ):
        write_log( message, "DEBUG" )
    
#----------------------------------------------------------------------------------------------------
def write_log_fatal( message ):
    write_log( message, "FATAL" )
    
#----------------------------------------------------------------------------------------------------
def save_log( RunNumber ):
    
    now = datetime.datetime.now()
    save_path="/afs/cern.ch/user/a/atlidali/w0/calibLoop/checker/log_forEachRun/{0}_{1}.log".format( RunNumber, int(time.mktime(now.timetuple())) )
    write_log_info( "Saved log file to: {0}".format(save_path) )
    
    fout = open(save_path, "a")
    fout.write( "\n\n\n----------------------------------------------\n")
    fout.write( checker_log )
    fout.close()
    
    from_addr = 'atlidali@cern.ch'
    to_addr = [ 'Hideyuki.Oide@cern.ch', 'Matthias.Danninger@cern.ch' ]
    for addr in to_addr:
        msg = create_message(from_addr, addr, 'IDAlign CalibrationLoopChecker Log Run {0}'.format( RunNumber) , checker_log)
        send(from_addr, addr, msg)

