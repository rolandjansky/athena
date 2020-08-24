# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.acmdlib
# @purpose a library to ease the writing of sub-command scripts
# @author Sebastien Binet
# @date January 2010

from __future__ import with_statement

__doc__ = "a library to ease the writing of sub-command scripts"
__author__ = "Sebastien Binet"

__all__ = [
    'Command',
    'command',
    'argument',
    'register',
    ]

### imports -------------------------------------------------------------------
import argparse
import textwrap
import importlib

### globals -------------------------------------------------------------------
ACMD_GROUPNAME = 'acmdlib.commands'
"""The name under which all commands are grouped
"""

ACMD_PARSER = argparse.ArgumentParser(
    prog="acmd",
    description="a general script interface with sub-commands",
    )

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
        register(plugin_name, self.fct.__module__)

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

class Plugins(object):
    """A simple plugin registration.
    """
    _plugins = {}

    @classmethod
    def register(cls, name, value):
        """Register plugin
        """
        cls._plugins[name] = value

    @classmethod
    def exists(cls, name):
        """Check if plugin exists
        """
        return name in cls._plugins

    @classmethod
    def load(cls, name):
        """Load given plugin and return it
        """
        try:
            return importlib.import_module(cls._plugins[name])
        except Exception as err:
            print("** could not load command [%s]:\n%s" % (name, err))

    @classmethod
    def loadAll(cls):
        """Load all plugins
        """
        for k in cls._plugins.keys():
            cls.load(k)

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

    return Plugins.register(name, value)
