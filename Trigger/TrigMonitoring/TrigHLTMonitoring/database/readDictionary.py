# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include("dictionary.py")

#def readDictionary(runtype, *vargs, **kwargs):
def readDictionary(*vargs, **kwargs):
    import sys

    ConfigName=[]
    ConfigRecostep=[]
    MontoolConfigCode=[]
    MontoolConfigScript=[]

    for (name, recostep),data in d.items():
       if data.has_key('Config'):                                                                                                                   
         ConfigName.append(name)
         ConfigRecostep.append(recostep)
         MontoolConfigCode.append(data['Config'])
    print MontoolConfigCode

    return ConfigName,ConfigRecostep,MontoolConfigCode
