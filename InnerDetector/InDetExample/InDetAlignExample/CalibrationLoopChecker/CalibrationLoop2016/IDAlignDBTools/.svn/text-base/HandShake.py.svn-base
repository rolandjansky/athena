#!/usr/bin/python

import os, sys, datetime, time
from getpass import getpass
import hashlib

import smtplib

from email.MIMEText import MIMEText
from email.Utils import formatdate

def create_message(from_addr, to_addr, subject, body):
    msg = MIMEText(body)
    msg['Subject'] = subject
    msg['From'] = from_addr
    msg['To'] = to_addr
    msg['Date'] = formatdate()
    return msg

def send(from_addr, to_addr, msg):
    s = smtplib.SMTP()
    s.connect()
    s.sendmail(from_addr, to_addr, msg.as_string())
    s.close()


if len(sys.argv) < 4:
    print "Usage: ./HandShake.py RunNumber Stream YourName (--exec)"
    sys.exit()

HandShakeFile="/afs/cern.ch/atlas/tzero/run/run2cafTOM/msgbox/idalignTOM.msg"
HandShakeLogFile="/afs/cern.ch/user/a/atlidali/w0/calibLoop/checker/log/HandShake_log.txt"
RunNumber= int(sys.argv[1])
stream=sys.argv[2]
name=sys.argv[3]
Message="call pdb updateDatasetPState data16_13TeV.{0:08d}.{1}.merge.RAW InDet aligned".format( RunNumber, stream )
dryrun=True
if len( sys.argv )>=5 :
    dryrun=False if sys.argv[4] == "--exec" else True

print "dryrun = ", dryrun

def do_handshake(name, Message ):
    now = datetime.datetime.now()
    out_log_message = "{0} - {1} - {2} - {3} \n".format( now.strftime("%Y-%m-%d-%H:%M:%S"), int(time.mktime(now.timetuple())), name, ( "" if dryrun==False else "(dryrun) " ) + Message ) 
    
    if dryrun==False:
        fout = open( HandShakeFile, "a" )
        fout.write( Message )
        fout.close()
        
    else:
        print "DRYRUN MODE ==> Not written out to idalignTOM.msg"
    
    from_addr = 'atlidali@cern.ch'
    to_addr = [ 'Hideyuki.Oide@cern.ch', 'Matthias.Danninger@cern.ch' ]
    for addr in to_addr:
        msg = create_message(from_addr, addr, 'IDAlign CalibrationLoop Handshake Log Run {0}'.format( RunNumber), out_log_message)
        send(from_addr, addr, msg)

    out_log = open( HandShakeLogFile, "a" )
    out_log.write( out_log_message )
    out_log.close()
    print out_log_message
    print "HandShake is done."

if name != "CRONJOB":
    proceed = raw_input("Do you really want to sign-off Run {0} {1}? [y/q]: ".format( RunNumber, stream ) )
    if proceed == "q":
        print " ==> Submission is canceled.\n"
        sys.exit()
    
    while True:
        proceed = raw_input("Are you really sure? [yes/no]: ")
        if proceed == "yes": break
        elif proceed == "no":
            print " ==> Submission is canceled.\n"
            sys.exit()
        
n_failure = 0
while True:
    if name == "CRONJOB":
        do_handshake( name, Message )
        break
    
    password = getpass("Input submission password: ")
    m = hashlib.md5()
    m.update( password )
    if( m.hexdigest() == "d192157084949cc99b50b9a028038d8f" ):
        do_handshake( name, Message )
        break
    elif n_failure == 3:
        print " ==> Failed in authentication. Take a coffee break and try again.\n"
        break
    else:
        n_failure+=1

