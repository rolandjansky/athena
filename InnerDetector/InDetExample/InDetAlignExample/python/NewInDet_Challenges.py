# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os

class setupChallenge:
        def __init__ (self
                      ,thisChallenge
                      ,geometrySetting):
                self.thisChallenge = thisChallenge
                self.GlobalTag = ""
                self.DetDescrVersion = ""
                self.DetDescrVersionNoField = ""

                if self.thisChallenge == "CSC":
                        if os.environ['HOSTNAME'] == 'tst01.ific.uv.es':
                                collisionDir = "/data1/cescobar/data/csc/multimuons/"
                        else:
                                collisionDir = "castor:/castor/cern.ch/user/a/atlidali/data/csc/multimuons/"

                        self.setup(CollisionDirectory = collisionDir
                              ,CollisionFileList = "inputData_CSC_multimuons_digits.txt"
                              ,CosmicBFieldDirectory = "castor:/castor/cern.ch/user/l/lytken/cosmic_13010/digitization/TRTBarrel/misaligned/"
                              ,CosmicBFieldFileList  = "inputData_CSC_CosmicsRel13_BFOn.txt"
                              ,CosmicNoBFieldDirectory = "castor:/castor/cern.ch/user/l/lytken/cosmic_13010/digitization/NoField/TRTBarrel/misaligned/"
                              ,CosmicNoBFieldFileList  = "inputData_CSC_CosmicsRel13_BFOff.txt")

                        if geometrySetting == 'Nominal':
                            self.DetDescrVersion = "ATLAS-CSC-01-00-00"
                            self.GlobalTag = 'OFLCOND-CSC-00-00-00'
                        if geometrySetting == 'Perfect':
                            self.DetDescrVersion = "ATLAS-CSC-01-02-00"
                            self.GlobalTag = 'OFLCOND-CSC-00-01-00'
                        if geometrySetting == 'Aligned':
                            self.DetDescrVersion = "ATLAS-CSC-01-00-00"
                            self.GlobalTag = 'OFLCOND-CSC-00-01-05'

                if self.thisChallenge == 'FDR1':
                        self.setup(CollisionDirectory = "castor:/castor/cern.ch/user/h/hawkings/calibstream/fdr1/"
                              ,CollisionFileList  = "inputData_FDR1_idcalibstream.txt")

                        self.DetDescrVersion = "ATLAS-GEO-08-00-00"
                        self.DetDescrVersionNoField = "ATLAS-GEONF-08-00-00"
                        if geometrySetting == 'Nominal':
                                self.GlobalTag = 'COMCOND-REPC-003-00'
                        if geometrySetting == 'Aligned':
                                self.GlobalTag = 'COMCOND-ES1C-000-00'

                if self.thisChallenge == 'FDR2':
                        self.setup(CollisionDirectory = "castor:/castor/cern.ch/user/b/bvendapi/FDR2_Pi/BS_files/CalibStream/"
                              ,CollisionFileList = "inputData_FDR2_idcalibstream.txt"
                              ,CosmicBFieldDirectory = "castor:/castor/cern.ch/user/b/bvendapi/cosmics_fdr2/"
                              ,CosmicBFieldFileLiast = "inputData_FDR2_cosmicstream.txt")


                if self.thisChallenge == 'M8plus':
                        self.setup(CosmicBFieldDirectory = "castor:/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/"
                                   ,CosmicBFieldFileList  = "inputData_M8plus_Cosmic_91800.txt"
                                   ,CosmicNoBFieldDirectory = "castor:/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/"
                                   ,CosmicNoBFieldFileList  = "inputData_M8plus_Cosmic_NoBField.txt")

                        self.DetDescrVersion = "ATLAS-GEO-03-00-00"
                        self.DetDescrVersionNoField = "ATLAS-GEONF-04-00-00"

                if self.thisChallenge == 'Cosmic09':
                        self.setup(CosmicBFieldDirectory = ""
                              #,CosmicBFieldDirectory = "castor:/castor/cern.ch/grid/atlas/atlasdatadisk/data09_cos/ESD/"
                              ,CosmicBFieldFileList  = "inputData_Cosmic09_BFOn.txt"
                              ,CosmicNoBFieldDirectory = ""
                              #,CosmicNoBFieldDirectory = "castor:/castor/cern.ch/grid/atlas/atlasdatadisk/data09_cos/ESD/"
                              ,CosmicNoBFieldFileList  = "inputData_Cosmic09_BFOff.txt")

                        self.DetDescrVersion = "ATLAS-GEO-08-00-00"
                        self.DetDescrVersionNoField = "ATLAS-GEONF-08-00-00"

                        if geometrySetting == 'Nominal':
                                self.GlobalTag = 'COMCOND-REPC-003-00'
                        if geometrySetting == 'Aligned':
                                self.GlobalTag = 'COMCOND-ES1C-000-00'


                if self.thisChallenge == 'CosmicStream':
                        self.setup(CosmicBFieldFileList =  "inputData_CosmicStream.txt"
                              ,CosmicBFieldDirectory = "rfio:/castor/cern.ch/user/s/sfyrla/91338_PEB/")

                if self.thisChallenge == 'CosmicsRel14':
                        self.setup(CosmicBFieldDirectory = "castor:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-simcos/rel14/ATLAS-GEO-03-00-00/dig/"
                              ,CosmicBFieldFileList  = "inputData_CosmicsRel14_BFOn.txt"
                              ,CosmicNoBFieldDirectory = "castor:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-simcos/rel14/ATLAS-GEONF-04-00-00/"
                              ,CosmicNoBFieldFileList  = "inputData_CosmicsRel14_BFOff.txt")

                if self.thisChallenge == 'CalibrationStream':
                        self.setup(CollisionDirectory = ""
                              ,CollisionFileList = "inputData_CalibrationStream.txt")

                if self.thisChallenge == 'CosmicSim09':
                        self.setup(CosmicBFieldFileList = "inputData_CosmicSim2009_BOn.txt"
                              ,CosmicNoBFieldFileList = "inputData_CosmicSim2009_BOff.txt")


                if self.thisChallenge == "FDR09":
                        self.setup(CollisionDirectory = "castor:/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/FDR2_IDCalibStream/ESD/"
                              ,CollisionFileList = "inputData_FDR09.txt")

                        if geometrySetting == 'Nominal':
                                self.DetDescrVersion = "ATLAS-CSC-02-00-00"
                                self.GlobalTag = 'OFLCOND-FDR-02-08-00'
                        if geometrySetting == 'Perfect':
                                self.DetDescrVersion = "ATLAS-CSC-02-00-00"
                                self.GlobalTag = 'OFLCOND-CSC-01-00-00'

                if self.thisChallenge == "MC09":
                        self.setup(CollisionDirectory = "castor:/castor/cern.ch/grid/atlas/atlasgroupdisk/perf-idtracking/dq2/mc09_valid/ESD/e436_s561_r731/mc09_valid.107271.Multimuons_pt9.recon.ESD.e436_s561_r731_tid076491/"
                              ,CollisionFileList = "inputData_multimuons09.txt")

                        if geometrySetting == 'Nominal':
                                self.DetDescrVersion = "ATLAS-GEO-08-00-00"
                                self.GlobalTag = 'OFLCOND-SIM-00-00-07'
                        if geometrySetting == 'Perfect':
                                self.DetDescrVersion = "ATLAS-GEO-08-00-00"
                                self.GlobalTag = 'OFLCOND-CSC-01-00-00'

                if self.thisChallenge == "MinBias":
                        self.setup(CollisionDirectory = "/afs/cern.ch/user/a/atlidali/w0/data/minbias/"
                              ,CollisionFileList = "inputData_minbias.txt")

                        if geometrySetting == 'Nominal':
                                self.DetDescrVersion = "ATLAS-GEO-08-00-00"
                                self.GlobalTag = 'OFLCOND-SIM-00-00-00'
                        if geometrySetting == 'Perfect':
                                self.DetDescrVersion = "ATLAS-GEO-08-00-00"
                                self.GlobalTag = 'OFLCOND-SIM-01-00-00'
                        if geometrySetting == 'Aligned':
                                self.DetDescrVersion = "ATLAS-GEO-08-00-00"
                                self.GlobalTag = 'OFLCOND-CSC-00-01-05'

                if self.thisChallenge == "Halo":
                        # Beam halo events
                        self.setup(BeamHaloDirectory = "castor:/castor/cern.ch/user/t/tcorneli/"
                              ,BeamHaloFileList = "inputData_halo_digits.txt")

                if self.thisChallenge == "BeamGas":
                        # Beam Gas O events
                        self.setup(BeamGasDirectory ="castor:/castor/cern.ch/user/s/stradlin/BeamGas_1.1/digit/hijing.O.digit/"
                              ,BeamGasFileList  = "inputData_beamgas_digits.txt")

                if self.thisChallenge == "900GeV":
                        self.setup(CollisionDirectory = "",
                                   CollisionFileList  = "CustomRun_900GeV.txt",
                                   CosmicNoBFieldFileList = "GoodRunList_Cosmic_NoBF_DPD_atlasdatadisk.txt")

                        if 'Nominal':
                                self.DetDescrVersion = "ATLAS-GEO-08-00-02"
                                # self.GlobalTag = "COMCOND-ES1PST-001-00"
                                self.GlobalTag = "COMCOND-ES1PS-001-00"
#                       self.DetDescrVersion = "ATLAS-GEO-08-00-02"
                        self.DetDescrVersionNoField = "ATLAS-GEONF-08-00-00"

                if self.thisChallenge == "7TeV":
                        self.setup(CollisionDirectory = "",
                                   CollisionFileList  = "CustomRun_7TeV.txt",
                                   CosmicNoBFieldFileList = "CustomRun_CosmicsNoBF_7TeV.txt",
                                   CosmicBFieldFileList = "CustomRun_CosmicsBF_7TeV.txt")
                        #self.GlobalTag = "COMCOND-ES1PST-002-00"
                        self.GlobalTag = ""  #2010 data
                        #self.DetDescrVersion = "ATLAS-GEO-10-00-00"
                        self.DetDescrVersion = ""
                        self.DetDescrVersionNoField = "ATLAS-GEONF-08-00-00"


                if self.thisChallenge == "SingleBeam":
                        self.setup(CollisionDirectory = ""
                                   ,CollisionFileList  = "CustomRun_SingleBeam.txt")
                        if 'Nominal':
                                self.DetDescrVersion = "ATLAS-GEO-03-00-00"
                                self.GlobalTag = "COMCOND-ES1C-000-00"

        
        def setup(self
                  ,CollisionDirectory = ""
                  ,CollisionFileList = ""
                  ,CosmicBFieldDirectory = ""
                  ,CosmicBFieldFileList  = ""
                  ,CosmicNoBFieldDirectory = ""
                  ,CosmicNoBFieldFileList  = ""):

                self.CollisionDirectory = CollisionDirectory
                self.CollisionFileList = CollisionFileList
                if self.CollisionFileList:
                        os.system("get_files -jo %s >/dev/null" % self.CollisionFileList)

                self.CosmicBFieldDirectory = CosmicBFieldDirectory
                self.CosmicBFieldFileList = CosmicBFieldFileList
                if self.CosmicBFieldFileList:
                        os.system("get_files -jo %s >/dev/null" % self.CosmicBFieldFileList)

                self.CosmicNoBFieldDirectory = CosmicNoBFieldDirectory
                self.CosmicNoBFieldFileList = CosmicNoBFieldFileList
                if self.CosmicNoBFieldDirectory:
                        os.system("get_files -jo %s >/dev/null" % self.CosmicNoBFieldFileList)
