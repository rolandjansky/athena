# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class RTTdefs:
    success =  0
    error   = -1

    def isSuccess(cls, code):
        if code == 0: return True
        return False
    isSuccess = classmethod(isSuccess)
    

    def isError(cls, code):
        return not RTTdefs.isSuccess(code)
    isError = classmethod(isError)

    def status(cls, code):
        if RTTdefs.isError(code): return 'error'
        return 'success'

    status = classmethod(status)


if __name__ == '__main__':

    print RTTdefs.status(5)
    print RTTdefs.status(0)
    print RTTdefs.status(-1)
    print RTTdefs.isSuccess(0)
   
