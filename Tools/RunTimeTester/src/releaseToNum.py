# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def releaseToNum(str):
    """
    Converts the release string x.y.z to a number
    (eg "8.3.0"-> 830)
    """
    num=0
    nightly = False

    if str.find('rel') != -1: # nightly
        nightly = True
        return [nightly, num]
    
    cnumbers=str.split('.')
    cnumbers.reverse()
    powerTen=1
    if len(cnumbers) != 3:
        print 'releaseToNum: bad release: '+str
    for cnum in cnumbers:
        num=num+powerTen*int(cnum)
        powerTen=powerTen*10

    return [nightly, num]

if __name__ == '__main__':
    print releaseToNum('8.3.0')
