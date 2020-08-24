# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

DEST='/queue/atlas.dqm.panic'

def panic(msg):
    import stomp
    import json
    from . import stompconfig
    import sys, os
    import traceback
    import time

    conn=stomp.Connection([('atlas-mb.cern.ch', 61013)], **stompconfig.config())
    conn.start()
    print('panic: started connection')
    
    conn.connect(wait=True)
    print('panic: connected')

    header={'MsgClass':'DQ', 
            'MsgType': 'Panic',
            'type':'textMessage', 
            'persistent': 'true',
            'destination': DEST,
            'ack': 'auto',
            }
    body=json.dumps({'msg': msg,
                     'command': sys.argv[0],
                     'arguments': sys.argv[1:],
                     'traceback': traceback.format_stack()[:-1],
                     'cwd': os.getcwd(),
                     'time': time.time(),
                     'usrtime': time.strftime('%Y-%m-%d %H:%H:%M %Z', time.localtime()),
                     })
    #print(msg)
    conn.send(body, **header)
    print('panic: sent message')
    print(header)
    print(body)
    conn.disconnect()

if __name__ == '__main__':
    panic('Testing!')
