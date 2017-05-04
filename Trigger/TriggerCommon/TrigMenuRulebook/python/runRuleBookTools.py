# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


__all__ = ["doPhysicsPrescales", "doCosmicStandbyPrescales", "PrintSummaryXML", "round_figures", "run_command", "linkfiles", "getPrescaleDir",
           "SummaryEntry", "BunchStructure", "XMLFiles"]


import sys, os, math, time

# defining a few data structures
from collections import namedtuple
# for configuration and for storing the uploadresult
BunchStructure = namedtuple("BunchStructure", "FILLED EMPTY EMPTY_AFTER_FILLED UNP_ISO UNP_NONISO")
XMLFiles = namedtuple("XMLFiles", "l1 hlt cost")
SummaryEntry = namedtuple('SummaryEntry', 'l1file hltfile lowlumi highlumi')



def linkfiles(xml):
    import __main__
    rbPath = __main__.rulebookPath
    if rbPath==None:
        os.system('get_files -xmls %s' % xml.cost)
        os.system('get_files -xmls %s' % xml.l1)
        os.system('get_files -xmls %s' % xml.hlt)
        os.system('get_files -scripts cnvXML.py')
    else:    
        os.system("ln -s %s/python TrigMenuRulebook &> /dev/null" % rbPath )
        os.system("ln -s %s/share/%s &> /dev/null" % (rbPath, xml.l1  ) )
        os.system("ln -s %s/share/%s &> /dev/null" % (rbPath, xml.hlt ) )
        os.system("ln -s %s/share/%s &> /dev/null" % (rbPath, xml.cost) )
        os.system("ln -s %s/scripts/cnvXML.py &> /dev/null" % rbPath )


def getPrescaleDir():
    if getattr(getPrescaleDir, "cache", None):
        return getPrescaleDir.cache
    prescale_dir = "prescales_%.f"%time.time()
    try:
        os.mkdir(prescale_dir)
    except OSError:
        prescale_dir = None
        print OSError, "but continue..."
    getPrescaleDir.cache = prescale_dir
    return prescale_dir


def round_figures(value, precision):
    """Returns x rounded to n significant figures."""
    if value == 0: return 0
    abs_value = abs(value)
    round_value =  round(abs_value, int(precision - math.ceil(math.log10(abs(abs_value)))) )
    return math.copysign(round_value, value)


def run_command(command):
    """Convert the command string to arguments for the imported processRules module"""
    import __main__
    rbPath = __main__.rulebookPath

    print sys.path

    import shlex
    command = "processRules " + command
    thisargs = sys.argv
    sys.argv = shlex.split(command)
    if rbPath==None:
        from TrigMenuRulebook.processRules import main
    else:
        from processRules import main
    status = main()
    sys.argv = thisargs
    return status




def doCosmicStandbyPrescales(what, rulebook, bs, xml, prescale_dir = None):

    lumi_ref = 1000

    extra_command = ""

    if what == "Cosmic":
        extra_command = " --target_empty=3000"
    elif what == "Standby":
        extra_command = " --target_filled=%s --target_empty=%s --target_unp_iso=%s --target_unp_noniso=%s" % (bs.FILLED, bs.EMPTY, bs.UNP_ISO, bs.UNP_NONISO)

    print "*** Running "+what
    command = "--rulebook=%s --log=rulebook_%s.log --force-rates-metadata --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s" % (rulebook, what, xml.hlt, xml.l1, lumi_ref)
    if what == "Cosmic":
        command += " --target_empty=3000"
        command += " --rates-xml=%s" % xml.cost
    elif what == "Standby":
        command += " --target_filled=%s --target_empty=%s --target_unp_iso=%s --target_unp_noniso=%s" % (bs.FILLED, bs.EMPTY, bs.UNP_ISO, bs.UNP_NONISO)
        command += " --rates-xml=%s" % xml.cost
    print
    print "*** Will execute:  %s ***" % command
    print
    status = run_command(command)

    print " *** Converting XMLs to uploadable XMLs"
    prescale_name = what
    command_cnv   = "python cnvXML.py --ps_name=%s --ps_xml=prescales%s.xml " % (prescale_name, lumi_ref)

    os.system(command_cnv)

    prescaleSetSummary = [ SummaryEntry(l1file='RuleBook_LV1PS_%s.xml' % prescale_name,
                                         hltfile='RuleBook_HLTPS_%s.xml' % prescale_name,
                                         lowlumi='0', highlumi='0')
                            ]
    PrintSummaryXML(what.upper(), prescaleSetSummary, bs.FILLED)

    if prescale_dir:
        mv_command = "mv RuleBook_*PS_%s* %s" % (prescale_name, prescale_dir)
        os.system(mv_command)




def doPhysicsPrescales(lumi_list, rulebook, bs, xml, userArgs=None):
    prescaleSetSummary = []
    for (lower_lumi_point, lumi_point) in lumi_list:

        print "*** Running:", lumi_point
        logname="log_"+str(lumi_point)
        
        command = "--rulebook=%s --log=rulebook_%s.log --force-rates-metadata --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s " % (rulebook, lumi_point, xml.hlt, xml.l1, lumi_point)
        command += " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % ( bs.FILLED, bs.EMPTY, bs.EMPTY_AFTER_FILLED, bs.UNP_ISO, bs.UNP_NONISO )
        command += " --rates-xml=%s" % xml.cost

        if userArgs:
            command += ' "%s"' % userArgs
        print
        print "*** Will execute:  %s ***" % command
        print
        status = run_command(command)

        if status != 0:
            sys.exit("processRule failed")

        print " *** Converting XMLs to uploadable XMLs"
        short_rulebookname = rulebook.split('.')[1].split('_rules')[0]
        prescale_name = "%s_%04.1f_%04.1fe32"%(short_rulebookname, (float(lower_lumi_point)/100.),float(lumi_point)/100.)
        prescale_name += "_%sb" % bs.FILLED
        if prescale_name.startswith('TrigMenuRulebook.'):
            prescale_name = prescale_name[17:]
        command_cnv   = "python cnvXML.py --ps_name=%s --ps_xml=prescales%s.xml " %(prescale_name, lumi_point)
        status = os.system(command_cnv)
        if status != 0:
            sys.exit("XML conversion failed")

        if getPrescaleDir():
            mv_command = "mv RuleBook_*PS_%s* %s"%(prescale_name,getPrescaleDir())
            status = os.system(mv_command)
            if status != 0:
                sys.exit("Move failed")


        prescaleSetSummary += [ SummaryEntry(l1file='RuleBook_LV1PS_%s.xml' % prescale_name,
                                             hltfile='RuleBook_HLTPS_%s.xml' % prescale_name,
                                             lowlumi='%.1fe32' % (float(lower_lumi_point)/100.),
                                             highlumi='%.1fe32' % (float(lumi_point)/100.) )]


        PrintSummaryXML('PHYSICS', prescaleSetSummary, bs.FILLED)


def doRatePredictions(ebntp):

    if len(sys.argv)>2:
        output_dir = sys.argv[2]
        try:
            os.mkdir(output_dir)
        except OSError:
            print OSError, "but continue..."

    cmd = "runEB.py \
    -i "+ebntp+" \
    -o "+output_dir+" \
    --pred-lumi="+str(lumi_point)+"e30 \
    --joblist rulebook \
    --forwardArgs='-k TRIGRATE --prescaleXML=prescales"+str(lumi_point)+".xml'"


    print 'Execute : %s' % cmd

    status = os.system(cmd)
    if status != 0:
        sys.exit("Rate prediction failed")



def PrintSummaryXML(alias, prescaleSetSummary, filled_bunches):
    try:
        import xml.etree.cElementTree as ET
    except ImportError:
        print "Can't print summary since I can't find package cElementTree"
        return
    def indent(elem, level=0):
        i = "\n" + level*"  "
        if len(elem):
            if not elem.text or not elem.text.strip():
                elem.text = i + "  "
            if not elem.tail or not elem.tail.strip():
                elem.tail = i
            for elem in elem:
                indent(elem, level+1)
            if not elem.tail or not elem.tail.strip():
                elem.tail = i
        else:
            if level and (not elem.tail or not elem.tail.strip()):
                elem.tail = i

    treeroot = ET.ElementTree(ET.Element('ALIAS_DEFINITION', {'alias':"%s" % alias, 'bunches':'%i' % filled_bunches} ))
    for entry in prescaleSetSummary:
        ET.SubElement(treeroot.getroot(),'SET',entry._asdict())
    indent(treeroot.getroot())
    treeroot.write("Set_%s.xml" % alias)
