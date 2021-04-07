# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.HypoToolAnalyser import HypoToolAnalyser

import os

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG
logger = logging.getLogger( __name__)

logger.setLevel(DEBUG)

def toolname(tool):
    """string containing class/instance name for and AlgTool"""
    
    return '%s/%s' % (tool.__class__.__name__, tool.name)

def tool_label(tool, do_dot=False):
    """make a label for a dot node consisting of the non-child attributes
    of an AlgTool"""


    indent = '   '
    if do_dot:
        label = toolname(tool)
    else:
        label = indent + toolname(tool)
    for k, v in tool._properties.items():
        # if v.__class__.__name__ == 'TrigJetHypoToolHelperNoGrouper':
        #    continue
            
        if v.__class__.__name__ == 'PrivateToolHandleArray':
            continue
        else:
            if do_dot:
                label += '\\n%s %s' % (k, str(v))
            else:
                label += '\n%s%s %s' % (indent, k, str(v))
    return label

    
def hypoToolToDot(node_labels, connections, toolname, dotdir):
    """ Produce a dot file to visualise the nested AlgTools used
    to configure a jet hypo AlgTool."""
   
    text = ['digraph G{']
    for k, l in connections.items():
        for n in l:
            text.append('%d -> %d;' % (k, n))

    text.append('\n')
    
    for k, label in  node_labels.items():
        description = '[shape=box, label="%s"];' % label
        text.append('%d %s' % (k, description))

    text.append('}')

    text = '\n'.join(text)

    if not dotdir:
        dotdir = os.environ['HOME']

    outfn  = os.path.join(dotdir, toolname+'.dot')

    
    with open(outfn, 'w') as fh:
        logger.debug('writing dot file to ' + outfn)
        fh.write(text)


def hypoToolDisplay(tool, do_dot=False, dotdir=''):
 
    analyser = HypoToolAnalyser(tool)
    node_table, connections = analyser.tables()
    
    node_labels = {k:tool_label(v, do_dot=False) for k, v in node_table.items()}
    s = [tool.name, '\n:']
    for k, v in node_labels.items():
        s.append('\n%3d\n%s' % (k, v))

    s.append('\nconnections:\n')

    for k, v in connections.items():
        s.append('%3d: %s' % (k, str(v)))

    text = '\n'.join(s)
    logger.info(text)

  
    if do_dot:
        node_labels = {k:tool_label(v, do_dot) for k, v in node_table.items()}
        hypoToolToDot(node_labels, connections, tool.name, dotdir)
        
            

