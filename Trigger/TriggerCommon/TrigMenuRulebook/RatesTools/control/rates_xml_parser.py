# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from model.basics.XMLTag import XMLTag
from model.basics.DataRange import DataRange
from model.basics.DataSelection import DataSelection
from model.COOL.COOLTime import COOLTime

from model.physics.filters.FilterDeadTime import FilterDeadTime
from model.physics.filters.FilterMask import FilterMask 






def apply_filter_from_xml(xml, selection):
    filter = None
    if not xml.GetAttributes().has_key("type"): raise Exception("No filter type specified")
    
    type = xml.GetAttribute("type").lower()
    
    if type == "deadtime":
        filter = FilterDeadTime()
        
        if xml.GetAttributes().has_key("value"): filter.SetThreshold(xml.GetAttribute("value"))
    
    
        
    else:
        raise Exception("Unknown filter type", type)
    
    
    if xml.GetAttributes().has_key("method"):
        method = xml.GetAttribute("method")        
        if not method  in filter.GetSupportedTypes(): raise Exception("Unknown filtering method", method)        
        filter.SetType(method)

    
    print selection
    return filter.GetFilteredValues(selection)



def run_selection_from_xml(xml):

    
    runNumbers = []
    if xml.GetAttributes().has_key("runNumber"):
        runNumbers.append(int(xml.GetAttribute("runNumber")))
        
    elif xml.GetAttributes().has_key("period"):
        #for runNumber in 
        #runNumbers.append(xml.GetAttribute("runNumber")
        print "[-] period-attribute for run tag not implemented yet"
        return DataSelection()        
    else:
        raise Exception("Empty run-tag found")
    
    dataRanges = []
    for runNumber in runNumbers:
        dataRanges.append( DataRange(*COOLTime().Run(runNumber)) )
    
    selection = DataSelection()    
    
    selection.SetListOfRanges(dataRanges)
    
    

    if not xml.GetIsSingle():
        for xmlFilter in xml.GetChildrenNamed("filter"):
            selection = apply_filter_from_xml(xmlFilter, selection)

    return selection
    
     
    
    

def data_selection_from_xml(xml):
    selection = DataSelection()
    for xmlRunSelection in xml.GetChildrenNamed("run"):
        selection = selection.Union(run_selection_from_xml(xmlRunSelection))
    
    for xmlFilter in xml.GetChildrenNamed("filter"):
        selection = apply_filter_from_xml(xmlFilter, selection)
    
    print "asd"
    print selection


def plot_from_xml(xml):
    for xmlDataselection in xml.GetChildrenNamed("dataselection"):
        data_selection_from_xml(xmlDataselection)





if __name__ == "__main__":
    xml = XMLTag().FromFile("test.xml")
    
    for xmlPlot in xml.GetChildrenNamed("plot"):
        plot_from_xml(xmlPlot)
        