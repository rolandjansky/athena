import pickle
map={}
map['inputTagFile']  = ['/afs/cern.ch/atlas/offline/test/POOLCollectionTools/TAG.102268._000029.pool.root.1']
map['dbCollection']   = 'atn_t0test_AtlasPOOL_102268_chk_Prov'
map['releaseConfig']  = 'AtlasPOOL,takeFromEnv'
map['dbConnect']      = 'oracle://int8r_nolb/atlas_tags_loading_test'
map['attChk'] = True
map['pid'] = '0'

f = open('tagUploadT0Chk.pickle', 'w')
pickle.dump(map, f)
f.close()

