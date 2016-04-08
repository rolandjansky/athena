/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JiveXML/XMLFormatTool.h"

namespace JiveXML {

  /**
   * Constructor 
   * - setup message service
   */
  XMLFormatTool::XMLFormatTool( const std::string& type , const std::string& name, const IInterface* p):
    AthAlgTool(type,name,p){
    
    //Provide interface
    declareInterface<IFormatTool>(this);

    //Intialize data members
    m_EventBuffer = 0;
  }

  /**
   * Intialize - called once at the beginning
   */
  StatusCode XMLFormatTool::initialize(){

    return StatusCode::SUCCESS;
  }

  /**
   * Finalize - called once at the end
   */
  StatusCode XMLFormatTool::finalize(){
    
    //Clear the event buffer if it exists
    if (m_EventBuffer) delete m_EventBuffer;
    return StatusCode::SUCCESS;
  }

  /**
   * Set the release tag
   * @param tag the name of the release
   */
  StatusCode XMLFormatTool::SetTag ( const TagType tag ){

    //Store this as release string
    m_release = tag.second;
    //Format properly
    while(m_release.find("/") != std::string::npos)
      m_release.replace(m_release.find("/"),1,"_");
    
    return StatusCode::SUCCESS; 
  }

  /**
   * Print the event header information
   * @param EventNumber the event number
   * @param RunNumber the run number
   * @param dataTime a date and time string
   */
  StatusCode XMLFormatTool::StartEvent(  const unsigned long EventNumber, const unsigned int RunNumber, std::string dateTime, 
					 const unsigned int lumiBlock, std::string eventProperty, std::string geometryVersion ){

    
    //Recreate the event buffer if it does not exists
    if (!m_EventBuffer)
      m_EventBuffer = new std::ostringstream();

    //Clear the event buffer
    m_EventBuffer->str("");

    //Print general XML header information
    (*m_EventBuffer) << "<?xml version=\"1.0\"?>" << std::endl
                     << "<?xml-stylesheet type=\"text/xsl\" href=\"JiveXML_event.xsl\"?>" << std::endl
                     << "<?ATLAS Release: \"" << m_release << "\"?>" << std::endl
                     << "<!DOCTYPE Event SYSTEM \"event.dtd\">"
                     << std::endl << std::endl << std::endl;

    //Print event open tag
    (*m_EventBuffer) << "<Event version=\"" << m_release << "\""
                     << " runNumber=\""   << RunNumber << "\""
                     << " eventNumber=\"" << EventNumber << "\""
                     << " lumiBlock=\""   << lumiBlock << "\""
                     << " dateTime=\"" << dateTime << "\""
                     << " geometryVersion=\""   << geometryVersion << "\""
                     << " eventProperty=\"" << eventProperty << "\""
                     << ">" << std::endl << std::endl << std::endl;

    return StatusCode::SUCCESS;
  }

  /**
   * Print event footer 
   */
  StatusCode XMLFormatTool::EndEvent(){

    //Print event close tag
    (*m_EventBuffer) << "</Event>\n";

    return StatusCode::SUCCESS;
  }


  /**
   * Add data from a certain component with a certain key to the event
   * @param component the component for which we write data
   * @param key the name of the data field we write
   * @param aMap the data that shall be written
   */
  StatusCode XMLFormatTool::AddToEvent(std::string component, std::string key, const DataMap * aMap) {
    
    DataMap::const_iterator itr;

    //check to see if any data is contained in the map before any formatting takes place
    bool map_is_empty = true;
    for(itr=aMap->begin(); itr!=aMap->end(); ++itr) {
      if (itr->second.size()) {
        map_is_empty = false;
        break;
      }
    }

    //return in case of empty map
    if(map_is_empty)  return StatusCode::SUCCESS;

    //Open a new tag
    (*m_EventBuffer) << "<" << component ;
 
    //Write number of entries taken from first non-multiple collection
    for (itr=aMap->begin(); itr!=aMap->end(); ++itr) {
      if ((itr->first.find("multiple"))==std::string::npos) {
        //Write out multiplicty and key
        (*m_EventBuffer) <<" count=\""<< itr->second.size()<< "\""
                       <<" storeGateKey=\""<< key<< "\"";
        break ;
      }
    }

    //Close the opening tag
    (*m_EventBuffer) << ">" << std::endl << std::endl;

    //Now start writing event information
    for(itr=aMap->begin(); itr!=aMap->end() ;++itr) {

      // Write first key from map
      (*m_EventBuffer) << "<" << (*itr).first <<">\n";
      
      // Write the elements in groups of 10 per line
      int ii=0;
      for (DataVect::const_iterator itr2 = (*itr).second.begin(); itr2!=(*itr).second.end(); ++itr2){
         ++ii;
         (*m_EventBuffer) << (*itr2);
         if ((*itr).first == "identifier"){
           (*m_EventBuffer) <<"\n";
         }
         else if(ii>=10){
           (*m_EventBuffer) <<"\n";
           ii=0;
         } else {
           (*m_EventBuffer) << " ";
         }
       }

       // I don't understand this bit
       int i = (*itr).first.find(" ");
       if (i != int(std::string::npos))
         (*m_EventBuffer) << "\n</" << (*itr).first.substr(0,i) <<">\n";
       else
         (*m_EventBuffer) << "\n</" << (*itr).first <<">\n";
    }
      
    /// Write a closing tag
    (*m_EventBuffer) << "</" << component << ">\n\n";
 
    return StatusCode::SUCCESS;
  }
 
  /// Return the formatted event
  const std::ostringstream* XMLFormatTool::getFormattedEvent() const {
    return m_EventBuffer;
  }


} //namespace















