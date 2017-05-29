#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
## run it that way:
## python compareHLTXML.py HLTMenu_125.xml HLTMenu_200.xml comp_all
## (or other option for sys.argv[3]


import xml.etree.cElementTree as ET
import sys

class Ddict(dict):
    def __init__(self, default=None):
        self.default = default

    def __getitem__(self, key):
        if not self.has_key(key):
            self[key] = self.default()
        return dict.__getitem__(self, key)

info_xml           = Ddict(dict)
sequence_info      = Ddict(dict)
algorithm_info     = Ddict(dict)
diff_sequence_info = Ddict(dict)
chain_id_info      = Ddict(dict)
chain_input_info   = Ddict(dict)
diff_input_info    = Ddict(dict)


print 
print '*** Comparing the two following menus:', sys.argv[1], sys.argv[2]
print 
number_of_items = 0

good_values = ["comp_chains","comp_streams","comp_conf","comp_chain_ids", "comp_te", "comp_all"]

if len(sys.argv)<4:
    sys.exit('==>Inputs should be three: xml$1 xml$2 comp_option {from %s}'% good_values)

what_to_compare = str(sys.argv[3])
if what_to_compare in good_values:
    print "*** Will %s" % what_to_compare
else:
    sys.exit('==>sys.argv[3] should be one of %s'% good_values)



for i in range(1,3):
    HLT_XML = ET.parse(sys.argv[i])
    HLT_INFO = HLT_XML.getroot()
    for e in HLT_INFO.findall('SEQUENCE_LIST/SEQUENCE'):
        algorithm = e.get('algorithm')
        input     = e.get('input')
        output    = e.get('output')
        string_to_check = input+"==>"+output 
        sequence_info[i][string_to_check]=algorithm
        algorithm_info[i][output] = (algorithm, input, output)
    for e in HLT_INFO.findall('CHAIN_LIST/CHAIN'):
        number_of_items+=1
        name    = e.get('chain_name')
        level   = e.get('level')
        counter = e.get('chain_counter')
        stream = ''
        for ee in e.findall('STREAMTAG_LIST/STREAMTAG'):
            if not stream == '':
                stream += ":"
            stream += ee.get("stream")
        info_xml[i][name]={counter:{level:stream}}
        trig_elements = []
        trig_sequences = []
        for ee in e.findall("SIGNATURE_LIST/SIGNATURE/TRIGGERELEMENT"):
            trig_elements.append(ee.get("te_name"))

        for elem in trig_elements:
            definition = "%s --> " % elem
            while elem in algorithm_info[i]:
                definition += "%s --> " % algorithm_info[i][elem][0]
                elem = algorithm_info[i][elem][1]
            definition += "%s" % elem
            trig_sequences.append(definition)
        chain_input_info[i][name] = sorted(trig_sequences, key = lambda x: x[x.find("-->"):])


def chain_comparisons(info_xml):
    print "====================== Chain Comparisons ======================>"
    for i in range(1,3):
        found_new_chains=False
        print
        print "*** Additional chains in menu", sys.argv[i]
        print "===>"
        for j in info_xml[i].keys():
            if i==1:
                k = 2
            if i==2:
                k = 1
            if not j in info_xml[k].keys():
                print "==> %35s"% j, "\tchainId:",info_xml[i][j].keys()
                found_new_chains=True
    if found_new_chains==False:
        print "==> None"
    print

if what_to_compare == "comp_all" or what_to_compare == "comp_chains":
    chain_comparisons(info_xml)


def stream_comparisons(info_xml):
    print "====================== Stream Comparisons ======================>"
    print
    found_stream_diff = False
    for j in info_xml[1].keys():
        if j in info_xml[2].keys():
            if not info_xml[1][j].values()==info_xml[2][j].values():        
                print "==> Streaming change for: %35s *** from ==> %30s *** to ==> %30s" % (j,  info_xml[1][j].values(), info_xml[2][j].values())
                found_stream_diff = True
    
    if found_stream_diff == False:
        print "==> None"
    print


if what_to_compare == "comp_all" or what_to_compare == "comp_streams":
    stream_comparisons(info_xml)


def configuration_comparisons(sequence_info):
    print "====================== Configuration Comparisons ======================>"
    print "This check is still shown, but is superceded by the 'Chain Trigger Elements' test"
    print
    found_sequence_diff = False
    for i in range(1,3):
        for j in sequence_info[i].keys():
            k = (i%2) + 1
            if j in sequence_info[k].keys():
                if not sequence_info[i][j]==sequence_info[k][j]:        
                    diff_sequence_info[j] = {sequence_info[i][j]:sequence_info[k][j]} 
                    found_sequence_diff = True
            else:
                if i==1:
                    diff_sequence_info[j] = {"Not present":sequence_info[i][j]}
                if i==2:
                    diff_sequence_info[j] = {sequence_info[i][j]:"Not present"}
                found_sequence_diff = True    

    for i in diff_sequence_info.keys():
        print "*** Input ==> Output: %s \n***  from ==> %s \n***    to ==> %s" % ( i, diff_sequence_info[i].values(), diff_sequence_info[i].keys())
        print

    if found_sequence_diff == False:
        print "==> None"
    print

if what_to_compare == "comp_conf":
    configuration_comparisons(sequence_info)

def chain_input_comparisons(chain_input_info):
    print "====================== Chain Trigger Elements  ======================>"
    for i in range(1,3):
        found_input_diff = False
        k = (i%2) + 1
        for j in chain_input_info[i].keys():
            
            if j not in chain_input_info[k]:
                #No need to print this out, the comp_chains test catches these
                #diff_input_info[j] = [ chain_input_info[1].get(j, ["Not present"]) , chain_input_info[2].get(j, ["Not present"])]
                #found_input_diff = True
                continue
                
            if len(chain_input_info[i][j]) != len(chain_input_info[k][j]):
                diff_input_info[j] = [chain_input_info[i][j] , chain_input_info[k][j], "Difference in TE length"]
                found_input_diff = True
                continue

            mismatch = ""
            
            for l in range(len(chain_input_info[i][j])):
                left = chain_input_info[i][j][l]
                right = chain_input_info[k][j][l]

                left = left[left.find("-->"):]
                right = right[right.find("-->"):]
                
                if left != right:
                    #diff_input_info[j] = [chain_input_info[i][j] , chain_input_info[k][j]]
                    #found_input_diff = True
                    mismatch += "%s " % l
                    #continue

            if len(mismatch) > 0:
                diff_input_info[j] = [chain_input_info[i][j] , chain_input_info[k][j], "Differences in TE elements with index: %s" % mismatch]
                found_input_diff = True

    if found_input_diff:
        print "Found a diff in the inputs"
    
    for i in diff_input_info.keys():
        print "*** Chain: %s %s" % (i, diff_input_info[i][2])
        print "***  from ==>"
        for te in diff_input_info[i][0]:
            print "      %s" % te
        print "***  to ==>"
        for te in diff_input_info[i][1]:
            print "      %s" % te
        print

    print
    if len(diff_input_info.keys()) == 0:
        print "==> None"
    print        

if what_to_compare == "comp_all" or what_to_compare == "comp_te":
    chain_input_comparisons(chain_input_info)
    
def chain_id_comparisons(info_xml):
    
    print "====================== Chain **ID** Comparisons ======================>"
    for i in range(1,3):
        found_new_chain_ids=False

        for j in info_xml[i].keys():
            if i==1:
                k = 2
            if i==2:
                k = 1
            if (j in info_xml[k].keys()) and (not info_xml[i][j].keys()==info_xml[k][j].keys()):
                chain_id_info[j]={str(info_xml[i][j].keys()):str(info_xml[k][j].keys())}
                found_new_chain_ids = True
#            print str(info_xml[i][j].keys()),str(info_xml[k][j].keys())

    for i in chain_id_info.keys():
        print "%45s changed: %5s  <==> %5s " % (i, chain_id_info[i].keys(),chain_id_info[i].values() )
        
    print
    if found_new_chain_ids==False:
        print "==> None"
        print

if what_to_compare == "comp_all" or what_to_compare == "comp_chain_ids":
    chain_id_comparisons(info_xml)
