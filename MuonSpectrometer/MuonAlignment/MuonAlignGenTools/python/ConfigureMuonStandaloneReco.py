# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   ConfigureMuonStandaloneReco.py
## @brief:  Used to setup muon standalone reconstruction for 
#           track-based alignment.  
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Include import include

from MuonRecExample.MuonRecFlags import muonRecFlags, mooreFlags, muonboyFlags
from RecJobTransforms.RecConfig import recConfig
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
from RecExConfig.RecFlags import rec 

from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

## ConfigureMuonStandaloneReco sets up muon reco for track-based alignment
class ConfigureMuonStandaloneReco () :

    def configureReco(self) :
        from AthenaCommon.DetFlags import DetFlags
        DetFlags.Muon_setOn()
        DetFlags.Calo_setOff()
        
        recAlgsFlags.doMuonIDStandAlone = False
        recAlgsFlags.doMuonIDCombined   = False

        # add some more flags available for standalone running
        import MuonRecExample.MuonRecStandaloneFlags
        
         # configure flags so that only Muon Standalone reco is run
        import MuonRecExample.MuonRecStandaloneOnlySetup

        muonRecFlags.useAlignmentCorrections = mualign.useOpticalAlignment()

        # cosmics setup
        #if align.dataType()=='cosmic' or align.dataType()=='simCosmic':

        ##### no more flags after this line #####
        print "align.dataType()=",align.dataType()
        if align.dataType()=='data' or align.dataType()=='cosmic' :
            muonRecTopOptions="MuonAlignGenTools/MuonDataRec_myTopOptions.py"
        else :
            muonRecTopOptions="MuonRecExample/MuonRec_topOptions.py"
        try:
            include(muonRecTopOptions)
            
        ###### put any user finetuning before this line #####
        except:
            # always write config so far for debugging
            from AthenaCommon.ConfigurationShelve import saveToAscii
            saveToAscii("config.txt")
            # but still exit with error
            raise
        
