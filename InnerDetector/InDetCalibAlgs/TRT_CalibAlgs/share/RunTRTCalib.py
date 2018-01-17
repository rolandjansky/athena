import os,sys,time,glob,fnmatch,random,time
from time import gmtime, strftime, mktime, time
from Tkinter import *
from CosmicTemplate import cosmic
from CollisionTemplate import collision


###################################################
################ JOB OPTIONS ######################
###################################################

def create_environment_variables(config,inputfile):
    ostring="""
python %s/dumpVersion.py -f %s | tee versiontags.txt;
export DETECTOR_DESCRIPTION=`grep GeoAtlas versiontags.txt | awk '{print $2}' `
echo $DETECTOR_DESCRIPTION
""" % (config["Calibdir"],inputfile)
    return ostring


##########################################################
################ BATCH SCRIPT GENERATORS #################
##########################################################

# creates a script that runs in $WORKDIR

def setup_batchscript_parallel(config,inputfiles,calibconstants,outputfile,iscalib,calfile,caltag,oiter,calsettings2,ijob):
    ostring="cd $WORKDIR\n"    
    ostring+="export STAGE_SVCCLASS=atlcal\n"
    ostring+="uname -a\n"

    cpcmd="cp -fv "
    if outputfile.find("/eos/")>=0:
        cpcmd="xrdcp -f "

    if iscalib:
        ostring+=config["SetupCmdCal"]+"\n"
    else:
        ostring+=config["SetupCmdRec"]+"\n"
        
    if config["NoRec"] and not iscalib:
        if float(config["DoShift"])>0:
            mint0=-float(config["DoShift"])
            maxt0=float(config["DoShift"])
            stept0=(maxt0-mint0)/int(config["MaxParallel"])
            t0shift=mint0+ijob*stept0
            ostring+=cpcmd + " root://eosatlas.cern.ch/"+outputfile+"_basic_shifted_%.1f.root.bz2 basic.root\n"%t0shift
            ostring+="root -b -q  basic.root '%s/graph_points.C(%f,%d)'\n"%(config["Calibdir"],t0shift,oiter+1)
            ostring+="cp -fv gp_* " + config["Batchdir"] + "/temp/.\n"
            if t0shift==0:
                ostring+="cp -fv shiftres_* " + config["Batchdir"] + "/temp/.\n"
        else:
            ostring+=cpcmd + "root://eosatlas.cern.ch/" + outputfile+"_basic.root.bz2 basic.root.bz2\n"
            ostring+="bunzip2 -v basic.root.bz2\n"
            ostring+="ln -sf " + config["Calibdir"] + "/bhadd.cpp\n"
            ostring+="ln -sf " + config["Calibdir"] + "/Makefile\n"
            ostring+="make bhadd\n"
            ostring+="./bhadd dumfile basic.root\n"
            ostring+=cpcmd + "tracktuple.root root://eosatlas.cern.ch/" + outputfile + "_tracktuple.root\n"
        return ostring

    if iscalib:
        prevcalfile=calfile.strip("%02i"%(oiter+1)) + "%02i"%oiter 
        if caltag == "error_Barrel":
                ostring         +=      cpcmd + "root://eosatlas.cern.ch/" + calfile + "_tracktuple.root tracktuple.root\n"
                ostring         +=      "ln -sf " + config["Calibdir"] + "/ErrorsOptimization.cpp\n"
                ostring         +=      "ln -sf " + config["Calibdir"] + "/Makefile\n"
                ostring         +=      "make ErrorsOptimization\n"

        elif caltag == "error_Endcap":
                ostring         +=      cpcmd + "root://eosatlas.cern.ch/" + calfile + "_tracktuple.root tracktuple.root\n"
                ostring         +=      "ln -sf " + config["Calibdir"] + "/ErrorsOptimization.cpp\n\n"
                ostring         +=      "ln -sf " + config["Calibdir"] + "/Makefile\n"
                ostring         +=      "make ErrorsOptimization\n"
        else:
                ostring         +=      cpcmd + "root://eosatlas.cern.ch/" + calfile + "_basic.root.bz2 merged.root.bz2\n"
                ostring         +=      cpcmd + "root://eosatlas.cern.ch/" + prevcalfile + "_oldt0s.txt calib_constants_in.txt\n"
                ostring         +=      "bunzip2 -v *.bz2\n"

    ifiles=[]
    #copy all inputfiles to local area
    if iscalib:
        ostring+="cp -v " + config["Batchdir"] + "/input/calibout_%02d.txt calibconstants.txt\n"%oiter
        ostring+="cp -v " + config["Calibdir"] + "/finedelays.txt .\n"
    for inputfile in inputfiles:
        if inputfile.find("castor:")>=0:
            ostring+="rfcp "+inputfile.split(":")[1]+" .\n"
        else:
            ostring+="cp "+inputfile+" .\n"

            #strip path from inputfile
        ifile=os.path.basename(inputfile)
        ifiles.append(ifile)

    t0shift=0
    if not iscalib:
        ostring+="ln -sf " + config["Calibdir"] + "/bhadd.cpp\n"
        ostring+="ln -sf " + config["Calibdir"] + "/Makefile\n"
        ostring+="make bhadd\n"
        if float(config["DoShift"])>0:
            mint0=-float(config["DoShift"])
            maxt0=float(config["DoShift"])
            stept0=(maxt0-mint0)/int(config["MaxParallel"])
            t0shift=mint0+ijob*stept0
            ishiftfiles=9+int(random.random()*10)
            print "SHIFTING T0s %f ns, USING FILES FROM JOB %d"%(t0shift,ishiftfiles)
            ostring+="#SHIFTING T0s %f ns\n"%t0shift
            if oiter>=0:
                ostring+="python " + config["Calibdir"] + "/cfilter.py t0shift %f "%t0shift + calibconstants + "\n"
            else:
                ostring+="python " + config["Calibdir"] + "/cfilter.py t0shift %f "%t0shift + config['Batchdir'] + '/input/calibout_start.txt'  + "\n"
            calibconstants = "dbconst.txt"
            #random.shuffle(config["shiftfiles"])
            inputfiles=config["shiftfiles"]
            #inputfiles=config["shiftfiles"][ishiftfiles]
            #print "###### %d"%len(config["shiftfiles"])
                                                                                                                        
    ostring+="ls -al\n"

    # set environment variables (detector description)
    if config["DataType"]=="ESD":
        "dum"

    #create jobOptions
    sys.path.append (config["Calibdir"])
    if iscalib:
        if not ( (caltag == "error_Barrel")  | (caltag == "error_Endcap")):
	        if ( (not config["IsCosmicMC"]) & (not config["IsCosmic"]) & (not config["IsCollision"]) & (not config["IsHeavyIons"]) ):
	            from ESDCalibTemplate import ESDCalib
	            jO=ESDCalib(config,ifiles,calibconstants,caltag.split("_",1)[1])
	        elif config["IsCosmicMC"]:
	            from CosmicMCCalibTemplate import cosmiccalib
	            jO=cosmiccalib(config,ifiles,calibconstants,caltag.split("_",1)[1])            
	        elif config["IsCollision"]:
        		if config["DataType"]=="ESD":
	            		#from CollisionCalibTemplate import collisioncalib
	            		#jO=collisioncalib(config,ifiles,calibconstants,caltag.split("_",1)[1],calsettings2)            
	            		from RAWCalibTemplate import collisioncalib
	            		jO=collisioncalib(config,ifiles,calibconstants,caltag.split("_",1)[1],calsettings2)            
        		if config["DataType"]=="RAW":
	            		from RAWCalibTemplate import collisioncalib
	            		jO=collisioncalib(config,ifiles,calibconstants,caltag.split("_",1)[1],calsettings2)            
	        elif config["IsHeavyIons"]:
        		if config["DataType"]=="ESD":
            			from CollisionCalibTemplate import collisioncalib
            			jO=collisioncalib(config,ifiles,calibconstants,caltag.split("_",1)[1],calsettings2)            
        		if config["DataType"]=="RAW":
	            		from RAWCalibTemplate import collisioncalib
	            		jO=collisioncalib(config,ifiles,calibconstants,caltag.split("_",1)[1],calsettings2)            
	        else:
	            from CosmicCalibTemplate import cosmiccalib
	            jO=cosmiccalib(   config,ifiles,calibconstants,caltag.split("_",1)[1],calsettings2)            
        else:
                ostring         +=      "./ErrorsOptimization tracktuple.root " + caltag + "\n"

    elif config["IsCosmic"]:
        jO=cosmic(config,ifiles,calibconstants,config["NumEvents"][oiter+1])
    elif config["IsCosmicMC"]:
        print "COSMIC MC!!!!"
        from CosmicMCTemplate import cosmicMC
        jO=cosmicMC(config,inputfiles,calibconstants)
    elif config["IsSingleBeam"]:
        from SingleBeamTemplate import singlebeam
        jO=singlebeam(config,inputfiles,calibconstants)
    elif config["IsCollision"]:
        if config["DataType"]=="ESD":
            from CollisionTemplate import collision
            jO=collision(config,ifiles,calibconstants,config["NumEvents"][oiter+1])
        if config["DataType"]=="RAW":
#            from IDTracksTemplate import idtracks
#            jO=idtracks(config,inputfiles,calibconstants,config["NumEvents"][oiter+1])
            from RAWTemplate import collision
            jO=collision(config,ifiles,calibconstants,config["NumEvents"][oiter+1])
    elif config["IsHeavyIons"]:
        if config["DataType"]=="ESD":
	        from HITemplate import collision
	        jO=collision(config,ifiles,calibconstants,config["NumEvents"][oiter+1])
        if config["DataType"]=="RAW":
	        from RAWHITemplate import collision
	        jO=collision(config,ifiles,calibconstants,config["NumEvents"][oiter+1])

    else:
        if config["DataType"]=="ESD":
            from ESDTemplate import ESD
            jO=ESD(inputfiles,calibconstants)
        if config["DataType"]=="RAW":
            from RAWTemplate import RAW
            jO=RAW(inputfiles,calibconstants,config)
            
    #add them to the script
    if not ( (caltag == "error_Barrel")  | (caltag == "error_Endcap")):
            ostring+="cat > joboptions.py  <<EOF\n"
            ostring+=jO+"\nEOF\n"
    
    #run athena
    if iscalib:
        if not ( (caltag == "error_Barrel")  | (caltag == "error_Endcap")):
                #ostring+="athena joboptions.py | tee log\n"
              #  ostring+="athena joboptions.py 2>&1\n"
               if config["DataType"]=="RAW":
                        ostring+="Reco_trf.py  inputBSFile=%s conditionsTag=%s --ignoreerrors=ALL  autoConfiguration=everything  maxEvents=%s preExec='rec.doEgamma=False;jetFlags.Enabled=False;InDetFlags.doPerfMon=False;InDetFlags.doMonitoringAlignment=False;InDetFlags.doMonitoringGlobal=False;InDetFlags.doMonitoringSCT=False;InDetFlags.doMonitoringPixel=False;InDetFlags.useBeamConstraint.set_Value_and_Lock(False);' geometryVersion=%s outputESDFile='TRT.ESD.root' postInclude='joboptions.py' outputESDFile=trt.ESD.root \n" %(ifiles[0], config['GLtag'], "1" , config["DetDescVer"]  )
               else:
                         ostring+="athena joboptions.py 2>&1\n"

    else:
        if config["SingleFile"]:
            #ostring+="athena joboptions.py >log 2>&1\n"
            #ostring+="athena joboptions.py | tee log\n"
            ostring+="athena joboptions.py 2>&1\n"
        else:
            #ostring+="athena joboptions.py | tee log | grep '00 events processed so far' \n"
            #ostring+="athena joboptions.py 2>&1\n"
            if config["DataType"]=="RAW":

                filetest=""
                count = 0
                for ii in ifiles:
                        filetest+=ii
                        count+=1		
                        if count < len(ifiles):
                        filetest+=","

                ostring+="Reco_trf.py  inputBSFile=%s conditionsTag=%s --ignoreerrors=ALL  autoConfiguration=everything  maxEvents=%s preExec='rec.doEgamma=False;jetFlags.Enabled=False;InDetFlags.doPerfMon=False;InDetFlags.doMonitoringAlignment=False;InDetFlags.doMonitoringGlobal=False;InDetFlags.doMonitoringSCT=False;InDetFlags.doMonitoringPixel=False;InDetFlags.useBeamConstraint.set_Value_and_Lock(False);' geometryVersion=%s outputESDFile='TRT.ESD.root' postInclude='joboptions.py' outputESDFile=trt.ESD.root \n" %(filetest, config['GLtag'], config["NumEvents"][oiter+1] , config["DetDescVer"]  )
            #	jO=collision(config,ifiles,calibconstants,config["NumEvents"][oiter+1])


            else:
            	ostring+="athena joboptions.py 2>&1\n"


        #ostring+="grep 'Number of events' log\n"

    #copy the output
    if config["GetConstants"]:
        ostring+="cp caliboutput.txt " + config['Batchdir'] + '/input/calibout_start.txt'  + "\n" 
    else:
        if iscalib:
	        if not ( (caltag == "error_Barrel")  | (caltag == "error_Endcap")):
	            ostring+=cpcmd + " calibout.root root://eosatlas.cern.ch/"+outputfile+".root\n"
	            ostring+=cpcmd + " calibout_rt.txt root://eosatlas.cern.ch/"+outputfile+"_rt.txt\n"
	            ostring+=cpcmd + " calibout_binrt.txt root://eosatlas.cern.ch/"+outputfile+"_binrt.txt\n"
	            ostring+=cpcmd + " calibout_t0.txt root://eosatlas.cern.ch/"+outputfile+"_t0.txt\n"
	            ostring+=cpcmd + " calibout_dict.txt root://eosatlas.cern.ch/"+outputfile+"_dict.txt\n"
	            ostring+=cpcmd + " calib_constants_out.txt root://eosatlas.cern.ch/"+outputfile+"_const.txt\n"
	            #ostring+=cpcmd + " log root://eosatlas.cern.ch/"+outputfile+"_log.txt\n"
	            if oiter==-1: ostring+= "cp caliboutput.txt "+config["Batchdir"]+"/input/calibout_start.txt\n"
	            #ostring+="cp ntuple.pmon.gz "+outputfile+"_pmon.gz\n"
	        else:
	                ostring+=cpcmd + " errorsout.root       root://eosatlas.cern.ch/"+outputfile+".root\n"
	                ostring+=cpcmd + " errors_rt.txt        root://eosatlas.cern.ch/"+outputfile+"_rt.txt\n"
	                ostring+=cpcmd + " errors_rt.ps         root://eosatlas.cern.ch/"+outputfile+".ps\n"
        else:
            if float(config["DoShift"])>0:
                ostring+=cpcmd + " basic.root root://eosatlas.cern.ch/"+outputfile+"_basic_shifted_%.1f.root.bz2\n"%t0shift
                ostring+="root -b -q  basic.root '%s/graph_points.C(%f,%d)'\n"%(config["Calibdir"],t0shift,oiter+1)
                ostring+="cp -fv gp_* " + config["Batchdir"] + "/temp/.\n"
                if t0shift==0:
                    ostring+="cp -fv shiftres_* " + config["Batchdir"] + "/temp/.\n"
                ostring+="cp -fv basic.root " + config["Batchdir"] + "/temp/basic_shifted_%02d_%.2f.root\n"%(oiter+1,t0shift)
            else:
                ostring+="./bhadd dumfile basic.root\n"
                ostring+="bzip2 -v basic.root\n"
                ostring+=cpcmd + " basic.root.bz2 root://eosatlas.cern.ch/"+outputfile+"_basic.root.bz2\n"
                ostring+=cpcmd + " dumfile.stat root://eosatlas.cern.ch/"+outputfile+"_basic.stat\n"
                ostring+="cat TRT_StrawStatusOutput*ewFormat.txt  >> TRT_StrawStatusOutput.txt\n"
                ostring+="cat TRT_StrawStatusOutput*Voltage_trips.txt  >> TRT_StrawLVStatusOutput.txt\n"
                ostring+=cpcmd + " TRT_StrawStatusOutput.txt root://eosatlas.cern.ch/"+outputfile+"_TRT_StrawStatusOutput.txt\n"
                ostring+=cpcmd + " TRT_StrawLVStatusOutput.txt root://eosatlas.cern.ch/"+outputfile+"_TRT_StrawLVStatusOutput.txt\n"
                ostring+=cpcmd + " tracktuple.root root://eosatlas.cern.ch/"+outputfile+"_tracktuple.root\n"
                #ostring+=cpcmd + " log root://eosatlas.cern.ch/"+outputfile+"_atlog.txt\n"


        #ostring+="cp monitoring.root "+outputfile+"_monitoring.root\n"
        
        #ostring+="cp log "+outputfile+"_athenalog.txt\n"

    ostring+="ls -al\n"

    return ostring


def setup_batchscript_merge(config,inputfiles,outputfile,mergecmd,oiter):
    ostring="cd $WORKDIR\n"
    ostring+="export STAGE_SVCCLASS=atlcal\n"
    ostring+=config["SetupCmdRec"]+"\n"
    ostring+="echo\n"
    
    cpcmd="cp -fv "
    if config["TempOnCastor"]:
        cpcmd="xrdcp -f "

    #copy inputfile to local area
    for i in inputfiles:
        if config["CleanRRoot"] and not config["TempOnCastor"]:
            ostring+="mv -v "+i.split('.root')[0]+"*.root.bz2 .\n"
            ostring+="mv -v "+i.split('.root')[0]+"_tracktuple.root .\n"
            if (oiter==0): ostring+="mv -v "+i.split('.root')[0]+"_TRT_StrawStatusOutput.txt .\n"
        else:
            ostring+=cpcmd + " root://eosatlas.cern.ch/"+i.split('.root')[0]+"_basic.root.bz2 .\n"
            ostring+=cpcmd + " root://eosatlas.cern.ch/"+i.split('.root')[0]+"_tracktuple.root .\n"
            if (oiter==0): ostring+=cpcmd +" root://eosatlas.cern.ch/"+i.split('.root')[0]+"_TRT_StrawStatusOutput.txt .\n"

    if config["UseHist"]:
        #if (oiter>0): ostring+="ln -sf " + config["Batchdir"] + "/input/calibout_%02d.txt precision_constants.txt\n"%(oiter-1)
        ostring+="ln -sf " + config["Calibdir"] + "/bhadd.cpp\n"
        ostring+="ln -sf " + config["Calibdir"] + "/TRT_StrawStatus_merge.cc\n"
        ostring+="ln -sf " + config["Calibdir"] + "/Makefile\n"
        ostring+="make bhadd\n"
        ostring+="make TRT_StrawStatus_merge\n"
    else:
        ostring+="ln -sf /afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/testarea/hadd/getentries.cpp\n"
        ostring+="ln -sf /afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/testarea/hadd/Makefile\n"
        ostring+="make getentries\n"
    ostring+="cp -fv /afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/testarea/hadd/hadd2 .\n"
    ostring+="bunzip2 -v *.bz2\n"
    ostring+="ls -al\n"

    if config["UseHist"]:
        ostring+="./bhadd merged_basic.root trtcalib*basic*.root\n"
        ostring+="hadd  merged_tracktuple.root trtcalib*tracktuple.root\n"
        if (oiter==0): ostring+="./TRT_StrawStatus_merge merged_strawstatus trtcalib*TRT_StrawStatusOutput.txt\n"
    else:
        ostring+="hadd merged_basic.root trtcalib*basic*.root\n"
        ostring+="./getentries merged_basic.root > merged_basic.root.stat\n"
        if (oiter==0): ostring+="./TRT_StrawStatus_merge merged_strawstatus trtcalib*TRT_StrawStatusOutput.txt\n"
        
    ostring+="bzip2 -v merged_basic.root.part*\n"
    ostring +="""ls -1 trtcalib*.root | awk '{printf "SOURCEFILE %s\\n",$1}' >>  merged_basic.root.stat\n"""

    if outputfile.find("output")>=0:
        for calarg in ["_all"]:
            ostring+=cpcmd + " merged_basic.root.part0.bz2 root://eosatlas.cern.ch/" +outputfile+"_basic" + calarg + ".root.bz2\n"
            ostring+=cpcmd + " merged_basic.root.stat root://eosatlas.cern.ch/" +outputfile+"_basic" + calarg + ".stat\n"
            ostring+=cpcmd + " merged_tracktuple.root root://eosatlas.cern.ch/" +outputfile+ "_tracktuple.root\n"
            if (oiter==0): ostring+=cpcmd + " merged_strawstatus root://eosatlas.cern.ch/" +outputfile+ "_TRT_StrawStatusOutput.txt\n"
    else:
        ostring+=cpcmd + " merged_basic.root.part0.bz2 root://eosatlas.cern.ch/" +outputfile+ "_basic.root.bz2\n"
        ostring+=cpcmd + " merged_basic.root.stat root://eosatlas.cern.ch/" +outputfile+ "_basic.stat\n"
        ostring+=cpcmd + " merged_tracktuple.root root://eosatlas.cern.ch/" +outputfile+ "_tracktuple.root\n"
        if (oiter==0): ostring+=cpcmd + " merged_strawstatus root://eosatlas.cern.ch/" +outputfile+ "_TRT_StrawStatusOutput.txt\n"
        ostring+=cpcmd + " stat.stat root://eosatlas.cern.ch/" +outputfile+ "_basic.stat\n"
        
    #copy back outputfile
    if outputfile.find("castor:")>=0:
        ostring+="xrdcp -f trtcalib.root root://eosatlas.cern.ch/"+outputfile.split(":")[1]+"\n"

    ostring+="ls -al\n"

    return ostring


def setup_batchscript_pcalib(config,inputfile,calibcmd,caltag,calsetting,optstring):
    ostring="cd $WORKDIR\n"
    ostring+=config["SetupCmdCal"]+"\n"
    
    cpcmd="cp -fv"
    if config["TempOnCastor"]:
        cpcmd="xrdcp -f"
        
        filename=inputfile.split(".root")[0].split("/")[len(inputfile.split(".root")[0].split("/"))-1] + "_calib"
    
    if caltag.find('all')>=0:
        ostring+=cpcmd + " -fv "+inputfile+"_basic" + caltag + ".root.bz2 merged.root.bz2\n"
    else:
        ostring+=cpcmd + " -fv "+inputfile+"_basic_all.root.bz2 merged.root.bz2\n"

    ostring+="bunzip2 -v *.bz2\n"
    ostring+="ln -sf " + config["Calibdir"] + "/finedelays.txt\n"
    ostring+="ln -sf " + config["Calibdir"] + "/CalibrateTRT.cpp\n"
    ostring+="ln -sf " + config["Calibdir"] + "/Makefile\n"
    ostring+="make CalibrateTRT\n"
    ostring+= "./CalibrateTRT " + calsetting + " " + optstring + " " + config["MinRT"] + " " + config["MinT0"] + " " + config["RtRelation"] + " merged.root -1 | tee log.txt\n"
    ostring+="grep -Ev '1065353216' calibout_t0.txt > calibout_t0.txt.tmp; diff calibout_t0.txt calibout_t0.txt.tmp >> log.txt; echo T0 >> log.txt\n"
    ostring+="grep -Ev '0 0 0 0 0' calibout_t0.txt.tmp > calibout_t0.txt.cleaned; diff calibout_t0.txt.tmp calibout_t0.txt.cleaned >> log.txt; echo T0 >> log.txt\n" 
    ostring+="grep -Ev '1065353216' calibout_rt.txt > calibout_rt.txt.tmp; diff calibout_rt.txt calibout_rt.txt.tmp >> log.txt; echo RT >> log.txt\n"
    ostring+="grep -Ev '0 0 0 0 0' calibout_rt.txt.tmp > calibout_rt.txt.cleaned; diff calibout_rt.txt.tmp calibout_rt.txt.cleaned >> log.txt; echo RT >> log.txt\n"
    ostring+="cp calibout.root " + config["TempDir"] + "/" + filename + caltag + ".root\n"
    ostring+="cp calibout_t0.txt.cleaned " + config["TempDir"] + "/" + filename + caltag + "_t0.txt\n"
    ostring+="cp calibout_rt.txt.cleaned " + config["TempDir"] + "/" + filename + caltag + "_rt.txt\n"
    ostring+="cp log.txt " + config["TempDir"] + "/" + filename + caltag + ".log\n"

    return ostring


def setup_batchscript_txt2db(config,calibconstants,pooloutput,dboutput,caltags,couttags,oiter,batchdir):

# merges the calibration output, and/or generates a summary pdf file

    ostring="cd $WORKDIR\n"
    ostring+="export STAGE_SVCCLASS=atlcal\n"
    ostring+=config["SetupCmdRec"]+"\n"

    cpcmd="cp -fv"
    if config["TempOnCastor"]:
        cpcmd="xrdcp -f root://eosatlas.cern.ch/"


    if config["MakePlots"]:
        ostring += "#JUST REMAKE THE PLOTS\n"
        ostring +="\n#copy calibration root files & makeplots.cpp\n" 
        ostring += "cp -uv " + batchdir + "/output/trtcalib_%02d_histograms.root .\n" % (oiter+1)
        ostring += "ln -sf " + config["Calibdir"] + "/makeplots.cpp\n"
        ostring += "ln -sf " + config["Calibdir"] + "/Makefile\n"
        ostring +="\n#compile makeplots\n" 
        ostring += "make makeplots\n"
        ostring +="\n#run makeplots\n" 
        ostring +="./makeplots itersum trtcalib_%02d_histograms.root "%(oiter+1) + os.path.abspath('.') + "/lastconfigfile " + os.path.abspath('.') + "/batch/output/trtcalib_00_histograms.root\n"
        ostring +="\n#exit if something went wrong\n" 
        ostring +='if [ "$?" -ne "0" ]; then\n'
        ostring +="  echo 'RETURNCODE=' $? ', EXITING!, NO FILES COPIED!'\n"
        ostring +="  exit 1\n"
        ostring +="fi\n"
        ostring +="\n#copy back the files\n" 
        ostring +="cp -fv trtcalib_%02d_histograms.root "%(oiter+1) + batchdir + "/output\n"
        ostring +="\n#add page numbers\n" 
        ostring +="""echo 'import os,sys' > addnum.py\n"""
        ostring +="""echo 'for line in open("itersum.ps").readlines():' >> addnum.py\n"""
        ostring +="""echo '    if line.find("%%Page")>=0:' >> addnum.py\n"""
        ostring +="""echo '        pagenum=line.split("%%Page: ")[-1].split()[0]' >> addnum.py\n"""
        ostring +="""echo '        print line.replace("%%Page: %s %s"%(pagenum,pagenum),"%%Page: %s %s \\n/Helvetica-Bold findfont 36 scalefont setfont 0 -40 moveto (Page %s) show"%(pagenum,pagenum,pagenum)).strip()' >> addnum.py\n"""
        ostring +="""echo '    else:' >> addnum.py\n"""
        ostring +="""echo '        print line.strip();' >> addnum.py\n"""
        ostring +="python addnum.py > itersum_new.ps\n"
        ostring +="\n#create pdf\n" 
        ostring +="ps2pdf itersum_new.ps " + batchdir + "/output/itersum_%02d.pdf\n" % (oiter+1)
 
        return ostring

    ostring += "##IF THE FILES ARE FOUND IN THE TEMP DIRECTORY DO THE MERGING\n"
    ostring += "#if [ -f %s ]; then\n" % (config["TempDir"] + "/trtcalib_%02d_calib" % (oiter+1) + "_all.root")

    ostring +="\n  #copy calibration root files & makeplots.cpp\n" 
    if config["CleanCRoot"]:
        ostring += "  mv -v " + config["TempDir"] + "/trtcalib_%02d_calib" % (oiter+1) + "*.root .\n"
    else:
        for caltag in couttags:
            if (caltag != "_barrel"):  
                ostring += "  " + cpcmd + config["TempDir"] + "/trtcalib_%02d_calib" % (oiter+1) + caltag + ".root .\n" 

    ostring += "  " + cpcmd + config["TempDir"] + "/trtcalib_%02d_tracktuple" % (oiter+1) + ".root .\n"
    ostring += "  ln -sf " + config["Calibdir"] + "/makeplots.cpp\n"
    ostring += "  ln -sf " + config["Calibdir"] + "/Makefile\n"
    ostring += "  cp -fv /afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/testarea/hadd/hadd2 .\n"

    ostring +="\n  #compile makeplots\n" 
    ostring += "  make makeplots\n"
    ostring +="  ls -al\n"

    ostring +="\n  #merge calibration and the tracktuple root files\n" 
    ostring +="  echo MERGING CALIBRATION OUTPUTS AND TRACKTUPLE!\n" 
    ostring +="  hadd merged_%02d_histograms.root trtcalib_%02d_calib*.root trtcalib_%02d_tracktuple.root > hadd2.txt\n" % (oiter+1,oiter+1,oiter+1)

    ostring +="\n  #run makeplots\n" 
    if (int(oiter)==-1):
        ostring +="  ./makeplots itersum merged_%02d_histograms.root "%(oiter+1) + os.path.abspath('.') + "/lastconfigfile merged_%02d_histograms.root\n"%(oiter+1)
    else:
        ostring +="  ./makeplots itersum merged_%02d_histograms.root "%(oiter+1) + os.path.abspath('.') + "/lastconfigfile " + os.path.abspath('.') + "/batch/output/trtcalib_00_histograms.root\n"

    ostring +="\n  #exit if something went wrong\n" 
    ostring +='  if [ "$?" -ne "0" ]; then\n'
    ostring +="    echo 'RETURNCODE NOT =0 EXITING!, NO FILES COPIED!'\n"
    ostring +="    exit 1\n"
    ostring +="  fi\n"

    ostring +="\n  #copy back the files\n" 
    ostring +="  cp -fv merged_%02d_histograms.root "%(oiter+1) + batchdir + "/output/trtcalib_%02d_histograms.root\n" % (oiter+1)
    ostring +="\n  #add page numbers\n" 
    ostring +="""  echo 'import os,sys' > addnum.py\n"""
    ostring +="""  echo 'for line in open("itersum.ps").readlines():' >> addnum.py\n"""
    ostring +="""  echo '    if line.find("%%Page")>=0:' >> addnum.py\n"""
    ostring +="""  echo '        pagenum=line.split("%%Page: ")[-1].split()[0]' >> addnum.py\n"""
    ostring +="""  echo '        print line.replace("%%Page: %s %s"%(pagenum,pagenum),"%%Page: %s %s \\n/Helvetica-Bold findfont 36 scalefont setfont 0 -40 moveto (Page %s) show"%(pagenum,pagenum,pagenum)).strip()' >> addnum.py\n"""
    ostring +="""  echo '    else:' >> addnum.py\n"""
    ostring +="""  echo '        print line.strip();' >> addnum.py\n"""
    ostring +="  python addnum.py > itersum_new.ps\n"

    ostring +="\n  #create pdf\n" 
    ostring +="  ps2pdf itersum_new.ps " + batchdir + "/output/itersum_%02d.pdf\n" % (oiter+1)
    ostring +="  cat finedelays_-1.txt >> " + batchdir + "/output/finedelays_%02i.txt\n" % (oiter+1)
    ostring +="  cat finedelays_1.txt >> " + batchdir + "/output/finedelays_%02i.txt\n" % (oiter+1)

    ostring +="\n  #send a status mail\n" 
    ostring+= it_statusmail(config,oiter,False)

    ostring += "\n##ELSE COPY THE FILES FROM THE OUTPUT DIRECTORY AND UPDATE THEM\n"
    ostring += "#else\n"

    ostring += "\n  ##copy calibration root files & makeplots.cpp\n" 
    ostring += "  #cp -fv " + batchdir + "/output/trtcalib_%02d_histograms.root .\n" % (oiter+1)
    ostring += "  #cp -fv " + batchdir + "/output/trtcalib_%02d_tracktuple.root .\n" % (oiter+1)
    ostring += "  #ln -sf " + config["Calibdir"] + "/makeplots.cpp\n"
    ostring += "  #ln -sf " + config["Calibdir"] + "/Makefile\n"
    ostring += "  #cp -fv /afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/testarea/hadd/hadd2 .\n"

    ostring += "\n  ##compile makeplots\n" 
    ostring += "  #make makeplots\n"

    ostring += "\n  ##remove the old tracktuple from the root file\n" 
    ostring += "  #./makeplots restore trtcalib_%02d_histograms.root\n"%(oiter+1) 

    ostring += "\n  ##merge calibration and the new tracktuple root files\n" 
    ostring += "  #echo MERGING CALIBRATION OUTPUTS AND TRACKTUPLE!\n" 
    ostring += "  #./hadd2 merged_%02d_histograms.root trtcalib_%02d_histograms.root trtcalib_%02d_tracktuple.root > hadd2.txt\n" % (oiter+1,oiter+1,oiter+1)

    ostring += "\n  ##run makeplots\n" 
    ostring += "  #./makeplots itersum merged_%02d_histograms.root "%(oiter+1) + os.path.abspath('.') + "/lastconfigfile " + os.path.abspath('.') + "/batch/output/trtcalib_00_histograms.root\n"

    ostring += "\n  ##exit if something went wrong\n" 
    ostring += '  #if [ "$?" -ne "0" ]; then\n'
    ostring += "  #  echo 'RETURNCODE=' $? ', EXITING!, NO FILES COPIED!'\n"
    ostring += "  #  exit 1\n"
    ostring += "  #fi\n"

    ostring += "\n  ##copy back the files\n" 
    ostring += "  #cp -fv merged_%02d_histograms.root "%(oiter+1) + batchdir + "/output/trtcalib_%02d_histograms.root\n"%(oiter+1)

    ostring +="\n  ##add page numbers\n" 
    ostring +="""  #echo 'import os,sys' > addnum.py\n"""
    ostring +="""  #echo 'for line in open("itersum.ps").readlines():' >> addnum.py\n"""
    ostring +="""  #echo '    if line.find("%%Page")>=0:' >> addnum.py\n"""
    ostring +="""  #echo '        pagenum=line.split("%%Page: ")[-1].split()[0]' >> addnum.py\n"""
    ostring +="""  #echo '        print line.replace("%%Page: %s %s"%(pagenum,pagenum),"%%Page: %s %s \\n/Helvetica-Bold findfont 36 scalefont setfont 0 -40 moveto (Page %s) show"%(pagenum,pagenum,pagenum)).strip()' >> addnum.py\n"""
    ostring +="""  #echo '    else:' >> addnum.py\n"""
    ostring +="""  #echo '        print line.strip();' >> addnum.py\n"""
    ostring +="  #python addnum.py > itersum_new.ps\n"

    ostring += "\n  ##create pdf\n" 
    ostring += "  #ps2pdf itersum_new.ps " + batchdir + "/output/itersum_%02d.pdf\n\n" % (oiter+1)

    ostring += "#fi\n"

    return ostring


def setup_batchscript_mkcalin(config,calibconstants,pooloutput,dboutput,caltags,couttags,oiter,batchdir):
    ostring="cd $WORKDIR\n"
    ostring+="export STAGE_SVCCLASS=atlcal\n"
    ostring+=config["SetupCmdRec"]+"\n"

    cpcmd="cp -fv"
    if config["TempOnCastor"]:
        cpcmd="xrdcp -f "

    if config["SplitAC"]: excluded_caltags=['_-1','_1','_-2','_2','_barrel']
    else: excluded_caltags=['_-1','_1','_-2','_2','_1_0','_1_1','_1_2','_-1_0','_-1_1','_-1_2']
    couttags.sort()
    
    #copy straw status files for first iteration
    if oiter+1==0:
        ostring += cpcmd + " root://eosatlas.cern.ch/" + config["TempDir"] + "/trtcalib_%02d_TRT_StrawStatusOutput.txt" % (oiter+1) + " %s/output/TRT_StrawStatusOutput.txt\n"%config["Batchdir"]

    for caltag in couttags:
        if not ( (caltag == "error_Barrel")  | (caltag == "error_Endcap")):
	        ostring += cpcmd + " root://eosatlas.cern.ch/" + config["TempDir"] + "/trtcalib_%02d_calib" % (oiter+1) + caltag + "_rt.txt .\n" 
	        ostring += cpcmd + " root://eosatlas.cern.ch/" + config["TempDir"] + "/trtcalib_%02d_calib" % (oiter+1) + caltag + "_t0.txt .\n" 
	        #ostring += cpcmd + " root://eosatlas.cern.ch/" + config["TempDir"] + "/trtcalib_%02d_calib" % (oiter+1) + caltag + "_const.txt .\n" 
        else:
                ostring += cpcmd + " root://eosatlas.cern.ch/" + config["TempDir"] + "/trtcalib_%02d_calib" % (oiter+1) + caltag + "_rt.txt .\n"

    #append the output files
    if config["DoErrorOptimization"]:
            ostring += "if [ -f %s ]; then\n" % ("trtcalib_%02d_calib" % (oiter+1) + "_all_t0.txt")
            ostring += "  echo '# Fileformat=2' >  calibout.txt\n"
            ostring += "  echo '# RtRelation' >>  calibout.txt\n"
            for caltag in couttags:
                if not ( (caltag == "error_Barrel")  | (caltag == "error_Endcap")):
                        if caltag not in excluded_caltags:
                            if config["RtRelation"]=="binned":
                                ostring += "  cat trtcalib_%02d_calib" % (oiter+1) + caltag  + "_binrt.txt >> " + "calibout.txt\n"
                            else:
                                ostring += "  cat trtcalib_%02d_calib" % (oiter+1) + caltag  + "_rt.txt >> " + "calibout.txt\n"
            ostring += "  echo '# errors' >>  calibout.txt\n"
            for caltag in couttags:
                if ( (caltag == "error_Barrel")  | (caltag == "error_Endcap")):
                        if caltag not in excluded_caltags:
                            if config["RtRelation"]=="binned":
                                ostring += "  cat trtcalib_%02d_calib" % (oiter+1) + caltag  + "_binrt.txt >> " + "calibout.txt\n"
                            else:
                                ostring += "  cat trtcalib_%02d_calib" % (oiter+1) + caltag  + "_rt.txt >> " + "calibout.txt\n"

            ostring += "  echo '# StrawT0' >>  calibout.txt\n"
            for caltag in couttags:
                if not ( (caltag == "error_Barrel")  | (caltag == "error_Endcap")):
                        if caltag not in excluded_caltags:
                            ostring += "  cat trtcalib_%02d_calib" % (oiter+1) + caltag  + "_t0.txt >> " + "calibout.txt\n"
                            ostring += "  cat trtcalib_%02d_calib" % (oiter+1) + caltag  + "_const.txt >> " + "oldt0s.txt\n"
            ostring += "  echo '#GLOBALOFFSET 0.0000' >>  calibout.txt\n"
    else:
            ostring += "if [ -f %s ]; then\n" % ("trtcalib_%02d_calib" % (oiter+1) + "_all_t0.txt")
            ostring += "  echo '# Fileformat=1' >  calibout.txt\n"
            ostring += "  echo '# RtRelation' >>  calibout.txt\n"
            for caltag in couttags:
                if not ( (caltag == "error_Barrel")  | (caltag == "error_Endcap")):
                        if caltag not in excluded_caltags:
                            if config["RtRelation"]=="binned":
                                ostring += "  cat trtcalib_%02d_calib" % (oiter+1) + caltag  + "_binrt.txt >> " + "calibout.txt\n"
                            else:
                                ostring += "  cat trtcalib_%02d_calib" % (oiter+1) + caltag  + "_rt.txt >> " + "calibout.txt\n"
            ostring += "  echo '# StrawT0' >>  calibout.txt\n"
            for caltag in couttags:
                if not ( (caltag == "error_Barrel")  | (caltag == "error_Endcap")):
                        if caltag not in excluded_caltags:
                            ostring += "  cat trtcalib_%02d_calib" % (oiter+1) + caltag  + "_t0.txt >> " + "calibout.txt\n"
                            ostring += "  cat trtcalib_%02d_calib" % (oiter+1) + caltag  + "_const.txt >> " + "oldt0s.txt\n"
            ostring += "  echo '#GLOBALOFFSET 0.0000' >>  calibout.txt\n"

    ostring+="fi\n\n"
#    #For cosmic, normalize the t0, to keep it average constant
    if (config["MeanT0"]>0) &  ( (config["IsCosmic"] | config["IsCosmicMC"]  )  ):
       filetocorrect = "%s_%02d.txt" % (config["CalPrefix"],oiter+1)
       ostring += "python "+config["Calibdir"]+"/NormalizeT0.py calibout.txt " +str(config["MeanT0"])+"\n\n"


    #configure the filter for the calibration constants
    iter=''
    filterargs=''
    if oiter==-1: iter='start'
    else: iter='%02d'%oiter
    iterp1='%02d'%(oiter+1)
    if config["DoT0"][oiter+1]=='none':
        filterargs+='keept0+' #no T0 calibration
    elif config["DoRt"][oiter+1]=='none':
        filterargs+='keeprt+' #no Rt calibration
    if not config["UsePol0"][oiter+1]:
        #filterargs+='shiftt0+' #Shift T0s
        filterargs+='shiftrt+' #Shift Rts

    #apply filter
    ostring+="ln -sf " + config["Calibdir"] + "/cfilter.py\n"
    ostring+="python cfilter.py %s_%s.txt calibout.txt oldt0s.txt "%(config["CalPrefix"],iter) + filterargs.strip('+') + '\n\n'

    #copy back files
    ostring+="cp -fv dbconst.txt %s_%02d.txt\n" % (config["CalPrefix"],oiter+1)
    ostring += cpcmd + "dictconst.txt root://eosatlas.cern.ch/" + config["TempDir"] + "/trtcalib_%02d_oldt0s.txt\n"%(oiter+1) 
    
    return ostring


##################################################
############### SUBMIT JOBS ######################
##################################################

def submit_parallel_job_batch(config,inputfiles,calibconstants,outputfile,sname,jname,waitlist,calsettings2,oiter,ijob):
    outf=open(sname,"w")
    outf.write("#!/bin/sh\n")
    ostring=setup_batchscript_parallel(config,inputfiles,calibconstants,outputfile.split(".root")[0],False,"","",oiter,calsettings2,ijob)
    outf.write(ostring)
    outf.close()
    os.system("chmod 755 "+sname)
    
    #print ijob%2
#%len(config["Batchqueue"].split(','))
    if len(config["Batchqueue"].split(','))>1:
        cmd='bsub -q '+config["Batchqueue"].split(',')[ijob%len(config["Batchqueue"].split(','))]+' -J '+jname
    else:
        cmd='bsub -q '+config["Batchqueue"].split(',')[0]+' -J '+jname
 
    if len(waitlist)>0:
        cmd+=" -w '"
        for i in range(len(waitlist)-1):
            cmd+="ended("+waitlist[i]+") && "
        cmd+="ended("+waitlist[len(waitlist)-1]+")'"
    cmd+=" -eo %s.e -oo %s.o" % (sname,sname)
    cmd+=" "+sname

    print cmd
    if config["SubmitRecJobs"]:
        os.system(cmd)


def submit_merge_job_batch(config,inputfiles,outputfile,mergecmd,batchq,sname,jname,waitlist,oiter):
    outf=open(sname,"w")
    outf.write("#!/bin/sh\n")
    ostring=setup_batchscript_merge(config,inputfiles,outputfile.split(".root")[0],mergecmd,oiter)
    outf.write(ostring)
    outf.close()
    os.system("chmod 755 "+sname)

    cmd='bsub -q '+batchq+' -J '+jname

    #if config["SubmitRecJobs"]:
    if len(waitlist)>0:
        cmd+=" -w '"
        for i in range(len(waitlist)-1):
            cmd+="ended("+waitlist[i]+") && "
        cmd+="ended("+waitlist[len(waitlist)-1]+")'"

    cmd+=" -eo %s.e -oo %s.o" % (sname,sname)
    cmd+=" "+sname

    print cmd
    if config["SubmitMergeJobs"]:
        os.system(cmd)


def submit_pcalib_job_batch(config,inputfile,calibcmd,batchq,caltag,sname,jname,waitlist,calsetting,optstring,datafile,calibconst,oiter,calsettings2):

    outf=open(sname,"w")
    outf.write("#!/bin/sh\n")
    ifile=[]
    if config["DoAthenaCalib"]:
        if type(datafile)==TupleType:
            ifile.append(datafile[1])
        outfile=config["TempDir"] + "/" + inputfile.split(".root")[0].split("/")[-1] + "_calib" + caltag
        ostring=setup_batchscript_parallel(config,ifile,calibconst,outfile,True,inputfile.split(".root")[0],caltag,oiter,calsettings2,0)
    else:
        ostring=setup_batchscript_pcalib(config,inputfile.split(".root")[0],calibcmd,caltag,calsetting,optstring)
    outf.write(ostring)
    outf.close()
    os.system("chmod 755 "+sname)

    cmd='bsub -q '+batchq+' -J '+jname

    if config["SubmitMergeJobs"]:
        if len(waitlist)>0:
            cmd+=" -w '"
            for i in range(len(waitlist)-1):
                cmd+="ended("+waitlist[i]+") && "
            cmd+="ended("+waitlist[len(waitlist)-1]+")'"

    cmd+=" -eo %s.e -oo %s.o" % (sname,sname)
    cmd+=" "+sname

    print cmd
    if config["SubmitCalJobs"]:
        os.system(cmd)


def submit_convert_job_batch(config,calibconstants,pooloutput,dboutput,sname,jname,waitlist,caltags,couttags,oiter,batchdir):
    outf=open(sname,"w")
    outf.write("#!/bin/sh\n")
    ostring=setup_batchscript_txt2db(config,calibconstants,pooloutput,dboutput,caltags,couttags,oiter,batchdir)
    outf.write(ostring)
    outf.close()
    os.system("chmod 755 "+sname)

    cmd='bsub -q '+config["Batchqueue"].split(',')[0]+' -J '+jname

    if config["SubmitCalJobs"]:
        if len(waitlist)>0:
            cmd+=" -w '"
            for i in range(len(waitlist)-1):
                cmd+="ended("+waitlist[i]+") && "
            cmd+="ended("+waitlist[len(waitlist)-1]+")'"

    cmd+=" -eo %s.e -oo %s.o" % (sname,sname)
    cmd+=" "+sname

    print cmd
    if config["SubmitCnvJobs"]:
        #extract calib constants from a caliboutput text file
        if (config["RTglobalValues"].find('file:')) >= 0:
            extract_const('rt',config["RTglobalValues"].split('file:')[1],'%s/tempconst_rt.txt' % config["Batchdir"])
        if (config["T0globalValues"].find('file:')) >= 0:
            extract_const('t0',config["T0globalValues"].split('file:')[1],'%s/tempconst_t0.txt' % config["Batchdir"])
        os.system(cmd)

def submit_mkcalin_job_batch(config,calibconstants,pooloutput,dboutput,sname,jname,waitlist,caltags,couttags,oiter,batchdir):
    outf=open(sname,"w")
    outf.write("#!/bin/sh\n")
    ostring=setup_batchscript_mkcalin(config,calibconstants,pooloutput,dboutput,caltags,couttags,oiter,batchdir)
    outf.write(ostring)
    outf.close()
    os.system("chmod 755 "+sname)

    cmd='bsub -q '+config["Batchqueue"].split(',')[0]+' -J '+jname

    if config["SubmitCalJobs"]:
        if len(waitlist)>0:
            cmd+=" -w '"
            for i in range(len(waitlist)-1):
                cmd+="ended("+waitlist[i]+") && "
            cmd+="ended("+waitlist[len(waitlist)-1]+")'"

    cmd+=" -eo %s.e -oo %s.o" % (sname,sname)
    cmd+=" "+sname

    print cmd
    if config["SubmitCnvJobs"] and not config["MakePlots"]:
        #extract calib constants from a caliboutput text file
        if (config["RTglobalValues"].find('file:')) >= 0:
            extract_const('rt',config["RTglobalValues"].split('file:')[1],'%s/tempconst_rt.txt' % config["Batchdir"])
        if (config["T0globalValues"].find('file:')) >= 0:
            extract_const('t0',config["T0globalValues"].split('file:')[1],'%s/tempconst_t0.txt' % config["Batchdir"])
        os.system(cmd)



##########################################################
############# SECONDARY JOB SCEDULERS ####################            
##########################################################        

def run_parallel_batch(config,inputlist,calibconstants,iter,waitlist,calsettings2):
    #run only maxjobs jobs (or less in case there are less than maxjobs inputfiles

    # try to make equal sized jobs
    if type(inputlist[0])==TupleType:
        
        totsizeall=0;
        for file in inputlist:
            totsizeall+=file[0]


        if config["SingleFile"]:
            inputlist2=[]
            inputlist2.append(inputlist[-1])
            #inputlist2.append(inputlist[int(len(inputlist)/2)])
            singlesize=inputlist2[0][0]
            inputlist=inputlist2
            print "USING FILE       : %s" % inputlist[0][1].split('/')[-1]

        totsize=0;
        for file in inputlist:
            totsize+=file[0]
        maxsize=totsize/config["MaxParallel"]
        print     "TOTAL DATA SIZE  : %i (%f GB)" % (totsizeall,float(totsizeall)/(1024*1024*1024))
        if config["SingleFile"]:
            print "SIZE OF FILE     : %i (%f GB)" % (singlesize,float(singlesize)/(1024*1024*1024))
            print "FRACTION OF DATA : %f" % (float(singlesize)/float(totsizeall))
        else:
            print "MAX JOB SIZE     : %i" %maxsize
            print

        jobsize=0
        ijob=0
        i=[]
        dummyshiftfiles=[]
        for file in inputlist:
            jobsize+=file[0]
            i.append(file[1])
            if jobsize>maxsize:
                 jobsize=0
                 ijob=ijob+1
                 #if ijob==5: config["shiftfiles"]=i
                 dummyshiftfiles+=i
                 i=[]
        config["shiftfiles"]=[]
        if float(config["DoShift"])>0:
            print 'SELECTING FILES!' 
            for shf in dummyshiftfiles:
                if config["NoRec"]:
                    config["shiftfiles"].append(shf)
                else:
                    if os.popen('export STAGE_SVCCLASS=atlcal; stager_qry -M ' + shf.split('castor:')[-1]).read().find("STAGED")>=0:
                        config["shiftfiles"].append(shf)

        jobsize=0
        ijob=0
        ifile=0
        i=[]
        outlist=[]
        jlist=[]
        for file in inputlist:
            ifile=ifile+1
            jobsize+=file[0]
            i.append(file[1])
            if jobsize>maxsize:
                 print "JOB %i CONTAINS %i BYTES (%i FILE(S))" % (ijob+1,jobsize,ifile)
                 ifile=0
                 jobsize=0
                 ijob=ijob+1
                 if float(config["DoShift"])>0:
                     sname="trtcal_%02d_%04d_t0shift.sh" % (iter,ijob)
                 else:
                     sname="trtcal_%02d_%04d.sh" % (iter,ijob)
                 jname=config["JobPrefix"]+"%02d_%04d" % (iter,ijob)
                 outname=config["TempReadDir"]
                 outname+="/trtcalib_%02d_%04d.root" % (iter,ijob)
                 if not ijob in config["ExcludeRuns"]:
                     outlist.append(outname)
                     jlist.append(jname)                     
                     submit_parallel_job_batch(config,i,calibconstants,outname,sname,jname,waitlist,calsettings2,iter-1,ijob)
                 else:
                     print 'EXCLUDED!!'
                 i=[]
        return jlist,outlist
      
    if len(inputlist)>config["MaxParallel"]:
        if len(inputlist)%config["MaxParallel"]==0: 
            files_per_job=len(inputlist)/config["MaxParallel"]
        else:
            files_per_job=len(inputlist)/config["MaxParallel"]+1
        iflist=[]
        start=-1
        end=-1
        for i in range(len(inputlist)/files_per_job):
            start=i*files_per_job
            end=(i+1)*files_per_job
            if end>(len(inputlist)):
                end=len(inputlist)
            if start>=len(inputlist)-1:
                print "More Jobs than inputfiles!!!"
            else:
                iflist.append(inputlist[start:end])
        if end<len(inputlist):
            iflist.append(inputlist[end:])

        count=1
        jlist=[]
        outlist=[]
        for i in iflist:
            outname=config["TempReadDir"]
            outname+="/trtcalib_%02d_%04d.root" % (iter,count)
            outlist.append(outname)
            if float(config["DoShift"])>0:
                sname="trtcal_%02d_%04d_t0shift.sh" % (iter,count)
            else:
                sname="trtcal_%02d_%04d.sh" % (iter,count)
            jname=config["JobPrefix"]+"%02d_%04d" % (iter,count)
            submit_parallel_job_batch(config,i,calibconstants,outname,sname,jname,waitlist,calsettings2,iter-1,0)
            jlist.append(jname)
            count=count+1
    else:
        files_per_job=1
        iflist=[]
        start=-1
        end=-1
        for i in range(len(inputlist)): 
            start=i
            end=(i+1)
            iflist.append(inputlist[start:end])
    
        count=1
        jlist=[]
        outlist=[]
        for i in iflist:
            print i
            outname=config["TempReadDir"]
            outname+="/trtcalib_%02d_%04d.root" % (iter,count)
            outlist.append(outname)
            if float(config["DoShift"])>0:
                sname="trtcal_%02d_%04d_t0shift.sh" % (iter,count)
            else:
                sname="trtcal_%02d_%04d.sh" % (iter,count)
            jname=config["JobPrefix"]+"%02d_%04d" % (iter,count)
            submit_parallel_job_batch(config,i,calibconstants,outname,sname,jname,waitlist,calsettings2,iter-1,0)
            jlist.append(jname)
            count=count+1


    return jlist,outlist


def run_merge_batch_partial(config,inputlist,mergecmd,batchq,iter,waitlist,counter):
    jlist=[]

    outname=config["TempDir"]
    outname+="/trtcalib_%02d_merge_%03d.root" % (iter,counter)
    sname="trtcal_%02d_merge_%03d.sh" % (iter,counter)
    jname=config["JobPrefix"]+"%02dm%03d" % (iter,counter)
    
    submit_merge_job_batch(config,inputlist,outname,mergecmd,batchq,sname,jname,waitlist,iter)
    jlist.append(jname)

    return jlist,outname

def run_merge_batch_final(config,inputlist,outdir,mergecmd,batchq,iter,waitlist):
    jlist=[]

    outname=outdir
    outname+="/trtcalib_%02d.root" % iter
    sname="trtcal_%02d_merge.sh" % iter
    jname=config["JobPrefix"]+"%02dmerge" % iter
    
    submit_merge_job_batch(config,inputlist,outname,mergecmd,batchq,sname,jname,waitlist,iter)
    jlist.append(jname)

    return jlist,outname


def run_parallel_calib_batch(config,inputfile,calibcmd,batchq,iter,waitlist,datafile,calibconst,oiter,calsettings2):
    jlist=[]
    caltags=[]
    couttags=[]

    icalib=0
    for calarg in calsettings2:
        print calarg
        sname="trtcal_%02d_calib" % iter + calarg + ".sh"
        jname=config["JobPrefix"]+"%02dc%02d" % (iter,icalib)

        if calarg == "error_Barrel":
                if config["DoErrorOptimization"][iter]:
                        submit_pcalib_job_batch(config,inputfile,calibcmd,batchq,calarg,sname,jname,waitlist,calsettings2[calarg],calsettings2[calarg],datafile,calibconst,oiter,calsettings2)
                        jlist.append(jname)
                        caltags.append(calarg)
                        couttags.append(calarg)
                        icalib=icalib+1
        elif calarg == "error_Endcap":
                if config["DoErrorOptimization"][iter]:
                        submit_pcalib_job_batch(config,inputfile,calibcmd,batchq,calarg,sname,jname,waitlist,calsettings2[calarg],calsettings2[calarg],datafile,calibconst,oiter,calsettings2)
                        jlist.append(jname)
                        caltags.append(calarg)
                        couttags.append(calarg)
                        icalib=icalib+1
        else:
                submit_pcalib_job_batch(config,inputfile,calibcmd,batchq,calarg,sname,jname,waitlist,calsettings2[calarg]["sel"],calsettings2[calarg]["opt"],datafile,calibconst,oiter,calsettings2)
                jlist.append(jname)
                caltags.append(calarg)
                couttags.append(calarg)
                icalib=icalib+1
        
    return jlist,caltags,couttags


def run_convert_batch(config,calibconstants,iter,waitlist,caltags,couttags,oiter,batchdir):
    jlist=[]

    sname="trtcal_%02d_cnv.sh" % iter
    jname=config["JobPrefix"]+"%02dcnv" % iter
    
    pooloutput="%s_%02d.pool.root" % (config["CalPrefix"],iter)
    dboutput="%s_%02d.db" % (config["CalPrefix"],iter)

    submit_convert_job_batch(config,calibconstants,pooloutput,dboutput,sname,jname,waitlist,caltags,couttags,oiter,batchdir)
    jlist.append(jname)

    return jlist


def run_mkcalin_batch(config,calibconstants,iter,waitlist,caltags,couttags,oiter,batchdir):
    jlist=[]

    sname="trtcal_%02d_mci.sh" % iter
    jname=config["JobPrefix"]+"%02dmci" % iter
    
    pooloutput="%s_%02d.pool.root" % (config["CalPrefix"],iter)
    dboutput="%s_%02d.db" % (config["CalPrefix"],iter)

    submit_mkcalin_job_batch(config,calibconstants,pooloutput,dboutput,sname,jname,waitlist,caltags,couttags,oiter,batchdir)
    jlist.append(jname)

    return jlist


def run_local(config,startconstants,flist):

    calibconstants=""
    if startconstants=="db":
        calibconstants=""
    else:
        if not os.path.isfile(startconstants):
            print "File with start calib constants does not exist: %s" % startconstants 
            print "Check configuration file!"
            sys.exit(-1)
        calibconstants=startconstan
    dumscript=setup_batchscript_parallel(config,flist,calibconstants,"local",False,"","",0)
    dumscript=dumscript.replace("EvtMax = -1","EvtMax = 10")
    dumscript=dumscript.replace("cd $WORKDIR\n","")
    dumscript=dumscript.replace("rfcp","if [ ! -f %s ] ; then\n rfcp %s .\nfi\n#" % (flist[0].split("/")[len(flist[0].split("/"))-1],flist[0].split(":")[1]))
    dumscript=dumscript.split(" >log 2>&1")[0] + "\n"
    os.system("rm -rf WORKDIR")
    os.system("mkdir WORKDIR")
    locscript=open("WORKDIR/runlocal.sh","w")
    locscript.write(dumscript)
    locscript.close()
    print "cd to WORKDIR and run runlocal.sh\n"



######################################################
############### MAIN JOB SCEDULER ####################            
######################################################

def run_iter(config,inputlist,iter,waitlist,startconstants):
    oiter=iter-1
    print "DoRt ", config["DoRt"][oiter+1]
    print "DoT0 ", config["DoT0"][oiter+1]
    print "Do Xenon/Argon straws??  ", config["DoArXe"]
    if config["DoRt"][oiter+1] == 'none':
     calsettings2=create_calsettings(config,'detector', config["DoT0"][oiter+1])
     print "DoRT at detector level, but later will be corrected"
    else :
     calsettings2=create_calsettings(config, config["DoRt"][oiter+1] , config["DoT0"][oiter+1])

    if oiter<0:
        if startconstants=="db":
            calibconstants=""
        else:
            if not os.path.isfile(startconstants):
                print "File with start calib constants does not exist: %s" % startconstants 
                print "Check configuration file!"
                sys.exit(-1)
            calibconstants=startconstants            
    else:
        calibconstants="%s_%02d.txt" % (config["CalPrefix"],oiter)

    calconst=calibconstants    

    jlist,outlist=run_parallel_batch(config,inputlist,calibconstants,iter,waitlist,calsettings2)
    if (not config["SubmitRecJobs"]):
        jlist=[]
        
    calibconstants="%s_%02d.txt" % (config["CalPrefix"],iter)
    calibconstantspool="%s_%02d.pool.root" % (config["CalPrefix"],iter)
    calibconstantsdb="%s_%02d.db" % (config["CalPrefix"],iter)

    counter=0
    max_merge=4

    #setupcmd=config["SetupCmd"]
    calibcmd=config["CalibCmd"]
    mergecmd=config["MergeCmd"]
    mergetxtcmd=config["MergeTxtCmd"]
    constdir=config["ConstDir"]
    batchq=config["Batchqueue"].split(',')[0]
    batchdir=config["Batchdir"]

    while len(outlist)>max_merge:
        noutlist=[]
        wlist=[]
        intermediate=len(outlist)/max_merge
        for i in range(intermediate):
            start=i*max_merge
            end=(i+1)*max_merge
            plist,oname=run_merge_batch_partial(config,outlist[start:end],mergecmd,batchq,iter,jlist[start:end],counter)
            counter=counter+1
            for i in plist:
                wlist.append(i)
            noutlist.append(oname)

        if intermediate*max_merge<len(outlist):
            start=intermediate*max_merge
            end=len(outlist)
            plist,oname=run_merge_batch_partial(config,outlist[start:end],mergecmd,batchq,iter,jlist[start:end],counter)
            counter=counter+1
            for i in plist:
                wlist.append(i)
            noutlist.append(oname)

        #now copy over noutlist and wlist
        outlist=noutlist
        jlist=wlist

    #now run final merge procedure
    #jlist,outname=run_merge_batch_final(config,outlist,outdir,mergecmd,batchq,iter,jlist)
    jlist,outname=run_merge_batch_final(config,outlist,config["TempDir"],mergecmd,batchq,iter,jlist)

    #run the parallel calibration
    inputlist.sort()
    mlist,caltags,couttags=run_parallel_calib_batch(config,outname,calibcmd,batchq,iter,jlist,inputlist[1],calconst,oiter,calsettings2)

    #make calibconst text files
    nlist = run_mkcalin_batch(config,calibconstants,iter,mlist,caltags,couttags,oiter,batchdir)

    #merge root files and generate db files
    olist = run_convert_batch(config,calibconstants,iter,nlist,caltags,couttags,oiter,batchdir)

    return nlist


###############################################
############## SOME FUNCTIONS #################
###############################################
def extractDDV2(config,sflist):
    print ""
    print "#####################################################################################################################################"
    print "##           "
    print "##           Extracting detector description version....."
    print "##           Extracting conditions tags....."
    print "##           Extracting field setup....."
    print "##           This can take a while, please wait.................."
    print "##           "
    user             =os.popen("echo $USER").readlines();
    testing = "/tmp/"+ user[0].split("\n")[0] + "/DumpSetting.sh"
    outf=open(testing,"w")
    outf.write("#!/bin/sh\n")
    ostring="# Script to dump some parameters\n"
    ostring+="cd /tmp/$USER/ \n"
    ostring+="export STAGE_SVCCLASS=atlcal\n"
    ostring+=config["SetupCmdRec"]+"\n"
    dumfile=(sflist[0])[1].split("castor:")[1]
    print "##           %s" % dumfile
    jO="""
inFile = '%s'""" % dumfile
    jO+="""
inputFileSummary = {}
import PyUtils.AthFile as athFile
fi = athFile.fopen(inFile)

inputFileSummary = fi.fileinfos
runNumber=inputFileSummary['run_number'][0]
geometry=inputFileSummary['geometry']
condtag=inputFileSummary['conditions_tag']
taginfo=inputFileSummary["tag_info"]
release=taginfo['AtlasRelease']
# --- Autosetup of magnetic field
from AthenaCommon.BFieldFlags import jobproperties
from CoolConvUtilities.MagFieldUtils import getFieldForRun
cool=getFieldForRun(runNumber)
jobproperties.BField.barrelToroidOn=False
jobproperties.BField.endcapToroidOn=False
jobproperties.BField.solenoidOn=False
if cool.toroidCurrent()>100.:
        jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
        jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
if cool.solenoidCurrent()>100.:
        jobproperties.BField.solenoidOn.set_Value_and_Lock(True)



print "=================== AUTO CONFIGURED B FIELD AND DETECTOR GEOMETRY: ======================="
print "===     Release                  : ", release
print "===     RunNumber                : ", runNumber
print "===     DetGeometry              : ", geometry
print "===     Conditionstags           : ", condtag
print "===     BField.barrelToroidOn    : ",jobproperties.BField.barrelToroidOn()
print "===     BField.endcapToroidOn    : ",jobproperties.BField.endcapToroidOn()
print "===     BField.solenoidOn        : ",jobproperties.BField.solenoidOn()
print "=========================================================================================="

theApp.EvtMax = 1
"""
    ostring+="cat > joboptions.py  <<EOF\n"
    ostring+=jO+"\nEOF\n"
    ostring+="athena joboptions.py > /tmp/$USER/logDUMP\n"
    outf.write(ostring)
    outf.close()
    os.system("chmod 755 /tmp/$USER/DumpSetting.sh")
    os.system("/tmp/$USER/DumpSetting.sh")

    release             =os.popen(" less /tmp/$USER/logDUMP | grep Release | awk '{print $4}'").readlines();
    runnumber           =os.popen(" less /tmp/$USER/logDUMP | grep RunNumber | awk '{print $4}'").readlines();
    detgeometry         =os.popen(" less /tmp/$USER/logDUMP | grep DetGeometry | awk '{print $4}'").readlines();
    conditionstag1       =os.popen(" less /tmp/$USER/logDUMP | grep Conditionstags | awk '{print $4}'").readlines();
    conditionstag	=  conditionstag1[0].split(",")[0]
    barreltoroidon      =os.popen(" less /tmp/$USER/logDUMP | grep BField.barrelToroidOn | awk '{print $4}'").readlines();
    endcaptoroidon      =os.popen(" less /tmp/$USER/logDUMP | grep BField.endcapToroidOn | awk '{print $4}'").readlines();
    solenoidon          =os.popen(" less /tmp/$USER/logDUMP | grep BField.endcapToroidOn | awk '{print $4}'").readlines();

    release1=release[0].split("-")
    release2= release1[1].split("\n")[0] + "," + release1[0] + ",32,runtime"

    print "## "
    print "##		The run number seen is:         %s"  % runnumber[0].split("\n")[0]
    print "## "
    print "##           The Release Used in the reco was: %s"  % release2
    print "##                   the one in the configuration file is: %s"  %config["ReleaseRec"]
    print "## "
    print "##		The Detector geometry  seen is: %s"  % detgeometry[0].split("\n")[0]
    print "##			the one in the configuration file is: %s"  %config["DetDescVer"]
    print "## "
    print "##		The Conditions Tag seen is:     %s"  % conditionstag.split("\n")[0]
    print "##			the one in the configuration file is: %s"  %config["GLtag"]
    print "## "
    print "##		The toroid for barrel is set:   %s"  % barreltoroidon[0].split("\n")[0]
    print "##			the one in the configuration file is: %s"  %config["barrelToroid"]
    print "## "
    print "##		The toroid for endcap is set:   %s"  % endcaptoroidon[0].split("\n")[0]
    print "##			the one in the configuration file is: %s"  %config["endcapToroid"]
    print "## "
    print "##		The solenoid is set:            %s"  % solenoidon[0].split("\n")[0]
    print "##			the one in the configuration file is: %s"  %config["Solenoid"]
    print "##      "
    print "##		Would you like to use settings read from file (recommended), say [y]"
    print "##           	in case you want to use the settings in config file say [n]"
    print "##      "
    inp = "empty"
    while ( (inp != 'y') & (inp != 'n') & (inp != 'yes') & (inp != 'no')  ):
            inp = raw_input("##           Enter yes [y] or no [n] ")
    print "##      "
    print "##      "
    print "##      "
    print "#####################################################################################################################################"
    if ((inp == 'y')|  (inp == 'yes')) :
        print "##           Contants will be changed"
        config["ReleaseRec"]    =       release2
        config["DetDescVer"]	=	detgeometry[0].split("\n")[0]
        config["GLtag"]		=	conditionstag.split("\n")[0]
        config["barrelToroid"]	=	barreltoroidon[0].split("\n")[0]
        config["endcapToroid"]	=	endcaptoroidon[0].split("\n")[0]
        config["Solenoid"] 	=	solenoidon[0].split("\n")[0]	


    return 0

def extractDDV(config,sflist):
    print
    print "Extracting detector description version (can take a few seconds) ...\n"
    dumfile=(sflist[0])[1].split("castor:")[1]
    os.system("rfcp %s dumfile" % dumfile)
    vertag=os.popen("%s; python %s/dumpVersion.py -f dumfile | grep GeoAtlas | grep ATLAS | awk '{print $2}'" % (config["SetupCmdCal"],config["Calibdir"])).readlines(); 
    glotag=os.popen("%s; python %s/dumpVersion.py -f dumfile | grep IOVDbGlobalTag | awk '{print $2}'" % (config["SetupCmdCal"],config["Calibdir"])).readlines(); 
    atlrel=os.popen("%s; python %s/dumpVersion.py -f dumfile | grep AtlasRelease | awk '{print $2}'" % (config["SetupCmdCal"],config["Calibdir"])).readlines(); 
    print "version is " + vertag[0].split("\n")[0]
    print "global tag is " + glotag[0].split("\n")[0]
    print "ATLAS release is " + atlrel[0].split("\n")[0]
    os.system("rm -f dumfile")    

    return vertag[0].split("\n")[0]

def it_statusmail(config,oiter,cnvfail):
    #ostring= "echo 'https://trtofflinecalibration.web.cern.ch/TRTOfflineCalibration/CalibrationRunList/%s//%s/calsum_%02i' > mail.txt\n" % (config["Dataset"],config["Tag"],oiter+1)
    ostring= ""
    ostring+="  echo >> mail.txt\n"
    ostring+="  echo 'FAILED:' >> mail.txt\n"
    if cnvfail:
        ostring+="  echo 'CNV job' >> mail.txt\n"
    ostring+="  grep -H 'Exited with'  %s/*_%02i_*.o | awk 'BEGIN {FS=\"/\"} ; {print $(NF-0)}'| awk 'BEGIN {FS=\":\"} ; {printf \"%%3i %%-30s %%20s\\n\",NR,$1,$2}' >> mail.txt\n" %  (config["Localdir"],oiter+1)    
    ostring+="  echo 'SUCCESSFULL:' >> mail.txt\n"
    ostring+="  grep -H 'Successfully completed' %s/*_%02i_*.o | awk 'BEGIN {FS=\"/\"} ; {print $(NF-0)}'| awk 'BEGIN {FS=\":\"} ; {printf \"%%3i %%-30s %%20s\\n\",NR,$1,$2}' >> mail.txt\n" %  (config["Localdir"],oiter+1)    
    ostring+="  cat mail.txt | mail -s 'TRT-CALIB-%s it. %02i exit status' %s >> mail.txt\n" % (config["JobPrefix"],oiter+1,config["UserMail"]) 
    
    return ostring

# function to extract calibration constants from a txt file
def extract_const(type,file,tempfile):
    fi=open(file,'r')
    fo=open(tempfile,'w')
    if type=='t0': ndat=3
    if type=='rt': ndat=6
    ostring=''
    lines=fi.readlines()
    i=0
    for line in lines:
        values=line.split(':')
        if len(values)>1:
            #print line
            #print values[1].split(' ')
            if len(values[1].split(' '))==ndat:
                i=i+1
                ostring+=line
    fo.write(ostring)            
    print 'extracted %i %s-constants to file: %s' % (i,type,tempfile)
    fi.close()
    fo.close()    

def analyzeJob(iteration,stepid):    
    files=os.popen('ls -tr trtcal_%02i_%s*.sh.o'% (iteration, stepid)).readlines()
    rfiles=os.popen('ls -tr trtcal_%02i_%s*.sh'% (iteration, stepid)).readlines()

    calctime=True
    njobstot=len(rfiles)
    
    njobs=0
    for i,file in enumerate(files):
        status='UNKNOWN'
        fbase=file.split('.sh')[0]
        finfo=(os.popen('ls -al --time-style=long-iso %s.sh.o'%fbase).readline()).split()
        rfinfo=(os.popen('ls -al --time-style=long-iso %s.sh'%fbase).readline()).split()
        contents=os.popen('cat %s.sh.o'%fbase).readlines()
        for content in contents:
            if content.find('CPU time')>=0:
                cputime=(content.split('\n')[0]).split()[3]
            if content.find('Exited with exit code')>=0:
                status='ERROR'
            if content.find('Successfully completed')>=0:
                status='OK'            

        if i==0:        
            year0=int(finfo[5].split('-')[0])
            month0=int(finfo[5].split('-')[1])
            day0=int(finfo[5].split('-')[2])
            hour0=int(finfo[6].split(':')[0])
            min0=int(finfo[6].split(':')[1])
            ttup0=(year0,month0,day0,hour0,min0,0,0,0,0)
            tm0=mktime(ttup0)
        year=int(finfo[5].split('-')[0])
        month=int(finfo[5].split('-')[1])
        day=int(finfo[5].split('-')[2])
        hour=int(finfo[6].split(':')[0])
        min=int(finfo[6].split(':')[1])    
        ttup=(year,month,day,hour,min,0,0,0,0)
        tm=mktime(ttup)

        ryear=int(rfinfo[5].split('-')[0])
        rmonth=int(rfinfo[5].split('-')[1])
        rday=int(rfinfo[5].split('-')[2])
        rhour=int(rfinfo[6].split(':')[0])
        rmin=int(rfinfo[6].split(':')[1])    
        rttup=(ryear,rmonth,rday,rhour,rmin,0,0,0,0)
        rtm=mktime(rttup)

        statline=''
        
        njobs=njobs+1
        statline = '%3i %s %s  %-20s:  status = %-8s, CPU time = %-12s, realtime = %s' % (i+1,finfo[5],finfo[6],fbase,status,"%3.1f min" % (float(cputime)/60),"%3.0f min" % ((tm-rtm)/60))
        if tm<rtm:
            statline += "   ... old"
        print statline

    return njobs,njobstot,statline
            
def build_filelist(inputdir,pattern,minlumi,maxlumi):
    flist=[]
    sflist=[]
    fdict={}
    lumidict={}

    index = -1
    for idir,dir in enumerate(inputdir):
        if dir.find("castor:")>=0:
            index = index + 1
            min = int(minlumi[index])
            max = int(maxlumi[index])
            if (min > max):
                print "#################                   ERROR on LUMIBLOCK DEF!!!           ##############################"
                sys.exit(0)
            if (min == max):
                min = -10
                max = 100000000000000000

            lines=os.popen("rfdir "+dir.split(":")[1]).readlines()
            for i in lines:
                tokens=i.split()
                fsize=tokens[4]
                if (int(tokens[4]) >= 502573):
                    if fnmatch.fnmatch(tokens[8],pattern[idir]):
                        fname = dir+"/"+tokens[8]
                        tokens2= tokens[8].split("_")
                        tokens3= tokens2[3].split("lb")
                        tokens3= tokens3[1].split(".")
                        lumi = int (tokens3[0])
                        if ((lumi >= min) & (lumi <= max) ):
                                lumidict[lumi]=lumi
                                #print lumi
                                #stagestat=os.popen('stager_qry -M %s'%fname.strip('castor:')).readlines()[1]
                                #if stagestat.find('STAGED')>=0:
                                #print 'USING STAGED FILE ' + fname.strip('castor:')
                                flist.append(fname)
                                sflist.append((int(fsize),fname))
                                fdict[int(fsize)]=fname
        else:
            if pattern[idir].find("[")>=0:
                print "Run range selected!!!"
                pattern2=pattern[idir].split(",")
                initial=pattern2[0].split("[")
                final=pattern2[1].split("]")
                ii=int(initial[1])
                while ii <= int(final[0]):
                    flist+=glob.glob(dir+"/"+initial[0]+str(ii)+final[1])
                    ii+=1
            else:
                flist=glob.glob(dir+"/"+pattern[idir])
            sflist=[];

    #for ilumi,lumi in enumerate(lumidict):
    #    print ilumi,lumi

    #sflist=fdict.items()
    #sflist.sort()
    #print sflist
    #sys.exit(0)
    
    return flist,sflist
    

def read_settings(args):

#    if len(sys.argv)<2:
#        print "Usage (gui)   : python %s config_file gui" % sys.argv[0]
#        print "Usage (no gui): python %s config_file" % sys.argv[0]
#        sys.exit(-1)

    config={}
    default={}
    optional={}

    if len(args)<2:
        configfile='configfile'
    else:
        if not os.path.isfile(args[1]):
            configfile='configfile'
        else:
            configfile=args[1]

    config["Configfile"]=configfile

    #initial settings
    default["UserMail"]      = 'string'
    optional["UserMail"]     = 'string'
    default["Release"]       = 'string'
    default["ReleaseRec"]    = 'string'
    optional["ReleaseRec"]   = 'string'
    default["Workdir"]       = 'string'
    default["WorkdirRec"]    = 'string'
    optional["barrelToroid"]   = 'bool'
    default["barrelToroid"]   = 'bool'
    optional["endcapToroid"]   = 'bool'
    default["endcapToroid"]   = 'bool'
    optional["Solenoid"]   = 'bool'
    default["Solenoid"]   = 'bool'
    default["MinLumi"]          = 'string'
    optional["MinLumi"]         = 'string'
    default["MaxLumi"]          = 'string'
    optional["MaxLumi"]         = 'string'
    default["Calibdir"]      = 'string'
    default["Batchqueue"]    = 'string'
    default["Inputdir"]      = 'string'
    default["Inputpattern"]  = 'string'
    default["StartConst"]    = 'string'
    default["MinT0"]         = 'string'
    default["MinRT"]         = 'string'
    default["RTtag"]         = 'string'
    default["T0tag"]         = 'string'
    default["DataType"]      = 'string'
    default["DetDescVer"]    = 'string'
    default["GLtag"]         = 'string'
    default["IOVstart"]      = 'int'
    default["IOVend"]        = 'int'
    default["MaxParallel"]   = 'int'
    default["NumIter"]       = 'int'
    default["StartIter"]     = 'int'
    default["IsCosmic"]      = 'bool'
    default["IsCollision"]      = 'bool'
    default["IsHeavyIons"]      = 'bool'
    default["IsCosmicMC"]      = 'bool'
    default["SubmitRecJobs"]    = 'bool'
    default["SubmitMergeJobs"]    = 'bool'
    default["SubmitCalJobs"]    = 'bool'
    default["SubmitCnvJobs"]    = 'bool'
    default["SplitAC"]    = 'bool'
    default["MagnetOn"]    = 'bool'
    default["GetLog"]    = 'bool'
    default["CleanRRoot"]    = 'bool'
    default["UseHist"]       = 'bool'
    default["CleanCRoot"]    = 'bool'
    default["CleanCTxt"]     = 'bool'
    default["CleanCLog"]     = 'bool'
    default["UseChipRef"]    = 'bool'
    default["UsePol0"]       = 'boolarray'
    default["FloatPol3"]     = 'bool'
    default["RtRelation"]    = 'string'
    default["RtBinning"]     = 'string'
    default["DoRt"]          = 'stringarray'
    default["JobPrefix"]     = 'string'
    default["DoT0"]          = 'stringarray'
    default["NumEvents"]     = 'intarray'
    default["MeanT0"]        = 'int'
    default["doEc-2"]        = 'bool'
    default["doEc2"]         = 'bool'
    default["doBA-1"]        = 'bool'
    default["doBA1"]         = 'bool'
    default["OutputLevel"]   = 'string'
    default["T0globalValues"]   = 'string'
    default["DoAthenaCalib"]    = 'bool'
    default["RTglobalValues"]   = 'string'
    default["IsSingleBeam"]     = 'bool' 
    default["Tag"]     = 'string' 
    default["DoShift"]     = 'string' 
    default["T0Offset"]     = 'string' 
    optional["GetConstants"]     = 'bool' 

    default["DoErrorOptimization"]     = 'boolarray'
    optional["DoErrorOptimization"]     = 'boolarray'

    default["DoArXe"]     = 'bool'
    optional["DoArXe"]     = 'bool'


    lines=open(configfile).readlines()

    #config["NumEvents"]=-1
    config["ExcludeRuns"]=[]

    #loop through lines skipping empty and comment lines
    for i in lines:
        j=i.strip()
        if len(j)<1 or j[0]=='#':
            continue
        
        #line must be a setting
        tokens=j.split("=")
        
        #check that it is in the defaults
        type=''
        for s,t in default.iteritems():
            if s==tokens[0].strip():
                if not type=='':
                    print "Same setting more than once: %s" % tokens[0]
                    print "Aborting ..."
                    sys.exit(-1)
                else:
                    type=t
                    if type=="intarray":
                        config[s]=[]
                        dums=tokens[1].strip().split(',')
                        for dum in dums:
                            config[s].append(int(dum))
#                        if len(config[s])==1: config[s]=int(tokens[1].strip())
                    elif type=="boolarray":
                        config[s]=[]
                        dums=tokens[1].strip().split(',')
                        for dum in dums:
                            config[s].append(dum=="True")
                    elif type=="stringarray":
                        config[s]=tokens[1].strip().split(',')
#                        if len(config[s])==1: config[s]=tokens[1].strip()
                    elif type=="int":
                        config[s]=int(tokens[1].strip())
                    elif type=="bool":
                        tempor=tokens[1].strip()
                        if tempor=="False":
                            config[s]=bool(0)
                        elif tempor=="True":
                            config[s]=bool(1)
                        else:
                            print "Bool variables only accepts values 'True' or 'False'"
                    else:
                        config[s]=tokens[1].strip()
        if type=='':
            print "Unknown configuration (are you using the a correct configfile?) : "+j
            print "Aborting ..."
            sys.exit(-1)

    #make sure all settings have been found
    missing=False
    for i in default.iterkeys():
        if (not i in config) & (not i in optional):
            print "Option [%s] is missing in the configuration file! " % i
            missing=True


    if missing:
        print "Not all settings are in the configuration File. Aborting ..."
        sys.exit(-1)
    else:
        print "All settings found. Starting ...\n"

    #read settings from commandline
    config["MakePlots"] = False
    config["MakeLocal"] = False
    config["GetCMTSetup"] = False
    config["ForceGeoTag"] = False
    config["SingleFile"] = False
    config["NoRec"] = False
    config["TempOnCastor"]=True
    config["GetConstants"]=False
    config["TempReadDir"]=''
    for arg in args:
        if arg=='getconst':
            config["GetConstants"] = True
            config["MaxParallel"] = 2
            config["SubmitRecJobs"] = True
            config["SubmitMergeJobs"] = False
            config["SubmitCalJobs"] = False
            config["SubmitCnvJobs"] = False
            config["NumIter"] = 1
            for ijob,nevt in enumerate(config["NumEvents"]):
                config["NumEvents"][ijob]=1
        if arg=='t0shift':
            config["DoShift"] = 2
            config["MaxParallel"] = 20
            config["SubmitRecJobs"] = True
            config["SubmitMergeJobs"] = False
            config["SubmitCalJobs"] = False
            config["SubmitCnvJobs"] = False
            config["NumIter"] = 1
            for ijob,nevt in enumerate(config["NumEvents"]):
                config["NumEvents"][ijob]=1000
        if arg=='makeplots':
            config["MakePlots"] = True
        if arg=='local':
            config["MakeLocal"] = True
        if arg=='cmt':
            config["GetCMTSetup"] = True
        if arg=='single':
            config["SingleFile"] = True
        if arg=='force':
            config["ForceGeoTag"] = True
        if arg=='norec':
            config["NoRec"] = True
        if arg=='afs':
            config["TempOnCastor"] = False
        if arg=='rt':
            config["DoT0"] = ['none']
        if arg=='t0':
            config["DoRt"] = ['none']
        if arg.find("=")>=0:
            cvar = arg.split("=")[0]
            cval = arg.split("=")[1]
            if cvar in default:
                if default[cvar]=='int':
                    config[cvar]=int(cval)
                if default[cvar]=='string':
                    config[cvar]=cval
                if default[cvar]=='bool':
                    if cval=="True" or cval=="true":
                        config[cvar]=True
                    elif cval=="False" or cval=="false":
                        config[cvar]=False
                    else:
                        print "Strange logical value: " + cval
                        sys.exit(-1)
            else:
                if cvar=='X':
                    config["ExcludeRuns"]=cval.split(',')
                if cvar=='temp':
                    config["TempReadDir"]=cval
                if cvar=='N':
                    config["NumIter"]=int(cval)
                if cvar=='Ne':
                    for ijob,nevt in enumerate(config["NumEvents"]):
                        config["NumEvents"][ijob]=int(cval)
                if cvar=='S':
                    config["StartIter"]=int(cval)
                if cvar=='P':
                    config["MaxParallel"]=int(cval)
                if cvar=='Sub':
                    if cval not in ["0000","1000","0100","0010","1100","1110","1111","0111","0011","0001"]:
                        print "Wrong subimssion combination ... use one of 0000, 1000, ,0100, 0010, 1100, 1110, 1111, 0111, 0011, 0001"
                        sys.exit(0);
                    config["SubmitRecJobs"] = bool(int((cval)[0]))
                    config["SubmitMergeJobs"] = bool(int((cval)[1]))
                    config["SubmitCalJobs"] = bool(int((cval)[2]))
                    config["SubmitCnvJobs"] = bool(int((cval)[3]))


    if not ('barrelToroid' in config):
        config["barrelToroid"]   = False
        if config["MagnetOn"]:
            config["barrelToroid"]   = True

    if not ('endcapToroid' in config):
        config["endcapToroid"]   = False
        if config["MagnetOn"]:
            config["endcapToroid"]   = True

    if not ('Solenoid' in config):
        config["Solenoid"]   = False
        if config["MagnetOn"]:
            config["Solenoid"]   = True

    if config["SingleFile"]:
        config["StartIter"]=0
        config["NumIter"]=1

    for iexv,exv in enumerate(config["ExcludeRuns"]):
        config["ExcludeRuns"][iexv]=int(exv)
                    
    config["SubPat"]=str(int(config["SubmitRecJobs"]))+str(int(config["SubmitMergeJobs"]))+str(int(config["SubmitCalJobs"]))+str(int(config["SubmitCnvJobs"]))
    if config["SubPat"] in ["1000","1100","1110","0111","0011","0001"]:
        config["NumIter"]=1

    if config["MakePlots"]:
        #config["NumIter"] = 1
        if not config["SubPat"]=="0000":
            config["SubPat"] = "0001"

    if config["JobPrefix"] == "":
        config["JobPrefix"] = chr(int(random.random()*25+65)) +  chr(int(random.random()*25+65)) + chr(int(random.random()*25+65))

    if config["Tag"] == "":
        config["Tag"]=strftime("%d-%b-%Y-%H.%M.%S", gmtime())

    if not "WorkdirRec" in config:
        config["WorkdirRec"]=config["Workdir"]
    else:
        if config["WorkdirRec"] == "":
            config["WorkdirRec"]=config["Workdir"]

    if not "ReleaseRec" in config:
        config["ReleaseRec"]=config["Release"]
    else:
        if config["ReleaseRec"] == "":
            config["ReleaseRec"]=config["Release"]

    if not "UserMail" in config:
        config["UserMail"]=(os.popen("echo $LOGNAME").readlines()[0]).split('\n')[0]+'@cern.ch'
    else:
        if config["UserMail"] == "":
            config["UserMail"]=(os.popen("echo $LOGNAME").readlines()[0]).split('\n')[0]+'@cern.ch'

    if not "MinLumi" in config:
        print "###   Lower LumiBlock not defined in ConfigFile, going to use 0 by default"
        config["MinLumi"] = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0"

    if not "MaxLumi" in config:
        print "###   Upper LumiBlock not defined in ConfigFile, going to use 0 by default"
        config["MaxLumi"] = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0"

    if not "DoErrorOptimization" in config:
        print "###   DoErrorOptimization was not found in the ConfigFile. Will be set to FALSE"
        config["DoErrorOptimization"] = [False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False]

    if not "DoArXe" in config:
        print "###   Setup for Xe/Ar was not found in the ConfigFile. Will be set to FALSE"
        config["DoArXe"] = False

    config["MultiInputdir"]=config["Inputdir"].split(',')
    config["MultiInputpattern"] = config["Inputpattern"].split(',')

    config["MultiMinLumi"]=config["MinLumi"].split(',')
    config["MultiMaxLumi"]=config["MaxLumi"].split(',')

    config["MultiDataset"]=[]
    for dir in config["MultiInputdir"]:
        config["MultiDataset"].append(dir.split("/")[-1])

    config["Batchdir"]=os.path.abspath('.') + '/batch'

    lines=[]
    cfile=open("lastconfigfile","w")
    lines.append("# Configurations for the last run\n\n")
    for k in config.iterkeys():
        if k in default:        
            lines.append("%-20s = %-20s\n" % (k,config[k]))
    for k in config.iterkeys():
        if k not in default:        
            lines.append("# %-20s = %-20s\n" % (k,config[k]))
    cfile.writelines(lines)
    cfile.close()

    return config


def create_calsettings(config,dort,dot0):
    flt=""
    brd=""
    #if config["UsePol0"]:
    #    flt+="0"
    if config["FloatPol3"]:
        flt+="3"
    if config["UseChipRef"]:
        brd+="B"

    trtsettings={} #settings for 1 job calibrating on TRT level
    trtsettings['CalibrateT0']="['TRT']"
    trtsettings['PrintT0Out']="['TRT']"
    trtsettings['CalibrateRt']="['TRT']"
    trtsettings['PrintRtOut']="['TRT']"    
    trtsettings['PrintLog']="['TRT']"
    trtsettings['NoHistograms']=""
    trtsettings['UseBoardRef']=""
    trtsettings['SubPart']="'user'"
    trtsettings['SplitBarrel']="True"

    detsettings={} #settings for 4 jobs calibrating on detector level
    detsettings['CalibrateT0']="['TRT','Detector']"
    detsettings['PrintT0Out']=""
    detsettings['CalibrateRt']="['TRT','Detector']"
    detsettings['PrintRtOut']=""    
    detsettings['NoHistograms']="['TRT']"
    detsettings['UseBoardRef']=""
    detsettings['PrintLog']="['TRT','Detector']"
    detsettings['SubPart']="'user'"
    detsettings['SplitBarrel']="True"

    laysettings={} #settings for 8 jobs calibrating on layer level
    laysettings['PrintT0Out']="['Layer','Module','Board','Chip','Straw']"
    laysettings['PrintRtOut']="['Layer']"
    laysettings['NoHistograms']="['TRT','Detector','Chip','Straw']"
    laysettings['UseBoardRef']=""
    laysettings['PrintLog']="['TRT','Detector','Layer','Module','Board','Chip']"
    laysettings['SubPart']="'user'"
    laysettings['SplitBarrel']="True"

    barsettings={} #settings for 1 job calibrating the whole barrel
    barsettings['CalibrateT0']="['TRT','Detector','Layer']"
    barsettings['PrintT0Out']="['Layer']"
    barsettings['PrintRtOut']="['Layer']"
    barsettings['NoHistograms']="['TRT']"
    barsettings['UseBoardRef']=""
    barsettings['PrintLog']="['TRT','Detector','Layer']"
    barsettings['SubPart']="'user'"
    barsettings['SplitBarrel']="False"


    # if no T0 calibration
    if dot0=='none':
        trtsettings['CalibrateT0']=""
        detsettings['CalibrateT0']=""
        detsettings['PrintT0Out']="['Detector']"
        laysettings['CalibrateT0']=""
        barsettings['CalibrateT0']=""

    #if TO calibration on board level    
    elif dot0=='board':
        laysettings['CalibrateT0']="['TRT','Detector','Layer','Module','Board']"

    #if TO calibration on chip level    
    elif dot0=='chip':
        laysettings['CalibrateT0']="['TRT','Detector','Layer','Module','Board','Chip']"
 
    #if TO calibration on straw level    
    elif dot0=='straw':
        laysettings['CalibrateT0']="['TRT','Detector','Layer','Module','Board','Chip','Straw']"
        laysettings['NoHistograms']="['TRT','Detector']"
 
    #if TO calibration on with chip finedelays    
    elif dot0=='chipref':
        laysettings['CalibrateT0']="['TRT','Detector','Layer','Module','Board','Chip']"
        laysettings['UseBoardRef']="['Chip']"


    if dort=='none':
        trtsettings['CalibrateRt']=""
        detsettings['CalibrateRt']=""
        laysettings['CalibrateRt']=""
        barsettings['CalibrateRt']=""
    
    elif dort=='trt':
        detsettings['CalibrateRt']=""
        laysettings['CalibrateRt']=""
        barsettings['CalibrateRt']=""

    elif dort=='detector':
        laysettings['CalibrateRt']="['TRT','Detector']"
        barsettings['CalibrateRt']="['TRT','Detector']"

    elif dort=='layer':
        laysettings['CalibrateRt']="['TRT','Detector','Layer']"
        barsettings['CalibrateRt']="['TRT','Detector','Layer']"

    elif dort=='module':
        laysettings['CalibrateRt']="['TRT','Detector','Layer','Module']"
        barsettings['CalibrateRt']="['TRT','Detector']"
        laysettings['PrintRtOut']="['Layer','Module']"
   
    calsetting={}
    calsetting["_all"]    ={"sel":"_*_-_-_-_-_-_-" ,"opt":"_RTPF" + flt + "_N_N_N_N_N_N","user":trtsettings}
    calsetting["_barrel"] ={"sel":"_*_1_*_-_-_-_-","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_N"+ flt + "_N"+ flt + "_N" + brd + flt + "_N","user":barsettings}
    calsetting["_-1"]     ={"sel":"_*_-1_-_-_-_-_-","opt":"_RTPQ"+ flt + "_RTPF"+ flt + "_N_N_N_N_N","user":detsettings}
    calsetting["_1"]      ={"sel":"_*_1_-_-_-_-_-" ,"opt":"_RTPQ"+ flt + "_RTPF"+ flt + "_N_N_N_N_N","user":detsettings}
    calsetting["_-1_0"]   ={"sel":"_*_-1_0_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_-1_1"]   ={"sel":"_*_-1_1_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_-1_2"]   ={"sel":"_*_-1_2_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_1_0"]    ={"sel":"_*_1_0_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_1_1"]    ={"sel":"_*_1_1_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_1_2"]    ={"sel":"_*_1_2_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_2"]      ={"sel":"_*_2_-_-_-_-_-","opt":"_RTPQ"+ flt + "_RTPF"+ flt + "_N_N_N_N_N","user":detsettings}
    calsetting["_-2"]     ={"sel":"_*_-2_-_-_-_-_-","opt":"_RTPQ"+ flt + "_RTPF"+ flt + "_N_N_N_N_N","user":detsettings}
    calsetting["_-2_a"]   ={"sel":"_*_-2_0,1,2,3_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_2_a"]    ={"sel":"_*_2_0,1,2,3_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_-2_b"]   ={"sel":"_*_-2_4,5,6,7_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_2_b"]    ={"sel":"_*_2_4,5,6,7_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_-2_c"]   ={"sel":"_*_-2_8,9,10,11_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_2_c"]    ={"sel":"_*_2_8,9,10,11_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_-2_d"]   ={"sel":"_*_-2_12,13_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}
    calsetting["_2_d"]    ={"sel":"_*_2_12,13_*_*_*_*","opt":"_RTPQ"+ flt + "_RTPQ"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF"+ flt + "_RTPF" + brd + flt + "_FQ","user":laysettings}

    calsetting["error_Barrel"]    =["error_barrel"]
    calsetting["error_Endcap"]    =["error_endcap"]

    return calsetting



def make_itlog(config):
    "test"
    

###############################################
############## MAIN ROUTINE ###################
###############################################

config=read_settings(sys.argv)

batchdir=config["Batchdir"]    
batchq=config["Batchqueue"]
inputdir=config["MultiInputdir"]
inputpattern=config["MultiInputpattern"]
rttag=config["RTtag"]
t0tag=config["T0tag"]
IOV=[config["IOVstart"],0,config["IOVend"],4294967295] 
maxjobs=config["MaxParallel"]
numiterations=config["NumIter"]
firstiteration=config["StartIter"]
startconstants=config["StartConst"]

minlumi=config["MultiMinLumi"]
maxlumi=config["MultiMaxLumi"]

calibcmd=config["Calibdir"] + "/CalibrateTRT"
oldcalibcmd=config["Calibdir"] + "/TRTCalibration.py"
mergecmd=config["Calibdir"] + "/Merge _-_-_-_-_-_*"
mergetxtcmd=config["Calibdir"] + "/MergeCalibOutput"
outdir= batchdir + "/output"
constdir= batchdir + "/input"
calibprefix= batchdir + "/input/calibout"

jobprefix=config["JobPrefix"]

#create a uniqe calibid (timestamp)
calibid=''
if not os.path.isdir(batchdir):
    calibid=strftime("%Y%m%d%H%M%S", gmtime())
else:
    calibid=os.popen('ls -1 %s/id_*'%batchdir).read().strip().split('id_')[-1]

#add some configuration flags
config["TempDir"]=batchdir + "/temp"
if config["TempOnCastor"]:
    config["TempDir"]='/eos/atlas/user/' + os.environ['USER'][0] + '/' + os.environ['USER'] + '/tempstore/temp_' + calibid 
config["SetupCmdRec"]="source /afs/cern.ch/project/gd/apps/atlas/slc3/local/setup.sh\nexport AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup\nalias asetup='source $AtlasSetup/scripts/asetup.sh'\nasetup " + config["ReleaseRec"] + " --testarea=" + config["WorkdirRec"] 
config["SetupCmdCal"]="source /afs/cern.ch/project/gd/apps/atlas/slc3/local/setup.sh\nexport AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup\nalias asetup='source $AtlasSetup/scripts/asetup.sh'\nasetup " + config["Release"] + " --testarea=" + config["Workdir"] 
config["CalibCmd"]=calibcmd
config["MergeCmd"]=mergecmd
config["MergeTxtCmd"]=mergetxtcmd
config["ConstDir"]=constdir
config["OutDir"]=outdir
config["CalPrefix"]=calibprefix
config["Localdir"]=os.path.abspath('.')

#just print the cmt setup command
if config["GetCMTSetup"]:
    print config["SetupCmdRec"]
    sys.exit(0)

#create a new batch directory if it doesn't already exist
if not os.path.isdir(batchdir):
    print "Batch directory = %s ...\n" % config["Batchdir"]
    os.system("mkdir -v " + batchdir)
    os.system("mkdir -v " + batchdir + "/input")
    os.system("mkdir -v " + batchdir + "/output")
    os.system("mkdir -v " + batchdir + "/temp")
    idfile=open(batchdir + '/id_' + calibid,'w')
    idfile.write(config["TempDir"])
    idfile.close()
    if config["TempOnCastor"]:
        os.system('export STAGE_SVCCLASS=atlcal; rfmkdir %s'%config["TempDir"])
    if config["SubPat"] not in ["0000"]: 
        print "\nCopying files for reference ...\n"
        os.system("cp -fv " + sys.argv[0] + " " + batchdir) # copy files for using as reference if any jobs are submitted 
        os.system("cp -fv %s %s/configfile" %("lastconfigfile",batchdir))
else:
#else run on files in the the provided one
    if config["SubPat"] not in ["0000"]:
        print "Using old batch directory %s" %  batchdir + ". Copying files for reference ...\n"
        os.system("cp -fv " + sys.argv[0] + " " + batchdir) # copy files for using as reference 
        os.system("cp -fv %s %s/configfile" %("lastconfigfile",batchdir))

if config["TempReadDir"]=='':
    config["TempReadDir"]=config["TempDir"]
else:
    config["TempReadDir"]='/eos/atlas/user/' + os.environ['USER'][0] + '/' + os.environ['USER'] + '/tempstore/temp_' + config["TempReadDir"]

if ((config["IsCollision"]==1) & (config["IsCosmic"]==1)):
    print "Collisions and IsCosmic both set to true!!!!!!"
    sys.exit(0)

if ((config["IsCollision"]==1) & (config["IsHeavyIons"]==1)):
    print "Collisions and HEAVI IONS can not be selected at same time!!!!!!!!!"
    sys.exit(0)

if ((config["IsHeavyIons"]==1) & (config["IsSingleBeam"]==1)):
    print "Collisions and IsSingleBeam both set to true!!!!!!"
    sys.exit(0)

if ((config["IsSingleBeam"]==1) & (config["IsCosmic"]==1)):
    print "IsCosmic and IsSingleBeam both set to true!!!!!!"
    sys.exit(0)

if ((config["IsCosmicMC"]==1) & (config["IsCosmic"]==1)):
    print "IsCosmic and IsCosmicMC both set to true!!!!!!"
    sys.exit(0)

if ((config["IsCosmicMC"]==1) & (config["IsSingleBeam"]==1)):
    print "IsCosmicMC and IsSingleBeam both set to true!!!!!!"
    sys.exit(0)

# dump joboptions to a file
jofile=open("%s/joboptions" % batchdir,"w")
if config["IsHeavyIons"]:
	if config["DataType"]=="ESD":
		from HITemplate import collision
		jofile.write(collision(config,["file1","file2"],"calibconstants",-1))
	if config["DataType"]=="RAW":
		from RAWHITemplate import collision
		jofile.write(collision(config,["file1","file2"],"calibconstants",-1))
else:
	jofile.write(collision(config,["file1","file2"],"calibconstants",-1))

jofile.close()

flist,sflist=build_filelist(inputdir,inputpattern,minlumi,maxlumi)
if config["MakeLocal"]: # just make a local WORKDIR
    locflist=[]
    locflist.append(flist[0])
    run_local(config,startconstants,locflist)        
    sys.exit(0)

if (len(flist)==0):
    print "#####################################################################################"
    print "## "
    print "##       error!"
    print "##       FILES NOT FOUND UNDER: " , inputdir
    print "##       error!"
    print "## "
    print "#####################################################################################"
    sys.exit()

if not config["ForceGeoTag"]:
    if ((config["IsCollision"]==1) | (config["IsCosmic"]==1)):     
        extractDDV2(config,sflist)

waitlist=[]
print "Making scripts and submitting jobs ...\n"
for i in range(firstiteration,firstiteration+numiterations):
    config["Iiter"]="%i"%i
    waitlist=run_iter(config,sflist,i,waitlist,startconstants)

print
print 'CALIBRATION ID: ', calibid 
print
print config["DoRt"]
print config["DoT0"]
print config["NumEvents"]
print config["UsePol0"]

