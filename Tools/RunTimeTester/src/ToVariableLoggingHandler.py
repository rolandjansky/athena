# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"A logging handler to get a message from memory to a variable"

import logging.handlers, copy

class ToVariableLoggingHandler(logging.handlers.MemoryHandler):
    def __init__(self):
        logging.handlers.MemoryHandler.__init__(self, 2048)
        self.copyTo=''
    def handle(self, record):
        "copy the message"
        
        logging.handlers.MemoryHandler.handle(self, record)
        self.copyTo = copy.deepcopy(self.format(record))
    def getMsg(self):
        " return the message in demand"
        
        return self.copyTo
