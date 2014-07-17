# This is a hack to use the TRK_VALID ntuple with some stuff turned off

InDetFlags.doPixelTrkNtuple.unlock()
InDetFlags.doSctTrkNtuple.unlock()
InDetFlags.doTrtTrkNtuple.unlock()
InDetFlags.doPixelClusterNtuple.unlock()
InDetFlags.doSctClusterNtuple.unlock()

InDetFlags.doPixelTrkNtuple.set_Value_and_Lock( False )
InDetFlags.doSctTrkNtuple.set_Value_and_Lock( False )
InDetFlags.doTrtTrkNtuple.set_Value_and_Lock( False )
InDetFlags.doPixelClusterNtuple.set_Value_and_Lock( False )
InDetFlags.doSctClusterNtuple.set_Value_and_Lock( False )

InDetFlags.doTrkNtuple.set_Value_and_Lock(True)
