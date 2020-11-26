#!/usr/bin/env python
import copy, re, os, sys

wdir = sys.argv[0].replace('generateExamples.py', '')
if not os.path.isabs(wdir): wdir = os.getcwd() + '/' + wdir

def import_skeleton(discarded_blocks = []):
    filename = 'skeleton.cxx'
    discard = False
    first_discarded_line = 0
    blocks = []
    buffer = ''
    with open(wdir + filename,'r') as f:
        linenumber = 0
        for line in f:
            linenumber += 1
            if line.startswith('['):
                opening = (line[1] != '/')
                if ']' not in line:
                    print "Parsing error on %s line %d: character ']' not found"%(filename,linenumber)
                    exit(-1)
                token = line.split(']')[0][(1 if opening else 2):]
                if opening:
                    if token in blocks:
                        print "Parsing error on %s line %d: request for opening block [%s] which is already open"%(filename,linenumber,token)
                        exit(-1)
                    blocks.append(token)
                else:
                    if len(blocks)==0 or blocks[-1] != token:
                        print "Parsing error on %s line %d: request for closing block [%s] which either isn't open or isn't the last one opened"%(filename,linenumber,token)
                        exit(-1)
                    blocks.pop()
                will_discard = any(b in discarded_blocks for b in blocks)
                if discard and not will_discard:
                    print '   discarded lines %d - %d from block [%s]'%(first_discarded_line,linenumber,token)
                elif not discard and will_discard:
                    first_discarded_line = linenumber
                discard = will_discard
                continue
            if discard: continue
            if 'DATA2015' in blocks: comma = ',' if ('DATA2016' not in discarded_blocks or 'DATA2017' not in discarded_blocks) else ''
            elif 'DATA2016' in blocks: comma = ',' if ('DATA2017' not in discarded_blocks) else ''
            elif 'DATA2017' in blocks: comma = ''
            else: comma = None
            if comma is not None: line = line.replace(' MAYBE_COMMA', comma)
            buffer += line
    return buffer

def import_stuffing(src, tag, comment_out = False):
	lines = None
	with open(wdir + src, 'r') as f:
		for line in f:
			if line.startswith('[example%s]'%(tag)): lines = ['/*\n'] if comment_out else []
			elif lines is not None:
				if line.startswith('[example'): break
				if line.startswith('+[example') and not comment_out: lines.append(import_stuffing(src, line[9:].split(']')[0], False))
				else: lines.append((' * ' if comment_out else '') + line)
	if lines is None or len(lines)==0:
		print 'ERROR: unable to import example' + tag + ' when parsing ' + src
		exit(-1)
	if comment_out: lines.append(' */')
	elif lines[-1].isspace(): del lines[-1]
	return ''.join(lines)
	
def import_comments(tag):
	return import_stuffing('comments.txt', tag, True)
	
def import_trigglob_config(tag):
	return import_stuffing('trigglob_config.cxx', tag, False)

def import_cptools_config(tag):
	return import_stuffing('cptools_config.cxx', tag, False)

def import_eventloop(tag):
	return import_stuffing('eventloop.cxx', tag, False)
	
def safe_format(txt, replacements):
    newtxt = txt.replace('{','{{').replace('}','}}')
    for token in replacements:
        newtxt = newtxt.replace('{{'+token+'}}','{'+token+'}')
    newtxt = newtxt.format(**replacements).replace('\t','    ')
    newtxt = re.sub(r'\n\s*?DELETE\-THIS\-LINE\s*?\n','\n',newtxt)
    if newtxt.count('DELETE-THIS-LINE') != 0:
        print 'ERROR: not all DELETE-THIS-LINE symbols were replaced'
    return newtxt
    
replacements = {
    'head_comments': 'DELETE-THIS-LINE', 
    'example_number': 'DELETE-THIS-LINE',
    'electron_extraproperties_declare': 'DELETE-THIS-LINE', 
    'electron_toolconfigs': 'DELETE-THIS-LINE', 
    'electron_tools_properties_set': 'DELETE-THIS-LINE',
    'electron_extraproperties_fill': 'DELETE-THIS-LINE',
    'muon_extraproperties_fill': 'DELETE-THIS-LINE',
    'trigglob_properties_set': 'DELETE-THIS-LINE',
    'eventloop_trigcounters': 'DELETE-THIS-LINE',
    'eventloop_electron_selection': 'DELETE-THIS-LINE',
    'eventloop_muon_selection': 'DELETE-THIS-LINE',
    'trigger_matching_requirements': 'DELETE-THIS-LINE',
}

def write_example(subs,tokens=[]):
    path = wdir + '/../'
    txt = import_skeleton(tokens) # 'ELECTRONS', 'MUONS', 'MULTITRIGGERS', '2016_RUNS'
    txt = safe_format(txt,r)
    filename = 'TrigGlobEffCorrExample%s.cxx'%(subs['example_number'])
    dest = path + filename
    writeCXX = True
    if os.path.exists(dest):
        with open(dest,'r') as f:
            oldtxt = f.read()
            if txt==oldtxt:
                print "INFO: %s didn't change, it won't be overwritten"%(filename)
                writeCXX = False
    if writeCXX:
        with open(dest,'w') as f:
            f.write(txt)

r = copy.deepcopy(replacements)
r['head_comments'] = import_comments('0')
r['example_number'] = '0'
r['electron_tools_properties_set'] = import_cptools_config('0:electron_properties_set')
r['trigglob_properties_set'] = import_trigglob_config('0')
r['eventloop_electron_selection'] = import_eventloop('0:electron_selection')
r['eventloop_muon_selection'] = import_eventloop('0:muon_selection')
r['trigger_matching_requirements'] = import_eventloop('0:trigger_matching')
write_example(r, ['MULTITRIGGERS','SPLITFUNC'])


r = copy.deepcopy(replacements)
r['head_comments'] = import_comments('1')
r['example_number'] = '1'
r['electron_extraproperties_declare'] = import_cptools_config('1and2:electron_extraproperties_declare')
r['electron_toolconfigs'] = import_cptools_config('1:electron_toolconfigs')
r['electron_tools_properties_set'] = import_cptools_config('1and2:electron_properties_set')
r['electron_extraproperties_fill'] = import_cptools_config('1and2:electron_extraproperties_fill')
r['trigglob_properties_set'] = import_trigglob_config('1')
r['eventloop_trigcounters'] = import_eventloop('1and2:trigcounters')
r['eventloop_electron_selection'] = import_eventloop('1and2:electron_selection')
r['eventloop_muon_selection'] = import_eventloop('1:muon_selection')
r['trigger_matching_requirements'] = import_eventloop('1:trigger_matching')
write_example(r,['SPLITFUNC'])


r = copy.deepcopy(replacements)
r['head_comments'] = import_comments('2')
r['example_number'] = '2'
r['electron_extraproperties_declare'] = import_cptools_config('1and2:electron_extraproperties_declare')
r['electron_toolconfigs'] = import_cptools_config('2:electron_toolconfigs')
r['electron_tools_properties_set'] = import_cptools_config('1and2:electron_properties_set')
r['electron_extraproperties_fill'] = import_cptools_config('1and2:electron_extraproperties_fill')
r['trigglob_properties_set'] = import_trigglob_config('2')
r['eventloop_trigcounters'] = import_eventloop('1and2:trigcounters')
r['eventloop_electron_selection'] = import_eventloop('1and2:electron_selection')
r['eventloop_muon_selection'] = import_eventloop('2:muon_selection')
r['trigger_matching_requirements'] = import_eventloop('2:trigger_matching')
write_example(r,['SPLITFUNC'])

r = copy.deepcopy(replacements)
r['head_comments'] = import_comments('3com') + '\n' + import_comments('3a')
r['example_number'] = '3a'
r['electron_extraproperties_declare'] = import_cptools_config('3a3b:electron_extraproperties_declare')
r['electron_toolconfigs'] = import_cptools_config('3a:electron_toolconfigs')
r['electron_tools_properties_set'] = import_cptools_config('3:electron_properties_set')
r['electron_extraproperties_fill'] = import_cptools_config('3:electron_extraproperties_fill')
r['trigglob_properties_set'] = import_trigglob_config('3a')
r['eventloop_electron_selection'] = import_eventloop('3a3b3c:electron_selection')
r['trigger_matching_requirements'] = import_eventloop('3a:trigger_matching')
write_example(r,['MUONS','DATA2016','DATA2017'])

r = copy.deepcopy(replacements)
r['head_comments'] = import_comments('3com') + '\n' + import_comments('3b')
r['example_number'] = '3b'
r['electron_extraproperties_declare'] = import_cptools_config('3c:electron_extraproperties_declare')
r['electron_toolconfigs'] = import_cptools_config('3b:electron_toolconfigs')
r['electron_tools_properties_set'] = import_cptools_config('3:electron_properties_set')
r['electron_extraproperties_fill'] = import_cptools_config('3:electron_extraproperties_fill')
r['trigglob_properties_set'] = import_trigglob_config('3b3c')
r['eventloop_electron_selection'] = import_eventloop('3a3b3c:electron_selection')
r['trigger_matching_requirements'] = import_eventloop('3b:trigger_matching')
write_example(r,['MUONS','DATA2016','DATA2017'])

r = copy.deepcopy(replacements)
r['head_comments'] = import_comments('3com') + '\n' + import_comments('3c')
r['example_number'] = '3c'
r['electron_extraproperties_declare'] = import_cptools_config('3c:electron_extraproperties_declare')
r['electron_toolconfigs'] = import_cptools_config('3c:electron_toolconfigs')
r['electron_tools_properties_set'] = import_cptools_config('3:electron_properties_set')
r['electron_extraproperties_fill'] = import_cptools_config('3:electron_extraproperties_fill')
r['trigglob_properties_set'] = import_trigglob_config('3b3c')
r['eventloop_electron_selection'] = import_eventloop('3a3b3c:electron_selection')
r['trigger_matching_requirements'] = import_eventloop('3c:trigger_matching')
write_example(r,['MUONS','DATA2016','DATA2017'])

r = copy.deepcopy(replacements)
r['head_comments'] = import_comments('3com') + '\n' + import_comments('3d')
r['example_number'] = '3d'
r['electron_extraproperties_declare'] = import_cptools_config('3d:electron_extraproperties_declare')
r['electron_toolconfigs'] = import_cptools_config('3d:electron_toolconfigs')
r['electron_tools_properties_set'] = import_cptools_config('3:electron_properties_set')
r['electron_extraproperties_fill'] = import_cptools_config('3:electron_extraproperties_fill')
r['trigglob_properties_set'] = import_trigglob_config('3d')
r['eventloop_trigcounters'] = import_eventloop('3d3e:trigcounters')
r['eventloop_electron_selection'] = import_eventloop('3d3e:electron_selection')
r['eventloop_muon_selection'] = import_eventloop('3d3e:muon_selection')
r['trigger_matching_requirements'] = import_eventloop('3d:trigger_matching')
write_example(r,['DATA2015','DATA2017'])

r = copy.deepcopy(replacements)
r['head_comments'] = import_comments('3com') + '\n' + import_comments('3e')
r['example_number'] = '3e'
r['electron_extraproperties_declare'] = import_cptools_config('3e:electron_extraproperties_declare')
r['electron_toolconfigs'] = import_cptools_config('3e:electron_toolconfigs')
r['electron_tools_properties_set'] = import_cptools_config('3:electron_properties_set')
r['electron_extraproperties_fill'] = import_cptools_config('3:electron_extraproperties_fill')
r['trigglob_properties_set'] = import_trigglob_config('3e')
r['eventloop_trigcounters'] = import_eventloop('3d3e:trigcounters')
r['eventloop_electron_selection'] = import_eventloop('3d3e:electron_selection')
r['eventloop_muon_selection'] = import_eventloop('3d3e:muon_selection')
r['trigger_matching_requirements'] = import_eventloop('3e:trigger_matching')
write_example(r,['DATA2015','DATA2017'])
