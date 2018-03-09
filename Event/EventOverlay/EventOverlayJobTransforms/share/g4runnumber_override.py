
#to get sim run number into output
from Digitization.DigitizationFlags import digitizationFlags

#digitizationFlags.simRunNumber = int(digitizationFlags.getHitFileRunNumber(athenaCommonFlags.PoolHitsInput.get_Value()[0]))

myCommand = 'dumpRunNumber.py EVNT.*'
import commands, re
sc,out = commands.getstatusoutput(myCommand)
if sc != 0:
    print 'ERR: problem:\n%s',str(out)
    raise SystemExit(sc)
myOutput = '0' 
for l in out.splitlines():
    if re.match('^run number: .', l):
        tempout = re.split('^run number: .',l)
        if len(tempout) > 1:
            myOutput = tempout[1].strip()
            del tempout    
    if len(myOutput) > 0 :
        digitizationFlags.simRunNumber = int(myOutput)
        ServiceMgr.EventSelector.RunNumber=digitizationFlags.simRunNumber.get_Value()
        # pileUpEventLoopMgr.mcRunNumber=digitizationFlags.simRunNumber.get_Value()
    else :
        print "Failed to find Run Number in hits file metadata."

print "simRunNumber: "
print ServiceMgr.EventSelector
print pileUpEventLoopMgr
