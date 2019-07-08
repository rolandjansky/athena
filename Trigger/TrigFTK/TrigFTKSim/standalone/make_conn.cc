#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include <TString.h>

#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <boost/iostreams/device/file.hpp>
//#include <boost/iostreams/concepts.hpp>
//#include <boost/iostreams/categories.hpp>
//#include <boost/algorithm/string/classification.hpp>
//#include <boost/filesystem.hpp> 
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

using namespace std;

class SSIDvector : public vector<int> {
public:
   SSIDvector(int n=0) : vector<int>(n) { }
   int operator<(const SSIDvector &b) const {
      if(size()<b.size()) return true;
      if(size()>b.size()) return false;
      for(size_t n=0;n<size();n++) {
         if((*this)[n]<b[n]) return true;
         if((*this)[n]>b[n]) return false;
      }
      return false;
   }
};

typedef map<SSIDvector,pair<int,vector<vector<int> > > >  ConnectionMap_t;

int main(int argc, char const *argv[]) {
   if(argc!=2) {
      cout<<"usage: "<<argv[0]<<" path\n";
      exit(0);
   }
   TString path(argv[1]);
   //
   // locate 8L sector files
   glob_t globList;
   struct stat statBuf;
   memset(&globList,0,sizeof(glob_t));
   TString sector8Lpath=path+"/sectors_raw_8L_reg*";
   int err=glob(sector8Lpath,GLOB_TILDE,0,&globList);
   if(err) {
      cout<<"Error: can not glob "<<sector8Lpath<<"\n";
      exit(1);
   }
   int nTowerMax=globList.gl_pathc;
   for(int tower=0;tower<nTowerMax;tower++) {
      sector8Lpath=path+TString::Format("/sectors_raw_8L_reg%d.patt*",tower);
      memset(&globList,0,sizeof(glob_t));
      err=glob(sector8Lpath,GLOB_TILDE,0,&globList);
      if(err) {
         cout<<"Can not glob "<<sector8Lpath<<", skipping\n";
         continue;
      }
      TString sectorFile[2];
      sectorFile[0]=globList.gl_pathv[0];
      TString sector12Lpath=path+
         TString::Format("/sectors_raw_12L_reg%d.patt*",tower);
      memset(&globList,0,sizeof(glob_t));
      err=glob(sector12Lpath,GLOB_TILDE,0,&globList);
      if(err) {
         cout<<"Can not glob "<<sector12Lpath<<", skipping\n";
         continue;
      }
      sectorFile[1]=globList.gl_pathv[0];
      int error=0;
      boost::iostreams::filtering_istream *sectorStream[2];
      for(int i=0;i<2;i++) {
         if(stat(sectorFile[i],&statBuf)) {
            cout<<"Can not stat "<<sectorFile[i]<<", skipping\n";
            error++;
         }
         if(!S_ISREG(statBuf.st_mode)) {
            cout<<"Not a regular file, skipping: "
                <<sectorFile[i]<<"\n";
            error++;
         }
         sectorStream[i]=new boost::iostreams::filtering_istream;
         if(sectorFile[i].EndsWith(".bz2")) {
            sectorStream[i]->push(boost::iostreams::bzip2_decompressor());
         }
         sectorStream[i]->push(boost::iostreams::file_source(sectorFile[i].Data()));
         cout<<"open "<<sectorFile[i]<<"\n";
      }
      if(error) continue;
      // read sector files
      
      vector<set<int> > moduleId;
      ConnectionMap_t connections; 
      int notFound=0;
      int found=0;
      for(int iType=0;iType<2;iType++) {
         int nSector,nPlane;
         (*sectorStream[iType])>>nSector>>nPlane;
         if(iType==0) moduleId.resize(nPlane);
         SSIDvector ssid(nPlane);
         for(int iSector=0;iSector<nSector;iSector++) {
            // read sector data
            int sectorID;
            (*sectorStream[iType])>>sectorID;
            //cout<<sectorID;
            if(sectorID!=iSector) {
               cout<<"error reading sector "<<iSector
                   <<" "<<sectorID<<"\n";
               exit(0);
            }
            // read module IDs
            for(int plane=0;plane<nPlane;plane++) {
               (*sectorStream[iType])>>ssid[plane];
               //cout<<" "<<ssid[plane];
            }
            // dummy
            int dummy,nTrack;
            (*sectorStream[iType])>>dummy>>nTrack;
            //cout<<" "<<dummy<<" "<<nTrack<<"\n";
            //if(sectorID>20) exit(0);
            SSIDvector ssid8L(moduleId.size());
            if(iType==0) {
               // if 8L: store known module IDs and store 8L SSID
               for(int plane=0;plane<nPlane;plane++) {
                  moduleId[plane].insert(ssid[plane]);
                  ssid8L[plane]=ssid[plane];
               }
               connections[ssid8L].first=iSector;
            } else {
               // if 12L:  locate known planes, skip others
               vector<int> connId(nPlane-moduleId.size()+2);
               connId[1]=iSector;
               size_t nExtra=2;
               size_t plane8=0;
               for(int plane=0;plane<nPlane;plane++) {
                  if((plane8<moduleId.size())&&
                     (moduleId[plane8].find(ssid[plane])!=
                      moduleId[plane8].end())) {
                     //cout<<" "<<ssid[plane]<<" "<<plane<<" -> "<<plane8<<"\n";
                     ssid8L[plane8++]=ssid[plane];
                  } else {
                     if(nExtra>=connId.size()) {
                        cout<<"12L sector="<<iSector<<" too few planes match\n";
                        exit(0);
                     }
                     //cout<<" "<<ssid[plane]<<" extra="<<nExtra<<"\n";
                     connId[nExtra++]=ssid[plane];
                  }
               }
               //exit(0);
               ConnectionMap_t::iterator ic=connections.find(ssid8L);

               /* cout<<" 8L:";
                  for(int k=0;k<ssid8L.size();k++) {
                     cout<<" "<<ssid8L[k];
                  }
                  cout<<" extra ";
                  for(int k=0;k<connId.size();k++) {
                     cout<<" "<<connId[k];
                  }
                  cout<<"\n";
               */
               if(ic==connections.end()) {
                  notFound++;
                  //cout<<"Number of sectors: "<<connections.size()<<"\n";
                  //cout<<"12L sector="<<iSector<<" not found, skipping\n";
                  /* for(ConnectionMap_t::const_iterator ic=connections.begin();
                      ic!=connections.end();ic++) {
                     if((*ic).second.first<20) {
                        cout<<(*ic).second.first;
                        for(int k=0;k<(*ic).first.size();k++) {
                           cout<<" "<<(*ic).first[k];
                        }
                        cout<<"\n";
                     }
                  }
                  exit(0); */
               } else {
                  found++;
                  vector<vector<int> > &conn12=(*ic).second.second;
                  conn12.push_back(connId);
               }
            }
         }
         if(iType==0) {
            cout<<"nSector="<<nSector<<" nPlane="<<nPlane
                <<" Number of sectors: "<<connections.size()<<"\n";
            for(size_t plane=0;plane< moduleId.size();plane++) {
               cout<<" 8L["<<plane<<"] n(mod)="<<moduleId[plane].size();
            }
            cout<<"\n";
         } else {
            cout<<"nSector="<<nSector<<" nPlane="<<nPlane
                <<" notFound="<<notFound<<" found="<<found<<"\n";
            // order connections by sector id, then print
            map<int,ConnectionMap_t::const_iterator> orderedConnections;
            for(ConnectionMap_t::const_iterator ic=connections.begin();
                ic!=connections.end();ic++) {
               orderedConnections[(*ic).second.first]=ic;
            }
            TString outname(TString::Format("sectors_raw_8L_reg%d.conn",tower));
            ofstream output(outname);
            int sectorID12=0;
            for(map<int,ConnectionMap_t::const_iterator>::const_iterator
                   oic=orderedConnections.begin();
                oic!=orderedConnections.end();oic++) {
               vector<vector<int> > const &conn12=(*(*oic).second).second.second;
               output<<(*oic).first<<" "<<conn12.size();
               for(size_t k=0;k<conn12.size();k++) {
                  output<<" 0 "<<sectorID12;
                  sectorID12++;
                  for(size_t l=2;l<conn12[k].size();l++) {
                     output<<" "<<conn12[k][l];
                  }
               }
               output<<"\n";
            }
         }
      }

      for(int i=0;i<2;i++) {
         delete sectorStream[i];
      }
   }
}
