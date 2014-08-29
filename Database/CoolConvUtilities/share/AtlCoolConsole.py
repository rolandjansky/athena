#!/usr/bin/env python
# version modified by Richard Hawkings from COOl1.3.2 distribution
# original by Sven Schmitt (Abstracture/Mainz)

import sys
import code
import readline
import atexit
import os
import re
import logging

log = logging.getLogger( __name__ )
log.setLevel( logging.INFO )

handler = logging.StreamHandler()
format = "%(levelname)s:%(name)s: %(message)s"
handler.setFormatter( logging.Formatter( format ) )
log.addHandler( handler )


historyFile = '.coolconsole_hist'

class HistoryConsole(code.InteractiveConsole):
    """
    This class extends InteractiveConsole with command line history. The
    history can be accessed with the usual CURSOR UP keystroke. It is also
    stored to a file '%s' in the user's home directory.
    """ % historyFile

    def __init__( self,
                  locals = None,
                  filename = "<console>",
                  histfile = os.path.join( os.environ["HOME"], historyFile) ):
        code.InteractiveConsole.__init__( self )
        self.init_history( histfile )

    def init_history( self, histfile ):
        readline.parse_and_bind( "tab: complete" )
        readline.set_history_length( 100 )
        if hasattr( readline, "read_history_file" ):
            try:
                readline.read_history_file( histfile )
            except IOError:
                pass
            atexit.register( self.save_history, histfile )

    def save_history( self, histfile ):
        readline.write_history_file( histfile )



class AtlCoolConsole( HistoryConsole ):

    commands = { 'help' : 'help overview',
                 'less' : 'list contents of folders, e.g. less "/a"',
                 'more' : 'list contents of folders (ATLAS specific)',
                 'ls' : 'list contents of foldersets, e.g. ls "/"',
                 'll' : 'list contents of foldersets with entry count',
                 'exit' : 'quit the interpreter session',
                 'quit' : 'alias for exit',
                 'open' : "open the specified database, "
                          "e.g. open 'sqlite://...'",
                 'cd'   : 'change directory',
                 'pwd'  : 'print current directory',
                 'rmdir' : 'remove a folder or folderset',
                 'usetag' : 'set subsequent list operations (less) to use given COOL tag',
                 'usechan' : 'set subsequent list operations (less) to display given COOL channel number only',
                 'userunlumi' : 'set limits for run/lumi blocks in more/less',
                 'usetimes' : 'set limits for timestamps in more/less',
                 'pws' : 'print current tag and channel selections used by less',
                 'listtags' : 'list tags in folder / folder set',
                 'listchans' : 'list channels in folder',
                 'listinfo' : 'list info about folder (type, columns)',
                 'filtertags' : 'list tags in folder / folder set with filtering',
                 'settag' : 'set hierarchical tag for a folder+parent',
                 'settginfo' : 'set tag description string',
                 'setchan' : 'set channel name/description',
                 'clonetag' : 'clone data from one tag to another',
                 'setdesc' : 'set folder description (meta-data) string',
                 'rmtag'  : 'remove tag or hierarchical tag relation to a parent',
                 'locktag' : 'lock a tag so the contents cannot be changed',
                 'headtag' : 'apply HEAD-style tag to a folder',
                 'tracetags': 'list tags defined in subfolders for parent'
                 }

    banner = "Welcome to AtlCoolConsole. Type 'help' for instructions."

    def __init__( self, connectString = None ):
        HistoryConsole.__init__( self )
        self.connectString = connectString


    def interact( self ):
        self.push( 'from CoolConvUtilities import AtlCoolTool' )
        if connectString is not None:
            self.push( 'this = AtlCoolTool.AtlCoolTool("%s")' % self.connectString )
            self.push( 'print this' )
        else:
            print ( "Not connected. Use the 'open' command to connect to "
                    "a database." )
        HistoryConsole.interact( self, self.banner )


    def parseLine( self, line ):
        for command in self.commands.keys():
            res = re.search( '^' + command + '\s*(?P<remainder>.*)', line )
            if res is not None:
                return command, res.group('remainder')
        return None, line


    def createInterpreterCommand( self, command, argumentString ):
        if command == 'less':
            return self.command_less( argumentString )
        elif command == 'more':
            return self.command_more( argumentString )
        elif command == 'ls':
            return self.command_ls( argumentString,False )
        elif command == 'll':
            return self.command_ls( argumentString,True )
        elif command == 'help':
            return self.command_help( argumentString )
        elif command == 'exit':
            return self.command_exit()
        elif command == 'quit':
            return self.command_exit()
        elif command == 'open':
            return self.command_open( argumentString )
        elif command =='listtags':
            return self.command_lstags( argumentString )
        elif command =='filtertags':
            return self.command_filtertags( argumentString )
        elif command =='listchans':
            return self.command_listchans( argumentString )
        elif command =='listinfo':
            return self.command_listinfo( argumentString )
        elif command =='rmdir':
            return self.command_rmdir( argumentString )
        elif command =='usetag':
            return self.command_usetag( argumentString )
        elif command =='usechan':
            return self.command_usechan( argumentString )
        elif command =='settag':
            return self.command_settag( argumentString )
        elif command =='settginfo':
            return self.command_settginfo( argumentString )
        elif command =='setchan':
            return self.command_setchan( argumentString )
        elif command =='userunlumi':
            return self.command_userunlumi( argumentString )
        elif command =='usetimes':
            return self.command_usetimes( argumentString )
        elif command =='clonetag':
            return self.command_clonetag( argumentString )
        elif command =='rmtag':
            return self.command_rmtag( argumentString )
        elif command =='locktag':
            return self.command_locktag( argumentString )
        elif command =='headtag':
            return self.command_headtag( argumentString )
        elif command =='tracetags':
            return self.command_tracetags( argumentString )
        elif command == 'setdesc':
            return self.command_setdesc( argumentString )
        elif command =='cd':
            return self.command_cd( argumentString )
        elif command =='pwd':
            return self.command_pwd()
        elif command =='pws':
            return self.command_pws()
        else:
            return None
        
    
    def command_less( self, argumentString ):
        if argumentString is None:
            raise Exception( "usage: less <folder>" )
        argumentString.strip()
        nodes = argumentString.split()
        cmds = []
        for node in nodes:
            cmds.append( 'this.less("%s", header=True)' % node )
        return ';'.join( cmds )

    def command_more( self, argumentString ):
        if argumentString is None:
            raise Exception( "usage: more <folder>" )
        argumentString.strip()
        nodes = argumentString.split()
        cmds = []
        for node in nodes:
            cmds.append( 'this.more("%s", header=True)' % node )
        return ';'.join( cmds )


    def command_ls( self, argumentString, doCount=False ):
        if argumentString is None: argumentString = '.'
        argumentString.strip()
        if argumentString == '': argumentString = '.'
        nodes = argumentString.split()
        cmds = []
        tof='False'
        if (doCount): tof='True'
        for node in nodes:
            cmds.append( 'this.ls("%s", header=True, doCount=%s)' % (node,tof) )
        return ';'.join( cmds )

    def command_pwd(self):
        return ';'.join(['this.pwd()'])

    def command_pws(self):
        return ';'.join(['this.pws()'])

    def command_lstags( self, argumentString ):
        if argumentString is None: argumentString = '.'
        argumentString.strip()
        if argumentString is None: argumentString = '.'
        nodes = argumentString.split()
        cmds = []
        for node in nodes:
            cmds.append( 'this.lstags("%s")' % node )
        return ';'.join( cmds )

    def command_filtertags(self,argumentString):
        if argumentString is None or argumentString=="": argumentString = '.'
        argumentString.strip()
        if argumentString is None: argumentString = '.'
        args = argumentString.split()
        node=args[0]
        pattern=""
        if len(args)>1:
            pattern=args[1]
        cmds = []
        cmds.append('this.lstags("%s",pattern="%s")' % (node,pattern))
        return ';'.join(cmds)
        
    
    def command_listchans( self, argumentString ):
        if argumentString is None: argumentString = '.'
        argumentString.strip()
        if argumentString is None: argumentString = '.'
        nodes = argumentString.split()
        cmds = []
        for node in nodes:
            cmds.append( 'this.listchans("%s")' % node )
        return ';'.join( cmds )

    def command_listinfo( self, argumentString ):
        if argumentString is None: argumentString = '.'
        argumentString.strip()
        if argumentString is None: argumentString = '.'
        nodes = argumentString.split()
        cmds = []
        for node in nodes:
            cmds.append( 'this.listinfo("%s")' % node )
        return ';'.join( cmds )

    def command_rmdir(self, argumentString ):
        argumentString.strip()
        cmds=[]
        cmds.append('this.rmdir("%s")' % argumentString)
        return ';'.join( cmds )

    def command_usetag(self, argumentString ):
        argumentString.strip()
        cmds=[]
        cmds.append('this.usetag("%s")' % argumentString)
        return ';'.join( cmds )

    def command_usechan(self, argumentString ):
        argumentString.strip()
        cmds=[]
        cmds.append('this.usechan("%s")' % argumentString)
        return ';'.join( cmds )
        
    def command_cd( self, argumentString ):
        if argumentString is None: argumentString ='/'
        argumentString.strip()
        if argumentString == '': argumentString = '/'
        cmds=[]
        cmds.append('this.cd("%s")' % argumentString)
        return ';'.join( cmds )

    def command_settag(self,argumentString):
        args=argumentString.split()
        if len(args)<3:
            print 'Usage: settag <folder> <foldertag> <parenttag>'
            return ''
        cmds=[]
        cmds.append('this.settag("%s")' % argumentString)
        return ';'.join(cmds)

    def command_settginfo(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            print 'Usage: settginfo <folder> <tag> <description>'
            return ''
        cmds=[]
        cmds.append('this.settginfo("%s")' % argumentString)
        return ';'.join(cmds)

    def command_setchan(self,argumentString):
        args=argumentString.split()
        if len(args)<3:
            print 'Usage: setchan <folder> <chanID> <chanName> {<chanDescr>}'
            return ''
        cmds=[]
        cmds.append('this.setchan("%s")' % argumentString)
        return ';'.join(cmds)

    def command_userunlumi(self,argumentString):
        args=argumentString.split()
        if len(args)!=1 and len(args)<4:
            print 'Usage: userunlumi <run1> {<LB1> <run2> <LB2>}'
            return ''
        cmds=[]
        cmds.append('this.userunlumi("%s")' % argumentString)
        return ';'.join(cmds)

    def command_usetimes(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            print 'Usage: usetimes <time1> <time2>'
            return ''
        cmds=[]
        cmds.append('this.usetimes("%s")' % argumentString)
        return ';'.join(cmds)

    def command_clonetag(self,argumentString):
        args=argumentString.split()
        if len(args)<3:
            print 'Usage: clonetag <folder> <sourcetag> <desttag>'
            return ''
        cmds=[]
        cmds.append('this.clonetag("%s")' % argumentString)
        return ';'.join(cmds)


    def command_rmtag(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            print 'Usage: rmtag <folder> <leaf or parent tag>'
            return ''
        cmds=[]
        cmds.append('this.rmtag("%s")' % argumentString)
        return ';'.join(cmds)

    def command_locktag(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            print 'Usage: locktag <folder> <tag> {action=l|p|u|r}'
            return ''
        cmds=[]
        cmds.append('this.locktag("%s")' % argumentString)
        return ';'.join(cmds)

    def command_headtag(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            print 'Usage: headtag <folder> <tag>'
            return ''
        cmds=[]
        cmds.append('this.headtag("%s")' % argumentString)
        return ';'.join(cmds)


    def command_tracetags(self,argumentString):
        args=argumentString.split()
        if len(args)<2:
            print 'Usage: tracetags <folder> <foldertag>'
            return ''
        cmds=[]
        cmds.append('this.tracetags("%s")' % argumentString)
        return ';'.join(cmds)


    def command_setdesc( self, argumentString ):
        args=argumentString.split()
        if len(args)<1:
            print 'Usage: setdesc <folder> {<newdesc>}'
            return ''
        cmds=[]
        cmds.append('this.setdesc("%s")' % argumentString)
        return ';'.join(cmds)
        
         
    def command_help( self, argumentString ):
        if argumentString is not None and argumentString != '':
            # we have a python "help(...)" command most likely
            # we only want to intercept "help", therefore return the original
            return 'help ' + argumentString
        print 'Available commands:'
        keys = self.commands.keys()
        keys.sort()
        for key in keys:
            print '  %(key)-8s : %(value)s' % { 'key' : key,
                                                'value' : self.commands[key] }
        print "These commands are shortcuts that are forwarded to a CoolTool"
        print "instance 'this', referring to the currently connected database."
        print "Since this environment is a fully functional python shell,"
        print "'this' can be used like any python object, e.g.:"
        print "  this.ls( '/' )"
        return ''


    def command_exit( self ):
        return 'import sys ; sys.exit(0)'


    def command_open( self, argumentString ):
        log.debug( 'argumentString: ' + argumentString )
        if ( not argumentString.startswith('"') and 
             not argumentString.startswith("'") ):
            argumentString = "'" + argumentString + "'"
            log.debug( 'argumentString: ' + argumentString )
        return 'this = AtlCoolTool.AtlCoolTool(%s)' % argumentString


    def push(self, line):
        command, remainder = self.parseLine( line )
        log.debug( 'command:   %s' % command )
        log.debug( 'remainder: %s' % remainder )

        res = self.createInterpreterCommand( command, remainder )
        if res is not None:
            log.debug( 'command: "%s"' % res )
            interpreterCommand = res
        else:
            interpreterCommand = line

        #HistoryConsole.push( self, 'print "cmd: %s"' % interpreterCommand )
        return HistoryConsole.push( self, interpreterCommand )




if __name__ == '__main__':


    usage = ( 'Usage: %s <connect string>\n'
              '\t<connect string>: a RAL connect string\n'
              ) % sys.argv[0]


    if len( sys.argv ) == 2:
        connectString = sys.argv[1]
    else:
        connectString = None

    tool = AtlCoolConsole( connectString )
    tool.interact()
    
    
