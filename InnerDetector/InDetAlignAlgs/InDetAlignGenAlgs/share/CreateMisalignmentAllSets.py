import os,glob

if os.path.exists('./PoolFileCatalog.xml'): os.remove('./PoolFileCatalog.xml')

#for MisalignmentMode in [11,12,13,21,22,23,31,32,33]:
for MisalignmentMode in [12,21,23,31]:
#for MisalignmentMode in [0]:
    
#for MisalignmentMode in [23]: #twist
#for MisalignmentMode in [12]: #ellipse
# for MisalignmentMode in [21]: #curl
#for MisalignmentMode in [0,31]: #telescope
    print 'creating set',MisalignmentMode
    
    if os.path.exists('./InDetRecESD.root'): os.remove('./InDetRecESD.root')
    
    leftOverFiles = glob.glob('MisalignmentSet%s*' % MisalignmentMode)
    for file in leftOverFiles:
        os.remove(file)
        
    os.system("athena.py -c 'MisalignmentMode=%s' CreateMisalignment.py 2>&1 | bzip2 > MisalignmentSet%s.log.bz2" % (MisalignmentMode,MisalignmentMode) )
    
