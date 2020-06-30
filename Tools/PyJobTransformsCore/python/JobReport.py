# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @package JobReport
#
#  @brief Main module containing the @em JobReport class and other ancillary classes @em JobInfo, @em FileInfo and @em TaskInfo.

from __future__ import with_statement, print_function
import os, sys, shutil, subprocess, itertools
import pickle
from copy import copy
from PyJobTransformsCore import AtlasErrorCodes, extraMetadata
from .xmlutil import XMLNode

__all__ = [ 'JobInfo', 'FileInfo', 'TaskInfo', 'JobReport', '_extraMetadataDict' ] 

# Get access to the extraMetadata.extraMetadataDict when this package is imported. 
_extraMetadataDict = extraMetadata.extraMetadataDict

## Container class used to store metadata associated with a job.
#  @details Metadata associated with a job is added as attributes
class JobInfo(XMLNode):

    ## Constructor for the JobInfo class.
    #  @param name Name of the job.
    #  @param contents Set the contents. Accepted values include @em XMLNode instances, 
    #  list of strings, plain strings and None. Defaults to @b None.
    #  @see xmlutil.XMLNode.setContents()
    def __init__(self,name,contents=None):
        XMLNode.__init__(self,name,contents)

    ## String representation of the JobInfo instance.
    #  @see xmlutil.XMLNode.__str__()
    #  @return String
    def __str__(self):
        return XMLNode.__str__(self)

    ## Return a shallow copy of self.
    #  @return copy of self (JobReport.JobInfo instance)
    def xmlNode(self):
        return copy(self)


## Container class used to store metadata for input and output files.
class FileInfo(XMLNode):

    ## Contructor for the FileInfo class.
    #  @param filename Name of the associated file.
    #  @param guid GUID associated with the file.
    #  @param metadata Dictionary of metadata associated with the file. 
    #  This dictionary is used to update the private @em xmlutil.XMLNode.__attributes dictionary attribute. Defaults to @b None.
    #  @see xmlutil.XMLNode.setAttributes()
    def __init__(self,filename,guid, metadata=None):
        XMLNode.__init__(self,"File")
        if metadata is None:
            metadata = {}
        self.setAttributes(filename=filename,guid=guid,metaData=metadata)

    ## Facility to allow the addition of metadata from another FileInfo instance (retain existing filename & guid).
    #  @return self (JobReport.FileInfo instance)
    def __iadd__(self,other):
        self.addMetaData( other.metaData() )
        return self

    ## Facility to allow FileInfo instance to be used as a key in a dictionary or similar data structures.
    #  @details The generated hash value is the identity of the associated file name.
    #  @return Integer
    def __hash__( self ):
        return id( self.filename() )
    
    ## Facility to determine equality between self and another FileInfo instance.
    #  @details Comparison criteria is simply the associated file name.
    #  @return Boolean
    def __eq__( self, other ):
        return self.filename() == other.filename()

    ## Facility to allow FileInfo instances to be compared.
    #  @details Comparison criteria is simply the associated file name.
    #  @return Integer (-1,0,1)
    def __cmp__( self, other ):
        fName = self.filename()
        oName = other.filename()
        if fName == oName:
            return 0
        if fName < oName:
            return -1
        return 1

    ## @brief String representation of the FileInfo instance.
    #  @details Pretty printing of information on the FileInfo instance (i.e. name, guid and associated metadata).
    #  @return String (multi-line)
    def __str__(self):
        indent = '  '
        me = [ "%s %s:" % (self.name(),self.filename()) ,
               "%sguid=%s" % (indent,self.guid()) ]
        meta = self.metaData()
        if meta:
            me += [ "%smetaData:" % (indent) ]
            for n,v in meta.items():
                me += [ "%s%s=%s" % (2*indent,n,v) ]
        return os.linesep.join(me)

    ## Getter function for the @em guid attribute.
    #  @warning If the @em guid attribute is missing, the function will return @b None.
    #  @return String
    def guid(self):
        return self.getAttribute("guid")

    ## Getter function for the @em filename attribute.
    #  @warning If the @em filename attribute is missing, the function will return @b None.
    #  @return String
    def filename(self):
        return self.getAttribute("filename")

    ## Getter function for metadata.
    #  @param name If @em name is omitted, the entire metadata dictionary is returned. 
    #  If specified, only the value of the associated metadata is returned. Defaults to @b None.
    #  @param moreOnly If set to @b True, the metadata dictionary is returned with several entries 
    #  (i.e. @em dataset, @em size, @em events and @em checkSum) omitted. 
    #  This is used by the routine that creates the job report gpickle file in the old format. 
    #  The new format does not require this alteration. Defaults to @b False.
    #  @return The entire metadata dictionary or a single value (depending on the @em name parameter).
    def metaData( self, name = None, moreOnly = None ):
        if moreOnly is None:
            moreOnly = False
        tempMetaData = self.getAttribute( "metaData" )
        if moreOnly:
            tempMetaData = copy( self.getAttribute( "metaData" ) )
            for itemToRemove in [ 'dataset', 'size', 'events', 'checkSum' ]:
                try:
                    tempMetaData.pop( itemToRemove )
                except KeyError:
                    pass
        if name is None:        
            return tempMetaData
        return tempMetaData.get( name )

    ## Update the existing metadata dictionary.
    #  @param metaDict A dictionary used to update the current metadata dictionary. 
    #  @return None
    def addMetaData(self,metaDict):
        self.metaData().update(metaDict)

    ## Clear the existing metadata dictionary.
    #  @return None
    def clearMetadata( self ):
        self.metaData().clear()

    ## Contruction of a new xmlutil.XMLNode instance to represent information contained in self.
    #  @return xmltuil.XMLNode instance
    def xmlNode(self):
        lfn = XMLNode( 'lfn' ).setAttribute( 'name', os.path.basename( self.filename() ) )
        logical = XMLNode( 'logical', lfn )
        fileNode = XMLNode( self.name(), logical ).setAttribute( 'ID', self.guid() )
        meta = self.metaData()
        for key,val in meta.items():
            metaNode = XMLNode( 'metadata' ).setAttributes( att_name = key, att_value = val )
            fileNode.addContents( metaNode )
        return fileNode

    ## Retrieve the XML representation of the information contained in self.
    #  @return String (multi-line)
    def getXML(self):
        return self.xmlNode().getXML()
    
    ## Retrieve the information contained in self as a dictionary.
    #  @return Dictionary
    def getDict( self ):
        _d = { 'name' : os.path.basename( self.filename() ),
               'guid' : self.guid() }
        try:
            _d.update( self.metaData() )
        finally:
            return _d

## @brief Container class used to store information pertaining to a (transform) task.
#  @details The TaskInfo class encapsulates all information pertaining to a particular transformation. 
#  It is designed to be recursively included to express and retain the relationships in composite transformations. 
#  Each transformation will have an associated TaskInfo instance describe the task it is running. 
#  Each TaskInfo instance may recursively include other sub-tasks (also TaskInfo instances).
class TaskInfo( XMLNode ):

    ## Constructor for TaskInfo.
    #  @param taskname Name of the task. Typically the same as the associated transformation. Defaults to @c DEFAULT
    #  @param metaData Dictionary of metadata to be associated with the task and stored as attributes of the task. 
    #  Defaults to an empty Dictionary.
    def __init__( self, taskname = None, metaData = None ):
        if metaData is None:
            metaData = {}
        if taskname is None:
            taskname = 'DEFAULT'
        XMLNode.__init__( self, "Task" )
        # insert metadata provided as attributes.
        self.setAttributes( taskname = taskname, metaData = metaData)
        ## Dictionary of JobInfo objects
        self.infos     = {}
        ## Dictionary of output FileInfo objects.
        self.outFiles  = {}
        ## Dictionary of input FileInfo objects
        self.inFiles   = {}
        ## Dictionary of AtlasErrorCodes.ErrorCategory objects. Stores the error categories of the errors encountered by the task.
        self.errorCategories = {}
        ## List of AtlasErrorCodes.ErrorInfo objects representing errors encountered by the task.
        self.errors    = []
        ## Dictionary of the number of errors of a particular severity required to be stored.
        self.errorLimits = { AtlasErrorCodes.FATAL : 10, AtlasErrorCodes.ERROR : 10, AtlasErrorCodes.WARNING : 10 }
        ## List of AtlasErrorCodes.ErrorInfo objects representing validation-related errors encountered by the task.
        self.validationErrors = []
        ## List of sub-tasks (i.e. TaskInfo objects). Sub-tasks only exist for composite tasks.
        self.subTasks  = []
        ## The command issued at the command-line to execute the associated transformation.
        self.command   = None

    ## Facility to allow TaskInfo objects to be @em combined (i.e. the updating of attributes with those from another TaskInfo object) 
    #  @remarks All attributes are updated apart from the @em taskname and any other attributes explicitly added post-contruction.
    #  @return self
    def __iadd__(self,other):
        self.infos.update( other.infos )
        self.outFiles.update( other.outFiles )
        self.inFiles.update( other.inFiles)
        self.addMetaData( other.metaData() )
        for o_err in other.errors:
            self.addError( o_err )
        for o_vError in other.validationErrors:
            self.addValidationError( o_vError )
        for o_errCat in other.errorCategories.values():
            self.addErrorCategory( o_errCat )
        for severity, limit in self.errorLimits.items():
            if limit + other.errorLimits[ severity ] <= 10:
                self.errorLimits[ severity ] = 0
        self.subTasks.extend( other.subTasks )
        return self

    ## String representation of the metadata associated with the TaskInfo instance and it's sub-tasks with the 
    #  relationship between nested tasks retained.
    #  @return String (multi-line)
    def __str__(self):
        indent = '  '
        me = [ "\n%s %s:" % (self.name(),self.taskName()) ]
        meta = self.metaData()
        if meta:
            me += [ "%smetaData:" % (indent) ]
            for n,v in meta.items():
                me += [ "%s%s=%s" % (2*indent,n,v) ]
        me.append( "Sub-tasks:" )
        me.extend( [ st.taskName() for st in self.subTasks ] )
        return os.linesep.join(me)
    
    ## Getter function for the @em taskname attribute.
    #  @return String
    def taskName( self ):
        return self.getAttribute( "taskname" )

    ## Generator function yielding the @em taskName TaskInfo instance and all its sub-tasks.
    #  @param trfName Name of the TaskInfo instance to begin the preoder/depth-first traversal. 
    #  Defaults to top-level task.
    #  @param createIfNotExist Boolean flag to denote if a new TaskInfo instance should be created 
    #  if a task @em trfName is not found. Defaults to @b False.
    #  @return TaskInfo instance
    def tasks( self, trfName = None, createIfNotExist = False ):
        taskFound = False
        if trfName in [ None, True, self.taskName() ]:
            trfName = None
            taskFound = True
            yield self
        elif trfName is False: # omit top level task
            trfName = None
        for subtask in self.subTasks:
            for st in subtask.tasks( trfName, False ):
                taskFound = True
                yield st
        if not taskFound and createIfNotExist:
            st = TaskInfo( trfName )
            self.subTasks.append( st )
            yield st

    ## Getter function for the @em metaData dictionary attribute.
    #  @param name Name of a particular metadata entry. If provided, the value associated with that particular metadata 
    #  entry is returned. If omitted, the entire @em metaData dictionary attribute is returned.
    #  @return String or Dictionary.
    def metaData(self,name=None):
        if name is None:
            return self.getAttribute("metaData")
        else:
            return self.getAttribute("metaData").get(name)

    ## Add new metadata entries and/or update existing ones.
    #  @param metaDict Dictionary containg metadata entries to add/update.
    #  @return None
    def addMetaData(self,metaDict):
        self.metaData().update( metaDict )

    ## Add an AtlasErrorCodes.ErrorCategory to the @em errorCategories Dictionary.
    #  @remarks The addition of an duplicate AtlasErrorCodes.ErrorCategory will not result in a replacement of the 
    #  existing entry but an increment in the AtlasErrorCodes.ErrorCategory.count value instead.
    #  @return None
    def addErrorCategory( self, category ):
        try:
            self.errorCategories[ category.acronym ].count += category.count
        except KeyError:
            self.errorCategories[ category.acronym ] = copy( category )
    
    ## Associate an AtlasErrorCodes.ErrorInfo object with this task. 
    #  @param error The AtlasErrorCodes.ErrorInfo object to add.
    #  @remarks If the error already exists, it will be @em added to the existing one.
    #  @see AtlasErrorCodes.ErrorInfo.__iadd__() on details of how AtlasErrorCodes.ErrorInfo objects are added together.
    #  @return None
    def addError( self, error ):
        for err in self.errors:
            if err == error:
                err += error
                break
        else:
            self.errors.append( copy( error ) )

    ## Associate a validation-related AtlasErrorCodes.ErrorInfo object with this task.
    #  @param error The AtlasErrorCodes.ErrorInfo object to add.
    #  @remarks If the validation error already exists, it will be @em added to the existing one.
    #  @see AtlasErrorCodes.ErrorInfo.__iadd__() on details of how AtlasErrorCodes.ErrorInfo objects are added together.
    #  @return None
    def addValidationError( self, error ):
        for ve in self.validationErrors:
            if ve == error:
                ve += error
                break
        else:
            self.validationErrors.append( copy( error ) )

    ## Associate a FileInfo object with the task.
    #  @param fileType Type of file to add. Accepted values are @c input and @c output.
    #  @param fileInfo The FileInfo object to associate with the task.
    #  @remarks If the FileInfo object already exists, it is @em added to the existing one.
    #  @see FileInfo.__iadd__() on details of how FileInfo objects are added together.
    #  @return None 
    def addFile( self, fileType, fileInfo ):
        fileDict = { 'INPUT' : self.inFiles, 'OUTPUT': self.outFiles }[ fileType.upper() ]
        try:
            fInfo = fileDict[ fileInfo.filename() ]
        except KeyError:
            fileDict[ fileInfo.filename() ] = copy( fileInfo )
        else:
            fInfo += fileInfo

    ## Contruction of a new xmlutil.XMLNode instance to represent information contained in self.
    #  @remarks JobInfo information is omitted as it is not required when writing out to XML.
    #  @return xmltuil.XMLNode instance
    def xmlNode(self):
        taskNode = XMLNode( self.name() ).setAttribute( 'name', self.taskName() )
        for metaKey, metaValue in self.metaData().items():
            metaNode = XMLNode( "metadata" ).setAttributes( name = metaKey, value = metaValue )
            taskNode.addContents( metaNode )
#        infoNode = XMLNode( "jobinfo" )
#        for infoObject in self.infos.values():
#            infoNode.addContents( infoObject.xmlNode() )
#        taskNode.addContents( infoNode )
        inFileNode = XMLNode( "inputfiles" )
        for inFileObject in self.inFiles.values():
            inFileNode.addContents( inFileObject.xmlNode() )
        taskNode.addContents( inFileNode )
        outFileNode = XMLNode( "outputfiles" )
        for outFileObject in self.outFiles.values():
            outFileNode.addContents( outFileObject.xmlNode() )
        taskNode.addContents( outFileNode )
        for sTask in self.subTasks:
            taskNode.addContents( sTask.xmlNode() )
        return taskNode

    ## Generate the XML string describing this task.
    #  @return String (multi-line)
    def getXML( self ):
        return self.xmlNode().getXML()
    
    ## Generate a customised Python dictionary containing the task's @em name, @em metadata, 
    #  associated @em files and @em sub-tasks.
    #  @return Dictionary
    def getDict( self ):
        _d = { 'name' : self.taskName(),
               'metadata' : self.metaData(),
               'inputfiles' : [ inFileObject.getDict() for inFileObject in self.inFiles.values() ],
               'outputfiles' : [ outFileObject.getDict() for outFileObject in self.outFiles.values() ],
               'subtasks' : [ sTask.getDict() for sTask in self.subTasks ] }
        return _d


## This class contains information on the results of execution of the associated transform.
class JobReport( object ):
    metadata_xml = 'metadata.xml'
    jobinfo_xml = 'jobInfo.xml'
    defaultFilenameBase = 'jobReport'
    fileExtensions = ( '.txt', '.pickle', '.xml' )
    # List of files created by the transform
    
    ## @brief Constructor for the JobReport class.
    #  @details A newly instantiated JobReport class is invalid for the purpose for which it was designed as 
    #  it does not contain the required information for it to be used effectively. Although not designed to be 
    #  used directly, it is automatically created (an appropriately populated) when the transform is executed. 
    #  A JobReport instance may be explicitly created as a container in which other JobReport instances may be 
    #  stored e.g. in composite transforms.
    def __init__( self ):
        # Can't be at class scope due to py3 scoping rules for comprehensions.
        JobReport.defaultFiles = [ self.defaultFilenameBase + ext for ext in self.fileExtensions ] + [ self.metadata_xml, self.jobinfo_xml ]

        self.reset()
        self.setCommand()
        # print everything by default
        self.setPrintOptions( [], [] )

    ## Persistify the JobReport if it was modified.
    def __del__( self ):
        if self.__modified: self.write()

    ## String representation of the Job Report. 
    #  @remarks The contents of this string is influenced by the use of the setPrintOptions() function.
    #  @return String (multi-line)
    def __str__( self ):
        if self.__printNone: return "" # return empty string
        # header
        header = "===== Job Report"
        if self._trf:
            header += " produced by %s" % self._trf
            if self._trfVersion:
                header += " version %s" % self._trfVersion
            if self._trfComment:
                header += " (%s)" % self._trfComment
        header += " ====="
        me = [ header ]
        # Summary
        if self._shouldPrintItem( 'Summary' ):
            me.append( 'Command=%s' % self.command() )
            me.append( 'ExitCode=%s' % self.exitCode() )
            ec = self.errorCode()
            ea = AtlasErrorCodes.getAcronym(ec)
            me.append( 'ErrorCode=%d (%s)' % (ec,ea) )
            # error category counts
            for cat in self.errorCategories():
                me.append( 'ErrorCategory=%s' % (cat,) )
            # fatals
            me.append( 'Fatals=%d' % self.fatalCount() )
            # errors
            me.append( 'Errors=%d' % (self.errorCount()) )
            # warnings
            me.append( 'Warnings=%d' % (self.warningCount()) )
            # output files
            for f in self.outputFiles():
                me.append( str(f) )
        # Errors
        if self._shouldPrintItem( 'Errors' ):
            for info in itertools.chain( self.validationErrors(), self.errors() ):
                me.append( str( info ) )
        # Other info
        for info in self.infos():
            if self._shouldPrintItem( info.name() ):
                me.append( str( info ) )
        return os.linesep.join( me )

    ## Function to test if a particular item should be included in the print out of the job report.
    #  @param name The name of the item to be checked.
    #  @return Boolean
    def _shouldPrintItem( self, name ):
        return not self.__printNone and name not in self.__printNotOptions and \
               ( not self.__printOptions or name in self.__printOptions )

    ## Resets all attributes to their default values.
    #  @return None
    def reset( self ):
        ## Name of the associated transform 
        self._trf = None
        ## Version of the asociated transform
        self._trfVersion = None
        ## Comments pertaining to the associated transform
        self._trfComment = None
        ## The internally associated TaskInfo instance.
        self._task = TaskInfo()
        ## Boolean flag to indicate if @c self has been modified. 
        self.__modified = False
        ## Exit code/error code manipulation Boolean flag. Determines if unrecognised errors are ignored.
        self.__ignoreUnknown = False
        ## Exit code/error code manipulation Boolean flag. Ignore all errors.
        self.__ignoreAll = False
        ## Exit code/error code manipulation Boolean flag. 
        #  Ignore errors if the athena.py job and the various validation routines performed were successful.
        self.__ignoreErrors = False
        ## Boolean flag to switch between old and new metadata and job report gpickle formats.
        #  @see writeMetaDataXML_new(), writeMetaDataXML_old(), GPickleContents_new() and GPickleContents_old().
        self.useNewMetadataFormat = False
        ## Boolean flag to determine if the validation routines performed on the associated transform was successful.
        #  @remarks This Boolean flag may be modified when the getExitError() function is run.
        self.validationSuccess = True
        ## Information tailored for the production system. This dictionary will typically be written to disk.
        #  @see writeGPickle().
        self.gpickle_dict = {}
        ## Boolean flag to allow printing of job report to be disabled.
        self.__printNone = False
        ## List of items to print. @b Note: @em [] denotes all items should be printed.
        self.__printOptions = []
        ## List of items not to print.
        self.__printNotOptions = []

    ## Associate the job report with av transform.
    #  @param name The name of the transform.
    #  @param version A String providing the version of the transform. Defaults to @b None.
    #  @param comment A String providing some description of the transform. Defaults to @b None.
    #  @remarks The internally associated TaskInfo instance is given the same parameters.
    #  @return None
    def setProducer( self, name, version = None, comment = None ):
        self._task.setAttributes( taskname = name, version = version, comment = comment )
        self._trf = name
        self._trfVersion = version
        self._trfComment = comment

    ## Getter function for retrieving the @em taskname attribute from the internally associated TaskInfo instance.
    #  @return String
    def producer( self ):
        return self._task.getAttribute( 'taskname' )

    ## Getter function for retrieving the @em version attribute from the internally associated TaskInfo instance.
    #  @return String
    def producerVersion( self ):
        return self._task.getAttribute( 'version' )

    ## Getter function for retrieving the @em comment attribute from the internally associated TaskInfo instance.
    #  @return String
    def producerComment( self ):
        return self._task.getAttribute( 'comment' )

    ## Getter function for retrieving the command issued at the command-line to run the transform performing 
    #  a particular task @em trfName.
    #  @param trfName The name of the transform to retrieve the command from. Defaults to @b None.
    #  @remarks If @em trfName is provided, the internally associated TaskInfo instance 
    #  (including all its sub-tasks, in any) is searched and a task/sub-task with a matching @em trfName is 
    #  accessed and its associated execution command retrieved. If @em trfName is omitted, the top-level task 
    #  (i.e. the task directly associated with this job report) is used.
    #  @return String
    def command( self, trfName = None ):
        return self.task( trfName ).command

    ## Record the command used at the command-line to execute the transform.
    #  @param command The command used at the command-line. Defaults to @c sys.argv.
    #  @param trfName The name of the task to associated the @em command with. Defaults to @b None.
    #  @return None
    def setCommand( self, command = None, trfName = None ):
        if command is None:
            command = sys.argv[0] + ' '
            for i in sys.argv[1:]:
                if i.find('=') > -1:
                    key, value = i.split('=', 1)
                    command += key + '=' + "'" + value.replace("'", "'\\''") + "'" + ' '
                else:
                    command += "'" + i.replace("'", "'\\''") + "'" + ' '
        self.task( trfName ).command = command
        self.__modified = True

    ## Customise the information presented when the job report is printed.
    #  @param options List of names of items to print. A comma-delimited string is also accepted. 
    #  An empty list denotes @em all items are to be printed. Defaults to the current value i.e. unchanged.
    #  @param notOptions List of names of items @em not to print. Takes precedence over @em options list. 
    #  A comma-delimited string is also accepted. Defaults to the current value i.e. unchanged.
    #  @warning if @em None is found in the @em options list, @em nothing will be printed.
    #  @return None
    def setPrintOptions( self, options = None, notOptions = None ):
        if options is None:
            options = self.__printOptions
        if notOptions is None:
            notOptions = self.__printNotOptions
        try:
            self.__printOptions = options.split( ',' )
        except AttributeError:
            self.__printOptions = copy( options )
        self.__printNone = 'None' in self.__printOptions
        try:
            self.__printNotOptions = notOptions.split( ',' )
        except AttributeError:
            self.__printNotOptions = copy( notOptions )

    ## Write out the current string presentation of the job report in a (one-off) customised format without 
    #  affecting the current formating.
    #  @param options List of names of items to print. A comma-delimited string is also accepted. 
    #  An empty list denotes @em all items are to be printed. Defaults to the current value i.e. unchanged.
    #  @param notOptions List of names of items @em not to print. Takes precedence over @em options list. 
    #  A comma-delimited string is also accepted. Defaults to the current value i.e. unchanged.
    #  @param output An open file(-like) object to write the job report to. Defaults to @c sys.stdout.
    #  @warning if @em None is found in the @em options list, @em nothing will be printed.
    #  @return None
    def dump(self,options=None,notOptions=None,output=sys.stdout):
        # remember current options
        oldOptions = self.__printOptions
        oldNotOptions = self.__printNotOptions
        # set new options (makes a copy)
        self.setPrintOptions(options,notOptions)
        # get the printout in a string
        meString = self.__str__()
        # restore options
        self.__printOptions = oldOptions
        self.__printNotOptions = oldNotOptions
        if meString:
            output.write( meString + os.linesep )

    ## Generator function yielding all (relevant) associated JobInfo instances.
    #  @param trfName Retrieval of all JobInfo instances starting from the task @em trfName.
    #  (preoder/depth-first traversal of tasks). Defaults to the top-level task.
    #  @return JobInfo instance
    def infos( self, trfName = None ):
        for t in self.tasks( trfName ):
            for i in t.infos.values():
                yield i

    ## Getter function for JobInfo instance.
    #  @param name Name of JobInfo instance to retrieve.
    #  @param trfName The name of the task to retrieve this JobInfo instance from. Defaults to the top-level task.
    #  @return String
    def info( self, name, trfName = None ):
        return self.task( trfName ).infos[ name ]

    ## Add a JobInfo instance to the selected task's dictionary of JobInfo instances.
    #  @param info JobInfo instance to be added.
    #  @param trfName The name of the task to add this JobInfo instance to. Defaults to the top-level task.
    #  @return None
    def addInfo(self, info, trfName = None):
        if info is None: 
            return
        self.task( trfName ).infos[ info.name() ] = copy( info )
        self.__modified = True

    ## Remove a JobInfo instance from the selected task's dictionary of JobInfo instances.
    #  @param name Name of the JobInfo instance to remove from the selected task's dictionary of JobInfo instances.
    #  @param trfName The name of the task to remove this JobInfo instance from. Defaults to the top-level task.
    #  @return None
    def removeInfo(self, name, trfName = None ):
        self.task( trfName ).infos.pop( name )
        self.__modified = True

    ## Generator function yielding all (relevant) associated output FileInfo instances.
    #  @param trfName Retrieval of all output FileInfo instances starting from the task @em trfName 
    #  (preoder/depth-first traversal of tasks). Defaults to the top-level task.
    #  @return FileInfo instance
    def outputFiles( self, trfName = None ):
        for t in self.tasks( trfName ):
            for oFile in t.outFiles.values():
                yield oFile

    ## Getter function for a particular output FileInfo instance.
    #  @param filename The name of the file described by the FileInfo instance.
    #  @param trfName The name of the task to retrieve this FileInfo instance from. Defaults to the top-level task.
    #  @return FileInfo instance (or @b None if retrieval was unsuccessful)
    def outputFile( self, filename, trfName = None ):
        for f in self.outputFiles( trfName ):
            if f.filename() == filename: return f
        return None

    ## Add a FileInfo instance to the selected task's dictionary of output FileInfo instances.
    #  @param info FileInfo instance to be added.
    #  @param trfName The name of the task to add this FileInfo instance to. Defaults to the top-level task.
    #  @warning If a similar entry (i.e. FileInfo instance with the same name) exists, it will be overwritten.
    #  @return None
    def addOutputFile( self, info, trfName = None ):
        if info is None: 
            return
        self.task( trfName ).addFile( 'OUTPUT', info )
        self.__modified = True

    ## Remove a FileInfo instance from the selected task's dictionary of output FileInfo instances.
    #  @param filename Name of the FileInfo instance to remove from the selected task's dictionary of output FileInfo instances.
    #  @param trfName The name of the task to remove this FileInfo instance from. Defaults to the top-level task.
    #  @return None
    def removeOutputFile( self, filename, trfName = None ):
        self.task( trfName ).outFiles.pop( filename )
        self.__modified = True

    ## Generator function yielding all (relevant) associated intput FileInfo instances.
    #  @param trfName Retrieval of all input FileInfo instances starting from the task @em trfName 
    #  (preoder/depth-first traversal of tasks). Defaults to the top-level task.
    #  @return FileInfo instance
    def inputFiles( self, trfName = None ):
        for t in self.tasks( trfName ):
            for iFile in t.inFiles.values():
                yield iFile

    ## Getter function for a particular input FileInfo instance.
    #  @param filename The name of the file described by the FileInfo instance.
    #  @param trfName The name of the task to retrieve this FileInfo instance from. Defaults to the top-level task.
    #  @return FileInfo instance (or @b None if retrieval was unsuccessful)
    def inputFile( self, filename, trfName = None ):
        for f in self.inputFiles( trfName ):
            if f.filename() == filename: return f
        return None

    ## Add a FileInfo instance to the selected task's dictionary of input FileInfo instances.
    #  @param info FileInfo instance to be added.
    #  @param trfName The name of the task to add this FileInfo instance to. Defaults to the top-level task.
    #  @warning If a similar entry (i.e. FileInfo instance with the same name) exists, it will be overwritten.
    #  @return None
    def addInputFile( self, info, trfName = None ):
        if info is None: 
            return
        self.task( trfName ).addFile( 'INPUT', info )
        self.__modified = True

    ## Remove a FileInfo instance from the selected task's dictionary of input FileInfo instances.
    #  @param filename Name of the FileInfo instance to remove from the selected task's dictionary of
    #  input FileInfo instances.
    #  @param trfName The name of the task to remove this FileInfo instance from.
    #  Defaults to the top-level task.
    #  @return None
    def removeInputFile( self, filename, trfName = None ):
        self.task( trfName ).inFiles.pop( filename )
        self.__modified = True

    ## Wrapper function to the generator function yielding all (relevant) associated TaskInfo instances.
    #  @param trfName Retrieval of all TaskInfo instances starting from the task @em trfName
    #  (preoder/depth-first traversal of tasks). If @c None, all sub-transform tasks including the top-level
    #  transform are retrieved. Defaults to @c None.
    #  @param createIfNotExist Boolean flag to denote if a new TaskInfo instance should be created 
    #  if the task @em trfName is not found. Defaults to @b False.
    #  @return TaskInfo instance
    def tasks( self, trfName = None, createIfNotExist = False ):
        return self._task.tasks( trfName, createIfNotExist )

    ## Getter function for TaskInfo instance.
    #  @param trfName The name of the task to retrieve. If @c None, all task including the top level
    #  task are retrieved. Defaults to @c None. 
    #  @param createIfNotExist Boolean variable to denote if a new TaskInfo instance should be created 
    #  if the task @em trfName is not found. Defaults to @c False.
    #  @exception KeyError is raised if the required task @em trfName cannot be found.
    #  @return TaskInfo instance
    def task( self, trfName = None, createIfNotExist = False ):
        try:
            return self.tasks( trfName, createIfNotExist ).next()
        except StopIteration:
            raise KeyError( "Task %s not found." % trfName )

    ## Add an AtlasErrorCodes.ErrorInfo instance to the current list of errors.
    #  @param error The AtlasErrorCodes.ErrorInfo instance to add.
    #  @param add Deprecated. Not in use.
    #  @remarks The list of error categories (from errors encountered by the associated transform) is updated as well.
    #  @return None
    def addError( self, error, add = None ):
        if error is None:
            return
        # set producer if not yet set
        producer = error.producer or self._trf or 'Unknown'
        error.producer = producer
        # Update list of errors for the task concerned.
        t = self.task( producer, createIfNotExist = True )
        t.errorLimits[ error.severity ] -= 1
        for e in t.errors:
            # error already exists: Merge error
            if e == error:
                e += error
                break
        else:
            if t.errorLimits[ error.severity ] < 0:
                t.errorLimits[ error.severity ] = 0
            else:
                t.errors.append( copy( error ) )
        self.__modified = True
        if error.code:
            self.addErrorCategory( error.category )

    ## Add an AtlasErrorCodes.ErrorInfo instance to the current list of validation-related errors.
    #  @param vError The AtlasErrorCodes.ErrorInfo instance to add.
    #  @remarks The list of error categories is @em not updated unlike the addError() function.
    #  @return None
    def addValidationError( self, vError ):
        if vError is None:
            return
        # set producer if not yet set
        producer = vError.producer or self._trf or 'Unknown'
        vError.producer = producer
        # Update list of vErrors for the task concerned.
        vErrors = self.task( producer, createIfNotExist = True ).validationErrors
        for e in vErrors:
            # vError already exists: Merge vError
            if e == vError:
                e += vError
                break
        else:
            vErrors.append( copy( vError ) )
        self.__modified = True

    ## Add a category of error to the list of error categories to a selected task.
    #  @param category The AtlasErrorCodes.ErrorCategory instance to add.
    #  @param trfName The name of the task to add this AtlasErrorCodes.ErrorCategory instance to.
    #  If @c None, AtlasErrorCodes.ErrorCategory from all sub-transform tasks are retrieved. Defaults to @c None.
    #  @return None
    def addErrorCategory( self, category, trfName = None):
        self.task( trfName ).addErrorCategory( category )
        self.__modified = True

    ## Adds the contents of the @em other report in one of three modes.
    #  @param report Instance of @em other report.
    #  @param mode 3 different modes are allowed:
    #  @li @c APPEND_TASK : @em Add the main task from the @em other report to the current list of subtasks.
    #  @li @c REPLACE     : @em Replace contents of current report with the contents of the @em other report.
    #  @li @c MERGE       : @em Merge contents of the @em other report to self.
    #  Defaults to @c APPEND_TASK.
    #  @exception AttributeError Mode provided not recognised.
    #  @exception KeyError The @em other report has missing @c athCode/athAcronym @c infos entries.
    #  @return None
    def addReport( self, report, mode = None ):
        APPEND_TASK, REPLACE, MERGE = 'APPEND_TASK', 'REPLACE', 'MERGE'
        if report == self:
            return
        if mode is None:
            mode = APPEND_TASK
        mode = mode.upper()
        if mode not in [ REPLACE, APPEND_TASK, MERGE ]:
            raise AttributeError( "'mode' parameter can only accepts the following values: [ 'REPLACE', 'APPEND_TASK', 'MERGE' ]" )
        # Add tasks
        if mode == APPEND_TASK:
            self._task.subTasks.append( report._task ) # append the top level task
            try:
                self._task.infos[ 'athCode' ] = report._task.infos[ 'athCode' ]
                self._task.infos[ 'athAcronym' ] = report._task.infos[ 'athAcronym' ]
            except KeyError:
                self.__modified = True
                #
                #
                # Commenting out on Nov 17, 2011 because it seems to
                # produce misleading reports when jobs fail.
                # Thomas Gadfort (tgadfort@bnl.gov)
                #
                #
                #raise KeyError( 'Attempting to add a report (task %s) with missing athCode/athAcronym.' % report._task.taskName() )
        elif mode == REPLACE:
            self._trf = report._trf
            self._trfVersion = report._trfVersion
            self._trfComment = report._trfComment
            self._task = report._task
        elif mode == MERGE:
            self._trf = report._trf
            self._trfVersion = report._trfVersion
            self._trfComment = report._trfComment or self._trfComment
            self._task += report._task
        self.__modified = True

    ## Access to private boolean variable that keeps track of whether the job report was modified.
    #  @see __modified
    #  @return Boolean value
    def modified(self):
        return self.__modified

    ## Generator for AtlasErrorCodes.ErrorCategory instances associated with a particular sub-transform task.
    #  @param trfName The name of the particular sub-transform task to retrieve the 
    #  AtlasErrorCodes.ErrorCategory instances from. If @c None, AtlasErrorCodes.ErrorCategory instances 
    #  associated with @em all sub-transform tasks will be retrieved. Defaults to @c None.
    #  @return AtlasErrorCodes.ErrorCategory instance
    def errorCategories( self, trfName = None ):        
        for t in self.tasks( trfName ):
            for errCat in t.errorCategories.values():
                yield errCat

    ## Generator for AtlasErrorCodes.ErrorInfo instances associated with a particular sub-transform task 
    #  and @em severity level.
    #  @param severity The @em severity level required. Defaults to a list that consists of 
    #  @em AtlasErrorCodes.FATAL and @em AtlasErrorCodes.ERROR error levels.
    #  @param trfName The name of the particular sub-transform task to retrieve the AtlasErrorCodes.ErrorInfo
    #  instance from. If @c None, AtlasErrorCodes.ErrorInfo instances associated with @em all sub-transform tasks
    #  will be retrieved. Defaults to @c None.
    #  @return AtlasErrorCodes.ErrorInfo instance
    def errors( self, severity = None, trfName = None ):
        if severity is None:
            severity = [ AtlasErrorCodes.FATAL, AtlasErrorCodes.ERROR ]
        elif not isinstance( severity, list ):
            severity = [ severity ]
        for t in self.tasks( trfName ):
            for e in t.errors:
                if e.severity in severity:
                    yield e

    ## Generator for validation errors associated with a particular sub-transform task.
    #  @param trfName The name of the particular sub-transform task to retrieve the validation errors from.
    #  If @c None, validation errors from @em all sub-transform tasks will be retrieved. Defaults to @c None.
    #  @remark Validation errors are AtlasErrorCodes.ErrorInfo instances.
    #  They are simply stored in a separate TaskInfo.validationErrors list.
    #  @return AtlasErrorCodes.ErrorCategory instance
    def validationErrors( self, trfName = None ):
        for t in self.tasks( trfName ):
            for e in t.validationErrors:
                yield e

    ## Count of number of AtlasErrorCodes.FATAL errors from a particular producer (or sub-transform task).
    #  @param producer The name of the particular sub-transform task to retrieve the count from.
    #  If @c None, the count will include @em all sub-transform tasks. Defaults to @c None.
    #  @return Integer
    def fatalCount( self, producer = None ):
        return len( list( itertools.chain( self.errors( AtlasErrorCodes.FATAL, producer ), self.validationErrors( producer ) ) ) )

    ## Count of number of AtlasErrorCodes.ERROR errors from a particular producer (or sub-transform task).
    #  @param producer The name of the particular sub-transform task to retrieve the count from.
    #  If @c None, the count will include @em all sub-transform tasks. Defaults to @c None.
    #  @return Integer
    def errorCount( self, producer = None ):
        return len( list( self.errors( AtlasErrorCodes.ERROR, producer ) ) )

    ## Count of number of AtlasErrorCodes.WARNING errors from a particular producer (or sub-transform task).
    #  @param producer The name of the particular sub-transform task to retrieve the count from.
    #  If @c None, the count will include @em all sub-transform tasks. Defaults to @c None.
    #  @return Integer
    def warningCount( self, producer = None ):
        return len( list( self.errors( AtlasErrorCodes.WARNING, producer ) ) )

    ## Retrieves the AtlasErrorCodes.ErrorInfo instance that corresponds to the first error of severity 
    #  AtlasErrorCodes.FATAL or AtlasErrorCodes.ERROR encountered. If the JobReport.__ignoreUnknown flag is set,
    #  the first AtlasErrorCodes.ErrorInfo instance that does not have an error code of @b TRF_UNKNOWN is returned.
    #  @remarks If there are @em any validation errors, the @c --ignoreerrors=True option (if used) will be
    #  ignored and the first validation error (also an AtlasErrorCodes.ErrorInfo instance) is returned instead.
    #  @return AtlasErrorCodes.ErrorInfo instance
    def getExitError( self ):
        for eInfo in self.validationErrors():
            self.validationSuccess = False
            return eInfo
        athenaOK = 'ATH_FAILURE' not in self.errorCategories()
        try:
            if athenaOK and self.info( 'athCode' ).contents() != '0':
                athenaOK = False
        except Exception:
            athenaOK = False
        for eInfo in self.errors():
            if self.__ignoreUnknown and eInfo.code == 69999 and athenaOK:
                continue
            return eInfo
        if athenaOK:
            return AtlasErrorCodes.ErrorInfo( acronym = 'OK' )
        return AtlasErrorCodes.ErrorInfo( acronym = 'ATH_FAILURE', severity = AtlasErrorCodes.FATAL )

    ## Retrieve the code associated with an AtlasErrorCodes.ErrorInfo instance.
    #  @param exitError The AtlasErrorCodes.ErrorInfo instance to retrieve the error code from. 
    #  If @c None, the JobReport.getExitError() function is used to retrieve the required
    #  AtlasErrorCodes.ErrorInfo instance. Defaults to @c None.
    #  @remarks The errorCode is coded as: 10000 * @b X + 100 * @b Y + 10 * @b Z.
    #  @return Integer
    def errorCode(self, exitError = None ):
        if exitError is None:
            error = self.getExitError()
        else:
            error = exitError
        return error.code

    ## Retrieve the error acronym associated with the exit error (an AtlasErrorCodes.ErrorInfo instance).
    #  @return String
    def errorAcronym(self):
        return self.getExitError().acronym

    ## A number based on the associated error code.
    #  @remarks If the JobReport.__ignoreAll boolean value is @c True, the exit code is @c 0.
    #  If the JobReport.__ignoreErrors boolean value is @c True and the @c athena.py successfully returns 
    #  without any validation errors, the exit code is @c 0.
    #  Otherwise, the exit code with simply be the @b Y portion of the error code.
    #  @return Integer
    def exitCode( self ):
        if self.__ignoreAll:
            return 0
        exitError = self.getExitError()
        if self.__ignoreErrors:
            try:
                athCode = self.info( 'athCode' ).contents()
            except Exception:
                pass
            else:
                # Return success if Athena returns success regardless of any errors detected
                if athCode == '0' and self.validationSuccess:
                    return 0
        err = self.errorCode( exitError )
        ex = (err % 10000) / 100
        # guard against zero exit code while having a non-zero error code
        if ex == 0 and err != 0:
            ex = err
            while ex > 255: ex /= 100
        return ex

    ## Append the transform/producer name to a given @em filename.
    #  @param filename The transform name is appended to the @em filename.
    #  @remarks If the transform name is not defined, the given @em filename is return unchanged.
    #  return String
    def producerFilename(self,filename):
        base,ext = os.path.splitext(filename)
        if self._trf is None:
            return filename
        producer = os.path.splitext( self._trf )[0]
        return base + '_' + producer + ext
    
    ## The current format of the metadata.xml file contain repeated elements (across entries).
    #  This script (courtesy of Pavel Nevski) extracts repeated metadata to be placed in a common area
    #  in the file so as to reduce the file size.
    #  @param filename Name of the file to apply the script to.
    #  @remarks This functions has been made a staticmethod.
    #  @return None
    @staticmethod
    def slimdownMetadata( filename ):
        temp1 = 'tempfile1_TOBEREMOVED_'
        temp2 = 'tempfile2_TOBEREMOVED_'
        try:
            # the actual execution
            subprocess.check_call( [ "slimmetadata", filename, temp1, temp2 ] )
        except subprocess.CalledProcessError as cpe:
            print ("Error slimming %s [%s]: %s" % ( filename, cpe.returncode, cpe.message ))
        else:
            print ("%s has been slimmed." % filename)
        # removing the temp files
        for f in [ temp1, temp2 ]:
            try:
                os.remove( f )
            except Exception:
                pass

    ## A wrapper method to allow for metadata to be written in two different formats
    #  depending on the boolean useNewMetadataFormat variable.
    #  @param writeFinalCopy Boolean value to pass to underlaying methods to determine
    #  if the final version of the metadata.xml file should be written. Defaults to @c True.
    #  @return None
    def writeMetaDataXML( self, writeFinalCopy = True ):
        if self.useNewMetadataFormat:
            self.writeMetaDataXML_new( writeFinalCopy )
        else:
            self.writeMetaDataXML_old( writeFinalCopy )
        self.__modified = False

    ## Write out metadata in the new format.
    #  @param writeFinalCopy Boolean value to determine if the final version of the
    #  metadata.xml file should be written. Defaults to @c True.
    #  @remarks This new format does away with the need for the JobReport.slimdownMetadata() method.
    #  @return None
    def writeMetaDataXML_new( self, writeFinalCopy = True ):
        headerLines = [ '<?xml version="1.0" encoding="UTF-8" standalone="no" ?>',
                        '<!-- ATLAS file meta-data catalog -->' ]
        xmlStr = os.linesep.join( headerLines )
        xmlStr += os.linesep + self._task.getXML()
        filename = self.producerFilename( JobReport.metadata_xml )
        print ("Writing new style %s file..." % filename)
        try:
            with open( filename, 'w' ) as metaFile:
                metaFile.write( xmlStr + os.linesep )
        except Exception as msg:
            print ("WARNING: Could not write metadata to file %s: %s" % ( filename, msg ))
            return
        if writeFinalCopy and self._trf:
            shutil.copyfile( filename, JobReport.metadata_xml )

    ## Write out metadata in the current format.
    #  @param writeFinalCopy Boolean value to determine if the final version of the
    #  metadata.xml file should be written. Defaults to @c True.
    #  @remarks The current format requires the JobReport.slimdownMetadata() method to be executed.
    #  @return None
    def writeMetaDataXML_old(self,writeFinalCopy=True):
        headerLines = [ '<?xml version="1.0" encoding="UTF-8" standalone="no" ?>' ,
                        '<!-- ATLAS file meta-data catalog -->' ,
                        '<!DOCTYPE POOLFILECATALOG SYSTEM "InMemory">' ]
        # gather all metadata names in the header
        metaNames = set()
        for f in self.outputFiles():
            metaNames.update( f.metaData().keys() )
        # add metadata names as META tags in xml header
        # add POOLFILECATALOG header
        xmlStr = os.linesep.join( headerLines )
        filesNode = XMLNode("POOLFILECATALOG")
        for m in metaNames:
            meta = XMLNode("META").setAttributes(name=m,type="string")
            filesNode.addContents( meta )
        for f in self.outputFiles():
            filesNode.addContents(f.xmlNode())
        xmlStr += os.linesep + filesNode.getXML()
        filename = self.producerFilename( JobReport.metadata_xml )
        print ("Writing %s file..." % filename)
        try:
            with open( filename, 'w' ) as metaFile:
                metaFile.write( xmlStr + os.linesep )
        except Exception as msg:
            print ("WARNING: Could not write metadata to file %s: %s" % ( filename, msg ))
            return
        # Temporary hack to remove redundant metadata.
        JobReport.slimdownMetadata( filename )
        if writeFinalCopy and self._trf:
            shutil.copyfile( filename, JobReport.metadata_xml )

    ## Write out the jobInfo.xml file.
    #  @param writeFinalCopy Boolean value to determine if the final version of the
    #  jobInfo.xml file should be written. Defaults to @c True.
    #  return None
    def writeJobInfoXML(self,writeFinalCopy=True):
        # header
        xmlStr = '<?xml version="1.0" encoding="UTF-8"?>'
        jobInfo = XMLNode("jtinfo")
        jobInfo.setAttribute('producer',self._trf)
        if self._trfVersion:
            jobInfo.setAttribute('producerversion',self._trfVersion)
        if self._trfComment:
            jobInfo.setAttribute('producercomment',self._trfComment)
        # command
        jobInfo.addContents( XMLNode("Command",self.command()) )
        # exit code
        jobInfo.addContents( XMLNode("ExitCode", self.exitCode()) )
        # error code
        err = self.getExitError()
        cat = err.category
        jobInfo.addContents( XMLNode("ErrorCode", cat.code) )
        jobInfo.addContents( XMLNode("ErrorAcronym", cat.acronym) )
        jobInfo.addContents( XMLNode("ErrorDescription", cat.description) )
        jobInfo.addContents( XMLNode("ErrorMessage", err.message) )
        jobInfo.addContents( XMLNode("ErrorSeverity", err.severity) )
        # add error category counts
        catFound = False
        for cat in self.errorCategories():
            catFound = True
            jobInfo.addContents( cat.xmlNode() )
        # Set OK
        if not catFound:
            jobInfo.addContents( AtlasErrorCodes.OK_CATEGORY.xmlNode() )
        # add fatal/error/warning counts
        jobInfo.addContents( XMLNode("warning",self.warningCount()) )
        jobInfo.addContents( XMLNode("error",self.errorCount()) )
        jobInfo.addContents( XMLNode("fatal",self.fatalCount()) )
        # add errors
        for error in itertools.chain( self.validationErrors(), self.errors() ):
            jobInfo.addContents( error.xmlNode() )
        # add any other info
        # combine the infos from the current task and all its subtasks
        tInfo = {}
        for info in self.infos():
            tInfo[ info.name() ] = info
        for info in tInfo.values():
            jobInfo.addContents( info.xmlNode() )
        # make XML string
        xmlStr += os.linesep + jobInfo.getXML()
        filename=self.producerFilename( JobReport.jobinfo_xml )
        print ("Writing %s file..." % filename)
        try:
            with open( filename, 'w' ) as jobFile:
                jobFile.write( xmlStr + os.linesep )
        except Exception as msg:
            print ("WARNING: Could not write job info to file %s: %s" % ( filename, msg ))
            return
        self.__modified = False
        if writeFinalCopy and self._trf:
            shutil.copyfile(filename,JobReport.jobinfo_xml)

    ## Write out the jobReport.txt.
    #  @param filenameBase Name of the file the text version of the Job Report will be based on.
    #  Defaults to @c JobReport.defaultFilenameBase.
    #  @param writeFinalCopy Boolean value to determine if the final version of the
    #  jobInfo.xml file should be written. Defaults to @c True.
    #  @return None
    def writeTXT(self,filenameBase=defaultFilenameBase,writeFinalCopy=True):
        if not filenameBase.endswith( '.txt' ):
            filenameBase += '.txt'
        filename = self.producerFilename( filenameBase )
        try:
            with open( filename, 'w' ) as txtFile:
                # always write full info to file
                self.dump( options = [], notOptions = [], output = txtFile )
        except Exception as msg:
            print ("WARNING: Could not write job report to file %s: %s" % ( filename, msg ))
            return
        self.__modified = False
        if writeFinalCopy and self._trf:
            shutil.copyfile( filename, filenameBase )

    ## Wrapper function to execute all JobReport persistency functions.
    #  @param writeFinalCopy Boolean value to be passed to the various JobReport persistency functions.
    #  Defaults to @c True.
    #  @return None
    def writeAll( self, writeFinalCopy = True ):
        self.writeGPickle( writeFinalCopy = writeFinalCopy )
        self.write( writeFinalCopy = writeFinalCopy )
        self.writeTXT( writeFinalCopy = writeFinalCopy )
        self.writeMetaDataXML( writeFinalCopy = writeFinalCopy )
        self.writeJobInfoXML( writeFinalCopy = writeFinalCopy )

    ## Dump the JobReport instance to a pickle file.
    #  @param filenameBase The name of the pickle file created will be based on @em filenameBase.
    #  Defaults to @c JobReport.defaultFilenameBase.
    #  @param writeFinalCopy Boolean value to determine if the final version of the
    #  jobReport.pickle file should be written. Defaults to @c True.
    #  @remarks The boolean return value signifies if the function was completed successfully.
    #  @return Boolean
    def write(self,filenameBase=defaultFilenameBase,writeFinalCopy=True):
        if not filenameBase.endswith('.pickle'):
            filenameBase += '.pickle'
        filename = self.producerFilename( filenameBase )
        try:
            with open( filename, 'w' ) as errorFile:
                pickle.dump( self, errorFile, 2 )
        except Exception as msg:
            print ("WARNING: Could not pickle job report to file %s: %s" % ( filename, msg ))
            return False
        self.__modified = False
        if writeFinalCopy and self._trf:
            shutil.copyfile( filename, filenameBase )
        return True

    ## A wrapper method to allow for jobReport.gpickle to be written in two different formats
    #  depending on the boolean useNewMetadataFormat variable.
    #  @return None
    def GPickleContents( self ):
        if self.useNewMetadataFormat:
            return self.GPickleContents_new()
        return self.GPickleContents_old()

    ## Generate the dictionary in the current format containing information required by
    #  the Production System. The sequence and hierarchy of sub-transform tasks are preserved.
    #  @remarks The structure of the dictionary to be returned has been pre-defined by
    #  the Production System.
    #  @return Dictionary
    def GPickleContents_new(self):
        # collect info about the error lines
        errorLineInfo={}
        for eltrf in itertools.chain( self.validationErrors(), self.errors() ):
            try:
                errorLineInfo[ eltrf.producer ].append( eltrf.xmlNode().getContents() )
            except KeyError:
                errorLineInfo[ eltrf.producer ] = [ eltrf.xmlNode().getContents() ]
        # not always the info is available 
        info_try_block={'AtlasRelease':'unknown',
                        'DBRelease':'unknown',
                        'Workdir':'unknown',
                        'RunDirUsedDisk':None,'RunDirAvailableDisk':None,
                        'athCode':None,'athAcronym':'unknown'}
        info_try_block_odict={'Machine':'unknown','Environment':'unknown'}
        for info in info_try_block.keys():
            try:
                info_try_block[info]=self.info( info ).getContents()[ info ]
            except Exception:
                pass
        for info in info_try_block_odict.keys():
            try:
                info_try_block_odict[info]=self.info( info ).getContents()
            except Exception:
                pass
        nevents = 0
        for oFile in self.outputFiles():
            evts = oFile.metaData( 'events' )
            if evts is not None and evts > nevents:
                nevents = evts
        trfExitCode = self.exitCode()
        trfCode = self.errorCode()
        errorCount = self.errorCount()
        fatalCount = self.fatalCount()
        
        trfAcronym = AtlasErrorCodes.getAcronym( trfCode )
        if trfExitCode == 0:
            if errorCount > 0:
                trfAcronym = AtlasErrorCodes.NEEDCHECK.acronym
            if fatalCount > 0:
                # There is a problem when trfExitCode=0 and there are fatal errors so reset to a TRF_EXE error
                print ("Warning: Trf exit code was 0, but fatal errors were detected. Resetting trf exit code to TRF_EXE.")
                trfAcronym = 'TRF_EXE'
                trfCode = AtlasErrorCodes.getCode( trfAcronym )

        # define the info structured as discussed with prodsys 

        self.gpickle_dict={'prodsys':{'athCode':info_try_block['athCode'],
                                      'athAcronym':info_try_block['athAcronym'],
                                      'trfExitCode':trfExitCode,
                                      'trfCode':trfCode,
                                      'trfAcronym':trfAcronym,
                                      'nevents':nevents,
                                      'more':{'Machine':info_try_block_odict['Machine']},
                                      'task':self._task.getDict()
                                     }, 
                           'more':{
                                      'Producer':self._trf,
                                      'ProducerVersion':self._trfVersion,
                                      'ProducerComment':self._trfComment, 
                                      'Command':self._task.command,  
                                      'Fatals':fatalCount, 
                                      'Errors':errorCount,
                                      'ErrorLines':errorLineInfo,
                                      'Warnings':self.warningCount(),
                                      'AtlasRelease':info_try_block['AtlasRelease'],
                                      'DBRelease':info_try_block['DBRelease'],
                                      'Workdir':info_try_block['Workdir'],
                                      'Environment':info_try_block_odict['Environment'],
                                      'RunDirUsedDisk':info_try_block['RunDirUsedDisk'],
                                      'RunDirAvailableDisk':info_try_block['RunDirAvailableDisk'] 
                                  }
                          }
        return self.gpickle_dict

    ## Generate the dictionary in a new format containing information required by the Production System.
    #  The sequence and hierarchy of sub-transform tasks are @em not preserved.
    #  @remarks The structure of the dictionary to be returned has been pre-defined by
    #  the Production System.
    #  @return Dictionary
    def GPickleContents_old(self):
        # collect limited info about the inputfiles
        ifiles=[]
        for i in self.inputFiles():
            try: 
                ifiles.append( { 'lfn' : os.path.basename( i.filename() ), 
                                 'GUID' : i.guid(), 
                                 'dataset' : i.metaData( 'dataset' ) } )
            except Exception:
                print ('JobReport collecting info input files:  problems with ', i)
        # collect info about outputfiles,
        # (metadata should be by file because of the combined trfs)  
        ofiles=[]                   
        for of in self.outputFiles():
            ofile={'lfn' : os.path.basename( of.filename() ),
                   'GUID' : of.guid(),
                   'dataset' : of.metaData( 'dataset' ),
                   'size' : of.metaData( 'size' ),
                   'events' : of.metaData( 'events' ),
                   'checkSum': None,
                   'more': {'metadata':of.metaData( moreOnly = True ) }
                   } 
            ofiles.append(ofile)
        # collect info about the error lines
        errorLineInfo={}
        for eltrf in itertools.chain( self.validationErrors(), self.errors() ):
            try:
                errorLineInfo[ eltrf.producer ].append( eltrf.xmlNode().getContents() )
            except KeyError:
                errorLineInfo[ eltrf.producer ] = [ eltrf.xmlNode().getContents() ]
        # not always the info is available 
        info_try_block={'AtlasRelease':'unknown',
                        'DBRelease':'unknown',
                        'Workdir':'unknown',
                        'RunDirUsedDisk':None,'RunDirAvailableDisk':None,
                        'athCode':None,'athAcronym':'unknown'}
        info_try_block_odict={'Machine':'unknown','Environment':'unknown'}
        for info in info_try_block.keys():
            try:
                info_try_block[info]=self.info( info ).getContents()[ info ]
            except Exception:
                pass
        for info in info_try_block_odict.keys():
            try:
                info_try_block_odict[info]=self.info( info ).getContents()
            except Exception:
                pass
        nevents = 0
        for oFile in self.outputFiles():
            evts = oFile.metaData( 'events' )
            if evts is not None and evts > nevents:
                nevents = evts
        trfExitCode = self.exitCode()
        trfCode = self.errorCode()
        errorCount = self.errorCount()
        fatalCount = self.fatalCount()
        # N.B. This is a bit dangerous - it's setting a different report in the gpickle
        # c.f. any other report file. It would be better to improve the exitCode and errorCode
        # directly when there are non-zero error and fatal counts. But it seems to be needed by
        # Tier-0 for now.
        trfAcronym = AtlasErrorCodes.getAcronym( trfCode )
        if trfExitCode == 0:
            if errorCount > 0:
                trfAcronym = AtlasErrorCodes.NEEDCHECK.acronym
            if fatalCount > 0:
                # There is a problem when trfExitCode=0 and there are fatal errors so reset to a TRF_EXE error
                print ("Warning: Trf exit code was 0, but fatal errors were detected. Resetting trf exit code to TRF_EXC.")
                trfAcronym = 'TRF_EXC'
                trfCode = AtlasErrorCodes.getCode( trfAcronym )

            
        # define the info structured as discussed with prodsys 
        self.gpickle_dict={'prodsys':{'athCode':info_try_block['athCode'],
                                      'athAcronym':info_try_block['athAcronym'],
                                      'trfExitCode':trfExitCode,
                                      'trfCode':trfCode,
                                      'trfAcronym':trfAcronym,
                                      'nevents':nevents,
                                      'jobInputs':ifiles,
                                      'jobOutputs':ofiles,
                                      'more':{'Machine':info_try_block_odict['Machine']}
                                     }, 
                           'more':{
                                      'Producer':self._trf,
                                      'ProducerVersion':self._trfVersion,
                                      'ProducerComment':self._trfComment, 
                                      'Command':self._task.command,  
                                      'Fatals':fatalCount, 
                                      'Errors':errorCount,
                                      'ErrorLines':errorLineInfo,
                                      'Warnings':self.warningCount(),
                                      'AtlasRelease':info_try_block['AtlasRelease'],
                                      'DBRelease':info_try_block['DBRelease'],
                                      'Workdir':info_try_block['Workdir'],
                                      'Environment':info_try_block_odict['Environment'],
                                      'RunDirUsedDisk':info_try_block['RunDirUsedDisk'],
                                      'RunDirAvailableDisk':info_try_block['RunDirAvailableDisk'] 
                                  }
                          } 
        return self.gpickle_dict

    ## Write out jobReport.gpickle file.
    #  @param filenameBase The name of the file to be created is based on @em filenameBase.
    #  Defaults to JobReport.defaultFilenameBase
    #  @param writeFinalCopy Boolean value to determine if the final version of the
    #  jobReport.gpickle file should be written. Defaults to @c True.
    #  @remarks The return boolean value indicates if the creation of the @em gpickle file was successful.
    #  return Boolean 
    def writeGPickle(self,filenameBase=defaultFilenameBase,writeFinalCopy=True):
        if not filenameBase.endswith('.gpickle'):
            filenameBase += '.gpickle'
        filename = self.producerFilename( filenameBase )
        try:
            with open( filename, 'w' ) as errorFile:
                pickle.dump( self.GPickleContents(), errorFile, 0 ) # text format
        except Exception as msg:
            print ("WARNING: Could not write gpickle job report to file %s: %s" % ( filename, msg ))
            return False
        self.__modified = False
        if writeFinalCopy and self._trf:
            shutil.copyfile(filename,filenameBase)
        return True

    ## Setter function for the private JobReport.__ignoreUnknown property.
    #  @param ignoreUnknown Boolean value to set the private variable to.
    #  @return None
    def setIgnoreUnknown( self, ignoreUnknown ):
        """Mask all TRF_UNKNOWN errors. See getExitCode()."""
        self.__ignoreUnknown = ignoreUnknown

    ## Setter function for the private JobReport.__ignoreAll property.
    #  @param ignoreAll Boolean value to set the private variable to.
    #  @return None
    def setIgnoreAll( self, ignoreAll ):
        """Force transform to return successful exit code [0]
        regardless of the actual exit codes. See exitCode()"""
        self.__ignoreAll = ignoreAll

    ## Setter function for the private JobReport.__ignoreErrors property.
    #  @param ignoreErrors Boolean value to set the private variable to.
    #  @return None
    def setIgnoreErrors( self, ignoreErrors ):
        """Force transform to return successful exit code [0] if athena returns successful [0]
        regardless of the actual exit codes. See exitCode()"""
        self.__ignoreErrors = ignoreErrors

