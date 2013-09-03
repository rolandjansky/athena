#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys

# Separate the a line in tokens
def SplitLines (line):
    line = line.strip()
    new_list = line.split('"')

    return new_list

if len(sys.argv) != 4:
	sys.exit("Usage: python trigger_XML_config_parser.py <LVL1 config (xml)> <HLT config (xml)> <output file>")

L1_file_path = sys.argv[1]
HLT_file_path = sys.argv[2]
sigs_file_path = sys.argv[3]

L1_file = open(L1_file_path,'r')
HLT_file = open(HLT_file_path,'r')
sigs_file = open(sigs_file_path,'w')

L1_chains = []
L2_chains = []
EF_chains = []

lines = L1_file.readlines()


curr_chain = []
for line in lines:
    
    n_line = SplitLines(line)

    if n_line[0].startswith('<TriggerItem '):
	found=False
	tmp_index=-1
	curr_index=0
	for field in n_line:
		if (field.find(" name=") != -1) and not found:
			found=True
			tmp_index=curr_index
		if found and (curr_index == tmp_index+1):
        		curr_chain.append(field)
		curr_index = curr_index + 1
			
	if not found:
		sys.exit("[!!] ERROR finding L1 name [!!]")

        curr_chain.append('L1')
	found=False
	tmp_index=-1
	curr_index=0
	for field in n_line:
		if (field.find(" ctpid=") != -1) and not found:
			found=True
			tmp_index=curr_index
		if found and (curr_index == tmp_index+1):
        		curr_chain.append(field)
		curr_index = curr_index + 1
	
	if not found:
		sys.exit("[!!] ERROR finding L1 ctpid [!!]")

        L1_chains.append(curr_chain)
        curr_chain = []

within_prescale = False
prescale = 0
ctpid = ''

for line in lines:
    
    n_line = SplitLines(line)

    if n_line[0].startswith('<Prescale '):
        within_prescale = True
	found = False
	tmp_index = -1
	curr_index = 0
	for field in n_line:
		if (field.find(" ctpid=") != -1) and not found:
			found=True
			tmp_index=curr_index
		if found and (curr_index == tmp_index+1):
        		ctpid = field
		curr_index = curr_index + 1
	
	if not found:
		sys.exit("[!!] ERROR finding L1 prescale ctpid [!!]")

    elif n_line[0].startswith('</Prescale>'):
        within_prescale = False
        for chain in L1_chains:
            if chain[2] == ctpid:
                if prescale > 0:
                    chain.append(prescale)
                    chain.append([])
                    chain.append([])
                else:
                    sys.exit("[!!] ERROR: Prescale of '0' found for ctpid = "+ctpid+" [!!]")
        ctpid = ''
        prescale = 0

    if within_prescale:
        if len(n_line) == 1:
            prescale = float(n_line[0])



lines = HLT_file.readlines()

within_chain = False
within_streams_list = False

curr_chain = []
lower_chains = []
streams = []
for line in lines:
    
    n_line = SplitLines(line)

    if n_line[0].startswith('<CHAIN '):
        within_chain = True

	found=False
	tmp_index=-1
	curr_index=0
	for field in n_line:
		if (field.find(" chain_name=") != -1) and not found:
			found=True
			tmp_index=curr_index
		if found and (curr_index == tmp_index+1):
        		curr_chain.append(field)
		curr_index = curr_index + 1
			
	if not found:
		sys.exit("[!!] ERROR finding HLT chain_name [!!]")

	found=False
	tmp_index=-1
	curr_index=0
	for field in n_line:
		if (field.find(" level=") != -1) and not found:
			found=True
			tmp_index=curr_index
		if found and (curr_index == tmp_index+1):
        		curr_chain.append(field)
		curr_index = curr_index + 1
			
	if not found:
		sys.exit("[!!] ERROR finding HLT level [!!]")

	found=False
	tmp_index=-1
	curr_index=0
	for field in n_line:
		if (field.find(" chain_counter=") != -1) and not found:
			found=True
			tmp_index=curr_index
		if found and (curr_index == tmp_index+1):
        		curr_chain.append(field)
		curr_index = curr_index + 1
			
	if not found:
		sys.exit("[!!] ERROR finding HLT chain_counter [!!]")
	
	found=False
	tmp_index=-1
	curr_index=0
	for field in n_line:
		if (field.find(" prescale=") != -1) and not found:
			found=True
			tmp_index=curr_index
		if found and (curr_index == tmp_index+1):
        		curr_chain.append(float(field))
		curr_index = curr_index + 1
			
	if not found:
		sys.exit("[!!] ERROR finding HLT prescale [!!]")

	found=False
	tmp_index=-1
	curr_index=0
	for field in n_line:
		if (field.find(" lower_chain_name=") != -1) and not found:
			found=True
			tmp_index=curr_index
		if found and (curr_index == tmp_index+1):
        		lower_chains.append([field,float('1')])
		curr_index = curr_index + 1
			
	if not found:
		sys.exit("[!!] ERROR finding HLT lower_chain_name [!!]")

        curr_chain.append(lower_chains) # Lower_chains
        lower_chains = []

    elif n_line[0].startswith('</CHAIN>') :

        within_chain = False

        if curr_chain[1] == 'L1':
            L1_chains.append(curr_chain)
        if curr_chain[1] == 'L2':
            L2_chains.append(curr_chain)
        if curr_chain[1] == 'EF':
            EF_chains.append(curr_chain)

        curr_chain = []

    if within_chain:

        if n_line[0].startswith('<STREAMTAG_LIST>'):
            within_streams_list = True
        elif n_line[0].startswith('</STREAMTAG_LIST>') :
            within_streams_list = False
            curr_chain.append(streams)
            streams = []

    if within_streams_list:

        if n_line[0].startswith('<STREAMTAG '):
	    stream = []
       	    found=False
	    tmp_index=-1
	    curr_index=0
	    for field in n_line:
	    	if (field.find(" stream=") != -1) and not found:
	    		found=True
	    		tmp_index=curr_index
	    	if found and (curr_index == tmp_index+1):
            		stream.append(field)
	    	curr_index = curr_index + 1
	    		
	    if not found:
	    	sys.exit("[!!] ERROR finding stream name [!!]")

       	    found=False
	    tmp_index=-1
	    curr_index=0
	    for field in n_line:
	    	if (field.find(" prescale=") != -1) and not found:
	    		found=True
	    		tmp_index=curr_index
	    	if found and (curr_index == tmp_index+1):
            		stream.append(float(field))
	    	curr_index = curr_index + 1
	    		
	    if not found:
	    	sys.exit("[!!] ERROR finding stream prescale [!!]")
     
       	    found=False
	    tmp_index=-1
	    curr_index=0
	    for field in n_line:
	    	if (field.find(" type=") != -1) and not found:
	    		found=True
	    		tmp_index=curr_index
	    	if found and (curr_index == tmp_index+1):
            		stream.append(field)
	    	curr_index = curr_index + 1
	    		
	    if not found:
	    	sys.exit("[!!] ERROR finding stream type [!!]")

            streams.append(stream)

for chain in L1_chains:
#    print "L1 chain: "+str(chain)
    if len(chain) < 6:
        sys.exit("[!!] ERROR: Chain "+chain[0]+" has too few elements [!!]")

for chain in L2_chains:
#    print "L2 chain: "+str(chain)
    if len(chain) < 6:
        sys.exit("[!!] ERROR: Chain "+chain[0]+" has too few elements [!!]")

for chain in EF_chains:
#    print "EF chain: "+str(chain)
    if len(chain) < 6:
        sys.exit("[!!] ERROR: Chain "+chain[0]+" has too few elements [!!]")

for chain in L2_chains:
    if len(chain[4]) > 0:
        for lower_chain in chain[4]:
            for L1_chain in L1_chains:
                if L1_chain[0] == lower_chain[0]:
                    lower_chain[1] = L1_chain[3]

for chain in L2_chains:
    new_prescale = chain[3]
    for lower_chain in chain[4]:
        new_prescale = new_prescale*lower_chain[1]
    chain[3] = new_prescale

for chain in EF_chains:
    if len(chain[4]) > 0:
        for lower_chain in chain[4]:
            for L2_chain in L2_chains:
                if L2_chain[0] == lower_chain[0]:
                    lower_chain[1] = L2_chain[3]
            for L1_chain in L1_chains:
                if L1_chain[0] == lower_chain[0]:
                    lower_chain[1] = L1_chain[3]

for chain in EF_chains:
    new_prescale = chain[3]
    for lower_chain in chain[4]:
        new_prescale = new_prescale*lower_chain[1]
    chain[3] = new_prescale

streams = []

for chain in EF_chains:
    for stream in chain[5]:
        found_stream = False
        for stored_stream in streams:
            if stored_stream == stream[0]:
                found_stream = True
        if not found_stream:
            streams.append(stream[0])

sigs_file.write("triggerRateTools.PrescaleOverrideList += [\n")
sigs_file.write("#--------------------------------------L1---------------------------------------\n")
for chain in L1_chains:
    sigs_file.write("\t[\""+chain[0]+"\",\""+str(chain[3])+"\"],\n")

sigs_file.write("#--------------------------------------L2---------------------------------------\n")
for chain in L2_chains:
    sigs_file.write("\t[\""+chain[0]+"\",\""+str(chain[3])+"\"],\n")

sigs_file.write("#--------------------------------------EF---------------------------------------\n")
for chain in EF_chains:
    sigs_file.write("\t[\""+chain[0]+"\",\""+str(chain[3])+"\"],\n")

sigs_file.write("]")

#sigs_file.write("#------------------------------------STREAMS------------------------------------\n")
#for stream in streams:
#    sigs_file.write("Stream\t"+stream+"\t")
#    first_chain = True
#    for chain in EF_chains:
#        for chain_stream in chain[5]:
#            if chain_stream[0] == stream:
#                if first_chain:
#                    sigs_file.write(chain[0]+","+str(chain_stream[1]))
#                    first_chain = False
#                else:
#                    sigs_file.write("|"+chain[0]+","+str(chain_stream[1]))
#    if first_chain:
#        sys.exit("[!!] ERROR: no chain found for stream: "+stream+"[!!]")
#    sigs_file.write("\n")
#sigs_file.write("#-------------------------------------CPLX--------------------------------------\n")
#sigs_file.write("#-------------------------------------------------------------------------------\n")

L1_file.close()
HLT_file.close()
sigs_file.close()




