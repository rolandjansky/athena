# jobOptions fragment to setup writing of LAr affected region


if not hasattr(topSequence, 'LArAffectedRegionAlg'):
    from LArAffectedRegion.LArAffectedRegionConf import LArAffectedRegionAlg
    theLArAffectedRegionAlg = LArAffectedRegionAlg()

    theLArAffectedRegionAlg.ReadingFromBytestream = False

    topSequence+=theLArAffectedRegionAlg

    from IOVDbSvc.CondDB import conddb
    conddb.addFolder ('', '/LAR/LArAffectedRegionInfo<metaOnly/>')
