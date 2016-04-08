import math

etas = [ -2.5 + float( i ) * 5. / 40. for i in range( 40 ) ]
etas.append( 2.5 )
print etas
for index, eta in enumerate( etas ):
  if index + 1 == len( etas ):
    continue
  print '%.3f %.3f %.5f %.5f Region%d' % ( etas[ index ], etas[ index + 1 ], 0, math.pi, index + 1 )
