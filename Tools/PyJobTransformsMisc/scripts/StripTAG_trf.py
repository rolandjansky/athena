#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__doc__ = """Strip Tag Transform"""

import sys, os, re, commands
import xml.dom.minidom
#from PyJobTransformsCore.trf import JobTransform,JobReport
from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore import AtlasErrorCodes

ListOfDefaultPositionalKeys=['GuidBoundaryList','DirectIn','CollRefName','InputTAGFile','InputAODFile','outputTAGFile']

class DirectInArg(BoolArg):
    """Is this a direct access site?"""
    def __init__(self, help='default', name = 'DirectIn'):
        BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class InputRAWFileListArg(ListOfStringsArg):
    """Takes list of comma seperated Files"""
    def __init__(self,help='default',name='InputRAWFileList'):
        ListOfStringsArg.__init__(self,help,name)

class StripTagTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self, authors = [ Author('Thomas Doherty','thomas.doherty@glasgow.ac.uk')] , help = __doc__ )

        #add arguments
        self.add( OutputTAGFileArg())
        self.add( InputTAGFileArg())
        self.add( InputAODFileArg(), '')
        self.add( InputRAWFileListArg(), '')
        self.add( InputESDFileArg(), '')
        self.add( DirectInArg(), "True")
       

    def runJob(self):

        # open the logfile
        logFile = open(self._logFilename,'a')

        directIn = self.getArgument('DirectIn').value()
        tagFileList = self.getArgument('InputTAGFile').value()
        inputAODFiles =  self.getArgument('InputAODFile').value()
        no_of_AOD_files = len(inputAODFiles)
        outputTAGFile = self.getArgument('OutputTAGFile').value()
        collRefName = 'StreamAOD_ref'
        inputFiles = inputAODFiles
        if no_of_AOD_files < 1:
            inputESDFiles =  self.getArgument('InputESDFile').value() 
            collRefName = 'StreamESD_ref'
            inputFiles = inputESDFiles
            no_of_ESD_files = len(inputESDFiles)
            if no_of_ESD_files < 1:
                inputRAWFiles =  self.getArgument('InputRAWFileList').value()
                collRefName = 'StreamRAW_ref'
                inputFiles = inputRAWFiles
                no_of_RAW_files = len(inputRAWFiles)
                if no_of_RAW_files < 1:
                    print 'ERROR : No input file given - it must be an AOD, ESD or RAW file'
                    raise TransformArgumentError('ERROR : No input file given - it must be an AOD, ESD or RAW file',error='TRF_INFILE' )

        logFile.write("directIn passed in from command line is: "+str(directIn))
        logFile.write( os.linesep)
        logFile.write("collRefName passed in from command line is: "+str(collRefName))
        logFile.write( os.linesep)
        logFile.write("InputTagFile passed in from command line is: "+str(tagFileList))
        logFile.write( os.linesep)
        logFile.write("inputFiles passed in from command line is: "+str(inputFiles))
        logFile.write( os.linesep)     

        currentDir = os.getcwd()
        
        # -----------------------------------------------------
        # collect GUIDs from PoolFileCatalog
        guidMapFromPFC = {}
        directTmpTurl = {}
        try:
            #print "===== PFC from pilot ====="
            logFile.write( "===== PFC from pilot =====" + os.linesep)
            tmpPcFile = open("PoolFileCatalog.xml")
            #print tmpPcFile.read()
            logFile.write(str(tmpPcFile.read()))
            tmpPcFile.close()
            # parse XML
            root  = xml.dom.minidom.parse("PoolFileCatalog.xml")
            files = root.getElementsByTagName('File')
            for file in files:
                # get ID
                id = str(file.getAttribute('ID'))
                # get PFN node
                physical = file.getElementsByTagName('physical')[0]
                pfnNode  = physical.getElementsByTagName('pfn')[0]
                # convert UTF8 to Raw
                pfn = str(pfnNode.getAttribute('name'))
                lfn = pfn.split('/')[-1]
                # append
                guidMapFromPFC[lfn] = id
                directTmpTurl[id] = pfn
        except:
            type, value, traceBack = sys.exc_info()
            logFile.write( "ERROR : Failed to collect GUIDs : %s - %s" % (type,value))
            logFile.write( os.linesep)
            print 'ERROR : Failed to collect GUIDs : %s - %s' % (type,value)
            raise TransformArgumentError('ERROR : Failed to collect GUIDs : %s - %s' % (type,value),error='TRF_INFILE' )

        filesToPfcMap = {}
        newInputFilesMap = {}

        for fileName in inputFiles:
            #print "looping through each file - this file is: "+str(fileName)
            if tagFileList != []:
                #print "TagFileList has a file in it" # called: "+tagFileList
        
                # create symlink from 
                if not directIn:
                    #print "Not a direct access site"
                    try:
                        os.symlink('%s/%s' % (currentDir,fileName),fileName)
                    except:
                        pass

                # parent files
                tmpLFNforPFC = fileName.split('/')[-1]
                tmpLFNforPFC = re.sub('__DQ2-\d+$','',tmpLFNforPFC)
                #print "tmpLFNforPFC is now: "+tmpLFNforPFC
                if not tmpLFNforPFC in tagFileList:
                    #print "tmpLFNforPFC is not in tagFileList"
                    # append to PFC map to produce PoolFC.xml
                    if guidMapFromPFC.has_key(tmpLFNforPFC):
                        #print "Checking that tmpLFNforPFC exists in PFC" 
                        filesToPfcMap[guidMapFromPFC[tmpLFNforPFC]] = fileName
                        #print "In this case GUID exists so fileName "+fileName+" is entered into filesToPfcMap array"    
                    else:
                        #print "ERROR : %s not found in the pilot PFC" % fileName
                        raise TransformArgumentError('ERROR : %s not found in the pilot PFC' % fileName,error='TRF' ) 
                    # skip parent files for GUID extraction when                             
                    continue

        for tagFile in tagFileList:

            outFile = open('guids.txt','w')
            outFile.write(":"+outputTAGFile[:-5])
            outFile.write( os.linesep)
            item =  "%s"
            # Before using guid - make sure it is from file given in input list
            for fileName in inputFiles:
                for guid in filesToPfcMap.iteritems():
                    print "guid[1] is: "+guid[1]
                    print "filename is: "+str(fileName)
                    if guid[1] == str(fileName):   
                        outFile.write(item % (str(guid[0])))
            outFile.close()
           
            com = 'CollSplitByGUID.exe -splitref %s -maxsplit 1 -src PFN:%s RootCollection -guidfile guids.txt' % (collRefName, tagFile)
            print "About to run this command (using guids.txt): "+com
            logFile.write("About to run this command (using guids.txt): "+com)
            logFile.write( os.linesep)
            p = subprocess.Popen(com,shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
            out = p.communicate()[0]
            status = p.returncode
            if status != 0:
                print out
                print status
                print "Failed to run %s" % com
                logFile.write(out)
                logFile.write(str(status))
                logFile.write("Failed to run %s" % com)
                raise TransformArgumentError('Failed to run %s' % com,error='TRF' )
        pfnMap=filesToPfcMap
        # create PoolFC.xml when copyToScratch + EventLookupSvc
        outFile = open('PoolFileCatalog.xml','w')
        # write header
        header = \
        """<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
        <!-- Edited By POOL -->
        <!DOCTYPE POOLFILECATALOG SYSTEM "InMemory">
        <POOLFILECATALOG>
        """
        outFile.write(header)
        # write files
        item = \
        """
          <File ID="%s">
            <physical>
              <pfn filetype="ROOT_All" name="%s"/>
            </physical>
            <logical/>
          </File>
        """
        for guid,pfn in pfnMap.iteritems():
            outFile.write(item % (guid.upper(),pfn))
        # write trailer
        trailer = \
        """
        </POOLFILECATALOG>
        """
        outFile.write(trailer)
        outFile.close()

        #print newInputFiles
        athenaReport = JobReport()
        athenaReport.setIgnoreUnknown( self._ignoreUnknown )
        athenaReport.setIgnoreAll( self._ignoreAll )
        athenaReport.setIgnoreErrors( self._ignoreErrors )

        # NOTE ********* CHANGING ERROR CODE TO 0 FROM 11000 HERE AS ATHENA NOT USED WITH THIS TRANSFORM !!!! **************
        errorCode = athenaReport.errorCode()
        if (errorCode==11000):
            athenaReport.addInfo( JobInfo( 'athCode', str( 0 ) ) )

        return athenaReport
       

# execute it if not imported
if __name__ == '__main__':
    trf = StripTagTransform()
    sys.exit(trf.exeSysArgs().exitCode())

