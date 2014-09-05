# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class InDetSGDeletionAlg:
    def __init__(self, container = "TrackCollection#", key = None):
        #
        # --- Delete the storegate container of the given type and key
        #     N.B.  Don't delete anything that makes the ESD/AOD
        #
        if not container.endswith('#'): container += '#'

        # --- Do nothing if deletion not configured
        from InDetRecExample.InDetJobProperties import InDetFlags
        if not InDetFlags.doSGDeletion(): return

        if isinstance(key, list):
            # Delete all containers in list
            for k in key: self.deletionAlg(container, k)
        else:
            # Delete single container
            self.deletionAlg(container, key)

    def deletionAlg(self, container, key):
        if not isinstance(key, str):
            print "InDetSGDeletionAlg: WARNING Key is not a string, ignoring!"
            return
        
        from InDetRecExample.InDetJobProperties import InDetFlags
        from AthenaCommon                       import CfgMgr
        from AthenaCommon.AlgSequence           import AlgSequence

        topSequence = AlgSequence()    

        # Call SGDeleteAlg
        InDetDelAlg = CfgMgr.SGDeleteAlg(name = "InDetSGDel_" + key)
        InDetDelAlg.ToDelete.ItemList = [container + key]
        topSequence += InDetDelAlg

        if InDetFlags.doPrintConfigurables():
            print InDetDelAlg
