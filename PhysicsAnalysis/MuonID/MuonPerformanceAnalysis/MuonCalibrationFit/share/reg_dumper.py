import math
etas = [ -2.5, -2.3, -2.0, -1.7, -1.5, -1.25, -1.05, -0.8, -0.4, 0.0, 0.4, 0.8, 1.05, 1.25, 1.5, 1.7, 2.0, 2.3, 2.5 ] 
#print etas
for index, eta in enumerate( etas ):
  if index + 1 == len( etas ):
    continue
  #print '%.2f %.2f 0 1 Region%d' % ( etas[ index ], etas[ index + 1 ], index + 1 )

etas = [ 0.0, 0.4, 0.8, 1.05, 1.25, 1.5, 1.7, 2.0, 2.3, 2.6 ] 
print 'Var:eta[min] Var:eta[max] Var:normphi[min] Var:normphi[max]  Name'
for index, eta in enumerate( etas ):
  if index + 1 == len( etas ):
    continue
  print '%.2f %.2f 0 1 RegPosLarge%d' % ( etas[ index ], etas[ index + 1 ], index + 1 )
  print '%.2f %.2f 1 2 RegPosSmall%d' % ( etas[ index ], etas[ index + 1 ], index + 1 )
  print '%.2f %.2f 0 1 RegNegLarge%d' % ( -etas[ index + 1 ], -etas[ index ], index + 1 )
  print '%.2f %.2f 1 2 RegNegSmall%d' % ( -etas[ index + 1 ], -etas[ index ], index + 1 )
