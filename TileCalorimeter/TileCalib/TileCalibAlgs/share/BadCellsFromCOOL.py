#=============================================================
#===
#=== Usage:
#===  athena.py -c "Folder='OFL02';Tag='RUN2-UPD4-15',tileSqlite=True;RUN=543210" BadCellsFromCOOL.py
#===
#=== the default is reading Oracle, to read from sqlite replica, set tileSqlite=True
#=== the default folder is OFL02 (UPD1 tag), valid options for the folder variable
#=== are 'ONL01', 'OFL01' or 'OFL02'
#=== Different tags can be specified with Tag= option
#=== if no RUN is given, the current situation is dumped.
#===
#=============================================================

include( "TileCalibAlgs/jobOptions_TileInfoDump.py" )
from TileConditions.TileCoolMgr import tileCoolMgr

#========================================================
#=== configure TileCoolMgr
#===
#=== Allows the modification of some global paramters
#=== steering the COOL access for Tile.
#===
#========================================================

if not 'Folder' in dir():
    Folder = 'OFL02'

if not 'Tag' in dir():
    if Folder == 'OFL02': Tag = 'RUN2-HLT-UPD1-00'
    else: Tag = ''

if not 'tileSqlite' in dir():
    tileSqlite = False

#============================================================
#=== select Oracle or sqlite replica
#============================================================
if tileSqlite:
    print 'Setting db connenction to tileSqlite.db'
    if Folder == 'ONL01':
        tileCoolMgr.setDbConn("onlStatAdc","tileSqlite.db")
    else:
        tileCoolMgr.setDbConn("oflStatAdc","tileSqlite.db")
else:
    print 'Setting db connenction to Oracle'

#============================================================
#=== select folder to dump
#============================================================
if Folder == 'ONL01':
    tileCoolMgr.setFolder("onlStatAdc","/TILE/ONL01/STATUS/ADC")

elif Folder == 'OFL01':
    athenaCommonFlags.isOnline = True
    tileCoolMgr.setFolder("oflStatAdc","/TILE/OFL01/STATUS/ADC")

elif Folder == 'OFL02':
    tileCoolMgr.setFolder("oflStatAdc","/TILE/OFL02/STATUS/ADC")

else:
    print 'Folder %s not valid' % Folderll
    print 'Use Folder = "ONL01", "OFL01" or "OFL02"'
    print 'Use tile=tileSqlite=True for dumping tileSqlite.db content'
    print 'Use Tag = "some_tag" to dump specific tag, otherwise UPD1 tag is used'
    exit(2)

if Tag in dir() and len(Tag)>0:
    tileCoolMgr.setTag("oflStatAdc",tag)

tileInfoDump.PrintEmscale           = False
tileInfoDump.PrintEmscaleOnl        = False
tileInfoDump.PrintLaser             = False
tileInfoDump.PrintIntegrator        = False
tileInfoDump.PrintPedestals         = False
tileInfoDump.PrintTimingCorrections = False
tileInfoDump.PrintPulseShapes       = False
tileInfoDump.PrintOfcsCool          = False
tileInfoDump.PrintMuID              = False
tileInfoDump.Print1gNoise           = False

tileInfoDump.PrintBadChannels       = True
tileInfoDump.PrintBadCells          = True

print tileInfoDump
