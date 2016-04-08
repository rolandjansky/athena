# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import string

def getText(nodeList):
  text = ''
  for node in nodeList:
    if node.nodeType == node.TEXT_NODE:
      text = text + node.data
      text = string.strip(text)
  return text
