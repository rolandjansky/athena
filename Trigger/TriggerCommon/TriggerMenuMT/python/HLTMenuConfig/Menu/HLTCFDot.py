# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

###### Here some graphical methods to produce dot files from Decision Handling
 # to visualize: dot -T pdf Step1.dot > Step1.pdf
 
from AthenaCommon.AlgSequence import AthSequencer
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import algColor
import itertools
from AthenaCommon.CFElements import getSequenceChildren, isSequence, compName

def create_dot():
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    return TriggerFlags.generateMenuDiagnostics() or ConfigFlags.Trigger.generateMenuDiagnostics


DrawHypoTools=True

def drawHypoTools(file, all_hypos):
    all_hypos=list(set(all_hypos))
    for hp in all_hypos:
        for hypotool in hp.tools:
            file.write("    %s[fillcolor=yellow,style=filled,shape= Mdiamond]\n"%(hypotool))
            file.write("    %s -> %s [style=dashed, color=grey]\n"%(compName(hp.Alg), hypotool))


def stepCF_ControlFlow_to_dot(stepCF):
    def _dump (seq, indent):
        o = list()
        if isSequence(seq):
            for c in getSequenceChildren( seq ):
                if isSequence(c):
                    o.append( ("%s[color=%s, shape=circle, width=.5, fixedsize=true ,style=filled]\n"%(compName(c),_seqColor(c)), indent) )
                else:
                    o.append( ("%s[fillcolor=%s,style=filled]\n"%(compName(c),algColor(c)), indent) )
                o.append( ("%s -> %s\n"%(compName(seq), compName(c)), indent))
                o.extend( _dump (c, indent+1) )
        return o

    def _parOR (seq):
        try:
            if seq.ModeOR is True:
                if seq.Sequential is False:
                    if seq.StopOverride is True:
                        return True
        except AttributeError:
            return False # Offline sequence may not have these set
        return False

    def _seqAND(seq):
        try:
            if seq.ModeOR is False:
                if seq.Sequential is True:
                    if seq.StopOverride is False:
                        return True
        except AttributeError:
            return False # Offline sequence may not have these set
        return False

    def _seqColor(seq):
        if _parOR(seq): 
            return "red"
        if _seqAND(seq): 
            return "dodgerblue3"

        return "black"

   

    with open('%s.CF.dot'%compName(stepCF), mode="wt") as file:
    #strict
        file.write( 'digraph step  {  \n'\
                    +' concentrate=true;\n'\
                    +' rankdir="LR";\n'
                    +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                    +'  edge [ fontname=Helvetica ]\n'
                    +'  %s   [shape=Mdiamond]\n'%compName(stepCF))

        indent=0
    #    out = [("%s[color=%s shape=circle]\n"%(stepCF.getName(),_seqColor(stepCF)), indent)]
        out = [("",indent)]
        out.extend( _dump( stepCF, indent=indent+1 ) )
        for n,i in out:
            line= "  "*i+ n
            file.write(line)

        file.write( '}\n')



def all_DataFlow_to_dot(name, step_list):
    with open('%s.dot'%(name), mode="wt") as file:
        file.write( 'digraph step  {  \n'\
                        +' concentrate=true;\n'\
                        +' rankdir="LR";\n'
                        +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                        +'  edge [ fontname=Helvetica ]\n'
                        +'  %s   [shape=Mdiamond]\n'%name)

        nstep=1
        last_step_hypoNodes=[]
        all_hypos=[]
        for cfseq_list in step_list:
            # loop over steps
            step_connections = []
            step_connections.extend(last_step_hypoNodes)
            # reset the last step
            last_step_hypoNodes =[]
            for cfseq in cfseq_list:
                file.write("  %s[fillcolor=%s style=filled]\n"%(compName(cfseq.filter.Alg),algColor(cfseq.filter.Alg)))
                step_connections.append(cfseq.filter)                      
                file.write(  '\n  subgraph cluster_%s {\n'%(cfseq.step.name)\
                            +'     concentrate=true;\n'
                            +'     node [color=white style=filled]\n'\
                            +'     style=filled\n'\
                            +'     color=lightgrey\n'\
                            +'     fontname=Helvetica\n'\
                            +'     label = %s\n'%(cfseq.step.name))

                cfseq_algs = []
                cfseq_algs.append(cfseq.filter)

                if len(cfseq.step.sequences)==0:
                    last_step_hypoNodes.append(cfseq.filter)

                for menuseq in cfseq.step.sequences:
                    cfseq_algs, all_hypos, last_step_hypoNodes = menuseq.buildDFDot(cfseq_algs,
                                                                                    all_hypos,
                                                                                    cfseq.step.isCombo,
                                                                                    last_step_hypoNodes,
                                                                                    file)

                                                                                     
                if cfseq.step.isCombo:
                    if cfseq.step.combo is not None:
                        file.write("    %s[color=%s]\n"%(compName(cfseq.step.combo.Alg), algColor(cfseq.step.combo.Alg)))
                        cfseq_algs.append(cfseq.step.combo)
                        last_step_hypoNodes.append(cfseq.step.combo)
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
        file.write( 'digraph step  {  \n'\
                    +' concentrate=true; \n'\
                    +' rankdir="LR";\n'
                    +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                    +'  edge [ fontname=Helvetica ]\n'
                    +'  %s   [shape=Mdiamond]\n'%name)


        all_hypos = []
        for cfseq in cfseq_list:
            file.write("  %s[fillcolor=%s style=filled]\n"%(compName(cfseq.filter.Alg),algColor(cfseq.filter.Alg)))
            for inp in cfseq.filter.getInputList():
                file.write(addConnection(name, compName(cfseq.filter.Alg), inp))

            file.write(  '\n  subgraph cluster_%s {\n'%(cfseq.step.name)\
                        +'     concentrate=true;\n'
                        +'     node [color=white style=filled]\n'\
                        +'     style=filled\n'\
                        +'     color=lightgrey\n'\
                        +'     fontname=Helvetica\n'\
                        +'     label = %s\n'%(cfseq.step.name))

            cfseq_algs = []
            cfseq_algs.append(cfseq.filter)

            for menuseq in cfseq.step.sequences:
                    cfseq_algs, all_hypos, _ = menuseq.buildDFDot(cfseq_algs,
                                                                  all_hypos,
                                                                  True,
                                                                  None,
                                                                  file)
            if cfseq.step.isCombo:
                if cfseq.step.combo is not None:
                    file.write("    %s[color=%s]\n"%(compName(cfseq.step.combo.Alg), algColor(cfseq.step.combo.Alg)))
                    cfseq_algs.append(cfseq.step.combo)
            file.write('  }\n')              

            file.write(findConnections(cfseq_algs))
            file.write('\n')

        if DrawHypoTools:
            drawHypoTools(file, all_hypos)
            
        file.write( '}')
        file.close()


def findConnections(alg_list):
    lineconnect=''

    alg_set = set(alg_list) # make them unique
    for nodeA, nodeB in itertools.permutations(alg_set, 2):
        ins=nodeB.getInputList()
        outs=nodeA.getOutputList()
        dataIntersection = list(set(outs) & set(ins))
        if len(dataIntersection) > 0:
            for line in dataIntersection:
                lineconnect+=addConnection(compName(nodeA.Alg), compName(nodeB.Alg), line)
#                print "Data connections between %s and %s: %s"%(nodeA.Alg.getName(), nodeB.Alg.getName(), line)

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
            lineconnect+=addConnection(compName(nodeA.Alg), compName(nodeB.Alg), line)
            #print 'Data DH connections between %s and %s: %s'%(nodeA.Alg.getName(), nodeB.Alg.getName(), line)
    return lineconnect
    


def getValuesProperties(node):
    Excluded = ["StoreGateSvc/DetectorStore", "StoreGateSvc"]
    values = []
    algs = []
    if isinstance(node.Alg, AthSequencer):
         seq=node.Alg
         algs = getSequenceChildren( seq )
         algs.pop(0) # remove InputMaker
    else:
        algs.append(node.Alg)

    
    for alg in algs:        
        for k, cval in alg.getValuedProperties().items():
            if type(cval) is list:  
                for val in cval:
                    if val == '': # CAT type(val) is None ??
                        if val not in Excluded:
                            values.append(val)            
            elif cval == '': # CAT type(val) is None ??
                if cval not in Excluded:
                    values.append(cval)
            else:
                continue
    return set(values)
        

def addConnection(nodeA, nodeB, label=''):
    line = "    %s -> %s \n"%(nodeA,nodeB)
    #line = "    %s -> %s [label=%s]\n"%(nodeA,nodeB,label)
    # print line
    return line

