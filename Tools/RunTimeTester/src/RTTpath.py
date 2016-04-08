# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os.path

class RTTpath:
  """class that holds various path fragments, and combines them to form
   the most complete path possible"""
  
  def __init__(self, *args):
    self.segments = []
      
    # sometimes an RTTpath is made with None in the arguments.
    # this means 'no such string'. Remove these Nones
    self.segments.extend([str(a) for a in args if a != None])
    
  def prepend(self, *args):
    segments = [str(s) for s in args]
    hold = self.segments
    self.segments.extend(hold)
  def postpend(self, *args):

    segments = [str(s) for s in args]
    self.segments.extend(segments)
  def path(self):
    "combine the segments to form a path"
    if not len(self.segments):
      return '' 
    path = reduce( os.path.join, self.segments)
    path = os.path.normpath(path)
    return path
  def isabs(self):
    if not len(self.segments): return False
    return os.path.isabs(self.path())
  def exists(self):
    if not len(self.segments): return False
    return os.path.exists(self.path())
  def __str__(self):
    if not len(self.segments): return ''
    return self.path()

class CheckedPath(RTTpath):
  def __init__(self, segments = []):
    RTTpath.__init__(self, segments)

  def __str__(self):
    if not self.exists():
      return ''
    return RTTpath.__str__(self)
    
if __name__ == '__main__':
  path = RTTpath('.', 'RTTpath.py')

  test = 0
  
  test = 1
  print 'Test %d' % test
  print 'path',path
  print 'isabs', path.isabs()
  print 'exists', path.exists()
  
  here = os.getcwd()
  print here
  path.prepend(here)
  
  
  test = 2
  print 'Test %d' % test
  print 'path',path
  print 'isabs', path.isabs()
  print 'exists', path.exists()
  

  path = RTTpath(here)
  path.postpend('RTTpath.py')
  
  
  test = 3
  print 'Test %d' % test
  print 'path',path
  print 'isabs', path.isabs()
  print 'exists', path.exists()
  

  path1 = RTTpath(here)
  path2 = RTTpath('Path.py')
  path2.prepend(path1)

  
  
  test = 4
  print 'Test %d' % test
  print 'path',path
  print 'isabs', path.isabs()
  print 'exists', path.exists()
  
