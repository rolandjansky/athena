# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time
def getTimes():
    formatedTime = time.strftime('%y/%m/%d@%H:%M', time.localtime())
    return (formatedTime, time.time())
