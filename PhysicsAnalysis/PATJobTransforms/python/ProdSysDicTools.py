#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Creation: David Cote (CERN) from Joao Firmino da Costa (DESY), April 2010
# Usage:
#   -downloads job transforms configurations from ProdSys database


#---------------------------------------
def CorrectDict(aDic):
    
    for key in aDic.keys():
        if str(aDic[key])=='NONE': del aDic[key]

    if aDic.has_key('preInclude'):
        aDic['preInclude']=aDic['preInclude'].replace(' ',',')

    if aDic.has_key('postInclude'):
        aDic['postInclude']=aDic['postInclude'].replace(' ',',')  

    return aDic

#----------------------------------------
def AddSimjobOptions(aDic):
    if not aDic.has_key('skipEvents'):
        aDic['skipEvents']=0

    if not aDic.has_key('randomSeed'):
        aDic['randomSeed']= 10

    if aDic.has_key('ConditionsTag'):
        aDic['conditionsTag']=aDic['ConditionsTag']
        del aDic['ConditionsTag']

    if aDic.has_key('JobConfig'):
        aDic['preInclude']=','.join(['SimulationJobOptions/%s'%i if i.find('SimulationJobOptions')<0 and i.find('/')<0 else i for i in aDic['JobConfig'].split(",")])
        del aDic['JobConfig']

    if aDic.has_key('PhysicsList'):
        aDic['physicsList']=aDic['PhysicsList']
        del aDic['PhysicsList']
    
    if aDic.has_key('Geometry'):
        aDic['geometryVersion']=aDic['Geometry']
        del aDic['Geometry']
    return

#----------------------------------------
def AddDigijobOptions(aDic):
    if not aDic.has_key('skipEvents'):
        aDic['skipEvents']=0

    if not aDic.has_key('digiSeedOffset1'):
        aDic['digiSeedOffset1']= 1

    if not aDic.has_key('digiSeedOffset2'):
        aDic['digiSeedOffset2']= 2

    if aDic.has_key('DigiRndmSvc'):
        aDic['digiRndmSvc']=aDic['DigiRndmSvc']
        del aDic['DigiRndmSvc']

    if aDic.has_key('ConditionsTag'):
        aDic['conditionsTag']=aDic['ConditionsTag']
        del aDic['ConditionsTag']

    if aDic.has_key('JobConfig'):
        aDic['preInclude']=','.join(['SimulationJobOptions/%s'%i if i.find('SimulationJobOptions')<0 and i.find('/')<0 else i for i in aDic['JobConfig'].split(",")])
        del aDic['JobConfig']

    if aDic.has_key('SamplingFractionDbTag'):
        aDic['samplingFractionDbTag']=aDic['SamplingFractionDbTag']
        del aDic['SamplingFractionDbTag']
        
    ## temp hack to remove pile-up hits
    pileupkeys = {'beamHaloHits': 'beamHaloHitsFile','beamGasHits': 'beamGasHitsFile',
                  'cavernHits': 'cavernHitsFile','minbiasHits': 'NDMinbiasHitsFile',
                  'SDMinbiasHits': 'SDMinbiasHitsFile', 'DDMinbiasHits': 'DDMinbiasHitsFile'}
    from PATJobTransforms.DefaultInputs import DefaultInputs
    for key in pileupkeys.keys():
        if aDic.has_key(key): 
            if DefaultInputs.has_key(pileupkeys[key]):
                aDic[pileupkeys[key]]=DefaultInputs[pileupkeys[key]]
            else:
                print "Removing argument: %s=%s. GetCommand.py does not handle background input files yet"%(key,aDic[key])
            del aDic[key]
    return

#-------------------------------

#def ChangeGenjobOptions(aDic):

#    if aDic.has_key('Input_file_base'):
#        tmp = aDic['Input_file_base']
#        del  aDic['Input_file_base']
#        aDic['inputGeneratorFile'] = tmp

#-------------------------------
def GetPANDAClient(amitag):
    import cx_Oracle
    #cur = cx_Oracle.connect('atlas_grisli_r/panda_c10@atlas_panda').cursor()
    cur = cx_Oracle.connect('atlas_grisli_r/panda_c10@adcr_panda').cursor()
    return cur

#-------------------------------
# Macro provided by Pavel
def ReadablePANDA(s):
    # This a a mess - panda is encoding the tag in some sort of HTML escaped form, so we need to un-escape it here
    return s.replace('%0B',' ').replace('%9B','; ').replace('%8B','"').replace('%3B',';').replace('%2C',',').replace('%2B','+')


#-----------------------------------
def GetInfoFromPANDA(amitag):
    import string
    from PATJobTransforms.DefaultInputs import DefaultInputs
    
    tagi=amitag
    cur=GetPANDAClient(amitag)
    c1  = cur.execute(" select trf,trfv,lparams,vparams,formats from t_trf_config where tag='%s' and cid=%d " %(tagi[:1],int(tagi[1:])) )
    trf,trfv,lp,vp,fmt=cur.fetchone()
    #print " lp ",lp
    #print "vp ",vp
    #print "fmt ", fmt
    #print "trf ",trf
    nbr= trf.count(',')+1 # determine how many job transforms exist
    llp=lp.split(';')
    vvp=vp.split(';')
    trf=trf.split(',')
    
    listDicsPANDA=[]
#    DicSW={}
#    DicSW['Release']=trfv
    
    hasDIGI = 0
    for i in range(0,nbr): # loop that fills for each nbr the dictionary with the physconf information 
        PhysDicPANDA={}
        InputDicPANDA={}        
        OutputDicPANDA={}        
        results={}
        
        results['amiTransform'] = trf[i]
        llp[i]=llp[i].replace(' ','_')
        llp[i]=llp[i].replace(',',' ')
        tempkey = string.split(llp[i]) # have created an array of keys
        items=len(tempkey)
        
        vvp[i]=vvp[i].replace(' ','lilili') 
        vvp[i]=vvp[i].replace(',',' ')
        vvp[i]=vvp[i].replace('lilili',',') 
        
        tempval = string.split(vvp[i]) # have created an array of values
        
        for j in range(0,items):
            PhysDicPANDA[tempkey[j]]= ReadablePANDA(tempval[j])
            PhysDicPANDA[tempkey[j]]=PhysDicPANDA[tempkey[j]].replace(';',',')
            PhysDicPANDA[tempkey[j]]=PhysDicPANDA[tempkey[j]].replace(' ',',')
            
        CorrectDict(PhysDicPANDA)
        input={}        
        
        if trf[i]=='csc_atlasG4_trf.py' or trf[i]=='AtlasG4_trf.py': # G4 sim jobTransform
            # assuming that there is no input file
            AddSimjobOptions(PhysDicPANDA)
            input['inputEvgenFile']=DefaultInputs['inputEvgenFile']
            OutputDicPANDA['outputHitsFile']={'dstype': 'HITS'}
            if PhysDicPANDA.has_key('DBRelease'):
                del PhysDicPANDA['DBRelease']
                #PhysDicPANDA['DBRelease']='/afs/cern.ch/atlas/www/GROUPS/DATABASE/pacman4/DBRelease/DBRelease-'+PhysDicPANDA['DBRelease']+'.tar.gz'

        elif trf[i]=='csc_digi_trf.py' or trf[i]=='Digi_trf.py': # DIGI jobTransform
            # assuming that there is no input file
            AddDigijobOptions(PhysDicPANDA)
            input['inputHitsFile']=DefaultInputs['inputHitsFile']
            OutputDicPANDA['outputRDOFile']={'dstype': 'RDO'}
            if PhysDicPANDA.has_key('DBRelease'):
                del PhysDicPANDA['DBRelease']
                #PhysDicPANDA['DBRelease']='/afs/cern.ch/atlas/www/GROUPS/DATABASE/pacman4/DBRelease/DBRelease-'+PhysDicPANDA['DBRelease']+'.tar.gz'
            hasDIGI = 1

        #elif trf[i]=='Evgen_trf.py' : # GEN jobTransform
        #    # assuming that there is no input file
        #    ChangeGenjobOptions(PhysDicPANDA)

        elif trf[i]=='Reco_trf.py':
            if hasDIGI==1:
                input['inputRDOFile']='myRDOFile.pool.root'
            else:
                if PhysDicPANDA.has_key('conditionsTag'):
                    if PhysDicPANDA['conditionsTag'].startswith('OFLCOND'):
                        input['inputRDOFile']=DefaultInputs['inputRDOFile']
                    elif PhysDicPANDA['conditionsTag'].startswith('COM'):
                        input['inputBSFile']=DefaultInputs['inputBSFile']
                else:
                    # there are no conditionsTAG, assuming it's MC
                    input['inputRDOFile']=DefaultInputs['inputRDOFile']

            fmt=fmt.replace('.',' ') 
            tmpfmt = string.split(fmt)
            items = len(tmpfmt)
            for j in range(0,items):
                if not tmpfmt[j].startswith('RDO'): 
                    key='output'+tmpfmt[j]+'File'
                    OutputDicPANDA[key]={'dstype': tmpfmt[j]}

        elif trf[i]=='Merging_trf.py':
            print fmt
            fmt=fmt.replace('.',' ').replace('HITS','Hits')
            tmpfmt = string.split(fmt)
            items = len(tmpfmt)
            print " fmt in merging loop : ",fmt," and items : ",items
            for j in range(0,items):
                inkey='input'+tmpfmt[j]+'File'
                try:
                    input[inkey]=DefaultInputs[inkey]
                    outkey='output'+tmpfmt[i]+'File'
                except KeyError:
                    print 'Warning: no default value for {0}. Probably TAG is incorrectly stored'.format(inkey)
                    sys.exit(1)
                OutputDicPANDA[outkey]={'dstype': tmpfmt[j]}
                
        elif trf[i]=='DigiMReco_trf.py':
            input['inputHitsFile']=DefaultInputs['inputHitsFile']
            fmt=fmt.replace('.',' ') 
            tmpfmt = string.split(fmt)
            items = len(tmpfmt)
            for j in range(0,items):
                if not tmpfmt[j].startswith('RDO'): 
                    key='output'+tmpfmt[j]+'File'
                    OutputDicPANDA[key]={'dstype': tmpfmt[j]}
            if not ('outputRDOFile' in OutputDicPANDA or 'tmpRDO' in OutputDicPANDA):
                OutputDicPANDA['tmpRDO'] = {'dstype': 'RDO.tmp'}

        else:
            raise RuntimeError("No support yet for job transform '%s' returned by ProdSys tag %s"%(trf[i],amitag))


        results['amiInputDic']=input
        CorrectDict(OutputDicPANDA)   
        results['amiOuputDic']=OutputDicPANDA
        results['amiPhysDic']=PhysDicPANDA            
#        results['amiRelease']=DicSW
        listDicsPANDA.append(results)

    return listDicsPANDA


