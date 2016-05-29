#!/usr/bin/env python

#Exampel command: UploadAMITag.larcaf.py wlampl xxx create_c909 AtlasProduction_20.1.5.6 noisebursts
 

import sys,pickle

patcharea='/afs/cern.ch/user/l/larcomm/w0/digitMon/LArCAF'
setupScript='/afs/cern.ch/atlas/tzero/software/setup/setuptrf.sh'
specialT0Setup='/afs/cern.ch/atlas/tzero/software/setup/specialsetup_tier0.sh'
trfsetuppath='/afs/cern.ch/atlas/tzero/software/setup/'
processConfigs = {}

processConfigs['noisebursts'] = {
    'inputs': { 'inputESDFile': {} },
    'outputs': { 'outputNTUP_LARNOISEFile': {'dstype': 'NTUP_LARNOISE'}, }, 
    'tasktransinfo': { 'trfpath': 'LArNoiseBursts_tf.py',
                       'trfsetupcmd': setupScript+ ' '+patcharea+' RELEASE ' + specialT0Setup},
    'phconfig': {}, 
    'description': 'Produced LAr noise burst ntuple from ESD files. Uses RELEASE' 
    }

processConfigs['noiseburstsmerge'] = {
    'inputs': { 'inputNTUP_LARNOISEFile': {} },
    'outputs': { 'outputNTUP_LARNOISE_MRGFile': {'dstype': 'NTUP_LARNOISE_MRG'}, }, 
    'tasktransinfo': { 'trfpath': 'NTUPMerge_tf.py',
                       'trfsetupcmd': setupScript+ ' '+patcharea+' RELEASE ' + specialT0Setup},
    'phconfig': {}, 
    'description': 'Merge LAr noise burst ntuple files. Uses RELEASE' 
    }


processConfigs['reco'] = {
    'inputs': { 'inputBSFile': {} },
    'outputs': { 'outputNTUP_SAMPLESMONFile': {'dstype': 'NTUP_SAMPLESMON'}, }, 
    'tasktransinfo': { 'trfpath': 'LArCAF_tf.py',
                       'trfsetupcmd': setupScript+ ' '+patcharea+' RELEASE ' + specialT0Setup},
    'phconfig': {}, 
    'description': 'Runs LAr Samples monitoring job on LArCells or LArCellsEmpty calibration streams. Uses RELEASE' 
    }


processConfigs['merge'] = {
    'inputs': { 'inputLArFiles': {} },
    'outputs': { 'outputLArFile': {'dstype': 'NTUP_SAMPLESMON'}, }, 
    'tasktransinfo': { 'trfpath': 'LArMerge_trf.py',
                       'trfsetupcmd': trfsetuppath+'setuptrf.sh '+patcharea+' RELEASE '+trfsetuppath+'specialsetup_tier0.sh' },
    'phconfig': {}, 
    'description': 'Merges LAr Samples monitoring ntuples. Uses RELEASE' 
    }


processConfigs['histmerge'] = {
    'inputs': { 'inputHistFiles': {'metatype':'inputLFNlistDA'} },
    'outputs': { 'outputLArHistFile': {'dstype': 'LARHIST'}, }, 
    'tasktransinfo': { 'trfpath': 'LArHistMerge_trf.py',
                       'trfsetupcmd': trfsetuppath+'setuptrf.sh '+patcharea+' RELEASE '+trfsetuppath+'specialsetup_tier0.sh' },
    'phconfig': {}, 
    'description': 'Merges LAr Monitoring Histograms. Uses RELEASE' 
    }






description_var = '. Used in production from Feb 2015 onwards.'

# example release: AtlasTier0-15.5.3.7

if __name__ == '__main__':
    if len(sys.argv)==1:
        print "##############"
        print "Application to create or update AMI tags (for manager only)\n"
        print "Usage:"
        print "UploadAMITag.py <login> <password> create_AMITag <release> <process>"
        print "or"
        print "UploadAMITag.py <login> <password> update_AMITag <release> <process>"
        print "##############"
        sys.exit(0)


    login=sys.argv[1]
    password=sys.argv[2]
    amiTagTmp=sys.argv[3]
    release=sys.argv[4]
    process=sys.argv[5]
    #process='larcaf'

    doWhat="Dunno"
    amiTag=''
    if amiTagTmp.startswith("create_"):
        doWhat="create"
        amiTag=amiTagTmp.replace("create_", "")
    elif amiTagTmp.startswith("update_"):
        doWhat="update"
        amiTag=amiTagTmp.replace("update_", "")

    #prepare input, output and config dictionaries

    #-----------------------------
    # Tier0 specific 'moreInfo'. Hopefully temporary.
    moreInfoDic = {}
    trfsetupcmd = processConfigs[process]['tasktransinfo']['trfsetupcmd'].replace('RELEASE', str(release))
    trfpath = processConfigs[process]['tasktransinfo']['trfpath'] 
    moreInfoDic['tasktransinfo'] = {'trfsetupcmd': trfsetupcmd, 'trfpath': trfpath}

    #---------------------
    #Get pyAMI client
    #try:
    #    import pyAMI.client
    #except ImportError:
    #    print "WARNING unable to import AMI from pyAMI with standard $PYTHONPATH."
    #    print "Will manually add pyAMI, then try again..."
    #    import sys
    #    sys.path.insert(0,'/afs/cern.ch/atlas/software/tools/pyAMI/5.0.0/lib')
    #    import pyAMI.client
    #print "import pyAMI was succesful"
    #amiclient=pyAMI.client.Client('atlas')


    from pyAMI.client import Client
    amiclient=Client('atlas')
    #from pyAMI.client import AMIClient
    #amiclient=Client(False)

    #------------------------
    #Build final AMI tag info
    s={} #stable values
    s['amiTag']=amiTag
    s['tagType']=amiTag[0]
    s['AMIUser']=login
    s['AMIPass']=password

    c={} #changing values
    c['inputs']=str(processConfigs[process]['inputs'].__str__())
    c['outputs']=str(processConfigs[process]['outputs'].__str__())
    c['SWReleaseCache']=str(release)
    if process=='reco' or process=='noisebursts':
        c['Geometry']='ATLAS-R2-2015-02-00-00'
        c['ConditionsTag']='CURRENT'
    else: #merging case
        c['Geometry']='n/a'
        c['ConditionsTag']='n/a'
    c['phconfig']=str(processConfigs[process]['phconfig'].__str__())
    c['transformation']=str(moreInfoDic['tasktransinfo']['trfpath'])
    c['trfsetupcmd']=str(moreInfoDic['tasktransinfo']['trfsetupcmd'])
    c['moreInfo']=str(moreInfoDic.__str__())
    c['description']=processConfigs[process]['description'].replace('RELEASE', str(release)) + description_var 


    #Upload info to AMI
    if doWhat=="create":
        l=['AddAMITag']
        for k in s.keys():
            l.append(k+"=\""+s[k]+"\"")
        for k in c.keys():
            l.append(k+"=\""+c[k]+"\"")
            
        print "l=",l
        result=amiclient.execute(l)
        
        print " ================================================ "

        print result
        print "\n\n###############################################"
        print "#  Successfully created new tag %s !!   :-)  #"%amiTag
        print "###############################################\n\n"        


    elif doWhat=="update":
        l=['UpdateAMITag']
        for k in s.keys():
            l.append(k+'='+s[k])

        s='xx'
        l.append('separator='+s)
        
        nKeys=len(c.keys())
        n=0
        tmp='updateField='
        for k in c.keys():
            tmp+=k
            n+=1
            if n<nKeys: tmp+=s
        l.append(tmp)

        n=0
        tmp='updateValue='
        for k in c.keys():
            tmp+=c[k]
            n+=1
            if n<nKeys: tmp+=s
        l.append(tmp)
        
        #print "l=",l
        result=amiclient.execute(l)
        print result


        print "\n\n#####################################################"
        print "#  Succesfully updated existing tag %s !!!   :-)  #"%amiTag
        print "#####################################################\n\n"        

    else:
        raise SyntaxError("Don't know what to do... amiTagTmp='%s'  doWhat='%s'"%(amiTagTmp,doWhat))
            
    print "To see the corresponding command, do:"
    print "GetCommand.py AMI=%s \n"%amiTag
    print "or go to:"
    print "http://ami.in2p3.fr/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?linkId=501"
    print "\n"
