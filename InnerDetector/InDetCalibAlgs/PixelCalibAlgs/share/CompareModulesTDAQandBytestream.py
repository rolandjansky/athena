
## GlobalFlags

from AthenaCommon.GlobalFlags import globalflags

globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'




### define the job

from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()

from PixelCalibAlgs.PixelCalibAlgsConf import CompareModulesTDAQandBytestream

topSequence += CompareModulesTDAQandBytestream()


from glob import glob
collection = [

#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0002._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0003._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0004._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0005._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0006._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0007._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0008._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0009._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0010._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0011._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0012._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0013._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0014._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0015._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0016._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0017._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0018._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0019._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0020._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0021._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0022._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0023._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0024._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0025._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0026._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0027._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0028._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0029._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0030._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0031._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0032._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0033._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0034._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0035._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0036._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0037._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0038._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0039._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0040._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0041._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0042._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0043._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0044._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0080._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0081._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0082._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0083._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0084._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0085._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0086._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0087._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0088._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0089._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0090._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0091._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0092._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0093._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0094._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0095._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0096._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0097._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0098._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0099._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0100._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0101._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0102._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0103._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0104._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0105._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0106._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0107._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0108._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0109._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0110._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0111._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0112._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0113._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0114._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0115._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0116._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0117._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0118._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0119._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0120._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0121._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0122._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0123._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0124._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0125._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0126._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0127._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0128._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0129._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0130._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0131._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0132._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0133._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0134._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0135._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0136._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0137._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0138._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0139._0001.1',
#    'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0140._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0141._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0142._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0143._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0144._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0145._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0146._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0147._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0148._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0149._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0150._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0150._0002.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0151._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0152._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0153._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0154._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0155._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0156._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0157._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0158._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0159._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0160._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0161._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0162._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0163._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0164._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0165._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0166._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0167._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0168._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0170._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0171._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0173._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0174._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0175._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0176._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0177._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0179._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0180._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0181._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0182._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0183._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0184._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0185._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0186._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0187._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0188._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0189._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0190._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0191._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0192._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0193._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0194._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0195._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0196._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0197._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0198._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0199._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0200._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0201._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0202._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0203._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0205._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0206._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0207._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0208._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0209._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0210._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0211._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0212._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0213._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0215._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0216._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0217._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0218._0002.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0219._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0220._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0221._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0222._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0223._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0224._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0225._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0226._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0227._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0228._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0229._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0230._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0231._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0232._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0233._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0234._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0236._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0237._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0238._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0239._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0240._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0241._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0242._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0243._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0244._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0245._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0246._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0247._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0248._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0249._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0250._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0251._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0252._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0253._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0254._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0255._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0256._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0257._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0258._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0259._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0260._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0261._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0262._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0263._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0264._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0265._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0266._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0267._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0268._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0269._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0270._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0271._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0272._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0273._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0274._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0275._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0276._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0277._0001.1',
     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0278._0001.1',
     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0281._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0282._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0286._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0287._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0288._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0289._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0290._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0291._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0292._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0293._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0294._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0295._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0296._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0297._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0298._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0299._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0302._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0308._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0312._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0313._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0314._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0315._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0316._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0317._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0318._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0319._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0321._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0322._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0328._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0329._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0330._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0331._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0332._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0333._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0334._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0335._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0336._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0337._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0338._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0339._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0340._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0341._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0342._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0343._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0344._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0345._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0346._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0347._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0348._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0349._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0350._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0351._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0353._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0354._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0355._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0356._0002.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0357._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0358._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0359._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0360._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0361._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0362._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0363._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0364._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0365._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0366._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0367._0001.1',
#     'rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247/data10_7TeV.00153030.physics_MinBias.recon.ESD.f247._lb0368._0001.1',
    ]

import PyUtils.MetaReader import read_metadata

inputfile = read_metadata(collection[0])
inputfile = inputfile[collection[0]]  # promote keys stored under input filename key one level up to access them directly

if inputfile['file_type'] == 'BS':
  globalflags.InputFormat = 'bytestream'
elif inputfile['file_type'] == 'POOL':
  globalflags.InputFormat = 'pool'
else:
  raise RuntimeError, "Unable to read input file (format not supported)"


if inputfile['file_type'] == 'POOL':
  globalflags.DetDescrVersion = inputfile['GeoAtlas']
else:
  globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-02'

globalflags.print_JobProperties()


### set up conddb

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.pixel_setOn()


from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from IOVDbSvc.CondDB import conddb
conddb.addFolder("TDAQ","/TDAQ/EnabledResources/ATLAS/PIXEL/Modules")

conddb.setGlobalTag('COMCOND-000-00')



## Input files

if globalflags.InputFormat() == 'pool':
  import AthenaPoolCnvSvc.ReadAthenaPool
  ServiceMgr.EventSelector.InputCollections = collection
  include("PixelByteStreamErrorsSvc_jobOptions.py")
elif globalflags.InputFormat() == 'bytestream':
  include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
  include("InDetRecExample/InDetReadBS_jobOptions.py")
  ServiceMgr.EventSelector.Input = collection


### configure the event selector
#ServiceMgr.EventSelector.RunNumber = 152166#152166#142166 #5
#ServiceMgr.EventSelector.SkipEvents = 100000;
ServiceMgr.EventSelector.EventsPerLB = 1



MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = DEBUG
#MessageSvc.defaultLimit = 10**5
#theApp.EvtMax                   = 200#000
MessageSvc.OutputLevel      = 3 #5 #3
MessageSvc.errorLimit       = 100000
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
