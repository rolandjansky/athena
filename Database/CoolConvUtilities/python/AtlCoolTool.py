# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from PyCool import cool
from re import match
import time,calendar
import CoolConvUtilities.AtlCoolLib as AtlCoolLib

from zlib import crc32

def expandConnectString( connectString ):
    """
    Expands a connect string.
    
    This expansion can occur when a connect string without a containing
    '://' or not in the format 'alias/DBNAME' is specified. In this case
    the string is interpreted as a sqlite file name and rewritten to a
    COOL compliant format:
    
        TEST.db  --> 'sqlite://;schema=TEST.db;dbname=TEST'
        TEST     --> 'sqlite://;schema=TEST;dbname=TEST'
    
    The filename can have a '.db' suffix which will be stripped for the
    'dbname' part of the connect string. Other suffixes will not be recognized.

    Actually for ATLAS, a simple string without / is interpreted as a database
    instance name within the SQLite file mycool.db, for consistency with
    AtlCoolLib behavior
    """
    if connectString.find( '://' ) == -1:
        if connectString.endswith( '.db' ):
            base = connectString[:-3]
        elif match("[a-zA-Z0-9_-]+/[A-Z0-9_-]{1,8}",connectString):
            # it is an alias/DBNAME
            return connectString
        else:
            base = connectString
        #return ( 'sqlite://;schema=%s;dbname=%s' ) % ( connectString, base )
        return ('sqlite://;schema=mycool.db;dbname=%s' % base)
    else:
        return connectString


def connect( connectString, verbose = False):
    """
    Connects to the given database and returns a tuple
       database, connectString
    where 'database' is a cool.IDatabase object and 'connectString' is the
    possibly expanded connectString that 'database' is based on.
    
    This expansion can occur when a connect string without a containing
    '://' is specified. In this case the string is interpreted as a sqlite
    file name and rewritten to a RAL compliant format:
    
        TEST.db  --> 'sqlite://;schema=TEST.db;dbname=TEST'
        TEST     --> 'sqlite://;schema=TEST;dbname=TEST'
    
    The filename can have a '.db' suffix which will be stripped for the
    'dbname' part of the connect string. Other suffixes will not be recognized.
    
    Note that the COOL database inside the file must have the same name as the
    base of the filename for this shortcut to work. Storing a COOL database
    MYTEST in a file mytest.db will not work.
    
    Set verbose to True to obtain an error print out.
    """
    connectString = expandConnectString( connectString )
    forceoracle=True
    debug=False
    if (';readoracle' in connectString):
        # new default is forceOracle - but giving keyword explicitly
        # forces debug printout
        connectString=connectString.replace(';readoracle','')
        debug=True
    if (';readsqlite' in connectString):
        connectString=connectString.replace(';readsqlite','')
        debug=True
        forceoracle=False
    try:
        dbSvc = cool.DatabaseSvcFactory.databaseService()
        readonly=True
        # frontier/logical cannot do update connections - only real dbs
        if ('oracle' in connectString or 'mysql' in connectString or 'sqlite' in connectString): readonly=False
        db=AtlCoolLib.indirectOpen(connectString,readonly,forceoracle,debug)
    except Exception as e:
        if 'The database does not exist' in str(e):
            print ("Creating new database")
            db = dbSvc.createDatabase( connectString )
        else:
            if verbose: print ('Error while connecting:', str(e))
            db = None
    return db, connectString


typeSizes = { cool.StorageType.Bool : 1,
              cool.StorageType.UChar : 1,
              cool.StorageType.Int16 : 2,
              cool.StorageType.UInt16 : 2,
              cool.StorageType.Int32 : 4,
              cool.StorageType.UInt32 : 4,
              cool.StorageType.UInt63 : 8,
              cool.StorageType.Int64 : 8,
              cool.StorageType.Float : 4,
              cool.StorageType.Double : 8,
              cool.StorageType.String255 : 255,
              cool.StorageType.String4k : 4000,
              cool.StorageType.String64k : 65535,
              cool.StorageType.String16M : 16777215,
              cool.StorageType.Blob64k : 65535,
              cool.StorageType.Blob16M : 16777215,
            }

def byteSize( spec ):
    """
    Determines the (effective) byte size of an AttributeListSpecification.
    This does not account for any extra size for the specification's own
    structure but just the byte sizes of the attributes. In other words an
    AttributeListSpecification with a single 'int' fields will report a byte
    size of 4.
    """
    size = 0
    for i in spec:
        size += typeSizes[i.storageType().id()]
    return size



class Info(dict):
    """
    This class extends a dictionary with a format string for its representation.
    It is used as the return value for the inspection commands, such that one
    can both query for the inspected object's information:
    
      res = tool.ls( '/a' )
      print (res['name'])
      
    as well as obtain a standard printout of the information:
    
      print (res)
    
    """
    def __init__( self, format = None ):
        super( Info, self ).__init__()
        self.format = format
    
    def __str__( self ):
        if self.format is None:
            return super( Info, self ).__str__()
        else:
            return self.format % self


class InfoList(list):
    """
    This class extends a list with a custom string representation.
    """
    def __str__( self ):
        return '\n'.join( [ str(i) for i in self ] )
        
    def __repr__( self ):
        return str( self )


class AtlCoolTool:

    def __init__( self, database ):
        """
        Initialize the object to a given database.
        
        If 'database' is of type string, it must be a RAL compliant connect 
        string or an sqlite filename following the requirements described 
        in CoolTool.connect.
        
        Otherwise it must be a valid cool.IDatabase object.
        """
        if type( database ) == str:
            self.db, connectString = connect( database )
        else:
            self.db = database
        self.curdir='/'
        self.curtag=""
        self.curchan=-1
        self.currunlumi=[cool.ValidityKeyMin,cool.ValidityKeyMax]
        self.curtimes=[cool.ValidityKeyMin,cool.ValidityKeyMax]
        self.useoracle=False

    
    def connectString( self ):
        return self.db.databaseId()

    def cd(self,node='/'):
        if (node==".."):
            node=self.curdir[:self.curdir.rfind('/')]
            if (node==""): node="/"
        else:
          if not node.startswith('/'): node=self.curdir+'/'+node
          if (node.startswith('//')): node=node[1:]
        if self.db.existsFolder(node) or self.db.existsFolderSet(node):
            self.curdir=node
        else:
            raise Exception('Node %s does not exist' % node)


    def pwd(self):
        res=InfoList()
        res.append('Current directory: %s' % self.curdir)
        return res


    def usetag(self,tag=""):
        res=InfoList()
        try:
            itag=int(tag)
            if self.db.existsFolder(self.curdir):
                f=self.db.getFolder(self.curdir)
            else:
                f=self.db.getFolderSet(self.curdir)
            taglist=f.listTags()
            # sort the tags in alphabetical order - convert to a list
            staglist=[]
            for tag in taglist:
                staglist+=[str(tag)]
            staglist.sort()
            if (itag<len(staglist)):
                self.curtag=staglist[itag]
            else:
                self.curtag=staglist[-1]
        except ValueError:
            if (tag!="HEAD"):
                self.curtag=tag.strip()
            else:
                self.curtag=""
        res.append('Changed current tag selection to %s' % self.curtag)
        return res

    def usechan(self,chan="ALL"):
        if (chan=="ALL" or chan=="all"):
            ichan=-1
        else:
            ichan=int(chan)
        res=InfoList()
        self.curchan=ichan
        if (ichan!=-1):
            res.append('Changed current channel selection to %i' % self.curchan)
        else:
            res.append('Changed current channel selection to ALL')
        return res
    
    def pws(self):
        res=InfoList()
        res.append('Current tag selection: %s' % self.curtag)
        if (self.curchan==-1):
            res.append('Current channel selection: ALL')
        else:
            res.append('Current channel selection: %i' % self.curchan)
        return res

    def rmdir(self, node):
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        chk=raw_input('Delete folder(set) '+node+' ? (y/n)')
        if not (chk=='y' or chk=='Y'):
            res.append('Deletion aborted!')
            return res
        if self.db.existsFolder(node) or self.db.existsFolderSet(node):
            try:
                retcode=self.db.dropNode(node)
                res.append('Folder dropped with return code '+str(retcode))
            except Exception as e:
                print (e)
                res.append('Could not drop folder')
        else:
            res.append('Folder '+node+' does not exist')
        return res

    def less( self, node, header = False, more=False ):
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res = InfoList()
        if self.db.existsFolder( node ):
            f = self.db.getFolder( node )
            fdesc=f.description()
            if (self.curchan==-1):
                chansel=cool.ChannelSelection.all()
            else:
                chansel=cool.ChannelSelection(self.curchan)
                res.append('Using channel: %i' % self.curchan)
            istime=(fdesc.find('<timeStamp>time')!=-1)
            if istime:
                limmin=self.curtimes[0]
                limmax=self.curtimes[1]
            else:
                limmin=self.currunlumi[0]
                limmax=self.currunlumi[1]
            if (limmin!=cool.ValidityKeyMin or limmax!=cool.ValidityKeyMax):
                res.append('Using rawIOV range [%i,%i]' % (limmin,limmax))
            if (self.curtag==''):
                restag=""
            else:
                if not self.curtag in f.listTags():
                    # tag is not defined here, try hierarchical tag
                    try:
                        restag=f.resolveTag(self.curtag)
                        res.append('Using tag selection: %s resolved to %s' % (self.curtag,restag))
                    except Exception:
                        res.append('Tag %s not defined here' % self.curtag)
                        return res
                else:
                    restag=self.curtag
                    res.append('Using tag selection: %s' % self.curtag)
            coolvec=(fdesc.find('CondAttrListVec')>=0 and fdesc.find('coracool')<0)
            if coolvec: print ("Folder has CoolVector payload")
            objs = f.browseObjects( limmin,limmax,chansel,restag )
            while objs.goToNext():
                if (more):
                    obj=objs.currentRef()
                    i=self.timeRep(obj.since(),istime)+" - "+self.timeRep(obj.until(),istime,True)+" ("+str(obj.channelId())+")"
                    if coolvec:
                        j=0
                        pitr=obj.payloadIterator()
                        while pitr.goToNext():
                            pobj=pitr.currentRef()
                            i+='\nVector element %i:' % j
                            i+=self.payloadRep(pobj,istime)
                            j+=1
                    else:
                        i+=self.payloadRep(obj.payload(),istime)
                else:
                    i = Info( '  %(str)s' )
                    i['str'] = str(objs.currentRef())
                res.append( i )
        else:
            raise Exception( "Node '%s' is not a folder" % node )

        return res

    def more( self, node, header=False ):
        return self.less(node,header,True)

    def timeRep(self,value,istime,isend=False):
        "Print COOL Validity Key in run/LB or time notation"
        trail=']'
        if isend: trail=')'
        if istime:
            if (value==cool.ValidityKeyMin):
                return "ValidityKeyMin"
            elif (cool.ValidityKeyMax-value<1000000000):
                return "ValidityKeyMax"
            else:
                stime=int(value/1000000000)
                return time.asctime(time.gmtime(stime))+" UTC"
        else:
            return "[%i,%i%s" % (value >> 32, value & 0xFFFFFFFF,trail)

    def payloadRep(self,payload,istime):
        "Pretty-print the payload of an object - helper function for more cmd"
        spec=payload.specification()
        i=""
        for idx in range(spec.size()):
            if (idx>0): i+=","
            typename=spec[idx].storageType().name()
            i+= " ["+spec[idx].name() + " (" + typename + ") : "
            if (typename.startswith("Blob")):
                blob=payload[idx]
                i+= "size=%i,chk=%i" % (blob.size(),crc32(blob.read()))
            else:
                i+= str(payload[idx])
            i+="]"
        return i

    def defaultFolderInfo( self ):
        res = Info( '  %(name)-16s  %(description)-16s'
                    '  %(cardinality)-8s  %(size)-12s' )
        res['name'] = 'Name'
        res['description'] = 'Description'
        res['cardinality'] = 'Count'
        res['size'] = 'Size'
        return res


    def folderInfo( self, node, doCount=False ):
        f = self.db.getFolder( node )
        res = self.defaultFolderInfo()
        res['name'] = node
        res['description'] = f.description()
        if (doCount):
            res['cardinality'] = f.countObjects( cool.ValidityKeyMin,
                                             cool.ValidityKeyMax,
                                             cool.ChannelSelection.all() )
            res['size'] = res['cardinality'] * byteSize( f.payloadSpecification() )
        else:
            res['cardinality']='-'
            res['size']='-'
        return res
        

    def defaultFolderSetInfo( self ):
        res = Info( '  %(name)-16s  %(description)-16s' )
        res['name'] = 'Name'
        res['description'] = 'Description'
        return res


    def folderSetInfo( self, node ):
        f = self.db.getFolderSet( node )
        res = self.defaultFolderSetInfo()
        res['name'] = node
        res['description'] = f.description()
        return res

        
    def ls( self, node = '/', header = False, doCount=False ):
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        
        res = InfoList()
        if self.db.existsFolderSet( node ):
            fs = self.db.getFolderSet( node )
            
            if header and fs.listFolderSets():
                res.append( self.defaultFolderSetInfo() )
            for n in fs.listFolderSets():
                res.append( self.folderSetInfo( n ) )
            
            if header and fs.listFolders():
                res.append( self.defaultFolderInfo() )
            for n in fs.listFolders():
                res.append( self.folderInfo( n, doCount ) )
        else:
            raise Exception( "Node '%s' is not a folderset" % node )
        return res

    def lstags(self,node='/',pattern=""):
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        f=None
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
            res.append('Listing tags for folder '+node)
        elif self.db.existsFolderSet(node):
            f=self.db.getFolderSet(node)
            res.append('Listing tags for folderset '+node)
        # Print obsolete tags if pattern is "showObsolete" 
        showObsolete=False
        if pattern=="showObsolete":
            showObsolete=True
            pattern=""
        if pattern!="":
            res.append('Only listing tags containing: "%s"' % pattern)
        if f is not None:
            staglist=[]
            taglist=f.listTags()
            # sort the tags in alphabetical order - convert to a list
            for tag in taglist:
                staglist+=[str(tag)]
            staglist.sort()
            for tag in staglist:
                # check match against pattern if defined
                if pattern!="":
                    if tag.find(pattern)<0: continue
                else:
                    # Filter Obsolete tags
                    if (not showObsolete) and (f.tagDescription(tag).find('OBSOLETE')>-1): continue
                # check the lock status of each tag
                locked=f.tagLockStatus(tag)
                if (locked==cool.HvsTagLock.UNLOCKED):
                    strlock='unlocked'
                elif (locked==cool.HvsTagLock.PARTIALLYLOCKED):
                    strlock='partially locked'
                else:
                    strlock='locked'
                desc=f.tagDescription(tag)
                res.append('%s (%s) [%s]' % (tag,strlock,desc))
        else:
            raise Exception("Node '%s' does not exist" % node)
        return res

    def listchans(self,node='/'):
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        f=None
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
            res.append('Listing channel IDs, names, descriptions for folder'+node)
            chanlist=f.listChannels()
            res.append('Total number of channels defined: %i' % len(chanlist))
            res.append('Seq ChannelNum ChannelName Desc')
            iseq=0
            for ichan in chanlist:
                try:
                    channame=f.channelName(ichan);
                except:
                    channame='<none>'
                try:
                    chandesc=f.channelDescription(ichan);
                except:
                    chandesc='<none>'
                res.append('%i: %i %s %s' % (iseq,ichan,channame,chandesc))
                iseq+=1
        else:
            raise Exception("Node '%s' is not a folder" % node)
        return res

    def listinfo(self,node='/'):
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
            if (f.versioningMode()==cool.FolderVersioning.MULTI_VERSION):
                vstr='multi'
            else:
                vstr='single'
            res.append('Specification for %s-version folder %s' % (vstr,node))
            fspec=f.payloadSpecification()
            for field in fspec:
                res.append(' %s (%s)' % ( field.name(),field.storageType().name()))
            res.append('Description: %s' % f.description())
        else:
            res.append('Node %s is not a folder' % node)
        return res

    def settag(self,argumentString):
        args=argumentString.split()
        if len(args)<3:
            raise Exception('Usage: settag <folder> <foldertag> <parenttag>')
        node=args[0]
        tag1=args[1]
        tag2=args[2]
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        f=None
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
            res.append('Setting tag '+tag1+' for folder '+node+ ' to parent tag '+tag2)
        elif self.db.existsFolderSet(node):
            f=self.db.getFolderSet(node)
            res.append('Setting tag '+tag1+' for folderset '+node+' to parent tag '+tag2)
        if f is not None:
            # check tag exists, confirm action if not
            if tag1 not in f.listTags():
                print ("WARNING: Tag %s does not exist in node %s" % (tag1,node))
                chk=raw_input("Do you want to proceed anyway (y/n)")
                if (chk.upper()!="Y"):
                    raise Exception('ABORTED - Tag %s does not exist' % tag1)
            try:
                f.createTagRelation(tag2,tag1)
            except Exception as e:
                print (e)
                res.append('createTagRelation fails')
        else:
            raise Exception("Node '%s' does not exist" % node)
        return res

    def setchan(self,argumentString):
        args=argumentString.split()
        if len(args)<3:
            raise Exception('Usage: setchan <folder> <chanID> <chanName> {<chanDescr>}')
        node=args[0]
        chanid=int(args[1])
        channame=args[2]
        if (len(args)>3):
            chandesc=args[3]
        else:
            chandesc=""
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        f=None
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
            if (f.existsChannel(chanid)):
                res.append('Resetting data for existing channel %i %s %s' % (chanid,channame,chandesc))
                f.setChannelName(chanid,channame)
                if (chandesc!=""):
                    f.setChannelDescription(chanid,chandesc)
            else:
                # have to create channel
                res.append('Creating data for channel %i %s %s' % (chanid ,channame,chandesc))
                f.createChannel(chanid,channame,chandesc)
            res.append('Done')
        else:
            res.append('Node %s is not a folder' % node)
        return res

    def settginfo(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            raise Exception('Usage: settginfo <folder> <tag> <description>')
        node=args[0]
        tag=args[1]
        desc=' '.join(args[2:])
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        f=None
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
        elif self.db.existsFolderSet(node):
            f=self.db.getFolderSet(node)
        else:
            res.append('Node %s does not exist' % node)
            return res
        res.append('Setting description for tag %s to %s' % (tag,desc))
        f.setTagDescription(tag,desc)
        return res

    def userunlumi(self,argumentString):
        args=argumentString.split()
        if len(args)==1:
            run1=int(args[0])
            run2=run1
            lb1=0
            lb2=0xFFFFFFFF
        elif len(args)<4:
            raise Exception('Usage: userunlumi <run1> {<LB1> <run2> <LB2>}')
        else:
            run1=int(args[0])
            lb1=int(args[1])
            run2=int(args[2])
            lb2=int(args[3])
        self.currunlumi[0]=(run1 << 32)+lb1
        self.currunlumi[1]=(run2 << 32)+lb2

    def usetimes(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            raise Exception('Usage: usetimes <t1> <t2>')
        for i in range(0,2):
            try:
                self.curtimes[i]=int(args[i])
            except ValueError:
                try:
                    ts=time.strptime(args[i]+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
                    self.curtimes[i]=int(calendar.timegm(ts))*1000000000
                except ValueError:
                    print ("ERROR in time specification, use e.g. 2007-05-25:14:01:00")
                    

    def rmtag(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            raise Exception('Usage: rmtag <folder> <parenttag>')
        node=args[0]
        tag1=args[1]
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        f=None
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
            res.append('Removing association of tag '+tag1+' to folder '+node)
        elif self.db.existsFolderSet(node):
            f=self.db.getFolderSet(node)
            res.append('Removing association of tag '+tag1+' to folderset '+node)
        if f is not None:
            # tag1 can be a leaf tag in the folder, or a parent tag
            taglist=f.listTags()
            if (tag1 in taglist):
                # tag1 is in the folder itself
                try:
                    f.deleteTag(tag1)
                    res.append('Removal of leaf tag '+tag1+' succeeded')
                except Exception as e:
                    print (e)
                    res.append('deleteTag fails')
            else:
                # tag1 is a tag relation to a parent
                try:
                    f.deleteTagRelation(tag1)
                except Exception as e:
                    print (e)
                    res.append('deleteTagRelation fails')
        else:
            raise Exception("Node '%s' does not exist" % node)
        return res

    def headtag(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            raise Exception('Usage: headtag <folder> <tag>')
        node=args[0]
        tag=args[1]
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        f=None
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
            res.append('Applying tag '+tag+' to HEAD of folder '+node)
            try:
                f.tagCurrentHead(tag,"AtlCoolConsole tag")
            except Exception as e:
                print (e)
                res.append('tagCurrentHead fails')
        else:
            raise Exception("Node '%s' does not exist" % node)
        return res

    def locktag(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            raise Exception('Usage: locktag <folder> <tag> {action=l|p|u|r}')
        node=args[0]
        tag=args[1]
        action='L'
        if len(args)>2:
            action=args[2].upper()
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        f=None
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
            if action=='R':
                action=='L'
                res.append('Folder %s is a leaf node, ignoring recursion' % node)
        elif self.db.existsFolderSet(node):
            f=self.db.getFolderSet(node)
        else:
            raise Exception("Node '%s' does not exist" % node)
        if tag not in f.listTags():
            raise Exception('Tag %s does not exist in node %s' % (tag,node))
        state=cool.HvsTagLock.LOCKED
        if (action=='U'): state=cool.HvsTagLock.UNLOCKED
        if (action=='P'): state=cool.HvsTagLock.PARTIALLYLOCKED
        if (action=='R'):
            # find all the tags below this one and lock them too
            nmod=0
            nodelist=self.db.listAllNodes()
            for inode in nodelist:
                if (inode[:len(node)]==node):
                    # look only in multiversion folders
                    multi=True
                    if (self.db.existsFolder(inode)):
                        subf=self.db.getFolder(inode)
                        if (subf.versioningMode==cool.FolderVersioning.SINGLE_VERSION): multi=False
                    else:
                        subf=self.db.getFolderSet(inode)
                    if (multi):
                        try:
                            rtag=subf.resolveTag(tag)
                            # skip tags which are UPD1 or UPD2 - leave unlocked
                            if (rtag.find('UPD1')==-1 and rtag.find('UPD2')==-1):
                                curstate=subf.tagLockStatus(rtag)
                                if (curstate!=state):
                                    subf.setTagLockStatus(rtag,state)
                                    nmod+=1
                                    res.append('Set lock for tag %s to %i' % (rtag,subf.tagLockStatus(rtag)))
                                else:
                                    res.append('Tag %s already at state %i' % (rtag,curstate))
                            else:
                                res.append('Skip tag %s due to UPD mode' % rtag)
                        except:
                            pass
            res.append('Changed state of %i tags' % nmod)
        else:
            # non-recursive tag
            f.setTagLockStatus(tag,state)
            res.append('Set lock for tag %s to %i' % (tag,f.tagLockStatus(tag)))
        return res

            
    def tracetags(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            raise Exception('Usage: tracetags <folder> <foldertag>')
        node=args[0]
        tag=args[1]
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        # node must be a folderset, not a folder for this command to make sense
        if self.db.existsFolderSet(node):
            f=self.db.getFolderSet(node)
        else:
            raise Exception("Node '%s' does not exist" % node)
        # check the given tag is actually defined in this folder
        taglist=f.listTags()
        if tag not in taglist:
            raise Exception("Tag '%s' does not exist in node '%s'" % (tag,node))
        # find all folder(sets) under this node
        res.append('Searching under %s for tags referenced by %s' % (node,tag))
        nodelist=self.db.listAllNodes()
        for inode in nodelist:
            inode = str(inode)
            if (inode[:len(node)]==node):
                # look only in multiversion folders
                multi=True
                if (self.db.existsFolder(inode)):
                    subf=self.db.getFolder(inode)
                    if (subf.versioningMode==cool.FolderVersioning.SINGLE_VERSION): multi=False
                else:
                    subf=self.db.getFolderSet(inode)
                if (multi):
                    try:
                        rtag=subf.resolveTag(tag)
                    except:
                        rtag='<nothing>'
                    res.append('Folder %s : tag %s' % (inode,rtag))
        return res

    def clonetag(self,argumentString):
        args=argumentString.split()
        if len(args)<3:
            raise Exception('Usage: clonetag <folder> <sourcetag> <desttag>')
        node=args[0]
        tag1=args[1]
        tag2=args[2]
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        res=InfoList()
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
            # do not allow cloning to existing UPDx tags
            if (tag2.find('UPD')>=0 and tag2 in f.listTags()):
                raise Exception('Cannot clone to existing UPDx destination tag %s' % tag2)
            res.append('Cloning tag '+tag1+' for folder '+node+ ' to dest tag '+tag2)
            try:
                f.cloneTagAsUserTag(tag1,tag2)
                res.append('All done')
            except Exception as e:
                res.append('cloneTagAsUserTag failed with error %s' % e)
        elif self.db.existsFolderSet(node) :  #clone tags in folderset mantaining tags hierarchy
            addedtag = []
            res.append('Cloning tag '+tag1+' for folder '+node+ ' to dest tag '+tag2) 
            nodelist=self.db.listAllNodes()
            for inode in nodelist:
                if (inode[:len(node)+1]==node+'/') or (node=='/' and inode[:len(node)]==node):
                    if self.db.existsFolder(inode):
                        finode=inode[:inode.rfind('/')]
                        if finode==('') : finode='/'
                        while finode!=node:
                            inode=finode
                            finode=inode[:inode.rfind('/')]
                            if finode==('') : finode='/'
                        try:
                            subf=self.db.getFolder(inode)
                        except:
                            subf=self.db.getFolderSet(inode)
                        try:
                            rtag=subf.resolveTag(tag1)
                            try:
                                if  rtag not in addedtag :
                                    addedtag.append(rtag)
                                    subf.createTagRelation(tag2,rtag)
                            except Exception as e:
                                print (e)
                                res.append('createTagRelation failed with error %s' %e)
                        except:
                            res.append('Folder %s : no tag selected' % inode)
            res.append('All done')
        else:
            raise Exception("Node '%s' does not exist" % node)
        return res


    def setdesc(self,argumentString):
        args=argumentString.split()
        if len(args)<1:
            raise Exception('Usage: setdesc <folder> {<descrption>}')
        node=args[0]
        if (len(args)>1):
            desc=args[1]
        else:
            desc=""
        if node is None or node == '' or node == '.':
            node=self.curdir
        elif not node.startswith('/'): node=self.curdir+'/'+node
        if (node.startswith('//')): node=node[1:]
        # special strings to expand
        if desc=="ATTRTIME" :
            # code for an inline CondAttrListCollection stored by timestamp
            desc='<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>'
        elif desc=="ATTRRUN" :
            # code for a inline AthenaAttributeList stored by run-event
            desc='<timeStamp>run-event</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>'
        elif desc=="ATTRCOLLTIME" :
            # code for an inline CondAttrListCollection stored by timestamp
            desc='<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'
        elif desc=="ATTRCOLLRUN" :
            # code for an inline CondAttrListCollection stored by run-event
            desc='<timeStamp>run-event</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'

        res=InfoList()
        f=None
        if self.db.existsFolder(node):
            f=self.db.getFolder(node)
            res.append('Current folder description: %s' % f.description())
            if (desc!=""):
                res.append('Set folder description to: %s' % desc)
                try:
                    f.setDescription(desc)
                except Exception as e:
                    print (e)
                    res.append('Set folder description failed')
        else:
            raise Exception("Node '%s' does not exist" % node)
        return res

    def __str__( self ):
        if self.db is not None:
            return "Connected to '%s'" % self.db.databaseId()
        else:
            return "Not connected"
