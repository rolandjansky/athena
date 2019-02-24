# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Constants import VERBOSE,INFO
from AthenaCommon.KeyStore import CfgItemList

from AthenaCommon.Logging import logging
msg = logging.getLogger( 'ItemList' )
msg.setLevel( VERBOSE )

msg = logging.getLogger( 'Athena' )
msg.setLevel( INFO )

msg.info( "[CfgItemList] test running..." )
esdList = CfgItemList( "esd" )

inDetEsdList = CfgItemList( "InDetEsd" )
inDetEsdList.add( "VxContainer#Conversions" )

esdList += inDetEsdList

esdList += CfgItemList( 
    "CaloEsd", 
    items = [ "CaloCellContainer#Boo",
              "CaloClusterContainer#BooBoo" ] 
    )
assert( esdList.CaloEsd() == [ "CaloCellContainer#Boo",
                               "CaloClusterContainer#BooBoo"] )

esdList += CfgItemList(
    "TrigEsd",
    items = [ "JetCollection#HLT_AutoKey*" ],
    allowWildCard = True
    )
assert( esdList.TrigEsd() == [ "JetCollection#HLT_AutoKey*" ] )

msg.info( "Should see a warning about attempt at adding invalid element..." )
foo = CfgItemList(
    "Foo",
    items = [ "Bla#Blah*" ]
    )
assert( foo() == [] )
del foo

## FIXME: AthenaPoolOutputStream is bringing theApp into our namespace...
msg.info( "Creating an AthenaPoolOutputStream..." )
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamESD = AthenaPoolOutputStream( "StreamESD" )
StreamESD.ItemList = esdList()

msg.info( "ESD item list: %r", esdList() )

msg.info( "removing TrigEsd item list" )
del esdList.TrigEsd
StreamESD.ItemList = esdList()
msg.info( "ESD item list: %r",StreamESD.ItemList )

caught = False
try:
    esdList += AthenaPoolOutputStream( "FooFoo" )
except TypeError,err:
    caught = True
    msg.info( err )
    msg.info( "Error has been caught, good" )
assert(caught)

msg.info( "ESD item list: %r", esdList() )

inDetEsdList.add( "AnotherVxContainer#Conversions" )

msg.info( "InDet list: %r",inDetEsdList() )
msg.info( "ESD item list: %r", esdList() )
assert( set(esdList()) == set([
    'CaloCellContainer#Boo',
    'CaloClusterContainer#BooBoo', 'VxContainer#Conversions',
    'AnotherVxContainer#Conversions'])
    )

msg.info( "[CfgItemList] test SUCCESSFULL" )
