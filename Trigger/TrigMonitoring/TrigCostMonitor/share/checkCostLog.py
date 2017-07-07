#!/usr/bin/env python

###########################################################################
# Check log file for successful extraction of cost ntuples from raw BS file
#
# Inputs:
#  - root_file: output ROOT file with cost ntuple
#  - log_file:  output log file from athena job which read BS file
#
# Output:
#  - True: no errors in processing and no ERROR or FATAL messages
#  - False: there are error(s) in processing
#

def getLog(name):
    import logging
    f = logging.Formatter("Py:%(name)s: %(levelname)s - %(message)s")
    h = logging.StreamHandler()
    h.setFormatter(f)
    
    log = logging.getLogger(name)
    log.setLevel(logging.INFO) 
    log.addHandler(h)
    return log

log = getLog('checkCostLog')

def getTrigCostJobStatus(root_file, log_file):
    try:
        return get_status(root_file, log_file)
    except:
        log.error("Exception caught in getTrigCostJobStatus()")
        return False

###########################################################################
# Check ROOT and log files for consitency and errors 
#
def get_status(rfile, lfile):

    countLogEvent  = -1
    countLogConfig = -1
    
    hasError = False

    # First, read log file
    import os
    if not os.path.isfile(lfile):
        log.error('Cost: LOG file does not exist')
        return False

    log.info('Cost: checking LOG file...')
    log_file = open(lfile)
    
    for l in  log_file:
        line = l.rstrip()
        
        if 'FATAL' in line:
            hasError = True
            log.error(line)
            
        if 'ERROR' in line:
            log.info('Ignore error: '+line)
            
        if line.count('POST_COST') < 1:
            continue
            
        if line.count('TrigCostRun.RunCostSave') and line.count('TrigMonEvent'):
            countLogEvent = int(line.rpartition('=')[-1])
            
        if line.count('TrigCostRun.RunCostSave') and line.count('TrigMonConfig'):
            countLogConfig = int(line.rpartition('=')[-1])
            
    log_file.close()

    # Check for errors
    if hasError:
        log.error('Cost: found fatal errors in LOG file')
        return False

    log.info('Cost: number of TrigMonEvent entries:  %d'%countLogEvent)
    log.info('Cost: number of TrigMonConfig entries: %d'%countLogConfig)

    # Check for non-negative counts
    if countLogEvent < 0:
        log.info('Cost: Failed to extract TrigMonEvent counts from LOG file')
        return False

    if countLogConfig < 0:
        log.info('Cost: Failed to extract TrigMonConfig counts from LOG file')
        return False
    
    if not os.path.isfile(rfile):
        log.error('ROOT file does not exist')
        return False
            
    import ROOT
    ROOT.gSystem.Load('libTrigMonitoringEventDict')
    
    file = ROOT.TFile(rfile)
    
    if not file.IsOpen():
        log.error('Cost: failed to open ROOT file')
        return False
        
    treeEvent  = file.Get("event")
    treeConfig = file.Get("config")
        
    countTreeEvent  = treeEvent.GetEntries()
    countTreeConfig = treeConfig.GetEntries()
    
    file.Close()

    log.info('Cost: number of ROOT event  tree entries: %d'%countTreeEvent)
    log.info('Cost: number of ROOT config tree entries: %d'%countTreeConfig)

    if countTreeEvent != countLogEvent:
        log.error('Cost: mismatch in number of event entries between ROOT and log files')
        return False

    if countTreeConfig != countLogConfig:
        log.error('Cost: mismatch in number of config entries between ROOT and log files')
        return False

    log.info('Cost: return OK')
    return True

###########################################################################
# Main function for command line execuation
#
if __name__ == "__main__":

    from optparse import OptionParser    
    p = OptionParser(usage="usage: -r <ROOT file> -l <log file>", version="1.0")

    p.add_option( "-r",
                  type    = "string",
                  dest    = "root_file",
                  default = "",
                  help    = "path to ROOT file" )

    p.add_option( "-l",
                  type    = "string",
                  dest    = "log_file",
                  default = "",
                  help    = "path to log file" )

    (options, args) = p.parse_args()  

    if options.root_file == None or len(options.root_file) < 2:
        p.error("Missing input ROOT file")
                
    if options.log_file == None or len(options.log_file) < 2:
        p.error("Missing input LOG file")              

    getTrigCostJobStatus(options.root_file, options.log_file)
