### MadGraphControl for LRSM model
### MadGraph5+Pythia8 for LRSM WR and NR (Keung-Senjanovic process)
### Heavily based on MC12JobOptions/trunk/gencontrol/MadGraphControl_Wprime_tb_hadronic.py and thanks to Mike Hance
### This JO differs from MadGraphControl_LRSM.py only in that the particle widths here are set manually
### MadGraphControl_LRSM.py should be used as standard


from MadGraphControl.MadGraphUtils import *

# Grab proccess card and move it into place
proccard = subprocess.Popen(['get_files','-data','MadGraph_proc_card_LRSM.dat'])
proccard.wait()
if not os.access('MadGraph_proc_card_LRSM.dat',os.R_OK):
    print 'ERROR: Could not get process card'
else:
    oldcard = open('MadGraph_proc_card_LRSM.dat','r')
    newcard = open('proc_card_mg5.dat','w')
    for line in oldcard:
        newcard.write(line)
    oldcard.close()
    newcard.close()

# Grab the run card and move it into place 
runcard = subprocess.Popen(['get_files','-data','MadGraph_run_card_LRSM.dat'])
runcard.wait()
if not os.access('MadGraph_run_card_LRSM.dat',os.R_OK):
    print 'ERROR: Could not get run card'
elif os.access('run_card.dat',os.R_OK):
    print 'ERROR: Old run card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_run_card_LRSM.dat','r')
    newcard = open('run_card.dat','w')
    for line in oldcard:
        if ' nevents ' in line:
            newcard.write('  %i       = nevents ! Number of unweighted events requested \n'%(1.05*runArgs.maxEvents))
        elif ' iseed ' in line:
            newcard.write('   %i      = iseed   ! rnd seed (0=assigned automatically=default)) \n'%(runArgs.randomSeed))
        elif ' ebeam1 ' in line:
            newcard.write('     %i     = ebeam1  ! beam 1 energy in GeV \n'%(runArgs.ecmEnergy/2))
        elif ' ebeam2 ' in line:
            newcard.write('     %i     = ebeam2  ! beam 2 energy in GeV \n'%(runArgs.ecmEnergy/2))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

# Setting mass points from jobConfig
MW2 = float(runArgs.jobConfig[0].split('WR')[-1].split('_')[0])
MN = float(runArgs.jobConfig[0].split('NR')[-1].split('.')[0])
runNumber = int(runArgs.jobConfig[0].split('MC15.')[-1].split('.')[0])
pdfset = str(runArgs.jobConfig[0].split('_LRSM')[0].split('_')[-1])

# Enable setting of particle widths
# massPoint: [ WW2, WN1, WN2, WN3 ] 
widthDict = {
	'WR3500_NR400' : [ 117.6438,8.836867e-07,8.843588e-07,8.880116e-07 ],
	'WR3800_NR3000' : [ 102.2351,0.0329303,0.03249515,0.03275106 ],
	'WR4200_NR200' : [ 141.7866,1.172203e-08,1.168137e-08,1.166057e-08 ],
	'WR4200_NR400' : [ 141.4243,4.250866e-07,4.254023e-07,4.271774e-07 ],
	'WR4500_NR800' : [ 150.2534,1.253927e-05,1.255824e-05,1.256666e-05 ],
	'WR4750_NR3000' : [ 137.7646,0.01077768,0.01065203,0.01071006 ],
	'WR5200_NR1000' : [ 173.2962,2.225115e-05,2.228752e-05,2.226758e-05 ],
	'WR5200_NR2000' : [ 166.0519,0.000808041,0.0007989131,0.0008037351 ],
	'WR1000_NR1400' : [ 24.98129,5.620727,5.62073,5.621622 ],
	'WR1500_NR1600' : [ 37.78459,0.3111176,0.3111174,0.3110671 ],
	'WR1500_NR1800' : [ 37.78459,2.443151,2.443152,2.443366 ],
	'WR2100_NR1500' : [ 58.41439,0.009500907,0.009522106,0.009548364 ],
	'WR2100_NR1800' : [ 54.77829,0.03129714,0.03135425,0.03114436 ],
	'WR2100_NR2200' : [ 53.07001,0.2273107,0.2273106,0.2272742 ],
	'WR2700_NR300' : [ 90.72197,5.456192e-07,5.455777e-07,5.484455e-07 ],
	'WR5200_NR3000' : [ 154.6004,0.007104404,0.007020242,0.007056453 ],
	'WR5800_NR500' : [ 195.4795,3.819173e-07,3.822552e-07,3.830528e-07 ],
	'WR5800_NR1000' : [ 193.8407,1.431155e-05,1.433455e-05,1.43218e-05 ],
	'WR5800_NR2000' : [ 187.3235,0.0005119932,0.0005062737,0.0005093027 ],
	'WR5800_NR3000' : [ 176.8221,0.004363355,0.004310982,0.004332285 ]
}
massPoint = "WR"+str(int(MW2))+"_NR"+str(int(MN))

# Grab parameter card and move it into place
# Widths are Auto in base param_card
paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_LRSM.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_LRSM.dat',os.R_OK):
    print 'ERROR: Could not get param card'
else:
    oldcard = open('MadGraph_param_card_LRSM.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if ' MW2' in line:
            newcard.write('   34 %e # MW2 \n'%(MW2))
        elif ' MN1' in line:
            newcard.write('  9900012 %e # MN1 \n'%(MN))
        elif ' MN2' in line:
            newcard.write('  9900014 %e # MN2 \n'%(MN))
        elif ' MN3' in line:
            newcard.write('  9900016 %e # MN3 \n'%(MN))
        elif ' WW2' in line:
            newcard.write('DECAY  34 %e # WW2 \n'%(widthDict[massPoint][0]))
        elif ' WN1' in line:
            newcard.write('DECAY 9900012 %e # WN1 \n'%(widthDict[massPoint][1]))
        elif ' WN2' in line:
            newcard.write('DECAY 9900014 %e # WN2 \n'%(widthDict[massPoint][2]))
        elif ' WN3' in line:
            newcard.write('DECAY 9900016 %e # WN3 \n'%(widthDict[massPoint][3]))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

# Triggering generation
process_dir = new_process()
generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir=process_dir)
stringy = 'madgraph.'+str(runNumber)+'.MadGraphPythia8EvtGen_'+pdfset+'_LRSM_WR'+str(int(MW2))+'_NR'+str(int(MN))
arrange_output(run_name='Test',proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')

# PYTHIA 8
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

evgenConfig.description = "MadGraph5+Pythia8 for LRSM WR and NR (Keung-Senjanovic process) with particle widths set manually"
evgenConfig.keywords = ["exotic","Wprime","BSM"]
evgenConfig.contact = ["Xanthe Hoad <xanthe.hoad@cern.ch>"]
evgenConfig.process = "pp > WR > l NR, NR > l WR, WR > j j"

# Finally, run the parton shower and configure MadGraph+Pythia
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
