# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

include("LArBadChannelTool/LArBadChannel2Ascii.py")


#Some overwrites to generate Executive Summary file with missing FEB information:
from IOVDbSvc.CondDB import conddb
conddb.addFolder("LAR_OFL","/LAR/BadChannelsOfl/MissingFEBs")
theLArBadChannelTool.CoolMissingFEBsFolder="/LAR/BadChannelsOfl/MissingFEBs"

theLArBadChannels2Ascii.FileName="CompleteBadChannelList.txt"
theLArBadChannels2Ascii.WithMissing=True
theLArBadChannels2Ascii.ExecutiveSummaryFile="ForIsabelle.txt"

svcMgr.IOVDbSvc.GlobalTag="CONDBR2-ES1PA-2014-01" 
