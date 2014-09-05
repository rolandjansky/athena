# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#file  :   TileCondProxyConf.py
#author:   nils.gollub@cern.ch 
#modified: lukas.pribyl@cern.ch

from TileConditions.TileConditionsConf import TileCondProxyFile_TileCalibDrawerFlt_ as TileCondProxyFileFlt
from TileConditions.TileConditionsConf import TileCondProxyFile_TileCalibDrawerBch_ as TileCondProxyFileBch
from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerFlt_ as TileCondProxyCoolFlt
from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerBch_ as TileCondProxyCoolBch
from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerOfc_ as TileCondProxyCoolOfc

from IOVDbSvc.CondDB import conddb

#
#____________________________________________________________________________
def getTileCondProxy(technology, type, source, name, **kwargs):

    tool = None

    #=============================================================
    #=== FILE
    #=============================================================
    if technology=='FILE':
        if   type=='Flt':
            tool = TileCondProxyFileFlt(name, Source = source)
        elif type=='Bch':
            tool = TileCondProxyFileBch(name, Source = source)

    #=============================================================
    #=== COOL
    #=============================================================
    elif technology=='COOL':

        from TileConditions.TileCoolMgr import tileCoolMgr
        folder  = tileCoolMgr.getFolder(source)
        folder2 = tileCoolMgr.getFolderTwo(source)
        tag     = tileCoolMgr.getTag(source)       
        conn    = tileCoolMgr.getDbConn(source)      
        key     = tileCoolMgr.getKey(source)

        folderstr  = ""
        folderstr2 = ""

        if(len(tag)):
            folderstr  = folder + " <tag>" + tag + "</tag> <key>" + key + "</key>"
            folderstr2 = folder2 + " <tag>" + tag + "</tag> <key>" + key + "</key>"
        else:
            folderstr  = folder + " <key>" + folder + "</key>"
            folderstr2 = folder2 + " <key>" + folder2 + "</key>"

        #=== add folders according to their COOL schema split type    
        if tileCoolMgr.isSplitMC(source):     
            conddb.addFolderSplitMC(conn, folderstr, folderstr2)
        if tileCoolMgr.isOfflineOnly(source):     
            conddb.addFolder(conn, folderstr)
        if tileCoolMgr.isSplitOnline(source):     
            conddb.addFolderSplitOnline(conn, folderstr, folderstr2)
        if tileCoolMgr.isSqlite(source):     
            conddb.addFolder(conn, folderstr)            

        #=== create the proxies
        if   type=='Flt':
            tool = TileCondProxyCoolFlt(name, Source = key)
        elif type=='Bch':
            tool = TileCondProxyCoolBch(name, Source = key)
        elif type=='Ofc':
            tool = TileCondProxyCoolOfc(name, Source = key)
            
    return tool

#
#____________________________________________________________________________
#def removeTileCondProxy(folder, **kwargs):
#    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
#    iovDbSvc=svcMgr.IOVDbSvc
 
#    for f in iovDbSvc.Folders:
#        for parts in f.replace('<',' ').replace('>',' ').split():
#            if folder in parts:
#                iovDbSvc.Folders.remove(f)
