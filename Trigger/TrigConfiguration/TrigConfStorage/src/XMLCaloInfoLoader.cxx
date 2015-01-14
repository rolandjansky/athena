/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLCaloInfoLoader.h"
#include "./XMLCaloSinCosLoader.h"
#include "./XMLBoostHelper.h"
#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/CaloSinCos.h"

#include <map>
#include <sstream>
#include <vector>
#include <iostream>
#include <typeinfo>

using namespace std;

bool
TrigConf::XMLCaloInfoLoader::load(CaloInfo& caloinfo) {

   if( !is_a("CaloInfo") ) return false;

   caloinfo.clear();

   int ival=0;
   float fval=0;
   string strval="";

   if(  readAttribute(pt(), "name", strval) )           caloinfo.setName(strval);
   if(  readAttribute(pt(), "version", ival) )          caloinfo.setVersion(ival);
   if(  readAttribute(pt(), "global_scale", fval) )     caloinfo.setGlobalScale(fval);
   if(  readAttribute(pt(), "global_em_scale", fval) )  caloinfo.setGlobalEmScale(fval);
   if(  readAttribute(pt(), "global_jet_scale", fval) ) caloinfo.setGlobalJetScale(fval);


   // jet weights
   vector<int> vals(12, 0);
   for(value_type v : pt()) {

      if(v.first!="JetWeight") continue;

      int num;
      if( !readAttribute(v.second, "num", num) ) continue;

      if ( num < 1 || num > 12) {
         TRG_MSG_WARNING("JetWeight's attribute num must be between 1 and 12, but tried to add " << num << ". ==> Ignore");
         continue;
      }

      // in the new style the weight is an attribute, in the old it is text
      if( !readAttribute(v.second, "weight", ival) ) {
         getTextContent(v.second,ival);
      }
      
      if (vals[num-1] != 0) {
         TRG_MSG_WARNING( "JetWeight " << num << " was already set to "
                          << vals[num-1] << " and now trying to set it to " 
                          << ival << ". ==> Keep the old value");
         continue;
      }
      vals[num-1] = ival;
   }

   for(int w : vals)
      caloinfo.addJetWeight(w);

   // read the parameters for Minimum TOB PT
   for(value_type v : pt()) {

      if(v.first != "MinimumTOBPt") continue;

      string thrtype;
      MinTOBPt mintob;

      readAttribute(v.second, "thrtype",  thrtype);
      readAttribute(v.second, "ptmin",  mintob.ptmin);

      if(thrtype=="EM") {
         caloinfo.setMinTobEM(mintob);
      } else if(thrtype=="TAU") {
         caloinfo.setMinTobTau(mintob);
      } else {
         unsigned int windowsize;
         readAttribute(v.second, "window", windowsize);
         if(thrtype=="JETS") {
            caloinfo.setMinTobJetSmall(mintob);
            caloinfo.setJetWindowSizeSmall(windowsize);
         } else {
            caloinfo.setMinTobJetLarge(mintob);            
            caloinfo.setJetWindowSizeLarge(windowsize);
         }
      }
   }



   // read the parameters for MET signifance
   ptree::const_assoc_iterator metsig = pt().find("METSignificance");
   if( metsig != pt().not_found() ) {
      int xsSigmaScale(0), xsSigmaOffset(0), xeMin(0), xeMax(0), teSqrtMin(0), teSqrtMax(0);
      readAttribute(metsig->second, "xsSigmaScale",  xsSigmaScale);
      readAttribute(metsig->second, "xsSigmaOffset", xsSigmaOffset);
      readAttribute(metsig->second, "xeMin",         xeMin);
      readAttribute(metsig->second, "xeMax",         xeMax);
      readAttribute(metsig->second, "teSqrtMin",     teSqrtMin);
      readAttribute(metsig->second, "teSqrtMax",     teSqrtMax);
      caloinfo.metSigParam().setValues(xsSigmaScale, xsSigmaOffset, xeMin, xeMax, teSqrtMin, teSqrtMax);
   }


   std::string isothrtype(""); 

   for (value_type v: pt()) {

      if ( v.first != "Isolation" ) continue;

      //Give thrtype the actual value, v.second.
      readAttribute(v.second, "thrtype", isothrtype );
       
      // Loop over all children. This actually retrieves "<xmlattr>" and "Parametrization".
      for(value_type top: v.second) {
         
         if (top.first != "Parametrization") continue;
         
         IsolationParam param;
  
         // assign values to the actual parameters
         param.setThrType(isothrtype);
         readAttribute(top.second, "isobit"    , ival ); param.setIsobit(ival);
         readAttribute(top.second, "offset"    , ival ); param.setOffset(ival); 
         readAttribute(top.second, "slope"     , ival ); param.setSlope(ival);
         readAttribute(top.second, "mincut"    , ival ); param.setMincut(ival);
         readAttribute(top.second, "upperlimit", ival ); param.setUpperlimit(ival);
         readAttribute(top.second, "etamin"    , ival ); param.setEtamin(ival);
         readAttribute(top.second, "etamax"    , ival ); param.setEtamax(ival);
         readAttribute(top.second, "priority"  , ival ); param.setPriority(ival);
         param.setIsDefined();
         caloinfo.setIsolation(isothrtype, param.isobit(), param);

      }
                
   }
   //caloinfo.print();


   // read the CaloSinCos parameters
   ptree::const_assoc_iterator sincos = pt().find("CaloSinCos");
   if(sincos != pt().not_found()) {
      CaloSinCos csc;
      XMLCaloSinCosLoader cscldr(m_storageMgr, 0);
      for(value_type v : sincos->second) {
         if(v.first=="<xmlattr>") continue;
         cscldr.setPtree(&v);
         cscldr.load(csc);
         caloinfo.addCaloSinCos(csc);
      }
   }

   return true;  
}
