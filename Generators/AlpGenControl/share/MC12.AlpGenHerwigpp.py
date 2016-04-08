# Common joboptions for Alpgen+Herwig++
# Contact: m.k.bugge@fys.uio.no

from AthenaCommon import Logging
log = Logging.logging.getLogger('Generate.Stop')

if not 'evgenConfig' in dir():
    raise RuntimeError('These jobOptions should be run through Generate_trf.py')

# setup Herwig++
include("MC12JobOptions/Herwigpp_UEEE4_CTEQ6L1_Common.py")

#Look for tarball, and extract if found
for f in os.listdir('./'):
    if '.tar' in f or '.tgz' in f:
        print 'Extracting archive ',f
        os.system('tar -xvf '+f)

#Look for input files (*.events)
inputOKevts = False
prefix = ''
for f in os.listdir('./'):
    fsplit = f.split('.')
    
    if fsplit[-1] == 'events':
        prf = fsplit[0]
        for i in xrange(1,len(fsplit)-1):
            prf += '.'+fsplit[i]
        prefix = prf
        
        #Throw an error if a .events file is already found
        if inputOKevts:
            raise RuntimeError('There is more than one .events file in run directory. Please process one input file at a time.')
        else:
            inputOKevts = True


if not inputOKevts:
    raise RuntimeError('Could not find unweighted events file *.events in run directory')

if not prefix+'.dat' in os.listdir('./'):
    raise RuntimeError('Could not find parameters file '+prefix+'.dat in run directory')

#Set up Herwig++ to process Alpgen inputs
topAlg.Herwigpp.doAlpgen = True
topAlg.Herwigpp.alpgenFilenamePrefix = prefix

#If you want to try different settings for Herwig++ - see http://folk.uio.no/magnarkb/HerwigppAlpgenUpdate.pdf
#topAlg.Herwigpp.useTwoLoopAlphas = False
#topAlg.Herwigpp.useShowerImprovement = False

# define metadata
evgenConfig.description = 'Alpgen+Herwig++' 
evgenConfig.keywords = ['Alpgen']
evgenConfig.contact  = ['m.k.bugge@fys.uio.no']

topAlg.Herwigpp.InFileDump='steeringFileAlpgen.in'

