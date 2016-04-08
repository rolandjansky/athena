#*******************************************************************
#
# JobOption to define global tags for Calo testbeam data 
#
# Important note : you need, either in your topOption of here
#
# the lines :
# from RecExCommon.GlobalFlags import GlobalFlags
# from RecExCommon.DetFlags import DetFlags
# 
# GlobalFlags.DetGeo.set_ctbh8()
# 
# if not 'doSim' in dir():
#     doSim=False   ( or true )
#
# if doSim:
#     # Reading G4 simulated data from Pool
#     GlobalFlags.DataSource.set_geant4()
#     GlobalFlags.InputFormat.set_pool()
# else:
#     # Reading ByteStream real data
#     GlobalFlags.DataSource.set_data()
#     GlobalFlags.InputFormat.set_bytestream()    
#
#
#*******************************************************************

# make sure that the proper DB is used for Table position reading
#LArCondCnvDbServer = 'atlobk02.cern.ch'

# set default flags if not done by a topOption :
# ---------------------------------------------


#set RecExTB flags if not done by a topOption :
if not 'doLAr' in dir():
    doLAr=True
    DetFlags.digitize.LAr_on()
if not 'doTile' in dir():
    doTile=True
    DetFlags.digitize.Tile_on()

#set RecExCommon flags if not done by a topOption :

# Sliding Window on EM
if not 'doEmCluster' in dir():
    doEmCluster=doLAr

# Combined Sliding Window?
if not 'doCaloCluster' in dir():
    doCaloCluster=doLAr and doTile

# Combined TopoCluster?
if not 'doCaloTopoCluster' in dir():
    doCaloTopoCluster=doLAr or doTile

#EMTB style clustering
if not 'doEMTBCluster' in dir():
    doEMTBCluster=doLAr


if not 'doLArHVCorrection' in dir():
    doLArHVCorrection=False
    

#now real internal business starts:
#-----------------------------------

if globalflags.InputFormat.is_zebra():
    doCaloNoise=False
    doLArCellDirect=True


if globalflags.DataSource.is_geant4():
    doCaloNoise=True
