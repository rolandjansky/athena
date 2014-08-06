import pickle
map={}
map['inputTagFile']  = ['PCT.001.ATN.test.TAG.r1#/afs/cern.ch/atlas/offline/test/POOLCollectionTools/TAG.102268._000025.pool.root.1']
map['inputTagFile']  += ['PCT.001.ATN.test.TAG.r1#/afs/cern.ch/atlas/offline/test/POOLCollectionTools/TAG.102268._000029.pool.root.1']
map['dbCollection']   = 'atn_t0test_102268_Prov_2'
map['releaseConfig']  = 'Atlas,takeFromEnv'
map['dbConnect']      = 'oracle://int8r_nolb/atlas_tags_loading_test'
map['addattr'] = ['test_id', 'uint', '25']
map['pid'] = '0'

f = open('tagUploadT0.pickle', 'w')
pickle.dump(map, f)
f.close()

