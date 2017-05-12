#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#=====================================================================
# Start of main code
#=====================================================================

NO_COLOUR=False
class bcolors:
    if not NO_COLOUR:
        HEADER = '\033[95m'
        OKBLUE = '\033[94m'
        #OKGREEN = '\033[32m'
        OKGREEN = '\033[38;5;28m'
        #OKRUNNING = '\033[92m'
        OKRUNNING = '\033[38;5;82m'
        WARNING = '\033[93m'
        FAIL = '\033[91m'
        ENDC = '\033[0m'
    else:
        HEADER = ''
        OKBLUE = ''
        OKGREEN = ''
        OKRUNNING = ''
        WARNING = ''
        FAIL = ''
        ENDC = ''

    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.OKRUNNING = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''

statOK=['done','finished']
statRUNNING=['running','scouting']
statSTARTING=['submitting']
statWARNING=['throttled']
statBAD=['broken','aborted']

def colourStatus(status):
    if status in statOK:
        status=bcolors.OKGREEN+status+bcolors.ENDC
    if status in statRUNNING:
        status=bcolors.OKRUNNING+status+bcolors.ENDC
    if status in statSTARTING:
        status=bcolors.OKBLUE+status+bcolors.ENDC
    if status in statWARNING:
        status=bcolors.WARNING+status+bcolors.ENDC
    if status in statBAD:
        status=bcolors.FAIL+status+bcolors.ENDC
    return status
        
def getEvtDoneTotal(reqDetails,steptype):
    totalEvt=reqDetails[steptype][3] #this is for the full chain including input and outpus DSs
    #totalEvt=reqDetails[steptype][2] #this is for the full chain for outputs only - should conserve "finished" status event decrease, doesn't work if evgen is still running
    #totalEvt=reqDetails[steptype][0] # this is what you would see in panda - as jobs from prev. step finish it increases the total
    doneEvt=reqDetails[steptype][1]
    return doneEvt,totalEvt

def rows_to_dict_list(cursor):
    #columns = [i[0] for i in cursor.description]
    columns = [i for i in cursor.description]
    #return [collections.OrderedDict(zip(columns, row)) for row in cursor]
    return columns

def checkTables(db,dbname) :
    
    try :
        tables=[]
        query = "SELECT owner, table_name FROM all_tables"
        outreq = db.execute(query)
        out = db.fetchall()
        #print out
        print
        print "Tables from %s:"%dbname
        for i in out:
            #if i[0]=='ATLAS_DEFT':
            #if i[0]=='ATLAS_PANDA':
            if i[0]==dbname:
                print i[1]
                tables.append(i[1])
        return tables
    except:
        pass

def checkTableColumns(db,dbname,table) :    

    try :
        
        #query = "select * from all_tab_cols where owner = 'ATLAS_DEFT' and table_name='%s' order by column_id " % (table)
        query = "select * from all_tab_cols where owner = '%s' and table_name='%s' order by column_id " % (dbname,table)
        print query
        outreq = db.execute(query)

        columns = db.fetchall()
        colnames=[]
        for i in range(len(columns)) :
            print columns[i][2], columns[i][3]
            colnames.append(columns[i][2])
        return colnames

    except Exception as inst:
        print type(inst)     # the exception instance
        print inst.args      # arguments stored in .args
        print inst           # __str__ allows args to be printed directly
        x, y = inst.args
        print 'x =', x
        print 'y =', y
        pass

    
def checkTableColumnsJobs(db,table) :    
    
    try :
        query = "select * from all_tab_cols where owner = 'ATLAS_PANDAARCH' and table_name='%s'" % (table)
        print query
        outreq = db.execute(query)
        columns = db.fetchall()
        for column in columns :
            print column[2], column[3]

        print out
    except:
        pass


def queryJobs(db,task) :


    outjobs=[]
    archiveDBs = ['atlas_pandaarch.jobsarchived', 'atlas_panda.jobsarchived4']
    for dbtype in archiveDBs:
        query = "select PANDAID,JOBSTATUS,CPUCONSUMPTIONTIME,NEVENTS,JOBMETRICS,CORECOUNT from %s where taskid=%s and JOBSTATUS in ('finished')" % (dbtype,task)
        try :
            outreq = db.execute(query)
        except Exception as inst:
            print type(inst)     # the exception instance
            print inst.args      # arguments stored in .args
            print inst           # __str__ allows args to be printed directly
            x, y = inst.args
            print 'x =', x
            print 'y =', y
            return False
        outjobs.extend(db.fetchall())
    return outjobs


def main():

  from datetime import datetime, timedelta
  import sys
  import collections
  import datetime
  import os
  import urllib2
  import re
  import json
  import sys
  import ast
  import csv

  import prodsys2
  p , j = prodsys2.ptr()
  jedidb=j.cursor()
  prodsysdb=p.cursor()
  
  # parse options from command line
  from optparse import OptionParser
  parser = OptionParser(usage = "usage: %prog arguments", version="%prog")
  parser.add_option("-v", "--verbose", dest="verbose", action="store_true",  help="print more information (default: %default)")
  parser.add_option("-r", "--request", dest="request", action="store",  help="request ID (default: %default)")
  parser.add_option("-b", "--brief", dest="brief", action="store_true",  help="Print brief request summary only (default: %default)")
  parser.add_option("-p", "--priority", dest="priority", action="store",  help="Search requests by priority (default: %default)")
  parser.add_option("-s", "--status", dest="status", action="store",  help="Search requests by status (default: %default)")
  parser.add_option("-S", "--step", dest="step", action="store",  help="Search requests by step (default: %default)")
  parser.add_option("-g", "--group", dest="group", action="store",  help="Search requests by group (default: %default)")
  parser.add_option("-c", "--campaign", dest="campaign", action="store",  help="Search requests by campaign (default: %default)")
  parser.add_option("-t", "--task", dest="task", action="store",  help="Search requests by task (default: %default)")
  parser.add_option("-N", "--topn", dest="topn", action="store",  help="Select only top N requests (default: %default)")
  parser.add_option("-d", "--DSID", dest="DSID", action="store",  help="Select only tasks with DSIDs in list (\",\" separated) (default: %default)")
  parser.add_option("-n", "--DSname", dest="DSname", action="store",  help="Select only tasks with search string in task name (default: %default)")
  parser.add_option("-T", "--tstatus", dest="tstatus", action="store",  help="Select only tasks with status (default: %default)")

  parser.add_option("-i", "--incsv", dest="incsv", action="store",  help="Select only requests from input csv file (default: %default)")
  parser.add_option("-o", "--output", dest="output", action="store",  help="Fill output csv file with results (default: %default)")

  parser.set_defaults(verbose=False, output=None, request=None, brief=False, priority=None, status=None, step=None, group=None, campaign=None, task=None, topn=20, DSID=None, DSname=None, tstatus=None, incsv=None)
  
  (options, args) = parser.parse_args()

  #print options
  
  if options.output:
      if os.access(options.output,os.R_OK):
          print "ERROR: Output file already exists - please remove first"
          return
           



  #restrict to just one request
  reqQuery=""
  if options.request:
      reqQuery=" and PR_ID in (%s)"%options.request

  if options.incsv:
      reqList=[]

      with open(options.incsv, 'rb') as csvfile:
         csvreader = csv.reader(csvfile, delimiter=',', quotechar='"')
         for row in csvreader:
             #print ', '.join(row)
             if not row[0].isdigit():
                 #skip header
                 continue
             reqList.append(row[0])
      if options.verbose: print ",".join(reqList)
      reqQuery=" and PR_ID in (%s)"%(",".join(reqList))
      csvfile.close()


  status=[]
  if options.status:
      for i in options.status.split(','):
          status.append(i)

  if options.verbose:
      print
      prodsystables=checkTables(prodsysdb,'ATLAS_DEFT')
      for table in prodsystables:
          print
          checkTableColumns(prodsysdb,'ATLAS_DEFT',table)
      print
      jeditables=checkTables(jedidb,'ATLAS_PANDA')     
      
      taskcols=checkTableColumns(jedidb,'ATLAS_PANDA','JEDI_DATASETS')
      #taskcols=checkTableColumns(jedidb,'ATLAS_PANDA','JEDI_TASKS')
      checkTableColumnsJobs(jedidb,'JOBSARCHIVED')
      # return
  

  #try:
  if True:

      reqDict=collections.OrderedDict()
      if not options.task:
          
          top=" where rownum <= %i"%int(options.topn)
          top=""
          if options.topn==-1:
              top=""
          qry = """
select PR_ID,MANAGER,STATUS,REQUEST_TYPE,CAMPAIGN,PHYS_GROUP,INFO_FIELDS,REFERENCE,DESCRIPTION from T_PRODMANAGER_REQUEST 
 where REQUEST_TYPE='MC' and 'VALI' not in PHYS_GROUP 
 """+reqQuery+"""
order by PR_ID desc"""+top
              
          if options.verbose :
              print qry
          req = prodsysdb.execute(qry)
          #requests = prodsysdb.fetchall()
          
          allRequests = {}
         
               
          request = prodsysdb.fetchone()
          while request:

              if options.verbose :
                  print request   
                  
              if request[2] == 'cancelled' or request[2] == 'test':
                  if options.verbose: print "Request %i ignored because status %s is in 'finished'|'cancelled'|'test'"%(request[0],request[2])
                  request = prodsysdb.fetchone()
                  continue

              #filter by status
              if len(status) and request[2] not in status:
                  if options.verbose: print "Ignoring request %i because status (%s) not in"%(request[0],request[2]),status
                  request = prodsysdb.fetchone()
                  continue


              lob=None
              priority=None

              if request[6]:
                  lob= request[6].read()
                  lob=ast.literal_eval(lob)
                  if 'priority' in lob:
                      priority=lob['priority']

                  if options.verbose :
                      print "lob:",lob

                  #filter by priority
                  if options.priority:
                      passPrio=False
                      if 'priority' in lob:
                          for x in lob['priority'].split(','):
                              if x<=options.priority:
                                  passPrio=True
                                  break
                      else:
                          print "WARNING: No priority could be extracted from request %i - ignoring"%request[0]
                          for i in lob:
                              print i,lob[i]
                          request = prodsysdb.fetchone()
                          continue

                      if not passPrio:
                          if options.verbose: print "Request %i ignored because priority %s > %s"%(request[0],lob['priority'],options.priority)
                          request = prodsysdb.fetchone()
                          continue
              else:
                  print "WARNING: No lob found for request %i - ignoring"%request[0]
                  request = prodsysdb.fetchone()
                  continue
              
              #set group
              group = str(request[5]).upper()[:4]
              if request[5] == 'Top' :
                  group = 'TOPQ'

              #filter by group
              if options.group and options.group != group:
                  if options.verbose: print "Request %i ignored because group %s != %s"%(request[0],group,options.group)
                  request = prodsysdb.fetchone()
                  continue

              #filter by campaign
              if options.campaign and options.campaign != request[4]:
                  if options.verbose: print "Request %i ignored because campaign %s != %s"%(request[0],request[4],options.campaign)
                  request = prodsysdb.fetchone()
                  continue

          
              reqDetails={}
              reqDetails['manager'] = request[1]
              reqDetails['status'] = request[2]
              reqDetails['campaign'] = request[4]
              reqDetails['group'] = group
              reqDetails['priority'] = priority
              reqDetails['description'] = request[8]
              reqDetails['tasks'] = collections.OrderedDict()
              reqDetails['info'] = lob
              reqDetails['Pile'] = [0,0,0,0]
              reqDetails['Simul'] = [0,0,0,0]
              reqDetails['Evgen'] = [0,0,0,0]
              reqDetails['All'] = [0,0,0,0]
              reqDetails['allPcNow'] = 0.
              reqDetails['allPcGuess'] = 0.

              reqDict[request[0]]=reqDetails

              
              #get next row
              request = prodsysdb.fetchone()
              #break
      else:
          print "ERROR: Task search not yet fully implemented"
          return

      #Now loop through requests to look at tasks
      for reqId,reqInfo in reqDict.iteritems():

          
          if not options.task:
              searchCriteria='ATLAS_PANDA.JEDI_TASKS.REQID=%s'%reqId
          else:
              searchCriteria='ATLAS_PANDA.JEDI_TASKS.JEDITASKID=%s'%options.task
              
          indsnevents="""
(select SUM(JEDI_DATASETS.NEVENTS) from ATLAS_PANDA.JEDI_DATASETS 
 where ATLAS_PANDA.JEDI_TASKS.JEDITASKID = ATLAS_PANDA.JEDI_DATASETS.JEDITASKID 
   and ATLAS_PANDA.JEDI_DATASETS.TYPE IN ('input', 'pseudo_input') 
   and (ATLAS_PANDA.JEDI_DATASETS.STREAMNAME is null or ATLAS_PANDA.JEDI_DATASETS.STREAMNAME not in ('IN_MINBIAS_LOW','IN_MINBIAS_HIGH','SEQNUMBER'))
) as INDSNEVENTS"""

          outdsnevents="""
(select SUM(JEDI_DATASETS.NEVENTS) from ATLAS_PANDA.JEDI_DATASETS 
 where ATLAS_PANDA.JEDI_TASKS.JEDITASKID = ATLAS_PANDA.JEDI_DATASETS.JEDITASKID 
   and ATLAS_PANDA.JEDI_DATASETS.TYPE in ('output') and ATLAS_PANDA.JEDI_DATASETS.STREAMNAME in ('OUTPUT0') 
) as OUTDSNEVENTS"""


          qry = """
select ATLAS_PANDA.JEDI_TASKS.JEDITASKID,
ATLAS_PANDA.JEDI_TASKS.TASKNAME,
ATLAS_PANDA.JEDI_TASKS.STATUS,
ATLAS_PANDA.JEDI_TASKS.CORECOUNT,
ATLAS_PANDA.JEDI_TASKS.CURRENTPRIORITY,
ATLAS_PANDA.JEDI_TASKS.PARENT_TID,
"""+indsnevents+""", 
"""+outdsnevents+""" 
from ATLAS_PANDA.JEDI_TASKS 
where ATLAS_PANDA.JEDI_TASKS.TASKTYPE='prod' 
  and %s order by ATLAS_PANDA.JEDI_TASKS.JEDITASKID """ % searchCriteria


          #qry="select * from ATLAS_PANDA.JEDI_DATASETS where ATLAS_PANDA.JEDI_DATASETS.JEDITASKID=10373957 and ATLAS_PANDA.JEDI_DATASETS.TYPE IN ('output')"
          #qry="select * from ATLAS_PANDA.JEDI_TASKS where ATLAS_PANDA.JEDI_TASKS.JEDITASKID=10186324"
          #qry="select * from ATLAS_PANDA.JEDI_DATASETS where ATLAS_PANDA.JEDI_DATASETS.JEDITASKID=10368557 and ATLAS_PANDA.JEDI_DATASETS.TYPE IN ('input', 'pseudo_input') and ATLAS_PANDA.JEDI_DATASETS.STREAMNAME not in ('IN_MINBIAS_LOW','IN_MINBIAS_HIGH')"           
          #qry="select * from ATLAS_PANDA.JEDI_TASKS where ATLAS_PANDA.JEDI_TASKS.JEDITASKID=10366748"           
          #qry="select * from ATLAS_PANDA.JEDI_DATASETS where ATLAS_PANDA.JEDI_DATASETS.JEDITASKID=10366748"           
          #qry="(select SUM(NEVENTS) from (select NEVENTS from ATLAS_PANDAARCH.JOBSARCHIVED where TASKID=ATLAS_PANDA.JEDI_TASKS.JEDITASKID and JOBSTATUS in ('finished') union all select NEVENTS from ATLAS_PANDA.JOBSARCHIVED4 where TASKID=ATLAS_PANDA.JEDI_TASKS.JEDITASKID and JOBSTATUS in ('finished') ) ) as INDSEVTJOBS"
          db=jedidb
          
          if options.verbose :
              print qry
              
          req = db.execute(qry)
          tasks = db.fetchall()

          taskDict=collections.OrderedDict()
        
          for task in tasks :

              if options.verbose: 
                  print task
                  #for i in range(len(task)):
                  #    #print indscols[i],task[i]
                  #    print taskcols[i],task[i]
                  #    #print stepcols[i],task[i]
                  #continue    
                  #print taskDict

              if len(task) < 8:
                  print "ERROR: Row only has %i (< 8) columns, skipping"%len(task)
                  continue

              tags=task[1].lower().split('.')[-1]
              if tags.split('_')[-1][0]=='p':
                  if options.verbose: print "Ignoring p-step:",task[1]
                  continue
              

              if len(task[1].split('.'))<3:
                  if options.verbose: print "ERROR: badly formed taskname: %s"%task[1]
                  continue
              else:
                  steptype=task[1].lower().split('.')[-2]



              if options.DSID and task[1].split('.')[1] not in options.DSID.split(","):
                  if options.verbose: print "Ignoring task %i because DSID %s != %s"%(task[0],task[1].split('.')[1],options.DSID)
                  continue

              if options.DSname and options.DSname not in task[1]:
                  if options.verbose: print "Ignoring task %i because search string %s not in %s"%(task[0],options.DSname,task[1])
                  continue



              nevents=task[6] if task[6] else 0.

              #Query jobs to get input for evgen (now not needed and significantly slower!)
              if False and steptype=='evgen':
                  qry="""
select SUM(NEVENTS) from (select PANDAID,TASKID,NEVENTS from ATLAS_PANDAARCH.JOBSARCHIVED 
 where TASKID = %s and JOBSTATUS in ('finished') 
union 
select PANDAID,TASKID,NEVENTS from ATLAS_PANDA.JOBSARCHIVED4 
 where TASKID=%s and JOBSTATUS in ('finished') )"""%(task[0],task[0])

                  jobreq = db.execute(qry)
                  jobs = db.fetchall()
                  if options.verbose: print jobs
                  nevents=jobs[0][0] if jobs[0][0] else 0.


              taskDetails={'TASKNAME':task[1],
                           'STATUS':task[2],
                           'CORECOUNT':task[3],
                           'CURRENTPRIO':task[4],
                           'PARENTTID':task[5],
                           'ORIGPARENTTID':task[5],
                           'INDSNEVENTS':nevents,
                           'MAXINDSNEVENTS':nevents,
                           'OUTDSNEVENTS':task[7] if task[7] else 0.,
                           'MAXOUTDSNEVENTS':task[7] if task[7] else 0.,
                           'STEP':steptype
                           }       

              #aborted/broken tasks where other identically named tasks exist are assumed to be retries - the orignals are ignored
              if task[2] in ['aborted','broken']:
                  sametnames=[t[0] for t in tasks if t[1] == task[1]]
                  if len(list(set(sametnames).difference(set([task[0]])))):
                      if options.verbose: print "Ignoring aborted/broken task %s because it seems to have been retried:"%task[0],list(sametnames)
                      continue

              #You made it! Add task to dictionary for request
              taskDict[task[0]]=taskDetails
            

              if options.verbose :
                  print task[0],taskDetails

          #try to construct nevents from previous steps(s) if INDSEVENTS=0
          for taskid,taskInfo in taskDict.iteritems():

              counter=0
              while taskDict[taskid]['INDSNEVENTS']==0.:
                  if taskDict[taskid]['PARENTTID'] in taskDict:
                      if options.verbose: print "newin",taskid,':',taskDict[taskid]['PARENTTID'],taskDict[taskid]['INDSNEVENTS'],taskDict[taskInfo['PARENTTID']]['INDSNEVENTS']
                      taskDict[taskid]['INDSNEVENTS']=taskDict[taskDict[taskid]['PARENTTID']]['INDSNEVENTS']
                      taskDict[taskid]['PARENTTID']=taskDict[taskDict[taskid]['PARENTTID']]['PARENTTID']
                  counter=counter+1
                  if counter==10:
                      print "ERROR: Did not reconstruct parentage correctly"
                      break


              ##Check if nevents in previous steps is larger

              # use outds because you want the lower events in finished tasks to be taken into account
              counter=0
              taskDict[taskid]['PARENTTID']=taskDict[taskid]['ORIGPARENTTID']
              #while  taskDict[taskid]['PARENTTID'] in taskDict and taskDict[taskid]['MAXOUTDSNEVENTS'] < taskDict[taskDict[taskid]['PARENTTID']]['OUTDSNEVENTS']:
              while  taskDict[taskid]['PARENTTID'] in taskDict:
                  if options.verbose: print "maxout",taskid,':',taskDict[taskid]['PARENTTID'],taskDict[taskid]['MAXOUTDSNEVENTS'],taskDict[taskInfo['PARENTTID']]['OUTDSNEVENTS']
                  if taskDict[taskid]['MAXOUTDSNEVENTS'] < taskDict[taskDict[taskid]['PARENTTID']]['OUTDSNEVENTS']:
                      taskDict[taskid]['MAXOUTDSNEVENTS']=taskDict[taskDict[taskid]['PARENTTID']]['OUTDSNEVENTS']
                  if taskDict[taskid]['PARENTTID'] == taskDict[taskDict[taskid]['PARENTTID']]['PARENTTID']:
                      if options.verbose: print "reached first step",taskDict[taskid]['PARENTTID']
                      break

                  taskDict[taskid]['PARENTTID']=taskDict[taskDict[taskid]['PARENTTID']]['PARENTTID']
                  #if taskDict[taskid]['PARENTTID'] in taskDict:
                  #    taskDict[taskid]['MAXOUTDSNEVENTS']=taskDict[taskDict[taskid]['PARENTTID']]['OUTDSNEVENTS']
                  #    taskDict[taskid]['PARENTTID']=taskDict[taskDict[taskid]['PARENTTID']]['PARENTTID']

                  counter=counter+1
                  if counter==10:
                      print "ERROR: Did not reconstruct parentage correctly - exiting"
                      break

              # use inds to be most conservative with guess percentage
              counter=0
              taskDict[taskid]['PARENTTID']=taskDict[taskid]['ORIGPARENTTID']
              #while  taskDict[taskid]['PARENTTID'] in taskDict and taskDict[taskid]['MAXINDSNEVENTS'] < taskDict[taskDict[taskid]['PARENTTID']]['INDSNEVENTS']:
              while  taskDict[taskid]['PARENTTID'] in taskDict:
                  if options.verbose: print "maxin",taskid,':',taskDict[taskid]['PARENTTID'],taskDict[taskid]['MAXINDSNEVENTS'],taskDict[taskInfo['PARENTTID']]['INDSNEVENTS'] 
                  if taskDict[taskid]['MAXINDSNEVENTS'] < taskDict[taskDict[taskid]['PARENTTID']]['INDSNEVENTS']:
                      taskDict[taskid]['MAXINDSNEVENTS']=taskDict[taskDict[taskid]['PARENTTID']]['INDSNEVENTS']
                  if taskDict[taskid]['PARENTTID'] == taskDict[taskDict[taskid]['PARENTTID']]['PARENTTID']:
                      if options.verbose: print "reached first step",taskDict[taskid]['PARENTTID']
                      break

                  taskDict[taskid]['PARENTTID']=taskDict[taskDict[taskid]['PARENTTID']]['PARENTTID']

                  #if taskDict[taskid]['PARENTTID'] in taskDict:
                  #    taskDict[taskid]['MAXINDSNEVENTS']=taskDict[taskDict[taskid]['PARENTTID']]['INDSNEVENTS']
                  #    taskDict[taskid]['PARENTTID']=taskDict[taskDict[taskid]['PARENTTID']]['PARENTTID']

                  counter=counter+1
                  if counter==10:
                      print "ERROR: Did not reconstruct parentage correctly - exiting"
                      break



          #Sum done / total events
          for taskid,taskInfo in taskDict.iteritems():

              #this has to be done here because otherwise looking back up the chain doesn't work!
              if options.step and taskInfo['STEP']!=options.step:
                  if options.verbose: print "Ignoring task %s because step %s != %s"%(taskid,taskInfo['STEP'],options.step)
                  continue


              #check task status
              if options.tstatus and taskInfo['STATUS']!=options.tstatus:
                  if options.verbose: print "Ignoring task %s because status %s != %s"%(taskid,taskInfo['STATUS'],options.step)
                  continue

              reqInfo['tasks'][taskid]=taskInfo

              if taskInfo['STATUS'] in ['aborted','broken']:
                  if options.verbose: print "Ignoring task %i for event totals because status = %s"%(taskid,taskInfo['STATUS'])
                  continue



              if taskInfo['STEP']=='evgen':
                  reqInfo['Evgen'][0] += int(taskInfo['INDSNEVENTS'])
                  reqInfo['Evgen'][1] += int(taskInfo['OUTDSNEVENTS'])
                  reqInfo['Evgen'][2] += int(taskInfo['MAXOUTDSNEVENTS'])
                  reqInfo['Evgen'][3] += int(taskInfo['MAXINDSNEVENTS'])
              elif taskInfo['STEP']=='simul':
                  reqInfo['Simul'][0] += int(taskInfo['INDSNEVENTS'])
                  reqInfo['Simul'][1] += int(taskInfo['OUTDSNEVENTS'])
                  reqInfo['Simul'][2] += int(taskInfo['MAXOUTDSNEVENTS'])
                  reqInfo['Simul'][3] += int(taskInfo['MAXINDSNEVENTS'])
              elif taskInfo['STEP']=='recon':
                  reqInfo['Pile'][0] += int(taskInfo['INDSNEVENTS'])
                  reqInfo['Pile'][1] += int(taskInfo['OUTDSNEVENTS'])
                  reqInfo['Pile'][2] += int(taskInfo['MAXOUTDSNEVENTS'])
                  reqInfo['Pile'][3] += int(taskInfo['MAXINDSNEVENTS'])
              reqInfo['All'][0] += int(taskInfo['INDSNEVENTS'])
              reqInfo['All'][1] += int(taskInfo['OUTDSNEVENTS'])
              reqInfo['All'][2] += int(taskInfo['MAXOUTDSNEVENTS'])
              #reqInfo['All'][3] += int(taskInfo['MAXINDSNEVENTS'])
              reqInfo['All'][3] += int(max(taskInfo['MAXOUTDSNEVENTS'],taskInfo['MAXINDSNEVENTS']))


          evgenDoneEvt,evgenTotalEvt=getEvtDoneTotal(reqInfo,'Evgen')
          simulDoneEvt,simulTotalEvt=getEvtDoneTotal(reqInfo,'Simul')
          pileDoneEvt,pileTotalEvt=getEvtDoneTotal(reqInfo,'Pile')
          allDoneEvt,allTotalEvt=getEvtDoneTotal(reqInfo,'All')
          
          statusset=set([reqInfo['tasks'][task]['STATUS'].strip() for task in reqInfo['tasks']])


          evgenPc   =100.*evgenDoneEvt/evgenTotalEvt if evgenTotalEvt else 0.
          simulPc   =100.*simulDoneEvt/simulTotalEvt if simulTotalEvt else 0.
          pilePc    =100.*pileDoneEvt/pileTotalEvt if pileTotalEvt else 0.
          allPcNow  =100.*allDoneEvt/reqInfo['All'][0] if reqInfo['All'][0] else 0.
          #allPcGuess=100.*allDoneEvt/max(reqInfo['All'][3],reqInfo['All'][2],reqInfo['All'][0]) if reqInfo['All'][3] or reqInfo['All'][2] or reqInfo['All'][0] else 0.
          allPcGuess=100.*allDoneEvt/reqInfo['All'][3] if reqInfo['All'][3] else 0.


          evgenPc   =100.*evgenDoneEvt/evgenTotalEvt if evgenTotalEvt else 0.
          simulPc   =100.*simulDoneEvt/simulTotalEvt if simulTotalEvt else 0.
          pilePc    =100.*pileDoneEvt/pileTotalEvt if pileTotalEvt else 0.
          allPcNow  =100.*allDoneEvt/reqInfo['All'][0] if reqInfo['All'][0] else 0.
          #allPcGuess=100.*allDoneEvt/max(reqInfo['All'][3],reqInfo['All'][2],reqInfo['All'][0]) if reqInfo['All'][3] or reqInfo['All'][2] or reqInfo['All'][0] else 0.
          allPcGuess=100.*allDoneEvt/reqInfo['All'][3] if reqInfo['All'][3] else 0.

          
          
          reqInfo['allPcNow'] = allPcNow
          reqInfo['allPcGuess'] = allPcGuess


          if options.brief:
              #brief format of output
              print "REQUEST %i - Summary: Evgen %.2f%% (%i) | Simul %.2f%% (%i) | Pile %.2f%% (%i), All now %.2f%% (guess %.2f%%)" % ( 
                  reqId, 
                  evgenPc, evgenTotalEvt, 
                  simulPc, simulTotalEvt,
                  pilePc, pileTotalEvt, 
                  allPcNow,
                  allPcGuess)

              if reqInfo['status']=='finished':             
                  print "REQUEST %i - status finished"%reqId
                  reqInfo['allPcNow'] = 100.
                  reqInfo['allPcGuess'] = 100.
              elif not len(statusset.difference(set(['finished','done']))):
                  print "REQUEST %i - All tasks finished or done"%reqId
                  reqInfo['allPcNow'] = 100.
                  reqInfo['allPcGuess'] = 100.
              
          else:
              #full output
              extra=""
              if reqInfo['status']=='finished':
                  extra="- status finished"
              elif not len(statusset.difference(set(['finished','done']))):
                  extra="- All tasks finished or done"

              print "REQUEST %i %s"%(reqId,extra)
              print "    Description: %s"%reqInfo['description']
              print "    Status:      %s"%reqInfo['status']
              print "    Group:       %s"%reqInfo['group']
              print "    Campaign:    %s"%reqInfo['campaign']
              print "    Priority:    %s"%reqInfo['priority']
              print "    Events summary: Evgen %.2f%% (%i) | Simul %.2f%% (%i) | Pile %.2f%% (%i), All now %.2f%% (guess %.2f%%)" % ( 
                  evgenPc, evgenTotalEvt, 
                  simulPc, simulTotalEvt,
                  pilePc, pileTotalEvt, 
                  allPcNow,
                  allPcGuess)

              if len(reqInfo['tasks']):
                  print "    Tasks (%i):"%len(reqInfo['tasks'])
                  print "    %-9s: %-90s | %-10s %-10s | %-4s %-7s | %-6s"%("TID","Taskname","EvtTot now","(guess)","%now","(guess)","Status")
                  print "    ----------------------------------------------------------------------------------------------------------------------------------------------------"
                  for tid,taskDetails in reqInfo['tasks'].iteritems():
                      taskDetails['STATUS']=colourStatus(taskDetails['STATUS']) 
                      print "    %-9s: %-90s | %-10s %-10s | %-4s %-7s | %-8s"%(
                          tid,
                          ".".join(taskDetails['TASKNAME'].split('.')[1:]),
                          #str(taskDetails['INDSNEVENTS']),
                          str(taskDetails['OUTDSNEVENTS']),
                          "("+str(max(taskDetails['MAXINDSNEVENTS'],taskDetails['MAXOUTDSNEVENTS'],taskDetails['INDSNEVENTS']))+")",
                          str(int(100.*taskDetails['OUTDSNEVENTS']/taskDetails['INDSNEVENTS'] if taskDetails['INDSNEVENTS'] else 0.))+"%",
                          "("+str(int(100.*taskDetails['OUTDSNEVENTS']/max(taskDetails['MAXINDSNEVENTS'],taskDetails['MAXOUTDSNEVENTS'],taskDetails['INDSNEVENTS']) if taskDetails['INDSNEVENTS'] or taskDetails['MAXINDSNEVENTS'] or taskDetails['MAXOUTDSNEVENTS'] else 0.))+"%)",
                          taskDetails['STATUS'])
              else:
                  print "No tasks found"


      if options.output:
          if options.incsv:
              outfile=open(options.output, 'a')
              with open(options.incsv, 'rb') as csvfile:
                 csvreader = csv.reader(csvfile, delimiter=',', quotechar='"')
                 for row in csvreader:
                     #print ', '.join(row)
                     if not row[0].isdigit():
                         #skip header
                         continue
                     for reqId,reqInfo in reqDict.iteritems():
                         if row[0] and int(row[0]) == int(reqId):
                             row.append(str(reqInfo['allPcGuess']))
                             print row
                             outfile.write(",".join(row)+"\n")
                             break
              outfile.close()
              csvfile.close()

      
#  except Exception as inst:
#        print type(inst)     # the exception instance
#        print inst.args      # arguments stored in .args
#        print inst           # __str__ allows args to be printed directly
#        x, y = inst.args
#        print 'x =', x
#        print 'y =', y
#        pass
#
  

#=====================================================================
# __name__
#=====================================================================
if __name__ == '__main__':
    main()
