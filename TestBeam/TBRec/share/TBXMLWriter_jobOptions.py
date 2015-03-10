#
# jobOptions file XML writer configuration
#
theApp.Dlls += [ "TBRec" ]
#
# -- algorithm
theApp.TopAlg += [ "TBXMLWriter/CaloXMLWriter" ]
    
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#
# algorithm configuration
CaloXMLWriter = Algorithm( "CaloXMLWriter" )
CaloXMLWriter.FileTopDirectory = "/home/lochp/data"
CaloCMLWriter.OutputFrequency  = 1
CaloXMLWriter.WriteTools = [ "TBXMLEventWriterTool/EventWriter",
                             "TBXMLCaloCellWriterTool/CellWriter" ]
#
# tool configurations
CaloXMLWriter.CellWriter.InputCellContainer = "AllCalo"
CaloXMLWriter.CellWriter.IncludedCalos = [ "LAREM",
                                           "LARHEC",
                                           "LARFCAL" ]
CaloXMLWriter.CellWriter.IncludedSamplings = [ "EME1" , "EME2",
                                               "HEC0",  "HEC1",  "HEC2",
                                               "FCAL0", "FCAL1", "FCAL2" ]
#
# the following properties may go away soon!
CaloXMLWriter.CellWriter.EtaMin            = 2.0
CaloXMLWriter.CellWriter.EtaMax            = 5.5
CaloXMLWriter.CellWriter.PhiMin            = 90*deg
CaloXMLWriter.CellWriter.PhiMax            = 180*deg
