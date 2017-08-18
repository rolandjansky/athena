# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.acmdlib
# @purpose a library to ease the writing of sub-command scripts
# @author Sebastien Binet
# @date January 2010

from __future__ import with_statement

__version__ = "$Revision: 464077 $"
__doc__ = "a library to ease the writing of sub-command scripts"
__author__ = "Sebastien Binet"

__all__ = [
    'Command',
    'command',
    'argument',
    'register',
    'register_file',
    ]

### imports -------------------------------------------------------------------
import sys
import extensions as ext_plugins
import argparse
import textwrap

### globals -------------------------------------------------------------------
ACMD_GROUPNAME = 'acmdlib.commands'
"""The name under which all commands are grouped
"""

ACMD_PARSER = argparse.ArgumentParser(
    prog="acmd",
    description="a general script interface with sub-commands",
    )
ACMD_PARSER.add_argument(
    '--version',
    action='version',
    version=__version__,
    help="show program's version number and exit")

ACMD_SUBPARSERS = ACMD_PARSER.add_subparsers(
    dest='command',
    title='commands',
    metavar='COMMAND',
    )

### classes -------------------------------------------------------------------
class Command(object):
    """A wrapper class to manage the creation of commands and their arguments

    this is very heavily inspired from:
    http://pypi.python.org/pypi/django-boss (MIT licence)
    """

    def __init__(self, fct, **kwargs):
        object.__init__(self)
        self.fct = fct
        self.parser = self._make_parser(**kwargs)
        self._init_arguments()
        plugin_name = kwargs.get('name') or self.name
        register(
            plugin_name,
            '%s:%s' % (self.fct.__module__, self.fct.__name__)
            )
        return

    @property
    def name(self):
        return self.fct.__name__.replace('_','-')

    @property
    def help(self):
        if getattr(self.fct, '__doc__', None):
            # just the first line of the doc string
            return self.fct.__doc__.splitlines()[0]

    @property
    def description(self):
        if getattr(self.fct, '__doc__', None):
            return textwrap.dedent(self.fct.__doc__)

    @property
    def add_argument(self):
        return self.parser.add_argument

    def __call__(self, *args, **kwargs):
        return self.fct(*args, **kwargs)
    
    def _make_parser(self, **kwargs):
        """Create and register a subparser for this command."""

        kwargs.setdefault('help', self.help)
        kwargs.setdefault('formatter_class',argparse.RawDescriptionHelpFormatter)
        kwargs.setdefault('description', self.description)
        kwargs.setdefault('name', self.name)
        names = (kwargs.get('name') or self.name).split('.')
        
        def _get_subparser(a):
            if a._subparsers:
                for action in a._subparsers._actions:
                    if isinstance(action, argparse._SubParsersAction):
                        return action
                raise RuntimeError('could not find adequate subparser')
            return a.add_subparsers(dest='command',
                                    title='commands',
                                    metavar='COMMAND')
        def _get_parser(node, idx, names):
            name = names[idx]
            if name in node.choices:
                return node.choices[name]
            full_name = ' '.join(names[:idx+1])
            args = {
                'name' : name,
                'help' : 'a group of sub-commands',
                }
            return node.add_parser(**args)
        
        parser = ACMD_PARSER
        node   = _get_subparser(parser)

        for i,n in enumerate(names[:-1]):
            node = _get_subparser(parser)
            parser = _get_parser(node, i, names)
                
        node = _get_subparser(parser)
        kwargs['name'] = names[-1]
        parser = node.add_parser(**kwargs)
        return parser

    def _init_arguments(self):
        if hasattr(self.fct, '_acmdlib_arguments'):
            while self.fct._acmdlib_arguments:
                args, kwargs = self.fct._acmdlib_arguments.pop()
                self.add_argument(*args, **kwargs)
        
    pass # Command

### functions -----------------------------------------------------------------

def command(*args, **kwargs):
    """Decorator to declare that a function is a command.
    """
    def deco(fct):
        return Command(fct, **kwargs)
    if args:
        return deco(*args)
    return deco

def argument(*args, **kwargs):
    """Decorator to add an argument to a command.
    """
    def deco(fct):
        if isinstance(fct, Command):
            cmd = fct
            cmd.add_argument(*args, **kwargs)
        else:
            if not hasattr(fct, '_acmdlib_arguments'):
                fct._acmdlib_arguments = []
            fct._acmdlib_arguments.append((args, kwargs))
        #print "===",args,kwargs,type(fct),fct
        return fct
    return deco
    
def register(name, value):
    """Registers a plugin, given a name and value.

    ex: register('check-file', 'PyUtils.CheckFileLib:fct')
    """
    group = ACMD_GROUPNAME
    return ext_plugins.register(group, name, value)

def register_file(path):
    """Registers a config-like file"""
    from ConfigParser import ConfigParser
    parser = ConfigParser()
    parser.read([path])
    for group in parser.sections():
        for name in parser.options(group):
            value = parser.get(group, name)
            ext_plugins.register(group, name, value)

# add a special command to list all registered commands
@command
@argument('-d','--detailed', action='store_true', default=False,
          help='print full help of each command')
def list_commands(args):
    """a special command to list all the commands 'acmd' can run
    """
    cmds = list(ext_plugins.get(group=ACMD_GROUPNAME))
    if len(cmds) == 0:
        print "::: no command found in registry"
        return 1
    print "::: found [%i] command%s" % (len(cmds),
                                        "s" if len(cmds)>1 else "")
    cmds.sort(cmp=lambda x,y: cmp(x.name, y.name))
    cmds = [cmd for cmd in cmds if cmd.name != 'list-commands']
    for i, cmd in enumerate(cmds):
        if args.detailed:
            print "="*80
        print " - %s" % (' '.join(cmd.name.split('.')),)
        if args.detailed:
            try:
                cmd.load().parser.print_help()
            except Exception,err:
                print "** could not inspect command [%s]:\n%s" % (
                    cmd.name,
                    err)
            print "="*80
            print ""
    return 0
#acmdlib.register('list-commands', 'PyUtils.acmdlib:list_commands')

# an argument to force the loading of all available commands
ACMD_PARSER.add_argument(
    '--load-commands',
    action='store_true',
    default=False,
    help='force the loading of all available commands')
def _load_commands():
    cmds = list(ext_plugins.get(group=ACMD_GROUPNAME))
    for cmd in cmds:
        try:
            cmd.load()
        except Exception,err:
            print "** could not load command [%s]:\n%s" % (
                cmd.name,
                err)

