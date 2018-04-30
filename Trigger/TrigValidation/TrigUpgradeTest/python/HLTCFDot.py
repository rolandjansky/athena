###### Here some graphical methods to produce dot files from Decision Handling
 # to visualize: dot -T pdf Step1.dot > Step1.pdf
 
from ViewAlgsTest.connectAlgorithmsIO import connectAlgorithmsIO, graph_generator
from AthenaCommon.AlgSequence import AthSequencer
from TrigUpgradeTest.MenuComponents import isHypoBase, isInputMakerBase, isFilterAlg


def algColor(alg):
    """ Set given color to Alg type"""
    if isHypoBase(alg): return "darkorchid1"
    if 'Combo'  in alg.name(): return "darkorchid1" #TMP for combo hypo
    if isInputMakerBase(alg): return "cyan3"
    if isFilterAlg(alg): return "chartreuse3"

    return "cadetblue1"

def stepCF_ControlFlow_to_dot(stepCF):
    def _dump (seq, indent):
        o = list()
        for c in seq.getChildren():            
            if isinstance(c, AthSequencer):
                o.append( ("%s[color=%s, shape=circle, width=.5, fixedsize=true ,style=filled]\n"%(c.name(),_seqColor(c)), indent) )
            else:
                o.append( ("%s[fillcolor=%s,style=filled]\n"%(c.name(),algColor(c)), indent) )
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

   

    with open('%s.CF.dot'%stepCF.name(), mode="wt") as file:
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
    with open('%s.dot'%(name), mode="wt") as file:
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
                file.write("  %s[fillcolor=%s style=filled]\n"%(cfseq.filter.algname,algColor(cfseq.filter.Alg)))            
                step_connections.append(cfseq.filter)                      
                file.write(  '\n  subgraph cluster_%s {\n'%(cfseq.name)\
                            +'     node [color=white style=filled]\n'\
                            +'     style=filled\n'\
                            +'     color=lightgrey\n'\
                            +'     fontname=Helvetica\n'\
                            +'     label = %s\n'%(cfseq.name))

                cfseq_algs = []
                cfseq_algs.append(cfseq.filter)

                for seq in cfseq.menuSeq.recoSeqList:
                    seq.reuse=False

                for seq in cfseq.menuSeq.recoSeqList:
                    cfseq_algs.append(seq.maker)
                    cfseq_algs.append(seq.sequence )
                    if seq.reuse==False:
                        file.write("    %s[fillcolor=%s]\n"%(seq.maker.algname, algColor(seq.maker.Alg)))
                        file.write("    %s[fillcolor=%s]\n"%(seq.sequence.algname, algColor(seq.sequence.Alg)))
                        seq.reuse=True
                cfseq_algs.append(cfseq.menuSeq.hypo)
                last_step_hypoNodes.append(cfseq.menuSeq.hypo)
                file.write("    %s[color=%s]\n"%(cfseq.menuSeq.hypo.algname, algColor(cfseq.menuSeq.hypo.Alg)))
                file.write('  }\n')              
                file.write(findConnections(cfseq_algs))
                file.write('\n')

            file.write(findConnections(step_connections))
            nstep+=1

        file.write( '}')
        file.close()
    

def stepCF_DataFlow_to_dot(name, cfseq_list):
    with open( '%s.DF.dot'%name, mode="wt" ) as file:
    #strict
        file.write( 'digraph step  {\n'\
                    +'\n'\
                    +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                    +'  edge [ fontname=Helvetica ]\n'
                    +'  %s   [shape=Mdiamond]\n'%name)


        for cfseq in cfseq_list:
            print cfseq.name
            file.write("  %s[fillcolor=%s style=filled]\n"%(cfseq.filter.algname,algColor(cfseq.filter.Alg)))
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

            for seq in cfseq.menuSeq.recoSeqList:
                cfseq_algs.append(seq.maker)
                cfseq_algs.append(seq.sequence )
                if seq.reuse==False:
                    file.write("    %s[fillcolor=%s]\n"%(seq.maker.algname, algColor(seq.maker.Alg)))
                    file.write("    %s[fillcolor=%s]\n"%(seq.sequence.algname, algColor(seq.sequence.Alg)))
                    seq.reuse=True
            cfseq_algs.append(cfseq.menuSeq.hypo)
            file.write("    %s[color=%s]\n"%(cfseq.menuSeq.hypo.algname, algColor(cfseq.menuSeq.hypo.Alg)))
            file.write('  }\n')              
            file.write(findConnections(cfseq_algs))
            file.write('\n')    

        file.write( '}')
        file.close()


def findConnections(alg_list):
    lineconnect=''
    import itertools
    for nodeA, nodeB in itertools.combinations(alg_list, 2):
        line=addConnection(nodeA, nodeB)
        lineconnect+=line
        print 'Data connections between %s and %s: %s'%(nodeA.algname, nodeB.algname, line)
    return lineconnect
  

def findConnections_within_sequence(alg_list):
    lineconnect=''
    import itertools
    for nodeA, nodeB in itertools.combinations(alg_list, 2):
        lineconnect+=findDHconnections(nodeA, nodeB)                                    
    return lineconnect

def findDHconnections(nodeA, nodeB):
    lineconnect=''
    # search connections vis DH
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
    if isinstance(node.Alg, AthSequencer):
         seq=node.Alg
         algs = seq.getChildren()
         algs.pop(0) # remove InputMaker
    else:
        algs.append(node.Alg)

    
    for alg in algs:        
        for k, cval in alg.getValuedProperties().items():
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
        

def addConnection(nodeA, nodeB, label=''):
    line = "    %s -> %s \n"%(nodeA,nodeB)
    #line = "    %s -> %s [label=%s]\n"%(nodeA,nodeB,label)
    # print line
    return line

