# MadGraphControl_Multicharges.py
# by Yury.Smirnov@cern.ch

mass_points_MC15c = [
"50", "200", "400", "600", "800", "1000", "1200", "1400", 
"50", "200", "400", "600", "800", "1000", "1200", "1400", 
"50", "200", "400", "600", "800", "1000", "1200", "1400", 
"50", "200", "400", "600", "800", "1000", "1200", "1400", 
"50", "200", "400", "600", "800", "1000", "1200", "1400", 
"50", "200", "400", "600", "800", "1000", "1200", "1400", 
"50", "200", "400", "600", "800", "1000", "1200", "1400", 
"50", "200", "400", "600", "800", "1000", "1200", "1400", 
"50", "200", "400", "600", "800", "1000", "1200", "1400", 
"50", "200", "400", "600", "800", "1000", "1200", "1400", 
"50", "200", "400", "600", "800", "1000", "1200", "1400"] #GeV

mass_points_MC16 = [
"500", "800", "1100", "1400", "1700", "2000", 
"500", "800", "1100", "1400", "1700", "2000", 
"500", "800", "1100", "1400", "1700", "2000", 
"500", "800", "1100", "1400", "1700", "2000", 
"500", "800", "1100", "1400", "1700", "2000", 
"500", "800", "1100", "1400", "1700", "2000"] #GeV

charge_points_MC15 = [
"2.0", "2.0", "2.0", "2.0", "2.0", "2.0", "2.0", "2.0",
"2.5", "2.5", "2.5", "2.5", "2.5", "2.5", "2.5", "2.5",
"3.0", "3.0", "3.0", "3.0", "3.0", "3.0", "3.0", "3.0",
"3.5", "3.5", "3.5", "3.5", "3.5", "3.5", "3.5", "3.5",
"4.0", "4.0", "4.0", "4.0", "4.0", "4.0", "4.0", "4.0",
"4.5", "4.5", "4.5", "4.5", "4.5", "4.5", "4.5", "4.5",
"5.0", "5.0", "5.0", "5.0", "5.0", "5.0", "5.0", "5.0",
"5.5", "5.5", "5.5", "5.5", "5.5", "5.5", "5.5", "5.5",
"6.0", "6.0", "6.0", "6.0", "6.0", "6.0", "6.0", "6.0",
"6.5", "6.5", "6.5", "6.5", "6.5", "6.5", "6.5", "6.5",
"7.0", "7.0", "7.0", "7.0", "7.0", "7.0", "7.0", "7.0"]

charge_points_MC16 = [
"2.0", "2.0", "2.0", "2.0", "2.0", "2.0",
"3.0", "3.0", "3.0", "3.0", "3.0", "3.0",
"4.0", "4.0", "4.0", "4.0", "4.0", "4.0",
"5.0", "5.0", "5.0", "5.0", "5.0", "5.0",
"6.0", "6.0", "6.0", "6.0", "6.0", "6.0",
"7.0", "7.0", "7.0", "7.0", "7.0", "7.0"]



firstDsid_MC15c = 305235
lastDsid_MC15c = 305322

firstDsid_MC16 = 310953
lastDsid_MC16 = 310988

if runArgs.runNumber == 303743 or runArgs.runNumber == 303749:	# MC15a
	massPoint=600 # all masses are in GeV
	chargePoint=2.0
elif runArgs.runNumber == 303744 or runArgs.runNumber == 303750:	# MC15a
	massPoint=900
	chargePoint=4.5
elif runArgs.runNumber == 303745 or runArgs.runNumber == 303747:	# MC15a
	massPoint=1300
	chargePoint=7.0
elif runArgs.runNumber == 303746 or runArgs.runNumber == 303748:	# MC15a
	massPoint=1000
	chargePoint=10.0
elif runArgs.runNumber>=firstDsid_MC15c and runArgs.runNumber<=lastDsid_MC15c:		# MC15c
	massPoint = int(mass_points_MC15c[runArgs.runNumber-firstDsid_MC15c])
	chargePoint = float(charge_points_MC15[runArgs.runNumber-firstDsid_MC15c])
elif runArgs.runNumber>=firstDsid_MC16 and runArgs.runNumber<=lastDsid_MC16:		# MC16
	massPoint = int(mass_points_MC16[runArgs.runNumber-firstDsid_MC16])
	chargePoint = float(charge_points_MC16[runArgs.runNumber-firstDsid_MC16])
else:
	raise RuntimeError("DSID %d is not recognized or it does not belong to the supported production campaigns" % runArgs.runNumber)

PdgId = 10000000+chargePoint*100
charge_int, charge_frac = divmod(chargePoint, 1) # integer part of chargePoint goes into charge_int (float), decimal part goes into charge_frac (float)
charge_frac *= 10
print 'Integer part of charge is', charge_int, ', fractional part is', charge_frac

if runArgs.runNumber>=303743 and runArgs.runNumber<=303748:	# MC15a, no jets
	print 'RunNumber is', runArgs.runNumber, ': no jets, mass is', massPoint, 'GeV, charge is +/-', chargePoint, 'e'
	evgenConfig.description="Drell-Yan (w/o jets) multi-charged particles generation for mass = %i GeV, charge = +/- %.1fe with MadGraph5_aMC@NLO+Pythia8, NNPDF23LO pdf and A14 tune in MC15" % (massPoint, chargePoint)
	proc_card = """
import model multichargedParticles
generate p p > qb%ip%i qb%ip%i~ QED=2""" % (charge_int, charge_frac, charge_int, charge_frac)
	proc_name = "Multicharges_NoJets_M%iZ%ip%i" % (massPoint, charge_int, charge_frac)
	evgenConfig.keywords += ['exotic','drellYan','BSM','0jet'] # should match words in 
        https://svnweb.cern.ch/trac/atlasoff/browser/Generators/EvgenJobTransforms/trunk/share/file/evgenkeywords.txt
elif runArgs.runNumber>=303749 and runArgs.runNumber<=303750:	# MC15a, 1 jet
	print 'RunNumber is', runArgs.runNumber, ': 1 jet, mass is', massPoint, 'GeV, charge is +/-', chargePoint, 'e'
	evgenConfig.description="Drell-Yan (w/ 1 jet) multi-charged particles generation for mass = %i GeV, charge = +/- %.1fe with MadGraph5_aMC@NLO+Pythia8, NNPDF23LO pdf and A14 tune in MC15" % (massPoint, chargePoint)
	proc_card = """
import model multichargedParticles
generate p p > qb%ip%i qb%ip%i~ j QED=2""" % (charge_int, charge_frac, charge_int, charge_frac)
	proc_name = "Multicharges_1Jet_M%iZ%ip%i" % (massPoint, charge_int, charge_frac)
	evgenConfig.keywords += ['exotic','drellYan','BSM','1jet']
else:	# MC15c or MC16, no jets of course
	evgenConfig.description="Drell-Yan multi-charged particles generation for mass = %i GeV, charge = +/- %.1fe with MadGraph5_aMC@NLO+Pythia8, NNPDF23LO pdf and A14 tune in MC15" % (massPoint, chargePoint)
	proc_card = """
import model multichargedParticles
generate p p > qb%ip%i qb%ip%i~ QED=2""" % (charge_int, charge_frac, charge_int, charge_frac)
	proc_name = "Multicharges_M%iZ%ip%i" % (massPoint, charge_int, charge_frac)
	evgenConfig.keywords += ['exotic','drellYan','BSM','longLived']

param_card_extras = { "MULTICHARGEDPARTICLES": { 'MQb%ip%i' %(charge_int, charge_frac): massPoint}} # changing mass in the param_card
run_card_extras = { 'lhe_version':'2.0'}

evgenConfig.contact = ['Yury.Smirnov@cern.ch']

evgenConfig.specialConfig = "MASS=%i;CHARGE=%.1f;preInclude=SimulationJobOptions/preInclude.Qball.py;MDT_QballConfig=True" % (massPoint, chargePoint)

include("MadGraphControl/MadGraphControl_Pythia8_A14_NNPDF23LO_EvtGen_Common.py") # common MadGraphOTF <-> Pythia8 liaison


# Edit PDGTABLE.MeV with MCPs mass

ALINE1="M %i                         %i.E+03       +0.0E+00 -0.0E+00 qb%ip%i             +" % (PdgId,massPoint,charge_int,charge_frac)
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

ALINE3="%i   qb%ip%i  %i (Mev/c) lepton 0" %(PdgId,charge_int,charge_frac,1000*massPoint)
ALINE4="%i   qb%ip%ibar  %i (Mev/c) lepton 0" %((-1)*PdgId,charge_int,charge_frac,1000*massPoint)

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
exec('genSeq.Pythia8.Commands += [\"'+str(int(PdgId))+':all = qb'+str(int(charge_int))+'p'+str(int(charge_frac))+' qb'+str(int(charge_int))+'p'+str(int(charge_frac))+'bar 2 '+str(chargePoint*3)+' 0 '+str(massPoint)+'\"]')
exec('genSeq.Pythia8.Commands += [\"-'+str(int(PdgId))+':all = qb'+str(int(charge_int))+'p'+str(int(charge_frac))+'bar qb'+str(int(charge_int))+'p'+str(int(charge_frac))+' 2 '+str(chargePoint*(-3))+' 0 '+str(massPoint)+'\"]')
