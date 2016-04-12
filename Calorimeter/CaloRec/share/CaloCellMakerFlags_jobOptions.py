# ---- choose subdetectors to decode

class CaloCellMakerFlags:
  # ---- step to be run
  # the hit => RawChannel is handled by existing DetFlags.digitize flags
  makeCellFromDigit=False
  makeCellFromRawChannel=True
  # direct hit to cell for LAr  
  doLArHitToCellDirect=False
  # direct hit to raw channel for tile
  doTileHitToRawChannelDirect=False
  doCaloNoise=True
  # make compactified/decompactified cell container *in addition* to existing one
  doCompactCellInAddition=False
  # make compactified/decompactified cell container to replace existing one
  doCompactCellInstead=False
