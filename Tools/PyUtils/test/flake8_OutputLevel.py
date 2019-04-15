#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Test for the OutputLevel flake8 plugin

DEBUG = 1

class C(object):
   def __init__(self):
      self.OutputLevel = DEBUG

c = C()

c.OutputLevel = DEBUG
c.OutputLevel = DEBUG # noqa
setattr(c, 'OutputLevel', DEBUG)
if c.OutputLevel > DEBUG:
    pass
