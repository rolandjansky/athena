
def transformXML2Json(xmlFile):
    import os

    # since there is no path resolver yet
    fullXmlFile = None
    for p in (['./'] + os.environ["XMLPATH"].split(':')):
        if os.path.exists( "%s/%s" % (p, xmlFile) ):
            fullXmlFile = "%s/%s" % (p, xmlFile)
            jsonFile = xmlFile.replace(".xml",".json")
            jsonFileDir = xmlFile.rsplit("/",1)[0] if "/" in xmlFile else None

    from TrigConfIO.MenuXML2JSONConverter import XML2JsonConverter
    converter = XML2JsonConverter()
    if fullXmlFile:
        if jsonFileDir:
            os.makedirs(jsonFileDir)
        converter.convertFile( fullXmlFile, jsonFile )
