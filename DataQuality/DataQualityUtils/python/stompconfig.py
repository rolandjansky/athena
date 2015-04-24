# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def config():
   with open('/afs/cern.ch/user/a/atlasdqm/atlas/mqinfo') as f:
       return { 'user': 'atlasdqm',
                'passcode': f.read().strip() }
   raise RuntimeError('Unable to read STOMP connection info')
