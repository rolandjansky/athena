/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ViewAlgs/TrigCompositeTraversal.h"


namespace ViewAlgs {

  const xAOD::TrigComposite* find(const xAOD::TrigComposite* start, const std::function<bool(const xAOD::TrigComposite*)>& filter) {
    if ( filter( start ) )
      return start;

    if ( start->hasObjectLink("seed") ) 
      return find(start->object<xAOD::TrigComposite>("seed"), filter);
    else 
      return nullptr;	
  }
 


  bool HasObject::operator()(const xAOD::TrigComposite* composite ) const {
    return composite->hasObjectLink(m_name);
  }



  bool setObjectLinks(  xAOD::TrigComposite* start, ElementLinkVector<xAOD::TrigCompositeContainer> linkVector){
    bool status = true;

    if (linkVector.size()!=0) start->setDetail( "IsComposite",1);    
    for (auto link: linkVector){
      int childkey= link.key();
      std::string name = start->name();
      makeObjName( name, childkey);
      start->setName(name);
      std::vector<int> readindex;
      std::string childname = std::to_string(childkey);
      start->getDetail( childname+"Indices", readindex);
      readindex.push_back(link.index() );
      start->setDetail( childname+"Indices", readindex ) ;
    }
    return status;
  }


  bool makeObjName(std::string& name, int childkey){
    std::string tmp;
    std::string childname = std::to_string(childkey);
    if (name.find(childname) == std::string::npos )
      name+="."+childname;  
    return true;
  }

  
  bool getChildKeys(std::string name, std::vector<int> &childkeys){
    childkeys.clear();
    std::stringstream Str(name);
    std::string childname, startname;
    int childkey;
    int count =0;
    while (getline(Str,childname, '.')){
      if (count == 0){
	startname = childname;
      }
      else{
	std::stringstream(childname)>>childkey;
	childkeys.push_back(childkey);
      }
      count++;
    }
    if (count==0){
      std::cout <<"TrigCompositeTraversal ViewAlgs::getChildKeys      WARNING Not found any child from name "<<name<< std::endl;
      return false;
    }
    return true;
  }



  bool getObjectLinks( const xAOD::TrigComposite* start, ElementLinkVector<xAOD::TrigCompositeContainer>& linkVector){    
    //recursively find in the seeds
    if ( start->hasObjectLink("seed") ) 
      return getObjectLinks(start->object<xAOD::TrigComposite>("seed"), linkVector);
    
    int isComposite=0;
    start->getDetail( "IsComposite",isComposite);    
    if (!isComposite) {
      std::cout <<"TrigCompositeTraversal ViewAlgs::getObjectLinks    WARNING  Proxy "<< start->name()<< " is not composite!"<<std::endl;
      return false;
    }

    bool status = true;
    std::vector<int> childkeys;
    std::string name=start->name();
    if (! getChildKeys(name, childkeys)) return false;

   for (auto key: childkeys){
     std::vector<int> indices;
     std::string childname = std::to_string(key);
     if (! (status = start->getDetail( childname+"Indices", indices))){
       std::cout <<"TrigCompositeTraversal               WARNING ViewAlgs::getObjectLinks:   "<< childname <<"Indices not found" <<std::endl;
       return status;
     }
     for (unsigned int index=0;index<indices.size();index++){
       linkVector.push_back(ElementLink<xAOD::TrigCompositeContainer>(key, index));
     }
   }
   return status;
 }

  

}
