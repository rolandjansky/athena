import os
from AthenaCommon.Logging import logging
logg = logging.getLogger('HITARMaker')
logg.info( '****************** STARTING HITARMaker *****************' )

logg.info( '**** Transformation run arguments' )
logg.info( str(runArgs) )


if hasattr( runArgs, 'outputTAR_CONFIGFile'):
    logg.info("making outputTAR_CONFIGFile " + runArgs.outputTAR_CONFIGFile)
else:
    logg.error("no outputTAR_CONFIGFile ??")

if hasattr( runArgs, 'InputLbnMapFile'):
    logg.info("using InputLbnMapFile " + runArgs.InputLbnMapFile)
else:
    logg.error("no InputLbnMapFile ??")

if hasattr( runArgs, 'HIfiltervtxdirectory'):
    logg.info("using HIfiltervtxdirectory " + runArgs.HIfiltervtxdirectory)
else:
    logg.error("no HIfiltervtxdirectory ??")

#Don't want to run over any events...
theApp.EvtMax = 5

#Make the filter/vtx.data.txt files
with open(runArgs.InputLbnMapFile) as f:
    for line in f:
        if line=='': break
        filenm=line.rstrip()
        printfunc (filenm)
        finfo=filenm.split(" ")
        run=finfo[3].replace(",","")
        lbn=finfo[5].replace(",","").zfill(4) #pad 44->0044
        printfunc (run+"_"+lbn)
        
        #copy the corresponding filter/vtx_run_lbn.data.txt files from EOS to here
        #runArgs.HIfiltervtxdirectory=/eos/atlas/atlascerngroupdisk/phys-hi/DataOverlay2015
        retval=os.system("xrdcp root://eosatlas.cern.ch/"+runArgs.HIfiltervtxdirectory+"/Run"+run+"/HI_"+run+"_"+lbn+"_filter_v1.txt .")
        if retval!=0: logg.error("failed to xrdcp root://eosatlas.cern.ch/"+runArgs.HIfiltervtxdirectory+"/Run"+run+"/HI_"+run+"_"+lbn+"_filter_v1.txt .")
        else: logg.info("succeeded xrdcp root://eosatlas.cern.ch/"+runArgs.HIfiltervtxdirectory+"/Run"+run+"/HI_"+run+"_"+lbn+"_filter_v1.txt .")
        retval=os.system("xrdcp root://eosatlas.cern.ch/"+runArgs.HIfiltervtxdirectory+"/Run"+run+"/HI_"+run+"_"+lbn+"_vtx_v1.txt .")
        if retval!=0: logg.error("failed to xrdcp root://eosatlas.cern.ch/"+runArgs.HIfiltervtxdirectory+"/Run"+run+"/HI_"+run+"_"+lbn+"_vtx_v1.txt .")
        else: logg.info("succeeded xrdcp root://eosatlas.cern.ch/"+runArgs.HIfiltervtxdirectory+"/Run"+run+"/HI_"+run+"_"+lbn+"_vtx_v1.txt .")

        #then append them to filter/vtx.data.txt
        retval=os.system("cat HI_"+run+"_"+lbn+"_filter_v1.txt >> filter.data.txt")
        if retval!=0: logg.error("failed to cat HI_"+run+"_"+lbn+"_filter_v1.txt >> filter.data.txt")
        else: logg.info("added to filter.data.txt")
        retval=os.system("cat HI_"+run+"_"+lbn+"_vtx_v1.txt >> vtx.data.txt")
        if retval!=0: logg.error("failed to cat HI_"+run+"_"+lbn+"_vtx_v1.txt >> vtx.data.txt")
        else: logg.info("added to vtx.data.txt")

#Make the TAR file
import tarfile
f=tarfile.open(name=runArgs.outputTAR_CONFIGFile, mode='w:gz')
f.add("filter.data.txt")
f.add("vtx.data.txt")
f.close()

# ---------------------------
# Post-include/exec
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)
if hasattr(runArgs, "postExec") and runArgs.postExec != 'NONE':
    for cmd in runArgs.postExec:
        exec(cmd)

