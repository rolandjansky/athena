/*
 * frontier client test C++ main program that can load any query
 * 
 * Author: Sergey Kosyakov
 *
 * $Id: TriggerFrontierClientTest.cxx 696788 2015-09-25 09:17:58Z czodrows $
 *
 * Copyright (c) 2009, FERMI NATIONAL ACCELERATOR LABORATORY
 * All rights reserved. 
 *
 * For details of the Fermitools (BSD) license see Fermilab-2009.txt or
 *  http://fermitools.fnal.gov/about/terms.html
 *
 */
#include "frontier_client/frontier-cpp.h"
#include "frontier_client/FrontierException.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>

#include <unistd.h>

static std::string escape_list="\\\'";
static std::string req_data="frontier_request:1:DEFAULT";

static void str_escape_quota(std::string *str) {
   std::string::size_type pos = 0;
   while(1) {
      pos=str->find_first_of(escape_list,pos);
      if(pos==str->npos) return;
      //std::cout<<"pos="<<pos<<'\n';
      str->insert(pos,1,'\\');    
      pos+=2;
   }
}

 
static void print_usage(char **argv) {
   std::cout<<"Usage: \n"<<argv[0]<<" -h\n\tPrint this info\n";
   std::cout<<"\n"<<argv[0]<<" [-r] [-n] [-c N] [-F N] [-f file_name]\n";
   std::cout<<"  if -f file_name is missing, reads query from stdin\n";
   std::cout<<"  [-r] means to force a reload\n";
   std::cout<<"  [-n] means do not print data\n";
   std::cout<<"  [-c N] repeat the query N count times\n";
   std::cout<<"  [-F N] fork after Nth repitition\n";
}
 


int main(int argc, char **argv) {
   //char vc;
   int vi;
   long long vl;
   float vf;
   double vd;
   std::string stringBuf;
   std::string *vs=&stringBuf;
   frontier::AnyData ad;
   char *file_name=0;
   int do_reload=0;
   int do_print=1;
   int repeat_count=1;
   int fork_count=0;
   int idx;
   std::string sql("");
  
   try {
      frontier::init();

      // check for -h
      for(int arg_ind=1; arg_ind<argc; arg_ind++)
         if(strcmp(argv[arg_ind],"-h")==0) {
            print_usage(argv);
            return 0;
         }
      

      for(int arg_ind=1; arg_ind<argc; arg_ind++) {

         const char* arg = argv[arg_ind];

         if(strcmp(arg,"-r")==0) {
            do_reload=1;
            continue;
         }
         
         if(strcmp(arg,"-n")==0) {
            do_print=0;
            continue;
         }

         if(strcmp(arg,"-c")==0) {
            if(argc==++arg_ind) {
               std::cerr << "No number specified after -c option\n";
               return 2;
            }
            sscanf(argv[arg_ind],"%d",&repeat_count);
            continue;
         }

         if(strcmp(arg,"-F")==0) {
            if(argc==++arg_ind) {
               std::cerr << "No number specified after -F option\n";
               return 2;
            }
            sscanf(argv[arg_ind],"%d",&fork_count);
            continue;
         }

         if(strcmp(arg,"-f")==0) {
            if(argc==++arg_ind) {
               std::cerr << "No file name specified after -f option\n";
               return 2;
            }
            file_name=argv[arg_ind];
            continue;
         }
      }
     
      std::ifstream in_file;
      if(file_name) {
         in_file.open(file_name);
         if(!in_file.is_open()) {
            std::cerr << "Can not open file " << file_name << '\n';
            return 2;
         }
      }

      while(1) {
         std::string tmp;      
         if(file_name) {
            if(!in_file.good()) break;
            std::getline(in_file,tmp,'\n');
         } else {
            if(!std::cin.good()) break;
            std::getline(std::cin,tmp,'\n');       
         }
         sql += tmp;
      }
      if(file_name) {in_file.close();}

      std::cout<<"Entered:\n"<<sql<<'\n';
    
      std::string param=frontier::Request::encodeParam(sql);
      std::cout<<"Param ["<<param<<"]\n";
          
      std::list<std::string> serverList;
      //serverList.push_back("http://lxfs6043.cern.ch:8080/Frontier3D");
      std::list<std::string> proxyList;
      //frontier::DataSource ds(serverList, proxyList);
      frontier::Connection con(serverList, proxyList);

      for(idx=0;idx<repeat_count;idx++) {
         if((fork_count>0)&&(idx==fork_count))
            fork();

         frontier::Session ses(&con);
         con.setReload(do_reload);

         frontier::Request req(req_data,frontier::BLOB);
         req.addKey("p1",param);

         std::vector<const frontier::Request*> vrq;
         vrq.push_back(&req);
         ses.getData(vrq);

         ses.setCurrentLoad(1);
      
         int field_num=0;
         
         std::cout<<"\nObject fields:\n";
         
         ses.next();
         // MetaData consists of one record with filed names.
         // Let's go over all fields:
         std::string name,type;
      
         while(!ses.isEOR()) {
            ses.assignString(&name);
            ses.assignString(&type);
            ++field_num;
            std::cout<<field_num<<" "<<(name)<<" "<<(type)<<std::endl;
         }


         // SECOND TIME
//          ses.setCurrentLoad(1);
//          field_num=0;
//          std::cout<<"\nObject fields:\n";
//          ses.next();
//          while(!ses.isEOR()) {
//             ses.assignString(&name);
//             ses.assignString(&type);
//             ++field_num;
//             std::cout<<field_num<<" "<<(name)<<" "<<(type)<<std::endl;
//          }
         // END SECOND TIME



	   
         int nrec=ses.getNumberOfRecords();
         std::cout<<"\nResult contains "<< nrec<<" objects.\n";
	  
         while(ses.next()) {
            if(!do_print)continue;
            for(int k=0;k<field_num;k++) {
               ses.getAnyData(&ad);
               switch(ad.type()) {
                  //case frontier::BLOB_TYPE_BYTE:       vc=ses.getByte(); break;
               case frontier::BLOB_TYPE_INT4:       
                  vi=ad.getInt(); 
                  std::cout<<vi; 
                  break;
               case frontier::BLOB_TYPE_INT8:       
                  vl=ad.getLongLong(); 
                  std::cout<<vl; 
                  break;
               case frontier::BLOB_TYPE_FLOAT:      
                  vf=ad.getFloat(); 
                  std::cout<<vf; 
                  break;
               case frontier::BLOB_TYPE_DOUBLE:     
                  vd=ad.getDouble(); 
                  std::cout<<vd; 
                  break;
               case frontier::BLOB_TYPE_TIME:       
                  vl=ad.getLongLong(); 
                  std::cout<<vl; 
                  break;
               case frontier::BLOB_TYPE_ARRAY_BYTE: 
                  if(!ad.getRawStrP()) {
                     std::cout<<"NULL";
                  }
                  else if (ad.getRawStrS() == 0)
                     std::cout<<"''"; 
                  else if (ad.getRawStrS() > 1000)
                     std::cout<<'('<<ad.getRawStrS()<<" byte blob)"; 
                  else {
                     vs=ad.getString(); 
                     str_escape_quota(vs);
                     std::cout<<'\''<<(*vs)<<'\''<<'('<<ad.getRawStrS()<<')'; 
                  }
                  break;	  
               default: 
                  std::cout<<"Error: unknown typeId "<<((int)(ad.type()))<<"\n"; 
                  exit(1);
               }
               if(k+1<field_num) {
                  std::cout<<" ";
               }
               ad.clean();
            }
            ad.clean();
            std::cout<<std::endl;
         }
         if(!ses.isEOF()) {
            std::cout<<"Error: must be EOF here\n";
            exit(1);
         }
      }
   }
   catch(const frontier::ConfigurationError& e) {
      std::cout << "Frontier configuration error caught: " << e.what() << std::endl;
      exit(1);
   }
   catch(const frontier::FrontierException& e) {
      std::cout << "Frontier exception caught: " << e.what() << std::endl;
      exit(1);
   }
   catch(std::exception& e) {
      std::cout << "Error: " << e.what() << "\n";
      exit(1);
   }
   catch(...) {
      std::cout << "Unknown exception\n";
      exit(1);
   }

   return 0;
}
