# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:   TestUtils.py
# @author: Frank Winklmeier
# @brief:  Python module for nightly tests 

class JobRecord:
   """Information about jobs"""
   
   summaryFile = "jobSummary.pickle"   # Pickle to this file
   
   def __init__(self):
      self.alias = ""      # alias name
      self.status = 0      # exit code
      self.walltime = 0    # in seconds
      self.logFile = None  # log file
      self.files = []      # files created by this job
      return

      
def printJobSummary(jobList, log):
   """
   Print the summary for a list of JobRecords.
   'log' is a logging logger
   Make sure you configured the logging module before.
   """

   log.info( "|"+"="*70 )
   log.info( "| Job summary:" )
   log.info( "|"+"="*70 )
   log.info( "| %-20s %-8s %-10s %-10s" % ("Job alias","Status","Time(M:S)","Log file") )
   log.info( "|"+"-"*70 )
   for j in jobList:
      strtime = "%d:%.2d" % (j.walltime/60,j.walltime%60)
      log.info( "| %-20s %-8d %-10s %-10s" % (j.alias,j.status,strtime,j.logFile) )

   log.info( "|"+"="*70 )
   return
