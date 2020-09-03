# minimal job with AlgSequence (topSequence) and ServiceMgr
include("AthExJobOptions/AthExJobOptions_MinimalJobOptionsNoEvt.py")

# set undefined flags to false
if not 'DoAMDBTables'    in dir(): DoAMDBTables=False
if not 'DoAGDDBlob'      in dir(): DoAGDDBlob=False
if not 'DoNSWDBlob'      in dir(): DoNSWDBlob=False
if not 'CheckAMDBTables' in dir(): CheckAMDBTables=False
if not 'CheckAGDDBlob'   in dir(): CheckAGDDBlob=False
if not 'CheckNSWDBlob'   in dir(): CheckNSWDBlob=False

if DoAMDBTables or CheckAMDBTables:

   # input amdb_simrec file needs to be defined in input_amdb_simrec either to generate tables from or to check against
   if not 'input_amdb_simrec' in dir() :
      sys.exit("ERROR : Please give the path to the amdb_simrec file in the variable 'input_amdb_simrec'!")
   if not os.path.isfile(input_amdb_simrec) :
      sys.exit("ERROR : File " + input_amdb_simrec + " either doesn't exist or isn't a file.")

   # AmdcsimrecAthenaSvc settings
   from AmdcAth.AmdcAthConf import AmdcsimrecAthenaSvc
   AmdcsimrecAthenaSvc.NameOfTheSource = "ASCII"
   # input_amdb_simrec is input file path expected as argument
   AmdcsimrecAthenaSvc.NameOfTheAmdbFile = input_amdb_simrec

   ServiceMgr += AmdcsimrecAthenaSvc()

if DoAGDDBlob or CheckAGDDBlob:

   if not 'input_agdd_xml' in dir() and not 'input_amdb_simrec' in dir():
      sys.exit("ERROR : Please give the path to either the amdb_simrec or the input_agdd_xml file!")
   elif not 'input_amdb_simrec' in dir() and not os.path.isfile(input_agdd_xml):
      sys.exit("ERROR : File " + input_agdd_xml + " either doesn't exist or isn't a file.")
   elif not 'input_agdd_xml' in dir() and not os.path.isfile(input_amdb_simrec):
      sys.exit("ERROR : File " + input_amdb_simrec + " either doesn't exist or isn't a file.")

   if 'input_amdb_simrec' in dir() and not 'input_agdd_xml' in dir():
      # need to setup AmdcsimrecAthenaSvc to read amdb file, i.e. AGDD xml part when not giving input_agdd_xml
      # AmdcsimrecAthenaSvc settings
      from AmdcAth.AmdcAthConf import AmdcsimrecAthenaSvc
      AmdcsimrecAthenaSvc.NameOfTheSource = "ASCII"
      # input_amdb_simrec is input file path expected as argument
      AmdcsimrecAthenaSvc.NameOfTheAmdbFile = input_amdb_simrec
      ServiceMgr += AmdcsimrecAthenaSvc()

if DoNSWDBlob or CheckNSWDBlob :
   # input xml file with NSW geometry needs to be definded as input_nsw_xml either to generate tables from or to check against
   if not 'input_nsw_xml' in dir():
      sys.exit("ERROR : Please give the path to the xml file for NSW geometry in the variable 'input_nsw_xml'!")
   if not os.path.isfile(input_nsw_xml) :
      sys.exit("ERROR : File " + input_nsw_xml + " either doesn't exist or isn't a file.")

if DoAMDBTables or CheckAMDBTables :
   # AmdcOracle is only needed to generate tables (either for uploading or checking)
   include("AmdcOracle/AmdcOracle_ForGeometryTask.py")

if CheckAMDBTables or CheckAGDDBlob or CheckNSWDBlob:
   # muon layout to get from the DB to check against needs to be defined in database_layout
   if not 'database_layout' in dir() :
      sys.exit("ERROR : Please give name of layout (e.g. MuonSpectrometer-R.08.01) in variable 'database_layout'!")

# to create PreSql files for AMDB tables
if DoAMDBTables :
   # AmdcDb table production settings
   include("AmdcDb/AmdcDb_TableProduction.py")

# to create PreSql files for AGDD blob
if DoAGDDBlob :
   # GeoModel is needed to make a test build of the volumes - in case of bugs it crashes
   include("MuonGeoModel/MuonGeoModel_MinimalSetup.py")
   # include writing settings for MuonAGDD
   include("MuonAGDD/MuonAGDD_BlobProduction.py")

# to create PreSql files for NSW blob
if DoNSWDBlob :
   # GeoModel is needed to make a test build of the volumes - in case of bugs it crashes
   include("MuonGeoModel/MuonGeoModel_MinimalSetup.py")
   # after the above include, GeoModelSvc should be setup, now use a recent layout to start from
   GeoModelSvc.AtlasVersion='ATLAS-R3S-2021-01-00-00'
   # since we have chosen a symmetric NSW layout and we execute this jobOptions file outside of a 
   # standard job transform, set the MuonDetectorTool and DetDescrCnvSvc properties by hand
   GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].HasCSC = False
   GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].HasSTgc = True
   GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].HasMM = True
   if hasattr(svcMgr,'DetDescrCnvSvc'):
       svcMgr.DetDescrCnvSvc.HasCSC=False
       svcMgr.DetDescrCnvSvc.HasSTgc=True
       svcMgr.DetDescrCnvSvc.HasMM=True

   # include writing settings for MuonAGDD
   include("MuonAGDD/NSWAGDD_BlobProduction.py")

# to check PreSql files for AMDB tables wrt uploaded files
if CheckAMDBTables :
   # AmdcDb settings for geometry checking
   include("AmdcDb/AmdcDb_TableCheck.py")

# to check AGDD blob wrt uploaded file
if CheckAGDDBlob :
   # to get the AGDD blob from a dedicated layout (needs database_layout to be defined)
   include("MuonGeoModel/MuonGeoModel_MinimalSetup.py")
   # include checking settings for MuonAGDD
   include("MuonAGDD/MuonAGDD_BlobCheck.py")

# to check AGDD blob wrt uploaded file
if CheckNSWDBlob :
   # to get the AGDD blob from a dedicated layout (needs database_layout to be defined)
   include("MuonGeoModel/MuonGeoModel_MinimalSetup.py")
   # after the above include, GeoModelSvc should be setup, now use a recent layout to start from
   GeoModelSvc.AtlasVersion='ATLAS-R3S-2021-01-00-00'
   # since we have chosen a symmetric NSW layout and we execute this jobOptions file outside of a 
   # standard job transform, set the MuonDetectorTool and DetDescrCnvSvc properties by hand
   GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].HasCSC = False
   GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].HasSTgc = True
   GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].HasMM = True
   if hasattr(svcMgr,'DetDescrCnvSvc'):
       svcMgr.DetDescrCnvSvc.HasCSC=False
       svcMgr.DetDescrCnvSvc.HasSTgc=True
       svcMgr.DetDescrCnvSvc.HasMM=True

   # include checking settings for NSWAGDD
   include("MuonAGDD/NSWAGDD_BlobCheck.py")
