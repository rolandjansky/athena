# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/Help.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Basic end-user help."""

import os
from Logging import logging


### data ---------------------------------------------------------------------
__version__ = '1.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'help' ]

log = logging.getLogger( "AthenaHelp" )
log.setLevel( logging.INFO )


### help text ----------------------------------------------------------------
class Topic:
   def __init__( self, id, title, text, web, action = None, super = None ):
      self.id       = id
      self.title    = title
      self.text     = text
      self.web      = web
      self.action   = action
      if not super and id:
         global book
         book.sub.append( self )
         self.super = book
      else:
         self.super = super
      self.sub      = []

book = Topic( '', 'Interactive Help', '', '' )


### interactive help main class ----------------------------------------------
class Help:
   def __repr__( self ):
      return "Type help() for interactive help, or help(object) for help about object."

   def __call__( self, *args, **kw ):
      if args or kw:
         apply( __builtins__[ 'help' ], args, kw )    # "normal" python help
         return

      log.info( 'Welcome to the Athena Interactive Help' )

      current = book

      choice = 'l'
      while choice != 'q':
       # handle user choice
         if choice == 'l':        # list available topics
            print "\nAvailable topics:"
            for index in range(len(current.sub)):
               print "      (%d) %s" % (index+1,current.sub[index].title)
            print
         elif choice == 'p':      # perform topic action
            if current.action:
               import __main__, StringIO
               for line in StringIO.StringIO( current.action ).readlines():
                  line = line[:-1]
                  print '==>', line
                  if line:
                     exec( line, __main__.__dict__, __main__.__dict__ )
               print
         elif choice == 'w':
            if current.web:
               os.system( 'lynx %s' % current.web )
         else:
            try:
               current = current.sub[int(choice)-1]
               print "\n --- " + current.title + " --- \n"
               print current.text
            except (IndexError,ValueError):
               pass

       # retrieve new input
         try:
            text = 'Your choice (use l to list topics,%s%s q to quit)? ' % \
               ((current.action and ' p to perform,' or ''),
                (current.web and ' w for web,' or ''))
            choice = raw_input( text )
         except (KeyboardInterrupt,EOFError):
            print
            break

help = Help()


### help text ----------------------------------------------------------------
Topic( id = 'Basics',
       title = 'Running Hello World',
       text = """In order to run Hello World you need to load the corresponding Dll,
and add it to the TopAlg sequence:

athena> theApp.Dlls += [ 'AthExHelloWorld' ]
athena> theApp.TopAlg += [ 'HelloWorld' ]

Finally, run the application with a certain number of events (e.g. 5):

athena> theApp.run( 5 )
""",
       web    = "https://uimon.cern.ch/twiki/bin/view/Atlas/WorkBookRunAthenaHelloWorld",
       action = """theApp.Dlls += [ 'AthExHelloWorld' ]
theApp.TopAlg += [ 'HelloWorld' ]
theApp.run(5)
""" )
