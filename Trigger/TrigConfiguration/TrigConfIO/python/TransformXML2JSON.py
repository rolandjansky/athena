# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def transformXML2Json(xmlFile):
    '''
    transforms a Run-2 style configuration xml file into a json file
    '''

    # protection in case we start to provide json
    if xmlFile.endswith(".json"):
        return xmlFile

    import os

    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'XML2Json' )

    msg.info("Attempting to convert %s", xmlFile)

    # since there is no path resolver yet
    fullXmlFile = None
    for p in (['./'] + os.environ["XMLPATH"].split(':')):
        if os.path.exists( "%s/%s" % (p, xmlFile) ):
            fullXmlFile = "%s/%s" % (p, xmlFile)
            break
        if os.path.exists( "%s/TriggerMenuXML/%s" % (p, xmlFile) ):
            fullXmlFile = "%s/TriggerMenuXML/%s" % (p, xmlFile)
            break
        if os.path.exists( "%s/TriggerMenuMT/%s" % (p, xmlFile) ):
            fullXmlFile = "%s/TriggerMenuMT/%s" % (p, xmlFile)
            break

    if not fullXmlFile:
        msg.error("Did not find file %s", xmlFile)
        return ""

    localJsonFile = "./" + xmlFile.rsplit("/",1)[-1].replace(".xml",".json")
    msg.info("Converting %s to %s", fullXmlFile, localJsonFile)

    from TrigConfIO.MenuXML2JSONConverter import XML2JsonConverter
    converter = XML2JsonConverter()
    converter.convertFile( fullXmlFile, localJsonFile )

    return localJsonFile
