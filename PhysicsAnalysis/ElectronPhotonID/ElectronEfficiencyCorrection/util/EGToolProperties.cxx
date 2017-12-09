/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "EGToolProperties.h"
#include "Messaging.h"

void dumbProperties(const asg::AsgTool& tool){
    ///Get the properties
    MSG_INFO("=============" );  
    MSG_INFO("Tool Name: " <<tool.name() );        
    MSG_INFO("Properties" );  
    auto propertyMap = tool.getPropertyMgr()->getProperties();
    for (auto  i : propertyMap){
        MSG_INFO("=============" );  
        MSG_INFO("Name : " <<i.first  <<  " Type : " <<i.second->typeName() );  
        int type= i.second->type();
        switch(type){    
        case Property::Type::BOOL :{
            bool value = *(tool.getProperty<bool>(i.first));
            MSG_INFO("Value : " <<value );        
            break;      
        }
        case Property::Type::INT:{
            int value = *(tool.getProperty<int>(i.first));
            MSG_INFO("Value : " <<value );        
            break;
        }
        case Property::Type::FLOAT :{
            float value = *(tool.getProperty<float>(i.first));
            MSG_INFO("Value : " <<value );        
            break;
        }
        case Property::Type::DOUBLE :{
            double value = *(tool.getProperty<double>(i.first));
            MSG_INFO("Value : " <<value );        
            break;
        }
        case Property::Type::STRING :{
            std::string value = *(tool.getProperty<std::string>(i.first));
            MSG_INFO("Value : " <<value );        
            break;
        }
        case Property::Type::STRINGVECTOR :{
            std::vector<std::string> value= *(tool.getProperty< std::vector<std::string> > (i.first));
            MSG_INFO("Values : ");        
            for (auto  j : value){
                MSG_INFO(j);        
            }
            break;
        }
        case Property::Type::FLOATVECTOR :{
            std::vector<float> value = *(tool.getProperty< std::vector<float> > (i.first));
            MSG_INFO("Values : ");        
            for (auto  j : value){
                MSG_INFO(j);        
            }
            break;
        }
        case Property::Type::INTVECTOR :{
            std::vector<int> value = *(tool.getProperty< std::vector<int> > (i.first));
            MSG_INFO("Values : ");        
            for (auto  j : value){
                MSG_INFO(j);        
            }
            break;
        }
        default :
            if(i.first == "DefaultRandomRunNumber" ){
                MSG_INFO("Value : " << *(tool.getProperty<unsigned int>(i.first)));
            }
            if(i.first == "OutputLevel" ){
                MSG_INFO("Value : " << static_cast<int> (tool.msg().level()));
            }
            break;
        }
    }
    MSG_INFO("=============" );  
}


