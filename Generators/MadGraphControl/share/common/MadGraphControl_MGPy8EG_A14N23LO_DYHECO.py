#MadGraphControl_MGPy8EG_A14N23LO_DYHECO2.py

PdgId = 10000000+charge*100
charge_int, charge_frac = divmod(charge, 1) # integer part of chargePoint goes into charge_int (float), decimal part goes into charge_frac (float)
charge_frac *= 10
print 'Integer part of charge is', charge_int, ', fractional part is', charge_frac

print 'RunNumber is', runArgs.runNumber, 'mass is', mass, 'GeV, charge is +/-', charge, 'e'
evgenConfig.description="Drell-Yan HECO generation for mass = %i GeV, charge = +/- %.1fe with MadGraph5_aMC@NLO+Pythia8, NNPDF23LO pdf and A14 tune in MC15" % (mass, charge)
proc_card = """
import model multichargedParticles
generate p p > qb%ip%i qb%ip%i~ QED=2""" % (charge_int, charge_frac, charge_int, charge_frac)
proc_name = "Multicharges_M%iZ%ip%i" % (mass, charge_int, charge_frac)
evgenConfig.keywords += ['exotic','drellYan','BSM','longLived']

param_card_extras = { "MULTICHARGEDPARTICLES": { 'MQb%ip%i' %(charge_int, charge_frac): mass}} # changing mass in the param_card
run_card_extras = {'lhe_version':'2.0' , 'ptheavy':'%s' % ptcut}

evgenConfig.contact = ['anthony.lionti@cern.ch']

evgenConfig.specialConfig = "MASS=%i;CHARGE=%.1f;preInclude=SimulationJobOptions/preInclude.Qball.py;MDT_QballConfig=True" % (mass, charge)

include("MadGraphControl/MadGraphControl_Pythia8_A14_NNPDF23LO_EvtGen_Common.py") # common MadGraphOTF <-> Pythia8 liaison


# Edit PDGTABLE.MeV with HECOs mass

ALINE1="M %i                         %i.E+03       +0.0E+00 -0.0E+00 qb%ip%i             +" % (PdgId,mass,charge_int,charge_frac)
ALINE2="W %i                         0.E+00         +0.0E+00 -0.0E+00 qb%ip%i              +" % (PdgId,charge_int,charge_frac)

import os, sys

pdgmod = os.path.isfile('PDGTABLE.MeV')
if pdgmod is True:
	os.remove('PDGTABLE.MeV')
os.system('get_files -data PDGTABLE.MeV')
f=open('PDGTABLE.MeV','a')
f.writelines(str(ALINE1))
f.writelines('\n')
f.writelines(str(ALINE2))
f.writelines('\n')
f.close()

del ALINE1
del ALINE2

# Edit G4particle_whitelist.txt with MCPs

ALINE3="%i   qb%ip%i  %i (Mev/c) lepton 0" %(PdgId,charge_int,charge_frac,1000*mass)
ALINE4="%i   qb%ip%ibar  %i (Mev/c) lepton 0" %((-1)*PdgId,charge_int,charge_frac,1000*mass)

pdgmod = os.path.isfile('G4particle_whitelist.txt')
if pdgmod is True:
	os.remove('G4particle_whitelist.txt')
os.system('get_files -data G4particle_whitelist.txt')
f=open('G4particle_whitelist.txt','a')
f.writelines(str(ALINE3))
f.writelines('\n')
f.writelines(str(ALINE4))
f.writelines('\n')
f.close()

del ALINE3
del ALINE4

# These two lines below fix an issue with Pythia 8.205 not recognizing multi-charged particles. For details please check the https://groups.cern.ch/group/hn-atlas-Generators/Lists/Archive/Flat.aspx?RootFolder=%2Fgroup%2Fhn-atlas-Generators%2FLists%2FArchive%2FError%20in%20Pythiacheck%20unknown%20particle%20code&FolderCTID=0x012002006E9F14B8795719469C62A1525BB20B42
exec('genSeq.Pythia8.Commands += [\"'+str(int(PdgId))+':all = qb'+str(int(charge_int))+'p'+str(int(charge_frac))+' qb'+str(int(charge_int))+'p'+str(int(charge_frac))+'bar 2 '+str(charge*3)+' 0 '+str(mass)+'\"]')
exec('genSeq.Pythia8.Commands += [\"-'+str(int(PdgId))+':all = qb'+str(int(charge_int))+'p'+str(int(charge_frac))+'bar qb'+str(int(charge_int))+'p'+str(int(charge_frac))+' 2 '+str(charge*(-3))+' 0 '+str(mass)+'\"]')
