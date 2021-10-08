from IOVDbSvc.CondDB import conddb
conddb.blockFolder("/Indet/Align")
conddb.blockFolder("/Indet/AlignITk")
DataBaseConnection = "<dbConnection>sqlite://;schema=MisalignmentSet99.db;dbname=OFLP200</dbConnection>" 
conddb.addFolderWithTag("",DataBaseConnection+"/Indet/AlignITk","InDetSi_MisalignmentMode_99", force=True)
conddb.blockFolder("/Indet/IBLDist")

SCT_GMX_DetectorTool.Alignable=True
pixelTool = svcMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
pixelTool.Alignable = True
