# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import glob
import sys
import commands
from socket import gethostname

class Files:
    """ Class used to store information for files. """
    
    def __init__(self,type,format,filepath,scpHost,geometry,
                 conditionsTag="",nfiles=1,datasetdef=""):

        self.type          = type   # file or dataset
        self.format        = format # RAW, RDO, ESD
        self.inputFilePath = filepath
        self.scpHost       = scpHost
        self.geometry      = geometry
        self.conditionsTag = ""
        self.nfiles        = nfiles
        self.datasetdef    = datasetdef
        self.castor        = False

        self.files               = []
        self.filesWithoutPath    = []

        for inputPath in self.inputFilePath:
            if inputPath.find('/castor/')>-1 :
                
                # always try ESD first (?)
                ret, out = commands.getstatusoutput("nsls "+inputPath+" | grep ESD")
                if len(out.split('\n'))==0:
                    ret, out = commands.getstatusoutput("nsls "+inputPath+" | grep root")
                filenames = out.split('\n')
                newfilenames=[]
                print "filenames: ",filenames
                for file in filenames:
                    print "file: ",file
                    newfile=inputPath+"/"+file
                    newfilenames.append(newfile)
                    print "newfile: ",newfile                
                self.castor=True
                filenames=newfilenames
            elif scpHost=='':
                #filenames = os.listdir(inputPath)
                #filenames = glob.glob(inputPath+"/*root*")
                filenames = glob.glob(inputPath+"/*")
            else:
                filenames = []
                import subprocess                
                sshCmd="ssh "+scpHost+" ls "+inputPath
                process = subprocess.Popen(sshCmd, shell=True, stdout=subprocess.PIPE)
                output = process.communicate()
                status=process.poll()

                outputstring = output[0]
                if outputstring == '':
                    print "problem with filelist"
                    sys.exit(3)
                while outputstring.partition('\n')[1] != '':
                    outputfile = inputPath+"/"+outputstring.partition('\n')[0]
                    filenames.append(outputfile)
                    outputstring = outputstring.partition('\n')[2]

            filenames.sort()

            self.files               += filenames
            filenamesWithoutPath = []
            for file in filenames :
                if file.find('/')>-1:
                    filenamesWithoutPath.append(file.rsplit("/",1)[1])
                else:
                    filenamesWithoutPath.append(file)
            self.filesWithoutPath    += filenamesWithoutPath

        if len(self.files)<1:
            print "no files found in inputPath: ",self.inputFilePath
            sys.exit(34)

    def scpString(self,iCPU=-1,nFiles=-1):

        if self.scpHost=="" :
            return ""

        print "nFiles: ",nFiles
        if nFiles>-1 and iCPU>-1:
            first = iCPU*nFiles
            last = first+nFiles
            files = self.files[first:last]
        else:
            files = self.files[:]
        print "files: ",files

        filestring = str(files).replace("[","").replace("]","").replace("'","").replace(",","\n")
        print "filestring: ",filestring

        host=gethostname().strip()
        print "...host...",host,"...scpHost...",self.scpHost,"..."
        if (host != self.scpHost) :
            outputfilefull=str(filestring.partition('\n')[0]).lstrip()
            file=outputfilefull.rsplit("/",1)[1]
            val="if [ -e /tmp/roberth/"+file+" ]; then echo \"have "+file+" already\"; else scp "+self.scpHost+":"+outputfilefull+" /tmp/roberth/.;fi\n"
            filestring = filestring.partition('\n')[2]
            
            while filestring.partition('\n')[1] != '':
                outputfilefull = str(filestring.partition('\n')[0]).lstrip()
                file=outputfilefull.rsplit("/",1)[1]
                val = val + "if [ -e /tmp/roberth/"+file+" ]; then echo \"have "+file+" already\"; else scp "+self.scpHost+":"+outputfilefull+" /tmp/roberth/.;fi\n"
                filestring = filestring.partition('\n')[2]                    
        else:
            val=""
            while filestring.partition('\n')[1] != '':
                outputfile = str(filestring.partition('\n')[0]).lstrip()
                val = val + "ln -s "+outputfile+" .\n"
                filestring = filestring.partition('\n')[2]                    
            if filestring!="":
                val = val + "ln -s "+str(filestring.partition('\n')[0]).lstrip()+" .\n"
        return val

    def writeFile(self,iterator,iCPU=-1,path="."):

        print "in writeFile"

        if self.scpHost=="":
            tmpFileList = self.files
        else:
            tmpFileList = self.filesWithoutPath

        print "tmpFileList: ",tmpFileList
        nFiles=iterator.nFilesPerJob
        print "nFiles ",nFiles," nFilesPerJob ",iCPU
        if nFiles>-1 and iCPU>-1:
            first = iCPU*nFiles
            last = first+nFiles
            print "first: ",first,", last: ",last
            files = tmpFileList[first:last]
            print "files: ",files
        elif nFiles>-1 and iCPU==-1 :
            files = tmpFileList[0:nFiles]
        else:
            files = tmpFileList[:]

        if len(files) == 0:
            return ""

        print "scpHost: ",self.scpHost
        print "gethostname: ",gethostname().strip()
        if self.scpHost!="" and gethostname().strip()!=self.scpHost:
            print "files: ",files
            newfiles = []
            for file in files:
                newfiles.append("/tmp/roberth/"+file)
            filestring = str(newfiles).replace(",",",\n")
        else:
            filestring = str(files).replace(",",",\n")

        print "filestring: ",filestring

        if self.castor:
            filestring = filestring.replace("/castor","rfio:/castor")

        filepy=path+"/inputfiles"
        if iterator.rootName:
            filepy += "_" + iterator.rootName
        if iCPU>-1 and nFiles>-1:
            filepy += "_index"+str(iCPU)
        filepy += ".py"
        
        filesOut = open(filepy,"w")        
        filesOut.write("inputFile = "+filestring+"\n");
        filesOut.close()

        return filepy
        
class DefaultFiles (Files):
    """ Class used to handle flags associated with input files for a number of default jobs. """

    @classmethod
    def manualFile(Files,inputFilePath,scpHost,format,geometry="",conditionsTag="",nfiles=1,
                   datasetdef=""):
        obj = Files("file",format,inputFilePath,scpHost,geometry,conditionsTag,nfiles,
                    datasetdef)
        return obj
    
    @classmethod
    def manualDataset(Files,datasetdef,format,geometry="",conditionsTag="",nfiles=1):        
        scpHost=""
        if format=="RAW":
            inputFilePath=["path/to/raw"]
        elif format=="RDO":
            inputFilePath=["/home/roberth/samples/ECID_20GeV_noCSC"]
        elif format=="ESD":
            inputFilePath=["path/to/esd"]
        else:
            print "bad file format!  must be RAW, RDO, or ESD!"            
        obj = Files("dataset",format,inputFilePath,scpHost,geometry,conditionsTag,nfiles,
                    datasetdef)
        return obj

    @classmethod
    def default(Files,name,type):
        conditionsTag=""
        datasetdef=""        
        if name=='L1_MC_EC_noCSC':
            inputFilePath = ["/home/roberth/samples/ECID_20GeV_noCSC"]
            scpHost       = "terrier09"
            geometry      = 'ATLAS-GEO-03-00-00'
            fileType      = 'RDO'
            nfiles        = 31
            if type=="dataset":
                print "no dataset definition for this sample"
        elif name=='L1_MC_EC_noCSC_bnl':
            inputFilePath = ["/usatlas/u/roberth/scratch0/samples/ECID_20GeV_noCSC"]
            scpHost       = ""
            geometry      = 'ATLAS-GEO-03-00-00'
            fileType      = 'RDO'
            nfiles        = 31
            if type=="dataset":
                print "no dataset definition for this sample"
        elif name=='g4digi':
            inputFilePath = ["/afs/cern.ch/user/r/roberth/scratch0/g4digi"]
            scpHost       = ""
            geometry      = 'ATLAS-CSC-01-02-00'
            fileType      = 'RDO'
            nfiles        = 1
            if type=="dataset":
                print "no dataset definition for this sample"
        elif name=='g4digi_bnl':
            inputFilePath = ["/usatlas/u/roberth/scratch0/samples/g4digi"]
            scpHost       = ''
            geometry      = 'ATLAS-CSC-01-02-00'
            fileType      = 'RDO'
            nfiles        = 1
            if type=="dataset":
                print "no dataset definition for this sample"
        elif name=='121080':
            inputFilePath = ["/castor/cern.ch/grid/atlas/DAQ/2009/00121080/physics_RPCwBeam"]
            scpHost       = ''
            geometry      = ''
            fileType      = 'RAW'
            nfiles        = 3018
            datasetdef    = "data09_cos.00121080.physics_RPCwBeam.daq.RAW"
        elif name=='153565_MinBias':
            inputFilePath = ["/data/data10_7TeV.00153565.physics_MinBias.merge.RAW"]
            scpHost       = "terrier10"
            geometry      = ''
            fileType      = 'RAW'
            nfiles        = 1
            datasetdef    = "data10_7TeV.00153565.physics_MinBias.merge.RAW"
        elif name=="cosmicMC":
            fileType      = 'RAW'
            datasetdef    = 'user10.KonstantinosBachas.mc09.108867.CosSimIDVolSolOnTorOn.s664.digit01.reco01.calhits01.v15636'
        elif name=="152221_MuonswBeam_DESD_SGLMU":
            inputFilePath = ["/data/data10_7TeV.00152221.physics_MuonswBeam.merge.DESD_SGLMU.f238_m428"]
            scpHost = "terrier09"
            geometry = ''
            fileType = "ESD"
            nfiles = 1
            datasetdef = 'data10_7TeV.00152221.physics_MuonswBeam.merge.DESD_SGLMU.f238_m428'

        elif name=="153565_MuonswBeam_DESD_SGLMU":
            inputFilePath = ["/tmp/roberth/data10_7TeV.00153565.physics_MuonswBeam.merge.DESD_SGLMU.r1297_p157_p159_tid138538_00"]
            scpHost = ""
            geometry = ''
            fileType = "ESD"
            nfiles = 32
            datasetdef = 'data10_7TeV.00153565.physics_MuonswBeam.merge.DESD_SGLMU.r1297_p157_p159_tid138538_00'

        elif name=="allRuns_MuonswBeam_DESD_SGLMU_r1299_p165_00":
            inputFilePath = ["/tmp/roberth/data10_7TeV.00155118.physics_MuonswBeam.merge.DESD_SGLMU.r1299_p161_p165_tid143723_00"]
            #inputFilePath = ["/tmp/roberth/data10_7TeV.00155160.physics_MuonswBeam.merge.DESD_SGLMU.r1299_p161_p165_tid143712_00"]
            scpHost = ""
            geometry = ''
            fileType = "ESD"
            nfiles = 139
            #nfiles = 2
            datasetdef = 'data10_7TeV.*.physics_MuonswBeam.merge.DESD_SGLMU.r1299*p165*_00'

        #elif name=="periodD":
        #    inputFilePath = ["/data/periodD/data10_7TeV.00165956.physics_Muons.merge.TAG.r1647_p306_p307_tid185039_00/"]
        #    scpHost       = 'terrier10'
        #    geometry      = ''
        #    fileType      = 'TAG'
        #    nfiles        = 10
        #    datasetdef    = 'data10_7TeV.periodD.physics_MuonswBeam.*.ESD.repro*'
        #    inputTagDef   = 'data10_7TeV.periodD.physics_MuonswBeam.*.TAG.repro*'

        elif name=="periodD":
            inputFilePath = ["/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/physics_Muons/0165956/data10_7TeV.00165956.physics_Muons.merge.TAG.f294_m614_m613/"]
            scpHost       = ''
            geometry      = ''
            fileType      = 'TAG'
            nfiles        = 10
            datasetdef    = 'data10_7TeV.periodD.physics_MuonswBeam.*.ESD.repro*'
            inputTagDef   = 'data10_7TeV.periodD.physics_MuonswBeam.*.TAG.repro*'

        elif name=="107206.singlepart_mu5.recon.ESD.e488_s634_s633_r870":
            inputFilePath = ["/tmp/roberth/mc09_valid.107206.singlepart_mu5.recon.ESD.e488_s634_s633_r870_tid099863"]
            scpHost  = ''
            geometry = ''
            nfiles   = 1
            fileType = 'ESD'
            datasetdef = "mc09_valid.107206.singlepart_mu5.recon.ESD.e488_s634_s633_r870_tid099863"
        elif name=="singlemu_5GeV_MDTEC":
            inputFilePath = ["/tmp/roberth/user10.RobertHarrington.CSC.2035462.singlepart_mu5GeV_MDT_ECA.50K_15.6.9.13.SIM_V2"]
            scpHost  = ''
            geometry = ''
            nfiles   = 1
            fileType = 'RDO'
            datasetdef = "user10.RobertHarrington.CSC.2035462.singlepart_mu5GeV_MDT_ECA.50K_15.6.9.13.SIM_V2"

        elif name=="bfield_off_MC_4GeV":
            inputFilePath = ["/data/group10.det-muon.12_0GeV.500000Events.ESD.20100629_123527.20100629_201811.20100701_093818"]
            scpHost       = 'terrier10'
            geometry      = ''
            nfiles        = 10
            fileType      = 'ESD'
            datasetdef    = "group10.det-muon.12_0GeV.500000Events.ESD.20100629_123527.20100629_201811.20100701_093818"

        elif name=="bfield_off_MC_6GeV":
            inputFilePath = ["/data/group10.det-muon.6_0GeV.500000Events.ESD.20100629_123348.20100629_140943.20100629_202006"]
            scpHost       = 'terrier10'
            geometry      = ''
            nfiles        = 10
            fileType      = 'ESD'
            datasetdef    = "group10.det-muon.6_0GeV.500000Events.ESD.20100629_123348.20100629_140943.20100629_202006"

        elif name=="bfield_off_MC_8GeV":
            inputFilePath = ["/data/group10.det-muon.8_0GeV.500000Events.ESD.20100629_123438.20100629_201859.20100705_062509"]
            scpHost       = 'terrier10'
            geometry      = ''
            nfiles        = 10
            fileType      = 'ESD'
            datasetdef    = "group10.det-muon.6_0GeV.500000Events.ESD.20100629_123438.20100629_201859.20100705_062509"
            
        elif name=="bfield_off_MC_12GeV":
            inputFilePath = ["/data/group10.det-muon.12_0GeV.500000Events.ESD.20100629_123527.20100629_201811.20100701_093818"]
            scpHost       = 'terrier10'
            geometry      = ''
            nfiles        = 76
            fileType      = 'ESD'
            datasetdef    = "group10.det-muon.12_0GeV.500000Events.ESD.20100629_123527.20100629_201811.20100701_093818"

        elif name=="bfield_off_MC_20GeV":
            inputFilePath = ["/data/group10.det-muon.20_0GeV.500000Events.ESD.20100629_134958.20100630_140447.20100630_201758"]
            scpHost       = 'terrier10'
            geometry      = ''
            nfiles        = 33
            fileType      = 'ESD'
            datasetdef    = "group10.det-muon.20_0GeV.500000Events.ESD.20100629_134958.20100630_140447.20100630_201758"

        elif name=="bfield_off_data":
            #inputFilePath = ["/tmp/roberth/data10_comm.00160303.calibration_EFMuonComm.recon.ESD.x34"]
            inputFilePath = ["/tmp/roberth/user.FelixRauscher.physics_Muons.0167963.reco.ESD.20101104.v2"]
            scpHost       = ''
            geometry      = ''
            nfiles        = 1
            fileType      = 'ESD'
            #datasetdef    = "group10.det-muon.20_0GeV.500000Events.ESD.20100629_134958.20100630_140447.20100630_201758"
            datasetdef    = ""

        elif name=='L1_zmumu_MC' or name=='L3_zmumu_MC' or name=='zmumu_MC':
            inputFilePath = ["/data/PythiaZmumu_1Lepton/mc10_7TeV.106051.PythiaZmumu_1Lepton.recon.ESD.e574_s933_s946_r1659_tid204812_00/"]
            scpHost       = 'terrier10'
            geometry      = ''
            nfiles        = 7
            fileType      = 'ESD'
            datasetdef    = "mc10_7TeV.106051.PythiaZmumu_1Lepton.recon.ESD.e574_s933_s946_r1659/"

        elif name=='15GeV_data':
            inputFilePath = ["/tmp/roberth/group.perf-muons.00165591.physics_Muons.reco.TMU15GEV.20101116.s1_ESD"]
#"/data/15GeV_data/group.perf-muons.162882.physics_Muons.reco.TMU15GEV.20101116.s1_ESD"]
            #scpHost       = 'terrier10'
            scpHost       = ''
            geometry      = ''
            nfiles        = 840
            #nfiles        = 1
            fileType      = 'ESD'
            datasetdef    = 'group.perf-muons*TMU15GEV.20101119*_ESD'
            
        elif name=="singlemu_12GeV":
            inputFilePath = ["/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Reconstruction/Mu12_15693"]
            scpHost       = ''
            geometry      = ''
            fileType      = 'ESD'
            nfiles        = 30
            datasetdef    = ''

        elif name=="singlemu_allPt":
            inputFilePath = ["/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Reconstruction/Mu2_15693","/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Reconstruction/Mu3_15693","/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Reconstruction/Mu4_15693","/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Reconstruction/Mu5_15693","/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Reconstruction/Mu6_15693","/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Reconstruction/Mu8_15693","/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Reconstruction/Mu10_15693","/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Reconstruction/Mu12_15693"]
            scpHost       = ''
            geometry      = ''
            fileType      = 'ESD'
            nfiles        = 240
            datasetdef    = ''

        elif name=='SGLMU_repro09_v01_data':
            inputFilePath = ["/tmp/roberth/data11_7TeV.00182997.physics_Muons.merge.DESD_SGLMU.r2603_p659_p662_tid494438_00/"]
#"/data/15GeV_data/group.perf-muons.162882.physics_Muons.reco.TMU15GEV.20101116.s1_ESD"]
            #scpHost       = 'terrier10'
            scpHost       = ''
            geometry      = ''
            nfiles        = 10
            #nfiles        = 1
            fileType      = 'ESD'
            datasetdef    = 'data11_7TeV.*.physics_Muons.merge.DESD_SGLMU.r2603_p659_p662/'
            

        else:
            print "bad name"            
            
        if type=="dataset" and inputFilePath==[]:
            if fileType=="RAW":
                inputFilePath = ["/data/data10_7TeV.00153565.physics_MinBias.merge.RAW"]
                scpHost       = "terrier09"
            elif fileType=="RDO":
                inputFilePath = ["/afs/cern.ch/user/r/roberth/scratch0/g4digi"]
                scpHost       = ""
                geometry      = 'ATLAS-CSC-01-02-00'
            elif fileType=="ESD":
                inputFilePath = ["/data/data10_7TeV.0015221.physics_MuonswBeam.merge.DESD_SGLMUf238_m428"]
                scpHost       = "terrier09"
            else:
                print "bad file format!  Use RAW, RDO, or ESD!"

        obj = Files(type,fileType,inputFilePath,scpHost,geometry,conditionsTag,nfiles,
                    datasetdef)

        return obj
