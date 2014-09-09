# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__version__ = '1.0.0'
__author__  = 'Yushu Yao (yyao@lbl.gov)'
 
__all__ = [ 'TheUserDataSvc', 'UserDataSvc' ]

from AthenaServicesConf import UserDataSvc as _UserDataSvc

class UserDataSvc(_UserDataSvc):
    __slots__ = [] 
    def __init__(self, name = 'UserDataSvc', **kwds):
        kwds['name'] = name
        super(UserDataSvc, self).__init__(**kwds)
          
    def __setattr__(self, name, value):

        callbase=True;
        
        if name=="OutputStream":
            callbase=False
            
        if name=="OutputFileName":
            print "Obsolete option 'OutputFileName', please use OutputStream=mystream instead!!"
            raise AttributeError('Obsolete property "OutputFileName"')
            
        if callbase:
            return super( UserDataSvc, self ).__setattr__( name, value )

# bwd compat
TheUserDataSvc = UserDataSvc
