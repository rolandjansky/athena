
from csv import reader
import math,os,glob,subprocess
import datetime
from collections import OrderedDict

import pyAMI.client


class UT:
    # This class contains the various tools required to upload values to the AMI physicsParameterVals DB table.
    
    def __init__(self,istest=True,verbose=False,alwaysyes=False,xsRefFile=None,delim=";"):
        # initialise
        self.istest=istest
        self.verbose=verbose
        self.alwaysyes=alwaysyes
        self.xsRefFile=xsRefFile
        self.delim=delim
        self.currentVals={}
        self.newVals={}
        if self.xsRefFile:
            self.xsRefLines=open(self.xsRefFile).readlines()

        if self.verbose: print "Initialising..."
       
        self.client=pyAMI.client.Client(
            'atlas'
             #'atlas-replica'
        )
        
        self.colors=self.bcolors(False)
        
    def checkUpload(self,origexplanation):
        # check that you really want to commit to the DB
        
        tmpans=False
        if self.alwaysyes:
            # always commit
            return True,False,origexplanation
            
        else:
            answer='X'
            while answer not in ['y','n','exp','a']: answer = raw_input('Continue with upload? (y/n/exp/a): ')
            if answer=='y':
                # upload with original explanation
                print self.colors.OKGREEN+"Commiting upload!"+self.colors.ENDC
                return True,False,origexplanation
            elif answer=='a':
                # upload with automatic explanation
                print self.colors.OKGREEN+"Commiting upload!"+self.colors.ENDC
                return True,False,"Automatic CentralPage upload"
            elif answer=='exp':
                # enter new explanation
                explanswer='X'
                while not tmpans:
                    explanation = raw_input('Please enter explanation: ')
                    explanswer='X'
                    while explanswer not in ['y','n','d','a']: explanswer = raw_input('Continue upload with new explanation "%s"? (y/n/d[go back to default]/a[automatic message]): '%explanation)
                    if explanswer in ['y','d','a']:
                        tmpans=True
                        
                print self.colors.OKGREEN+"Commiting upload!"+self.colors.ENDC
                if explanswer=='y':
                    return True,True,explanation        
                elif explanswer=='a':
                    return True,False,"Automatic CentralPage upload"
                elif explanswer=='d':
                    return True,False,origexplanation        
    
            else:
                # do not commit
                print self.colors.OKBLUE+"Not comitting upload."+self.colors.ENDC
                return False,False,origexplanation
           
        
    
    def cmdExec(self,cmd,explanation):
        # execute AMI upload command
        print "-->",cmd
        oldexplanation=explanation
        doUpload,newExplanation,explanation=self.checkUpload(oldexplanation)
        if doUpload:
            if newExplanation:
                #cmd.replace('--explanation="%s"'%oldexplanation,'--explanation="%s"'%explanation)
                newcmd=cmd.split(' --')
                for n,x in enumerate(newcmd):
                    if 'explanation=' in x:
                        newcmd[n]='explanation="%s"'%explanation
                        break
                    
                cmd=' --'.join(newcmd)
                print "-->",cmd
                
            if not self.istest:
                result = self.client.execute(cmd)
                print result
            else:
                print "dummy!"
                
        return doUpload
    
    def getXSRef(self,HORef,explanation=None):
        # Get reference cross section
        foundRefXS=False
        HOXS="UNKNOWN"
        HOSrc="UNKNOWN"

        # Read from Cross section Reference file
        for highXsecLine in self.xsRefLines:
            refexplanation="Unknown"
            highXsecInfo=highXsecLine.split(',')
            if len(highXsecInfo)<3:
                print self.colors.FAIL+"ERROR: Higher-order cross section line only has %i fields (expected 3)"%len(highXsecInfo)+self.colors.ENDC
                continue
                        
            highXsecRef=highXsecInfo[0].strip()
            highXsecVal=highXsecInfo[1].strip()
            if explanation:
                highXsecSrc=explanation
            else:
                highXsecSrc=highXsecInfo[2].strip()       
            
            if str(highXsecRef) == str(HORef):
                if highXsecVal=="Undefined" or highXsecVal=="":
                    print self.colors.FAIL+"ERROR: Higher order cross section value undefined - skipping"+self.colors.ENDC
                    highXsecVal="Unknown"
                    #continue
                elif highXsecRef=="Undefined" or highXsecRef=="":
                    print self.colors.FAIL+"ERROR: Higher order cross section reference undefined - skipping"+self.colors.ENDC
                    highXsecRef="Unknown"
                    #continue
                elif highXsecSrc=="Undefined" or highXsecSrc=="":
                    print self.colors.WARNING+"WARNING: Higher order cross section source undefined - skipping"+self.colors.ENDC
                    
                refexplanation=highXsecVal+","+highXsecSrc
                HOXS=highXsecVal
                HOSrc=highXsecSrc
                foundRefXS=True
                break

        if foundRefXS:
            #check if recalculation of HOXS is required...
            print self.colors.FAIL+"New k-factor for existing HORef - do I need to recalculate HOXS and and propage to all samples with HORef?!"+self.colors.ENDC
            self.findAllSamplesForXSRef()
        else:
            print self.colors.FAIL+"ERROR: Reference cross section %s not matched to CrossSectionReference file."%(HORef)+self.colors.ENDC
            explanation="Unknown"
            #HORef,HOXS,

            # Enter new values for reference, cross section and source
            HORef = "UNKNOWN"
            newanswer='X'
            tmpans=False

            while not tmpans:
                HORef = raw_input('Please enter new HORef for %s (or type "skip"): '%(HORef))
                if HORef=='skip':
                    HORef="UNKNOWN"
                    newanswer='skip'
                    tmpans=True
                    continue
                newanswer='X'
                while newanswer not in ['y','n','skip']: newanswer = raw_input('Continue upload with new HORef = %s? (y/n/skip): '%HOXS)
                if newanswer in ['y','skip']:
                    tmpans=True

            HOXS = "UNKNOWN"
            newanswer='X'
            tmpans=False
            while not tmpans and newanswer!='skip':
                HOXS = raw_input('Please enter new HOXS for %s (or type "skip"): '%(HORef))
                if HOXS=='skip':
                    HOXS="UNKNOWN"
                    newanswer='skip'
                    tmpans=True
                    continue
                newanswer='X'
                while newanswer not in ['y','n','skip']: newanswer = raw_input('Continue upload with new HOXS = %s? (y/n/skip): '%HOXS)
                if newanswer in ['y','skip']:
                    tmpans=True

            HOSrc = "UNKNOWN"
            tmpans=False
            while not tmpans and newanswer!='skip':
                HOSrc = raw_input('Please enter new HOSrc for %s (or type "skip"): '%(HORef))
                if HOSrc=='skip':
                    HOSrc="UNKNOWN"
                    newanswer='skip'
                    tmpans=True
                    continue

                newanswer='X'
                while newanswer not in ['y','n','skip']: newanswer = raw_input('Continue upload with new HOSrc = %s? (y/n/skip): '%HOSrc)
                if newanswer in ['y','skip']:
                    tmpans=True
                    
            if newanswer=='skip':
                print self.colors.OKBLUE+"INFO: Skipping new HOXS/HOSrc value for %s"%(HORef)+self.colors.ENDC
                highXsecVal="UNKNOWN"
            else:
                print self.colors.OKGREEN+"INFO: Adding new HOXS,HOSrc values %s,%s for %s"%(HOXS,HOSrc,HORef)+self.colors.ENDC
                self.addNewXSRefEntry(HORef,HOXS,HOSrc)
            

        return HOXS,HORef,HOSrc



    def getScope(self,ds):
        # get Rucio scope from DS name
        return ds.split('.')[0].split('_')[0]



    def updateValue(self,ds,param,value,explanation,highXsecRef=None):
        # update a DB value for a given ds
        if param == "crossSection":
            self.updateXS(ds,self.getScope(ds),value,explanation)
        elif param == "genFiltEff":
            self.updateFiltEff(ds,self.getScope(ds),value,explanation)
        elif param == "kFactor":
            #@todo work out how to upload new values to CrossSectionReference file if needed
            self.updateKFactor(ds,self.getScope(ds),value,explanation,highXsecRef)
        elif param == "processGroup":
            self.updateProcGroup(ds,self.getScope(ds),value,explanation)
        elif param == "physicsStatus":
            self.updatePhysicsStatus(ds,self.getScope(ds),value,explanation)
        elif param == "crossSectionRef":
            self.updateXSRef(ds,self.getScope(ds),value,explanation)
        else:
            print self.colors.FAIL+"ERROR: %s upload not currently supported"%(param)+self.colors.ENDC


    def removeParamEntry(self,ds,scope,param,explanation):
        # remove value (set end date) for a given ds and parameter
        cmd='RemovePhysicsParameterVals --paramName="%s" --logicalDatasetName="%s" --scope="%s" --physicsGroup="PMG" --explanation="%s"'%(param,ds,scope,explanation)
        self.cmdExec(cmd,explanation)

        
    def updateProcGroup(self,ds,scope,processGroup,explanation):
        # update processGroup
        cmd='AddPhysicsParameterVals --paramName="%s" --paramValue="%s" --logicalDatasetName="%s" --physicsGroup="PMG" --scope="%s" --explanation="%s"'%("processGroup",processGroup,ds,scope,explanation)
        self.cmdExec(cmd,explanation)
           
        
    def updateXS(self,ds,scope,xsVal,explanation):
        # update cross section
        cmd='AddPhysicsParameterVals --paramName="%s" --paramValue="%s" --logicalDatasetName="%s" --physicsGroup="PMG" --scope="%s" --explanation="%s"'%("crossSection",xsVal,ds,scope,explanation)
        self.cmdExec(cmd,explanation)

    
    def updateFiltEff(self,ds,scope,effVal,explanation):
        # update filter efficiency
        cmd='AddPhysicsParameterVals --paramName="%s" --paramValue="%s" --logicalDatasetName="%s" --physicsGroup="PMG" --scope="%s" --explanation="%s"'%("genFiltEff",effVal,ds,scope,explanation)
        self.cmdExec(cmd,explanation)
    
    def updateXSRef(self,ds,scope,XSRefVal,explanation):
        # update cross section reference
        highXsecVal,highXsecRef,highXsecSrc=self.getXSRef(XSRefVal)
        refexplanation=highXsecVal+","+highXsecSrc
        cmd='AddPhysicsParameterVals --paramName="%s" --paramValue="%s" --logicalDatasetName="%s" --physicsGroup="PMG" --scope="%s" --explanation="%s"'%("crossSectionRef",XSRefVal,ds,scope,refexplanation)
        self.cmdExec(cmd,explanation)
        if float(self.currentVals[ds]["kFactor"])==1.0:
            print "INFO: New XSRef - need to (re)calculate kFactor"
            self.updateKFactor(ds,scope,self.calcKFactor(highXsecRef,highXsecVal,self.newVals[ds]['crossSection']),explanation,highXsecRef,False)
            
    def updateKFactor(self,ds,scope,kFactorVal,explanation,highXsecRef,updateRef=True,refexplanation=""):
        # update k-factor
        # k-factor explanation should be crossSectionRef value and crossSectionRef explanation should be "<HOXS>,<HOXS source>"

        highXsecVal,highXsecRef,highXsecSrc=self.getXSRef(highXsecRef,refexplanation)

        if highXsecRef=="Unknown":
            # just upload k-factor with standard explanation because cross section reference can't be found
            cmd='AddPhysicsParameterVals --paramName="%s" --paramValue="%s" --logicalDatasetName="%s" --physicsGroup="PMG" --scope="%s" --explanation="%s"'%("kFactor",kFactorVal,ds,scope,explanation)
            self.cmdExec(cmd,explanation)
        else:
            # upload proper explanation and corresponding crossSectionRef
            cmd='AddPhysicsParameterVals --paramName="%s" --paramValue="%s" --logicalDatasetName="%s" --physicsGroup="PMG" --scope="%s" --explanation="%s"'%("kFactor",kFactorVal,ds,scope,highXsecRef)
            doExecKFact=self.cmdExec(cmd,highXsecRef)
            if doExecKFact:
                if updateRef and highXsecRef!=self.currentVals[ds]["crossSectionRef"]:                   
                    newexplanation=highXsecVal+","+highXsecSrc
                    if newexplanation!=refexplanation:
                        cmd_Ref='AddPhysicsParameterVals --paramName="crossSectionRef" --paramValue="%s" --logicalDatasetName="%s" --physicsGroup="PMG" --scope="%s" --explanation="%s"'%(highXsecRef,ds,scope,newexplanation)
                        self.cmdExec(cmd_Ref,refexplanation)
                    else:
                        print "INFO: crossSectionRef explanation the same as exiting - not uploading"
                else:
                    print "INFO: Not uploading crossSectionRef it is identical to current value (%s)"%(highXsecRef)
    
    def updatePhysicsStatus(self,ds,scope,val,explanation):
        # update physics status
        cmd='AddPhysicsParameterVals --paramName="%s" --paramValue="%s" --logicalDatasetName="%s" --physicsGroup="PMG" --scope="%s" --explanation="%s"'%("physicsStatus",val,ds,scope,explanation)
        self.cmdExec(cmd,explanation)
        
                    
    def calcKFactor(self,highXsecRef,highXsecVal,Xsec,BR=1.0):
        # calculate k-factor from HOXS value
        print "INFO: - XsecRef = %s, HOxs = %s"%(highXsecRef,highXsecVal)
        kFactor=(float(highXsecVal)/1000.)/(float(Xsec)/float(BR))
        print "kFactor = %f - HOxs= %f / ( xs=%f / BR=%f)"%(kFactor,float(highXsecVal)/1000.,float(Xsec),float(BR))
        return kFactor
    
    def calcKactorForXSGroup():
        # calculate k-factor for several samples inside a crossSectionGroup
        print "Need implementation"
                   
    def checkForAMIUpdate():
        # May want to check for updates in AMI automatic cross section calculatiob because then k-factors would need recalculating 
        print "Need implementation"

    def getDSListFromFile(self,filename):
        # Get list of datasets from input file
        outlist=[]
        dslist=[]
        myfile=open(filename)
        inlist=myfile.readlines()
        for line in inlist[1:]:
            if not len(line.strip()) or line.strip()[0]=='#':
                continue
            elements=line.split(self.delim)
            if elements[0] not in outlist:
                outlist.append(elements[0])

        return outlist

    
    
    def getFieldsInFile(self,filename):
        # work out what getMetadata fields are present in input file
        #getMetadata.py --timestamp="2017-11-24 21:59:10" --physicsGroups="PMG,MCGN" --fields="ldn,dataset_number,crossSection,kFactor,genFiltEff,processGroup,crossSectionRef" --inDS="mc15_13TeV.410111.MadGraphPythia8EvtGen_A14NNPDF23LO_ttee_Np0.evgen.EVNT.e4265"
        myfile=open(filename)
        inlist=myfile.readlines()
        foundCommand=False
        fielddict={}
        for line in inlist[1:]:
            if '#getMetadata.py' in line:
                foundCommand=True
                fields=[arg for arg in line.split('--') if 'fields=' in arg][0].replace('fields=','')
                fielddict={}
                fielddict={f.strip():n for n,f in enumerate(fields.replace('"','').split(','))}
                #need to make sure cross section is first in case we recalculate k-factor from it later
                d = OrderedDict()
                if 'crossSection' in fielddict:
                    d.update({'crossSection':fielddict['crossSection']})
                    del fielddict['crossSection']
                    d.update(fielddict)                  
                return d
        for line in inlist[0]:
            foundCommand=True
            fielddict={}
            fielddict={f.split('/')[0].strip():n for n,f in enumerate(line.replace('"','').split(':'))}
            d = OrderedDict()
            if 'crossSection' in fielddict:
                d.update({'crossSection':fielddict['crossSection']})
                del fielddict['crossSection']
                d.update(fielddict)               
            return d


        self.colors.FAIL+"ERROR: Failed to find getMetadata.py command in file"+self.colors.ENDC
        return


    def getDetailsFromFile(self,filename):
        # make dictionary for all the fields in every row of the input file
        filedict={}
        fielddict=self.getFieldsInFile(filename)
        
        myfile=open(filename)
        inlist=myfile.readlines()

        for line in inlist[1:]:
            if not len(line.strip()) or line.strip()[0]=='#' or 'ldn' in line:
                continue
            elements=line.strip().split(self.delim)
            if len(elements)<len(fielddict):
                print "ERROR: Number of elements found in line (%i) less than those in query fields (%i)"%(len(elements),len(fielddict))
                print "ERROR:  -> Elements:",elements
                break
            filedict[elements[fielddict["ldn"]]]={f:elements[fielddict[f]].strip() for f in fielddict if f != "ldn"}

        return filedict

        
    def getCurrentVals(self,dslist,fields):
        # get current values in AMI DB by running getMetadata.py query
        for f in glob.glob("/tmp/"+os.environ["USER"]+"/my.datasets*.txt"): os.remove(f)
        nline=1
        counter=1
        nDS=len(dslist)
        for ds in dslist:       
            if not os.access("/tmp/"+os.environ["USER"]+"/my.datasets"+str(counter)+".txt",os.R_OK):
                dsfile=open("/tmp/"+os.environ["USER"]+"/my.datasets"+str(counter)+".txt",'w')
            else:
                dsfile=open("/tmp/"+os.environ["USER"]+"/my.datasets"+str(counter)+".txt",'a')
            dsfile.write(ds+"\n")
            dsfile.close()
            nline=nline+1
            if nline > 500:
                nline=1
                counter=counter+1
        
        
        print "INFO: Found %i datasets, split into %i getMetadata queries"%(nDS,counter)
                
        for f in glob.glob("/tmp/"+os.environ["USER"]+"/metadata*.txt"): os.remove(f)
        metaFile=open("/tmp/"+os.environ["USER"]+"/metadata.txt",'w')
    
        
        for i in range(1,counter+1):
            print "getMetadata.py --delim="+self.delim+" --inDsTxt=/tmp/"+os.environ["USER"]+"/my.datasets"+str(i)+".txt --outFile=/tmp/"+os.environ["USER"]+"/metadata"+str(i)+".txt --fields"," ".join([f for f,n in fields.iteritems()])
            getMeta = subprocess.Popen(["getMetadata.py","--delim="+self.delim+"","--inDsTxt=/tmp/"+os.environ["USER"]+"/my.datasets"+str(i)+".txt","--outFile=/tmp/"+os.environ["USER"]+"/metadata"+str(i)+".txt","--fields"]+[f for f,n in fields.iteritems()])
            #,"ldn","dataset_number","crossSection","kFactor","genFiltEff","processGroup","crossSectionRef"])
            getMeta.wait()
            tmpMetaFile=open("/tmp/"+os.environ["USER"]+"/metadata"+str(i)+".txt",'r')
            metaFile.write(tmpMetaFile.read())
            tmpMetaFile.close()
        
            
        metaFile.close()
        
        metaFile=open("/tmp/"+os.environ["USER"]+"/metadata.txt",'r')
        metaFileList=metaFile.readlines()
        nDSFound=len([ ln for ln in metaFileList[1:] if ln[0]!='#' and len(ln.strip())])
        if nDS == nDSFound:
            print "INFO: %i/%i datasets found in AMI"%(nDSFound,nDS)
        else:
            print "WARNING: %i/%i datasets found in AMI"%(nDSFound,nDS)

        return "/tmp/"+os.environ["USER"]+"/metadata.txt"


    def addNewXSRefEntry(self,HORef,HOXS,HOSrc):
        # Add new entry to CrossSectionReference file
        #XsecTTbarNominal,831.76,https://twiki.cern.ch/twiki/bin/view/LHCPhysics/TtbarNNLO#Top_quark_pair_cross_sections_at
        #@todo
        reffile=open(self.xsRefFile,'a')
        reffile.write("\n%s,%s,%s"%(HORef,HOXS,HOSrc))
        reffile.close()
                
    def findAllSamplesForXSRef(self):
        # Search AMI DB by crossSectionRef to return all relevant ds
        #@todo
        print "findAllSamplesForXSRef not implemented"


    class bcolors  :
        # colors for print statements
        def __init__(self,nocolour=False):
            self.nocolour=nocolour
        
            if not nocolour:
                self.HEADER = '\033[95m'
                self.OKBLUE = '\033[94m'
                self.OKGREEN = '\033[92m'
                self.WARNING = '\033[93m'
                self.FAIL = '\033[91m'
                self.ENDC = '\033[0m'
            else:
                self.HEADER = ''
                self.OKBLUE = ''
                self.OKGREEN = ''
                self.WARNING = ''
                self.FAIL = ''
                self.ENDC = ''
            
        def disable(self):
            self.HEADER = ''
            self.OKBLUE = ''
            self.OKGREEN = ''
            self.WARNING = ''
            self.FAIL = ''
            self.ENDC = ''

        
