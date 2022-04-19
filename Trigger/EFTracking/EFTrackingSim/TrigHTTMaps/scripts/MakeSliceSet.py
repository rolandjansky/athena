import os
import argparse
import sys
import itertools

# python MakeSliceSet.py /eos/atlas/atlascerngroupdisk/det-htt/HTTsim/user/abkahn/ExtrapolationEngine/ATLAS-P2-ITK-22-02-00/ATLAS-P2-ITK-22-02-00_01eta03_03phi05.root --outfilelabel rmaps/nosmear_eta_01_03_phi_03_05

parser = argparse.ArgumentParser(description='')
parser.add_argument('--test',dest='test', action='store_true')
parser.add_argument('inname', metavar='inputfile', type=str, nargs='+',
                    help='input file name (extrapolator files or directory')
parser.add_argument('--outfilelabel', action='store', type=str, 
                    help='label to start outfile with', default="Unlabeled")


args = parser.parse_args()

def runAbortIfFails(command):
    retv=os.system(command)
    if retv!=0:
        sys.exit(retv)


keylayers=["pixel,barrel,0",
           "pixel,barrel,4",
           "strip,barrel,0",
           "strip,barrel,2"
]

configs=["--nslices 6",
         "--fullgran"]

for keylayer,config in  itertools.product(keylayers,configs):
    logname = args.outfilelabel+"_"+keylayer.replace(",","_")+"_"+config.replace("--","").replace(" ","")+".log"
    runAbortIfFails("python module_driven_slicing.py  --keylayer %s %s --outfilelabel %s --trim 0.01  %s > %s" %
                    (keylayer,config,args.outfilelabel," ".join(args.inname),logname))

