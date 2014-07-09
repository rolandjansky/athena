#!/usr/bin/env python2.5

__author__ = "Joerg Stelzer stelzer@cern.ch"

if __name__ == '__main__':

    from TrigConfigSvc.TrigConfigSvcUtils import getKeysFromName

    #configkeys = getKeysFromName('sqlite_file:TriggerMenuSQLiteFile_rel_2.sqlite','%')
    configkeys = getKeysFromName('TRIGGERDB','Cosmic%', MCOnly=True)
    print configkeys


