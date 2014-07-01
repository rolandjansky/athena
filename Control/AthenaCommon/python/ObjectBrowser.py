# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/share/ObjectBrowser.py
# Author: Wim Lavrijsen (LBNL, WLavrijsen@lbl.gov)

import sys

try:
   import idlelib.ObjectBrowser as ibrowser
except ImportError, problem:
   print """It would appear that IDLE is not properly installed. IDLE, the python
IDE, comes standard with python 2.3. For older pythons, please install
it from http://sourceforge.net/projects/idlefork.
"""

 # detailed exception information follows:
   import traceback
   traceback.print_tb( sys.exc_traceback )

 # stop processing
   raise problem

import Tkinter


try:
   gmod = sys.modules[ 'gaudimodule' ]
except KeyError:
   gmod = None


class GaudiAlgTypeTreeItem( ibrowser.ObjectTreeItem ):
   def GetSubList( self ):
      props = self.object.properties()

      sublist = []
      for prop in props:
         item = ibrowser.make_objecttreeitem(
            str(prop) + " =",
            eval( 'self.object.%s' % prop ),
            lambda value, key=prop, object=self.object:
               setattr(object, key, value))
         sublist.append( item )

      return sublist

GaudiSvcTypeTreeItem = GaudiAlgTypeTreeItem


class TopTreeItem( ibrowser.ObjectTreeItem ):
   def __init__( self, title, names, values ):
      assert( len(names) == len(values) )

      ibrowser.ObjectTreeItem.__init__( self, title, names, None )

      self.names = names
      self.values = values

   def GetSubList( self ):
      sublist = []

      for i in range(len(self.names)):
         item = ibrowser.make_objecttreeitem( self.names[i], self.values[i] )
         sublist.append( item )

      return sublist


def initTreeItemsDispatch():
   if gmod != None:
    ## add Gaudi specific tree items
      if not ibrowser.dispatch.has_key( gmod.iAlgorithm ):
         ibrowser.dispatch[ gmod.iAlgorithm ] = GaudiAlgTypeTreeItem

      if not ibrowser.dispatch.has_key( gmod.iService ):
         ibrowser.dispatch[ gmod.iService ] = GaudiSvcTypeTreeItem


class Browser( Tkinter.Frame ):
   def __init__( self, master = None ):
    # setup Tk as necessary
      if master == None:
         master = Tkinter.Tk()

    # setup base
      Tkinter.Frame.__init__( self, master )
      self.master.title( 'Gaudi Object Browser' )

    # create dispatching as necessary
      initTreeItemsDispatch()

    # construct the browser
      sc = ibrowser.ScrolledCanvas( self,  height = 400, width = 640,
          bg = 'white', highlightthickness = 0, takefocus = 1 )
      sc.frame.pack( expand = 1, fill = 'both' )

    # fill the initial nodes
      dct = sys.modules[ '__main__' ].__dict__

      algs, svcs = {}, {}
      if gmod != None:
         algs = self.select( dct, gmod.iAlgorithm )
         svcs = self.select( dct, gmod.iService )

      top = TopTreeItem( 'objects',
         [ 'algorithms', 'services', 'globals' ],
         [ algs,         svcs,       dct       ] )

    # GUI stuff
      node = ibrowser.TreeNode( sc.canvas, None, top )
      node.expand()
      node.update()

      self.pack( expand = 1, fill = 'both' )

   def select( self, fromDict, whatType ):
      """Select objects of the requested type from the given dictionary."""

      selection = {}
      for name, value in fromDict.items():
         if type(value) == whatType:
            selection[ name ] = value

      return selection
