#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##===================================================================================================
## Name:        inputFilePeeker.py
## Author:      Michael Boehler (DESY) and David Cote (DESY)
## Created:     April 2008
## Description: the inputFilePeeker provides a dictionary with all infos from the meta data
## DR : should be a cache mechanism so it is executed only once
##===================================================================================================

__all__ = [
    "inputFileSummary",
    ]
### globals ----------------------------------------------------------------
inputFileSummary = {}

def _setup():

    global inputFileSummary
    import os
    from RecExConfig.RecFlags import rec
    import AthenaCommon.Logging as L
    from AthenaCommon.Resilience import treatException

    #define a logger
    msg = L.logging.getLogger('inputFilePeeker' )
    msg.info("Executing   inputFilePeeker.py")

    # special setup for online reconstruction so far
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline():
        # set minimal items of inputFileSummary
        inputFileSummary={'file_type':'bs',
                          'evt_type':['IS_DATA','IS_ATLAS','IS_PHYSICS'],
                          'TagStreamsRef':''}
        return
    
    #get input file name
    from RecExConfig.RecoFunctions import InputFileNames
    inFiles=InputFileNames()
    if len(inFiles) < 1:
        msg.error("No input files specified yet! Cannot do anything.")

    #create and fill inputFileSummary (DC: looping through input files if necessary)
    import PyUtils.AthFile as athFile
    failed_trials = 0
    for inFile in inFiles:
        try:
            fi = athFile.fopen(inFile)
            inputFileSummary = fi.fileinfos
        except Exception,err:
            msg.warning("Unable to open file [%s]"%inFile)
            msg.warning('caught:\n%s',err)
            import traceback
            traceback.print_exc()
            continue

        ## Making sure that stream_names is always defined
        if not inputFileSummary.has_key('stream_names'):
            msg.warning("AthFile didn't find key 'stream_names'. Recovering it but that's unexpected.")
            inputFileSummary['stream_names']=[]
        
        #First try to catch the no entries case
        if inputFileSummary['stream_names'] == []:
            try:
                #print fi.infos['metadata_items'][0][1]
                inputFileSummary['stream_names'] = [fi.infos['metadata_items'][0][1]]
            except Exception, err:
                msg.info("Unable to find stream names in file metadata.")

        #If stream_names still not found, check for bytestream case or give default value
        if inputFileSummary['stream_names']==None or inputFileSummary['stream_names']==[]:
            if inputFileSummary['file_type']=='bs':
                msg.info("stream_names not present in input bytestream file. Giving default name 'StreamRAW'")
                inputFileSummary['stream_names']=['StreamRAW']
            else:
                inputFileSummary['stream_names']=['Unknw']
                msg.warning("Unable to find stream_name from input file. This HAS an effect on auto-configuration!")

        #At this point streamsName is always setup

        #DR TAG do not have run number        
        if len(inputFileSummary['run_number']) >0 or 'TAG' in inputFileSummary['stream_names'] :
            msg.info("Successfully filled inputFileSummary from file %s"%inFile)
            break
        else:
            msg.warning("Unable to fill inputFileSummary from file %s. File is probably empty. Will try again with next (if any)."%inFile)

        ## everything failed...
            failed_trials += 1
        ## now, we failed too many times.
        ## trigger an athfile cache-flush to not waste too much memory
        ## with file summaries which are irrelevant.
        ## FIXME: should the trigger be jobo-settable ?
        if failed_trials > 10:
            msg.warning("Unable to fill inputFileSummary [%d] times. flushing athfile cache..." % failed_trials)
            athFile.flush_cache()
        pass

    #consistency check
    if len(inputFileSummary.items()) < 1:
        msg.error("Unable to build inputFileSummary from any of the specified input files. There is probably a problem.")
        return

    #Exception: if input is TAG, you need to follow the link to fill inputFileSummary
    msg.info("Extracted streams %s from input file " % inputFileSummary['stream_names'] )   


    inputFileSummary['TagStreamsRef']=None

    if 'TAG' in inputFileSummary['stream_names']:

        import PyUtils.PoolFile as pf
        tagStreamsRef=pf.extract_streams_from_tag(inFile,nentries=1)

        from RecExConfig.AutoConfiguration import GetDefaultTagRefStream
        streamTarget=GetDefaultTagRefStream(tagStreamsRef)
        msg.info ( "will redirect to target %s " % streamTarget )

        # now get the file on which the TAG is pointing
        from PyUtils.PoolFile import PoolFileCatalog as pfc
        newInFile=None
        catalog_name = [pfc.DefaultCatalog]
        # ensure consistency of (read) catalogs' content b/w
        # python PoolFileCatalog and C++ PoolSvc.ReadCatalog
        try:
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            if (hasattr(svcMgr, 'PoolSvc') and
                hasattr(svcMgr.PoolSvc, 'ReadCatalog')):
                # add PoolSvc.ReadCatalog, not overwrite, otherwise
                # default PoolFileCatalog would be removed
                catalog_name += list(svcMgr.PoolSvc.ReadCatalog[:])
                pass
        except Exception, err:
            msg.info(
                'problem getting ReadCatalog value from svcMgr.PoolSvc:\n%s',
                err)
            pass
        try:
            #get guid of file to be navigated to, then get corresponding physics file name
            aTagStreamsRef=tagStreamsRef[streamTarget][0]
            newInFile=pfc(catalog=catalog_name).pfn(aTagStreamsRef)
            msg.info ( "reading TAG redirected to file fid: %s pfn:%s " % (aTagStreamsRef,newInFile))
            try:
                fi = athFile.fopen(newInFile)
            except Exception:    
                msg.warning ( "AthFile.fopen failed ! Could not redirect input TAG to first target file %s. Probably not available. Now trying them all." % newInfile )
                newInFile=None
        except Exception:
            msg.warning ( "could not redirect input TAG to first target file %s. Probably not in catalog. Now trying them all." % aTagStreamsRef )
            newInFile=None

        if newInFile is None:    
            #get ALL guid of all files to be navigated to, then get corresponding physics file name of first available one
            allTagStreamsRef=pf.extract_streams_from_tag(inFile)[streamTarget]            
            for aTagStreamsRef in allTagStreamsRef:
                try:
                    newInFile=pfc(catalog=catalog_name).pfn(aTagStreamsRef)
                    fi = athFile.fopen(newInFile)
                    msg.info ( "finally redirected input TAG to file fid: %s pfn:%s " % (aTagStreamsRef,newInFile))
                    break
                except Exception:

                    newInFile=None

        if newInFile is None:
            raise RuntimeError,"unable to redirect tag to any file. Autoconfiguration fails"
        else:
            inputFileSummary = fi.fileinfos
            # store information in inputFileSummary
            inputFileSummary['TagStreamsRef']=inFile

        
    #event data

    from RecExConfig.RecoFunctions import ListOfTupleToDic
    if 'eventdata_items' not in inputFileSummary:
        inputFileSummary['eventdata_items']=[]
        pass
    fullListTuple = inputFileSummary['eventdata_items']
    inputFileSummary['eventdata_itemsDic']=ListOfTupleToDic(fullListTuple)
    fullList = []
    if fullListTuple:
        for iTuple in fullListTuple :
            item = '%s#%s' % iTuple
            fullList.append(item)
            pass
    inputFileSummary['eventdata_itemsList']=fullList
    
    
    #meta-data
    if 'metadata_items' not in inputFileSummary:
        inputFileSummary['metadata_items']=[]
        pass
    fullListTuple = inputFileSummary['metadata_items']
    inputFileSummary['metadata_itemsDic']=ListOfTupleToDic(fullListTuple)
    fullList = []
    if fullListTuple:
        for iTuple in fullListTuple :
            item = '%s#%s' % iTuple
            fullList.append(item)
            pass
    inputFileSummary['metadata_itemsList']=fullList

    #Catch common problems
    if inputFileSummary['conditions_tag']==None:
        inputFileSummary['conditions_tag']=""

    if inputFileSummary['evt_type']==[] and inputFileSummary['file_type']=='bs':
        inputFileSummary['evt_type']=('IS_DATA', 'Unknown', 'Unknown')
        msg.warning('Bytestream input: guessing that evt_type=IS_DATA, but this is not 100% certain. Using auto-configuration is not safe if this info is wrong.')

    if inputFileSummary['evt_type']==[] and inputFileSummary['nentries']==0:             
        conditionsTag=inputFileSummary['conditions_tag']
        if conditionsTag.find('SIM')>0:
            inputFileSummary['evt_type']=('IS_SIMULATION', 'IS_ATLAS', 'IS_PHYSICS')
        else:
            inputFileSummary['evt_type']=('IS_DATA', 'Unknown', 'Unknown')
            pass
        msg.warning("Input file has zero events and hence no EventInfo object. Guessed that evt_type=%s, but this is not certain. Using auto-configuration is not safe if this info is wrong."%(inputFileSummary['evt_type'][0]))
        pass

    #Final print out (DEBUG)
    msg.debug("inputFileSummary is:")
    msg.debug(str(inputFileSummary))
    return

# execute function at module import
_setup()            

# clean-up
del _setup
