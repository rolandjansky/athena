##Script to run Reco_tf.py on the lxplus batch system
##author:PF
##pierfrancesco.butti@cern.ch
######################################################
 
import sys,os
from datetime import datetime
import glob


def OptParsing():
    from optparse import OptionParser
    parser=OptionParser()
    Now=datetime.now()
    parser.add_option("--preInclude",dest="preInclude",help="preInclude options",default="")
    parser.add_option("--preExec",dest="preExec",help="preExec options",default="")
    parser.add_option("--inFilesDir",dest="inFilesDir",help="Directory Containing the inputFiles",default="")
    parser.add_option("--inFilesList",dest="inFilesList",help="List of files",default="")
    parser.add_option("--postInclude",dest="postInclude",help="postInclude options",default="")
    parser.add_option("--postExec",dest="postExec",help="postExec options",default="")
    parser.add_option("--inType",dest="inType",help="Input File Type:RDO,BS,ESD,AOD",default="BS")
    parser.add_option("--outType",dest="outType",help="Output File Type: ESD,AOD", default="ESD")
    parser.add_option("--amiTag",dest="amiTag",help="Parse the options from a particular amiTAG",default="")
    parser.add_option("--OptFile",dest="OptFile",help="Parse options from a File",default="")
    parser.add_option("--queue",dest="queue",help="Queue where to send the jobs (1nd,1nh,8nh,1nw,2nw,8nm,atlasb1)",default="8nh")
    parser.add_option("--sampleType",dest="sampleType",help="sampleType:data, mc.",default="data")
    parser.add_option("--conditionsTag",dest="conditionsTag",help="Conditions Tag. Overrides the default one",default="") #MC: OFLCOND-RUN12-SDR-14
    parser.add_option("--Rel",dest="Rel",help="Which athena Release",default="19.3.0.1")
    parser.add_option("--debug",dest="debug",help="debug",type='int',default=0)
    parser.add_option("--nFilesPerJob",dest="nFilesPerJob",help="nFilesPerJob",default=1,type="int")
    parser.add_option("--OutName",dest="OutName",help="Name to give to the jobs",default="JobTime_"+Now.strftime('%Y_%m_%d_%H_%M_%S'))
    (config,sys.argv[1:]) = parser.parse_args(sys.argv[1:])
    return config

def main():
    config = OptParsing()
    sampleType=config.sampleType
    jobname = config.OutName
    inputFileList = config.inFilesDir
    inputFilesFromList = config.inFilesList
    nFilesPerJob = config.nFilesPerJob
    postExec=config.postExec
    inType=config.inType
    outType=config.outType
    conditionsTag="CONDBR2-ES1PA-2014-01"
    inputType=""
    outputType=""
    debug=config.debug
    triggerFlags="from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags;HLTMonFlags.doBphys=False;"

    
    if sampleType=="mc":
        triggerFlags=""
        conditionsTag="OFLCOND-RUN12-SDR-14"
        

    if config.conditionsTag!="":
        conditionsTag=config.conditionsTag
    

    if conditionsTag=="OFLCOND-RUN12-SDR-14":
        postExec+="'conddb.addOverride(\"/PIXEL/PixdEdx\",\"PixdEdx_January2011_mc-000-00\")'"


    if inType=="RDO":
        inputType="--inputRDOFile="
    elif inType=="BS":
        inputType="--inputBSFile="
    elif inType=="ESD":
        inputType="--inputESDFile="
    else:
        print "WARNING: input type not known: ",inType


    if outType=="ESD":
        outputType="--outputESDFile="
    elif outType=="AOD":
        outputType="--outputAODFile="
    else:
        print "WARNING:output type not known: ", outType


    if debug:
        print "inType="+inType+" triggerFlags="+triggerFlags+" postExec="+postExec
    
    if inputFileList != "":
        print "From Dir"
        if not inputFileList.endswith("/"):
            inputFileList=inputFileList+"/"
        
        inputFileList = glob.glob(inputFileList+"data*")
    
    if inputFilesFromList !="":
        inputFileList=[]
        List=open(inputFilesFromList,'r')
        for line in List:
            inputFileList.append(line.strip())

            
    
            
    
    nJobs = int(len(inputFileList))/nFilesPerJob
    if int(len(inputFileList)) % nFilesPerJob != 0:
        nJobs+=1
        
    print "Total Input Files number", len(inputFileList)
    print "Sending a total of ",nJobs, "jobs"
    
    PWD=os.environ["PWD"]+"/"

    if not os.path.exists("batchScripts"):
        os.mkdir("batchScripts")
    
    if not os.path.exists("batchLogs"):
        os.mkdir("batchLogs")

    for job in xrange(nJobs):
        script = open("batchScripts/batch_script_"+str(job)+"_"+jobname+".lsf","w")
        script.write("#BSUB -J RecoTf_job"+str(job)+"_M6_"+jobname+"\n")
        script.write("#BSUB -o "+PWD+"batchLogs/RecoTf_job"+str(job)+"_M6_"+jobname+".log\n")
        script.write("#BSUB -e "+PWD+"batchLogs/RecoTf_job"+str(job)+"_M6_"+jobname+".err\n")
        script.write("#BSUB -q "+config.queue+"\n\n\n")
        script.write("#     setup the environment\n")
        script.write("source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh "+config.Rel+" --testarea="+PWD+" --single\n")
        script.write("mkdir "+PWD+"M6_jobN_"+str(job)+"_"+jobname+"\n")
        script.write("cd "+PWD+"M6_jobN_"+str(job)+"_"+jobname+"\n")
        
        InputFiles=""
        
    
        if len(inputFileList)-job*nFilesPerJob >= nFilesPerJob:
            #print len(inputFileList)-job*nFilesPerJob
            for iFile in xrange(nFilesPerJob):
                InputFiles+=inputFileList[job*nFilesPerJob+iFile]+","
        
        else:
                for iFile in xrange(len(inputFileList)-job*nFilesPerJob):
                    InputFiles+=inputFileList[job*nFilesPerJob+iFile]+","
                    
        
        InputFiles=InputFiles.strip(",")
        if debug:
            print InputFiles
        
        #Setting Up and running the command
        #cmd = "Reco_tf.py --conditionsTag='CONDBR2-ES1PA-2014-01' --beamType='cosmics' --ignoreErrors=True --autoConfiguration='everything' --maxEvents=-1  --postExec='ServiceMgr.InDetSCT_ConditionsSummarySvc.ConditionsServices=[\"InDetSCT_ConfigurationConditionsSvc\",\"InDetSCT_FlaggedConditionSvc\",\"InDetSCT_ByteStreamErrorsSvc\",\"InDetSCT_ReadCalibDataSvc\"]' --preExec='rec.doJetMissingETTag=False;rec.doTau=False;from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArHVCorr=False;jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False);from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags;HLTMonFlags.doBphys=False;DQMonFlags.doJetMon=False;DQMonFlags.doMissingEtMon=False;DQMonFlags.enableLumiAccess=False;InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;larCondFlags.OFCShapeFolder.set_Value_and_Lock(\"\")' --geometryVersion='ATLAS-R2-2015-01-01-00' --inputBSFile="+InputFiles+" --outputESDFile=myESD.pool.root --postInclude='"+PWD+"InDetMonitoringAlignment.py'"
        #

        
            
        #cmd = "Reco_tf.py --conditionsTag='"+conditionsTag+"' --beamType='cosmics' --ignoreErrors=True --autoConfiguration='everything' --maxEvents=-1  --postExec="+postExec+" --preExec='rec.doJetMissingETTag=False;rec.doTau=False;from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArHVCorr=False;jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False);InDetFlags.doMonitoringAlignment.set_Value_and_Lock(True);"+triggerFlags+"DQMonFlags.doJetMon=False;DQMonFlags.doMissingEtMon=False;DQMonFlags.enableLumiAccess=False;InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);InDetFlags.useBroadClusterErrors.set_Value_and_Lock(True);InDetFlags.doTrackSegmentsPixel.set_Value_and_Lock(False);InDetFlags.doTrackSegmentsSCT.set_Value_and_Lock(False);InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock(False); DQMonFlags.doStreamAwareMon=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;larCondFlags.OFCShapeFolder.set_Value_and_Lock(\"\");' --geometryVersion='ATLAS-R2-2015-02-01-00' "+inputType+InputFiles+" "+outputType+"my"+outType+".pool.root --postInclude='"+PWD+"LayerMaterialOverrider.py','"+PWD+"InDetMonitoringAlignment.py'"

        #19.3 Command
        
        cmd = "Reco_tf.py --beamType='cosmics' --ignoreErrors=True --autoConfiguration='everything' --maxEvents=-1 --conditionsTag='"+conditionsTag+"' --geometryVersion=ATLAS-R2-2015-02-00-00 --preExec='rec.doJetMissingETTag=False;from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArHVCorr=False;jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags;HLTMonFlags.doBphys=False;DQMonFlags.enableLumiAccess=False;InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;larCondFlags.OFCShapeFolder.set_Value_and_Lock(\"\");InDetFlags.ForceCoraCool=True;' --postExec='from IOVDbSvc.CondDB import conddb;conddb.addOverride(\"/GLOBAL/TrackingGeo/LayerMaterialV2\",\"AtlasLayerMat_v18_ATLAS-R2-2015\");' "+inputType+InputFiles+" "+outputType+"my"+outType+".pool.root --postInclude='"+PWD+"ErrorScaling.py','"+PWD+"InDetMonitoringAlignment.py'"

        script.write(cmd+"\n")
        #script.write("rm *ESD*root\n")
        script.write("cd -\n")
        script.close()
        
        os.system("chmod +x batchScripts/batch_script_"+str(job)+"_"+jobname+".lsf")
        os.system("bsub <batchScripts/batch_script_"+str(job)+"_"+jobname+".lsf")
    

if __name__=="__main__":
    main()
