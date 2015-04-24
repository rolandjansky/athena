# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# +---------------------------------------------+
# | Vassilis D. Kazazakis - DBInfo.py v1.0	|
# | Returns COOL DB connection information      |
# | Updated by M.D'Onofrio: new folder added    |
# | for history plots 				|
# +---------------------------------------------+

# user & password information should be handled by each individual program

def getReadServer():
    return "ATLAS_COOLPROD"

def getWriteServer():
    #return "ATLAS_COOLWRITE"
    return ""

def getFolder():
    #return "/GLOBAL/DQM/HIST"
    return "/GLOBAL/DETSTATUS/DQMFOFL"

def getFolders():
    #return "/GLOBAL/DQM/HIST"
    return ["/GLOBAL/DETSTATUS/DQMFOFL", "/GLOBAL/DETSTATUS/SHIFTOFL"]

def getFolderH():
    #return "/GLOBAL/DQM/HIST"
    return "/GLOBAL/DETSTATUS/DQMFOFLH"

def getDbName():
#	return "OFLP200"
    return "CONDBR2"
#	return "STRMP200"

def getSchema():
    #return "ATLAS_COOLOFL_GLOBAL"
    return "MyCOOL_histo.db"

def getChannelDict():
    from DetectorStatus.DetStatusLib import DetStatusNames
    dsn = DetStatusNames()
    channelDict = dsn.namedict.copy()
##     channelDict = dict([])
    
##     # the following channels are for the M4 #
##     #channelDict["default"] = 0
##     #channelDict["default_dqmf"] = 1
    
##     # DB Structure as in DBQuery (from DetStatusLib.py)
    
##     # Inner Detector 
##     channelDict["PIXB"] = 101
##     channelDict["PIX0"] = 102 #new: Pixel B-layer 
##     channelDict["PIXEA"] = 104
##     channelDict["PIXEC"] = 105
    
##     channelDict["SCTB"] = 111
##     channelDict["SCTEA"] = 114
##     channelDict["SCTEC"] = 115

##     channelDict["TRTB"] = 121
##     channelDict["TRTEA"] = 124
##     channelDict["TRTEC"] = 125

##     # obsolete 
##     #channelDict["IDGB"] = 131
##     #channelDict["IDGEA"] = 134
##     #channelDict["IDGEC"] = 135
##     #channelDict["IDAB"] = 141
##     #channelDict["IDAEA"] = 144
##     #channelDict["IDAEC"] = 145
##     # new ID Alignement and Global
##     channelDict["IDGL"] = 130
##     channelDict["IDAL"] = 140
##     channelDict["IDBS"] = 150
##     channelDict["IDPF"] = 160
##     channelDict["IDVX"] = 161
##     channelDict["IDBCM"] = 170
	
##     # LAr 
##     channelDict["EMBA"] = 202
##     channelDict["EMBC"] = 203
##     channelDict["EMECA"] = 204
##     channelDict["EMECC"] = 205
##     channelDict["HECA"] = 214
##     channelDict["HECC"] = 215
##     channelDict["FCALA"] = 224
##     channelDict["FCALC"] = 225
    
##     # TileCal
##     channelDict["TIGB"] = 230
##     channelDict["TILBA"] = 232
##     channelDict["TILBC"] = 233
##     channelDict["TIEBA"] = 234
##     channelDict["TIEBC"] = 235
    
##     # MBTS
##     channelDict["MBTSA"] = 244
##     channelDict["MBTSC"] = 245
    
##     # TileCal+LAr
##     channelDict["CALBA"] = 251
##     channelDict["CALEA"] = 254
##     channelDict["CALEC"] = 255
    
##     # Muon Detectors 
##     channelDict["MDTBA"] = 302
##     channelDict["MDTBC"] = 303
##     channelDict["MDTEA"] = 304
##     channelDict["MDTEC"] = 305
##     channelDict["RPCBA"] = 312
##     channelDict["RPCBC"] = 313
##     channelDict["TGCEA"] = 324
##     channelDict["TGCEC"] = 325
##     channelDict["CSCEA"] = 334
##     channelDict["CSCEC"] = 335
    
##     # Lumi
##     channelDict["LCD"] = 350
##     channelDict["LCDA"] = 353
##     channelDict["LCDC"] = 354
##     channelDict["ALFA"] = 360
##     channelDict["ZDC"] = 370
    
##     # Trigger
##     channelDict["L1CAL"] = 401
##     channelDict["L1MUB"] = 402
##     channelDict["L1MUE"] = 403
##     channelDict["L1CTP"] = 404
##     channelDict["TRCAL"] = 411
##     channelDict["HLTEF"] = 412
##     channelDict["TRBJT"] = 421
##     channelDict["TRBPH"] = 422
##     channelDict["TRCOS"] = 423
##     channelDict["TRELE"] = 424
##     channelDict["TRGAM"] = 425
##     channelDict["TRJET"] = 426
##     channelDict["TRMET"] = 427
##     channelDict["TRMBI"] = 428
##     channelDict["TRMUO"] = 429
##     channelDict["TRTAU"] = 430
##     channelDict["TRIDT"] = 431
    
##     channelDict["LUMI"] = 450
##     channelDict["RUNCLT"] = 460
##     channelDict["RCOPS"] = 461

##     channelDict["ATLGL"] = 480
##     channelDict["ATLSOL"] = 481
##     channelDict["ATLTOR"] = 482
    
##     # Physics Objects 
##     channelDict["EIDB"] = 501
##     channelDict["EIDCR"] = 502
##     channelDict["EIDE"] = 503
##     channelDict["PIDB"] = 505
##     channelDict["PIDCR"] = 506
##     channelDict["PIDE"] = 507
##     channelDict["EIDF"] = 508
##     channelDict["EIDSOFT"] = 509
##     channelDict["MSTACO"] = 510
##     channelDict["MMUIDCB"] = 511
##     channelDict["MMUIDVX"] = 512
##     channelDict["MMUGIRL"] = 513
##     channelDict["MMUBOY"] = 514
##     channelDict["MMUIDSA"] = 515
##     channelDict["MMUTAG"] = 516
##     channelDict["MMTIMO"] = 517
##     channelDict["MCMUTAG"] = 518
##     channelDict["MCALLHR"] = 519
##     channelDict["JETB"] = 521
##     channelDict["JETEA"] = 524
##     channelDict["JETEC"] = 525
##     channelDict["JETFA"] = 526
##     channelDict["JETFC"] = 527
##     channelDict["MET"] = 530
##     channelDict["METCALO"] = 531
##     channelDict["METMUON"] = 532
##     channelDict["BTGLIFE"] = 541
##     channelDict["BTGSOFTE"] = 544
##     channelDict["BTGSOFTM"] = 545
##     channelDict["TAUB"] = 551
##     channelDict["TAUCR"] = 552
##     channelDict["TAUE"] = 553
	
    #########################################
    
    return channelDict
    
def getChannelDictH():
    # This is all deprecated of course
    print 'Why are you calling this function? This is very very deprecated ... - ponyisi, 17/6/10'
    channelDict = dict([])


    channelDict["/InnerDetector/IDAlignment/ExtendedTracks_NoTriggerSelection/GenericTracks/Npixhits_per_track"]=10039
    channelDict["/InnerDetector/IDAlignment/ExtendedTracks_NoTriggerSelection/GenericTracks/Nscthits_per_track"]=10040
    channelDict["/InnerDetector/IDAlignment/ExtendedTracks_NoTriggerSelection/GenericTracks/Ntrthits_per_track"]=10041
    
    channelDict["/InnerDetector/IDAlignment/Tracks_NoTriggerSelection/HitEfficiencies/measurements_eff_vs_layer_barrel"]=10042
    
    channelDict["/InnerDetector/IDAlignment/Tracks_NoTriggerSelection/GenericTracks_Detail/Npixhits_per_track_barrel"]=10043  
    channelDict["/InnerDetector/IDAlignment/Tracks_NoTriggerSelection/GenericTracks_Detail/Npixhits_per_track_eca"]=10044  
    channelDict["/InnerDetector/IDAlignment/Tracks_NoTriggerSelection/GenericTracks_Detail/Npixhits_per_track_ecc"]=10045  
    channelDict["/InnerDetector/IDAlignment/Tracks_NoTriggerSelection/GenericTracks_Detail/Nscthits_per_track_barrel"]=10046  
    channelDict["/InnerDetector/IDAlignment/Tracks_NoTriggerSelection/GenericTracks_Detail/Nscthits_per_track_eca"]=10047  
    channelDict["/InnerDetector/IDAlignment/Tracks_NoTriggerSelection/GenericTracks_Detail/Nscthits_per_track_ecc"]=10048  
    channelDict["/InnerDetector/IDAlignment/Tracks_NoTriggerSelection/GenericTracks_Detail/Ntrthits_per_track_barrel"]=10049  
    channelDict["/InnerDetector/IDAlignment/Tracks_NoTriggerSelection/GenericTracks_Detail/Ntrthits_per_track_eca"]=10050 
    channelDict["/InnerDetector/IDAlignment/Tracks_NoTriggerSelection/GenericTracks_Detail/Ntrthits_per_track_ecc"]=10051  


    channelDict["/InnerDetector/IDAlignment/PrimVtx/NoTriggerSelection/pvChiSqDof"]=10082
    channelDict["/InnerDetector/IDAlignment/PrimVtx/NoTriggerSelection/pvX"]=10083
    channelDict["/InnerDetector/IDAlignment/PrimVtx/NoTriggerSelection/pvY"]=10084
    channelDict["/InnerDetector/IDAlignment/PrimVtx/NoTriggerSelection/pvZ"]=10085

    channelDict["/InnerDetector/IDAlignment_Perf/Jpsi-->mumu/NoTriggerSelection/Jpsi_invmass"]=100102
    channelDict["/InnerDetector/IDAlignment_Perf/Kshort-->pipi/NoTriggerSelection/ks_mass"]=100117

        
    channelDict["/InnerDetector/IDAlignment_Perf/Z-->ee/NoTriggerSelection/Zee_Eopasym_perevent"]=100198
    channelDict["/InnerDetector/IDAlignment_Perf/Z-->ee/NoTriggerSelection/Zee_Eop_incl"]=100199
    channelDict["/InnerDetector/IDAlignment_Perf/W-->enu/NoTriggerSelection/Wenu_Eop_incl"]=100200


    channelDict["/InnerDetector/IDAlignment_Perf/Z-->mumu/NoTriggerSelection/z-mass_trks"]=100215

        
    channelDict["/InnerDetector/Pixel/PixelExpert/Timing/m_Pixel_track_Lvl1A"]=100220
    channelDict["/InnerDetector/Pixel/PixelExpert/ToT/m_Pixel_track_cluster_ToT"]=100221 
    channelDict["/InnerDetector/Pixel/PixelExpert/General/m_Pixel_track_clus_groupsize"]=100222



    #########################################
        
    return channelDict

