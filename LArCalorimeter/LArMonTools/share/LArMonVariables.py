#
# All needed variables are set below
#
if 'EvtNo' not in dir():
    EvtNo=10000000 #  Number of events to process
    
if 'SubDet' not in dir():
    SubDet = 'Barrel'
    
if 'InputDir' not in dir():
    InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2009/00102562"
    
if 'FullFileName' in dir():    
   if 'RunNumber' not in dir():
       RunNumber = int(FullFileName.strip().split('.')[1])

   if 'Type' not in dir():
       Type = str(FullFileName.strip().split('.')[2].strip().split('-')[1])
       print(Type)

   if 'Partition' not in dir():
       Partition = str(FullFileName.strip().split('.')[6].split('-')[1])
       print(Partition)

   if not 'FullFileNameTab' in dir():
       FullFileNameTab = [ InputDir+"/"+FullFileName ]

   pass

if 'FullFileName' not in dir():
     include("LArCalibProcessing/GetInputFiles.py")
     if 'FilePrefix' not in dir():
        FilePrefix="data18"
     FullFileName =[]
     FullFileName+=GetInputFilesFromTokens(InputDir,int(RunNumber),FilePrefix,".*")
     if len(FullFileName) == 0:
        print("No input files !!!")
        sys.exit(-1)
     word = FullFileName[0].split('-')
     Type = word[1]
    
    
if not 'online' in dir():
    online = False

if not 'OutputDir' in dir():
    OutputDir="rootFiles/"

if not 'OutputNtupleDir' in dir():
    OutputNtupleDir = "rootFiles/"

if not 'runAccumulator' in dir(): 
    runAccumulator = False # :average mode, = True:transparent mode

if not 'RefRunNumber' in dir():
    RefRunNumber = 2095     # runnumber of reference file

if not 'LArDigitKey' in dir():
    LArMonFlags.LArDigitKey = 'HIGH'   # for LArRawChannelBuilder

if not 'LArRawChannelKey' in dir():
    LArRawChannelKey="LArRawChannels"


if not 'DelayNtuple' in dir():
    DelayNtuple = False

if not 'PeakOF' in dir():
    PeakOF = False

if not 'simpleBuilder' in dir():
    simpleBuilder = False

#
# Output file naming
#
include('LArMonTools/LArMonOutputFileName.py')
