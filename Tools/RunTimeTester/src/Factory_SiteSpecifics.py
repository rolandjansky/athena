# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Factory_BaseClass    import Factory
from  SiteSpecifics       import * 


# -------------------------------------------------------------------------
# SiteSpeocifics Factory
# -------------------------------------------------------------------------

class SiteSpecificsFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)
    
    self.dict = {
      "CERN"     : CERNSiteSpecifics,
      "CERNslc4" : CERNslc4SiteSpecifics,
      "PetersMac": PetersMacSiteSpecifics,
      "UCLslc3"  : UCLslc3SiteSpecifics,
      "Lancaster": LancasterSiteSpecifics
      }



  def  create(self, site):
    """ provide an instance of a SiteSpecifics:
You give the site of a group - and you get an object with
details for that site."""
      
    return self.dict[site]()

# -------------------------------------------------------------------------
