# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#file: TileCoolMgr.py
#author: nils.gollub@cern.ch
#modifications: lukas.pribyl@cern.ch

from TileConditions.TileCoolMgrClass import TileCoolMgr
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from IOVDbSvc.CondDB import conddb

import string

tileCoolMgr = TileCoolMgr(isMC=(globalflags.DataSource()!='data'), 
                          isOnline=athenaCommonFlags.isOnline(),
                          dbInstance=conddb.GetInstance())

def GetTileOfcCoolSource(ofcType, runType = 'PHY'):
    return 'Ofc' + string.capwords(ofcType,'/').replace('/','') + runType.lower().capitalize()

def AddTileOfcCoolSource(ofcType, runType = 'PHY', splitOnline = False):
    ofcSource = GetTileOfcCoolSource(ofcType, runType)
    ofcFullType = ofcType.upper() + '/' + runType.upper()
    ofcFolder = '/TILE/ONL01/FILTER/' + ofcFullType
    if splitOnline:
        ofcOfflineFolder = '/TILE/OFL02/FILTER/' + ofcFullType
        tileCoolMgr.addSource(ofcSource, ofcFolder, defConnStr, "", ofcOfflineFolder, 'SplitOnline')
    else:
        tileCoolMgr.addSource(ofcSource, ofcFolder, defConnStr, "", ofcFolder, 'SplitMC')



