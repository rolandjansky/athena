#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,xml.dom.minidom

class tct_generateXML:
    def __init__(self):
        self.castorloc="/castor/cern.ch/grid/atlas/DAQ/2009/"
        self.fallbackloc="/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/"
        self.runnums=["00135664","00135492","Latest"]
        self.lateststreams = ['physics_CosmicMuons']
        self.nevent=500
        self.amitag="f160"
        self.nparjobs=3
        self.mergeCommands={'MergeJobIDComm':{'run':'00135664','streams':['IDCosmic'],'type':'IDCOMM'},
                            'MergeJobAODandTAG':{'run':'00135664','streams':['IDCosmic','L1Calo'],'type':'AODandTAG'}}
        self.dqwebdisplays={'StaticComparison':{'config':'Tier0ChainTests.TCTDQWebDisplayCosmics',
                                                'input':['cosmics_run.latest.hcfg',
                                                         'cosmics_minutes10.latest.hcfg',
                                                         'cosmics_minutes30.latest.hcfg']},
                            'HanFromReleaseComparison':{'config':'Tier0ChainTests.ReleaseDQWebDisplayCosmics',
                                                        'output':'run__han__release.root'},
                            'ComparisonWithYesterday':{'config':'Tier0ChainTests.DynamicDQWebDisplayCosmics',
                                                       'input':['cosmics_prevrun.latest.hcfg',
                                                                'cosmics_prevminutes10.latest.hcfg',
                                                                'cosmics_prevminutes30.latest.hcfg'],
                                                       'output':'run__han.root'},
                            }
        self.dqfallbackfiles=['/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_run.191526.hcfg',
                              '/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_minutes10.1.12.hcfg',
                              '/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_minutes30.1.9.hcfg']
        self.mydom=self.createCore()
       
   
    def generateRecoJobs(self):
        # Add list of reconstruction jobs as a parallel chain
        recoel = self.mydom.createElement("parallel")
        self.mydom.getElementsByTagName("sequential")[0].appendChild(recoel)
        for runnum in self.runnums:
            for stream in self.outputStreams(runnum):
                for seq in range(1,self.nparjobs):
                    self.createRecoCE(runnum,stream,seq)   

    def generateMergeJobs(self):
        # Add list of reconstruction jobs as a parallel chain
        self.allowedMergeTypes=['AODandTAG','IDCOMM']
        mergeel = self.mydom.createElement("parallel")
        self.mydom.getElementsByTagName("sequential")[0].appendChild(mergeel)
        print self.mergeCommands
        for mergeCommand in self.mergeCommands:
            self.createMergeCE(mergeCommand)
        

    def generateDQWebDisplays(self):
        # Add list of data quality job displays as a parallel chain
        dqwebel = self.mydom.createElement("parallel")
        self.mydom.getElementsByTagName("sequential")[0].appendChild(dqwebel)
        for dqwebname in self.dqwebdisplays:
            self.createDQWebCE(dqwebname,self.dqwebdisplays[dqwebname])


    def outputStreams(self,runnum):
        inputloc=self.castorloc+runnum
        if runnum == "Latest":
            return self.lateststreams
        OutStr=[]
        streams = os.popen("nsls %s" % inputloc).readlines()
        if len(streams) == 0:
            print "Run "+runnum+" not found on castor"
            return OutStr
        for stream in streams:
            if 'physics' in stream: 
                stream = stream.strip('\n').lstrip('physics_')
                OutStr.append(stream)
        return OutStr

    def createRecChainFileOut(self,stream,seq):
        filelist=[]
        filelist = filelist+["job_"+stream+"_"+str(seq)+".pool.root"]
        if stream == "IDCosmic":
            filelist = filelist+["myMergedMonitoring_"+stream+"_"+str(seq)+".root"]
        return filelist

    def createCore(self):
        # Create XML file header
        domCore=xml.dom.minidom.parseString(self.xmlCoreString())
        return domCore

    def createRecoCE(self,runnum,stream,seq):
        # This is the template for the job definition    
        doc1=xml.dom.minidom.parseString(self.xmlRecoString())

        # Set the jobTransformJobName 
        jobNameEl=doc1.getElementsByTagName("jobTransformJobName")[0]
        jobNameEl.childNodes[0].data="Run"+runnum+"_"+stream+str(seq)
    
        # Set the jobTransformSummaryName 
        jobNameElList=doc1.getElementsByTagName("argvalue")
        for iEl,jobNameEl in enumerate(jobNameElList):
            if not 'SummaryName' in jobNameEl.childNodes[0].data: continue
            jobNameEl.childNodes[0].data="Run"+runnum+"_"+stream+str(seq)+".summary.txt"
            break
    
        # Set the jobTransformCmd    
        cmdEl=doc1.getElementsByTagName("jobTransformCmd")[0]
        inputloc=self.castorloc+runnum
        if runnum == "Latest":
            cmdEl.childNodes[0].data="tct_recotrf2.sh -job "+str(seq)+" -trigstr "+stream+" -nfiles 1 -nevents "+str(self.nevent)+" -castor "+inputloc+" -amitag "+self.amitag+" -castorsubdir"
        else:
            cmdEl.childNodes[0].data="tct_recotrf2.sh -job "+str(seq)+" -trigstr "+stream+" -nfiles 3 -nevents "+str(self.nevent)+" -runnum "+runnum+" -castor "+inputloc+" -amitag "+self.amitag
            pass
    
        # Get the list of outputfiles to keep       
        #fileL = self.createRecChainFileOut(stream,seq)
        #for fileN in fileL:
        #    cfo = doc1.createElement("chainfileout")    
        #    cfoText=doc1.createTextNode(fileN)
        #    cfo.appendChild(cfoText)
        #    chainEl=doc1.getElementsByTagName("chainElement")[0]        
        #    chainEl.appendChild(cfo)
        #    pass

        # Insert reco CE into main xml. Make a deep copy        
        x = self.mydom.importNode(doc1.childNodes[0],True)
        self.insertXML(x)
        return

    def createMergeCE(self,mc):
        print "Merging with command "
        print self.mergeCommands[mc]
        runnum=self.mergeCommands[mc]['run']
        streams=self.mergeCommands[mc]['streams']
        type=self.mergeCommands[mc]['type']
        # check run number
        if runnum not in self.runnums:
            print "merging for undefined run requested, return"+runnum
            return
        #check streams
        definedStreams=self.outputStreams(runnum)
        for stream in streams:
            if stream not in definedStreams:
                print "merging for undefined stream requested, run "+runnum+"stream "+stream
                return
        # check type
        if type not in self.allowedMergeTypes:
            print "merging for undefined tyoe requested, return"+type
        # Make sure the files needed are kept
        inputFiles=[]
        # This is the list of reco elements, used to find the add point 
        recEls=self.mydom.getElementsByTagName("jobTransformJobName")
        
        filetype=type
        if type=="AODandTAG": filetype = "AOD"
        for stream in streams:
            for seq in range(1,self.nparjobs):
                inputFileNeeded="Run"+runnum+"_"+stream+str(seq)+"."+filetype+".pool.root"
                inputFiles.append(inputFileNeeded)
                trfName="Run"+runnum+"_"+stream+str(seq)
                recElNeed = None
                for recEl in recEls:
                    if recEl.childNodes[0].data == trfName:
                        cfo = self.mydom.createElement("chainfileout")    
                        cfoText=self.mydom.createTextNode(inputFileNeeded)
                        cfo.appendChild(cfoText)
                        recEl.parentNode.parentNode.appendChild(cfo)
                        print "I found tag with job name "+trfName

    def createDQWebCE(self,dqwebname,dqwebdict):
        # This is the template for the job definition    
        doc1=xml.dom.minidom.parseString(self.xmlDQWebString())

        # Set the jobTransformJobName 
        jobNameEl=doc1.getElementsByTagName("jobTransformJobName")[0]
        jobNameEl.childNodes[0].data="DQWebDisplay_"+dqwebname
    
        # Set the jobTransformSummaryName 
        jobNameElList=doc1.getElementsByTagName("argvalue")
        for iEl,jobNameEl in enumerate(jobNameElList):
            if not 'SummaryName' in jobNameEl.childNodes[0].data: continue
            jobNameEl.childNodes[0].data="Run"+runnum+"_"+stream+str(seq)+".summary.txt"
            break
    
        # Set the jobTransformCmd    
        cmdEl=doc1.getElementsByTagName("jobTransformCmd")[0]
        inputloc=self.castorloc+runnum
        if runnum == "Latest":
            cmdEl.childNodes[0].data="tct_recotrf2.sh -job "+str(seq)+" -trigstr "+stream+" -nfiles 1 -nevents "+str(self.nevent)+" -castor "+inputloc+" -amitag "+self.amitag+" -castorsubdir"
        else:
            cmdEl.childNodes[0].data="tct_recotrf2.sh -job "+str(seq)+" -trigstr "+stream+" -nfiles 3 -nevents "+str(self.nevent)+" -runnum "+runnum+" -castor "+inputloc+" -amitag "+self.amitag
            pass
    
        # Get the list of outputfiles to keep       
        fileL = self.createRecChainFileOut(stream,seq)
        for fileN in fileL:
            cfo = doc1.createElement("chainfileout")    
            cfoText=doc1.createTextNode(fileN)
            cfo.appendChild(cfoText)
            chainEl=doc1.getElementsByTagName("chainElement")[0]        
            chainEl.appendChild(cfo)
            pass

        # Insert reco CE into main xml. Make a deep copy        
        x = self.mydom.importNode(doc1.childNodes[0],True)
        self.insertXML(x)
        return

    def insertXML(self,xmlFrag):
        # Append the CE to the last parallel sequence
        mainel = self.mydom.getElementsByTagName("parallel")
        mainel[len(mainel)-1].appendChild(xmlFrag)


    def WriteXMLFile(self,filename,verbose):
        # write explicit XML file
        fout = open(filename,'w')
        lines = self.mydom.toprettyxml().split('\n')
        for i,line in enumerate(lines):
            if line.strip('\t') == '' or line.strip(' ') == '': continue # remove empty lines
            line = line.replace('\t','  ')
            shortline = line.strip()
            test1 = False
            if i < len(lines) - 2:
                compline = lines[i+2].replace('\t','')
                compline = compline.strip()
                compline = compline.replace('/','')
                if shortline == compline: test1 = True
                pass
            test2 = False
            if i > 2:
                compline = lines[i-2].replace('\t','')
                compline = compline.strip()
                compline = compline.replace('<','</')
                if shortline == compline: test2 = True
                pass
            test3 = False
            if i > 1:
                compline1 = lines[i-1].replace('\t','')
                compline1 = compline1.strip()
                compline2 = lines[i+1].replace('\t','')
                compline2 = compline2.strip()
                compline2 = compline2.replace('/','')
                if len(compline1) and compline1 == compline2: test3 = True
                pass
            if test1 or test3:
                if test3: line = line.strip()
                fout.write(line)
                if verbose:
                    print line,
            else:
                if test2: line = line.strip()
                fout.write(line+'\n')
                if verbose:
                    print line
                pass
            pass
        fout.close()
        return

    def xmlCoreString(self):
        jttemp="""\
<?xml version="1.0"?>
<!-- MB: Search for RUNNUMBER to replace the run input collections and setting -->
<!-- MB: xml file validation: http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/onlineValidation/validator.html -->
<!-- <!DOCTYPE unifiedTestConfiguration SYSTEM "https://test-rtt.web.cern.ch/test-rtt/brinick/Results/unifiedTestConfiguration.dtd"> --> <!-- xmltest -->
<!DOCTYPE unifiedTestConfiguration SYSTEM "http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/unifiedTestConfiguration.dtd"> <!-- official -->

<unifiedTestConfiguration>

<atn/>
<kv/>
<rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">
<rttContactPerson>Renaud Bruneliere</rttContactPerson>
<refRelease>14.5.0</refRelease>

<jobList>
<chain>
<chainName>Tier0ChainJob</chainName> <!-- Only letters allowed, no white space or punctuation -->
<sequential>

<!-- Get latest AMI tag configuration -->
<chainElement>
<jobTransform>
<doc>getAMITag</doc>
<jobTransformJobName>DummyJob_FirstRTTjobSometimesFails</jobTransformJobName>
<jobTransformCmd>
tct_getAmiTag.py f160 ami_recotrf.cmdargs
</jobTransformCmd>
<group>TCTTransform</group>
<queue>short</queue>
<batchWallTime>15</batchWallTime>
</jobTransform>
<chainfileout>ami_recotrf.cmdargs</chainfileout>
</chainElement>

</sequential>
</chain>
</jobList>

<jobGroups>

<jobGroup name="TCTTransform" parent="Transform">
<keepFilePattern>*.root</keepFilePattern>
<keepFilePattern>*.pdf</keepFilePattern>
<keepFilePattern>*.log</keepFilePattern>
<keepFilePattern>*.html</keepFilePattern>
<keepFilePattern>*.summary.txt</keepFilePattern>
<keepFilePattern>*MyCOOL_histo.db</keepFilePattern>
<keepFilePattern>*.pickle</keepFilePattern>
<keepFilePattern>*.gpickle</keepFilePattern>
</jobGroup>

</jobGroups>

</rtt>
</unifiedTestConfiguration>
        """
        return jttemp

    def xmlRecoString(self):
        jttemp="""\
              <chainElement>
                <jobTransform> <doc>RecoTrf</doc>
                <jobTransformJobName> JobName </jobTransformJobName>
                <jobTransformCmd>
                     TrfCommand
                </jobTransformCmd>
                <group>TCTTransform</group>
                <queue>long</queue>
                <batchWallTime>420</batchWallTime>
                <castorSvcClass>t0atlas</castorSvcClass>
                <test>
                  <modulename>RttLibraryTools</modulename>
                  <testname>FileGrepper</testname>
                  <arg>
                    <argname>inputFile</argname>
                    <argvalue>*_log</argvalue>
                  </arg>
                  <arg>
                    <argname>outputFile</argname>
                    <argvalue> SummaryName </argvalue> 
                  </arg>
                  <arg>
                    <argname>searchList</argname>
                    <argvalue>All files copied from castor are ok</argvalue>
                  </arg>
                </test>
                </jobTransform>
            </chainElement>
        """
        return jttemp

    def xmlDQWebString(self):
        jttemp="""\
            <chainElement>
              <jobTransform>
                <doc>DQJob</doc>
                <jobTransformJobName>JobName</jobTransformJobName>
                <jobTransformCmd>
                     TrfCommand
                </jobTransformCmd>

                <group>TCTTransform</group>
                <chaindataset_info>
                  <jobTransformData/>
                  <chaindatasetName>myMergedMonitoring_IDCosmic.root</chaindatasetName>
                  <dataset_info>
                    <jobTransformData/>
                    <datasetName>/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/myMergedMonitoring_IDCosmic_0.root</datasetName>
                  </dataset_info>
                </chaindataset_info>

                <queue>short</queue>
                <batchWallTime>60</batchWallTime>

              </jobTransform>
            </chainElement>
        """
        return jttemp

x=tct_generateXML()
# Create core of XML file
# mydom = x.createCore()

# Add list of reconstruction jobs
# recoel = mydom.createElement("parallel")
# mydom.getElementsByTagName("sequential")[0].appendChild(recoel)

#for runnum in runnums:
#    if runnum == "Latest":
#        streams = lateststreams
#        inputloc=castorloc
#    else:
#        inputloc=castorloc+runnum
#        streams = os.popen("nsls %s" % inputloc).readlines()
#        pass
#    for stream in x.outputStreams(runnum):
#        for seq in range(1,3):
#            x.createRecoCE(mydom,runnum,stream,seq)

x.generateRecoJobs()
x.generateMergeJobs()
# x.generateDQWebDisplays()

# Write XML file
filename = 'Tier0ChainTests_test_AtlasTier0.xml'
x.WriteXMLFile(filename,False)
