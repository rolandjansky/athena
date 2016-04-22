# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class Spec:

  def __init__ ( self, glob, variations = [] ):
    self.Glob = glob
    self.Vars = variations

  def __str__ ( self ):
    return ' Global Value: %s, Variations: %s' % ( str( self.Glob ), str( self.Vars ) )
