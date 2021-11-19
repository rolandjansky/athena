# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import stomp
import socket
import logging

MSGSERVER='atlas-mb.cern.ch'
MSGPORT=61013

logging.basicConfig()
#logging.getLogger('stomp.py').setLevel(logging.DEBUG)

class ATLASDQMListener(object):
    def __init__(self, listener, dest='/topic/atlas.dqm.progress', 
                  selector=None):
        self.listener = listener
        self.dest = dest
        self.selector = selector

    def __enter__(self):
        if stomp.__version__ >= (6,1,0):
            return self.__enter61__()
        else:
            logging.critical("Unable to find stomp.py >= 6.1.0, can't proceed")
            raise ValueError("Version of stomp.py is too old")

    def __enter61__(self):
        serverlist=[_[4] for _ in socket.getaddrinfo(MSGSERVER, MSGPORT,
                                                     socket.AF_INET, 
                                                     socket.SOCK_STREAM)]
        
        from . import stompconfig
        self.conns = []
        if hasattr(self.listener, 'conn'):
            self.listener.conn=[]
        for svr in serverlist:
            auth = stompconfig.config()
            cfg = {}
            cfg['heartbeats'] = (0,0)
            cfg['reconnect_attempts_max'] = 3
            conn=stomp.Connection([svr], **cfg)
            conn.set_listener('somename',self.listener)
            if hasattr(self.listener, 'conn'):
                self.listener.conn.append(conn)
            conn.connect(wait=True, **auth)
            hdr = {}
            if self.selector is not None: hdr['selector'] = self.selector
            if hasattr(self.listener, 'ack_mode'):
                ack_mode=self.listener.ack_mode
            else:
                ack_mode='auto'
            conn.subscribe(destination=self.dest, ack=ack_mode, headers = hdr, id=len(self.conns))
            self.conns.append(conn)
        return self

    def __exit__(self, eType, eValue, eTrace):
        for conn in self.conns:
            try:
                conn.disconnect()
            except Exception as e:
                print('Exception closing connections:' + repr(e))
                pass
        return False
