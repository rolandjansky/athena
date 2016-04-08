#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import pickle

def WriteRecoTrfSignature():

    signatureDic={}
    signatureDic['InputAndOutputFileTypes']={}
    signatureDic['phconfig']=[]


    #A few examples of inputs and their possible outputs... (not all yet)
    signatureDic['InputAndOutputFileTypes']['inputESDFile']=['outputAODFile','outputESDFile','outputNTUP_TRIGFile','outputDESDM_EGAMMAFile']
    signatureDic['InputAndOutputFileTypes']['inputBSFile']=['outputBSFile','outputESDFile','outputCBNTFile','outputNTUP_TRKVALIDFile','outputTAG_COMMFile']
    signatureDic['InputAndOutputFileTypes']['inputAODFile']=['outputTAGFile','outputDAODM_SGLMUFile']


    #A few examples of physics configuration parameters... (not all yet)
    signatureDic['phconfig'].append('autoConfiguration')
    signatureDic['phconfig'].append('preExec')
    signatureDic['phconfig'].append('postExec')
    signatureDic['phconfig'].append('preInclude')
    signatureDic['phconfig'].append('postInclude')
    signatureDic['phconfig'].append('geometryVersion')
    signatureDic['phconfig'].append('conditionsTag')
    signatureDic['phconfig'].append('beamType')
    
    outName="Reco_trf.signature.pickle"
    f = open(outName, 'w')
    pickle.dump(signatureDic, f)
    f.close()

    print "salut Pavel"

    #Towards an automated writing of the pickled dic...
    from PATJobTransforms.Configuration import ConfigDic
    #searching for outputs from ESD input
    for key in ConfigDic.keys():
        obj=ConfigDic[key]
        if hasattr(obj,'isOutput') and hasattr(obj,'subSteps'):
            for s in obj.subSteps:
                if s.startswith('e2'):
                    print key


if __name__ == '__main__':
    WriteRecoTrfSignature()

        
