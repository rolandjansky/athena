###
# Configure LAr retriever to write out digits 
###

#Add the single LAr digit retriever
AlgoJiveXML.DataTypes += ["JiveXML::LArDigitRetriever/LArDigitRetriever"]
#Remove the individual LAr non-digit retrievers
AlgoJiveXML.DataTypes.remove("JiveXML::CaloLArRetriever/CaloLArRetriever")
AlgoJiveXML.DataTypes.remove("JiveXML::CaloHECRetriever/CaloHECRetriever")
AlgoJiveXML.DataTypes.remove("JiveXML::CaloFCalRetriever/CaloFCalRetriever")
#Enable all digits
ToolSvc.LArDigitRetriever.DoLArDigit=True
ToolSvc.LArDigitRetriever.DoHECDigit=True
ToolSvc.LArDigitRetriever.DoFCalDigit=True
