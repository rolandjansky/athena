# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def getText(node):

    s = ''
    for n in node.childNodes:
        if n.nodeType == node.TEXT_NODE:
            s += n.data
    return s


def getSingleNodeText(node, tag):
            nodes = node.getElementsByTagName(tag)
            snode = nodes[0]
            return getText(snode)

