/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MemoryMonitor.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"

using namespace rapidjson;

int ReadSmaps(pid_t mother_pid, unsigned long values[4], bool verbose){
      std::vector<pid_t> cpids;
      char smaps_buffer[32];
      snprintf(smaps_buffer,32,"pstree -A -p %ld | tr \\- \\\\n",(long)mother_pid);
      FILE* pipe = popen(smaps_buffer, "r");
      if (pipe==0) {
	if (verbose) 
	  std::cerr << "MemoryMonitor: unable to open pstree pipe!" << std::endl;
        return 1;
      }
	
      char buffer[256];
      std::string result = "";
      while(!feof(pipe)) {
        if(fgets(buffer, 256, pipe) != NULL) {
          result += buffer;
          int pos(0);
          while(pos<256 && buffer[pos] != '\n' && buffer[pos] != '(') {
            pos++;}
          if(pos<256 && buffer[pos] == '(' && pos>1 && buffer[pos-1] != '}') {
            pos++;
            pid_t pt(0);
            while(pos<256 && buffer[pos] != '\n' && buffer[pos] != ')') {
              pt=10*pt+buffer[pos]-'0';
              pos++;}
            cpids.push_back(pt);} } } 
      pclose(pipe);

      long tsize(0);
      long trss(0);
      long tpss(0);
      long tswap(0);
      std::vector<std::string> openFails;

      for(std::vector<pid_t>::const_iterator it=cpids.begin(); it!=cpids.end(); ++it) {
        snprintf(smaps_buffer,32,"/proc/%ld/smaps",(long)*it);
       
        FILE *file = fopen(smaps_buffer,"r");
        if(file==0) {
          openFails.push_back(std::string(smaps_buffer));} 
        else { 
          while(fgets(buffer,256,file)) {
            if(sscanf(buffer,"Size: %80ld kB",&tsize)==1) values[0]+=tsize;
            if(sscanf(buffer,"Pss: %80ld kB", &tpss)==1)  values[1]+=tpss;
            if(sscanf(buffer,"Rss: %80ld kB", &trss)==1)  values[2]+=trss;
            if(sscanf(buffer,"Swap: %80ld kB",&tswap)==1) values[3]+=tswap; } 
          fclose(file);} } 
       if(openFails.size()>3 && verbose) {
         std::cerr << "MemoryMonitor: too many failures in opening smaps files!" << std::endl;
         return 1; }
     return 0;
   }

std::condition_variable cv;
std::mutex cv_m;
bool sigusr1 = false;

void SignalCallbackHandler(int /*signal*/) { std::unique_lock<std::mutex> l(cv_m); sigusr1 = true; cv.notify_one(); }

int MemoryMonitor(pid_t mpid, char* filename, char* jsonSummary, unsigned int interval){
     
     signal(SIGUSR1, SignalCallbackHandler);

     unsigned long values[4] = {0,0,0,0};
     unsigned long maxValues[4] = {0,0,0,0};
     unsigned long avgValues[4] = {0,0,0,0};
     int iteration = 0;
     time_t lastIteration = time(0) - interval;
     time_t currentTime;

     // Open iteration output file     
     std::ofstream file;  
     file.open(filename);
     file << "Time\t\t\t\tVMEM\tPSS\tRSS\tSwap" << std::endl;

     const char json[] = "{\"Max\": {\"maxVMEM\": 0, \"maxPSS\": 0,\"maxRSS\": 0, \"maxSwap\": 0}, \"Avg\": { \"avgVMEM\": 0,\"avgPSS\": 0, \"avgRSS\": 0, \"avgSwap\": 0}}";
     Document d;
     d.Parse(json);
     std::ofstream file2; // for realtime json dict
     StringBuffer buffer;
     Writer<StringBuffer> writer(buffer);

     std::stringstream tmpFile;
     tmpFile << jsonSummary << "_tmp" ;
     std::stringstream newFile;
     newFile << jsonSummary << "_snapshot";

     int tmp = 0;
     Value& v1 = d["Max"];
     Value& v2 = d["Avg"];
     Value::MemberIterator i1;
     Value::MemberIterator i2;

     // Monitoring loop until process exits
     while(kill(mpid, 0) == 0 && sigusr1 == false){

        bool wroteFile = false;
        if (time(0) - lastIteration > interval){         
 
          iteration = iteration + 1;
          ReadSmaps( mpid, values);

          currentTime = time(0);
          file << currentTime << "\t" << values[0]<< "\t" << values[1] << "\t" << values[2] << "\t" << values[3] << std::endl;  

          // Compute statistics
          for(int i=0;i<4;i++){
             avgValues[i] = avgValues[i] + values[i];
             if (values[i] > maxValues[i])
               maxValues[i] = values[i];
             values[i] = 0;
             lastIteration = time(0);}

          // Reset buffer
          buffer.Clear();
          writer.Reset(buffer);

          // Create JSON realtime summary
          for (std::pair<Value::MemberIterator, Value::MemberIterator> i= std::make_pair(v1.MemberBegin(), v2.MemberBegin()); i.first != v1.MemberEnd() && i.second != v2.MemberEnd(); ++i.first, ++i.second){
             i.first->value.SetUint(maxValues[tmp]);
             i.second->value.SetUint(avgValues[tmp]/iteration);
             tmp += 1;}
          tmp = 0;

          // Write JSON realtime summary to a temporary file (to avoid race conditions with pilot trying to read from file at the same time)
          d.Accept(writer);
          file2.open(tmpFile.str());
          file2 << buffer.GetString() << std::endl;
          file2.close();
          wroteFile = true;
       }
  
       // Move temporary file to new file
       if (wroteFile) {
         if (rename(tmpFile.str().c_str(), newFile.str().c_str()) != 0) {
           perror ("rename fails");
           std::cerr << tmpFile.str() << " " << newFile.str() << "\n";
         }
       }
       
       std::unique_lock<std::mutex> lock(cv_m); 
       cv.wait_for(lock, std::chrono::seconds(1));     
   }
   file.close();

   // Cleanup
   if (remove(newFile.str().c_str()) != 0)
     perror ("remove fails");

   // Write final JSON summary file
   file.open(jsonSummary);
   file << buffer.GetString() << std::endl;
   file.close();

   return 0;
 }

int main(int argc, char *argv[]){

    if(argc != 9) { 
        std::cerr << "Usage: " << argv[0] << " --pid --filename --json-summary --interval \n " <<  std::endl;
        return -1;}

    pid_t pid=-1; char* filename = NULL; char* jsonSummary=NULL; int interval = 600;

    for (int i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "--pid") == 0) pid = atoi(argv[i+1]); 
      else if (strcmp(argv[i],"--filename") == 0) filename = argv[i+1];
      else if (strcmp(argv[i],"--json-summary") == 0) jsonSummary = argv[i+1];
      else if (strcmp(argv[i], "--interval") == 0) interval = atoi(argv[i+1]);
    }

    if (pid < 2) {
      std::cerr << "Bad PID.\n";
      return 1;
    }

    if (!jsonSummary) {
      std::cerr << "--json-summary switch missing.\n";
      return 1;
    }

    MemoryMonitor(pid, filename, jsonSummary, interval);

    return 0;
}


