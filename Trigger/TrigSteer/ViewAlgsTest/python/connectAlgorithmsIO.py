# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class GraphGenerator:
  def __init__(self):
    self.__nodes = set()
    self.__links = set()
  
  def addConnection(self, src_algo, src_prop_name, dst_algo, dst_prop_name):
    self.__nodes.add((type(src_algo).__name__, src_algo.name()))
    self.__nodes.add((type(dst_algo).__name__, dst_algo.name()))
    
    self.__links.add((src_algo.name(), src_prop_name, dst_algo.name(), dst_prop_name))
  
  def saveGraph(self, graphName, outputFileName):
    with open(outputFileName, 'w') as f:
      spaces_level = 0
      spaces = lambda: ' ' * 4 * spaces_level
      write_to_file = lambda text: f.write(spaces() + text + '\n')
      write_to_file("## to visualise dot -T pdf workflow.dot > workflow.pdf")

      write_to_file('digraph {')
      spaces_level += 1
      
      write_to_file('label="' + graphName + '";')
      write_to_file('nodesep=0.6;')
      write_to_file('splines=true;')
      write_to_file('sep="+25,25";')
      write_to_file('overlap=scalexy;')
      write_to_file('rankdir=LR;')       
      write_to_file('node [fontsize=14, shape=box,fontname="helvetica"];')
      write_to_file('edge [minlen=2,fontname="helvetica"];')   
      write_to_file('')
      
      for node in self.__nodes:
        if node[0] == 'DecisionAlg':
          write_to_file(node[1] + ' [label="' + node[0] + '\\n' + node[1] + '", shape=hexagon, color=yellow, style=filled];')
        else:
          write_to_file(node[1] + ' [label="class: ' + node[0] + '\\n' + node[1] + '"];')
        
      write_to_file('')
      
      for link in self.__links:
        write_to_file(link[0] + ' -> ' + link[2] + ' [labelfontsize=12, labeldistance=2.5, labelangle=35, headlabel=<<font color="#00CC00"><b>' + link[3] + '</b></font>>, taillabel=<<font color="#CC0000"><b>' + link[1] + '</b></font>>];')
      
      spaces_level -= 1
      write_to_file('}')
  

graph_generator = GraphGenerator()



from Menu import getOutput, setOutput, menu_ingredients

def connectAlgorithmsIO(producer, consumer, propset=True):
  #dst_algo, dst_prop_name, src_algo, src_prop_name):
  #print "connectAlgorithmsIO(consumer=(%s, %s), producer=(%s, %s))"%(consumer[0].name(), consumer[1],producer[0].name(), producer[1])  
  if propset:
    setOutput(consumer[0], consumer[1], getOutput(producer[0], producer[1]))  
  graph_generator.addConnection(producer[0], producer[1], consumer[0], consumer[1])


def connectHypoToMenu(producer, consumer):
    setOutput(consumer, 'InputHypoDecisions', getOutput(producer[0], producer[1]))
    graph_generator.addConnection(producer[0], producer[1], consumer, 'InputHypoDecisions')
    menu_ingredients.addHypo(consumer, producer[0])



class DH:
  def __init__(self, alg, prop):
    self.alg  = alg
    self.prop = prop
  def __str__(self):
    return self.alg.getName()+"."+self.prop
    
  def __lshift__(self, inp):
    """ Enabling convenience notion consumer << producer"""
    if type(inp).__name__ in ('list', 'tuple'):
      [ self << i for i in inp ]
    else:
      print "setting data dependency: ", self, " <-- ", inp
      connectAlgorithmsIO( consumer=( self.alg, self.prop ), producer=( inp.alg, inp.prop ) )

  def __rshift__(self, out):
    """Enabling convenient notion producer >> consumer"""
    if type(out).__name__ in ('list', 'tuple'):
      [ self >> i for i in out ]
    else:
      print "setting data dependency: ", self, " --> ", out
      connectAlgorithmsIO( consumer=( out.alg, out.prop ), producer=( self.alg, self.prop ) )
    
# class In (IOHandle):
#   pass
# class Out (IOHandle):
#   pass
