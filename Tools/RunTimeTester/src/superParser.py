# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from exc2string2            import exc2string2
from Factory_JobsXMLReader  import JobsXMLReaderFactory
from JobMaker               import JobMaker

from RTTSException          import RTTCodingError

# -------------------------------------------------------------------------

def superParser(paths, descFactory, minderFactory, logger):
    """Returns a list of all minders (RTT, RTTpilot and ATN)"""
    

    jobListElement  = paths.packageXMLfile['rtt/jobList[0]']
    try:
        atnElement  = paths.packageXMLfile['atn[0]']
    except IndexError:
        atnElement = None
            
    factory  = JobsXMLReaderFactory(logger)

    def makeMinders((mode, element), paths):
        xmlReader = factory.create(mode, element, paths, descFactory)

        try:
            jobMinders = JobMaker(xmlReader, paths,  descFactory, minderFactory, logger).jobMinders
            return jobMinders
        except:
            msg  = 'Exception while parsing package xml file '
            msg += 'mode: %s package %s \n' % (mode, paths.package)
            msg += 'Traceback:\n%s' % exc2string2() 
            raise RTTCodingError(msg)

        return []

    # Always prcocess RTT jobs
    modes = {paths.mode: jobListElement} # to make RTT and RTTpilot jobs

    # process ATN if requested
    if paths.atnPilotJobs:
        if atnElement: modes['ATN'] = atnElement 
        
    jobMinders = []
    [jobMinders.extend(makeMinders(m, paths)) for m in modes.items()]
    
    # remove pilot jobs if they are NOT requested
    if not paths.rttPilotJobs:
        jobMinders = [j for j in jobMinders if not j.rttPilotJob]

    return jobMinders

