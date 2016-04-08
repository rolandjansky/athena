# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Vbfnlo subprocesses
#
#   Authors: Florian Kiss <florian.kiss@cern.ch>
#            
#            
#
#########################################################################################################################

#! /usr/bin/env python
import os,sys,time,subprocess,shutil
from AthenaCommon import Logging
log = Logging.logging.getLogger('VbfnloUtils')


class VbfnloConfig():
  error = 0

  outputEventsName = 'VbfnloOutput._1.events'
  VbfnloPath = os.environ['VBFNLOPATH']
  executablePath = VbfnloPath
  executablePath += '/bin/vbfnlo'

  if VbfnloPath=='':
    log.error('VBFNLO Setup Failed! Please set VbfnloPath correctly!')
    error = 1

  TestArea = os.environ['TestArea']

  
  nEvents = 1000
  energy = 8000
  muF = "125d0"
  muR = "125d0"
  pdf = 0
  randomSeed = 42
  runNumber = 777777

  process = 100
  lo_iterations = 4
  lo_points = 21
  hmass = "125d0"

  #spin2 parameters
  spin2 = "false"
  f1 = "1d0"
  f2 = "1d0"
  f3 = "0d0"
  f4 = "0d0"
  f5 = "1d0"
  lambd = "1.5d3"
  nff = "0d0"
  lambdaff = "4d2"

  #spin0 anomalous couplings
  anom_cpl = "false"
  formfactor_spin0 = ".false."
  mass_scale = "200d0"
  ffac_param = "2"
  parametr1 = ".true."
  lambda5 = "480d0"
  g5e_hww = "1d0"
  g5e_hzz = "1d0"
  g5e_hgg = "1d0"
  g5e_hgz = "1d0"
  g5o_hww = "0d0"
  g5o_hzz = "0d0"
  g5o_hgg = "0d0"
  g5o_hgz = "0d0"
  #parametr2 = ".false."
  #parametr3 = ".false."
  treefacw = "0d0"
  treefacz = "0d0"
  loopfac = "0d0"

  
  def __init__(self,runArgs=None):

    self.runArgs = runArgs
    if runArgs == None :
      log.warning('No run arguments found! Using defaults.')
      #print 'No run arguments found! Using defaults.'
    else :
      if hasattr(self.runArgs,'ecmEnergy'):
        self.energy = self.runArgs.ecmEnergy
        
      if hasattr(self.runArgs,'maxEvents'):
        if self.runArgs.maxEvents > 0 :
          self.nEvents = self.runArgs.maxEvents
      
      if hasattr(self.runArgs,'randomSeed'):
        self.randomSeed = self.runArgs.randomSeed
        
      if not hasattr(runArgs,'inputGeneratorFile'):
        runArgs.inputGeneratorFile = self.outputEventsName+'.tar.gz'

      if hasattr(runArgs,'runNumber'):
        self.runNumber = runArgs.runNumber 


  def launchOnTheFly(self):
    self.selectProcess()
    self.generateRunCard()
    self.generateEvents()
    return 0
  

  def selectProcess(self):
    if self.runNumber == 169883:
      self.process = 101
      
    elif self.runNumber == 169884:
      self.process = 101
      self.anom_cpl = "true"
      self.generateAnomCouplDat()

    elif self.runNumber == 169885:
      self.process = 101
      self.anom_cpl = "true"
      self.g5e_hww = "0d0"
      self.g5e_hzz = "0d0"
      self.g5e_hgg = "0d0"
      self.g5e_hgz = "0d0"
      self.g5o_hww = "1d0"
      self.g5o_hzz = "1d0"
      self.g5o_hgg = "1d0"
      self.g5o_hgz = "1d0"
      self.generateAnomCouplDat()

    elif self.runNumber == 169886:
      self.process = 191
      self.spin2 = "true"
      self.generateSpin2Dat()
      self.lo_points = 23
      
    elif self.runNumber == 777777:
      log.info('No default settings for couplings and processes used. The user is responsible for a correct setup!')

    else:
      log.error('RunNumber is not yet connected to any VBFNLO process. Please retry with different a RunNumber.')
      self.error = 1
    return 0
  
  
  def generateRunCard(self):
    log.info("Writing vbfnlo.dat  to "+str(self.TestArea)+"/vbfnlo.dat")
    #print "Writing vbfnlo.dat  to "+str(self.TestArea)+"/vbfnlo.dat" 
    
    with open( str(self.TestArea)+'/vbfnlo.dat','w') as f:
    #with open( 'vbfnlo.dat','w') as f:
      f.write( "!   General parameters of the calculation !-------------------------------------------\n")
      f.write( "PROCESS              = "+ str(self.process) +"                  ! Identifier for process\n")
      f.write( "LOPROCESS_PLUS_JET   = false                ! switch: LO process with 1 additional jet\n")
      f.write( "LEPTONS              = 11 -11 13 -13 15 -15 ! final state leptons\n")
      f.write( "LO_ITERATIONS        = "+ str(self.lo_iterations) +"         ! number of iterations for LO calculation\n")
      f.write( "NLO_ITERATIONS       = 4         ! number of iterations for LO real-emissions calc.\n")
      f.write( "LO_POINTS            = "+ str(self.lo_points) +"        ! number of points for LO calculation (= 2^..)\n")
      f.write( "NLO_POINTS           = 25        ! number of points for real-emissions calc. \n")
      f.write( 'LO_GRID              = "grid2_1" "grid2_2" "grid2_3" "grid2_4" ! names of gridfiles for LO calculation\n')		     
      f.write( 'NLO_GRID             = "grid3_1" "grid3_2" "grid3_3" "grid3_4" ! names of gridfiles for real emissions calculation\n')   
      f.write( 'PHTN_GRID            = "grid4_1" "grid4_2" "grid4_3" "grid4_4" ! names of gridfiles for photon emission calculation\n')  
      f.write( 'FLOOP_GRID           = "grid5_1" "grid5_2" "grid5_3" "grid5_4" ! names of gridfiles for fermion loop calculation\n')     
      f.write( "NLO_SWITCH           = false	 ! switch: nlo/lo calculation\n")							     
      f.write( "EWCOR_SWITCH         = false     ! Whether electroweak corrections are included\n")					     
      f.write( "FERMIONLOOP          = 3         ! Contribution of gluon-induced fermionic loops for diboson processes\n\n")               

      f.write( "ECM	             = "+ str(self.energy) +"	 ! collider center-of-mass energy\n")
      f.write( "BEAM1                = 1         ! type of beam 1 (1=proton, -1 = antiproton)\n")
      f.write( "BEAM2  	             = 1         ! type of beam 2 (1=proton, -1 = antiproton)\n\n")
      
      f.write( "ID_MUF	             = 0         ! ID for factorization scale\n")
      f.write( "ID_MUR	             = 0         ! ID for renormalization scale\n")
      f.write( "MUF_USER             = "+ str(self.muF) + "     ! user defined factorization scale, if MUF is set to 0\n")
      f.write( "MUR_USER             = "+ str(self.muR) + "     ! user defined renormalization scale, if MUR is set to 0\n")
      f.write( "XIF	             = 1d0       ! scale factor xi for mu_F (not mu^2!!)\n")	
      f.write( "XIR	             = 1d0	 ! scale factor xi for mu_R\n\n")

      f.write( "!   Physics parameters\n")
      f.write( "!------------------------\n") 
      f.write( "HMASS       = "+ str(self.hmass) +"         ! Higgs mass\n")
      f.write( "HTYPE       = 0               ! Type of Higgs produced\n")
      f.write( "MODEL       = 1               ! model: 1 for SM, 2 for MSSM\n")
      f.write( "HWIDTH      = -999d0          ! Higgs width (set to -999d0 for internal calculation)\n")
      f.write( "TOPMASS     = 172.4d0         ! Top mass\n")
      f.write( "BOTTOMMASS  = 4.855d0         ! Bottom Pole mass\n")
      f.write( "CHARMMASS   = 1.65d0          ! Charm Pole mass\n")
      f.write( "TAU_MASS    = 1.77684D0       ! Tau mass\n")
      f.write( "EWSCHEME    = 3               ! Choose scheme for electroweak parameters (1,2,3,4,5,6)\n")
      f.write( "DEL_ALFA    = 0.059047686d0   ! Delta alfa\n")
      f.write( "ANOM_CPL    = "+ str(self.anom_cpl) +"           ! Anomalous couplings\n") 
      f.write( "KK_MOD      = false           ! Warped Higgsless Model\n")
      f.write( "SPIN2       = "+ str(self.spin2) + "            ! Spin-2 model\n")
      f.write( "EW_APPROX   = 5               ! Approximation used when calculating electroweak \n\n")
      
      f.write( "FERMI_CONST = 1.16637d-5      ! Fermi Constant\n")
      f.write( "INVALFA     = 137.035999679D0 ! 1/fine-structure constant\n")
      f.write( "SIN2W       = 0.23116d0      ! Weak mixing angle\n")
      f.write( "WMASS       = 80.398d0        ! W mass\n")
      f.write( "ZMASS       = 91.1876d0       ! Z mass\n\n")

      f.write( "!   Parameters for the LHA event output\n")
      f.write( "!-----------------------------------------\n")
      f.write( "LHA_SWITCH            = true      ! Les Houches interface only for LO calculation\n")
      f.write( "LHA_FILE              = event.lhe  ! Name of Les Houches output file\n")
      f.write( "HEPMC_SWITCH          = false      ! HepMC interface only for LO calculation\n")
      f.write( "HEPMC_FILE            = event.hepmc ! Name of HepMC output file\n")
      f.write( "PREMAXW_FRAC          = 0.005       ! fraction of phase space points to calculate pre-maximal weight\n")
      f.write( "UNWEIGHTING_SWITCH    = true      ! unweighted/weighted (T/F) events event files\n")
      f.write( "PREMAXW_FRAC          = 0.005       ! fraction of phase space points to calculate pre-maximal weight\n")
#      f.write( "PRENEVUNW             = 1000       ! number of events to calculate pre-maximal weight\n")
      f.write( "TAUMASS               = false      ! Include mass of the tau lepton(s) in the event file for VBF processes\n\n")
      
      
      f.write( "!   PDF set parameters\n")
      f.write( "!------------------------\n")
      f.write( "PDF_SWITCH      = "+ str(self.pdf) +"	! which pdfs to use:\n") 
      f.write( "!                          0 = hard-wired cteq6 (LO) and CT10 (NLO)\n")
      f.write( "!                          1 = lhapdf (use LO_PDFNAME etc to specify)\n")
      f.write( "!                          2 = hard-wired mrst2004qed\n")
      f.write( "!                          3 = hard-wired MSTW2008\n")
      f.write( "! choose pdfset and pdfmember here. Look at the LHAPDF manual for details.\n")
      f.write( "LO_PDFNAME    = cteq6ll.LHpdf\n")
      f.write( "NLO_PDFNAME   = CT10.LHgrid\n")
      f.write( "LO_PDFMEMBER  = 0\n")
      f.write( "NLO_PDFMEMBER = 0\n\n")		

      f.write( "!   Parameters for histogram creation\n")
      f.write( "!---------------------------------------\n")
      f.write( "XSECFILE        = xsection        ! name of output-file (+ .out)\n")
      f.write( "ROOT            = false           ! create root-file?\n")
      f.write( "TOP             = false           ! create top-drawer file?\n")
      f.write( "GNU             = false	          ! create gnu-plot script file?\n")
      f.write( "DATA	        = false	          ! create data file?\n")
      f.write( "REPLACE         = true            ! replace output files?\n")
      f.write( "ROOTFILE = histograms      ! name of root-file ( + '.root')\n")
      f.write( "TOPFILE  = histograms      ! name of top-drawer file ( + '.top')\n")
      f.write( "GNUFILE  = histograms	   ! name of gnuplot file ( + '.gp')\n")
      f.write( "DATAFILE = histograms	   ! name of data directory ( + '.dir')\n\n")

    with open( str(self.TestArea)+ '/random.dat','w') as f:
      f.write( "SEED = " + str(self.randomSeed))

    #copy/generate cuts.dat
#    log.info("For now copying cuts.dat  to "+str(self.TestArea)+"/vbfnlo.dat")
#    shutil.copy(self.VbfnloPath+'/share/VBFNLO/cuts.dat', self.TestArea)

    log.info("Writing cuts.dat  to "+str(self.TestArea)+"/cuts.dat")
    with open( str(self.TestArea)+'/cuts.dat','w') as f:
      f.write( " ! input file for the cut parameters						 \n")
      f.write( " 										 \n")
      f.write( " !   Jet cuts									 \n")
      f.write( " !-------------									 \n")
      f.write( " RJJ_MIN    = 0.4d0      ! min jet-jet R separation				 \n")
      f.write( " Y_P_MAX    = 5.0d0      ! max pseudorapidity for partons 			 \n")
      f.write( " PGENKTJET  = -1.0d0      ! exponent of generalised k_T algorithm		 \n")
      f.write( " PT_JET_MIN = 10.0d0     ! min jet pT						 \n")
      f.write( " Y_JET_MAX  = 5.0d0      ! max jet rapidity					 \n")
      f.write( " 										 \n")
      f.write( " !   Lepton cuts (only applied to charged leptons)				 \n")
      f.write( " !--------------------------------------------------				 \n")
      f.write( " Y_L_MAX    = 20d0      ! max lepton rapidity					 \n")
      f.write( " PT_L_MIN   = -10.0d0     ! min lepton pT					 \n")
      f.write( " MLL_MIN    = 15.0d0     ! min. m_l+l- for any comb. of opposite charged leptons \n")
      f.write( " MLL_MAX    = 1d20       ! max. m_l+l- for any comb. of opposite charged leptons \n")
      f.write( " RLL_MIN    = 0.4d0      ! min lepton-lepton R separation			 \n")
      f.write( " RLL_MAX    = 50.0d0     ! max lepton-lepton R separation			 \n")
      f.write( " 										 \n")
      f.write( " !   Photon cuts								 \n")
      f.write( " !----------------								 \n")
      f.write( " Y_G_MAX    = 5d0      ! max pseudorapidity for photons			 \n")
      f.write( " PT_G_MIN   = 0d0        ! min transverse momentum for photons			 \n")
      f.write( " RGG_MIN    = 0.4d0      ! min photon-photon R separation			 \n")
      f.write( " RGG_MAX    = 50.0d0     ! max photon-photon R separation			 \n")
      f.write( " PHISOLCUT  = 0.4d0      ! photon isolation cut					 \n")
      f.write( " EFISOLCUT  = 1d0        ! efficiency of photon isolation cut			 \n")
      f.write( " 										 \n")
      f.write( " !   Additional cuts								 \n")
      f.write( " !---------------------------------						 \n")
      f.write( " RJL_MIN    = 0.6d0      ! min jet-lepton R separation				 \n")
      f.write( " RJG_MIN    = 0.4d0      ! min jet-photon R separation				 \n")
      f.write( " RLG_MIN    = 0.6d0      ! min lepton-photon R separation			 \n")
      f.write( " MLG_MIN    = 0.0d0      ! min. m_lg for any comb. of charged leptons and photons \n")
      f.write( " MLG_MAX    = 1.d20      ! max. m_lg for any comb. of charged leptons and photons \n")
      f.write( " 										 \n")
      f.write( " !   Vector boson fusion cuts (only applied to tagging jets in VBF processes)	 \n")
      f.write( " !----------------------------------------------------------------------------	 \n")
      f.write( " ETAJJ_MIN  = 0d0        ! jet-jet rapidity separation				 \n")
      f.write( " YSIGN      = false      ! jets #1 and #2 must have opposite sign rapidity	 \n")
      f.write( " LRAPIDGAP  = false      ! leptons fall inside rapidity gap			 \n")
      f.write( " DELY_JL    = 0.0d0      ! min y-dist of leptons from tagging jets		 \n")
      f.write( " GRAPIDGAP  = false      ! photons fall inside rapidity gap			 \n")
      f.write( " DELY_JG    = 0.0d0      ! min y-dist of photons from tagging jets		 \n")
      f.write( " 										 \n")
      f.write( " MDIJ_MIN   = 0.0d0    ! dijet min mass cut on tag jet  			 \n")
      f.write( " MDIJ_MAX   = 1d20       ! dijet max mass cut on tag jet  			 \n")
      f.write( " 										 \n")
      f.write( " !   Jet Veto									 \n")
      f.write( " !---------------------------------						 \n")
      f.write( " JVETO      = false      ! veto jet cuts					 \n")	 
      f.write( " DELY_JVETO = 0.0d0      ! min veto-tag y-dist					 \n") 
      f.write( " YMAX_VETO  = 4.5d0      ! max |y| for veto jet					 \n")
      f.write( " PTMIN_VETO = 50.0d0     ! min pT for veto jet                                   \n")  
      
    return 0
    

  def generateSpin2Dat(self):
    log.info("Writing spin2coupl.dat  to "+str(self.TestArea)+"/spin2coupl.dat")
    
    with open( str(self.TestArea)+'/spin2coupl.dat','w') as f:
      f.write( "!! Parameters for spin-2 model\n\n")
      f.write( "f1 = " + str(self.f1) + "\n")
      f.write( "f2 = " + str(self.f2) + "\n")
      f.write( "f3 = " + str(self.f3) + "\n")
      f.write( "f4 = " + str(self.f4) + "\n")
      f.write( "f5 = " + str(self.f5) + "\n\n")
      f.write( "! Triplet couplings\n\n")
      f.write( "f6 = 0d0\n")
      f.write( "f7 = 0d0\n\n")
      f.write( "! energy scale of couplings (singlet and triplet) in GeV\n\n")
      f.write( "lambda = " + str(self.lambd) + "\n\n")

      f.write( "sp2mass     = " + str(self.hmass) +  "! mass of a spin-2 singlet particle\n")
      f.write( "msp2trippm  = " + str(self.hmass) +  "! mass of a (pos. or neg.) charged spin-2 triplet particle\n")
      f.write( "msp2tripn   = " + str(self.hmass) +  "! mass of a neutral spin-2 triplet particle\n\n")

      f.write( "! branching ratio parameters for the widths (0.2 <= brrat(tripn/pm) <= 1.0)\n")
      f.write( "!(Spin-2 particles could decay into particles which are not considered here)\n\n")

      f.write( "brrat        =    1.0d0     ! branching ratio for sp2width (singlet width)\n")
      f.write( "brrattrippm  =    1.0d0     ! branching ratio for sp2trippmwidth (charged triplet width)\n")
      f.write( "brrattripn   =    1.0d0     ! branching ratio for sp2tripnwidth (neutral triplet width)\n\n")

      f.write( "! parameters of the formfactor\n\n")

      f.write( "nff      = " + str(self.nff) +  "    ! exponent of the formfactor\n")
      f.write( "lambdaff = " + str(self.lambdaff) +  "    ! energy scale of the formfactor (in GeV)\n")
    return 0


  def generateAnomCouplDat(self):
    log.info("Writing anom_HVV.dat  to "+str(self.TestArea)+"/vbfnlo.dat")

    with open( str(self.TestArea)+'anom_HVV.dat','w') as f:
      f.write( '------------------------------set anomalous HVV couplings\n')
      f.write( '------------------------------set form factor\n')
      f.write( 'FORMFACTOR = ' + str(self.formfactor_spin0) + '   ! form factor on(=.true.) or off(=.false.)\n')
      f.write( 'MASS_SCALE = ' + str(self.mass_scale) + '   ! mass scale for new physics\n')
      f.write( 'FFAC       = ' + str(self.ffac_param) + '         ! ffac=1: m^2/(q1^2-m^2)*m^2/(q2^2-m^2), ffac=2: C0(m,q1^2,q2^2,mH^2)\n')
      f.write( '------------------------------Parametrisation: Phys. Lett. B591, 297\n')
      f.write( 'PARAMETR1  = .true.    ! switch for this parametrisation; on(=.true.) or off(=.false.)\n')
      f.write( 'LAMBDA5    = ' + str(self.lambda5) + '   !lambda5\n')
      f.write( 'G5E_HWW    = ' + str(self.g5e_hww) + '  ! g5e_hww, CP-even\n')
      f.write( 'G5E_HZZ    = ' + str(self.g5e_hzz) + '  ! g5e_hzz\n') 
      f.write( 'G5E_HGG    = ' + str(self.g5e_hgg) + '  ! g5e_hgg\n')
      f.write( 'G5E_HGZ    = ' + str(self.g5e_hgz) + '  ! g5e_hgz\n')
      f.write( 'G5O_HWW    = ' + str(self.g5o_hww) + '  ! g5o_hww, CP-odd\n')
      f.write( 'G5O_HZZ    = ' + str(self.g5o_hzz) + '  ! g5o_hzz\n') 
      f.write( 'G5O_HGG    = ' + str(self.g5o_hgg) + '  ! g5o_hgg\n')
      f.write( 'G5O_HGZ    = ' + str(self.g5o_hgz) + '  ! g5o_hgz\n')
      f.write( ' ------------------------------Parametrisation of the L3-Collaboration					  \n')
      f.write( ' PARAMETR2  = .false.   ! switch for this parametrisation; on(=.true.) or off=(.false.)			  \n')
      f.write( ' D_EVEN     = 0.0d0     ! d,     CP-even								  \n')
      f.write( ' DB_EVEN    = 0.0d0     ! d_B										  \n')
      f.write( ' DG1Z_EVEN  = -0.060D0  ! Delta_g1_Z									  \n')
      f.write( ' DKGAM_EVEN = 0.077D0   ! Delta_kappa_gamma								  \n')
      f.write( ' D_ODD      = 0.0d0     ! d,    CP-odd									  \n')
      f.write( ' DB_ODD     = 0.0d0     ! d_B										  \n')
      f.write( ' KGAM_ODD   = 0.0d0     ! kappa_gamma									  \n')
      f.write( ' HVV1       = 4         ! choice for anomalous couplings used: hzg = 0, hgg = 1, hzz = 2, hww = 3, all = 4\n')
      f.write( ' -----------------------------Parametrisation: Phys. Lett. B318, 155					  \n')
      f.write( ' -----------------------------(f_BW=0) in (GeV^-2)							  \n')
      f.write( ' PARAMETR3  = .false.   ! switch for this parametrisation; on(=.true.) or off(=.false.)			  \n')
      f.write( ' FWW_EVEN   = 0.0d0     ! f_WW/Lambda^2, CP-even							  \n')
      f.write( ' FBB_EVEN   = 0.0d0     ! f_BB/Lambda^2									  \n')
      f.write( ' FW_EVEN    = -1.44d-5  ! f_W/Lambda^2									  \n')
      f.write( ' FB_EVEN    = 3.82D-5   ! f_B/Lambda^2									  \n')
      f.write( ' FWW_ODD    = 0.0d0     ! f_WW/Lambda^2, CP-odd								  \n')
      f.write( ' FBB_ODD    = 0.0d0     ! f_BB/Lambda^2									  \n')
      f.write( ' FB_ODD     = 0.0d0     ! f_B/Lambda^2									  \n')
      f.write( ' HVV2       = 4         ! choice for anomalous couplings used: hzg = 0, hgg = 1, hzz = 2, hww = 3, all = 4\n')
      f.write( ' -----------------------------for all Parametrisations 							  \n')
      f.write( ' TREEFACW   = ' + str(self.treefacw) + '      ! factor for SM hWW tensor (sin(alpha-beta) in MSSM)        \n')
      f.write( ' TREEFACZ   = ' + str(self.treefacz) + '      ! factor for SM hZZ tensor (sin(alpha-beta) in MSSM)	  \n')
      f.write( ' LOOPFAC    = ' + str(self.loopfac) + '      ! factor multiplying hZgamma and hgammagamma sm loops        \n')
    return 0


  def generateEvents(self):
    if self.error == 1:
      log.error('An error occured! No event generation!')
      return 0  
      
    #print 'Generating events'
    log.info('Start Generating VBFNLO LHEF events at '+str(time.asctime()))
    log.info('Running ' + str(self.executablePath))

    generate = subprocess.Popen( [self.executablePath,''])
    generate.wait()
    os.rename('event.lhe',self.outputEventsName)
    
    rezip = subprocess.Popen(['tar','cvzf',str(self.outputEventsName) + '.tar.gz',self.outputEventsName])
    rezip.wait()
    
    log.info('Finished at '+str(time.asctime()))
    return 0
