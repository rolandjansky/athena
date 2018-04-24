# minimal job with AlgSequence (topSequence) and ServiceMgr
include("AthExJobOptions/AthExJobOptions_MinimalJobOptionsNoEvt.py")

# set undefined flags to false
if not 'DoAMDBTables'    in dir(): DoAMDBTables=False
if not 'DoAGDDBlob'      in dir(): DoAGDDBlob=False
if not 'CheckAMDBTables' in dir(): CheckAMDBTables=False
if not 'CheckAGDDBlob'   in dir(): CheckAGDDBlob=False

if DoAMDBTables or DoAGDDBlob or CheckAMDBTables or CheckAGDDBlob :

   # input amdb_simrec file needs to be defined in input_amdb_simrec either to generate tables from or to check against
   if not 'input_amdb_simrec' in dir() :
      sys.exit("ERROR : Please give the path to the amdb_simrec file in the variable 'input_amdb_simrec'!")

   # AmdcsimrecAthenaSvc settings
   from AmdcAth.AmdcAthConf import AmdcsimrecAthenaSvc
   AmdcsimrecAthenaSvc.NameOfTheSource = "ASCII"
   #  input_amdb_simrec is input file path expected as argument
   AmdcsimrecAthenaSvc.NameOfTheAmdbFile = input_amdb_simrec

   ServiceMgr += AmdcsimrecAthenaSvc()

if DoAMDBTables or CheckAMDBTables :
   # AmdcOracle is only needed to generate tables (either for uploading or checking)
   include("AmdcOracle/AmdcOracle_ForGeometryTask.py")

if CheckAMDBTables or CheckAGDDBlob :
   # muon layout to get from the DB to check against needs to be defined in database_layout
   if not 'database_layout' in dir() :
      sys.exit("ERROR : Please give name of layout (e.g. MuonSpectrometer-R.08.01) in variable 'database_layout'!")

# to create PreSql files for AMDB tables
if DoAMDBTables :
   # AmdcOracle settings for geometry tasks
   include("AmdcDb/AmdcDb_TableProduction.py")

# to create PreSql files for AGDD blob
if DoAGDDBlob :
   # GeoModel is needed to make a test build of the volumes - in case of bugs it crashes
   database_layout=""
   include("MuonGeoModel/MuonGeoModel_MinimalSetup.py")
   # AmdcOracle settings for geometry tasks
   include("MuonAGDD/MuonAGDD_BlobProduction.py")

# to create PreSql files for AMDB tables
if CheckAMDBTables :
   # AmdcOracle settings for geometry tasks
   include("AmdcDb/AmdcDb_TableCheck.py")

if CheckAGDDBlob :
   # AmdcOracle settings for geometry tasks
   # to get the AGDD blob from a dedicated layout (needs database_layout to be defined)
   include("MuonGeoModel/MuonGeoModel_MinimalSetup.py")
   include("MuonAGDD/MuonAGDD_BlobCheck.py")

