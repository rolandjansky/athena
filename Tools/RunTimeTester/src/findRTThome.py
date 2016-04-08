# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import os

def findRTThome():
    userDictPetersMac={
        'cfg': '/Users/peter/RTTdevelopment/RTTChain/cfg/cfg.xml',
        'src': '/Users/peter/TestArea/RunTimeTester-00-02-54_2/src',
        'base': '/Users/peter/TestArea/RunTimeTester-00-02-54_2/'
        }


    userDicts = {
        'petersMac':     userDictPetersMac
        }

    here = 'pcucl02'
    if os.environ.get('HOSTNAME', '').find('.ucl.') != -1: here = 'ucl'
    
    print 'printing HOST',os.environ.get('HOST', '')

    if os.environ.get('HOST', '').find('peter-4.home') != -1: here = 'petersMac' 

    user = os.environ.get('USER', '')
    host = os.environ.get('HOSTNAME', '')

    print here, user
    userDict = userDicts[here]
    cfg  = userDict['cfg']
    src  = userDict['src']
    base = userDict['base']
    
    return base, cfg, src

    
if __name__ == '__main__':
    (base, cfg, src) = findRTThome()
    print base
    print cfg
    print src
