evgenConfig.description = "SuperChic4 MC gamma + gamma pp collisions at 13000 GeV to 2 gamma ALP mediated"
evgenConfig.keywords = ["2photon","2photon"]
#evgenConfig.weighting = 0
evgenConfig.contact = ["gen.tateno@cern.ch"]

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
if int(runArgs.ecmEnergy) != 13000:
    evgenLog.error(" Set beam energy in JO initialization with parameter rts ")
    sys.exit(1)


genSeq.Superchic.Initialize = [
    "rts 13000d0",              # set the COM collision energy (in fortran syntax)
    "isurv 4",                  # Model of soft survival
    "intag 'in5'",              # for input files
    "PDFname 'MMHT2014lo68cl'", # PDF set name
    "PDFmember 0",              # PDF member
    "proc 68",                  # Process number (59 = gg->gg, 56 = gg->ee, 68 = gg->a->gg )
    "beam 'prot'",              # Beam type ('prot', 'ion')
    "outtg 'out'",              # for output file name
    "sfaci .true.",             # Include soft survival effects
    "diff 'el'",                # interaction: pure elastic (el), single prodton dissociation (ds), double dissociation (dd)
    "an 208d0",
    "az 82d0",
    "rz 6.68d0",
    "dz 0.447d0",
    "rn 6.7d0",
    "dn 0.55d0",
    "ionqcd 'coh'",
    "ncall 10000",              # Number of calls for preconditioning
    "itmx 10",                  # Number of iterations for preconditioning
    "prec 0.5d0",               # precision
    "ncall1 10000",
    "inccall 10000",
    "itend 1000",
    "iseed 34",
    "genunw .true.",
    "nev 1",                    # 1 or a few
    "erec 'hepevt'",
    "readwt .false.",
    "wtmax 0d0",
    "ymin -2.4d0",              # Minimum object rapidity Y_X
    "ymax 2.4d0",               # Maximum object rapidity Y_X
    "mmin 6d0",                 # Minimum object mass M_X
    "mmax 500d0",               # Maximum object mass M_X
    "gencuts .true.",           # Generate cuts below
    "scorr .true.",
    "fwidth .true.",
    "ptxmax 100d0",
    "ptamin 3.0d0",             # Minimum pT of outgoing object a (gamma)
    "ptbmin 3.0d0",             # Minimum pT of outgoing object b (gamma)
    "etaamin -2.4d0",           # Minimum eta of outgoing object a
    "etaamax  2.4d0",           # Maximum eta of outgoing object a
    "etabmin -2.4d0",           # Minimum eta of outgoing object b
    "etabmax  2.4d0",           # Maximum eta of outgoing object b
    "acoabmax 100d0",
    "rjet 0.6d0",
    "jalg 'antikt'",
    "malp 1000d0",              # ALP mass (GeV)
    "gax 0.001d0",              # ALP coupling (GeV^-1)
    "alpt 'ps'"                 # ALP type (scalar - 'sc', pseudoscalar - 'ps')
    ]
