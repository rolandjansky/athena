#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
 Generate run list for each period using the pyAMI interface to the COMA db.

 Need to authenticate (once) and setup pyami via:

source /afs/cern.ch/atlas/software/tools/atlasmeta/setup.sh
export PYAMI_CONFIG_DIR=~/private/.pyami
ami auth
"""
 
__author__  = 'Carl Gwilliam'
__version__ = ''
__usage__   = '''%prog [options]'''
 
import os,sys
 # the above line needed to ensure correct python environment since pyAMI 4.0.3
import pyAMI.client

from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-c', '--config', dest='config', default='', help='AMI authentication file')
(options,args) = parser.parse_args()
 
class AMIWrapper:
     def __init__(self):
         "Initalise AMI"
         self.ami=pyAMI.client.Client('atlas')
         #if options.config:
         #    self.configFileName = os.path.expanduser(options.config)
         #else:
             #sys.exit('No authentication file specified')
         #self.ami.readConfig(self.configFileName)
         if options.config:
             self.configFileName = os.path.expanduser(options.config)
         else:
             sys.exit('No authentication file specified')
         print self.configFileName
         #self.ami=pyAMI.client.Client('atlas',key_file=self.configFileName,ignore_proxy=True,verbose=True)
 
     def run(self, cmd):
         """
         Execute an AMI command given as a list of command and paramters (ami format) or
         space separated string (for convenience)
         """
         if isinstance(cmd, basestring):
             cmd = cmd.split()
         
         results = self.ami.execute(cmd,format='dict_object')

         return results.get_rows()
             
     def periods(self, period=None, level=None, project=None, status=None):
         "Get list of periods.  By default return all periods"
         
         cmd = 'ListDataPeriods'
         if period:  cmd += ' period='+period
         if level:   cmd += ' periodLevel='+level
         if project: cmd += ' projectName='+project
         if status:  cmd += ' status='+status        
         return self.run(cmd)
 
     def runsForPeriod(self, period, project):
         "Get info on runs for a particular period"
 
         cmd = 'GetRunsForDataPeriod period='+period
         cmd += ' projectName='+project
         print cmd
         return self.run(cmd)
 
     def runListsPerPeriod(self, location='/afs/cern.ch/user/a/atlidbs/nt/DataPeriods/'):
         """
         Generate files containing run lists for periods if they don't already exist
         """
         # Don't only update unlocked periods since they may change as looked so need to check at least once more
         # Need to find a better way fo doing this but for now update all
         periods = self.periods() #status='unlocked') 
         path = os.path
         num = 0

         for p in periods:
             projectDir =path.normpath(location + '/' + p['projectName'])
 
             if not path.exists(projectDir):
                 os.makedirs(projectDir)
             
             filename = '%s/%s.runs.list' % (projectDir,p['period'])
             
             if path.exists(filename): continue
 
             # Need to remove the file as it might have changed from last time
             #if path.exists(filename):
             #    os.system('rm ' + filename)
 
             print '* Creating run list for %(projectName)s %(period)s ...' % p
 
             num += 1
             #if p['period'] == 'A1' and p['projectName'] == 'data15_1beam':
             #  continue
             try:
               runs = self.runsForPeriod(p['period'], p['projectName'])
             except:
               continue
             runList = '\n'.join(sorted([r['runNumber'] for r in runs]))
             runList += '\n' # Needed for cat to work!
             print runList
             with open(filename, 'w') as f:
                 f.write(runList)
 
             print '  ... written to', filename, '\n' 
 
         if not num:
             print '* No new period infomation to create'
  
if __name__ == '__main__':
     ami = AMIWrapper()
     ami.runListsPerPeriod()    


