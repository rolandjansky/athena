# SG 2 NC Grid 500k+ run numbers

points2sg0 = []
massrange = range(1000,2000,200)+range(2000,4200,400)
for mGluino in sorted(range(400,1000,400)+massrange):
    for mSquark in sorted(range(400,1000,400)+massrange):
        points2sg0 += [[mSquark,mGluino]]
therun = runNumber-500000
if therun>=0 and therun<4*len(points2sg0):
    arun = therun%len(points2sg0)
    gentype = 'ALL'
    log.info('Registered generation of grid twenty-two, squark-gluino with intermediate neutralino/chargino mLSP=0 grid point '+str(therun)+' decoded into mass point '+str(arun)+' with gen '+str(gentype))
    stringy = str(int(points2sg0[arun][0]))+'_'+str(int(points2sg0[arun][1]))+'_0'
    for q in squarks: masses[q] = points2sg0[arun][0]
    masses['1000021'] = points2sg0[arun][1]
    masses['1000022'] = 0
    masses['1000023'] = 106
    masses['1000024'] = 106
    decaytype='SG2NCGrid'
    njets=1
    use_decays=False
    use_Tauola=False
