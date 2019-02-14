# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import MadGraphUtils

def getDictFromCard(card_loc,lowercase=False):
    card=open(card_loc)
    mydict={}
    for line in iter(card):
        if not line.strip().startswith('#'): # line commented out
            command = line.split('!', 1)[0]
            comment = line.split('!', 1)[1] if '!' in line else ''
            if '=' in command:
                setting = command.split('=')[-1].strip()
                value = '='.join(command.split('=')[:-1]).strip()
                if lowercase:
                    value=value.lower()
                    setting=setting.lower()
                mydict[setting]=value
    card.close()
    return mydict

def settingIsTrue(setting):
    if setting.replace("'",'').replace('"','').replace('.','').lower() in ['t','true']:
        return True
    return False

def totallyStripped(x):
    y=str(x).lower().strip()
    # remove leading and trailing "/'
    while len(y)>0 and (y[0]=='"' or y[0]=="'"):
        y=y[1:]
    while len(y)>0 and (y[-1]=='"' or y[-1]=="'"):
        y=y[:-1]
    return y

def checkSetting(key_,value_,mydict_):
    key=totallyStripped(key_)
    value=totallyStripped(value_)
    mydict={}
    for k in mydict_:
        mydict[totallyStripped(k)]=totallyStripped(mydict_[k])
    return key in mydict and mydict[key]==value
    
def checkSettingExists(key_,mydict_):
    key=totallyStripped(key_)
    keys=[]
    for k in mydict_:
        keys+=[totallyStripped(k)]
    return key in keys

def is_version_or_newer(args):
    version=MadGraphUtils.getMadGraphVersion()
    vs=[int(v) for v in version.split('.')]

    # this is lazy, let's hope there wont be a subversion > 100...
    y=int(100**max(len(vs),len(args)))
    testnumber=0
    for x in args:
        testnumber+=x*y
        y/=100

    y=int(100**max(len(vs),len(args)))
    versionnumber=0
    for x in vs:
        versionnumber+=x*y
        y/=100
    return testnumber>=versionnumber

def isNLO_from_run_card(run_card):
    f = open(run_card,'r')
    if "parton_shower" in f.read().lower():
        f.close()
        return True
    else:
        f.close()
        return False
