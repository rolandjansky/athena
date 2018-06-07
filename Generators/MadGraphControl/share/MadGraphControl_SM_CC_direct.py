# Chargino mass plane, grid 3

points2c = []
for sum in (sumList):
    diffI = xrange(1,int(sum/75)+1)
    diffL = [25,50]
    for item in diffI: diffL += [ item*80 ]
    for diff in diffL:
        msq = (sum+diff)/2.
        if msq>800 or msq-diff<0 or msq<200: continue
        point = [msq,msq-diff]
        points2c += [ point ]
therun = runNumber-300000
if therun>=0 and therun<len(points2c):
    log.info('Registered generation of grid three, direct CC point '+str(therun))
    masses['1000024'] = points2c[therun][0]
    masses['1000022'] = points2c[therun][1]
    stringy = str(int(points2c[therun][0]))+'_'+str(int(points2c[therun][1]))
    gentype='CC'
    decaytype='direct'
    njets=1
    use_decays=False
