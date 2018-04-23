

###### Here some graphical methods


from ViewAlgsTest.connectAlgorithmsIO import connectAlgorithmsIO, graph_generator
from AthenaCommon.AlgSequence import AthSequencer


def algColor(alg):
    if "Hypo" in alg: return "darkorchid1"
    if "Filter" in alg: return "chartreuse3"
    if "InputMaker" in alg: return "cyan3"
    return "cadetblue1"
    
def stepCF_ControlFlow_to_dot(stepCF):
    def _dump (seq, indent):
        o = list()
        for c in seq.getChildren():
            if "AthSequencer" in c.getFullName():
                o.append( ("%s[color=%s, shape=circle, width=.5, fixedsize=true ,style=filled]\n"%(c.name(),_seqColor(c)), indent) )
            else:
                o.append( ("%s[fillcolor=%s,style=filled]\n"%(c.name(),algColor(c.name())), indent) )
            o.append( ("%s -> %s\n"%(seq.name(), c.name()), indent))
            o.extend( _dump (c, indent+1) )
        return o

    def _parOR (seq):
        if seq.ModeOR is True:
            if seq.Sequential is False:
                if seq.StopOverride is True:
                    return True
        return False

    def _seqAND(seq):
        if seq.ModeOR is False:
            if seq.Sequential is True:
                if seq.StopOverride is False:
                    return True
        return False

    def _seqColor(seq):
        if _parOR(seq): return "red"
        if _seqAND(seq): return "blue"
        return "black"

   

    # to visualize: dot -T pdf Step1.dot > Step1.pdf
    file = open( '%s.CF.dot'%stepCF.name(), mode="wt" )
    #strict
    file.write( 'digraph step  {\n'\
                +'\n'\
                +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                +'  edge [ fontname=Helvetica ]\n'
                +'  %s   [shape=Mdiamond]\n'%stepCF.name())

    indent=0
#    out = [("%s[color=%s shape=circle]\n"%(stepCF.name(),_seqColor(stepCF)), indent)]
    out = [("",indent)]
    out.extend( _dump( stepCF, indent=indent+1 ) )
    for n,i in out:
        line= "  "*i+ n
        file.write(line)

    file.write( '}\n')



def all_DataFlow_to_dot(name, step_list):
    file = open( '%s.dot'%(name), mode="wt" )
    file.write( 'digraph step  {\n'\
                    +'\n'\
                    +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                    +'  edge [ fontname=Helvetica ]\n'
                    +'  %s   [shape=Mdiamond]\n'%name)
    
    nstep=1
    last_step_hypoNodes=[]
    for cfseq_list in step_list:
        # loop over steps
        step_connections = []
        step_connections.extend(last_step_hypoNodes)
        # reset the last step
        last_step_hypoNodes =[]
        for cfseq in cfseq_list:
            print cfseq.name
            file.write("  %s[fillcolor=%s style=filled]\n"%(cfseq.filter.algname,algColor(cfseq.filter.algname)))            
            step_connections.append(cfseq.filter)                      
            file.write(  '\n  subgraph cluster_%s {\n'%(cfseq.name)\
                        +'     node [color=white style=filled]\n'\
                        +'     style=filled\n'\
                        +'     color=lightgrey\n'\
                        +'     fontname=Helvetica\n'\
                        +'     label = %s\n'%(cfseq.name))

            cfseq_algs = []
            cfseq_algs.append(cfseq.filter)

            for seq in cfseq.menuSeq.nodeSeqList:
                seq.reuse=False

            for seq in cfseq.menuSeq.nodeSeqList:
                cfseq_algs.append(seq.maker)
                cfseq_algs.append(seq.sequence )
                if seq.reuse==False:
                    file.write("    %s[fillcolor=%s]\n"%(seq.maker.algname, algColor(seq.maker.algname)))
                    file.write("    %s[fillcolor=%s]\n"%(seq.sequence.algname, algColor(seq.sequence.algname)))
                    seq.reuse=True
                cfseq_algs.append(seq.hypo)
                last_step_hypoNodes.append(seq.hypo)
                file.write("    %s[color=%s]\n"%(seq.hypo.algname, algColor(seq.hypo.algname)))
            file.write('  }\n')              
            file.write(findConnections(cfseq_algs))
            file.write('\n')

        file.write(findConnections(step_connections))
        nstep+=1
  
    file.write( '}')
    file.close()
    

def stepCF_DataFlow_to_dot(name, cfseq_list):
    # to visualize: dot -T pdf Step1.dot > Step1.pdf
    file = open( '%s.DF.dot'%name, mode="wt" )
    #strict
    file.write( 'digraph step  {\n'\
                +'\n'\
                +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                +'  edge [ fontname=Helvetica ]\n'
                +'  %s   [shape=Mdiamond]\n'%name)


    for cfseq in cfseq_list:
        print cfseq.name
        file.write("  %s[fillcolor=%s style=filled]\n"%(cfseq.filter.algname,algColor(cfseq.filter.algname)))
        for inp in cfseq.filter.getInputList():
            file.write(addConnection(name, cfseq.filter.algname, inp))
           
        file.write(  '\n  subgraph cluster_%s {\n'%(cfseq.name)\
                    +'     node [color=white style=filled]\n'\
                    +'     style=filled\n'\
                    +'     color=lightgrey\n'\
                    +'     fontname=Helvetica\n'\
                    +'     label = %s\n'%(cfseq.name))

        cfseq_algs = []
        cfseq_algs.append(cfseq.filter)       

        for seq in cfseq.menuSeq.nodeSeqList:
            cfseq_algs.append(seq.maker)
            cfseq_algs.append(seq.sequence )
            if seq.reuse==False:
                file.write("    %s[fillcolor=%s]\n"%(seq.maker.algname, algColor(seq.maker.algname)))
                file.write("    %s[fillcolor=%s]\n"%(seq.sequence.algname, algColor(seq.sequence.algname)))
                seq.reuse=True
            cfseq_algs.append(seq.hypo)
 
            file.write("    %s[color=%s]\n"%(seq.hypo.algname, algColor(seq.hypo.algname)))
        file.write('  }\n')              
        file.write(findConnections(cfseq_algs))
        file.write('\n')    
  
    file.write( '}')
    file.close()


def findConnections(alg_list):
    lineconnect=''
    for nodeA in alg_list:
        for nodeB in alg_list:
            if nodeA is nodeB:
                continue
            dataIntersection = list(set(nodeA.getOutputList()) & set(nodeB.getInputList()))
            if len(dataIntersection) > 0:
                for line in dataIntersection:
                    lineconnect+=addConnection(nodeA.algname,nodeB.algname, line)
                    print 'Data connections between %s and %s: %s'%(nodeA.algname, nodeB.algname, line)
            
    return lineconnect

def findConnections_wihtin_sequence(alg_list):
    lineconnect=''
    import itertools
    for nodeA, nodeB in itertools.combinations(alg_list, 2):
        lineconnect+=findDHconnections(nodeA, nodeB)                                    
    return lineconnect

def findDHconnections(nodeA, nodeB):
    lineconnect=''
    # search connections vis DH
#    print "Find connection between %s and %s"%(nodeA.algname, nodeB.algname)
    outs= getValuesProperties(nodeA)
    ins = getValuesProperties(nodeB)
    dataIntersection = list(set(outs) & set(ins))
    if len(dataIntersection) > 0:
        for line in dataIntersection:
            lineconnect+=addConnection(nodeA.algname,nodeB.algname, line)
            print 'Data DH connections between %s and %s: %s'%(nodeA.algname, nodeB.algname, line)
    return lineconnect
    


def getValuesProperties(node):
    Excluded = ["StoreGateSvc/DetectorStore", "StoreGateSvc"]
    values = []
    algs = []
    if "AthSequencer" in node.Alg.getFullName():
         seq=node.Alg
         algs = seq.getChildren()
         algs.pop(0)
         #.pop(0) # remove inputmaker
    else:
        algs.append(node.Alg)

    
    for alg in algs:        
#        print "prop of alg %s"%(alg.name())
        for k, cval in alg.getValuedProperties().items():
#            print "appending %s"%(cval)
            if type(cval) == type(list()):  
                for val in cval:
                    if type(val) == type(''):
                        if val not in Excluded:
                            values.append(val)            
            elif type(cval) == type(''):
                if cval not in Excluded:
                    values.append(cval)
            else:
                continue
            #getattr(alg, prop))
    return set(values)
        

def addConnection(nodeA, nodeB, label):
    line = "    %s -> %s \n"%(nodeA,nodeB)
    #line = "    %s -> %s [label=%s]\n"%(nodeA,nodeB,label)
    # print line
    return line

