
from TriggerJobOpts.TriggerFlags import TriggerFlags
if TriggerFlags.readLVL1configFromXML():
    import os
    xmlFile = TriggerFlags.inputLVL1configFile()

    # since there is no path resolver yet
    fullXmlFile = None
    for p in (['./'] + os.environ["XMLPATH"].split(':')):
        if os.path.exists( "%s/%s" % (p, xmlFile) ):
            fullXmlFile = "%s/%s" % (p, xmlFile)
            jsonFile = xmlFile.split("/")[-1].replace(".xml",".json")

    from TrigConfIO.MenuXML2JSONConverter import XML2JsonConverter
    converter = XML2JsonConverter()
    if fullXmlFile:
        converter.convertFile( fullXmlFile, jsonFile )
