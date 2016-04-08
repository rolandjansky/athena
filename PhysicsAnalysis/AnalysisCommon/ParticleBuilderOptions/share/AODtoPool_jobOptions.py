include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/AOD_Builder_jobOptions.py")
include( "ParticleBuilderOptions/AOD_OutputList_jobOptions.py")

# for McTruth
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")

# set output item list
Stream1.ItemList = AOD_ItemList
