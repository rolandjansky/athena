/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/CaloCellGroup.h"
#include <stdlib.h>
#include <ctype.h>
#include <limits>

CaloCellGroup::CaloCellGroup() :
  m_pos(false),
  m_neg(false),
  m_emb(false),
  m_emecow(false),
  m_emeciw(false),
  m_hec(false),
  m_fcal(false),
  m_caloCellId(NULL)
{
}


bool CaloCellGroup::setDefinition(const CaloCell_ID* caloCellId, const std::string& definition, MsgStream& logStr) {
  m_caloCellId=caloCellId;
  m_fieldBoundaries.clear();
  m_values.clear();
  m_emb=false;
  m_emecow=false;
  m_emeciw=false;
  m_hec=false;
  m_fcal=false;
  m_pos=false;
  m_neg=false;
    
  std::vector<std::string> subdefs(2+NFIELDS);
  std::string valuesStr;
  
  std::size_t pos1,pos2;
  pos1=definition.find("[");
  if( pos1==std::string::npos) {
    complain(logStr,definition);
    return false;
  }

  std::size_t i,iField=0;
  for (i=1+pos1;i<definition.size();i++) {
    char c=definition[i];
    if (isspace(c)) continue;
    if (c==']') break;
    if (c=='/') {
      //std::cout << "Subfield " << iField << ": " << subdefs[iField] << std::endl;
      iField++;
      if (iField >= subdefs.size()) break;
    }
    else
      subdefs[iField].push_back(c);
  }
  
  if (iField!=subdefs.size()-1 || i==definition.size()) {
    logStr << MSG::ERROR << "Definition String [" << definition << "] malformed. Expect " 
	   << NFIELDS << " fileds separated by '/'" << endmsg;
    return false;
  }

  valuesStr=definition.substr(i+1);
  //std::cout << "ValueStr " << valuesStr << std::endl;

  //Interpret subcalo-string (first field)
  std::string& subCaloStr=subdefs[0];
  size_t len;
  pos1=0;
  do {
    pos2=subCaloStr.find(",",pos1);
    if (pos2==std::string::npos)
      len=subCaloStr.size();
    else
      len=pos2-pos1;
    if (subCaloStr.compare(pos1,len,"EMB")==0)
      m_emb=true;
    else if(subCaloStr.compare(pos1,len,"EMECIW")==0)
      m_emeciw=true;
    else if(subCaloStr.compare(pos1,len,"EMECOW")==0)
      m_emecow=true;
    else if(subCaloStr.compare(pos1,len,"HEC")==0)
      m_hec=true;
    else if(subCaloStr.compare(pos1,len,"FCAL")==0)
      m_fcal=true;
    else {
      logStr << MSG::ERROR << "Unknown sub-calo name '"<< subCaloStr.substr(pos1,len) <<"'found. " 
	     << "Allowed values are EMB, EMECIW, EMECOW, HEC and FCAL" << endmsg;
      return false;
    }
    pos1=pos2+1;
  }
  while(pos2!=std::string::npos);

  //Interpret side-field (second field)
  if (subdefs[1].size()!=1) {
    complain(logStr,subdefs[1]);
    return false;
  }

  if (subdefs[1][0]=='A') 
    m_pos=true;
  else if (subdefs[1][0]=='C') 
    m_neg=true;
  else if (subdefs[1][0]=='*')
    m_pos=m_neg=true;
  else {
    logStr << MSG::ERROR << "Allowed values for side are: A,C or * (means both), found " << subdefs[1] << endmsg;
    return false;
  }
  //Interpret remaining fields
  for (std::size_t i=0;i<NFIELDS;i++) {
    std::string& subdef=subdefs[i+2];
    int lower=std::numeric_limits<int>::min();
    int upper=std::numeric_limits<int>::max();
    if (subdef.compare("*")!=0) {
      std::size_t pos1=subdef.find('-');
      if (pos1==std::string::npos) {
	if (isdigit(subdef[0])) 
	  lower=upper=atol(subdef.c_str());
	else
	  logStr << MSG::ERROR << "Malformed string [" << subdef << "], expected numerical value!" << endmsg;
      }
      else {//Found '-'
	std::string lStr=subdef.substr(0,pos1);
	std::string uStr=subdef.substr(pos1+1);
	if (lStr.size()>0) {
	  if (isdigit(lStr[0]))
	      lower=atol(lStr.c_str());
	  else
	    logStr << MSG::ERROR << "Malformed string [" << subdef << "], expected numerical value!" << endmsg;
        }
	if (uStr.size()>0) {
	  if (isdigit(uStr[0]))
	    upper=atol(uStr.c_str());
	  else
	    logStr << MSG::ERROR << "Malformed string [" << subdef << "], expected numerical value!" << endmsg;
        }
      }    
    }//end not *
    m_fieldBoundaries.push_back(std::make_pair(lower,upper));
  }//end loop over fields


  const char* ptr1=valuesStr.c_str();
  char* ptr2;
  float val;
  //errno=0;
  do {
    val=strtof(ptr1,&ptr2);
    if (ptr1!=ptr2)
      m_values.push_back(val);
    ptr1=const_cast<const char*>(ptr2+1);
  }
  while(*ptr2!='\0');
 
  logStr << MSG::INFO << "Sussefully parsed defintion string" << endmsg;
  return true;
}



bool CaloCellGroup::inGroup(const Identifier id) const {
  //std::cout << "Check if in Group..";
  if (!m_caloCellId) {
    //throw exception;
    //std::cout << "No Calo Cell Id" << std::endl;
    return false;
  };

  if (!m_caloCellId->is_lar(id))  
    return false;  //Not a lar cell
  
  //std::cout << "have calo..";

  if (m_caloCellId->is_em_barrel(id) && !m_emb) return false;
  if (m_caloCellId->is_em_endcap_inner(id) && !m_emeciw) return false;
  if (m_caloCellId->is_em_endcap_outer(id) && !m_emecow) return false;
  if (m_caloCellId->is_hec(id) && !m_hec) return false;
  if (m_caloCellId->is_fcal(id) && !m_fcal) return false;

  //std::cout << "have subcalo..";

  //Check side
  const int pn=m_caloCellId->pos_neg(id);
  if (pn<0 && !m_neg) return false;
  if (pn>0 && !m_pos) return false;

  //std::cout << "have side..";

  const int layer=m_caloCellId->sampling(id);
  if (layer<m_fieldBoundaries[0].first || layer>m_fieldBoundaries[0].second) return false;

  //std::cout << "have layer..";

  const int region=m_caloCellId->region(id);
  if (region<m_fieldBoundaries[1].first || region>m_fieldBoundaries[1].second) return false;

  //std::cout << "have region..";

  const int eta=m_caloCellId->eta(id);
  if (eta<m_fieldBoundaries[2].first || eta>m_fieldBoundaries[2].second) return false;


  //std::cout << "have eta..";

  const int phi=m_caloCellId->phi(id);
  if (phi<m_fieldBoundaries[3].first || phi>m_fieldBoundaries[3].second) return false;

  //std::cout << "have phi..match! ";
  return true;
}



void CaloCellGroup::complain(MsgStream& logStr, const std::string problem) const {
  logStr << MSG::ERROR << "Malformed definition string '"<<problem << "'" << endmsg;
  return;
}




bool CaloCellGroupList::setDefinition(const CaloCell_ID* caloCellId, const std::vector<std::string>& definitions, 
				       MsgStream& logStr){

  m_groups.clear();
  m_defaults.clear();

  if (definitions.size()==0) {
    logStr << MSG::WARNING << "No definition string found" << endmsg;
    return true;
  }


  const std::string& defaultValueStr=definitions[0];

  const char* ptr1=defaultValueStr.c_str();
  char* ptr2;
  float val;
  //errno=0;
  do {
    val=strtof(ptr1,&ptr2);
    if (ptr1!=ptr2)
      m_defaults.push_back(val);
    ptr1=const_cast<const char*>(ptr2+1);
  }
  while(*ptr2!='\0');


  m_groups.resize(definitions.size()-1);
  for (size_t i=0;i<m_groups.size();i++) {
    bool s=m_groups[i].setDefinition(caloCellId,definitions[i+1],logStr);
    if (!s) return false;
    if (m_groups[i].getValue().size()!=m_defaults.size()) {
      logStr << MSG::ERROR << "Incosistent number of values!" << endmsg;
      return false;
    }
  }
  m_initialized=true;
  return true;
}


const std::vector<float>& CaloCellGroupList::valuesForCell(const Identifier id) {
  if (!m_initialized) {
    //throw exception
    return m_defaults;
  }

  const std::vector<float>* result=&m_defaults;

  std::vector<CaloCellGroup>::const_iterator it=m_groups.begin();
  std::vector<CaloCellGroup>::const_iterator it_e=m_groups.end();
  for(;it!=it_e;++it) {
    //std::cout << " G " << i++;
    if (it->inGroup(id)) result=&(it->getValue());
  }
  return *result;
}

void CaloCellGroupList::dump(const CaloCell_ID* caloCellId) {

  std::cout << "Definitions: " << std::endl;
  for (size_t i=0;i<m_groups.size();i++) {
    std::cout << "Group " << i << ":" << std::endl;
    m_groups[i].printDef();
  }
  std::cout << "Results: "<< std::endl;
  for (const Identifier& id : caloCellId->cell_range()) {
    std::cout << "Values for " << caloCellId->show_to_string(id); 
    const std::vector<float>& x=this->valuesForCell(id);
    std::cout << " [";
    for (std::size_t i=0;i<x.size();i++) 
      std::cout << x[i] << " ";
    std::cout << "]" << std::endl;
  }
  return;
}

void CaloCellGroupList::printDef() const {
  std::cout << "Definitions: " << std::endl;
  for (size_t i=0;i<m_groups.size();i++) {
    std::cout << "Group " << i << ":" << std::endl;
    m_groups[i].printDef();
  }



}

void CaloCellGroup::printDef() const {

  std::cout << "Values";
  for (std::size_t i=0;i<m_values.size();i++)
    std::cout << " " << m_values[i];
  std::cout << " will be used for the following region:" << std::endl;
  std::cout << "SubCalos: " <<m_emb<<m_emecow<<m_emeciw<<m_hec <<m_fcal <<std::endl;
  std::cout << "pos=" << m_pos << " neg=" << m_neg << std::endl;
  std::cout << "Layer [" << m_fieldBoundaries[0].first << " - " << m_fieldBoundaries[0].second << "]" << std::endl;
  std::cout << "Region [" << m_fieldBoundaries[1].first << " - " << m_fieldBoundaries[1].second << "]" << std::endl;
  std::cout << "Eta [" << m_fieldBoundaries[2].first << " - " << m_fieldBoundaries[2].second << "]" << std::endl;
  std::cout << "Phi [" << m_fieldBoundaries[3].first << " - " << m_fieldBoundaries[3].second << "]" << std::endl << std::endl;
}
