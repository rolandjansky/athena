# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import os, subprocess

from TrigCostBase.TrigCostBaseConfig import getLog

log = getLog('TrigCostExecPython')

#-----------------------------------------------------------------------------
# Read schema version and data version for a given run
#
def readPartitionConfig(run):    
    
    #
    # Set CORAL authentical path
    #
    cap = os.getenv('CORAL_AUTH_PATH')
    os.putenv('CORAL_AUTH_PATH','/afs/cern.ch/atlas/project/tdaq/databases/.coral')

    #
    # Read configuration
    #
    rn_com = ['rn_ls', '-c', 'oracle://atlr/rn_r', '-w', 'atlas_run_number', '-n', str(run), '-m', str(run)]    
    rn_out = subprocess.Popen(rn_com, stdout=subprocess.PIPE).communicate()[0]

    
    if cap == None: os.putenv('CORAL_AUTH_PATH', '')
    else:           os.putenv('CORAL_AUTH_PATH', cap)

    #
    # Parse output rn_ls assuming this format:
    #
    #================================================================================================================================
    #|    Name |    Num | Start At (UTC)       | Duration | User |                  Host | Partition |  Config | Comment            |
    #================================================================================================================================
    #| point-1 | 165231 | 2010-Sep-16 20:26:20 | 11:53:10 | crrc | pc-tdq-onl-27.cern.ch |     ATLAS | 224.156 | Clean stop of run. |
    #================================================================================================================================
    #
    
    rn_lines = rn_out.split('\n')

    for l in rn_lines:
        if not l.count(str(run)):
            continue
        rn_data = l.split('|')

        if len(rn_data) < 9:
            log.warning('Failed to parse output 1 of rn_ls:\n'+rn_out)
            return

        conf_list = rn_data[8].split('.')

        if len(rn_data) < 2:
            log.warning('Failed to parse output 2 of rn_ls:\n'+rn_out)
            return

        res = {}        
        res['schema_version'] = int(conf_list[0])
        res['data_version']   = int(conf_list[1])

        log.info('Run number:     '+str(run))
        log.info('Schema version: '+str(res['schema_version']))
        log.info('Data   version: '+str(res['data_version']))
        
        return res

    log.warning('Failed to read schema and data versions for run: '+str(run))
    return {}

#-----------------------------------------------------------------------------
# Download partition schema and data file
#
def readPartitionData(schema_version, data_version):

    sfile = '/tmp/ATLAS.'+str(schema_version)+'.schema.xml'
    dfile = '/tmp/ATLAS.'+str(schema_version)+'.'+str(data_version)+'.data.xml'
    
    com = 'oks_get_data -c oracle://atlas_oks/r -w atlas_oks_archive -s '+str(schema_version)+' -n '+str(data_version)+' -m '+sfile+' -f '+dfile+' -q Partition \'(all (object-id "ATLAS" =))\' -r 1000'

    cap = os.getenv('CORAL_AUTH_PATH')
    os.putenv('CORAL_AUTH_PATH','/afs/cern.ch/atlas/project/tdaq/databases/.coral')

    p = subprocess.Popen(com, shell = True)
    sts = os.waitpid(p.pid, 0)[1]
    
    if cap == None: os.putenv('CORAL_AUTH_PATH', '')
    else:           os.putenv('CORAL_AUTH_PATH', cap)

    res = {}

    res['schema_file'] = sfile
    res['data_file']   = dfile

    return res

#-----------------------------------------------------------------------------
# Read ROB-ROS map from partition data file
#
def readROSData(data_file, xml_file, useROBIN=False):
    
    """
    Read partition and dump configuration for ros/rob to xml file.
    
    Please note that you should refer to all details of:
    https://svnweb.cern.ch/trac/atlastdaq/browser/DAQ/DataFlow/dccommon/trunk/src/ROSConf.cxx
    
    In order to compute the valid channels correctly (for instance, take into
    account if a ROS or ROBIN was disabled).
    
    ~Werner Wiedenmann
    """

    import sys, eformat
    from pm.project import Project
    from xml.dom.minidom import Document

    xmldoc = Document()
    xmldata = xmldoc.createElement("ros-rob-data")
    xmldoc.appendChild(xmldata)

    log.info('Loading database...')
    db = Project(data_file)
    
    log.info('Retrieving all ROSs...')
    part = db.getObject('Partition', 'ATLAS')

    #
    # Add MET ROS
    #
    lar_ros=AddMETROS(xmldoc,ROSName='ROS-LAR-MET',RobinName='ROBIN-LAR-MET',useROBIN=useROBIN, startROB=8192001,nROBs=67)
    xmldata.appendChild(lar_ros)

    til_ros=AddMETROS(xmldoc,ROSName='ROS-TIL-MET',RobinName='ROBIN-TIL-MET',useROBIN=useROBIN, startROB=8257605,nROBs=8)
    xmldata.appendChild(til_ros)
    
    #
    # Parse partition data and extract ROB-ROS data
    #
    for ros in part.get('ROS'):
        print ros.id, '= [',
        
        xmlros = xmldoc.createElement("ros")
        xmlros.setAttribute("id",ros.id)
        xmldata.appendChild(xmlros)
        
        for robin in ros.Contains:
            rob_ids_print = []
            if useROBIN:
                print
                xmlrobin = xmldoc.createElement("robin")
                xmlrobin.setAttribute("id",robin.id)
                xmlros.appendChild(xmlrobin)
            for rom in robin.Contains:
                sd = eformat.helper.SubDetector.values[ros.Detector.LogicalId] 
                source_id = eformat.helper.SourceIdentifier(sd, rom.Id)
                rob_ids_print.append("0x%08x" % source_id.code())
                xmlrob = xmldoc.createElement("rob")
                xmlrob.setAttribute("id","0x%08x" % source_id.code())
                if not useROBIN:
                    xmlros.appendChild(xmlrob)
                else:
                    xmlrobin.appendChild(xmlrob)
                del xmlrob
                
            print ', '.join(rob_ids_print),
        del xmlros
        print ']'

    # write xml to file
    fp = open(xml_file,"w")
    xmldoc.writexml(fp,indent="  ", addindent="  ", newl="\n", encoding="UTF-8")

#-----------------------------------------------------------------------------
# Add ROS-MET-LAR and ROS-MET-TIL by hand
#
def AddMETROS(xmldoc,ROSName='ROS-LAR-MET',RobinName='ROBIN-LAR-MET',useROBIN=False, startROB=8192001,nROBs=67):

    xmlros = xmldoc.createElement("ros")
    xmlros.setAttribute("id",ROSName)

    if useROBIN:
        print
        xmlrobin = xmldoc.createElement("robin")
        xmlrobin.setAttribute("id",RobinName)
        xmlros.appendChild(xmlrobin)
    i=0
    while i<(nROBs+1):
        xmlrob = xmldoc.createElement("rob")
        xmlrob.setAttribute("id","0x%08x" %(startROB+i))
        if not useROBIN:
            xmlros.appendChild(xmlrob)
        else:
            xmlrobin.appendChild(xmlrob)
        del xmlrob
        i+=1

    return xmlros
            
#-----------------------------------------------------------------------------
# Read partition configuration and data for a given run and write ROB-ROS map
#
def writeROB2ROSXML(run, xml_file, useROBIN=False):

    try:
        confs = readPartitionConfig(run)
        
        files = readPartitionData(confs['schema_version'], confs['data_version'])

        readROSData(files['data_file'], xml_file, useROBIN)
        
        if os.path.isfile(files['data_file']):
            os.remove(files['data_file'])

        if os.path.isfile(files['schema_file']):
            os.remove(files['schema_file'])

    except KeyError:
        log.error('Failed to retrieve partition data for run: ' +str(run))
        

#-----------------------------------------------------------------------------
# Load necessary shared libraries
#
def setPlotDefaults(ROOT, options = None):

    ROOT.gStyle.SetFillColor(10)           
    ROOT.gStyle.SetFrameFillColor(10)      
    ROOT.gStyle.SetCanvasColor(10)         
    ROOT.gStyle.SetPadColor(10)            
    ROOT.gStyle.SetTitleFillColor(0)       
    ROOT.gStyle.SetStatColor(10)   
    
    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetFrameBorderMode(0) 
    ROOT.gStyle.SetPadBorderMode(0)   
    ROOT.gStyle.SetDrawBorder(0)      
    ROOT.gStyle.SetTitleBorderSize(0)
    
    ROOT.gStyle.SetFuncWidth(2)
    ROOT.gStyle.SetHistLineWidth(2)
    ROOT.gStyle.SetFuncColor(2)
    
    ROOT.gStyle.SetPadTopMargin(0.08)
    ROOT.gStyle.SetPadBottomMargin(0.16)
    ROOT.gStyle.SetPadLeftMargin(0.16)
    ROOT.gStyle.SetPadRightMargin(0.12)

    ROOT.gROOT.SetStyle("Plain");
  
    # set axis ticks on top and right
    ROOT.gStyle.SetPadTickX(1)         
    ROOT.gStyle.SetPadTickY(1)         
  
    # Set the background color to white
    ROOT.gStyle.SetFillColor(10)           
    ROOT.gStyle.SetFrameFillColor(10)      
    ROOT.gStyle.SetCanvasColor(10)         
    ROOT.gStyle.SetPadColor(10)            
    ROOT.gStyle.SetTitleFillColor(0)       
    ROOT.gStyle.SetStatColor(10)           
  
  
    # Turn off all borders
    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetFrameBorderMode(0) 
    ROOT.gStyle.SetPadBorderMode(0)   
    ROOT.gStyle.SetDrawBorder(0)      
    ROOT.gStyle.SetTitleBorderSize(0) 
  
    # Set the size of the default canvas
    ROOT.gStyle.SetCanvasDefH(400)          
    ROOT.gStyle.SetCanvasDefW(650)          
    #gStyle->SetCanvasDefX(10)
    #gStyle->SetCanvasDefY(10)   
  
    # Set fonts
    font = 42
    ROOT.gStyle.SetLabelFont(font,"xyz")
    ROOT.gStyle.SetStatFont(font)       
    ROOT.gStyle.SetTitleFont(font)      
    ROOT.gStyle.SetTitleFont(font,"xyz")
    ROOT.gStyle.SetTextFont(font)       
    ROOT.gStyle.SetTitleX(0.3)        
    ROOT.gStyle.SetTitleW(0.4)        
  
   # Set Line Widths
   #gStyle->SetFrameLineWidth(0)
    ROOT.gStyle.SetFuncWidth(2)
    ROOT.gStyle.SetHistLineWidth(2)
    ROOT.gStyle.SetFuncColor(2)
  
   # Set margins
    ROOT.gStyle.SetPadTopMargin(0.08)
    ROOT.gStyle.SetPadBottomMargin(0.16)
    ROOT.gStyle.SetPadLeftMargin(0.16)
    ROOT.gStyle.SetPadRightMargin(0.12)
  
   # Set tick marks and turn off grids
    ROOT.gStyle.SetNdivisions(505,"xyz")
  
   # Set Data/Stat/... and other options
    ROOT.gStyle.SetOptDate(0)
    ROOT.gStyle.SetDateX(0.1)
    ROOT.gStyle.SetDateY(0.1)
   #gStyle->SetOptFile(0)
    ROOT.gStyle.SetOptStat(1110)
    ROOT.gStyle.SetOptFit(111)
    ROOT.gStyle.SetStatFormat("6.3f")
    ROOT.gStyle.SetFitFormat("6.3f")
   #gStyle->SetStatTextColor(1)
   #gStyle->SetStatColor(1)
   #gStyle->SetOptFit(1)
   #gStyle->SetStatH(0.20)
   #gStyle->SetStatStyle(0)
   #gStyle->SetStatW(0.30)
   #gStyle -SetStatLineColor(0)
    ROOT.gStyle.SetStatX(0.919)
    ROOT.gStyle.SetStatY(0.919)
    ROOT.gStyle.SetOptTitle(0)
   #gStyle->SetStatStyle(0000)    # transparent mode of Stats PaveLabel
    ROOT.gStyle.SetStatBorderSize(0)
  
   # Adjust size and placement of axis labels
    ROOT.gStyle.SetLabelSize(0.065,"xyz")
   #gStyle -> SetLabelOffset(0.005,"xyz")
   #gStyle -> SetTitleY(.98)
    ROOT.gStyle.SetTitleOffset(1.0,"xz")
    ROOT.gStyle.SetTitleOffset(1.15,"y")
    ROOT.gStyle.SetTitleSize(0.065, "xyz")
    ROOT.gStyle.SetLabelSize(0.065, "xyz")
    ROOT.gStyle.SetTextAlign(22)
    ROOT.gStyle.SetTextSize(0.12)
  
    # Set paper size for life in the US
    ROOT.gStyle.SetPaperSize(ROOT.TStyle.kA4)
  
   # Set color pallete
    ROOT.gStyle.SetPalette(1)
  
   #gStyle -> SetOptFit(0)
    ROOT.gStyle.SetOptStat("reimo")
   #gStyle -> SetStatStyle(0)	
    ROOT.gStyle.SetHistMinimumZero(True)
   
    ROOT.gROOT.ForceStyle()   
