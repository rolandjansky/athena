# hold common post option configuration for RTT test
#print memory usage for all events (in case of memory leak crash)
try:
    CBNTAA_Audit().nEventPrintMem=999999
except Exception:
    print "no CBNTAA_Audit available"
