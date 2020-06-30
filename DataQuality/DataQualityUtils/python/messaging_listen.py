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
        if stomp.__version__ >= (4,1,11):
            return self.__enter41__()
        else:
            return self.__enter31__()

    def __enter31__(self):
        serverlist=[_[4] for _ in socket.getaddrinfo(MSGSERVER, MSGPORT,
                                                     socket.AF_INET, 
                                                     socket.SOCK_STREAM)]
        
        from . import stompconfig
        self.conns = []
        if hasattr(self.listener, 'conn'):
            self.listener.conn=[]
        for svr in serverlist:
            #print 'set up', svr
            cfg = stompconfig.config()
            cfg['heartbeats'] = (0,0)
            cfg['reconnect_attempts_max'] = 3
            cfg['version'] = 1.1
            conn=stomp.Connection([svr], **cfg)
            #print('set up Connection')
            conn.set_listener('somename',self.listener)
            if hasattr(self.listener, 'conn'):
                self.listener.conn.append(conn)
            #print('Set up listener')
            conn.start()

            #print('started connection')

            conn.connect(wait=True)
            #print('connected')
            hdr = {}
            if self.selector is not None: hdr['selector'] = self.selector
            if hasattr(self.listener, 'ack_mode'):
                ack_mode=self.listener.ack_mode
            else:
                ack_mode='auto'
            conn.subscribe(destination=self.dest, ack=ack_mode, headers = hdr, id=len(self.conns))
            #print('subscribed')
            self.conns.append(conn)
        return self

    def __enter41__(self):
        serverlist=[_[4] for _ in socket.getaddrinfo(MSGSERVER, MSGPORT,
                                                     socket.AF_INET, 
                                                     socket.SOCK_STREAM)]
        
        from . import stompconfig
        self.conns = []
        if hasattr(self.listener, 'conn'):
            self.listener.conn=[]
        for svr in serverlist:
            #print 'set up', svr
            cfg = stompconfig.config()
            #cfg['heart-beat'] = (5000,5000)
            cfg['reconnect_attempts_max'] = 3
            cfg['version'] = 1.1
            cfg['login'] = cfg['user']
            conn=stomp.Connection([svr], heartbeats=(180000,180000))
            #print('set up Connection')
            conn.set_listener('somename',self.listener)
            if hasattr(self.listener, 'conn'):
                self.listener.conn.append(conn)
            #print('Set up listener')
            conn.start()

            #print('started connection')

            conn.connect(wait=True, **cfg)
            #print('connected')
            hdr = {}
            if self.selector is not None: hdr['selector'] = self.selector
            if hasattr(self.listener, 'ack_mode'):
                ack_mode=self.listener.ack_mode
            else:
                ack_mode='auto'
            conn.subscribe(destination=self.dest, ack=ack_mode, headers = hdr, id=len(self.conns))
            #print('subscribed')
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
