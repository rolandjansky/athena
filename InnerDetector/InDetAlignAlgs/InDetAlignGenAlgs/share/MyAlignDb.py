SCT_GMX_DetectorTool.Alignable = True
###pixelTool.Alignable = 


from IOVDbSvc.CondDB import conddb
conddb.blockFolder("/Indet/AlignITk")
DataBaseConnection = "<dbConnection>sqlite://;schema=MisalignmentSet2_p01.db;dbname=OFLP200</dbConnection>" 
conddb.addFolderWithTag("",DataBaseConnection+"/Indet/AlignITk","InDetSi_MisalignmentMode_", force=True)
conddb.blockFolder("/Indet/IBLDist")
