# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
PathNameBuilder is a class that builds path names using
inputs from HardCoded Legals and parameters set by the user
"""

from os.path import join
from RTTpath import RTTpath

class PathNameBuilder:
  def __init__(self, argBag, hardCoded, legals, siteSpecificsObj, logger):

    self.release         = argBag.release
    self.cmtConfig       = argBag.cmtConfig
    self.originalBranch  = argBag.originalBranch
    self.isNightly       = argBag.isNightly
    self.topProject      = argBag.topProject
    self.otherProject    = argBag.otherProject
    self.distArea        = argBag.distArea

    self.hardCoded       = hardCoded
    self.legals          = legals
    self.logger          = logger

  def latestCopiedReleaseLink(self, key, topProject):
    l = join(
      self.distArea,
      self.hardCoded.getNightlyPathFragment(key[1]),
      topProject,
      self.hardCoded.getLatestCopiedReleaseLinkName(key)
      )
    
    return l

  def pathToFindReleaseDate(self):
    "needed by the NICOS class"

    path = RTTpath(
      self.distArea,
      self.hardCoded.nightlyDirIfAny(self.originalBranch, self.isNightly),
      self.topProject,
      self.legals.convertLogicalNightly(self.release),
      )
    
    if not path.exists():
      m  = 'PathNameBuilder::'
      m += 'pathToFindReleaseDate()'
      m += ' non-existent path  ' + str(path)
      self.logger.warning(m)
      self.logger.warning('isNightly %s' % self.isNightly)
    return path
    
  def pathToNICOSLatestCopiedLink(self):
    "needed by StampObject to determine latest nightly release name."
    
    key  = (self.cmtConfig, self.originalBranch, self.topProject) 
    path = RTTpath(self.latestCopiedReleaseLink(key, self.topProject))
    
    if not path.exists():
      m  = 'PathNameBuilder::'
      m += 'pathToNICOSLatestCopiedLink()'
      m += ' non-existent path  ' + str(path)
      self.logger.warning(m)
      
    return path
    
  def getReleasePath(self, cvsPackage):
    path = RTTpath(cvsPackage.getPackageCmtPath())

    if not path.exists():
      m  = 'PathNameBuilder::'
      m += 'getReleasePath()'
      m += ' non-existent path  ' + str(path)
      self.logger.warning(m)
      
    return path

  def getInstallArea(self, cvsPackage):
    path =  RTTpath(str(self.getReleasePath(cvsPackage)),
                    self.hardCoded.installAreaDirName())
    
    if not path.exists():
      m  = 'PathNameBuilder::'
      m += 'getInstallArea()'
      m += ' non-existent path  ' + str(path)
      self.logger.warning(m)
        
    return path

  def getShareArea(self, cvsPackage):
    path = RTTpath(str(cvsPackage.packageSharePath))

    if not path.exists():
      m  = 'PathNameBuilder::'
      m += 'getShareArea()'
      m += ' non-existent path  ' + str(path)
      self.logger.warning(m)
     
    return path
    
  def cmtDir(self, cmtPath):
    path = RTTpath(cmtPath, self.hardCoded.cmtAreaDirName())
    
    if not path.exists():
      m  = 'PathNameBuilder::'
      m += 'cmtDir()'
      m += ' non-existent path  ' + str(path)
      self.logger.warning(m)
      
    return path
  
