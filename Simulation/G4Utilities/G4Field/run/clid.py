# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

exec python -tx "$0" "$@"

# Copyright (c) 2002-2003, cetull@lbl.gov
# Driver for Athena CLID Generator
# BUGS: 
# * None Known
#
import string, re
import sys, getopt, os
import clidGen

# PROGRAM NAME
prog = os.path.basename(sys.argv[0])

# DEBUG Flag: Turns on DEBUG printout (default = false)
debug = 0
# MACRO Flag: Turns on macro option (default = false)
macro = 0
# SILENT Flag: Turns on silent option (default = false)
silent = 0
# CLID DataBase
cliddb = ''
# List of regular expressions to process
relist = []
# List of "CLID==NAME"
valist = []

#
# Help Messages
#
def usage_message():
    print '''usage:''',prog,'''[-hmsuv] [--db=<clid.db>] [name ...] [clid ...]'''
#
def usage_message_full():
    print '''usage:''',prog,'''[-dhmsuv] [--db=<clid.db>] [name ...] [clid ...]'''
#
def help_message():
    print prog,'''- Athena CLID Generator
Usage:''',prog,'''<options> [<ClassName>] [<CLID>]
Options and arguments:  
-b <clid_db>: Reconcile generated CLID with CLID DataBase <clid_db>.
-f <regex>  : Find entry whose ClassName or CLID matches <regex>.
              (See Python re module for syntax of regex.)
-h          : Print this help message and exit.
-m          : Print macro call for StoreGate.
-s          : Be more silent on output. Print _only_ result.
              (N.B. Overrides -m option.)
-u          : Print usage message and exit.
-v          : Print version string and exit.
--db=<clid_db>: Synonym for -b.
--find=<regex>: Synonym for -f.
--help        : Synonym for -h.
--macro       : Synonym for -m.
--silent      : Synonym for -s.
--usage       : Synonym for -u.
--version     : Synonym for -v.
Examples: '''
    print '    ',prog,'-h'
    print '    ',prog,'--version'
    print '    ',prog,'LArCell'
    print '    ',prog,'--db=clid.db LArCell'
    print '    ',prog,'--db=clid.db 2001'
    print '    ',prog,'--db=clid.db -f "Cr"'

#
#
def help_message_full():
    print prog,'''- Athena CLID Generator
Usage:''',prog,'''<options> [<ClassName>] [<CLID>]
Options and arguments:  
-b <clid_db>: Reconcile generated CLID with CLID DataBase <clid_db>.
-d          : Print DEBUG information during execution.
-f <regex>  : Find entry whose ClassName or CLID matches <regex>.
              (See Python re module for syntax of regex.)
-h          : Print this help message and exit.
-m          : Print macro call for StoreGate.
-s          : Be more silent on output. Print _only_ result.
              (N.B. Overrides -m option.)
-u          : Print usage message and exit.
-v          : Print version string and exit.
--db=<clid_db>: Synonym for -b.
--debug       : Synonym for -d.
--find=<regex>: Synonym for -f.
--help        : Synonym for -h.
--macro       : Synonym for -m.
--silent      : Synonym for -s.
--usage       : Synonym for -u.
--version     : Synonym for -v.
Examples: '''
    print '    ',prog,'-h'
    print '    ',prog,'--version'
    print '    ',prog,'-d LArCell'
    print '    ',prog,'--db=clid.db LArCell'
    print '    ',prog,'--db=clid.db 2001'
    print '    ',prog,'--db=clid.db -f "Cr"'

#
# Error Message
#
def err_message():
    print prog,'''- Error:
You tried to use an unknown option or the argument for an option that
requires one was missing.
Try `''',prog,''' -h\' for more information.'''

#
# Unimplemented Option Message
#
def unimpl_message(AAA):
    print prog,'''- Error:''',AAA,'''
You tried to use an unimplemented option. This feature will be
available in a future release.
Try `''',prog,''' -h\' for more information.'''

#
# Depricated Option Message
#
def depricated_message(AAA):
    print prog,'''- Error:''',AAA,'''
You tried to use a depricated option. This feature is no longer
available, though the option remains for compatability.
Try `''',prog,''' -h\' for more information.'''

#
# Parse the options and their arguments.
#
kflags = 'b:df:hmsuvx:'
kargs = ['db=','debug','find=','help','macro','silent','usage','version']
try:
    options, xarguments = getopt.getopt(sys.argv[1:],kflags,kargs)
except getopt.error:
    err_message()
    sys.exit(0)

#
# Require at least one argument.
#
if len(sys.argv) == 1:
    help_message()
    sys.exit(0)

#
# Process known options.
#
for a in options[:]:
    # STANDARD UTILITY OPTIONS
    if a[0] == '-d' or a[0] == '--debug':
        debug = 1
        options.remove(a)
    if a[0] == '-h' or a[0] == '--help':
        help_message()
        options.remove(a)
        sys.exit(0)
    if a[0] == '-s' or a[0] == '--silent':
        silent = 1
        options.remove(a)
    if a[0] == '-u' or a[0] == '--usage':
        usage_message()
        options.remove(a)
        sys.exit(0)
    if a[0] == '-v' or a[0] == '--version':
        print sys.argv[0],'$Revision: 1.1.1.1 $ (08jul02-cetull@lbl.gov)'
        options.remove(a)
        sys.exit(0)
    # CLID OPTIONS
    if a[0] == '-b' or a[0] == '--db':
        if debug: print 'CLID DB =',a[1]
        cliddb = a[1]
        options.remove(a)
    if a[0] == '-f' or a[0] == '--find':
        if debug: print 'CLID FIND =',a[1]
        relist.append(a[1])
        options.remove(a)
    if a[0] == '-m' or a[0] == '--macro':
        macro = 1
        options.remove(a)
    if a[0] == '-x':
        valist.append(a[1])
        options.remove(a)

#
# Process unknown options.
#
for a in options[:]:
    unimpl_message(a)
    options.remove(a)

#
# Initialize 
#
if cliddb: 
    cgen = clidGen.clidGenerator(cliddb)
else:
    cgen = clidGen.clidGenerator("")
if debug: print "repository = ",cgen._clidGenerator__clidRep

#
# Process ClassName arguments
#
for a in xarguments:
    try:
        i = int(a)
    except:
        n = a
        c = cgen.genClidFromName(a)
    else:
        n = cgen.getNameFromClid(i)
        c = i
        if not n:
            n = ">>>Not Found in CLIDDB<<<"
    if debug: 
        print c,n,'(',hash(n),hash(n)&cgen._clidGenerator__mask,"%8x"%c,')'
    else:
        if silent: print c
        else:
            if macro: print "CLASS_DEF(",n,",",c,", 1 )"
            else: print c,n

#
# Process RegEx list
#
for p in relist:
    print '>>> Searching CLIDDB for (',p,') <<<'
    print cgen.findPattern(p)

#
# Process Validation list
#
for p in valist:
    pp = string.split(p,'==')
#   print 'Validating(',pp,')'
    ppid = cgen.getClidFromName(pp[1])
    if ppid == int(pp[0]):
        print pp,'is a valid pair.',ppid
    else:
        print pp,'is not a valid pair.',ppid
        sys.exit(1)
#
# Write out new repository
#
cgen.writedb("new.db")
#
