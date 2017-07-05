#!/usr/bin/python

import os,commands,sys,logging
import ROOT
import time,math,cmath

class Truth:
    def __init__(self,log,logLevel):
        if log=='':
            self.logger = logging.getLogger('Truth')
            hdlr = logging.FileHandler('Truth.log',"w")
            formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
            hdlr.setFormatter(formatter)
            self.logger.addHandler(hdlr)
            self.logger.setLevel(logLevel)
        else:
            self.logger = log
            
    def getFilePar(self):
        com="grep RTTINFO MYRTTINFOS.txt | cut -d= -f2 | tail -n1"
        res,testName=commands.getstatusoutput(com)
        if res!=0 :
            self.logger.error(testName)
            return -1
        
        nTestName=''
        for i in testName:
            if i !=' ':
                nTestName+=i;

        filePar=nTestName+".par";
        com="wget http://cern.ch/CaloRTT/"+filePar
        res,out=commands.getstatusoutput(com)
        if res!=0 :
            self.logger.error(out)
            return -1
        return filePar

    def initHTML(self):
        f=open("Truth.html","w")
        f.write("<html><head><title>Truth</title><link rel=\"StyleSheet\" href=\"rtt.css\" type=\"text/css\" /><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\"><meta http-equiv=\"Content-Language\" content=\"en\"></head>")
        f.write("<body>")
        f.close()


        f=open("Truth.txt","w")
        f.close()
        
    def closeHTML(self):
        f=open("Truth.html","a")
        f.write("</body>")
        f.write("</html>")

    def mysign(self,val):
        if val<0.:
            return -1
        else:
            return 1

    def saveHisto(self,hist,filePar,opt):
        html= open('Truth.html','a')

        txt= open("Truth.txt","a");

        tmp=hist.GetTitle()
        name=""
        for i in tmp:
            if i==' ':
                i='_'
            if i=='(':
                i='z'
            if i==')':
                i='z'
            name+=i

        com='grep RTTParm_truth_'+name+ ' '+filePar +' | cut -d= -f2 | tail -n1'

        res,max=commands.getstatusoutput(com)
        if res!=0:
            self.logger.error('Problem to the par of '+name+' : '+max)
            return -1

        can=ROOT.TCanvas()
        # only 3 digits
        ROOT.TGaxis.SetMaxDigits(3)
        
        hist.SetLineColor(2)
        hist.SetLineStyle(2)
        hist.Draw(opt)

        txt.write("################################\n");
        txt.write("Name : "+str(hist.GetTitle())+'\n')
        txt.write("Mean = "+str(hist.GetMean())+'\n')
        txt.write("RMS = "+str(hist.GetRMS())+'\n')

        try:
            float(max)
        except:
            max=0

        isOk=0
        if math.fabs(float(hist.GetMean())) > float(max):
            isOk=-1
            txt.write("TEST FAILED (lim = "+str(max)+')\n')
        else:
            txt.write("TEST OK (lim = "+str(max)+')\n')

        can.Modified()
        can.Update()

        epsname=name+".eps"
        gifname=name+".gif"

        can.SaveAs(epsname)

        com1='pstopnm -ppm -xborder 0 -yborder 0 -portrait '+epsname
        com2='ppmtogif '+epsname+'001.ppm >'+gifname
        
        os.system(com1)
        os.system(com2)
        
        html.write("<CENTER><BR>")
        html.write("<BR>")
        html.write("<IMG SRC=\""+gifname+'\"><BR>');
        html.write("<HR>")

        if isOk == -1 :
            html.write("<BR>")
            html.write("<CENTER><font color=red>TEST FAILED (lim = "+str(max)+")</font></CENTER>")
        else:
            html.write("<BR>")
            html.write("<CENTER><font color=green>TEST OK (lim = "+str(max)+")</font></CENTER>")
            html.write("<BR>")
        html.write("<HR>")
        
        html.close()
        txt.close()

        return isOk

    def doElectrons(self,filePar):
        self.initHTML()
        
        #f = ROOT.TFile('ntuple.root')
        #f.cd('CollectionTree')
        #mytree=ROOT.gDirectory.Get('CollectionTree')

        ##################################
        #os.system('ls data/*root.* > ntuples.txt')
        os.system('ls ntuple.root > ntuples.txt')
        
        f=open('ntuples.txt','r')
        lines=f.readlines()
        f.close()

        chain = ROOT.TChain('CollectionTree')

        for li in lines:
            chain.Add('./'+li.strip())

        mytree=chain
        #################################

        m_drmin  = ROOT.TH1F("m_drmin","drmin",100,-0.2,0.2)
        m_h1   = ROOT.TH1F("m_h1","Energy resolution",100,-0.25,0.25)
        m_h1.SetXTitle("(E_{t}(reco)-E_{t}(true))/E_{t}(true)")

        m_h2   = ROOT.TH1F("m_h2","Phi resolution",100,-0.01,0.01)
        m_h2.SetXTitle("#Phi resolution (rad)")

        m_h3   = ROOT.TH1F("m_h3","Eta resolution in the barrel",100,-0.01,0.01)
        m_h3.SetXTitle("#eta resolution")
        
        m_h4   = ROOT.TH1F("m_h4","Eta resolution in the endcap",100,-0.01,0.01)
        m_h4.SetXTitle("#eta resolution")

        m_h5   = ROOT.TH1F("m_h5","Efficiency vs eta",50,-3,3)  
        m_h5.SetXTitle("#eta")

        m_tmp1   = ROOT.TH1F("m_tmp1","EtaGen",50,-3,3)
        m_tmp2  = ROOT.TH1F("m_tmp2","cl_eta",50,-3,3) 

        entries=mytree.GetEntriesFast()

        for jentry in xrange( entries ):
            ientry=mytree.LoadTree( jentry )
            if ientry < 0 :
                break

            nb=mytree.GetEntry(jentry)
            if nb<=0:
                continue
            
            nEvent = int(mytree.IEvent)
            if nEvent<0:
                continue

            indEle=[]
            iele=0
            for ipart in range(0,mytree.NPar):
                if abs((mytree.Type)[ipart])==11 and (mytree.GenStat)[ipart]==1 and (mytree.KMothNt)[ipart]==-1:
                    indEle.append(ipart)
                    m_tmp2.Fill((mytree.EtaGen)[indEle[iele]])

                    iele+=1

                    if iele>1:
                        logger.info('two many electrons')
                        return -1

            nele=iele

            # a quel cluster correspond quel electron ?
            # je tourne sur ts les clusters de l ev
            for ic in range(0,mytree.cl_nc): 
                etacl = (mytree.cl_eta)[ic]
                phicl = (mytree.cl_phi)[ic]
                etcl  = (mytree.cl_et)[ic]

                m_drmin.Fill((mytree.ZV)[0]);
                etae = (mytree.EtaGen)[indEle[0]];
                if math.fabs((mytree.cl_eta)[ic])>1.475 :
                    etaclcor = cmath.asinh(cmath.sinh((mytree.cl_eta)[ic])*(1-(mytree.ZV)[mytree.IVPrimary]/(self.mysign((mytree.cl_eta)[ic])*3800.0)))
                    phiclcor = (mytree.cl_phi)[ic]+(0.3*3800.0*(-(mytree.Type)[indEle[0]]/11.0)*self.mysign((mytree.cl_eta)[ic]))/((mytree.cl_et)[ic]*cmath.sinh((mytree.cl_eta)[ic]))
                    m_h4.Fill((etaclcor-etae).real)
                else:
                    etaclcor = cmath.asinh(cmath.sinh((mytree.cl_eta)[ic])-(mytree.ZV)[mytree.IVPrimary]/1600.0)
                    m_h3.Fill((etaclcor-etae).real)
                    phiclcor = (mytree.cl_phi)[ic]+(0.3*1600.0*(-(mytree.Type)[indEle[0]]/11.0)/(mytree.cl_et)[ic])

                phie = (mytree.PhiGen)[indEle[0]]
                ete  = (mytree.PtGen)[indEle[0]]
                
                try:
                    m_h2.Fill(phiclcor.real-phie)
                except:
                    m_h2.Fill(phiclcor-phie)
                m_h1.Fill((etcl-ete)/ete)

                m_tmp1.Fill(etae)

        ROOT.gStyle.SetOptFit(1011)

        m_h1.Fit("gaus")
        m_h2.Fit("gaus")
        m_h3.Fit("gaus")
        m_h4.Fit("gaus")
    		    
        res1=self.saveHisto(m_h1,filePar,'')
        res2=self.saveHisto(m_h2,filePar,'')
        res3=self.saveHisto(m_h3,filePar,'')
        res4=self.saveHisto(m_h4,filePar,'')
        m_h5.Divide(m_tmp1,m_tmp2,1,1,"B")
        
	res5=self.saveHisto(m_h5,filePar,"E")

        if res1==-1 or res2==-1 or res3==-1 or res4==-1 or res5==-1: 
            return -1
        else:
            return 0
            
    def doTop(self,filePar):
        self.initHTML()
        
        f = ROOT.TFile('ntuple.root')
        #f.cd('CollectionTree')
        mytree=ROOT.gDirectory.Get('CollectionTree')

        m_drmin  = ROOT.TH1F("m_drmin","drmin",100,-0.2,0.2)
        m_h1   = ROOT.TH1F("m_h1","Energy resolution",100,-0.25,0.25)
        m_h1.SetXTitle("(E_{t}(reco)-E_{t}(true))/E_{t}(true)")

        m_h2   = ROOT.TH1F("m_h2","Phi resolution",100,-0.01,0.01)
        m_h2.SetXTitle("#Phi resolution (rad)")

        m_h3   = ROOT.TH1F("m_h3","Eta resolution in the barrel",100,-0.01,0.01)
        m_h3.SetXTitle("#eta resolution")
        
        m_h4   = ROOT.TH1F("m_h4","Eta resolution in the endcap",100,-0.01,0.01)
        m_h4.SetXTitle("#eta resolution")

        m_h5   = ROOT.TH1F("m_h5","Efficiency vs eta",50,-3,3)  
        m_h5.SetXTitle("#eta")

        m_tmp1   = ROOT.TH1F("m_tmp1","EtaGen",50,-3,3)
        m_tmp2  = ROOT.TH1F("m_tmp2","cl_eta",50,-3,3) 

        entries=mytree.GetEntriesFast()

        for jentry in xrange( entries ):
            ientry=mytree.LoadTree( jentry )
            if ientry < 0 :
                break

            nb=mytree.GetEntry(jentry)
            if nb<=0:
                continue
            
            nEvent = int(mytree.IEvent)
            if nEvent<0:
                continue

            indEle=[]
            iele=0
            for ipart in range(0,mytree.NPar):
                if  abs((mytree.Type)[ipart])==11 and abs((mytree.Type)[(mytree.KMothNt)[ipart]])==24 :
			#indEle[iele]=ipart
                        indEle.append(ipart)
			m_tmp2.Fill((mytree.EtaGen)[indEle[iele]])
			iele=+1
                        if iele>4:
                            logger.info('two many electrons')
                            return -1

            nele=iele

            # a quel cluster correspond quel electron ?
            # je tourne sur ts les clusters de l ev
            for ic in range(0,mytree.cl_nc): 
                drmin = 9999.
                im    = 0

                # pour un cluster donne je tourne sur tous les electrons primaires trouves precedemment et je minimise dr pour savoir celui qui est le plus pres du cluster 
                for iele in range(0,nele):
                    deta = (mytree.EtaGen)[indEle[iele]] - (mytree.cl_eta)[ic];
                    dphi = (mytree.PhiGen)[indEle[iele]] - (mytree.cl_phi)[ic];
                    
                    if dphi > math.pi: 
                        dphi = math.fabs(dphi) - 2.*math.pi
                    dr = math.sqrt(dphi*dphi + deta*deta)

                    if dr < drmin:
                        drmin = dr
                        im    = iele

                # l'electron matchant le cluster a l'indice im

                m_drmin.Fill(drmin);

                if drmin < 0.1  : 
                    etacl = (mytree.cl_eta)[ic]
                    phicl = (mytree.cl_phi)[ic]
                    etcl  = (mytree.cl_et)[ic]
                    etae = (mytree.EtaGen)[indEle[im]]
                    if math.fabs((mytree.cl_eta)[ic])>1.475 :
                        etaclcor = cmath.asinh(cmath.sinh((mytree.cl_eta)[ic])*(1-(mytree.ZV)[mytree.IVPrimary]/(self.mysign((mytree.cl_eta)[ic])*3800.0)));
                        phiclcor = (mytree.cl_phi)[ic]+(0.3*3800*(-(mytree.Type)[indEle[im]]/11.0)*self.mysign((mytree.cl_eta)[ic]))/((mytree.cl_et)[ic]*cmath.sinh((mytree.cl_eta)[ic]));
                        m_h4.Fill((etaclcor-etae).real)
                    else:
                        etaclcor = cmath.asinh(cmath.sinh((mytree.cl_eta)[ic])-(mytree.ZV)[mytree.IVPrimary]/1600.0)
                        phiclcor = (mytree.cl_phi)[ic]+(0.3*1600.0*(-(mytree.Type)[indEle[im]]/11.0)/(mytree.cl_et)[ic])
                        m_h3.Fill((etaclcor-etae).real)

                    phie = (mytree.PhiGen)[indEle[im]]
                    ete  = (mytree.PtGen)[indEle[im]]

                    try:
                        m_h2.Fill(phiclcor.real-phie)
                    except:
                        m_h2.Fill(phiclcor-phie)
                    m_h1.Fill((etcl-ete)/ete)	 
                    m_tmp1.Fill(etae)


        ROOT.gStyle.SetOptFit(1011)

        m_h1.Fit("gaus")
        m_h2.Fit("gaus")
        m_h3.Fit("gaus")
        m_h4.Fit("gaus")
    		    
        res1=self.saveHisto(m_h1,filePar,'')
        res2=self.saveHisto(m_h2,filePar,'')
        res3=self.saveHisto(m_h3,filePar,'')
        res4=self.saveHisto(m_h4,filePar,'')
        m_h5.Divide(m_tmp1,m_tmp2,1,1,"B")
        
	res5=self.saveHisto(m_h5,filePar,"E")

        if res1==-1 or res2==-1 or res3==-1 or res4==-1 or res5==-1: 
            return -1
        else:
            return 0
    
    def doPhotons(self,filePar):
        self.initHTML()
        
        #f = ROOT.TFile('ntuple.root')
        #f.cd('CollectionTree')
        #mytree=ROOT.gDirectory.Get('CollectionTree')

        ##################################
        #os.system('ls photons/*root* > ntuples.txt')
        os.system('ls ntuple.root > ntuples.txt')
        f=open('ntuples.txt','r')
        lines=f.readlines()
        f.close()

        chain = ROOT.TChain('CollectionTree')

        for li in lines:
            chain.Add('./'+li.strip())

        mytree=chain
        #################################

        m_drmin  = ROOT.TH1F("m_drmin","drmin",100,-0.2,0.2)
        m_h1   = ROOT.TH1F("m_h1","Energy resolution",100,-0.25,0.25)
        m_h1.SetXTitle("(E_{t}(reco)-E_{t}(true))/E_{t}(true)")

        m_h2   = ROOT.TH1F("m_h2","Phi resolution",100,-0.01,0.01)
        m_h2.SetXTitle("#Phi resolution (rad)")

        m_h3   = ROOT.TH1F("m_h3","Eta resolution in the barrel",100,-0.01,0.01)
        m_h3.SetXTitle("#eta resolution")
        
        m_h4   = ROOT.TH1F("m_h4","Eta resolution in the endcap",100,-0.01,0.01)
        m_h4.SetXTitle("#eta resolution")

        m_h5   = ROOT.TH1F("m_h5","Efficiency vs eta",50,-3,3)  
        m_h5.SetXTitle("#eta")

        m_tmp1   = ROOT.TH1F("m_tmp1","EtaGen",50,-3,3)
        m_tmp2  = ROOT.TH1F("m_tmp2","cl_eta",50,-3,3) 

        entries=mytree.GetEntriesFast()

        for jentry in xrange( entries ):
            ientry=mytree.LoadTree( jentry )
            if ientry < 0 :
                break
            nb=mytree.GetEntry(jentry)
            if nb<=0:
                continue          
            nEvent = int(mytree.IEvent)
            if nEvent<0:
                continue
            dRmin = 999.
            TheNearestCluster = -1

            #print int(mytree.IEvent),int(mytree.cl_nc),len(mytree.PtGen)
            if mytree.cl_nc==0 or len(mytree.PtGen)==0:
                continue
            for i in range(0,int(mytree.cl_nc)):
                #print 'i=',i
                dphi = 1000.
                deta = 1000.
	       
                # resolution in energy
                val=((mytree.cl_et)[i]-(mytree.PtGen)[0])/(mytree.PtGen)[0]
                
                m_h1.Fill(val)        
                # resolution in phi
                if  (mytree.cl_phi)[i]-(mytree.PhiGen)[0] < 6: 
                    dphi =((mytree.cl_phi)[i]-(mytree.PhiGen)[0])
                    m_h2.Fill(dphi)

                # resolution in eta barrel corrected by the z vertex spread
                deta=0.
                if  math.fabs((mytree.EtaGen)[0])<1.475:
                    deta =(mytree.cl_eta)[i]-cmath.asinh(cmath.sinh((mytree.EtaGen)[0])+(mytree.ZV)[mytree.IVPrimary]/1600.)
                    m_h3.Fill(deta.real)
                elif math.fabs((mytree.EtaGen)[0])>= 1.475 :
                    deta = (mytree.cl_eta)[i]-cmath.asinh(cmath.sinh((mytree.EtaGen)[0])/(1-self.mysign((mytree.EtaGen)[0])*(mytree.ZV)[mytree.IVPrimary]/3800.))
                    
                    m_h4.Fill(deta.real)
		
                if (math.fabs(dphi) > math.pi):
                    dphi = 2.0*math.pi - math.fabs(dphi)
                dR = math.sqrt(deta.real*deta.real+dphi*dphi)
                if dR < dRmin:
                    dRmin = dR
                    TheNearestCluster = i
	      
	    
            m_tmp2.Fill((mytree.EtaGen)[0])
            if TheNearestCluster >= 0 and dRmin < 0.1:
                m_tmp1.Fill((mytree.EtaGen)[0])

        ROOT.gStyle.SetOptFit(1011)

        m_h1.Fit("gaus")
        m_h2.Fit("gaus")
        m_h3.Fit("gaus")
        m_h4.Fit("gaus")
    		    
        res1=self.saveHisto(m_h1,filePar,'')
        res2=self.saveHisto(m_h2,filePar,'')
        res3=self.saveHisto(m_h3,filePar,'')
        res4=self.saveHisto(m_h4,filePar,'')
        m_h5.Divide(m_tmp1,m_tmp2,1,1,"B")
        
	res5=self.saveHisto(m_h5,filePar,"E")

        if res1==-1 or res2==-1 or res3==-1 or res4==-1 or res5==-1: 
            return -1
        else:
            return 0

    def doPions(self,filePar):
        self.initHTML()
        
        #f = ROOT.TFile('ntuple.root')
        #f.cd('CollectionTree')
        #mytree=ROOT.gDirectory.Get('CollectionTree')

        ##################################
        #os.system('ls pions/*root* > ntuples.txt')
        os.system('ls ntuple.root > ntuples.txt')
        f=open('ntuples.txt','r')
        lines=f.readlines()
        f.close()

        chain = ROOT.TChain('CollectionTree')

        for li in lines:
            chain.Add('./'+li.strip())

        mytree=chain
        #################################

        m_drmin  = ROOT.TH1F("m_drmin","drmin",100,-0.2,0.2)
        m_h1   = ROOT.TH1F("m_h1","Energy resolution",100,-0.5,0.5)
        m_h1.SetXTitle("(p_{t}(reco)-p_{t}(true))/p_{t}(true)")

        m_h2   = ROOT.TH1F("m_h2","Phi resolution",100,-0.08,0.08)
        m_h2.SetXTitle("#Phi resolution (rad)")

        m_h3   = ROOT.TH1F("m_h3","Eta resolution in the barrel",100,-0.1,0.1)
        m_h3.SetXTitle("#eta resolution")
        
        m_h4   = ROOT.TH1F("m_h4","Eta resolution in the endcap",100,-0.1,0.1)
        m_h4.SetXTitle("#eta resolution")

        m_h5   = ROOT.TH1F("m_h5","Efficiency vs eta",50,-3,3)  
        m_h5.SetXTitle("#eta")

        m_tmp1   = ROOT.TH1F("m_tmp1","EtaGen",50,-3,3)
        m_tmp2  = ROOT.TH1F("m_tmp2","cl_eta",50,-3,3) 

        entries=mytree.GetEntriesFast()

        for jentry in xrange( entries ):
            ientry=mytree.LoadTree( jentry )
            if ientry < 0 :
                break

            nb=mytree.GetEntry(jentry)
            if nb<=0:
                continue
            
            nEvent = int(mytree.IEvent)
            if nEvent<0:
                continue

#            print 'nevent=',nEvent

            indPi=[]
            ipi=0

#            print ' mytree.NPar=',mytree.NPar

            for ipart in range(0,mytree.NPar):
                if  (mytree.KMothNt)[ipart]<0 and (abs((mytree.Type)[ipart])==111 or abs((mytree.Type)[ipart])==211) :
#                    print ' mother=',(mytree.KMothNt)[ipart]
#                    print ' (mytree.Type)[ipart]=',(mytree.Type)[ipart]
                    indPi.append(ipart)
                    m_tmp2.Fill((mytree.EtaGen)[indPi[ipi]])
                    ipi=ipi+1
                    if ipi>4:
                        logger.info('two many pions')
                        return -1

#            print 'ipi=',ipi

            if ipi>0:
#                print ' pion type=', abs((mytree.Type)[indPi[0]])
#                print ' # of ConeH1TowerJets=', mytree.Cone7H1TowerJetsjetNum
#                print ' # of ConeH1TopoJets=', mytree.Cone7H1TopoJetsjetNum
                if abs((mytree.Type)[indPi[0]])==111:
                    ncl=0
#                    print ' mytree.cl_nc=',mytree.cl_nc
                    for ic in range(0,mytree.cl_nc): 
                        etacl = (mytree.cl_eta)[ic]
                        phicl = (mytree.cl_phi)[ic]
                        etcl  = (mytree.cl_et)[ic]
                        ncl = ncl +1
#                        print ' cluster etcl,etacl,phicl=',etcl,etacl,phicl
#                        print ' gen et,etacl,phicl=',(mytree.PtGen)[0],(mytree.EtaGen)[0],(mytree.PhiGen)[0]
                        
                        etae = (mytree.EtaGen)[indPi[0]]
                        if math.fabs((mytree.cl_eta)[ic])>1.475 :
                            m_h4.Fill(etacl-etae)                   
                        else:
                            m_h3.Fill(etacl-etae)
                        
                        phie = (mytree.PhiGen)[indPi[0]]
                        ete  = (mytree.PtGen)[indPi[0]]

                        try:
                            m_h2.Fill(phicl.real-phie)
                        except:
                            m_h2.Fill(phicl-phie)

                        m_h1.Fill((etcl-ete)/ete)	 
                        m_tmp1.Fill(etae)
 
#                        print ' nclusters=',ncl

#                if abs((mytree.Type)[indPi[0]])==211 and mytree.Cone7H1TowerJetsjetNum>0 :
                if abs((mytree.Type)[indPi[0]])==211 and mytree.Cone7H1TopoJetsjetNum>0 :

#                    print ' number of jets =', mytree.Cone7H1TopoJetsjetNum
#                    print ' jet and topo gen e =',(mytree.jetEtCone7H1TopoJets)[0],(mytree.PtGen)[indPi[0]]
#                    print ' jet and gen eta=',(mytree.jetEtaCone7H1TopoJets)[0],(mytree.EtaGen)[0]
#                    print ' jet and gen phi=',(mytree.jetPhiCone7H1TopoJets)[0],(mytree.PhiGen)[0]
                        
                    etajet = (mytree.jetEtaCone7H1TopoJets)[0]
                    phijet = (mytree.jetPhiCone7H1TopoJets)[0]
                    etjet  = (mytree.jetEtCone7H1TopoJets)[0]
                    
                    etae = (mytree.EtaGen)[indPi[0]]
                    if abs(etajet)>1.475 :
                        m_h4.Fill(etajet-etae)   
                    else:
                        m_h3.Fill(etajet-etae)
                        
                    phie = (mytree.PhiGen)[indPi[0]]
                    ete  = (mytree.PtGen)[indPi[0]]
                        
                    try:
                        m_h2.Fill(phijet.real-phie)
                    except:
                        m_h2.Fill(phijet-phie)
                        
                    m_h1.Fill((etjet-ete)/ete)	 
                    m_tmp1.Fill(etae)
        
        ROOT.gStyle.SetOptFit(1011)

        m_h1.Fit("gaus")
        m_h2.Fit("gaus")
        m_h3.Fit("gaus")
        m_h4.Fit("gaus")
    		    
        res1=self.saveHisto(m_h1,filePar,'')
        res2=self.saveHisto(m_h2,filePar,'')
        res3=self.saveHisto(m_h3,filePar,'')
        res4=self.saveHisto(m_h4,filePar,'')
        m_h5.Divide(m_tmp1,m_tmp2,1,1,"B")
        
	res5=self.saveHisto(m_h5,filePar,"E")

        if res1==-1 or res2==-1 or res3==-1 or res4==-1 or res5==-1: 
            return -1
        else:
            return 0


    def run(self):

        ROOT.gROOT.Reset()
        ROOT.gStyle.SetStatW(0.19)
        ROOT.gStyle.SetStatFontSize(0.03)
        ROOT.gStyle.SetOptStat(111110)

        filePar=self.getFilePar()
        if filePar==-1:
            self.logger.error('getFilePar() failed')
            return

        
        if filePar.find('Photons')>=0:
            return self.doPhotons(filePar)
            
        if filePar.find('Electrons')>=0:
            return self.doElectrons(filePar)
            
        if filePar.find('Top')>=0 or filePar.find('top')>=0:
            return self.doTop(filePar)

        if filePar.find('Pi')>=0:
            return self.doPions(filePar)

        # if filePar.find('Zee')>=0:
        #    return self.doZee(filePar)

        # if filePar.find('H2e2mu')>=0:
        #    return self.doH2e2mu(filePar)

        
            


#a=Truth(logging.INFO)
#a.run()
