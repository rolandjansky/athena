#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# WriteTierZeroArgdict.py
#
# Utility to create json argument dictionary for a given AMI tag, filename and datasetname 
#
# To see script usage, type: 
#            python CreateTierZeroArgdict.py -h 
#
# Examples of usage:  python CreateTierZeroArgdict.py f622 data15_13TeV.00284473.physics_Main.daq.RAW data15_13TeV.00284473.physics_Main.daq.RAW._lb0267._SFO-3._0001.data 
#                     python CreateTierZeroArgdict.py --maxEvents 20 --jobnr 1 --ncores 4 --NoMergeTypeList AOD,ESD,DRAW_EGZ,DRAW_EMU,DRAW_TAUMUH,DRAW_ZMUMU f622 data15_13TeV.00284473.physics_Main.daq.RAW data15_13TeV.00284473.physics_Main.daq.RAW._lb0267._SFO-3._0001.data 


import os, sys, json, traceback, re, argparse
from pprint import pprint as pp

#####################################
# command line argument parsing 
#####################################
parser = argparse.ArgumentParser(add_help=True,usage='\n\nThis utility creates json argument dictionary and prints instructions for running a Tier-0 job with it. \
                                                      \nThis script shall be used only with reconstruction related amitags (x/f/c flavours). \
                                                      \nAMI tag, dataset and file names are required input arguments. \
                                                      \n\
                                                      \n1. source AMI from the command line:\
                                                      \n        source /afs/cern.ch/atlas/software/tools/pyAMI/setup.sh \
                                                      \n2. set up an encrypted configuration file to be able to connect to AMI with your AMI username and password, type:\
                                                      \n        ami auth\
                                                      \n3. run the script: \
                                                      \n        CreateTierZeroArgdict.py [-h] [--maxEvents MAXEVENTS] [--ncores NCORES] \
                                                      \n                                      [--NoMergeTypeList NOMERGETYPELIST] \
                                                      \n                                      amitag dsname inputflname')
#parser = argparse.ArgumentParser(add_help=True)
parser.add_argument("amitag", type=str, help="specify amitag")
parser.add_argument("dsname", type=str, help="specify datasetname")
parser.add_argument("inputflname", type=str, help="specify filename")

parser.add_argument("--maxEvents",
                  dest='maxEvents', type=int, default=20,
                  help = 'specify maximum number of events to process (default: %d)' % 20)

parser.add_argument('--ncores',
                  dest='ncores',  type=int, default=1,
                  help='specify number of cores for athemaMP job (default %s)' % 1)

parser.add_argument('--NoMergeTypeList',
                  dest='NoMergeTypeList', type=str, default='AOD,ESD,DRAW_EGZ,DRAW_EMU,DRAW_TAUMUH,DRAW_ZMUMU',
                  help='specify comma separated list of output types which you do not want to be merged on the node when running in multicore mode (default: %s)' % 'AOD,ESD,DRAW_EGZ,DRAW_EMU,DRAW_TAUMUH,DRAW_ZMUMU')

args = parser.parse_args()


import pyAMI.client 

#print "args: ",args

# positional arguments
amitag= args.amitag 
dsname=args.dsname 
inputflname=args.inputflname 
#
# default values of optiona arguments
maxEvents=args.maxEvents
ncores=args.ncores
NoMergeTypeList=args.NoMergeTypeList

taskstep= 'recon'

####################
# utility functions  
####################

def resolve(tag) :
    try :
        amiuser = '' 
        amipwd  = ''
        amiendpoint  = 'atlas'
        ami = pyAMI.client.Client(endpoint = amiendpoint)

        command = ['GetAMITagInfo','-amiTag=%s' % tag,]

        if amiuser and amipwd :
            amires = ami.execute(command, format = 'dict_object', AMIUser = amiuser, AMIPass = amipwd)
        else :
            amires = ami.execute(command, format = 'dict_object')

        # extract the result
        r = amires.get_rows('amiTagInfo')[0]   # assuming simple tag, i.e. not chained
        # TODO get rid of these empty strings/str/eval at some point 
        res = {'outputs' : eval(r.get('outputs','{}')),
               'inputs' : eval(r.get('inputs','{}')),
               'phconfig' : eval(r.get('phconfig','{}')),
               'moreInfo' : eval(r.get('moreInfo','{}')),
               'transformation' : str(r.get('transformation','')),
               'trfsetupcmd' : str(r.get('trfsetupcmd','')),     
               }
    except :
        traceback.print_exc()
        res = 'error'
    return res


def evalDatasetNameFion(dsname,dsfionstr):
    # returns derived dsn or None
    # raises 'error' in case of problems
    tokens = dsfionstr.split()
    if tokens[0] == 'lambda' :
        dsfion=eval(dsfionstr)
        newdsname=dsfion(dsname)
    elif tokens[0] == 'replace' :
        newdsname = dsname
        pairs = tokens[1:]
        while len(pairs) > 1 :
            newdsname = newdsname.replace(pairs[0],pairs[1])
            pairs = pairs[2:]
    elif tokens[0] == 'replace_nth' :
        parts = dsname.split('.')
        pairs = tokens[1:]
        while len(pairs) > 1 :
            pos = int(pairs[0])        # pos starts counting from 1 !
            new = pairs[1]
            parts[pos-1] = new
            pairs = pairs[2:]
        newdsname = '.'.join(parts)
    else :
        prnt("ERROR >>> unexpected dsnamefion syntax")
        raise Exception('error')
    return newdsname


# Executed from the command line
if __name__ == '__main__':
    
    # extracting information about input file name partition ID for naming of the outputs/logfile/argdict 
    partID='._0001'
    try:
        partIDlist = inputflname.split('._')[1:]
        partIDlist[-1]=partIDlist[-1].split('.')[0]
        if len(partIDlist)!=0 :
            if partIDlist[0]: 
                partID='._'+'._'.join(partIDlist)
            else:
                pass
        else: 
            prnt("WARNING >>> input file name does not have the expected format - no partition identifiers separated by '._' found in the inputfilename lbXXXX._SFO-X._XXXX, ._lb0000._SFO-0._0000.job,<jobnr>.log will be used instead for the log file name")
            pass
    except:
        pass 
    
    logfilename=dsname+"."+ amitag+"."+taskstep+partID+".job.log"
    jsonflname=dsname+"."+ amitag+"."+taskstep+partID+".job.argdict.json"
    jobargdict={}
    
    # resolve AMI tag
    ami_tag_content = resolve(amitag)
     
    # translating AMI tag info into job argument dictionary

    # always adding/rewriting maxEvents field with the number of events passed as parameter to this script
    phconfig=ami_tag_content.get('phconfig',{})
    phconfig['maxEvents']=maxEvents
    
    # disabling merging of outputs on the node when running athenaMP
    # must be specified as a comma separated list of types (parameter to this script) 
    if ncores>1: 
        for mergetype in NoMergeTypeList.split(","):
            if phconfig.has_key("athenaMPMergeTargetSize") : 
                phconfig["athenaMPMergeTargetSize"].update({mergetype:0.0})
            else: 
                phconfig["athenaMPMergeTargetSize"]={mergetype:0.0}
    
    # adding multicore processing instruction in case ncores > 1             
    if ncores > 1 : 
        parncores='--nprocs=%i' % ncores
        phconfig['athenaopts']=[parncores]
    
    jobargdict.update(phconfig)
    
    #checking if AMI tag contains empty 'inputBSFile' dictionary 
    if ami_tag_content.get('inputs',{}).has_key('inputBSFile'):
        # if yes - writing the inputfile into the argument dictionary according to the Tier-0 nomenclature
        if not ami_tag_content.get('inputs',{})['inputBSFile'] :
            jobargdict['inputBSFile']=[dsname+'#'+inputflname]
        else :
            print "ERROR - not recognized inputs in AMI tag; script works only for the following AMI tag configuration of the inputs \n 'inputs': {'inputBSFile': {}}"
            sys.exit(1)
    
    # starting translating the output dataset names form the ifMatch expressions specified in the AMI tag  
    outputs= ami_tag_content.get('outputs',{})
    dstype= dsname.split('.')[-1]
    outdatasets={}

    for k,v in outputs.items() :
        parname = k #v.get('parname')
        dstp = v.get('dstype')
        # handle meta cases
        if dstp == '!likeinput' :
            dstp = dstype
        elif dstp.startswith('!replace') : # allow multiple, pair-wise replace
            pcs = dstp.split()
            pairs = pcs[1:]
            dstp = dstype
            while len(pairs) > 1 :
                dstp = dstp.replace(pairs[0],pairs[1])
                pairs = pairs[2:]
    
        ifmatch = v.get('ifMatch',None)
        # skip this output if there is an ifMatch and it does not match
        if ifmatch :
            m = re.match(ifmatch,dsname)
            if not m : continue
    
        dsfionstr = v.get('dsnamefion',None)
        # interpret dsfion string
        if dsfionstr :
            outdsname = evalDatasetNameFion(dsname,dsfionstr)
        else :
            # apply default nomenclature
            pcs=dsname.split('.')
            if len(pcs)<5 : 
                print "Dataset name must have at least 5 parts separated by comma to comply with Tier-0 naming convention: <project>.<runnr>.<stream>.<granularity>.<type>"
                sys.exit(1)
            pcs[3]=taskstep
            pcs[4]=dstp
            pcs.append(str(amitag))
            outdsname='.'.join(pcs)
    
        #outdsname = outdsname+tasktag
        #outdatasets[parname] = [outdsname,dstp]
        outdatasets[parname] = outdsname

    # assembling the dictionary of outputs 
    for parname in outdatasets.keys() :
        if not parname.startswith('_') :
            outdsname=outdatasets[parname]
            outfilename=outdsname+partID
            outfilevalue=outdsname+'#'+outfilename
            jobargdict[parname]=outfilevalue
    
    jobargdictfile = open(jsonflname,"w")
    json.dump(jobargdict, jobargdictfile,sort_keys=True, indent=4, separators=(',', ': '))
    jobargdictfile.close()
    

    print "####################################################################" 
    print "#\n# To run a local job:"
    print "#\n# 1. Copy the following files into a directory from which you intend to run your job:\n#    %s\n#    %s " %(jsonflname,inputflname)
    print "#\n# 2. Setup the environment: \n#    source "+ ami_tag_content.get('trfsetupcmd',{})   
    print "#\n# 3. Run the job: \n#    python -u `which "+ami_tag_content.get('transformation',{})+"` --argJSON="+jsonflname+" &> "+logfilename 
    print "#"
    print "####################################################################" 
    print "#\n# Note aside: to copy inputfiles from EOS (if still present there) one can usually use commands like:"
    print "#    xrdcp -f --nopbar root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/%s/%s/%s/%s/%s %s" % (dsname.split(".")[0],dsname.split(".")[2],dsname.split(".")[1],dsname,inputflname,inputflname)
    print "#\n# OR from castor disk - assuming the file is STAGED:"
    print "#     export STAGE_SVCCLASS=t0atlas" 
    print "#     rfcp /castor/cern.ch/grid/atlas/rucio/raw/%s/%s/%s/%s/%s ." % (dsname.split(".")[0],dsname.split(".")[2],dsname.split(".")[1],dsname,inputflname)
    print "#"
    print "####################################################################" 
    
