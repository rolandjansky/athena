import pickle
map={}
#map['inputTagFile']  = ['PCT_MC.111.ATN.test.TAG.r1#/afs/cern.ch/atlas/offline/test/POOLCollectionTools/TAG.102268._000025.pool.root.1']
map['inputTagFile']  = ['PCT_MC.111.ATN.test.TAG.r1#/afs/cern.ch/atlas/offline/test/POOLCollectionTools/TAG.00828528._000001.pool.root.1']
map['dbCollection']   = 'atn_t0test_111_MC_2'
map['dbConnect']      = 'oracle://int8r_nolb/atlas_tags_loading_test'

f = open('tagUploadT0MC.pickle', 'w')
pickle.dump(map, f)
f.close()

