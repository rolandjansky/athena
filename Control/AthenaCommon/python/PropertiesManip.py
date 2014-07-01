# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/PropertiesManip.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Convenience functions to manipulate Gaudi object properties."""


### data ---------------------------------------------------------------------
__version__ = '1.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'appendItemsToList', 'removeItemsFromList', 'removeFromList' ]


### item appending for a list property --------------------------------------
def appendItemsToList( owner, property, items ):
   """Append the items to owner.property."""

   if type(property) != str:
      property = property.name()

   if type(items) != list:
      items = [ items ]

   exec( 'owner.%s += items' % property )


def removeItemsFromList( owner, property, items ):
   """Remove all occurrences of the items from owner.property."""

   if type(property) != str:
      property = property.name()

   if type(items) != list and type(items) != tuple:
      items = [ items ]

   current = getattr( owner, property )
   exec( 'owner.%s = [ e for e in current if not e in items ]' % property )


def removeFromList( owner, property, item ):
   """Remove the first occurrence of item from owner.property."""

   if type(property) != str:
      property = property.name()

   current = getattr( owner, property )
   current.remove( item )
   exec( 'owner.%s = current' % property )
