evgenConfig.description = "Superchic gamma + gamma UPC collisions at 5020 GeV"
evgenConfig.keywords = ["2photon","2lepton"]
#evgenConfig.weighting = 0
evgenConfig.contact = ["prabhakar.palni@cern.ch"]

if not os.path.exists('inputs'):
    os.makedirs('inputs')
if not os.path.exists('evrecs'):
    os.makedirs('evrecs')

from Superchic_i.Superchic_iConf import Superchic_i
genSeq += Superchic_i("Superchic")
genSeq.Superchic.McEventKey = "GEN_EVENT"
evgenConfig.generators += ["Superchic"]

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
_evgenstream = AthenaPoolOutputStream("StreamEVGEN")
_evgenstream.ItemList = ["2101#*","133273#GEN_EVENT"]

del _evgenstream


# TODO: Sort out proper param setting based on runArgs.ecmEnergy
if int(runArgs.ecmEnergy) != 5020:
    evgenLog.error(" Set beam energy in JO initialization with parameter rts ")
    sys.exit(1)

    
genSeq.Superchic.Initialize = \
    ["rts 5.02d3",                     # set the COM collision energy (in fortran syntax)
    "isurv 4",                        # Model of soft survival
    "intag 'in5'",                    # for input files
    "PDFname 'MMHT2014lo68cl'",       # PDF set name
    "PDFmember 0",                    # PDF member
    "proc 59",                        # Process number (59 = gg->gg, 56 =gg->ee )
    "beam 'ion'",                     # Beam type ('prot', 'ion')
    "outtg 'out'",                    # for output file name
    "sfaci .false.",                  # Include soft survival effects
    "ncall 100000",                   # Number of calls for preconditioning
    "itmx 1",                        # Number of iterations for preconditioning
    "prec 1.0d0"                    # precision
    ]
