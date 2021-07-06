# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import subprocess, os


class SuperChicConfig:
    superchicpath = os.environ['SUPERCHICPATH']
    
    #SuperChic specific variables for input.DAT, see writeInputDAT function for more elaboration
    rts = "13d3" #collision energy (GeV)
    isurv = "4" #Model of soft survival (from 1 -> 4, corresponding to arXiv:1306.2149)
    intag = "'in13'" #tag for input files
    PDFname = "'MMHT2014lo68cl'"
    PDFmember = "0"
    proc = "57" #Please consult Superchic Manual https://superchic.hepforge.org/
    beam = "'prot'"
    outtg = "'out'"
    sfaci = ".true."
    diff = "'el'" #interaction: elastic ('el'), single ('sd','sda','sdb') and double ('dd') dissociation.
    an = "208d0"
    az = "82d0"
    rz = "6.68d0"
    dz = "0.447d0"
    rn = "6.7d0"
    dn = "0.55d0"
    ionqcd = "'coh'"
    ncall = "10000"
    itmx = "10"
    prec = "0.5d0"
    ncall1 = "10000"
    inccall = "10000"
    itend = "1000"
    iseed = "34"
    genunw = ".true"
    nev = "500"
    erec = "'lhe'" #output file type
    readwt = ".false."
    wtmax = "0d0"
    ymin = "-2.4d0" # Minimum object rapidity Y_X
    ymax = "2.4d0" # Maximum object rapidity Y_X
    mmin = "6d0" # Minimum object mass M_X
    mmax = "500d0" # Maximum object mass M_X
    gencuts = ".true." # Generate cuts below
    scorr = ".true."
    fwidth = ".true."
    ptxmax = "100d0"
    ptamin = "3.0d0" # Minimum pT of outgoing object a (gamma)
    ptbmin = "3.0d0" # Minimum pT of outgoing object b (gamma)
    etaamin = "-2.4d0" # Minimum eta of outgoing object a
    etaamax = "2.4d0" # Maximum eta of outgoing object a
    etabmin = "-2.4d0" # Minimum eta of outgoing object b
    etabmax = "2.4d0" # Maximum eta of outgoing object b
    acoabmax = "100d0"
    ptcmin = "0d0"
    etacmin = "-2.5d0"
    etacmax = "2.5d0"
    ptdmin = "0d0"
    etadmin = "-2.5d0"
    etadmax = "2.5d0"
    ptemin = "0d0"
    etaemin = "-2.5d0"
    etaemax = "2.5d0"
    ptfmin = "0d0"
    etafmin = "-2.5d0"
    etafmax = "2.5d0"
    rjet = "0.6d0"
    jalg = "'antikt'"
    m2b = "0.133d0"
    pdgid1 = "211"
    pdgid2 = "-211"
    malp = "1000d0"
    gax = "0.001d0"
    alpt = "'ps'"
    mpol = "500d0"
    mmon = "933d0"
    gamm = "10d0"
    mcharg = "100d0"
    mneut = "80d0"
    

def writeInputDAT(Init):
    
    outF = open("input.DAT", "w")
    outF.write("***********************************************************************************\n")
    outF.write("****** Initialize again if FIRST FIVE PARAMETERS ARE CHANGED (and beam = 'prot'):******\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.rts + "                     ! [rts] : CMS collision energy (GeV) \n")
    outF.write(Init.isurv + "                         ! [isurv] : Model of soft survival (from 1 -> 4)\n")
    outF.write(Init.intag + "                     ! [intag] for input files  \n")
    outF.write("***********************************************************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.PDFname + "                ! [PDFname] : PDF set \n")
    outF.write(Init.PDFmember  + "                       ! [PDFmember] : PDF member \n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.proc + "                             ! [proc] : Process number \n")
    outF.write(Init.beam + "                           ! [beam] : Beam type ('prot', 'ion', 'ionp' or 'el') \n")
    outF.write(Init.outtg + "                           ! [outtg] : for output file \n")
    outF.write(Init.sfaci + "                          ! [sfaci] : Include soft survival effects \n")
    outF.write("***********************************************************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.diff + "                          ! [diff] : dissociation flag \n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.an + "                                  ! [an] : Ion mass number \n")
    outF.write(Init.az + "                                   ! [az] : Ion atomic number \n")
    outF.write(Init.rz + "                                 ! [rz] : Ion proton density - radius \n")
    outF.write(Init.dz + "                                ! [dz] : Ion proton density - skin thickness \n")
    outF.write(Init.rn + "                                  ! [rn] : Ion neutron density - radius \n")
    outF.write(Init.dn + "                                  ! [dn] : Ion neutron density - skin thickness \n")
    outF.write(Init.ionqcd + "                               ! [ionqcd] : Coherent ('coh') or incoherent ('incoh') for QCD-induced processes \n")
    outF.write("***********************************************************************************\n")
    outF.write("*************Integration parameters************************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.ncall + "                              ! [ncall] : Number of calls for preconditioning \n")
    outF.write(Init.itmx + "                                  ! [itmx] : Number of iterations for preconditioning \n")
    outF.write(Init.prec + "                                 ! [prec] :  Relative accuracy (in %) in main run \n")
    outF.write(Init.ncall1 + "                               ! [ncall1] : Number of calls in first iteration \n")
    outF.write(Init.inccall + "                              ! [inccall] : Number of increase calls per iteration \n")
    outF.write(Init.itend + "                                ! [itend] : Maximum number of iterations \n")
    outF.write(Init.iseed + "                                 ! [iseed] : Random number seed (integer > 0) \n")
    outF.write("***********************************************************************************\n")
    outF.write("********************Unweighted events**********************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.genunw + "                              ! [genunw] : Generate unweighted events \n")
    outF.write(Init.nev + "                                 ! [nev] : Number of events (1 or a small value is recommended) ( the number of produced total events are controlled by athena ) \n")
    outF.write(Init.erec + "                               ! [erec] : Event record format ('hepmc','lhe','hepevt') \n")
    outF.write(Init.readwt + "                             ! [readwt] : Set to true to read in pre-calculated maxium weight below \n")
    outF.write(Init.wtmax +  "                               ! [wtmax] : Maximum weight \n")
    outF.write("***********************************************************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write("*******************   general cuts ************************************************\n")
    outF.write(Init.ymin + "                               ! [ymin] : Minimum object rapidity Y_X \n")
    outF.write(Init.ymax + "                               ! [ymax] : Maximum object rapidity Y_X \n")
    outF.write(Init.mmin + "                               ! [mmin] : Minimum object mass M_X (redundant for resonance production) \n")
    outF.write(Init.mmax + "                               ! [mmax] : Maximum object mass M_X (redundant for resonance production) \n")
    outF.write(Init.gencuts + "                             ! [gencuts] : Generate cuts below \n")
    outF.write(Init.scorr + "                             ! [scorr] : Include spin correlations (for chi_c/b decays) \n")
    outF.write(Init.fwidth + "                             ! [fwidth] : Include finite width (for chi_c decays)  \n")
    outF.write("***********************************************************************************\n")
    outF.write("************ See manual for momentum assignments***********************************\n")
    outF.write("***********************************************************************************\n")
    outF.write("******************* Proton Cuts ***************************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.ptxmax + "                               ! [ptxmax] : max pT of the system  \n")
    outF.write("***********************************************************************************\n")
    outF.write("**********2 body final states : p(a) + p(b) ***************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init. ptamin + "                               ! [ptamin] \n")
    outF.write(Init. ptbmin + "                               ! [ptbmin] \n")
    outF.write(Init. etaamin + "                              ! [etaamin] \n")
    outF.write(Init. etaamax + "                              ! [etaamax] \n")
    outF.write(Init. etabmin + "                              ! [etabmin] \n")
    outF.write(Init. etabmax + "                              ! [etabmax] \n")
    outF.write(Init. acoabmax + "                             ! [acoabmax] \n")
    outF.write("***********************************************************************************\n")
    outF.write("****** 3 body final states : p(a) + p(b) + p(c) ***********************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.ptamin + "                                ! [ptamin] \n")
    outF.write(Init.ptbmin  + "                               ! [ptbmin] \n")
    outF.write(Init.ptcmin + "                                ! [ptcmin] \n")
    outF.write(Init.etaamin + "                               ! [etaamin] \n")
    outF.write(Init.etaamax + "                               ! [etaamax] \n")
    outF.write(Init.etabmin  + "                              ! [etabmin] \n")
    outF.write(Init.etabmax + "                               ! [etabmax]	 \n")
    outF.write(Init.etacmin + "                               ! [etacmin] \n")
    outF.write(Init.etacmax + "                               ! [etacmax]	 \n")
    outF.write("***********************************************************************************\n")
    outF.write("****** 4 body final states : p(a) + p(b) + p(c) + p(d) ****************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.ptamin + "                                ! [ptamin] \n")
    outF.write(Init.ptbmin + "                                ! [ptbmin] \n")
    outF.write(Init.ptcmin + "                                ! [ptcmin] \n")
    outF.write(Init.ptdmin + "                                ! [ptdmin] \n")
    outF.write(Init.etaamin + "                               ! [etaamin] \n")
    outF.write(Init.etaamax + "                               ! [etaamax] \n")
    outF.write(Init.etabmin + "                               ! [etabmin] \n")
    outF.write(Init.etabmax + "                               ! [etabmax] \n")
    outF.write(Init.etacmin + "                               ! [etacmin] \n")
    outF.write(Init.etacmax + "                               ! [etacmax] \n")
    outF.write(Init.etadmin + "                               ! [etacmin] \n")
    outF.write(Init.etadmax + "                               ! [etadmax] \n")
    outF.write("***********************************************************************************\n")
    outF.write("****** 6 body final states : p(a) + p(b) + p(c) + p(d) + p(e) + p(f) **************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.ptamin + "                                ! [ptamin] \n")
    outF.write(Init.ptbmin + "                                ! [ptbmin] \n")
    outF.write(Init.ptcmin + "                                ! [ptcmin]  \n")
    outF.write(Init.ptdmin + "                                ! [ptdmin] \n")
    outF.write(Init.ptemin + "                                ! [ptemin] \n")
    outF.write(Init.ptfmin + "                                ! [ptfmin]  \n")
    outF.write(Init.etaamin + "                               ! [etaamin] \n")
    outF.write(Init.etaamax + "                               ! [etaamax] \n")
    outF.write(Init.etabmin + "                               ! [etabmin] \n")
    outF.write(Init.etabmax + "                               ! [etabmax]	\n")
    outF.write(Init.etacmin + "                               ! [etacmin] \n")
    outF.write(Init.etacmax + "                               ! [etacmax] \n")
    outF.write(Init.etadmin + "                               ! [etadmin]  \n")
    outF.write(Init.etadmax + "                               ! [etadmax]  \n")
    outF.write(Init.etaemin + "                               ! [etaemin]  \n")
    outF.write(Init.etaemax + "                               ! [etaemax] \n")
    outF.write(Init.etaemin + "                               ! [etafmin] \n")
    outF.write(Init.etafmax + "                               ! [etafmax]	\n")
    outF.write("***********************************************************************************\n")
    outF.write("******************* jet algorithm parameters **************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.rjet + "                                  ! [rjet] : Jet Radius \n")
    outF.write(Init.jalg + "                                  ! [jalg] : Jet algorithm ('antikt','kt') \n")
    outF.write("***********************************************************************************\n")
    outF.write("***** chi_c/b two-body decays *****************************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.m2b + "                                   ! [m2b] : mass of decay particles \n")
    outF.write(Init.pdgid1 + "                                ! [pdgid1] : PDG number particle 1 \n")
    outF.write(Init.pdgid2 + "                                ! [pdgid2] : PDG number particle 2 \n")
    outF.write("***********************************************************************************\n")
    outF.write("*******  ALP Parameters ***********************************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.malp + "                                  ! [malp] : ALP mass (GeV) \n")
    outF.write(Init.gax + "                                   ! [gax] : ALP coupling (GeV^-1) \n")
    outF.write(Init.alpt + "                                  ! [alpt] : AlP type (scalar - 'sc', pseudoscalar - 'ps') \n")
    outF.write("***********************************************************************************\n")
    outF.write("**** Monopole Parameters **********************************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.mpol + "                                  ! [mpol] : Monopole mass \n")
    outF.write(Init.mmon + "                                  ! [mmon] : Monopolium mass \n")
    outF.write(Init.gamm + "                                  ! [gamm] : Monopolium width \n")
    outF.write("***********************************************************************************\n")
    outF.write("****   SUSY Parameters ************************************************************\n")
    outF.write("***********************************************************************************\n")
    outF.write(Init.mcharg + "                                ! [mcharg] : Chargino/Slepton mass \n")
    outF.write(Init.mneut  + "                                ! [mneut]  : Neutralino mass \n")
    outF.write("***********************************************************************************\n")
    outF.write("***********************************************************************************\n")

    outF.close()
    
    return


def SuperChicInitialize(Init):

    print("Starting SuperChic Initialization")
    
    try:
        outputLog = open("init.log", 'w')
        inputDAT = open('input.DAT')
        
        init = subprocess.Popen([Init.superchicpath+"/bin/init"], stdin=inputDAT, stdout=outputLog, stderr=outputLog)

        (output, err) = init.communicate()
        init.wait()

        print(output)
        print(err)

    except OSError:
        print("init executable or file not found")
        raise

    except IOError:
        print("problem with file IO; potentially input.DAT not created correctly")
        raise

#    except:
#        print("Non-OSError or IOError in init execution block")
#        raise
        
    #try... except does not catch fortran errors, instead check that the files made by init are filled
    try:
        fileName = "inputs/sdcoh"+Init.intag[1:-1]+".dat"
        print(fileName)
        if( os.path.isfile(fileName) ):
            print("input file made by init")
            InFile = open(fileName, "r")
            line_count = 0
            for line in InFile:
                if line != "\n":
                    line_count += 1
            InFile.close()
            if(line_count < 10):
                raise Exception('Input not filled properly')
        else:
            raise Exception('no Input file produced')

    except Exception as inst:
        print(inst.args)
        raise

    outputLog.close()
    inputDAT.close()

    return


def SuperChicExecute(Init):

    print("Starting SuperChic Itself")

    try:
        outputLog = open("superchic.log", 'w')
        inputDAT = open('input.DAT')
        
        superchic = subprocess.Popen([Init.superchicpath+"/bin/superchic"], stdin=inputDAT, stdout=outputLog, stderr=outputLog)

        (output, err) = superchic.communicate()
        superchic.wait()

        print(output)
        print(err)

    except OSError:
        print("Superchic executable or file not found")
        raise

    except IOError:
        print("problem with IO; potentially input.DAT not created correctly")
        raise

#    except:
#        print("Non-OSError or IOError in Superchic execution block")
#        raise

    #try... except does not catch fortran errors, instead check if the LHE file is filled
    try:
        fileName = "evrecs/evrec"+Init.outtg[1:-1]+".dat"
        print(fileName)
        if( os.path.isfile(fileName) ):
            print("LHE file made by Superchic")
            LHEFile = open(fileName, "r")
            line_count = 0
            for line in LHEFile:
                if line != "\n":
                    line_count += 1
            LHEFile.close()
            if(line_count < 10):
                raise Exception('LHE not filled properly')
        else:
            raise Exception('no LHE file produced')

    except Exception as inst:
        print(inst.args)
        raise

    outputLog.close()
    inputDAT.close()

    return



def SuperChicRun(Init):
    
    writeInputDAT(Init)
    SuperChicInitialize(Init)
    SuperChicExecute(Init)
    
    return
