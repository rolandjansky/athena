#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,re,time,sys,string
import cx_Oracle
import xmlrpclib
import ConfigParser 
config = ConfigParser.ConfigParser()
config.read('config.ini')
datapath = config.get('Oracle', 'DataPath')
dqmpass = config.get('dqm', 'pass')

def GetNumberOfCosmicCaloEvents(RUN):
    run=str(RUN)
    connection = cx_Oracle.connect(datapath)
    query="SELECT SUM(NREVENTS) FROM SFO_TZ_File WHERE RUNNR=:arg_1 and STREAMTYPE=:arg_2 and STREAM=:arg_3 "
    cursor     = connection.cursor()
    cursor.execute(query, arg_1=run, arg_2="physics", arg_3="CosmicCalo" )
    lblist = cursor.fetchall()
    events=0
    if len(lblist)>0 and lblist[0][0] != None:
        events = lblist[0][0]
    cursor.close()
    connection.close()
    return events


def GetNumberOfProcessedEvents(server,RUN,stream,PROCESSING):
    run=str(RUN)
    processing=str(PROCESSING)

    run_spec = {'stream': stream, 'source': 'tier0', 'low_run': run, 'high_run' : run, 'proc_ver' : processing, }
    dic = server.get_hist_nentries(run_spec, 'LAr/LAR_GLOBAL/Run_Parameters/NbOfEventsVsLB')
    proc_events = 0
    if run in dic: 
        proc_events = int(dic[run])
    else:
        tmp_stream="tmp_"+stream
        run_spec = {'stream': tmp_stream, 'source': 'tier0', 'low_run': str(run), 'high_run' : run, 'proc_ver' : processing, }
        dic = server.get_hist_nentries(run_spec, 'LAr/LAR_GLOBAL/Run_Parameters/NbOfEventsVsLB')
        if run in dic: 
            proc_events = int(dic[run])

    return proc_events

# check for defects database name for get_defects_lb
#https://twiki.cern.ch/twiki/bin/view/Atlas/DQWebServiceAPIs#get_defects_lb
def GetDefectsDBName(firstrun,lastrun=-1):
    minrun1=int(config.get('run1', 'minrun'))
    maxrun1=int(config.get('run1', 'maxrun'))
    if int(firstrun) >= minrun1 and \
       ((int(lastrun)>0 and int(lastrun) <= maxrun1) or \
        (int(lastrun)<=0 and int(firstrun) <= maxrun1)) :
        defectsDBname=config.get('run1', 'defectsDB') # Run 1
    else:
        defectsDBname=config.get('run', 'defdefectsDB') # Production
    return defectsDBname

# expected that get_run_information have some info about RUN
# GetListOfRuns should choose good run list befor GetRunInfoFromDQM call
def GetRunInfoFromDQM(RUN):
    run=str(RUN)
    data={};

    s = xmlrpclib.ServerProxy('https://'+dqmpass+'@atlasdqm.cern.ch')
    run_spec = {'stream': 'physics_IDCosmic', 'source': 'tier0', 'low_run': int(run), 'high_run': int(run)}

    # runinfo structure:
    # * Run type (string)
    # * Project tag (string)
    # * Partition name (string)
    # * Number of events passing Event Filter (integer)
    # * Run start (seconds after 1970) (integer)
    # * Run end (seconds after 1970, 0 if run ongoing) (integer)
    # * Number of luminosity blocks (-1 if run ongoing) (string)
    # * Data source (string) - this is only kept for backward
    # compatibility and should be IGNORED
    # * Detector mask (string)
    # * Recording enabled (integer)
    # * Number of events in physics streams (integer) 
    runinfo = s.get_run_information(run_spec)
    data["NLB"] = str(runinfo[run][6])
    #Run start
    start_time=int(runinfo[run][4])
    if start_time==0: 
        data["START"] = "No info"
    else:
        data["START"] = time.asctime(time.localtime(start_time))
    #Run stop
    stop_time=int(runinfo[run][5])
    if stop_time==0: 
        data["RUNSTOP"] = "Run ongoing"
    else:
        data["RUNSTOP"] = time.asctime(time.localtime(stop_time))

    run_duration_insec=stop_time-start_time

        
    # endoflooptime structure:
    # * the scheduled end of the Tier-0 calibration period in seconds
    # past January 1 1970,
    # * is whether the loop has actually ended or not (1=ended, 0=still open) 
    endoflooptime=s.get_end_of_calibration_period(run_spec) 
    if run in endoflooptime.keys():
        efl_time=endoflooptime[run][0]
        if int(efl_time)==0: 
            data["ENDCALIBLOOP"] = "No info"
        else:
            data["ENDCALIBLOOP"] = time.asctime(time.localtime(efl_time))
    else:
        data["ENDCALIBLOOP"] = "No info"

    #Lumi
    # beam lum info structure:
    # 0 Max beam energy during run (float)
    # 1 Stable beam flag enabled during run (boolean)
    # 2 ATLAS ready flag enabled during run (boolean)
    # 3 Total integrated luminosity (/nb) - online best estimate (float)
    # 4 ATLAS ready luminosity (/nb) - online best estimate (float) 
    beamluminfo=s.get_run_beamluminfo(run_spec)
    if run in beamluminfo.keys():
        lumint=beamluminfo[run][3]        
        lumavg=(float(lumint)/run_duration_insec)*1.E27
        data["LUMAVG"]="%.1e cm-2s-1" % (lumavg)
        lumATLAS=float(beamluminfo[run][4])/1000.
        data["LUMI"]="%.1f pb-1" % (lumATLAS)
        if beamluminfo[run][1]:
            data["STABLEBEAM"]="True"
        else:
            data["STABLEBEAM"]="False"
    else:
        data["LUMAVG"]="No info"
        data["LUMI"]="No Info"
        data["STABLEBEAM"]="No Info"

    #Defects
    list_of_defects = ["LAR_UNCHECKED","LAR_BULK_UNCHECKED","LAR_UNCHECKED_FINAL","LAR_DATACORRUPT","LAR_EMBA_COVERAGE","LAR_EMBC_COVERAGE","LAR_HECA_COVERAGE","LAR_HECC_COVERAGE","LAR_EMECA_HVNONNOMINAL","LAR_EMECC_HVNONNOMINAL","LAR_HECA_HVNONNOMINAL","LAR_HECC_HVNONNOMINAL","LAR_EMECA_HVNONNOM_CORRECTED","LAR_EMECC_HVNONNOM_CORRECTED","LAR_HECA_HVNONNOM_CORRECTED","LAR_HECA_HVNONNOM_CORRECTED","LAR_EMECA_HVTRIP","LAR_EMECC_HVTRIP","LAR_HECA_HVTRIP","LAR_HECC_HVTRIP","LAR_EMECA_NOISEBURST","LAR_EMECC_NOISEBURST","LAR_HECA_NOISEBURST","LAR_HECC_NOISEBURST","LAR_EMECA_SEVNOISEBURST","LAR_EMECC_SEVNOISEBURST","LAR_HECA_SEVNOISEBURST","LAR_HECC_SEVNOISEBURST","LAR_EMECA_HVRAMPUP","LAR_EMECC_HVRAMPUP","LAR_HECA_HVRAMPUP","LAR_HECC_HVRAMPUP","LAR_EMECA_NOISYCHANNEL","LAR_EMECC_NOISYCHANNEL","LAR_HECA_NOISYCHANNEL","LAR_HECC_NOISYCHANNEL","LAR_BADCELLS_ONHOLD","LAR_FIRSTDEFECTS_UNCHECKED"]
        
    defectsDBname=GetDefectsDBName(run)
    defectsmap = s.get_defects_lb(run_spec,list_of_defects, 'HEAD', False, False, defectsDBname)

    data["DEFECTS"]="NOTDONE"
    data["ES"]="NOTDONE"
    data["BULK"]="NOTDONE"
    data["FINAL"]="NOTDONE"
    data["HVNONNOMINAL"]="ABSENT"
    if run in defectsmap.keys():
        if "LAR_FIRSTDEFECTS_UNCHECKED" not in defectsmap[run]: 
            data["DEFECTS"]="DONE"
            if 'LAR_UNCHECKED' not in defectsmap[run]: 
                data["ES"]="DONE"
            if 'LAR_BULK_UNCHECKED' not in defectsmap[run]: 
                data["BULK"]="DONE"
            if 'LAR_EMECA_HVNONNOMINAL' in defectsmap[run] or \
               'LAR_EMECC_HVNONNOMINAL' in defectsmap[run] or \
               'LAR_HECA_HVNONNOMINAL' in defectsmap[run] or \
               'LAR_HECC_HVNONNOMINAL' in defectsmap[run]: 
                data["HVNONNOMINAL"]="PRESENT"
        if 'LAR_UNCHECKED_FINAL' not in defectsmap[run]: 
            data["FINAL"]="DONE"

    #GetNumberOfProcessedCosmicCaloEvent2 
    data["ESPROC"]=str(GetNumberOfProcessedEvents(s,run,"physics_CosmicCalo",1))
    data["BULKPROC"]=str(GetNumberOfProcessedEvents(s,run,"physics_CosmicCalo",2))
    #GetNumberOfProcessedExpressStreamEvents3
    data["ESPROCEXPRESS"]=str(GetNumberOfProcessedEvents(s,run,"express_express",1))
    data["BULKPROCEXPRESS"]=str(GetNumberOfProcessedEvents(s,run,"express_express",2))
    #GetNumberOfProcessedJetTauEtmissEvents2
    data["ESPROCJETTAU"]=str(GetNumberOfProcessedEvents(s,run,"physics_JetTauEtmiss",1))
    data["BULKPROCJETTAU"]=str(GetNumberOfProcessedEvents(s,run,"physics_JetTauEtmiss",2))

    #dummy
    data["TEMPEXPRESS"]=""

    return data

def GetRunInfoFromOracle(RUN):
    run=str(RUN)
    data={};

    connection = cx_Oracle.connect(datapath)
#    query="SELECT SUM(NREVENTS),LUMIBLOCKNR FROM SFO_TZ_File WHERE RUNNR=:arg_1 and STREAMTYPE=:arg_2 and STREAM=:arg_3 GROUP BY LUMIBLOCKNR ORDER BY LUMIBLOCKNR"

    query="SELECT SUM(NREVENTS) FROM SFO_TZ_File WHERE RUNNR=:arg_1 and STREAMTYPE=:arg_2 and STREAM=:arg_3 "

    cursor     = connection.cursor()
    cursor.execute(query, arg_1=run, arg_2="physics", arg_3="CosmicCalo" )
    lblist = cursor.fetchall()
    events=0
    if len(lblist)>0 and lblist[0][0] != None:
        events = lblist[0][0]
    data["CosmicCaloTot"]=str(events)
    cursor.close()

    cursor     = connection.cursor()
    cursor.execute(query, arg_1=run, arg_2="express", arg_3="express")
    lblist = cursor.fetchall()
    events=0
    if len(lblist)>0 and lblist[0][0] != None:
        events = lblist[0][0]
    data["expressTot"]=str(events)
    cursor.close()

    cursor     = connection.cursor()
    cursor.execute(query, arg_1=run,  arg_2="physics", arg_3="JetTauEtmiss" )
    lblist = cursor.fetchall()
    events=0
    if len(lblist)>0 and lblist[0][0] != None:
        events = lblist[0][0]
    data["JetTauEtmissTot"]=str(events)
    cursor.close()

    connection.close()

    return data

# just dummy
def GetCAFInfo(RUN):
    run=str(RUN)
    data={};
    data["LCE"]="NOT LAUNCHED YET"
    data["ESCLEANING"]="NOT LAUNCHED YET"
    data["BULKCLEANING"]="NOT LAUNCHED YET"
    data["LIST"]="NO"    
    return data

if __name__ == "__main__":

    # for test purpose
    run=248115
#    run=215433
    run=215091
    data= GetRunInfoFromDQM(run)
    data.update(GetRunInfoFromOracle(run))
    data.update(GetCAFInfo(run))
    print data
