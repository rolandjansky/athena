# #########################################

# ----------------------------- Catalog file
Service("PoolSvc").SortReplicas = False                                  # NEEDS TO BE CHECKED
PoolSvc = Service( "PoolSvc" )
if isOnline and useEmon:
  PoolSvc.ReadCatalog += ["xmlcatalog_file:/sw/DbData/poolcond/PoolCat_comcond.xml"]
#  PoolSvc.ReadCatalog += ["xmlcatalog_file:/sw/DbSuppForMx/poolcond/PoolCat_comcond.xml"]
#  PoolSvc.ReadCatalog += ["xmlcatalog_file:/det/lar/lar/project/databases/cond08_data.000001.lar.COND/PoolFileCatalog.xml"]
#  PoolSvc.ReadCatalog += ["xmlcatalog_file:/det/lar/lar/project/databases/comcond.000006.lar_conditions.recon.pool.v0000/PoolFileCatalog.xml"]

#ToolSvc.InDetCosmicsEventPhaseTool.GlobalOffset = 0

# ----------------------------- Printout
print "CHECK POINT PRINTING"
globalflags.print_JobProperties()


