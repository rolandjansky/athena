# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaCommon.Constants import VERBOSE,INFO
from AthenaCommon.KeyStore import CfgKeyStore


from AthenaCommon.Logging import logging
msg = logging.getLogger( 'KeyStore' )
msg.setLevel( VERBOSE )

msg = logging.getLogger( 'Athena' )
msg.setLevel( INFO )

msg.info( "[KeyStore] test running..." )
ks = CfgKeyStore( "KeyStore" )
print (ks)

msg.info( "[KeyStore] test SUCCESSFULL" )

