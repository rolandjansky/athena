# Onestep SS / GG through either a chargino or a neutralino

points2b = []
toremove = [210,240,270,300,330,410,490,570,650,730,810,890,970,1050,1130,1197.5]
for sum in ([0,50,100,150,200]+sumList):
    diffI = xrange(1,int(sum/75)+1)
    diffL = [25,50]
    for item in diffI: diffL += [ item*80 ]
    for diff in diffL:
        msq = (sum+diff)/2.
        if msq>1200 or msq-diff<0 or msq<200: continue
        if msq in toremove: continue
        point = [msq,msq-diff]
        points2b += [ point ]
therun = runNumber-400000
if therun>=0 and therun<len(points2b):
    log.info('Registered generation of grid eight, one-step SS point '+str(therun))
    for q in squarks: masses[q] = points2b[therun][0]
    masses['1000022'] = points2b[therun][1]
    masses['1000024'] = 0.5*(points2b[therun][0]+points2b[therun][1])
    masses['1000023'] = 0.5*(points2b[therun][0]+points2b[therun][1])
    stringy = str(int(points2b[therun][0]))+'_'+str(int(masses['1000024']))+'_'+str(int(points2b[therun][1]))
    gentype='SS'
    decaytype='onestepCN'
    njets=1
    use_decays=False
    use_Tauola=False
