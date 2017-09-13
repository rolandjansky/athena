/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopConfiguration/AodMetaDataAccess.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector> 
#include <string>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/scope_exit.hpp>

#include "PathResolver/PathResolver.h"

namespace top {


AodMetaDataAccess::AodMetaDataAccess() : m_data(nullptr) { }


AodMetaDataAccess::~AodMetaDataAccess() {
   delete m_data;
}


void AodMetaDataAccess::loadWithFilesFrom(std::string const & fileListPath) {

   // Implementation using PathResolver
   std::string filename = "TopConfiguration/AodMetaDataReader.py";
   // Use the path resolver to find the first file in the list of possible paths ($PYTHONPATH)
   std::string exePath = PathResolver::find_file(filename, "PYTHONPATH");

   if(exePath == ""){
     std::cout << "ERROR::AodMetaDataAccess - could not find file \n";
     std::cout << filename << "\n";
     exit(1);
   }
   std::cout << "AodMetaDataAccess::Found " << exePath << std::endl;


   int pipefd[2] = { -1, -1 };
   if (pipe(pipefd) == -1)
      throw std::runtime_error("pipe() failed");
   int pid = fork();
   if (pid == -1) {
      std::for_each(pipefd, pipefd+2, close);
      throw std::runtime_error("fork() failed");
   }

   if (pid == 0) {
      close(pipefd[0]);
      if (dup2(pipefd[1], STDOUT_FILENO) != -1)
         execlp(exePath.c_str(), "AodMetaDataReader.py", "--format", "plain", "--files-from", fileListPath.c_str(), nullptr);
      perror("popen(AodMetaDataReader.py)");
      _exit(1);
   }

   BOOST_SCOPE_EXIT(pid) {
      int status;
      while (waitpid(pid, &status, 0) == -1 && errno == EINTR);
   } BOOST_SCOPE_EXIT_END
   close(pipefd[1]);
   
   boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(pipefd[0], boost::iostreams::close_handle);

   auto * data = new std::map< std::string, std::map< std::string, std::string > >(); // FIXME memory leak on exception
   bool eof = false;
   for (std::string line; std::getline(stream, line); ) {
      if (eof)
         throw std::runtime_error("received data from AodMetaDataReader after end-of-stream marker");
      if (line.empty()) {
         eof = true;
      }
      else {
         std::vector<std::string> tokens;
         boost::split(tokens, line, boost::is_any_of("\t"));
         if (tokens.size() != 3)
            throw std::runtime_error("received malformed record from AodMetaDataReader");
         auto & folder = (*data)[tokens[0]];
         bool inserted = folder.insert(std::make_pair(tokens[1], tokens[2])).second;
         if (!inserted)
            throw std::runtime_error("received duplicate entry from AodMetaDataReader");
      }
   }
   if (!eof)
      throw std::runtime_error("data stream from AodMetaDataReader not properly terminated");
   m_data = data;
}


std::string AodMetaDataAccess::get(std::string const & folderName, std::string const & keyName) const {
   if (m_data == nullptr)
      throw std::logic_error("meta-data not loaded");
   auto it1 = m_data->find(folderName);
   if (it1 == m_data->end())
     throw std::logic_error("the folderName "+folderName+" does not exist in the metadata");
   auto const & folder = it1->second;
   auto it2 = folder.find(keyName);
   if (it2 == folder.end())
     throw std::logic_error("the keyName "+keyName+" does not exist in the metadata");

   return m_data->at(folderName).at(keyName);
}


std::string AodMetaDataAccess::get(std::string const & folderName, std::string const & keyName, std::string const & defaultValue) const {
   if (m_data == nullptr)
      throw std::logic_error("meta-data not loaded");
   auto it1 = m_data->find(folderName);
   if (it1 == m_data->end())
      return defaultValue;
   auto const & folder = it1->second;
   auto it2 = folder.find(keyName);
   if (it2 == folder.end())
      return defaultValue;
   return it2->second;
}


bool AodMetaDataAccess::valid() const {
   return (m_data != nullptr);
}


bool AodMetaDataAccess::isSimulation() const {
   auto projectName = get("/TagInfo", "project_name");
   if (boost::equals(projectName, "IS_SIMULATION"))
      return true;
   if (boost::starts_with(projectName, "data"))
      return false;
   throw std::invalid_argument("unrecognized value in meta-data entry for key 'project_name'");
}

bool AodMetaDataAccess::isAFII() const{
  if (!isSimulation()) return false; // No need to check if not MC
  auto simulatorName = get("/Simulation/Parameters","Simulator");
  if (boost::equals(simulatorName, "ATLFASTII")) // AFII will have this simulator
    return true;
  else return false; // Otherwise must be FullSim
}

}
