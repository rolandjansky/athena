# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# JobOption fragment to set up the AtlasFieldSvc
# Valerio Ippolito - Harvard University

# inspired by https://svnweb.cern.ch/trac/atlasoff/browser/MuonSpectrometer/MuonCnv/MuonCnvExample/trunk/python/MuonCalibConfig.py

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon import CfgMgr
MagField__AtlasFieldSvc=CfgMgr.MagField__AtlasFieldSvc

#--------------------------------------------------------------

def AtlasFieldSvc(name="AtlasFieldSvc",**kwargs):
  if athenaCommonFlags.isOnline():
    kwargs.setdefault( "UseDCS", False )
    kwargs.setdefault( "UseSoleCurrent", 7730 )
    kwargs.setdefault( "UseToroCurrent", 20400 )
  else:
    kwargs.setdefault( "UseDCS", True )
  
  return CfgMgr.MagField__AtlasFieldSvc(name,**kwargs)

def AtlasFieldCacheCondAlg(name="AtlasFieldCacheCondAlg",**kwargs):
  if athenaCommonFlags.isOnline():
    kwargs.setdefault( "UseDCS", False )
    # currents for scaling wrt to map currents
    kwargs.setdefault( "UseSoleCurrent", 7730 )
    kwargs.setdefault( "UseToroCurrent", 20400 )
    kwargs.setdefault( "LockMapCurrents", True )
  else:
    kwargs.setdefault( "UseDCS", True )
  return CfgMgr.MagField__AtlasFieldCacheCondAlg(name,**kwargs)

def AtlasFieldMapCondAlg(name="AtlasFieldMapCondAlg",**kwargs):
  if athenaCommonFlags.isOnline():
    # For old tests, must update field from defautl jobOpt
    kwargs.setdefault( "UseMapsFromCOOL", False )
    # online has the map loaded at start (for the future - uncomment if needed)
    # kwargs.setdefault( "LoadMapOnStart", True )

  return CfgMgr.MagField__AtlasFieldMapCondAlg(name,**kwargs)



def H8FieldSvc(name="H8FieldSvc",**kwargs):
  return CfgMgr.MagField__H8FieldSvc(name,**kwargs)

def GetFieldSvc(name="AtlasFieldSvc",**kwargs):
  if GlobalFlags.DetGeo == 'ctbh8':
    return H8FieldSvc(name, **kwargs)
  else:
    return AtlasFieldSvc(name, **kwargs)

def GetFieldCacheCondAlg(name="AtlasFieldCacheCondAlg",**kwargs):
    return AtlasFieldCacheCondAlg(name, **kwargs)
    
def GetFieldMapCondAlg(name="AtlasFieldMapCondAlg",**kwargs):
    return AtlasFieldMapCondAlg(name, **kwargs)
