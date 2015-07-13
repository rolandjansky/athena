#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import getopt
import re,time,sys,cgi,string
#import cgitb; cgitb.enable()
## import test
import cx_Oracle
import datetime
import ConfigParser 
config = ConfigParser.ConfigParser()
config.read('config.ini')


def GetCurrentShifterNames():
    date = datetime.datetime.now()
    return GetShifterNames(date)

def GetShifterNames(date):

    str_date = date.strftime("%d-%b-%y").upper();# DD-MON-YY
    shifters={}
    OraclePass=config.get('Oracle', 'ShifterNamePass')
    connection = cx_Oracle.connect(OraclePass)
    cursor = connection.cursor()
    query = "select LNAME,FNAME,EMAIL from PUB_SUM_ALLOCATION_V join PUB_PERSON on ALLOCATED_PERSON_ID = PUB_PERSON.ID where task_id = :task_id  and trunc(dt) = trunc(to_date('"+str_date+"'))"
    cursor.execute(query,task_id=529752) # first shifter
    allocated_info = cursor.fetchmany()
    if not allocated_info:
        shifters["SURNAMESHIFTER1"]="NONE"
        shifters["FIRSTNAMESHIFTER1"]="NONE"
        shifters["EMAILSHIFTER1"]="NONE"
    else:
        shifters["SURNAMESHIFTER1"]=str(allocated_info[0][0].lower().title())
        shifters["FIRSTNAMESHIFTER1"]=str(allocated_info[0][1].lower().title())
        shifters["EMAILSHIFTER1"]=str(allocated_info[0][2])
    cursor.close()
    cursor = connection.cursor()
    cursor.execute(query,task_id=530361) # second shifter
    allocated_info = cursor.fetchmany()
    if not allocated_info:
        shifters["SURNAMESHIFTER2"]="NONE"
        shifters["FIRSTNAMESHIFTER2"]="NONE"
        shifters["EMAILSHIFTER2"]="NONE"
    else:
        shifters["SURNAMESHIFTER2"]=str(allocated_info[0][0].lower().title())
        shifters["FIRSTNAMESHIFTER2"]=str(allocated_info[0][1].lower().title())
        shifters["EMAILSHIFTER2"]=str(allocated_info[0][2])
    cursor.close()
    connection.close()
    return shifters

if __name__ == "__main__":
    date=datetime.datetime(2012,6,15)
    print GetShifterNames(date)
