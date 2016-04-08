# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class NullXMLConverter:
    def __init__(self, minder):
        self.logger = minder.logger

    def update(self):
        self.logger.debug("NullXMLConverter::update")

    def setSummaryCallBack(self, callBackFn):
        self.callBack = callBackFn
