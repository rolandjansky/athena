/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/CaloInfo.h"
#include <iostream>

using namespace std;
using namespace TrigConf;

CaloInfo::CaloInfo()
   : L1DataBaseclass(),
     m_GlobalScale( 1 ),
     m_JetWeights(),
     m_CaloSinCos(),
     m_METSigParam(),
     m_IsolationHAIsoForEMthr( 5 ),
     m_IsolationEMIsoForEMthr( 5 ),
     m_IsolationEMIsoForTAUthr( 5 ),
     m_tobEM(),
     m_tobTau(),
     m_tobJetSmall(),
     m_tobJetLarge(),
     m_JetWindowSizeSmall( 0 ),
     m_JetWindowSizeLarge( 0 )
{}
  
void
CaloInfo::addJetWeight(int jw) {
   m_JetWeights.push_back(jw);
}
 

void
CaloInfo::addCaloSinCos(const TrigConf::CaloSinCos& csc) {
   m_CaloSinCos.push_back(csc);
}

void
CaloInfo::clear() {
   m_GlobalScale = 1;
   m_CaloSinCos.clear();
   m_JetWeights.clear();

   m_IsolationHAIsoForEMthr.clear();
   m_IsolationEMIsoForEMthr.clear();
   m_IsolationEMIsoForTAUthr.clear();

   m_IsolationHAIsoForEMthr.resize(5);
   m_IsolationEMIsoForEMthr.resize(5);
   m_IsolationEMIsoForTAUthr.resize(5);

}


void
CaloInfo::setIsolation(const std::string & isothrtype, unsigned int bit, const IsolationParam & isopar) {
   if(bit<1 || bit>5) {
      cout << "Isolation bit is outside the range [1,5] : " << bit << " - not being used" << endl;
      return;
   }
   if(isothrtype=="HAIsoForEMthr") {
      m_IsolationHAIsoForEMthr[bit-1] = isopar;
   } else if(isothrtype=="EMIsoForEMthr") {
      m_IsolationEMIsoForEMthr[bit-1] = isopar;
   } else if(isothrtype=="EMIsoForTAUthr") {
      m_IsolationEMIsoForTAUthr[bit-1] = isopar;
   } else {
      cout << "Isolation threshold " << isothrtype << " is unknown [HAIsoForEMthr, EMIsoForEMthr, EMIsoForTAUthr] - not being used" << endl;
   }
}

  
void
CaloInfo::print(const std::string& indent, unsigned int /*detail*/) const {

   cout << indent <<  "================================================ " << endl; 
   cout << indent << "CaloInfo: " << endl; 
   printNameIdV(indent);
   cout << indent << "  Global em scale : " << globalEmScale() << endl;
   cout << indent << "  Global jet scale: " << globalJetScale() << endl;
   if(m_JetWeights.size()>0) {
      cout << indent << "  JetWeights:   ";
      for(unsigned int i=0; i<m_JetWeights.size(); ++i) {
         cout << i << ": " << m_JetWeights[i] << ", ";
         if((i+1)%6==0) cout << endl;
         if(i==5) cout << indent << "                ";
      }
   }
   m_METSigParam.print(indent);
   for(const IsolationParam & isop : m_IsolationHAIsoForEMthr) {
      isop.print(indent);
   }
   cout << "================================================ " << endl; 
}  

void
TrigConf::CaloInfo::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   
   indent(xmlfile, indentLevel, indentWidth)
      << "<CaloInfo name=\""<< name() <<"\" global_em_scale=\""<< globalEmScale() <<"\" global_jet_scale=\""<< globalJetScale() <<"\">" << endl;

   bool hasJetWeights(false);
   for(int jw : m_JetWeights) // check if there is a jetweight that is not 0
      if(jw!=0) hasJetWeights = true;

   if(hasJetWeights) {
      unsigned int num(1);
      for(int jw : m_JetWeights)
         indent(xmlfile, indentLevel+1, indentWidth)
            << "<JetWeight num=\"" << num++ << "\" weight=\""<< jw <<"\"/>" << endl;
   }

   metSigParam().writeXML(xmlfile, indentLevel+1, indentWidth);

   // isolation parametrization
   indent(xmlfile, indentLevel+1, indentWidth)
      << "<Isolation thrtype=\"HAIsoForEMthr\">" << endl; 
   for(const IsolationParam & isop : m_IsolationHAIsoForEMthr)
      isop.writeXML(xmlfile, indentLevel+2, indentWidth);
   indent(xmlfile, indentLevel+1, indentWidth)
      << "</Isolation>" << endl; 

   indent(xmlfile, indentLevel+1, indentWidth)
      << "<Isolation thrtype=\"EMIsoForEMthr\">" << endl; 
   for(const IsolationParam & isop : m_IsolationEMIsoForEMthr)
      isop.writeXML(xmlfile, indentLevel+2, indentWidth);
   indent(xmlfile, indentLevel+1, indentWidth)
      << "</Isolation>" << endl; 

   indent(xmlfile, indentLevel+1, indentWidth)
      << "<Isolation thrtype=\"EMIsoForTAUthr\">" << endl;
   for(const IsolationParam & isop : m_IsolationEMIsoForTAUthr)
      isop.writeXML(xmlfile, indentLevel+2, indentWidth);
   indent(xmlfile, indentLevel+1, indentWidth)
      << "</Isolation>" << endl; 


   indent(xmlfile, indentLevel+1, indentWidth)
      << "<MinimumTOBPt thrtype=\"EM\" ptmin=\"" << m_tobEM.ptmin << "\" etamin=\"" << m_tobEM.etamin << "\" etamax=\"" << m_tobEM.etamax << "\" priority=\"" << m_tobEM.priority <<  "\"/>" << endl;
   indent(xmlfile, indentLevel+1, indentWidth)
      << "<MinimumTOBPt thrtype=\"TAU\" ptmin=\"" << m_tobTau.ptmin << "\" etamin=\"" << m_tobTau.etamin << "\" etamax=\"" << m_tobTau.etamax << "\" priority=\"" << m_tobTau.priority <<  "\"/>" << endl;
   indent(xmlfile, indentLevel+1, indentWidth)
      << "<MinimumTOBPt thrtype=\"JETS\" window=\"" << m_JetWindowSizeSmall << "\" ptmin=\"" << m_tobJetSmall.ptmin << "\" etamin=\"" << m_tobJetSmall.etamin << "\" etamax=\"" << m_tobJetSmall.etamax << "\" priority=\"" << m_tobJetSmall.priority <<  "\"/>" << endl;
   indent(xmlfile, indentLevel+1, indentWidth)
      << "<MinimumTOBPt thrtype=\"JETL\" window=\"" << m_JetWindowSizeLarge << "\" ptmin=\"" << m_tobJetLarge.ptmin << "\" etamin=\"" << m_tobJetLarge.etamin << "\" etamax=\"" << m_tobJetLarge.etamax << "\" priority=\"" << m_tobJetLarge.priority <<  "\"/>" << endl;


   indent(xmlfile, indentLevel, indentWidth) << "</CaloInfo>" << endl;

}

