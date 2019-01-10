from MadGraphControl.MadGraphUtils import *
import fileinput
import shutil
import subprocess
import os

# General settings
nevents=5500
runName='C4f_c-0p4'
runMode=2 # 0 = single machine, 1 = cluster, 2 = multicore
cluster_type="lsf"
cluster_queue='2nd' #8nm 1nh 8nh 1nd 2nd 1nw 2nw (m = minute, d = day, w = week)
required_accuracy=0.001
mass_window = [ 100 , 250 ]

### PROCESS
mgproc="""define l+ = l+ ta+
define l- = l- ta-
generate p p > w+ b j $$ wp+ wp- w+ w- z a g QED=3 QCD=0 EFT=2 [QCD]
add process p p > w- b~ j $$ wp+ wp- w+ w- z a g QED=3 QCD=0 EFT=2 [QCD]"""
name='t-channel'
process="p p > w+ b j"
model='TEFT_EW_bR_Wp-c4f'

##necessary gridpack settings
gridpack_mode=True
gridpack_dir='madevent/'  

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(runName)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model """+model+"""
set complex_mass_scheme True
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


# --------------------------------------------------------------
#  Start building the cards
# --------------------------------------------------------------

process_dir = new_process(grid_pack=gridpack_dir)

#run_card.dat and set parameters
#run_cardloc=process_dir+'/Cards/run_card.dat'
run_card_extras = {
    'pdlabel'                  : 'lhapdf',
	'lhaid'                    : 260000,
	'reweight_scale'           : 'False',
  'reweight_PDF'             : 'False',
  'store_rwgt_info'          : 'False',
	'parton_shower'            :'PYTHIA8',
	'jetalgo'                  : '-1',
	'jetradius'               : '0.4',
	'ptj'                      : '5.0',
  'etaj'                      : '5.0',
  'dynamical_scale_choice'    : '0',
	'fixed_ren_scale'           : 'True',
  'fixed_fac_scale'           : 'True',
  'muR_ref_fixed'             : '172.5',
  'muF_ref_fixed'             : '172.5',
  'bwcutoff'                  : '50'}

runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),
  run_card_new='run_card.dat', 
	nevts=nevents,
  rand_seed=runArgs.randomSeed,
  beamEnergy=beamEnergy,
  xqcut=0.,
	extras=run_card_extras)

#param_card.dat
param_cardloc=process_dir+'/Cards/param_card.dat'

param_card_blocks = {
'dim6': {
    'Lambda' : '1.000000e+03', 
    'ccc'    : '1.000000e+00',
    'ctG'    : '0', 
    'cft'    : '0',
    'cfb'    : '0', 
    'cfQ1'   : '0', 
    'cfQ3'   : '0', 
    'ctW'    : '0', 
    'ctB'    : '0',
    'cff'    : '0', 
    'cbW'    : '0', 
    'muprime' : '1.725000e+02'}, 
'eft' : {
    'C4f' : '-0.4'}
}

param_card_decays = """#
#*************************
#      Decay widths      *
#*************************
#
#      PDG        Width
DECAY  1   0.000000e+00
#
#      PDG        Width
DECAY  2   0.000000e+00
#
#      PDG        Width
DECAY  3   0.000000e+00
#
#      PDG        Width
DECAY  4   0.000000e+00
#
#      PDG        Width
DECAY  5   0.000000e+00
#
#      PDG        Width                                                 
DECAY  6   1.360000e+00                                                 
#  BR             NDA  ID1    ID2   ...                                 
   1.000000e+00   2    5  24 # 1.32                                     
DECAY -6   1.360000e+00                                                 
#  BR             NDA  ID1    ID2   ...                                 
   1.000000e+00   2   -5 -24 # 1.32                                     
#
#      PDG        Width
DECAY  11   0.000000e+00
#
#      PDG        Width
DECAY  12   0.000000e+00
#
#      PDG        Width
DECAY  13   0.000000e+00
#
#      PDG        Width
DECAY  14   0.000000e+00
#
#      PDG        Width
DECAY  15   0.000000e+00
#
#      PDG        Width
DECAY  16   0.000000e+00
#
#      PDG        Width
DECAY  21   0.000000e+00
#
#      PDG        Width
DECAY  22   0.000000e+00
#
#      PDG        Width
DECAY  23   2.495200e+00
#
#      PDG        Width
DECAY  -24   2.085000e+00                                               
#  BR             NDA  ID1    ID2   ...                                 
      3.3370000e-01   2    1  -2                                         
      3.337000e-01   2    3  -4                                         
      1.082000e-01   2   11 -12                                         
      1.082000e-01   2   13 -14                                         
      1.082000e-01   2   15 -16                                         
DECAY  24   2.085000e+00                                                
#  BR             NDA  ID1    ID2   ...                                 
      3.337000e-01   2   -1   2                                         
      3.337000e-01   2   -3   4                                         
      1.082000e-01   2  -11  12                                         
      1.082000e-01   2  -13  14                                         
      1.082000e-01   2  -15  16                                         
#
#      PDG        Width
DECAY  25   4.070000e-03
#
#      PDG        Width
DECAY  34   0.000000e+00
#                                                
#      PDG        Width
DECAY  9000002   2.495200e+00
#
#      PDG        Width
DECAY  9000003   2.085000e+00
#
#      PDG        Width
DECAY  9000004   2.085000e+00
"""

#create param card
oldcard = open(param_cardloc,'r')
newcard = open('param_card.dat','w')
inBlock = False
theBlock = ''
toKeep = True
mark_rm  = '## INFORMATION FOR DECAY'
mark_keep = '#==========================================================='
for line in oldcard:
  if line.startswith(mark_rm):
    toKeep = False
  if line.startswith(mark_keep):
    toKeep = True
  new_line = line
  for block in param_card_blocks:
    if """Block """+block in line:
      #print(block)
      inBlock = True
      theBlock = block
  if "###################################" in line:
    inBlock = False
  if inBlock == True:    
    for key in param_card_blocks[theBlock]:
        if key in line:
          if key == line.split('#')[1].lstrip(' ').split(' ')[0]:
            value = line.lstrip(' ').split(' ')[1]
            new_line = line.replace(str(value),str(param_card_blocks[theBlock][key]),1)
  if toKeep == True:
    newcard.write(new_line)
  if """Block mass""" in line:
    newcard.write("""    -6 1.725000e+02 # MTbar
   -24 7.982436e+01 # MWmin 
""")
newcard.write(param_card_decays)
oldcard.close()
newcard.close()

#madspin card
madspin_card_loc='madspin_card.dat'

if not hasattr(runArgs, 'inputGenConfFile'):
    fMadSpinCard = open('madspin_card.dat','w')
    fMadSpinCard.write('import Events/'+runName+'/events.lhe.gz\n')                  
    fMadSpinCard.write('set ms_dir MadSpin\n')                                        
else:                                                                               
    os.unlink(gridpack_dir+'Cards/madspin_card.dat')                                  
    fMadSpinCard = open(gridpack_dir+'Cards/madspin_card.dat','w')                    
    fMadSpinCard.write('import '+gridpack_dir+'Events/'+runName+'/events.lhe.gz\n')  
    fMadSpinCard.write('set ms_dir '+gridpack_dir+'MadSpin\n')                        
    fMadSpinCard.write('set seed '+str(10000000+int(runArgs.randomSeed))+'\n')        
fMadSpinCard.write('''set Nevents_for_max_weigth 250 # number of events for the estimate of the max. weight (default: 75)
set max_weight_ps_point 1000  # number of PS to estimate the maximum for each event (default: 400)
decay t > w+ b, w+ > l+ vl
decay t~ > w- b~, w- > l- vl~
decay w+ > l+ vl
decay w- > l- vl~
launch''')
fMadSpinCard.close()  
    
print_cards() #prints the cards in the output (terminal)
cluster_size=100

# cuts file: to define a invariant mass cut around the top mass
fileN = process_dir+'/SubProcesses/cuts.f'
markdecl  = '      passcuts_user=.true. ! event is okay; otherwise it is changed'
markcuts  = 'C PUT HERE YOUR USER-DEFINED CUTS'
markfunc  = 'C NO NEED TO CHANGE ANY OF THE FUNCTIONS BELOW'	
for line in fileinput.input(fileN, inplace=1):
    if line.startswith(markdecl):
        print """

! -------------------------------- !
! --- wbj special analysis ------- !
! -------------------------------- !

! --- Single Top cuts
      double precision pw(0:3), pbjet(0:3), ptop(0:3)
! --- Stuff for Jets
      integer maxtrack,maxjet
      parameter (maxtrack=nexternal, maxjet=nexternal)
      real*8 pjet03(0:3,maxtrack),ptrack(0:3,maxtrack)
      integer ntracks, njets, jetvec(maxtrack)
      double precision jet_ktradius, jet_ktptmin, palgo
! --- analysis functions
      double precision getpt, get_minv1, get_etarapidity
! --- stuff for flavour tagging
      integer i_W, n_Ws, q_W, i_btop, n_btops, pdg_btop 

"""
    print line,
    if line.startswith(markcuts):
        print """
! -------------------------------- !
! --- wbj special analysis ------- !
! -------------------------------- !

      !- get the W
      i_W = 0
      n_Ws = 0
      q_W = 0
      do i=1,nexternal
        if (abs(ipdg(i)).eq.24) then
          i_W=i
          n_Ws = n_Ws +1
          q_W = ipdg(i)/24
        endif
      enddo
      if (n_Ws.ne.1) then
        write(*,*) "WBJ ERROR 1: number of W = ",n_Ws
        stop
      endif

      !- get b from top
      i_btop = 0
      n_btops = 0
      pdg_btop = 5*q_W 
      do i=1,nexternal
        if ((istatus(i).eq.1).and.(ipdg(i).eq.pdg_btop)) then   
          i_btop = i
          n_btops = n_btops+1
        endif
      enddo
      if ((i_btop.eq.0).or.(n_btops.ne.1)) then
        write(*,*) "WBJ ERROR 2: number of btops = ",n_btops
        stop
      endif

    
c --- W momentum
      do i=0,3
        pw(i)=p(i,i_W)
      enddo

c --- add the final state partons, to the tracks for the clustering
c --- start with the b from the top
      do j=0,3
        ptrack(j,1) = p(j,i_btop)
      enddo
      ntracks=1
      do i=1,nexternal
        if ((istatus(i).eq.1).and.(i.ne.i_W).and.(i.ne.i_btop)) then
          ntracks=ntracks+1
          do j=0,3
            ptrack(j,ntracks) = p(j,i)
          enddo
        endif
      enddo

c --- Make call to fastjet
      njets=0
      do i=0,3
       pjet03(i,1)=0d0
       pjet03(i,2)=0d0
       pjet03(i,3)=0d0
      enddo
      jetvec(1)=0
      jetvec(2)=0
      jetvec(3)=0
      jet_ktradius = 0.4d0
      jet_ktptmin  = 0.0d0
      PALGO=-1d0

c******************************************************************************
c     call FASTJET to get all the jets
c
c     INPUT:
c     input momenta:               ptrack(0:3,nexternal), energy is 0th component
c     number of input momenta:     ntracks
c     radius parameter:            jet_ktradius
c     minumum jet pt:              jet_ktptmin
c     jet algorithm:               palgo, 1.0=kt, 0.0=C/A, -1.0 = anti-kt
c
c     OUTPUT:
c     jet momenta:                             pjet03(0:3,nexternal), E is 0th cmpnt
c     the number of jets (with pt > SYCUT):    njets
c     the jet for a given particle 'i':        jetvec(i),   note that this is
c     the particle in ptrack, which doesn't necessarily correspond to the particle
c     label in the process


      CALL amcatnlo_fastjetppgenkt(PTRACK,NTRACKS,
     & JET_KTRADIUS,JET_KTPTMIN,PALGO,PJET03,NJETS,JETVEC)

!     get the bjet
!     jetvec = parton index in ptrack
      if (jetvec(1).eq.1) then
        do i=0,3
          pbjet(i) = pjet03(i,1)
        enddo
      else if (jetvec(1).eq.2) then
        do i=0,3
          pbjet(i) = pjet03(i,2)
        enddo
      else if (jetvec(1).eq.3) then
        do i=0,3
          pbjet(i) = pjet03(i,3)
        enddo
      else
        write(*,*) "maybe this"
        passcuts_user=.false.
        return
      endif

      if (pbjet(0).eq.0) then
        write(*,*) "WBJ ERROR 3: no b-jet"
        stop
      endif

      do i=0,3
        ptop(i) = pbjet(i)+pw(i)
      enddo


!----CUTS--------
c invariant mass cut on top
      if ((get_minv1(ptop).gt."""+str(mass_window[1])+"""d0) !--inv
     & .or.(get_minv1(ptop).lt."""+str(mass_window[0])+"""d0) !-inv
     &   .or.(getpt(pbjet).lt.5d0)) then
        passcuts_user=.false.
        return
      endif
!----CUTS--------

! -------------------------------- !
! --- wbj special analysis ------- !
! -------------------------------- !
"""
    if line.startswith(markfunc):
        print """
! ---------------------------------------- c
! --- some helpful analysis functions  --- c
! ---------------------------------------- c
      function getpt(p)
      implicit none
      double precision p(0:3), getpt
      getpt=sqrt(p(1)**2+p(2)**2)
      return
      end

      function get_yrapidity(p)
      implicit none
      double precision p(0:3), get_yrapidity
      get_yrapidity=(p(0)+p(3))/(p(0)-p(3))
      if (get_yrapidity.lt.1d-12) then
      get_yrapidity=100d0
      else
      get_yrapidity=0.5d0*log(get_yrapidity)
      endif
      return
      end

      function get_etarapidity(p)
      double precision p(0:3), get_etarapidity
      get_etarapidity=sqrt(p(1)**2+p(2)**2+p(3)**2)
      get_etarapidity=(get_etarapidity+p(3))/(get_etarapidity-p(3))
      if (get_etarapidity.lt.1d-12) then
      get_etarapidity=100d0
      else
      get_etarapidity=0.5d0*log(get_etarapidity)
      endif
      return
      end

      function get_minv1(p)
      implicit none
      double precision p(0:3), get_minv1, minv_sq
      minv_sq = p(0)**2-p(1)**2-p(2)**2-p(3)**2
      get_minv1 = sqrt(abs(minv_sq))
      return
      end
! ---------------------------------------- c
! --- some helpful analysis functions  --- c
! ---------------------------------------- c
"""
##Done with cuts.f    

#raise RuntimeError('Manual issued exit! IGNORE ERRORS this exit is intended!')

#generate events with MG+MS  
generate(run_card_loc='run_card.dat',
  param_card_loc='param_card.dat',
  mode=runMode,
  njobs=8,
  proc_dir=process_dir,
  run_name=runName,
  madspin_card_loc=madspin_card_loc,
  required_accuracy=required_accuracy,
  grid_pack=gridpack_mode,
  gridpack_dir=gridpack_dir,
  nevents=nevents,
  random_seed=runArgs.randomSeed,
  cluster_type=cluster_type,
  cluster_queue=cluster_queue)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(runName)

outputDS=arrange_output(run_name=runName,
    proc_dir=process_dir,
    outputDS=stringy+'._00001.events.tar.gz',
    lhe_version=3,
    saveProcDir=True)

runArgs.inputGeneratorFile=outputDS
