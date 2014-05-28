#include<iostream.h>
#include<vector.h>
#include<string.h>

std::vector<std::string> inspect(){
     std::string pwd(gDirectory->GetPath());
     gDirectory->cd(); // go home
     std::vector<std::string> s = traversedir( gDirectory );
     gDirectory->cd( pwd.c_str()); 
     return s;
 }

std::vector<std::string> traversedir( TDirectory *dir) {
     std::vector<std::string> histograms;
     TIter nextkey( dir->GetListOfKeys() );
     TKey *key, *oldkey=0;
     while ( (key = (TKey*)nextkey() )) {
         TObject *obj = key->ReadObj();
         if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
             std::string fullname(gDirectory->GetPath());
	     fullname +="/";
             fullname += obj->GetName();
             histograms.push_back(fullname);
	     std::cout<<fullname<<std::endl;
         } else if ( obj->IsA()->InheritsFrom( "TDirectory" ) ) {
             std::string pwdd(gDirectory->GetPath());
		gDirectory->cd(obj->GetName());
             std::vector<std::string> r = traversedir(gDirectory);
             // append it to the list
             gDirectory->cd(pwdd.c_str());
             //copy(r.begin(), r.end(), histograms.end());
         }
     }
     return histograms;
 }
