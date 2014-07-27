# python script to convert TrigIDSCAN configuration into old-style job option
#  fragment files to be read in standalone IDScan.
# One needs to setup athena for this to work, but one does not need to run the
#  script inside athena.
# V. E. Ozcan, University College London
# M. Sutton, University of Sheffield

import sys

if len(sys.argv)<2 or len(sys.argv)>3:
  print "Usage : python",sys.argv[0],"<idscanInstanceName> [<idscanAlgoName>]"
  print "        The default value of <idscanAlgoName> is <idscanInstanceName>."
  print "   Ex : python",sys.argv[0],"TrigIDSCAN_FullScan IdScan_FullScan"
  sys.exit()

sys.path.append("../python")

idscanInstanceName = sys.argv[1]
if len(sys.argv)==2:
  idscanAlgoName = idscanInstanceName
else:
  idscanAlgoName = sys.argv[2]

exec( "from TrigIDSCAN.TrigIDSCAN_Config import " + idscanInstanceName )
exec( "idscan = " + idscanInstanceName + "()" )

outfile = open("jobOfragment_"+idscanInstanceName+".py", 'w')
sys.stdout = outfile

from datetime import datetime
print "# Standalone config file generated on " + datetime.now().isoformat(' ')
print

print "# TrigIDSCAN. parameters"
for key,type in sorted(idscan.__class__._properties.iteritems()):
  if ( str(type).find("GaudiHandle") == -1 ) :
    try:
      value = idscan.__getattribute__(str(key))
      if ( value.__class__ != str ):
        print idscanAlgoName + "." + key + " = " + value, ";"
      else:
        print idscanAlgoName + "." + key + " = " + '"' + value + '" ;'
    except: pass

for key,type in sorted(idscan.__class__._properties.iteritems()):
  if ( str(type).find("GaudiHandle") > -1 ) :
    try:
      tool = idscan.__getattribute__(str(key))
      props = tool._properties
      valcount = 0
      for tkey,ttype in sorted(props.iteritems()):
        try:
          value = tool.__getattribute__(tkey)
          if ( value.__class__ != str ):   # not interested in string properties of tools
            if valcount == 0:
              print "\n# " + key + ". parameters"
              valcount += 1
            print idscanAlgoName+"." +key+"."+tkey, " = ", value, ";"
        except: pass
    except: pass

sys.stdout = sys.__stdout__
print "jobOfragment created - happy standalone idscanning"
