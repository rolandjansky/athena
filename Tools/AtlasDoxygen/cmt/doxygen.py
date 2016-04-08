# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



# -*- coding: iso-8859-1 -*-

import os
import sys
import string
import popen2
import stat
import re
import time
import commands
import select

from os.path import join

############################################################################
## Error and success return function
def S_ERROR( sMessage = '' ):
  return { 'Status': ERROR, 'OK' : 0, 'Message' : sMessage  }
    
def S_OK( sValue = None, sPname = 'Value' ):
  dResult = { 'Status': 'OK', 'OK' : 1 }
  if sValue is not None:
    dResult[ sPname ] = sValue
  return dResult
  
############################################################################  
class SubprocessExecuter:

    def __init__( self, iTimeout = False ):
        self.changeTimeout( iTimeout )
        self.iBufferLimit = 5242880 # 5MB limit for data

    def changeTimeout( self, iTimeout ):
        self.iTimeout = iTimeout
        if self.iTimeout == 0:
            self.iTimeout = False
        
    def __readFromPipe( self, oPipe, iBaseLength = 0 ):
        sData = ""
        iMaxSliceLength = 8192
        iLastSliceLength = 8192
        
        while iLastSliceLength == iMaxSliceLength:
            sReadBuffer = os.read( oPipe, iMaxSliceLength )
            iLastSliceLength = len( sReadBuffer )
            sData += sReadBuffer
            if len( sData ) + iBaseLength > self.iBufferLimit:
                dRetVal = S_ERROR( "Reached maximum allowed length (%d bytes) for called function return value" % self.iBufferLimit )
                dRetVal[ 'ReadData' ] = sData
                return dRetVal
            
        return S_OK( sData )
                    
    def __executePythonFunction( self, oFunc, stArgs, oWritePipe ):
        try:
            os.write( oWritePipe, "%s\n" % str( S_OK( oFunc( *stArgs ) ) ) )
        except Exception, v:
            os.write( oWritePipe, "%s\n" % str( S_ERROR( str( v ) ) ) )
        try:
            os.close( oWritePipe )
        finally:
            os._exit(0)
    
    def __selectFD( self, lR, iTimeout = False ):
        if self.iTimeout and not iTimeout:
            iTimeout = self.iTimeout
        if not iTimeout: 
            return select.select( lR , [], [] )[0]
        else:
            return select.select( lR , [], [], iTimeout )[0]
    
    def pythonCall( self, oFunction, stArgs ):
        oReadPipe, oWritePipe = os.pipe()
        iPid = os.fork()
        if iPid == 0:
            os.close( oReadPipe )
            self.__executePythonFunction( oFunction, stArgs, oWritePipe )
            os.close( oWritePipe )
        else:
            os.close( oWritePipe )
            lReadable = self.__selectFD( [ oReadPipe ] )
            if len( lReadable ) == 0:
                os.close( oReadPipe )
                os.kill( iPid, 9 )
                os.waitpid( iPid, 0 )
                return S_ERROR( "%d seconds timeout for '%s' call" % ( self.iTimeout, oFunction.__name__ ) )
            elif lReadable[0] == oReadPipe:
                dData = self.__readFromPipe( oReadPipe )
                os.close( oReadPipe )
                os.waitpid( iPid, 0 )
                if dData[ 'OK' ]:
                    return eval( dData[ 'Value' ] )
                return dData
            
    def __generateSystemCommandError( self, sMessage ):
        retVal = S_ERROR( sMessage )
        retVal[ 'stdout' ] = self.lBuffers[0][0]
        retVal[ 'stderr' ] = self.lBuffers[1][0]
        return retVal
        
    def __readFromFile( self, oFile, iBaseLength, bAll ):
        try:
            if bAll:
                sData = "".join( oFile.readlines() )
            else:
                sData = oFile.readline()
        except Exception, v:
            pass 
        if sData == "":
            #self.checkAlive()
            self.bAlive = False
        if len( sData ) + iBaseLength > self.iBufferLimit:
            dRetVal = S_ERROR( "Reached maximum allowed length (%d bytes) for called function return value" % self.iBufferLimit )
            dRetVal[ 'ReadData' ] = sData
            return dRetVal
            
        return S_OK( sData )

    def __readFromSystemCommandOutput( self, oFile, iDataIndex, bAll = False ):
        retVal = self.__readFromFile( oFile, len( self.lBuffers[ iDataIndex ][0] ), bAll )
        if retVal[ 'OK' ]:
            self.lBuffers[ iDataIndex ][0] += retVal[ 'Value' ]
            if not self.oCallback == None:
                while self.__callLineCallback( iDataIndex ):
                    pass
            return S_OK()
        else:
            self.lBuffers[ iDataIndex ][0] += retVal[ 'ReadData' ]
            os.kill( self.oChild.pid, 9 )
            self.oChild.wait()
            return self.__generateSystemCommandError( "Exceeded maximum buffer size ( %d bytes ) timeout for '%s' call" % ( self.iBufferLimit, self.sCmd ) )

    def systemCall( self, sCmd, oCallbackFunction = None ):
        self.sCmd = sCmd
        self.oCallback = oCallbackFunction
        self.oChild = popen2.Popen3( self.sCmd, True )
        self.lBuffers = [ [ "", 0 ], [ "", 0 ] ]
        iInitialTime = time.time()
        iExitStatus = self.oChild.poll()

        while iExitStatus == -1:
            retVal = self.__readFromCommand()
            if not retVal[ 'OK' ]:
                return retVal
            if self.iTimeout and time.time() - iInitialTime > self.iTimeout:
                os.kill( self.oChild.pid, 9 )
                self.oChild.wait()
                self.__readFromCommand( True )
                self.oChild.fromchild.close()
                self.oChild.childerr.close()
                return self.__generateSystemCommandError( "Timeout (%d seconds) for '%s' call" % ( self.iTimeout, sCmd ) )
            iExitStatus = self.oChild.poll()
  
        self.__readFromCommand(True )

        self.oChild.fromchild.close()
        self.oChild.childerr.close() 
        return S_OK( ( iExitStatus / 256, self.lBuffers[0][0], self.lBuffers[1][0] ) )

    def __readFromCommand( self, bLast = False ):
        if bLast:
            retVal = self.__readFromSystemCommandOutput( self.oChild.fromchild, 0, True )
            if not retVal[ 'OK' ]:
                return retVal
            retVal = self.__readFromSystemCommandOutput( self.oChild.childerr, 1, True )
            if not retVal[ 'OK' ]:
                return retVal
        else:
            lReadable = self.__selectFD( [ self.oChild.fromchild, self.oChild.childerr ], 1 )
            if self.oChild.fromchild in lReadable:
                retVal = self.__readFromSystemCommandOutput( self.oChild.fromchild, 0 )
                if not retVal[ 'OK' ]:
                    return retVal
            if self.oChild.childerr in lReadable:
                retVal = self.__readFromSystemCommandOutput( self.oChild.childerr, 1 )
                if not retVal[ 'OK' ]:
                    return retVal
        return S_OK()

    
    def __callLineCallback( self, iIndex ):
        iNextLine = self.lBuffers[ iIndex ][0][ self.lBuffers[ iIndex ][1]: ].find( "\n" )
        if iNextLine > -1:
            self.oCallback( iIndex, self.lBuffers[ iIndex ][0][ self.lBuffers[ iIndex ][1]: self.lBuffers[ iIndex ][1] + iNextLine ] )
            self.lBuffers[ iIndex ][1] += iNextLine + 1 
            return True
        return False

############################################################################    
def redirectOutput(index, buffer):
    """Filter function to redirect the std output and error of the job
       executable for real-time debugging 
    """
    print buffer


############################################################################      
def exeCommand( sCmd, iTimeout = 0, oLineCallback = redirectOutput):
    """Return ( status, output, error, pythonError ) of executing cmd in a shell."""
    oSPE = SubprocessExecuter( iTimeout )
    retVal = oSPE.systemCall( sCmd, oLineCallback )
    if retVal[ 'OK' ]:
        return retVal[ 'Value' ][0], retVal[ 'Value' ][1], retVal[ 'Value' ][2], 0
    else:
        if re.search("Timeout",retVal['Message']):
          return 1, retVal['stdout'], retVal['Message'], 2
        else:  
          return 1, retVal['stdout'], retVal['stderr'], 1
    
#################################################################################
###################################### EoF ######################################
#################################################################################
#----------------------------------------------------
def removedirs (top):
    # Delete everything reachable from the directory named in 'top'.
    # CAUTION:  This is dangerous!  For example, if top == '/', it
    # could delete all your disk files.
    if os.path.exists (top):
        for root, dirs, files in os.walk (top, topdown=False):
            for name in files:
                name = root + '/' + name
                os.chmod (name, stat.S_IWRITE)
                #print name
                os.remove (name)
            for name in dirs:
                name = root + '/' + name
                os.chmod (name, stat.S_IWRITE)
                #print name
                os.rmdir (name)
        os.chmod (top, stat.S_IWRITE)
        #print top
        os.rmdir (top)
# def removedirs (top)


#----------------------------------------------------
#
# Executing a shell command
#
def execute (cmd):
    #print '> ' + cmd
    r, w, e = popen2.popen3(cmd)
    lines = []
    for line in r.readlines():
	##print line
        line = string.rstrip (line)
        lines.append (line)
    #for line in e.readlines():
    #    line = string.rstrip (line)
    #    lines.append (line)
    r.close()
    w.close()
    e.close()
    return lines
# def execute (cmd)

#----------------------------------------------------
#
# Display a set of lines
#
def show_lines (lines):
    for line in lines:
        print line
# def show_lines (lines)

#----------------------------------------------------
#
# Wrapper class for CMT.
#
# Prefetch the set of macros and tags
#
class Cmt:

    

    def expand_pattern (self, text, p1, p2):
        v = text
        rexp = ''
        for c in p1:
            rexp += r'[' + c + r']'
        rexp += '([^' + p2 + r']*)[' + p2 + r']'
        while True:
            ms = re.findall (rexp, v)
            if len(ms) == 0:
                break
            for m in ms:
                if m in self.macros:
                    v = string.replace (v, p1 + m + p2, self.macros[m])
                elif m in os.environ:
                    v = string.replace (v, p1 + m + p2, os.environ [m])
                else:
                    v = string.replace (v, p1 + m + p2, '')
        # while True
        return (v)

    def expand (self, text):
        v = text
        v = self.expand_pattern (v, '$(', ')')
        v = self.expand_pattern (v, '${', '}')
        v = self.expand_pattern (v, '%', '%')
        return (v)
    # expand
    
    # Pre-fetch the macro values and expand all of them
    def init_macros (self):
        lines = execute (self.cmtexe + ' show macros')
        for line in lines:
            w = string.split (line, '=')
            name = w[0]
            if len(w)>=2:             
                 value = re.sub ('^[\']', '', w[1])
                 value = re.sub ('[\']$', '', value)
                 self.macros [name] = value
        for key in self.macros.keys():
            self.macros[key] = self.expand (self.macros[key])
            #print key + '=' + v
        # for key in self.macros.keys():
    # init_macros

    def init_sets (self):
        lines = execute (self.cmtexe + ' show sets')
        for line in lines:
            w = string.split (line, '=')
            name = w[0]
            if len(w)>=2:
                 value = re.sub ('^[\']', '', w[1])
                 value = re.sub ('[\']$', '', value)
                 self.macros [name] = value
        for key in self.macros.keys():
            self.macros[key] = self.expand (self.macros[key])
            #print key + '=' + v
        # for key in self.macros.keys():
    # init_sets

    def init_tags (self):
        lines = execute (self.cmtexe + ' show tags')
        for line in lines:
            w = string.split (line, ' ')
            name = w[0]
            #print 'tag=' + name
            self.tags [name] = True
    # init_tags

    def init_uses (self):
        lines = execute (self.cmtexe + ' show uses')
        for line in lines:
            if line[0] == '#':
                if line[0:5] == '# use':
                    w = string.split (line, ' ')
                    #print 'init_uses> [' + w[2] + ']'
                    self.top_uses.append (w[2])
                continue
            self.uses.append (line)
    # init_uses

    # Constructor
    def __init__(self, tags=""):
        self.cmtexe = os.environ ['CMTROOT'] + '/' + os.environ ['CMTBIN'] + '/cmt.exe -tag_add=Doxygen,'+tags+' '
        self.macros = dict ()
        self.tags = dict ()
        self.sets = dict ()
        self.uses = []
        self.top_uses = []
        self.init_sets ()
        self.init_macros ()
        self.init_tags ()
        self.init_uses ()
    # __init__

    def macro_value (self, name):
        if not self.macros.has_key (name):
            return ''
        #print 'self.macros['+name+']=' + self.macros[name]
        return self.macros[name]
    # macro_value

    def tag (self, name):
        if not self.tags.has_key (name):
            return False
        return True
    # tag

    def do (self, cmd):
        execute (self.cmtexe + ' ' + cmd)
    # do

# class Cmt

#----------------------------------------------------
#
# Copy the template Doxyfile from the Doxygen interface
# Expand against CMT macros
# Manually add some Doxyfile statements
#
def filter_doxyfile (doxygen_input, doxygen_html_output, tagfiles, package_tagfile):
    XmlYes = 0
    # FIXME - commenting out for the moment, until I get everything else working
    
    # xmllist = open (DoxygenRoot + '/cmt/XMLList.txt')
    # lines = xmllist.readlines ()
    # xmllist.close ()
    # for packlist in lines:
    #     w = string.split (packlist)
    #     if len (w) > 0:
    #         if w[0] == package:
    #             XmlYes=1
    #             break
    f = open (DoxygenRoot + '/cmt/Doxyfile')
    lines = f.readlines ()
    f.close ()
    g = open ('Doxyfile', 'w')  
    package = package_tagfile.split('/')[-1].split('.')[0]
    # print "Going over Doxyfile lines:"
    for line in lines:
        w = string.split (line)
        if len (w) > 0:
            if w[0] == 'INPUT':
                line = 'INPUT = ' + doxygen_input + '\n'
            elif w[0] == 'OUTPUT_DIRECTORY':
                line = 'OUTPUT_DIRECTORY = ' + doxygen_html_output + '\n'
            elif w[0] == 'TAGFILES':
                line = 'TAGFILES = ' + ' '.join(tagfiles) + '\n'
            elif w[0] == 'GENERATE_TAGFILE':
                line = 'GENERATE_TAGFILE = ' + package_tagfile + '\n'
            elif w[0] == 'EXTERNAL_SEARCH_ID':
                line = 'EXTERNAL_SEARCH_ID = ' + package + '\n'
            elif w[0] == 'EXTRA_SEARCH_MAPPINGS':
                line = 'EXTRA_SEARCH_MAPPINGS = ' + package + '=' + '../../' + package + '/html\n'
            elif (w[0] == 'GENERATE_HTML'):
                if package_tagfile=='':
                  # Only generate HTML in the second pass, where we DON'T make a tag file
                  line = 'GENERATE_HTML = YES' + '\n'
                else:
                  line = 'GENERATE_HTML = NO' + '\n'
            elif (XmlYes and w[0] == 'GENERATE_XML'):
                line = 'GENERATE_XML           = ' + 'YES' + '\n'
            else:
                line = cmt.expand (line)
#         if line.startswith("STRIP_FROM_PATH"):
#             print line
#        print line
        g.write (line)
    g.close ()
# filter_doxyfile

#----------------------------------------------------
#
# Construct the list of expected tag files from the list of used packages
# and the list of used packages to display in the HTML table
#
def filter_uses ( output_dir ):
  used_packages = ''
  tagfiles = []
  # common_output = cmt.macro_value ('DOXYGEN_COMMON_OUTPUT_DIRECTORY')+'/InstallArea/doc' #FIXME! Should use something like build_output_dir

  for line in cmt.uses:
      w = string.split (line)
      package = w[1]
        
      if package == 'CMT':
          continue

      offset = cmt.macro_value (package + '_offset')

      if offset == 'CMT':
          continue

      if offset != '':
          offset = offset + '/'

      # I don't know why we care about finding the tagfile - just assume they exist? EJWM      
      # path = os.path.normpath (build_output_dir (package) + '/' + package + '.tag')
      # print 'path=',path
      # if os.path.isfile (path):
      #     print package + '.tag found in ' + path
      #     tagfiles = tagfiles + ' ' + path
      
      if package in cmt.top_uses:
          # print '    top use=',package
          used_packages = used_packages + ' ' + offset + package
          # New approach - fill tagfiles with the 'topuses' and complete doxygen-compatible format.
          # Doxygen uses funky format here: path/to/compB/compB.tag=path/to/compB/htmldocs
          # tagfile = common_output+"/"+package + '.tag='+"../../"+package+'/html'
          # tagfile = output_dir+'/'+package + '.tag='+output_dir+'/'+package+'/html'
          tagfile = output_dir+'/'+package + '.tag=../../'+package+'/html'
          # print 'tagfile = '+tagfile
          tagfiles.append(tagfile)
      # else:
      #     print 'NOT top use=',package
          
  #for line in cmt.uses

  return used_packages, tagfiles
# filter_uses

#----------------------------------------------------
#
# Construct the command to list a directory according to the platform
#
def get_ls_cmd ():
    #print 'Platform=' + sys.platform
    if sys.platform == 'win32':
        return 'dir'
    else:
        return 'ls'
# get_ls_cmd

#----------------------------------------------------
#
# Perform the relocation of html files WRT tagfiles
#
def relocate (output, tagfiles, html_mapping):
    print 'Now relocating output files'

    #tagfiles += cmt.macro_value ('Doxygen_extra_tagfiles')

    # tagfiles contains the list of all <P>.tag files for all used packages
    #print 'tagfiles = [' + tagfiles + ']'

    if len(tagfiles) == 0:
        print 'No tag files -> no way to relocate'
        return
    
    # We build a dictionary
    #    dict <tag-file-name, doc-dir>
    # where, for each tag file, we store the address where to install
    # Here again we have to consider the installarea strategy
    # Absolute file paths are converted into URLs
    subs = dict ()

    for f in string.split (tagfiles):
        filename = os.path.basename (f)
        package = string.replace (filename, '.tag', '')
        d = build_output_dir (package)
        for m in html_mapping:
            d = string.replace (d, os.path.normpath (m[0]), m[1])
        subs[filename] = string.replace (d, '\\', '/') + '/html/'

    os.chdir (output + '/html')

    # We now list all files that may refer to a tag, ie tree.js and *.html
    files = []
    for name in os.listdir ('.'):
        if name == 'tree.js':
            files.append (name)
        elif re.search ('[.]html$', name) != None:
            files.append (name)
    # for

    if len(files) == 0:
        print 'No file to relocate!'
        return

    # Handle all tags
    for key in subs:
        if key == '':
            continue
        if key == 'Doxygen.tag':
            continue
        if key == '_doc':
            continue
        if key == '_cgi':
            continue

        #print 'Substituting ' + subs[key] + ' for each occurence of tag file ' + key

        pattern1 = r'doxygen="(' + key + r'):([^ \"\t\>\<]*)" (href|src)="'
        repl1    = r'doxygen="' + subs[key] + r'" \3="' + subs[key]

        # Special for tree.js
        pattern2 = r'"(' + key + r')\:([^ \"\t\>\<]*)", "'
        repl2 = r'"' + subs[key] + '", "'

        #print 'p1 = ' + pattern1 + ' r = ' + repl1
        #print 'p2 = ' + pattern2 + ' r = ' + repl2

        for f in files:
            #print 'Editing ' + f
            fbak = f + '.bak' + key
            #fbak = f + '.bak'
            #print 'renaming ' + f + ' into ' + fbak
            if not os.path.isfile (f):
                #print 'File ' + f + ' not found '
                continue

            # A copy of the original file is created
            # to keep the unchanged text while the converted one is filled in
            #
            if os.path.isfile (fbak):
                os.remove (fbak)
            os.rename (f, fbak)
            if f != 'tree.js':
                pattern = pattern1
                repl = repl1
            else:
                pattern = pattern2
                repl = repl2

            g = open (f, 'w')
            gbak = open (fbak)

            for line in gbak:
                line2 = re.sub (pattern, repl, line)
                #if line2 != line:
                #  print line
                #  print '->' + line2
                g.write (line2)
            # for line in gbak

            g.close ()
            gbak.close ()
 
            # Now that the original text file is converted, we
            # can throw the original copy away
            #
            if os.path.isfile (fbak):
                os.remove (fbak)

        # for f in files
    # for key in subs

# relocate

#========================================================
#
# We consider two cases according to the build_strategy (with or without installarea)
#
# with_installarea:
#    <package_cmtpath>/InstallArea/doc/<package>/html
# without_installarea:
#    <package_root>/doc/html

# This is identical to build_output_dir(), so let's just use that? EJWM

# def build_tag_dir (package):
#     common_output = cmt.macro_value ('DOXYGEN_COMMON_OUTPUT_DIRECTORY')
#     if common_output == '':
#         common_output = cmt.macro_value (package + '_cmtpath')
#     project      = cmt.macro_value (package + '_project')
#     package_root = cmt.macro_value (package + '_root')
# 
#     if cmt.tag (project + '_with_installarea'):
#         return (os.path.normpath (common_output + '/InstallArea/doc/'))
#     else:
#         return (os.path.normpath (package_root + '/doc/'))
# def build_tag_dir (package)

def build_output_dir (package):
    common_output = cmt.macro_value ('DOXYGEN_COMMON_OUTPUT_DIRECTORY')
    if common_output == '':
        common_output = cmt.macro_value (package + '_cmtpath')

    project      = cmt.macro_value (package + '_project')
    package_root = cmt.macro_value (package + '_root')

    if cmt.tag (project + '_with_installarea'):
        return (os.path.normpath (common_output + '/InstallArea/doc/'))
    else:
        return (os.path.normpath (package_root + '/doc/'))
# def build_output_dir (package)

#----------------------------------------------------
#
# Parse html files in dir and replaces pattern with repl
#
def parse_html(dir,pattern,repl):
  files = os.listdir(dir)
  for file in files:
    if file.find('.html')>0:
      file = dir + '/' + file
      #print 'file = ',file
      f = open (file)
      # Read file contents to memory. Warning! This may be a memory hog
      contents = f.read ()
      f.close ()
      #print 'contents=', contents
      # Substitute pattern with repl in contents
      contents = re.sub(pattern,repl,contents);
      #print 'contents_new=', contents_new
      # Write to the same file
      f = open (file,'w')
      f.write(contents);
      f.close ()
  # for f in files:
#def parse_html(dir,pattern,repl)

def f (mapping):
    return (string.split (mapping, '@'))
# def f
    
def has_mainpage ():
    if not os.path.isdir (os.path.normpath ('../doc')):
        return (False)
    cmd = get_ls_cmd () + ' ' + os.path.normpath ('../doc')
    lines = execute (cmd)
    for line in lines:
        if string.lower(line) == 'mainpage.h':
            return (True)
    return (False)

# def has_mainpage


#----------------------------------------------------
# Main
#----------------------------------------------------

if len(sys.argv)==2:
   tags=sys.argv[1][5:]
else:
   tags=""   
print 'Running Local Doxygen'
cmt = Cmt (tags)
#print cmt.tags


#----------------------------------------------------
# Preparing the configuration

DoxygenRoot              = cmt.macro_value ('ATLASDOXYGENROOT')
package                  = cmt.macro_value ('package')
project                  = cmt.macro_value (package + '_project')
package_root             = cmt.macro_value (package + '_root')
package_cmtpath          = cmt.macro_value (package + '_cmtpath')

DOXYGEN_STRIP_FROM_PATH  = cmt.macro_value ('DOXYGEN_STRIP_FROM_PATH')
DOXYGEN_INPUT            = cmt.macro_value ('DOXYGEN_INPUT')
DOXYGEN_FILE_PATTERNS    = cmt.macro_value ('DOXYGEN_FILE_PATTERNS')
DOXYGEN_EXCLUDE          = cmt.macro_value ('DOXYGEN_EXCLUDE')
DOXYGEN_EXCLUDE_PATTERNS = cmt.macro_value ('DOXYGEN_EXCLUDE_PATTERNS')

html_mapping = map (f, string.split (cmt.macro_value ('DOXYGEN_HTML_MAPPING')))

print "DoxygenRoot = ", DoxygenRoot

output = build_output_dir (package)
writeHTML=False
package_tagfile=''
if 'doxygen_tags' in cmt.tags:
  print 'Pass #1 - Making doxygen tags (no HTML)'
  package_tagfile = os.path.normpath (output + '/' + package + '.tag')
  # package_tagfile = package + '.tag'
  # output = "."
else:
  # Pass #2 so make HTML
  print "Pass #2 - Writing HTML in this pass... (will use tags made in pass#1)"
  writeHTML=True

print 'output=[' + output + ']'
print 'input=[' + DOXYGEN_INPUT + ']'
print 'DOXYGEN_FILE_PATTERNS=[' + DOXYGEN_FILE_PATTERNS + ']'
print 'package_root=' + package_root
print 'package_tagfile=' + package_tagfile

DOXYGEN_GENERATE_TAGFILE = package_tagfile
print 'DOXYGEN_GENERATE_TAGFILE=' + DOXYGEN_GENERATE_TAGFILE

used_packages=''
tagfiles=[]
if writeHTML:
    # Only care about this for pass#2
    used_packages, tagfiles = filter_uses (output_dir=output)
print 'used_packages = ' + used_packages
print 'tagfiles = ' + ' '.join(tagfiles)

#----------------------------------------------------
# Preparing output

#print 'output = ' + output

if output and not os.path.isdir (output):
    os.makedirs (output)

if not os.path.isdir ('../doc'):
    os.makedirs ('../doc')

if not has_mainpage ():
    f = open ('../doc/mainpage.h', 'w')
    f.write ('/**\n')
    f.write ('\n')
    f.write (' <i>This page has been automatically generated by the tool. Please remove this line when you start creating your own mainpage</i>\n')
    f.write ('\n')
    f.write ('@mainpage\n')
    f.write ('\n')
    f.write ('@htmlinclude used_packages.html\n')
    f.write ('\n')
    f.write ('@include requirements\n')
    f.write ('\n')
    f.write ('*/\n')
    f.close ()

# Only worry about this when we're working with tagfiles?
if not writeHTML and not os.path.isdir (os.path.dirname (package_tagfile)):
    dirname = os.path.dirname (package_tagfile)
    if (dirname):
        os.makedirs (dirname)

#----------------------------------------------------
# Constructing the Doxyfile
#
# we first have to check the specified list of possible input file patterns
# since Doxygen is unable to survive empty specifications
#
doxygen_input = ''
for d in string.split (DOXYGEN_INPUT):
    d = os.path.normpath (d)
    if os.path.isdir (d):
        for p in string.split (DOXYGEN_FILE_PATTERNS):
            cmd = get_ls_cmd () + ' ' + os.path.normpath (d + '/' + p)
            lines = execute (cmd)
            if len (lines) != 0:
                doxygen_input = doxygen_input + ' ' + d
                break

# when the input is empty we must stop before doxygen is run to avoid generating an error
if doxygen_input == '':
    print 'Empty input for Doxygen'
    exit

print 'input for Doxygen = ' + doxygen_input

# Operations will now occur from the ../doc directory
os.chdir ('../doc')

package_version   = cmt.macros['version']
package_svnpath   = 'https://svnweb.cern.ch/trac/atlasoff/browser'+ package_root.replace(package_cmtpath,'') + '/tags/' + package_version

# copy new header into package doc (so it will be processed by Doxygen) - still need to insert SVN link
if writeHTML:
  # print 'Copying Layout XML, and adding SVN tab'
  f = open (DoxygenRoot + '/cmt/DoxygenLayout.xml')
  lines = f.readlines ()
  f.close ()
  g = open ('DoxygenLayout.xml', 'w')
  #print "Going over HTML lines:"
  for line in lines:
    # print line
    # Insert the link to SVN 
    if '<!-- SVNLINK -->' in line:
      g.write ('<tab type="user" visible="yes" url="'+ package_svnpath +'" title="SVN"/>')
    g.write (line)
  g.close ()

filter_doxyfile (doxygen_input, output+'/'+package, tagfiles, package_tagfile)

# # End here for debugging
# import sys
# sys.exit()

if writeHTML:
    #----------------------------------------------------
    # Constructing used_packages.html
    #
    # We have to guess where documentation of used packages are installed
    #
    f = open ('used_packages.html', 'w')
    f.write ('<center><table border="1"><tr><td><center><i><b>Used packages</b></i></center></td></tr>\n')
    for u in string.split (used_packages):
        p = os.path.basename (u)
        output = build_output_dir (p)
        r = output + '/'+p
        t = os.path.normpath (output + '/' + p + '.tag')
        if os.path.isfile (t): # Tag file exists
          for m in html_mapping:
            r = string.replace (r, os.path.normpath(m[0]), m[1])
          r = string.replace (r, '\\', '/')
          f.write ('<tr><td><a href="' + r + '/html/index.html">' + u + '</a></td></tr>\n')
        else: # Package does not exist
          f.write ('<tr><td>' + u + '</td></tr>\n')
        # if os.path.isfile (t)
    # for u in string.split (used_packages)

    f.write ('</table></center>\n')
    f.close ()

#----------------------------------------------------
# Running doxygen
print 'Current location =  ' + os.getcwd()
print 'Now running doxygen with PATH= \n' + cmt.macro_value ('PATH')
    
os.putenv ('PATH', cmt.macro_value ('PATH'))

status, text, error, pythonError  = exeCommand('doxygen')
#status, text = commands.getstatusoutput('doxygen')
#show_lines (string.split (text, '\n'))
#print status

# FIXME! Disabling this for the moment - not really sure what it's supposed to do...
#relocate (output, tagfiles, html_mapping)

# Below doesn't work
# parse_html(package_html_path,'<li>\n *<form action="search.php" method="get">',
#            '<li><a href="' + package_svnpath + '"><span>SVN</span></a></li>\n    <li>\n      <form action="search.php" method="get">')
