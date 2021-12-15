#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


import subprocess, os, shlex, re

from AthenaCommon import Logging

## Get handle to Athena logging
logger = Logging.logging.getLogger("Superchic_i")


class SuperChicConfig:

    def __init__(self, runArgs):
        self.superchicpath = os.environ['SUPERCHICPATH']
        
        #SuperChic specific variables for input.DAT, see writeInputDAT function for more elaboration
        self.rts = 13000. #collision energy (GeV)
        if hasattr(runArgs,"ecmEnergy"):
            self.rts = runArgs.ecmEnergy

        self.isurv = 4 #Model of soft survival (from 1 -> 4, corresponding to arXiv:1306.2149)
        self.intag = 'in' #tag for input files
        self.PDFname = 'MMHT2014lo68cl'
        self.PDFmember = 0
        self.proc = 5 #Please consult Superchic Manual https://superchic.hepforge.org/
        self.beam = 'prot'
        self.outtg = 'out'
        self.sfaci = True
        self.diff = 'el' #interaction: elastic ('el'), single ('sd','sda','sdb') and double ('dd') dissociation.
        self.an = 208
        self.az = 82
        self.rz = 6.68
        self.dz = 0.447
        self.rn = 6.7
        self.dn = 0.55
        self.ionqcd = 'coh'
        self.ncall = 10000
        self.itmx = 10
        self.prec = 0.5
        self.ncall1 = 10000
        self.inccall = 10000
        self.itend = 1000

        self.iseed = 34
        if hasattr(runArgs,"randomSeed"):
            self.iseed  = runArgs.randomSeed

        self.genunw = True

        self.nev = "500"
        if hasattr(runArgs,"maxEvents"):
            self.nev = runArgs.maxEvents

        self.erec = 'lhe' #output file type
        self.readwt = False
        self.wtmax = 0
        self.ymin = -2.4 # Minimum object rapidity Y_X
        self.ymax = 2.4 # Maximum object rapidity Y_X
        self.mmin = 6. # Minimum object mass M_X
        self.mmax = 500. # Maximum object mass M_X
        self.gencuts = True # Generate cuts below
        self.scorr = True
        self.fwidth = True
        self.ptxmax = 100.
        self.ptamin = 3.0 # Minimum pT of outgoing object a (gamma)
        self.ptbmin = 3.0 # Minimum pT of outgoing object b (gamma)
        self.etaamin = -2.4 # Minimum eta of outgoing object a
        self.etaamax = 2.4 # Maximum eta of outgoing object a
        self.etabmin = -2.4 # Minimum eta of outgoing object b
        self.etabmax = 2.4 # Maximum eta of outgoing object b
        self.acoabmax = 100.
        self.ptcmin = 0
        self.etacmin = -2.5
        self.etacmax = 2.5
        self.ptdmin = 0
        self.etadmin = -2.5
        self.etadmax = 2.5
        self.ptemin = 0
        self.etaemin = -2.5
        self.etaemax = 2.5
        self.ptfmin = 0
        self.etafmin = -2.5
        self.etafmax = 2.5
        self.rjet = 0.6
        self.jalg = 'antikt'
        self.m2b = 0.133
        self.pdgid1 = 211
        self.pdgid2 = -211
        self.malp = 1000
        self.gax = 0.001
        self.alpt = 'ps'
        self.mpol = 500
        self.mmon = 933
        self.gamm = 10
        self.mcharg = 100
        self.mneut = 80

    def toFortran(self):

        def fortDouble(x):
            return str(x)+"d0"
        def fortInt(x):
            return str(x)
        def fortBool(x):
            return '.true.' if x else '.false.'
        def fortStr(x):
            return "'{}'".format(x)
        
        conf = ""
        conf+="***********************************************************************************\n"
        conf+="****** Initialize afain if FIRST FIVE PARAMETERS ARE CHANGED (and beam = 'prot'):******\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.rts) + "                     ! [rts] : CMS collision energy (GeV) \n"
        conf+=fortInt(self.isurv) + "                         ! [isurv] : Model of soft survival (from 1 -> 4)\n"
        conf+=fortStr(self.intag) + "                     ! [intag] for input files  \n"
        conf+="***********************************************************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortStr(self.PDFname) + "                ! [PDFname] : PDF set \n"
        conf+=fortInt(self.PDFmember)  + "                       ! [PDFmember] : PDF member \n"
        conf+="***********************************************************************************\n"
        conf+=fortInt(self.proc) + "                             ! [proc] : Process number \n"
        conf+=fortStr(self.beam) + "                           ! [beam] : Beam type ('prot', 'ion', 'ionp' or 'el') \n"
        conf+=fortStr(self.outtg) + "                           ! [outtg] : for output file \n"
        conf+=fortBool(self.sfaci) + "                          ! [sfaci] : Include soft survival effects \n"
        conf+="***********************************************************************************\n"
        conf+="***********************************************************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortStr(self.diff) + "                          ! [diff] : dissociation flag \n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.an) + "                                  ! [an] : Ion mass number \n"
        conf+=fortDouble(self.az) + "                                   ! [az] : Ion atomic number \n"
        conf+=fortDouble(self.rz) + "                                 ! [rz] : Ion proton density - radius \n"
        conf+=fortDouble(self.dz) + "                                ! [dz] : Ion proton density - skin thickness \n"
        conf+=fortDouble(self.rn) + "                                  ! [rn] : Ion neutron density - radius \n"
        conf+=fortDouble(self.dn) + "                                  ! [dn] : Ion neutron density - skin thickness \n"
        conf+=fortStr(self.ionqcd) + "                               ! [ionqcd] : Coherent ('coh') or incoherent ('incoh') for QCD-induced processes \n"
        conf+="***********************************************************************************\n"
        conf+="*************Integration parameters************************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortInt(self.ncall) + "                              ! [ncall] : Number of calls for preconditioning \n"
        conf+=fortInt(self.itmx) + "                                  ! [itmx] : Number of iterations for preconditioning \n"
        conf+=fortDouble(self.prec) + "                                 ! [prec] :  Relative accuracy (in %) in main run \n"
        conf+=fortInt(self.ncall1) + "                               ! [ncall1] : Number of calls in first iteration \n"
        conf+=fortInt(self.inccall) + "                              ! [inccall] : Number of increase calls per iteration \n"
        conf+=fortInt(self.itend) + "                                ! [itend] : Maximum number of iterations \n"
        conf+=fortInt(self.iseed) + "                                 ! [iseed] : Random number seed (integer > 0) \n"
        conf+="***********************************************************************************\n"
        conf+="********************Unweighted events**********************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortBool(self.genunw) + "                              ! [genunw] : Generate unweighted events \n"
        conf+=fortInt(int(self.nev*1.01)) + "                                 ! [nev] : Number of events (preferably controlled by maxEvents option in Gen_tf command) \n"
        conf+=fortStr(self.erec) + "                               ! [erec] : Event record format ('hepmc','lhe','hepevt') \n"
        conf+=fortBool(self.readwt) + "                             ! [readwt] : Set to true to read in pre-calculated maxium weight below \n"
        conf+=fortDouble(self.wtmax) +  "                               ! [wtmax] : Maximum weight \n"
        conf+="***********************************************************************************\n"
        conf+="***********************************************************************************\n"
        conf+="*******************   general cuts ************************************************\n"
        conf+=fortDouble(self.ymin) + "                               ! [ymin] : Minimum object rapidity Y_X \n"
        conf+=fortDouble(self.ymax) + "                               ! [ymax] : Maximum object rapidity Y_X \n"
        conf+=fortDouble(self.mmin) + "                               ! [mmin] : Minimum object mass M_X (redundant for resonance production) \n"
        conf+=fortDouble(self.mmax) + "                               ! [mmax] : Maximum object mass M_X (redundant for resonance production) \n"
        conf+=fortBool(self.gencuts) + "                             ! [gencuts] : Generate cuts below \n"
        conf+=fortBool(self.scorr) + "                             ! [scorr] : Include spin correlations (for chi_c/b decays) \n"
        conf+=fortBool(self.fwidth) + "                             ! [fwidth] : Include finite width (for chi_c decays)  \n"
        conf+="***********************************************************************************\n"
        conf+="************ See manual for momentum assignments***********************************\n"
        conf+="***********************************************************************************\n"
        conf+="******************* Proton Cuts ***************************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.ptxmax) + "                               ! [ptxmax] : max pT of the system  \n"
        conf+="***********************************************************************************\n"
        conf+="**********2 body final states : p(a) + p(b) ***************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.ptamin) + "                               ! [ptamin] \n"
        conf+=fortDouble(self.ptbmin) + "                               ! [ptbmin] \n"
        conf+=fortDouble(self.etaamin) + "                              ! [etaamin] \n"
        conf+=fortDouble(self.etaamax) + "                              ! [etaamax] \n"
        conf+=fortDouble(self.etabmin) + "                              ! [etabmin] \n"
        conf+=fortDouble(self.etabmax) + "                              ! [etabmax] \n"
        conf+=fortDouble(self.acoabmax) + "                             ! [acoabmax] \n"
        conf+="***********************************************************************************\n"
        conf+="****** 3 body final states : p(a) + p(b) + p(c) ***********************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.ptamin) + "                                ! [ptamin] \n"
        conf+=fortDouble(self.ptbmin)  + "                               ! [ptbmin] \n"
        conf+=fortDouble(self.ptcmin) + "                                ! [ptcmin] \n"
        conf+=fortDouble(self.etaamin) + "                               ! [etaamin] \n"
        conf+=fortDouble(self.etaamax) + "                               ! [etaamax] \n"
        conf+=fortDouble(self.etabmin)  + "                              ! [etabmin] \n"
        conf+=fortDouble(self.etabmax) + "                               ! [etabmax]	 \n"
        conf+=fortDouble(self.etacmin) + "                               ! [etacmin] \n"
        conf+=fortDouble(self.etacmax) + "                               ! [etacmax]	 \n"
        conf+="***********************************************************************************\n"
        conf+="****** 4 body final states : p(a) + p(b) + p(c) + p(d) ****************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.ptamin) + "                                ! [ptamin] \n"
        conf+=fortDouble(self.ptbmin) + "                                ! [ptbmin] \n"
        conf+=fortDouble(self.ptcmin) + "                                ! [ptcmin] \n"
        conf+=fortDouble(self.ptdmin) + "                                ! [ptdmin] \n"
        conf+=fortDouble(self.etaamin) + "                               ! [etaamin] \n"
        conf+=fortDouble(self.etaamax) + "                               ! [etaamax] \n"
        conf+=fortDouble(self.etabmin) + "                               ! [etabmin] \n"
        conf+=fortDouble(self.etabmax) + "                               ! [etabmax] \n"
        conf+=fortDouble(self.etacmin) + "                               ! [etacmin] \n"
        conf+=fortDouble(self.etacmax) + "                               ! [etacmax] \n"
        conf+=fortDouble(self.etadmin) + "                               ! [etacmin] \n"
        conf+=fortDouble(self.etadmax) + "                               ! [etadmax] \n"
        conf+="***********************************************************************************\n"
        conf+="****** 6 body final states : p(a) + p(b) + p(c) + p(d) + p(e) + p(f) **************\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.ptamin)  + "                                ! [ptamin] \n"
        conf+=fortDouble(self.ptbmin)  + "                                ! [ptbmin] \n"
        conf+=fortDouble(self.ptcmin)  + "                                ! [ptcmin]  \n"
        conf+=fortDouble(self.ptdmin)  + "                                ! [ptdmin] \n"
        conf+=fortDouble(self.ptemin)  + "                                ! [ptemin] \n"
        conf+=fortDouble(self.ptfmin)  + "                                ! [ptfmin]  \n"
        conf+=fortDouble(self.etaamin)  + "                               ! [etaamin] \n"
        conf+=fortDouble(self.etaamax)  + "                               ! [etaamax] \n"
        conf+=fortDouble(self.etabmin) + "                               ! [etabmin] \n"
        conf+=fortDouble(self.etabmax) + "                               ! [etabmax]	\n"
        conf+=fortDouble(self.etacmin) + "                               ! [etacmin] \n"
        conf+=fortDouble(self.etacmax) + "                               ! [etacmax] \n"
        conf+=fortDouble(self.etadmin) + "                               ! [etadmin]  \n"
        conf+=fortDouble(self.etadmax) + "                               ! [etadmax]  \n"
        conf+=fortDouble(self.etaemin) + "                               ! [etaemin]  \n"
        conf+=fortDouble(self.etaemax) + "                               ! [etaemax] \n"
        conf+=fortDouble(self.etaemin) + "                               ! [etafmin] \n"
        conf+=fortDouble(self.etafmax) + "                               ! [etafmax]	\n"
        conf+="***********************************************************************************\n"
        conf+="******************* jet algorithm parameters **************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.rjet) + "                                  ! [rjet] : Jet Radius \n"
        conf+=fortStr(self.jalg) + "                                  ! [jalg] : Jet algorithm ('antikt','kt') \n"
        conf+="***********************************************************************************\n"
        conf+="***** chi_c/b two-body decays *****************************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.m2b) + "                                   ! [m2b] : mass of decay particles \n"
        conf+=fortInt(self.pdgid1) + "                                ! [pdgid1] : PDG number particle 1 \n"
        conf+=fortInt(self.pdgid2) + "                                ! [pdgid2] : PDG number particle 2 \n"
        conf+="***********************************************************************************\n"
        conf+="*******  ALP Parameters ***********************************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.malp) + "                                  ! [malp] : ALP mass (GeV) \n"
        conf+=fortDouble(self.gax) + "                                   ! [gax] : ALP coupling (GeV^-1) \n"
        conf+=fortStr(self.alpt) + "                                  ! [alpt] : AlP type (scalar - 'sc', pseudoscalar - 'ps') \n"
        conf+="***********************************************************************************\n"
        conf+="**** Monopole Parameters **********************************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.mpol) + "                                  ! [mpol] : Monopole mass \n"
        conf+=fortDouble(self.mmon) + "                                  ! [mmon] : Monopolium mass \n"
        conf+=fortDouble(self.gamm) + "                                  ! [gamm] : Monopolium width \n"
        conf+="***********************************************************************************\n"
        conf+="****   SUSY Parameters ************************************************************\n"
        conf+="***********************************************************************************\n"
        conf+=fortDouble(self.mcharg) + "                                ! [mcharg] : Chargino/Slepton mass \n"
        conf+=fortDouble(self.mneut)  + "                                ! [mneut]  : Neutralino mass \n"
        conf+="***********************************************************************************\n"
        conf+="***********************************************************************************\n"

        return conf 

    def outputLHEFile(self):
        return "evrecs/evrec"+self.outtg+".dat"


def writeInputDAT(Init):
    
    with open("input.DAT", "w") as outF:
        outF.write(Init.toFortran())
    
    return


def run_command(command, stdin = None):
    """
    Run a command and print output continuously
    """
    process = subprocess.Popen(shlex.split(command), stdout=subprocess.PIPE, stdin=stdin)
    while True:
        output = process.stdout.readline().decode("utf-8")
        if output == '' and process.poll() is not None:
            break
        if output:
            # remove ANSI escape formatting characters
            reaesc = re.compile(r'(\x9B|\x1B\[)[0-?]*[ -\/]*[@-~]')
            text = reaesc.sub('', output.strip())
            logger.info(text)

    rc = process.poll()
    return rc


def SuperChicInitialize(Init, stdin=None):

    logger.info("Starting SuperChic Initialization")

    if not os.path.exists('inputs'):
        os.makedirs('inputs')
    if not os.path.exists('evrecs'):
        os.makedirs('evrecs')
    if not os.path.exists('outputs'):
        os.makedirs('outputs')


    try:
        inputDAT = open('input.DAT')

    except IOError:
        raise Exception("problem with file IO; potentially input.DAT not created correctly")
    else:

        try: 
            rc = run_command(Init.superchicpath+"/bin/init", inputDAT) 

        except OSError:
            raise Exception("init executable or file not found")

        except Exception:
            raise Exception("Non-OSError or IOError in init execution block")

    if rc:
        raise Exception('Unexpected error in superchic init execution')
        
    return


def SuperChicExecute(Init):

    logger.info("Starting SuperChic Itself")

    if not os.path.exists('inputs'):
        os.makedirs('inputs')
    if not os.path.exists('evrecs'):
        os.makedirs('evrecs')
    if not os.path.exists('outputs'):
        os.makedirs('outputs')

    
    try:
        inputDAT = open('input.DAT')
    except IOError:
        raise  Exception ("problem with IO; potentially input.DAT not created correctly")
    else: 

        try: 
            rc = run_command(Init.superchicpath+'/bin/superchic', stdin=inputDAT)

        except OSError:
            raise Exception("Superchic executable or file not found")

        except Exception:
            raise Exception("Non-OSError or IOError in Superchic execution block")

    if rc:
        raise Exception('Unexpected error in superchic execution')

    return



def SuperChicRun(Init, genSeq):

    # dump the job configuration for fortran code
    print(Init.toFortran())

    # attach config to genSequence for later usin in showering
    genSeq.SuperChicConfig = Init
    
    writeInputDAT(Init)
    SuperChicInitialize(Init)
    SuperChicExecute(Init)
    
    return
