import os,shutil,subprocess

def generateMassSpectrum(m_sq=1000,m_gl=1000,m_LSP=0):
     
    oldcardname= 'MassSpectrum_SG_pMSSM_template.in'
    oldcardproc= subprocess.Popen(['get_files','-data',oldcardname]).communicate()
    if not os.access(oldcardname, os.R_OK):
        print 'ERROR: Could not get mass templatete file'
        raise RuntimeError("parameter card '%s' missing!"%oldcardname)
    
    oldcard = open('MassSpectrum_SG_pMSSM_template.in','r')

    massCardName= 'MassSpectrum_%s_%s_%s.in' %(m_sq,m_gl,m_LSP)
    newcard = open(massCardName,'w')
    
    if (m_LSP==0):    m_LSP=0.5
    
    squarks = []
    masses = {}
    for anum in [1,2,3,4]:
        squarks += [str(1000000+anum),str(-1000000-anum),str(2000000+anum),str(-2000000-anum)]
    for q in squarks: masses[q] = m_sq
    masses['1000021'] = m_gl
    masses['1000022'] = m_LSP
    m_heavy=450000
    
    blockName = ""
    for line in oldcard:
        if line.strip().startswith('BLOCK'):
            blockName = line.strip().split()[1]
            newcard.write(line)
            continue	
        if line.strip().startswith('#'):
            newcard.write(line)
            continue
        
        pdgid = None
        comment= None
        if blockName == 'MASS' and len(line.strip().split()) > 0:
            pdgid = line.strip().split()[0]
            comment= line.strip().split('#')[-1]
        if pdgid != None and blockName == 'MASS' and masses.has_key(pdgid):
            newcard.write('   %s    %E  # %s \n'%(pdgid,float(masses[pdgid]),comment))
        elif pdgid != None and blockName == 'MASS':  
            newcard.write(line) 
	    
        if blockName != 'MASS':
            newcard.write(line)
    
    oldcard.close()
    newcard.close()	    
    return massCardName
