# add for memory tracking
import Hephaestus.MemoryTracker as memtrack
# Try to write the leaks into separate file 
try: 
  memtrack.depth( 22 )            # sets the total search depth
  memtrack.MemoryTrace.size( 19 ) # size of traces kept in memory
  memtrack.outstream( open("LeakCheck.txt","w") )
except: 
  print "ERROR setting memory depth"

