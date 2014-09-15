# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   ConfigureFullReco.py
## @brief:  Used to setup full reconstruction for 
#           track-based alignment.  
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Include            import include
from MuonRecExample.MuonRecFlags     import muonRecFlags
from RecJobTransforms.RecConfig      import recConfig
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

## ConfigureFullReco sets up full reconstruction for track-based alignment.
#  
class ConfigureFullReco () :

    def configureReco(self) :

        from AthenaCommon.DetFlags import DetFlags
        DetFlags.Muon_setOn() 
        DetFlags.ID_setOn()

        #==============================================================
        # Job Configuration parameters:
        #==============================================================
        # Signal to include file that this is not the postOptions,
        # in case the jobConfig is also the postOptions
        isPostOptions = False
                
        #--- Configuration part: ---------------------------------------
        doRestrictedESD = recConfig.restrictedESD
        doMinimalRec = recConfig.doMinimalRec
        doBackTracking = recConfig.doBackTracking

        indetRecos = ( 'xKalman', 'iPatRec', 'newTracking' )
        doAODall=False  # TODO: = recConfig.<something>
        # set all known flags to False
        for reco in indetRecos:
            exec 'do%s = False' % reco

        #
        # set requested flags to True
        #
        recosOn = []
        recosAll = list(indetRecos)
        if 'all' in recConfig.doIndetReco:
            # add all known indet recos
            for reco in indetRecos: recosOn.append(reco)

        # add all requested recos (avoid doubles)
        for reco in recConfig.doIndetReco:
            if reco != 'all':
                if reco not in recosOn : recosOn.append(reco)
                if reco not in recosAll: recosAll.append(reco)
        for reco in recosOn:
            exec 'do%s = True' % reco

        # Determine doAODall flag
        doAODall = True
        for reco in indetRecos:
            if reco not in recosOn:
                doAODall = False
                break

        # Switch off some parts of combined reco
        if 'doObjMissingET' in recConfig.combinedRecoOff:
            doObjMissingET=False

        include( "RecExCond/RecExCommon_flags.py" )

        # Switch off sub-detectors
        for detOff in recConfig.detectorsOff:
            cmd='DetFlags.%s_setOff()' % detOff
            try:
                exec cmd
                recoLog.info(cmd)
            except AttributeError:
                recoLog.error("Can not switch off detector %s", detOff)
                raise

        if 'InDetFlags' in dir():
           InDetFlags.usePrimVertexZcoordinate(recConfig.usePrimVertexZcoordinate)
           InDetFlags.doSlimming=recConfig.enableTrackSlimming

           
        #--------------------------------------------------------------
        # Do the job!
        #--------------------------------------------------------------
        include( "RecExCommon/RecExCommon_topOptions.py" )

        #
        # Include user options
        #
        if recConfig.postOptionsESD:
            # Signal to include file that this is the postOptions,
            # in case the jobConfig is also the postOptions
            isPostOptions = True
            include( recConfig.postOptionsESD )
        #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        #if align.doVP1():
        #    from VP1Algs.VP1AlgsConf import VP1Alg
        #    topSequence += VP1Alg()
	
    def configureCosmicReco(self) :

        from AthenaCommon.DetFlags import DetFlags
        DetFlags.Muon_setOn() 
        DetFlags.ID_setOn()

        #==============================================================
        # Job Configuration parameters:
        #==============================================================
        # Signal to include file that this is not the postOptions,
        # in case the jobConfig is also the postOptions
        isPostOptions = False
                
        #--- Configuration part: ---------------------------------------
        doRestrictedESD = recConfig.restrictedESD
        doMinimalRec = recConfig.doMinimalRec
        doBackTracking = recConfig.doBackTracking

        indetRecos = ( 'xKalman', 'iPatRec', 'newTracking' )
        doAODall=False  # TODO: = recConfig.<something>
        # set all known flags to False
        for reco in indetRecos:
            exec 'do%s = False' % reco

        #
        # set requested flags to True
        #
        recosOn = []
        recosAll = list(indetRecos)
        if 'all' in recConfig.doIndetReco:
            # add all known indet recos
            for reco in indetRecos: recosOn.append(reco)

        # add all requested recos (avoid doubles)
        for reco in recConfig.doIndetReco:
            if reco != 'all':
                if reco not in recosOn : recosOn.append(reco)
                if reco not in recosAll: recosAll.append(reco)
        for reco in recosOn:
            exec 'do%s = True' % reco

        # Determine doAODall flag
        doAODall = True
        for reco in indetRecos:
            if reco not in recosOn:
                doAODall = False
                break

        # Switch off some parts of combined reco
        if 'doObjMissingET' in recConfig.combinedRecoOff:
            doObjMissingET=False

        include( "RecExCond/RecExCommon_flags.py" )

        # Switch off sub-detectors

        include( "RecJobTransforms/csc_reco_detflags.py" )

        if 'InDetFlags' in dir():
           InDetFlags.usePrimVertexZcoordinate(recConfig.usePrimVertexZcoordinate)
           InDetFlags.doSlimming=recConfig.enableTrackSlimming

 
        #--------------------------------------------------------------
        # Do the job!
        #--------------------------------------------------------------
        include( "RecExCommon/RecExCommon_topOptions.py" )

        #
        # Include user options
        #
        if recConfig.postOptionsESD:
            # Signal to include file that this is the postOptions,
            # in case the jobConfig is also the postOptions
            isPostOptions = True
            include( recConfig.postOptionsESD )
        #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        #if align.doVP1():
        #    from VP1Algs.VP1AlgsConf import VP1Alg
        #    topSequence += VP1Alg()
