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
def send_email( RunNumber ):
    
    now = datetime.datetime.now()
    
    from_addr = 'atlidali@cern.ch'
    to_addr = [ 'Hideyuki.Oide@cern.ch', 'Matthias.Danninger@cern.ch' ]
    for addr in to_addr:
        msg = create_message(from_addr, addr, 'WARNING IDAlign missing calibration_IDTracks detected Run {0}'.format( RunNumber) , "calibration_IDTracks was not found for Run {0}.\nNeeds manual intervention.".format( RunNumber ))
        send(from_addr, addr, msg)


#----------------------------------------------------------------------------------------------------
# main
if __name__ == "__main__":
    run_number = int( sys.argv[1] )
    
    send_email( run_number )
