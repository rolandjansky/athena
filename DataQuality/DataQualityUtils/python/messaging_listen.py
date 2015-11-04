# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import stomp
import random
import time
import socket

MSGSERVER='atlas-mb.cern.ch'
MSGPORT=61013

class ATLASDQMListener(object):
    def __init__(self, listener, dest='/topic/atlas.dqm.progress', 
                  selector=None):
        self.listener = listener
        self.dest = dest
        self.selector = selector

    def __enter__(self):
        serverlist=[_[4] for _ in socket.getaddrinfo(MSGSERVER, MSGPORT,
                                                     socket.AF_INET, 
                                                     socket.SOCK_STREAM)]
        
        import stompconfig
        self.conns = []
        for svr in serverlist:
            #print 'set up', svr
            conn=stomp.Connection([svr], **stompconfig.config())
            #print('set up Connection')
            conn.set_listener('somename',self.listener)
            #print('Set up listener')
            conn.start()

            #print('started connection')

            conn.connect(wait=True)
            #print('connected')
            hdr = {}
            if self.selector is not None: hdr['selector'] = self.selector
            conn.subscribe(destination=self.dest, ack='auto', headers = hdr)
            #print('subscribed')
            self.conns.append(conn)
        return self

    def __exit__(self, eType, eValue, eTrace):
        for conn in self.conns:
            try:
                conn.disconnect()
            except Exception, e:
                print('Exception closing connections:' + `e`)
                pass
        return False
