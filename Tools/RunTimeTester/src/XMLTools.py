# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os.path


def nodeToString(node):
  """
  return text content of this possed noed and
  all its children
  """
  
  text = getText([node])
  for n in node.childNodes:
    text += nodeToString(n)

  return text
    
def getText(nodeList):
  text = ''
  for node in nodeList:
    #    if node.nodeType == node.TEXT_NODE:
    if isTextNode(node):      
      text = text + node.data.strip()
      text = text.strip()
      
  return text
               

def isTextNode(node):
  return node.nodeType == node.TEXT_NODE


def isElementNode(node):
  return node.nodeType == node.ELEMENT_NODE

def getElementNodes(nodeList):
    return [n for n in nodeList if isElementNode(n)]


