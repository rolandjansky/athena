/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Author Marco Vanadia vanadiam@roma1.infn.it
//Class based on algorithms introduced by Philipp Fleischmann in GnaMon

//Be careful that in order to preserve a chamber-like structure, the first tube in a layer is tube #1
//the first layer in a ML is layer#1, the first ML in a chamber is ML #1, but the first mezzanine in a 
//ML is mezzanine #0. So to get the first tube in a layer you've got to use the method Layer::getTube(1),
//to get the first Mezzanine in a ML you've got to use the method Multilayer::getMezzanine(0);
//further more, loops involving tubes, layers and multilayer must have ending conditions like 
// i<=Ntubes, while loops involving Mezzanines must have ending conditions like i<Nmezzanines

//In chambers with ML1 shorter than ML2,
//empty spaces are treated just like tubes, but a special status value NOTUBE is assigned to them,
//and they are ignored in the rest of the analysis. In this way the real tubes are assigned to the 
//right positions in the layer.
//So the method Layer::getNtubes() returns the number of both real and fake tubes, that is the right number
//to use in loops; if you want the number of REAL tubes in layer, use Layer::getNrealtubes();

//#include "./MDTDqaDeadElements.h"
#include "MuonCalibStandAloneExtraTools/MDTDqaDeadElements.h"

using namespace std;

//Status values conventions

#define NOTUBE 0           //Used for chambers with ML1 smaller than ML2 
#define DEADCHAMBER 1
#define LOWSTATISTICSCHAMBER 7
#define DEADML 2
#define INEFFICIENTML 2
#define DEADLAYER 3
#define NOISYLAYER 8
#define INEFFICIENTLAYER 3
#define DEADMEZZANINE 4
#define NOISYMEZZANINE 8
#define LOWSTATMEZZANINE 7
#define INEFFICIENTMEZZANINE 10
#define DEADTUBE 5
#define NOISYTUBE 8

namespace MuonCalib {

///////////////////////////////////////////////////////////////////////////////////////////////////////
  //methods of class Tube

  Tube::Tube(int the_status, int the_position, Layer *the_layer) {
    m_status=the_status;
    m_position=the_position;
    m_value=-999;      //Arbitrary, it makes easier to find tubes which were not filled
    m_parent_layer=the_layer;
    m_layer=-1;
    m_ml=-1;
  }

  int Tube::getStatus() {
    return m_status;
  }
  int Tube::getValue() {
    return m_value;
  }
  int Tube::getPosition() {
    return m_position;
  }
  int Tube::getLayer() {
    return m_layer;
  }
  int Tube::getML() {
    return m_ml;
  }
  void Tube::setStatus(int the_status) {
    m_status=the_status;
  }
  void Tube::setValue(int the_value) {
    m_value=the_value;
  }
  void Tube::setPosition(int the_position) {
    m_position=the_position;
  }
  Layer* Tube::getParentLayer() {
    return m_parent_layer;
  }
  void Tube::setLayer(int the_layer) {
    m_layer=the_layer;
  }
  void Tube::setML(int the_ml) {
    m_ml=the_ml;
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////
  //methods of class Layer

  Layer::Layer() {
    m_ntubes=0;
    m_entries=0;
    m_status=10;
    m_median=-1;
    m_median_deviation=-1;
  }

  Tube* Layer::addTube(int the_status, int the_position) {
    Tube *t = new Tube(the_status, the_position, this);
    m_tubes.push_back(t);
    m_ntubes++;
    return t;
  }

  Tube* Layer::getTube(int the_position) {
    return m_tubes[the_position-1];
  }

  int Layer::getNtubes() {
    return m_ntubes;
  }

  double Layer::getEntries() {
    return m_entries;
  }

 int Layer::getStatus() {
    return m_status;
  }

  double Layer::getMedian() {
    return m_median;
  }

  double Layer::getMedianDeviation() {
    return m_median_deviation;
  }

  void Layer::setStatus(int the_status) {
    m_status=the_status;
    for(int i=1; i<=getNtubes(); i++) {
      Tube *t=getTube(i);
      if(t->getStatus()==0) continue;
      t->setStatus(the_status);
    }
  }

  void Layer::addEntries(double the_entries) {
    m_entries+=the_entries;
  } 

  void Layer::calculateStatistics() {
    //calculateStatistics only calculate median and median deviation of the object, not of its sub-objects
    //to calculate these values for the whole chamber and its sub-parts, use the method MDTChamber::updateStatistics()

    vector<double> values;
 
    for(int k=1; k<=this->getNtubes(); k++) {
      Tube *t= getTube(k);
      if((t->getStatus())!=10) continue;
      values.push_back(t->getValue());

    }
    //Calculate median
    int size=values.size();
    if(size==0) return;
    sort(values.begin(), values.end());
    if((size%2)==1) m_median=values[(size-1)/2];
    else m_median=(double)(values[size/2]+values[(size/2)-1])/2.;

    //Calculate median absolute deviation

    for(int i=0; i<size; i++) values[i]=std::abs(values[i]-m_median);

    sort(values.begin(), values.end());
    
    if((size%2)==1) m_median_deviation=values[(size-1)/2];
    else m_median_deviation=(double)(values[size/2]+values[(size/2)-1])/2.;

    return; 
  }

  int Layer::getNrealtubes() {
    int ntubes=0;
    for(int i=1; i<=getNtubes(); i++) {
      Tube *t=getTube(i);
      if(t->getStatus()==0) continue;
      ntubes++;
    }
    return ntubes;
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //methods of class Mezzanine

  Mezzanine::Mezzanine() {
    m_entries=0;
    m_ntubes=0;
    m_status=10;
    m_median=-1;
    m_median_deviation=-1;
    m_70min=-999;
    m_70max=-999;
  }
  void Mezzanine::addTube(Tube *t) {
    m_tubes.push_back(t);
    m_ntubes++;
  }
  Tube* Mezzanine::getTube(int the_position) {
    return m_tubes[the_position-1];
  }

  int Mezzanine::getNtubes() {
    return m_ntubes;
  }
  int Mezzanine::get70min() {
    return m_70min;
  }
  int Mezzanine::get70max() {
    return m_70max;
  }

  double Mezzanine::getEntries() {
    return m_entries;
  }

  int Mezzanine::getStatus() {
    return m_status;
  }

  double Mezzanine::getMedian() {
    return m_median;
  }

  double Mezzanine::getMedianDeviation() {
    return m_median_deviation;
  }

  void Mezzanine::addEntries(double the_entries) {
    m_entries+=the_entries;
  }

  void Mezzanine::setStatus(int the_status) {
    m_status=the_status;
    for(int i=1; i<=getNtubes(); i++) {
      Tube *t=getTube(i);
      if(t->getStatus()==0) continue;
      t->setStatus(the_status);
    }
  }

  void Mezzanine::calculateStatistics() {
    //calculateStatistics only calculate median and median deviation of the object, not of its sub-objects
    //to calculate these values for the whole chamber and its sub-parts, use the method MDTChamber::updateStatistics()

    //Here we must also compute the entries in mezzanine, because of the way we built it

    vector<double> values;
    for(int k=1; k<=this->getNtubes(); k++) {
      Tube *t= getTube(k);
      if((t->getStatus())==0) continue; //Double check just not to lose entries;
      this->addEntries(t->getValue());
      if((t->getStatus())!=10) continue;
      values.push_back(t->getValue());
    }
    //Calculate median

    int size=values.size();
    if(size==0) return;
    sort(values.begin(), values.end());

    m_70min=(int)values[(int)(3.0*(double)size/10.)];
    m_70max=(int)values[(int)(7.0*((double)size/10.))];

    if((size%2)==1) m_median=values[(size-1)/2];
    else m_median=(double)(values[size/2]+values[(size/2)-1])/2.;

    //Calculate median absolute deviation

    for(int i=0; i<size; i++) values[i]=std::abs(values[i]-m_median);

    sort(values.begin(), values.end());
    
    if((size%2)==1) m_median_deviation=values[(size-1)/2];
    else m_median_deviation=(double)(values[size/2]+values[(size/2)-1])/2.;

    return; 
  }  //end Mezzanine::calculateStatistics

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 //methods of class Multilayer

  Multilayer::Multilayer() {
    m_nlayers=0;
    m_entries=0;
    m_nmezzanines=0;
    m_status=10;
    m_median=-1;
    m_median_deviation=-1;
  }

  void Multilayer::addLayer() {
    Layer *l= new Layer();
    m_layers.push_back(l);
    m_nlayers++;
  }

  Tube* Multilayer::addTube(int the_layer, int the_status, int the_position) {
    while(the_layer>getNlayers()) addLayer();
    return (getLayer(the_layer))->addTube(the_status, the_position);
  }
  Layer* Multilayer::getLayer(int the_position) {
    return m_layers[the_position-1];
  }

  Mezzanine* Multilayer::addMezzanine() {
    Mezzanine *mezz=new Mezzanine();
    m_mezzanines.push_back(mezz);
    m_nmezzanines++;
    return mezz;
  }

  Mezzanine* Multilayer::getMezzanine(int pos) {
    if(pos>=static_cast<int>(m_mezzanines.size())) return NULL;
    return m_mezzanines[pos];
  }

  int Multilayer::getNlayers() {
    return m_nlayers;
  }
  int Multilayer::getNmezzanines() {
    return m_nmezzanines;
  }
  double Multilayer::getEntries() {
    return m_entries;
  }
  int Multilayer::getStatus() {
    return m_status;
  }
  double Multilayer::getMedian() {
    return m_median;
  }
  double Multilayer::getMedianDeviation() {
    return m_median_deviation;
  }

  void Multilayer::setStatus(int the_status) {
    m_status=the_status;
    for(int i=1; i<=getNlayers(); i++) {
      Layer *l=getLayer(i);
      l->setStatus(the_status);
    }
    for(int i=0; i<getNmezzanines(); i++) {
      Mezzanine *mezz=getMezzanine(i);
      mezz->setStatus(the_status);
    }
    
  }

  void Multilayer::addEntries(double the_entries) {
    m_entries+=the_entries;
  }

  void Multilayer::calculateStatistics() {
    //calculateStatistics only calculate median and median deviation of the object, not of its sub-objects
    //to calculate these values for the whole chamber and its sub-parts, use the method MDTChamber::updateStatistics()

    vector<double> values;

    for(int j=1; j<=getNlayers(); j++) {
      Layer *l=getLayer(j);
      for(int k=1; k<=l->getNtubes(); k++) {
	Tube *t= l->getTube(k);
	if((t->getStatus())!=10) continue;
	values.push_back(t->getValue());
      }
    }
    //Calculate median

    int size=values.size();
    if(size==0) return;
    sort(values.begin(), values.end());
    if((size%2)==1) m_median=values[(size-1)/2];
    else m_median=(double)(values[size/2]+values[(size/2)-1])/2.;

    //Calculate median absolute deviation

    for(int i=0; i<size; i++) values[i]=std::abs(values[i]-m_median);

    sort(values.begin(), values.end());
    
    if((size%2)==1) m_median_deviation=values[(size-1)/2];
    else m_median_deviation=(double)(values[size/2]+values[(size/2)-1])/2.;

    return; 
  }  //end Multilayer::calculateStatistics

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods of Class MDTChamber

//This method reads the reference histograms and builds the chamber with the correct geometry

  MDTChamber::MDTChamber(TH1 *href, TString the_name) : 
    m_mean(-999.), m_standard_deviation(-999.), m_90min(-1), m_90max(-1), m_70(-1) {
    m_nmultilayers=0;
    m_entries=0;
    m_status=10;
    m_name=the_name;
    m_median=-1;
    m_median_deviation=-1;

    //Now we use a complicated strategy to read the Reference histogram in the correct way;
    //the main difficulty is in chambers that have layers smaller than others, so we have to understand
    //whether an empty space must be assigned to the end of the previous layer or to the beginning of the
    //next one.
    //Empty spaces are treated just like tubes, but a special status value NOTUBE is assigned to them,
    //and they are ignored in the rest of the analysis. In this way the real tubes are assigned to the 
    //right positions in the layers.

    int the_multilayer=1;
    int the_layer=1;

    int nbins=href->GetNbinsX();

    int old_layer=the_layer;
    bool holesx=false;  
    int pos=1;
    bool newml=true;
    bool afterfirstempty=false;
    int lastbin=-1;
   
    for(int bin=1; bin<=nbins; bin++) {
      int bincontent=(int)href->GetBinContent(bin);
      if((bincontent==0)&&(bin==1)) holesx=true;
      if((bincontent!=0)&&(!afterfirstempty))afterfirstempty=true;
      if((lastbin!=0)&&(bincontent==0)&&holesx) pos=1;
      lastbin=bincontent;

      if(bincontent==0) {
	    if(holesx==true) {
	      if(afterfirstempty) {
		addTube(the_multilayer,the_layer+1,NOTUBE,pos++);
		old_layer=the_layer+1;
	      }	else {
		addTube(the_multilayer,the_layer,NOTUBE,pos++);
	      }
	      continue;
	    } else {
	      addTube(the_multilayer,the_layer, NOTUBE, pos++);
	      continue;
	    }
      }
      if((bincontent>0)&&(newml)) {
	the_multilayer=2;
	pos=1;
	newml=false;
	holesx=false;
      }
      bincontent=std::abs(bincontent);
      the_layer=(int)floor(((double)bincontent)/10.);
      if(old_layer!=the_layer) {
	pos=1;
      }

      int the_status;
      if((bincontent%10)==0) the_status=10;
      else the_status=bincontent%10;
      addTube(the_multilayer, the_layer, the_status, pos++);
      old_layer=the_layer;
    }
    //Here we build the mezzanine structure of the chamber
    buildMezzanines();
  }
  //end MDTChamber::MDTChamber

  void MDTChamber::addMultilayer() {
    Multilayer *ml=new Multilayer();
    m_multilayers.push_back(ml);
    m_nmultilayers++;
  }
  Multilayer* MDTChamber::getMultilayer(int the_position) {
    return m_multilayers[the_position-1];
  }
  Tube* MDTChamber::addTube(int the_multilayer, int the_layer, int the_status, int the_position) {
    while(the_multilayer>getNmultilayers()) addMultilayer();
    Tube *t= getMultilayer(the_multilayer)->addTube(the_layer, the_status, the_position);

    if(t!=NULL) {
      t->setLayer(the_layer);
      t->setML(the_multilayer);
    }
    return t;
  }

  int MDTChamber::getNmultilayers() {
    return m_nmultilayers;
  }

  double MDTChamber::getEntries() {
    return m_entries;
  }

  int MDTChamber::getStatus() {
    return m_status;
  }

  double MDTChamber::getMedian() {
    return m_median;
  }

  double MDTChamber::getMedianDeviation() {
    return m_median_deviation;
  }

  double MDTChamber::getMean() {
    return m_mean;
  }

  double MDTChamber::getStandardDeviation() {
    return m_standard_deviation;
  }

  TString MDTChamber::getName() {
    return m_name;
  }

  // These three values are explained later in the tube analysis
  int MDTChamber::get90min() {
    return m_90min;
  }

  int MDTChamber::get90max() {
    return m_90max;
  }

  int MDTChamber::get70() {
    return m_70;
  }

  void MDTChamber::setStatus(int the_status) {
    m_status=the_status;
    for(int i=1; i<=getNmultilayers(); i++) {
      Multilayer *ml=getMultilayer(i);
      ml->setStatus(the_status);
    }
  }

  void MDTChamber::addEntries(double the_entries) {
    m_entries+=the_entries;
  }

  void MDTChamber::buildMezzanines() {
    int ntubespermezzanine=0;
    int mezzanine=0;
    
    for(int i=1; i<=getNmultilayers(); i++) {
      Multilayer *ml=getMultilayer(i);
      for(int j=1; j<=ml->getNlayers();j++) {
	if(ml->getNlayers()==3) ntubespermezzanine=8;
	if(ml->getNlayers()==4) ntubespermezzanine=6;
	if((i==1)&&(getName().Contains("BIR4"))) ntubespermezzanine=3;  
								   //BIR wiht eta=4 are the only chambers with the 
	Layer *l=ml->getLayer(j);				   //FIRST mezzanine made off rows of only 4 tubes	
	int tubeinmezzanine=0;
	mezzanine=0;

	for(int k=1; k<=l->getNtubes();k++) {
	  Tube *t=l->getTube(k);
	  if((t->getStatus()==0)) continue;
	  while((mezzanine+1)>ml->getNmezzanines()) ml->addMezzanine();
	  Mezzanine *mezz=ml->getMezzanine(mezzanine);
	  mezz->addTube(t);
	  //DEBUG
	  //t->setStatus(mezzanine+1);
	  //
	  tubeinmezzanine++;
	  if(tubeinmezzanine==ntubespermezzanine) {
	    mezzanine++;
	    tubeinmezzanine=0;
	    if(getName().Contains("BIR4")) ntubespermezzanine=6;  //Same as before
	  }
          
	}
      }
    }

    return;
  }  //end MDTChamber::buildMezzanines

  void MDTChamber::calculateStatistics() {
    //calculateStatistics only calculate median and median deviation of the object, not of its sub-objects
    //to calculate these values for the whole chamber and its sub-parts, use the method MDTChamber::updateStatistics()

    vector<double> values;
    double sum=0;
    int ntubes=0;
    for(int i=1; i<=getNmultilayers(); i++) {
      Multilayer *ml=getMultilayer(i);
      for(int j=1; j<=ml->getNlayers(); j++) {
	Layer *l=ml->getLayer(j);
	
	for(int k=1; k<=l->getNtubes(); k++) {
	  Tube *t= l->getTube(k);
	  if((t->getStatus())!=10) continue;
	  sum+=t->getValue();
	  ntubes++;
	  values.push_back(t->getValue());
	  
	}
      }
    }
    int size=values.size();
    if(size==0) return;

    //Calculate mean

    if(ntubes!=0) m_mean=sum/(double)ntubes;
    else m_mean = -1.;
    
    //Calulate standard deviation

    double SD =0;
    for(int i=0; i<size;i++) SD+=pow(m_mean-values[i],2);
    m_standard_deviation=sqrt(SD/(double)size);

    //Calculate median
    sort(values.begin(), values.end());

    //Here I use this vector to calculate the minumum tube value calculated without the 10% less popoulated tube
    //and the maximum tube value calulated without the 10% more popoulated tubes
    //and also the maximum tube value calulated without the 30% more popoulated tubes;
    //These values are used in the tube analysis
    m_90min=(int)values[(int)((double)size/10.)];
    m_90max=(int)values[(int)(9.*((double)size/10.))];
    m_70=(int)values[(int)(7.*((double)size/10.))];

    if((size%2)==1) m_median=values[(size-1)/2];
    else m_median=(double)(values[size/2]+values[(size/2)-1])/2.;


    //Calculate median absolute deviation

    for(int i=0; i<size; i++) {
      values[i]=std::abs(values[i]-m_median);
    }
    sort(values.begin(), values.end());
    
    if((size%2)==1) m_median_deviation=values[(size-1)/2];
    else m_median_deviation=(values[size/2]+values[(size/2)-1])/2;

    return; 
  }  //end MDTChamber::calculateStatistics
  
  void MDTChamber::updateStatistics() {
    //Recalculate statistics for the whole object and its sub-parts
    
    this->calculateStatistics();
   
    for(int i=1; i<=getNmultilayers(); i++)
      {
	Multilayer *ml=getMultilayer(i);
	for(int j=1; j<=ml->getNlayers(); j++)
	  {
	    Layer *l=ml->getLayer(j);
	    l->calculateStatistics();
	  }
	for(int j=0; j<ml->getNmezzanines(); j++)
	  {
	    Mezzanine *mezz=ml->getMezzanine(j);
	    mezz->calculateStatistics();
	  }
	ml->calculateStatistics();
      }
    this->calculateStatistics();
  }

  void MDTChamber::Print() {
    //     cout<<"---------------Chamber "<<this->getName()<<"----------------"<<endl<<endl;
    //     for(int i=1; i<=getNmultilayers(); i++) {
    //       Multilayer *ml=this->getMultilayer(i);
    //       for(int j=1; j<=ml->getNlayers(); j++) {
    //	 Layer *l=ml->getLayer(j);
    //	 for(int k=1; k<=l->getNtubes(); k++) {
    //	   Tube *t=l->getTube(k);
    //	   if(t->getStatus()==0) cout<<" ";
    //	   else {
    //	     if(t->getStatus()==10) cout<<"O"; 
    //	     else cout<<"X";
    //	   }
    //	 }
    //	 cout<<endl;
    //       }
    //       cout<<endl<<endl;
    //     }
    //     cout<<"--------------------------------------------------------------------"<<endl<<endl;
  }  //end MDTChamber::Print

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //Methods of class MDTDqaDeadElements

  MDTDqaDeadElements::MDTDqaDeadElements() {
    m_verbose=false;                     
    m_write_report=true;                //Set to true to have a debug report with all the values needed for the analysis
    m_write_compact_report=true;         //Set to true to have a compact report on dead/noisy objects found
    m_do_analysis=true;                  //Set to true to do the analysis
    m_print_chambers=false;              //Set to true to print the chambers layout in stdout, usually for debug
    m_write_list_of_dead_tubes=false;    //Set to true to get in output a list of dead tubes; It doesn't matter
					 //whether they are in a dead ML, L, mezzanine or they are single dead tubes, 
                                         //they are presented one by one in output. 
    m_do_noisy=false;                    //Set to true to do the analisys for noisy objects
					 //Be careful, at the moment it is not well tuned yet

    m_deadtubes=0;
    m_deadmezzanines=0;
    m_deadlayers=0;
    m_deadmultilayers=0;
    m_deadchambers=0;
    m_lowstatisticschambers=0;
    m_lowstatisticsfortubeschambers=0;
    m_noisytubes=0;
    m_ntubes=0;
    m_lowstatmezzanines=0;

    m_dead_tubes_per_sector=0;
    m_dead_mezzanines_per_sector=0;
    m_dead_layers_per_sector=0;
    m_dead_multilayers_per_sector=0;
    m_dead_chambers_per_sector=0;
    m_lowstat_chambers_per_sector=0;
  }

  void MDTDqaDeadElements::setVerbose(bool the_verbose) {
    m_verbose=the_verbose;
  }

  void MDTDqaDeadElements::setWriteReport(bool the_write_report) {
    m_write_report=the_write_report;
  }
  void MDTDqaDeadElements::setWriteCompactReport(bool the_write_compact_report) {
    m_write_compact_report=the_write_compact_report;
  }

  void MDTDqaDeadElements::setDoAnalysis(bool the_do_analysis) {
    m_do_analysis=the_do_analysis;
  }

  void MDTDqaDeadElements::setPrintChambers(bool the_print_chambers) {
    m_print_chambers=the_print_chambers;
  }

  void MDTDqaDeadElements::setDoNoisy(bool the_do_noisy) {
    m_do_noisy=the_do_noisy;
  }

  void MDTDqaDeadElements::setWriteListOfDeadTubes(bool the_write_list_of_dead_tubes) {
    m_write_list_of_dead_tubes=the_write_list_of_dead_tubes;
  }

  void  MDTDqaDeadElements::MDTDqaDeadElementsAnalysis(TFile *f) {
    //This is the "main" method of the class, it controls the reading, the analysis and the writing of histograms
    //calling the appropriate method
    
    int nregionsanalysed=0;
    int nsectorsanalysed=0;
    int nchambersanalysed=0;

    //    cout<<endl<<"--------------Start of MDTDqaDeadElementsAnalysis--------------------"<<endl<<endl;

    if(m_write_report) {
      TString filename=f->GetName();
      filename+="_DeadElementsAnalysis_Report.txt";
      m_file_report.open((const char*)filename);
    }
    if(m_write_compact_report) {
      TString filename=f->GetName();
      filename+="_DeadElementsAnalysis_CompactReport.txt";
      //filename="DeadElements_CompactReport.txt";
      m_comp_report.open((const char*)filename);
    }
    if(m_write_list_of_dead_tubes) {
      TString filename=f->GetName();
      filename+="_ListOfDeadTubes.txt";
      m_filelistofdeads.open((const char*)filename);
    }
    for(int iregion=1; iregion<=4; iregion++) {
      string region_name;
      if(iregion==1) region_name="Barrel_A";	
      if(iregion==2) region_name="Barrel_C";
      if(iregion==3) region_name="Endcap_A";
      if(iregion==4) region_name="Endcap_C";
      string regiondir_name="/MDT/"+region_name;
      
      //      if(m_verbose) cout<<endl<<"------------------------Analyzing region: "<<regiondir_name<<"------------------"<<endl;

      TDirectory* regiondir=(TDirectory*)f->Get(regiondir_name.c_str());
      if(!regiondir) {
	//	cout<<"MDTDqaDeadElements::MDTDqaDeadElementsAnalysis WARNING: region not found in rootfile"<<endl;
	continue;
      }
      TH2F *hsummary=(TH2F*)regiondir->FindObjectAny("DeadElements");
      if(!hsummary) {
	//	cout<<"MDTDqaDeadElements::MDTDqaDeadElementsAnalysis WARNING: DeadElements histogram not found: "<<region_name<<endl;
	continue;
      }
      hsummary->Reset();

      if(m_do_analysis)nregionsanalysed++;

      for(int isector=1; isector<=16; isector++) {
	//These values are necessary to fill the summary plot "DeadElements" in the region folder
	m_dead_tubes_per_sector=0;
	m_dead_mezzanines_per_sector=0;
	m_dead_layers_per_sector=0;
	m_dead_multilayers_per_sector=0;
	m_dead_chambers_per_sector=0;
	m_lowstat_chambers_per_sector=0;
        
	TString sector_name=(isector<10)?("Sector0"):("Sector");
	sector_name+=isector;
	TDirectory *sectordir=(TDirectory*)regiondir->Get((const char*)sector_name);

	//	if(m_verbose) cout<<endl<<"--------------------Analysing Sector "<<sector_name<<"------------------"<<endl;

	if(!sectordir) {
	  //	  if(m_verbose) cout<<"Sector not found in rootfile"<<endl;
	  continue;
	}  
	if(m_do_analysis)nsectorsanalysed++;

	TIter next(sectordir->GetListOfKeys());
	TKey *key;
	while((key=(TKey*)next())) {
	  string chamber_name=key->GetName();
	  if(chamber_name=="OVERVIEW") continue;
	  TDirectory *chamberdir=(TDirectory*)sectordir->Get(chamber_name.c_str());
	  TDirectory *deadstatusdir=(TDirectory*)sectordir->Get((chamber_name+"/DeadStatus").c_str());
	  TDirectory *expertdir=(TDirectory*)sectordir->Get((chamber_name+"/Expert").c_str());

	  //	  if(m_verbose) cout<<endl<<"------------------Analysing chamber "<<chamber_name<<"---------------"<<endl;
	  
	  if(!chamberdir) {
	    //	    cout<<"Failed to load chamberdir from rootfile"<<endl;
	    continue;
	  }

	  if(!deadstatusdir) {
	    //	    cout<<"Failed to load deadstatusdir from rootfile"<<endl; 
	    continue;
	  }

	  if(!expertdir) {
	    //	    cout<<"Failed to load expertdir from rootfile"<<endl; 
	    continue;
	  }

	  TH1F *hgeom=(TH1F*)expertdir->FindObjectAny("DeadTubeRefMap");
	  if(!hgeom) {
	    //	    cout<<"DeadTubeRefMap histogram not found: "<<chamber_name<<endl;
	    continue;
	  }
	  MDTChamber *chamber= new MDTChamber(hgeom,TString(chamber_name));

	  TH1F *hadccut=(TH1F*)chamberdir->FindObjectAny("a_HitsPerTubeAdcCut");
	  if(!hadccut) {
	    //	    cout<<"a_HitsPerTubeAdcCut histogram not found: "<<chamber_name<<endl;
	    delete chamber; chamber=0;
	    continue;
	  }
	  TH2F *hdeadchannels=(TH2F*) expertdir->FindObjectAny("ChamberDeadChannels");
	  if(!hdeadchannels) {
	    //	    cout<<"ChamberDeadChannels histogram not found: "<<chamber_name<<endl;
	    delete chamber; chamber=0;
	    continue;
	  }
	  TH2F *hdeadtubes=(TH2F*) deadstatusdir->FindObjectAny("ChamberDeadTubes");
	  if(!hdeadtubes) {
	    //	    cout<<"ChamberDeadTubes histogram not found: "<<chamber_name<<endl;
	    delete chamber; chamber=0;
	    continue;
	  }
	  TH1F *hdeadmap=(TH1F*) expertdir->FindObjectAny("DeadTubeMap");
	  if(!hdeadmap) {
	    //	    cout<<"DeadTubeMap histogram not found: "<<chamber_name<<endl;
	    delete chamber; chamber=0;
	    continue;
	  }
	  //	  if(m_verbose) cout<<"Reading a_HitsPerTubeAdcCut to fill chamber tubes entries"<<endl;
	  fillChamber(chamber,hadccut);
	  
	  hdeadmap->Reset();
	  hdeadchannels->Reset();
	  hdeadtubes->Reset();
	  
	  //	  if(m_verbose) cout<<endl<<"Starting DEAD elements analysis: "<<chamber_name<<endl;
	  if(m_do_analysis)doAnalysis(chamber);
	  if(m_print_chambers) chamber->Print();
	  if(m_do_analysis) nchambersanalysed++;
	  
	  //	  if(m_verbose) cout<<"Filling 2D and layer by layer map: "<<chamber_name<<endl;
	  //DEBUG
	  //chamber->buildMezzanines();
	  //
	  
	  //To fix the correct values in the output histograms
	  hdeadchannels->SetMinimum(0);
	  hdeadchannels->SetMaximum(10);
	  hdeadtubes->SetMinimum(0);
	  hdeadtubes->SetMaximum(4);
	  hdeadmap->SetMinimum(-50);
	  hdeadmap->SetMaximum(50);

	  fillDeadMap(chamber,hdeadchannels,hdeadtubes,hdeadmap,deadstatusdir,chamberdir);
                
	  if(m_write_list_of_dead_tubes) PrintListOfDeadTubes(chamber);	
	  delete chamber; chamber=0;
	  
	}

	hsummary->SetBinContent(isector,1,m_lowstat_chambers_per_sector);
	hsummary->SetBinContent(isector,2,m_dead_chambers_per_sector);
	hsummary->SetBinContent(isector,3,m_dead_multilayers_per_sector);
	hsummary->SetBinContent(isector,4,m_dead_layers_per_sector);
	hsummary->SetBinContent(isector,5,m_dead_mezzanines_per_sector);
	hsummary->SetBinContent(isector,6,m_dead_tubes_per_sector);
	
      }//end of loop on sectors
    }//end of loop on regions

    if(m_write_report) {
      m_file_report.close();
    }
    if(m_write_compact_report) {
      m_comp_report.close();
    }
    if(m_write_list_of_dead_tubes) {
      m_filelistofdeads.close();
    }
    //    if(m_verbose) cout<<endl<<"--------------------------------------------------------"<<endl;
    //    cout<<endl<<endl;
    //    cout<<"Regions analysed: "<<nregionsanalysed<<endl; 
    //    cout<<"Sectors analysed: "<<nsectorsanalysed<<endl; 
    //    cout<<"Chambers analysed: "<<nchambersanalysed<<endl; 
    //    cout<<"Tube analyzed: "<<m_ntubes<<endl;
    //    cout<<"Found: "<<m_deadtubes<<" Dead Tubes; "<<m_deadmezzanines<<" Dead Mezzanines; "<<m_deadlayers<<" Dead Layers; ";
    //    cout<<m_deadmultilayers<<" Dead Multilayers; "<<m_deadchambers<<" Dead Chambers; ";
    //    cout<<m_lowstatisticschambers<<" Chambers with low statistics; "<<m_lowstatisticsfortubeschambers;
    //    cout<<" Chambers with statistics too low to analyze tubes;"<<endl;
    //    cout<<"Found "<<m_lowstatmezzanines<<" mezzanines with low statistics"<<endl;
    //    if(m_do_noisy) cout<<"Noisy tubes: "<<m_noisytubes<<endl;
    //    cout<<endl<<"--------------End of MDTDqaDeadElementsAnalysis--------------------"<<endl<<endl;

    if(m_write_list_of_dead_tubes) {
      TString filename=f->GetName();
      filename+="_DeadElementsAnalysis_Report.txt";
    }
    f->Write();
    return;
  }  //end MDTDqaDeadElements::MDTDqaDeadElementsAnalysis

  void MDTDqaDeadElements::fillChamber(MDTChamber *chamber, TH1 *h) {
    //This method fills tubes values
    
    int nbins=1;

    for(int i=1; i<=chamber->getNmultilayers(); i++) {
      int bincontent=-1;

      Multilayer *ml=chamber->getMultilayer(i);
       
      for(int j=1; j<=ml->getNlayers(); j++) {
	Layer *l=ml->getLayer(j);
	   
	for(int k=1; k<=l->getNtubes(); k++) {
	  Tube *t= l->getTube(k);
	  bincontent=(int)h->GetBinContent(nbins++);
	  t->setValue(bincontent);

	  //Here we equalize the answer from BOG0,1,2,3 shorter tubes;
               
	  if(((chamber->getName()).Contains("BOG"))&&(!((chamber->getName()).Contains("BOG8")))) {
	    if(k<=6) t->setValue(t->getValue()*2);
	    if(k>(l->getNtubes()-6))  t->setValue((int)(t->getValue()*2.2));
	  }

	  //Add entries to ml,l, chamber; it is necessary to do it
	  //manually because you first build chamber structure, than you fill it;
	  //The mezzanine entries are updated later, when you call the mezzanine->calculateStatistics();
	  chamber->addEntries(bincontent);
	  ml->addEntries(bincontent);
	  l->addEntries(bincontent);

	}
	
      }
    }
  }  //end MDTDqaDeadElements::fillChamber

  void MDTDqaDeadElements::fillDeadMap(MDTChamber *chamber, TH2F *hdeadchannels, TH2F *hdeadtubes, TH1F *hdeadmap, TDirectory *deadstatusdir, TDirectory *chamberdir ) {
    //This method, after the analysis, fills the output histograms    

    int ntubes=1;
    TH1F *hdeadtubestatus=(TH1F*)chamberdir->FindObjectAny("b_DeadTubeStatus");
    if(!hdeadtubestatus) {
      //      cout<<"MDTDqaDeadElements::fillDeadMap WARNING: b_DeadTubeStatus not found "<<chamber->getName()<<endl;
      return;
    }
    hdeadtubestatus->Reset();  

    for(int i=1; i<=chamber->getNmultilayers(); i++) {
      Multilayer *ml=chamber->getMultilayer(i);
      for(int j=1; j<=ml->getNlayers(); j++) {
	Layer *l=ml->getLayer(j);
	TString deadlayername="DeadTubes_ML";
	deadlayername+=i;
	deadlayername+="_L";
	deadlayername+=j;
	int ntubeslayer=1;
    
	TH1F *hdeadperlayer=(TH1F*)deadstatusdir->FindObjectAny((const char*)deadlayername);
	if(!hdeadperlayer) {
	  //	  if(m_verbose) cout<<"MDTDqaDeadElements::fillDeadMap WARNING: "<<deadlayername<<" histogram not found: "<<chamber->getName()<<endl;
	  continue;
	}
	hdeadperlayer->Reset();

	for(int k=1; k<=l->getNtubes(); k++) {
	  Tube *t= l->getTube(k);
	  
	  int iML = i ;
	  int iLy = j ;
	  int iTube = k ;
	  int ibin = (iML-1)*(ml->getNlayers())*(l->getNtubes()) + (iLy-1)*(l->getNtubes())+iTube ;

	  if(t->getStatus()==NOTUBE) {
	    hdeadmap->SetBinContent(ntubes,0);
	  } else {
	    int ml;
	    if(i==1) ml=-1;
	    else ml=1;
	    int layer=j;
	    hdeadmap->SetBinContent(ntubes,ml*(10*layer+(t->getStatus())%10));
	    hdeadchannels->SetBinContent(k,j+5*(i-1),t->getStatus());
	    double contr=0;
	    if(t->getStatus()==7) { // status 7 used for lowstat obj
	      contr=0.5;
	    }
	    if(t->getStatus()==0) { // status 0 used for no tube in chamber
	      hdeadtubes->SetBinContent(k,j+5*(i-1),1); //white
	    } else if(t->getStatus()==10) { // status 10 used for working obj
	      contr=1;
	      hdeadtubes->SetBinContent(k,j+5*(i-1),2); //green
	    } else if(t->getStatus()==7 ) { 
	      hdeadtubes->SetBinContent(k,j+5*(i-1),3); // yellow
	    } else { 
	      hdeadtubes->SetBinContent(k,j+5*(i-1),4); // red
	    }
	    double contr2=contr ;
	    if(t->getStatus()!=0 && contr==0) contr2=0.1 ;
	    if(hdeadperlayer) hdeadperlayer->SetBinContent(ntubeslayer,contr2);
	    if(hdeadtubestatus) hdeadtubestatus->SetBinContent(ibin,contr2);
	    ntubeslayer++;
	  }
	  ntubes++;
	  
	}
      }
    }
  }  //end MDTDqaDeadElements::fillDeadMap

  void MDTDqaDeadElements::doAnalysis(MDTChamber *chamber) {
    //This methods decides whether the chamber seems dead as it has MINENTRIES entries, or the chamber has not enough
    //statistics to be analysed, or the chamber can be analysed; in the latter case, this method starts the 
    //analysis on MLs, Layers, Mezzanines, Tubes

    const int MINENTRIES=0; //number of entries under or equal to the chamber is considered empty
    const int MINMEDIAN=10; //median of chamber under or equal to you looks to the ratio to decide if you have enough
			    // statistics
    const int MAXRATIO=3;   //median/median_dev value; if you are under or equal to MAXRATIO and 
                            //under or equal to MINMEDIAN, the 
                            //chamber has too low statistics and too high fluctuations to be analysed;

    if(m_write_report) m_file_report<<endl<<"Analysing Chamber "<<chamber->getName()<<"------------------------"<<endl;

    if(chamber->getEntries()<=MINENTRIES) {
      chamber->setStatus(DEADCHAMBER);
      if(m_write_report) m_file_report<<chamber->getName()<<" Appears to be dead chamber as it has no entries"<<endl<<endl;
      if(m_write_compact_report) m_comp_report<<chamber->getName()<<" dead_CHAMBER"<<endl;
      m_deadchambers++;
      m_dead_chambers_per_sector++;
      return;
    }

    chamber->calculateStatistics();
    
    double median=chamber->getMedian();
    double median_deviation=chamber->getMedianDeviation();

    double ratio=10;
    if((median!=0)&&(median_deviation==0)) ratio=-1;
    if((median!=0)&&(median_deviation!=0)) ratio=median/median_deviation;
    if(m_write_report) m_file_report<<"entries="<<chamber->getEntries();
    if(m_write_report) m_file_report<<" median= "<<median<<"; median_dev= "<<median_deviation<<"; ratio="<<ratio<<endl;

    if((median<=MINMEDIAN)&&(ratio<=MAXRATIO)) {
      chamber->setStatus(LOWSTATISTICSCHAMBER);
      if(m_write_report) m_file_report<<chamber->getName()<<" Has Low Statistics, it won't be analysed"<<endl;
      if(m_write_compact_report) m_comp_report<<chamber->getName()<<" LOW STATISTICS"<<endl;
      m_lowstatisticschambers++;
      m_lowstat_chambers_per_sector++;
      return;
    }
    if(m_write_report) m_file_report<<chamber->getName()<<" Has enough statistics, starting other analysis"<<endl;

    //Here you start with chamber's sub-parts analysis

    analyseMultilayers(chamber);
    analyseLayers(chamber);
    analyseMezzanines(chamber);

    chamber->updateStatistics();  //It is necessary to update the chamber statistics because
    //dead elements you've found before this point must not be considered
    //in median and median deviation value of the chamber in order to analyse the tubes
    analyseTubes(chamber);
    //reanalyseMezzanines(chamber);
    //this method sets a mezzanine as dead if more than 50% of its tubes are dead. For the moment keep it diasbled until the dead tube analysis is fixed
     
    if(m_write_report) m_file_report<<endl<<"END OF ANALYSIS "<<chamber->getName()<<"------------------------"<<endl;
    if(m_write_report) m_file_report<<endl<<endl;
    /*DEBUG for mezzanines
      for(int i=1; i<=chamber->getNmultilayers(); i++)
        { 
           Multilayer *ml=chamber->getMultilayer(i);    
           for(int j=0; j<ml->getNmezzanines(); j++)
		{
		  (ml->getMezzanine(j))->setStatus(j+1);
		}

	}
      */

  }  //end MDTDqaDeadElements::doAnalysis

  void MDTDqaDeadElements::analyseMultilayers(MDTChamber *chamber) {
    //This method decides whether both multilayers have to low statistics to be compared one with the other one,
    //or one of the multilayer gives so much less hits than the other than it is dead or inefficient
    //ot the two multilayers gives similar responses, so we assume that they ar both ok;
    //If one ML is noisy and the other is ok, the noisy one will appear ok, the other one inefficient or dead;
    //here we're assuming that the higher is the better;

    const int MIN_MAXMEDIAN_ML=2; //The low statistic condition is: higher median of the two MLs below MIN_MAXMEDIAN_ML;

    const int MIN_ML_MED=1;       //Dead condition is {[(mlmed<MIN_ML_MED)OR(10*mlmed<ml2med)]
    const int SPREADDEAD=3;       //                   AND[(mlmed1-mlmed2)/meanmeddev]<-SPREADDEAD
                                  //                   AND[higher mezzanine med of ml< ml2med-SPREADDEAD*meanmeddev]}

                                  //The check on the mezzanines is beacuse some chambers have MLs with only 1 mezzanine 
				  //working properly
 
    const int SPREADINEFFICIENT=5; //Inefficient condition is {[(mlmed-ml2med)/meanmeddev)<-SPREADINEFFICIENT]
				   //                 AND[higher mezzanine med of ml< ml2med-SPREADINEFFICIENT*meanmeddev];

    double maxMLmedian=0;

    for(int i=1; i<=chamber->getNmultilayers(); i++) {
      Multilayer *ml=chamber->getMultilayer(i);
      ml->calculateStatistics();
      double med=ml->getMedian();
      if(med>maxMLmedian) maxMLmedian=med;
    }
    if(m_write_report) m_file_report<<"MLs MAXMEDIAN="<<maxMLmedian<<endl;
    if(maxMLmedian<MIN_MAXMEDIAN_ML) {
      if(m_write_report) m_file_report<<"MLs have not enough statistics to be analized"<<endl;
      return;
    }
    if(chamber->getNmultilayers()==1) {
      if(m_write_report) m_file_report<<"Only 1ML in chamber; ML analysis ok;"<< endl;
      return;
    }
    for(int i=1; i<=chamber->getNmultilayers(); i++) {
      Multilayer *ml=chamber->getMultilayer(i);
      Multilayer *ml2;
      if(i==1) ml2=chamber->getMultilayer(i+1);
      else ml2=chamber->getMultilayer(i-1);

      double maxMezzMed=0;
      
      for(int j=0; j<ml->getNmezzanines(); j++) {
	Mezzanine *mezz=ml->getMezzanine(j);
	mezz->calculateStatistics();
	double medmezz=mezz->getMedian();
	if(medmezz>maxMezzMed) maxMezzMed=medmezz;
      }
  
      double meanMedDev=(ml->getMedianDeviation()+ml2->getMedianDeviation())/2.;
      double diff=ml->getMedian()-ml2->getMedian();

      double diffToOther=diff/meanMedDev;
      if(m_write_report) m_file_report<<"ML"<<i<<"entries="<<ml->getEntries();
      if(m_write_report) m_file_report<<"; median="<<ml->getMedian()<<"; diffToOther="<<diffToOther<<"; ";
      if(m_write_report) m_file_report<<"maxMezzMed="<<maxMezzMed<<endl;  

      int contr=0;
      if((ml->getMedian()<=MIN_ML_MED)||((10*ml->getMedian())<ml2->getMedian())) {
	if(diffToOther<-SPREADDEAD) {
	  if(maxMezzMed<(ml2->getMedian()-SPREADDEAD*meanMedDev)) {
	    if(m_write_report) m_file_report<<"ML "<<i<<" seems to be dead ML"<<endl;
	    if(m_write_compact_report) m_comp_report<<chamber->getName()<<" dead_MULTILAYER "<<i<<endl;
	    ml->setStatus(DEADML);
	    m_deadmultilayers++;
	    m_dead_multilayers_per_sector++;
	    contr=1;
	  }
	}
      } else if((diffToOther<-SPREADINEFFICIENT)&&(maxMezzMed<(ml2->getMedian()-SPREADINEFFICIENT*meanMedDev))) {
	if(m_write_report) m_file_report<<"ML "<<i<<" seems to be inefficient ML"<<endl;
	if(m_write_compact_report) m_comp_report<<chamber->getName()<<" inefficient_MULTILAYER "<<i<<endl;
	ml->setStatus(INEFFICIENTML);
	m_deadmultilayers++;
	m_dead_multilayers_per_sector++;
	contr=1;
      }	

      if(contr==0) {if(m_write_report) m_file_report<<"ML "<<i<<" OK"<<endl;}
      
    }
    return;
  }  //end MDTDqaDeadElements::analyseMultilayers

  void MDTDqaDeadElements::analyseLayers(MDTChamber *chamber) {
     //This method first of all decides if layers have or not enough statistics to be analysed;
     //then it compares layers with all other layers of the whole chamber in order to find dead or inefficient ones

     const int MIN_MAXLAYER_MED=2; //If max median of layers is below this value, the statistics is too low to analyze them;

     const int MIN_LAYER_MED=2;           //Dead condition is {[ lmed<MINLAYERMED]}
     const int SPREADDEAD=3;              //        AND{[(lmed-medofotherlayersmed)/medofotherlayersmeddev]<-SPREADDEAD
     const int MIN_OTHERMEDIANS_DEAD=50;  //             OR[medianofothermedians>MIN_OTHERMEDIANS_DEAD]}
					  //        AND{[maxMezzMedian<(medofothermed-SPREADDEAD*medofothermeddev)]
    					  //             OR[maxMezzMedian<=MIN_OTHERMEDIANS_DEAD]}
  

     const int SPREADINEFFICIENT=5; // Inefficient condition is 
				    //     {[(lmed-medofotherlayersmed)/medofotherlayersmeddev]<-SPREADINEFFICIENT}
                                    //      AND {maxMezzMedian<(medofothermed-SPREADINEFFICIENT*medofothermeddev)}

     const int SPREADHOT=100;       //These values used for the hot layers analysis need to be tuned
     const int MINMEDHOT=100;

     double maxMedianLayers=0;

     for(int i=1; i<=chamber->getNmultilayers(); i++) {
       Multilayer *ml=chamber->getMultilayer(i);
       if(ml->getStatus()!=10) continue;
       for(int j=1; j<=ml->getNlayers(); j++) {
	 Layer *l=ml->getLayer(j);
	 l->calculateStatistics();
	 double med=l->getMedian();
	 if(med>maxMedianLayers) maxMedianLayers=med;
       }
       
     }
     if(maxMedianLayers<MIN_MAXLAYER_MED) {
       if(m_write_report) m_file_report<<"Layers without enough statistics, not analysed"<<endl;
       return;
     }	

     for(int i=1; i<=chamber->getNmultilayers(); i++) {
       Multilayer *ml=chamber->getMultilayer(i);
       if(ml->getStatus()!=10) continue;
       for(int j=1; j<=ml->getNlayers(); j++) {
	 Layer *l=ml->getLayer(j);
	 //double med=l->getMedian();
	 double medianOfOtherMedians=0;
	 double medianOfOtherMediansDevs=0;
	 
	 vector<double> values;
	 vector<double> values_dev;
	 
	 double maxMezzMedian=0;

	 for(int k=0; k<ml->getNmezzanines(); k++) {
	   Mezzanine *mezz=ml->getMezzanine(k);
	   vector<double> values_mezz;
	   for(int y=1; y<=mezz->getNtubes(); y++) {
	     Tube *t=mezz->getTube(y);
	     if(t->getStatus()!=10) continue;
	     if(t->getParentLayer()!=l) continue;
	     values_mezz.push_back(t->getValue());
	   }
	   int size_mezz=values_mezz.size();
	   //	   if(size_mezz==0) {cout<<"BAD DEFINED MEZZANINE IN CHAMBER"<<chamber->getName()<<endl; continue;}
	   double mezzmed;
	   sort(values_mezz.begin(), values_mezz.end());
	   if((size_mezz%2)==1) mezzmed=values_mezz[(size_mezz-1)/2];
	   else mezzmed=(double)(values_mezz[size_mezz/2]+values_mezz[(size_mezz/2)-1])/2.;
	   if (mezzmed>maxMezzMedian) maxMezzMedian=mezzmed;
	 }
	 for(int k=1; k<=chamber->getNmultilayers(); k++) {
	   Multilayer *ml2=chamber->getMultilayer(k);
	   for(int t=1; t<=ml2->getNlayers(); t++) {
	     Layer *l2=ml->getLayer(t);
	     if (l2==l) continue;
	     values.push_back(l2->getMedian());
	     values_dev.push_back(l2->getMedianDeviation());
	   }  
	 }

	 int size=values.size();
	 //	 if(size==0) {cout<<"BAD DEFINED LAYER IN CHAMBER"<<chamber->getName()<<endl; continue;}
	 sort(values.begin(), values.end());
	 if((size%2)==1) medianOfOtherMedians=values[(size-1)/2];
	 else medianOfOtherMedians=(double)(values[size/2]+values[(size/2)-1])/2.;

	 int size_dev=values_dev.size();
	 sort(values_dev.begin(), values_dev.end());
	 if((size_dev%2)==1) medianOfOtherMediansDevs=values_dev[(size_dev-1)/2];
	 else medianOfOtherMediansDevs=(values_dev[size_dev/2]+values_dev[(size_dev/2)-1])/2.;

	 int contr=0;
	 double diff=l->getMedian()-medianOfOtherMedians;
	 double diffToOthers;
	 if(diff==0) diffToOthers=0;
	 else {
	   if(medianOfOtherMediansDevs==0) diffToOthers=100;
	   else diffToOthers=diff/medianOfOtherMediansDevs;
	 }
	 if(m_write_report) m_file_report<<"Layer"<<j<<" entries= "<<l->getEntries()<<" median="<<l->getMedian()<<"; diffToOther="<<diffToOthers<<"; ";
	 if(m_write_report) m_file_report<<"medianOfOthersMedian="<<medianOfOtherMedians<<"; ";
	 if(m_write_report) m_file_report<<"medianOfOthersMedianDev="<<medianOfOtherMediansDevs<<"; ";
	 if(m_write_report) m_file_report<<"maxMezzMedian="<<maxMezzMedian<<endl;
	 
	 if((diffToOthers>SPREADHOT)&&(l->getMedian()>MINMEDHOT)) {
	   if(!m_do_noisy) continue;
	   
	   if(m_write_report) m_file_report<<"ML "<<i<<" Layer "<<j<<" seems to be noisy layer"<<endl;
	   if(m_write_compact_report) m_comp_report<<chamber->getName()<<" noisy_LAYER "<<i<<" "<<j<<endl;
	   l->setStatus(NOISYLAYER);
	   contr=1;
	 }

	 if(l->getMedian()<MIN_LAYER_MED) {
	   if((diffToOthers<=-SPREADDEAD)||(medianOfOtherMedians>MIN_OTHERMEDIANS_DEAD)) {
	     if((maxMezzMedian<(medianOfOtherMedians-SPREADDEAD*medianOfOtherMediansDevs))||(maxMezzMedian<=MIN_OTHERMEDIANS_DEAD)) {
	       if(m_write_report) m_file_report<<"ML "<<i<<" Layer "<<j<<" seems to be dead layer"<<endl;
	       if(m_write_compact_report) m_comp_report<<chamber->getName()<<" dead_LAYER "<<i<<" "<<j<<endl;
	       l->setStatus(DEADLAYER);
	       m_deadlayers++;
	       m_dead_layers_per_sector++;
	       contr=1;
	     }
	   }
	 } else if((diffToOthers<-SPREADINEFFICIENT)&&(maxMezzMedian<(medianOfOtherMedians-SPREADINEFFICIENT*medianOfOtherMediansDevs))) {
	   if(m_write_report) m_file_report<<"ML "<<i<<" Layer "<<j<<" seems to be inefficient layer"<<endl;
	   if(m_write_compact_report) m_comp_report<<chamber->getName()<<" inefficient_LAYER "<<i<<" "<<j<<endl;
	   l->setStatus(INEFFICIENTLAYER);
	   m_deadlayers++;
	   m_dead_layers_per_sector++;
	   contr=1;
	 }
	 if(contr==0) {if(m_write_report) m_file_report<<"ML "<<i<<" Layer "<<j<<" ok"<<endl;}
	 
       }
     }
     
     return;

  }  //end MDTDqaDeadElements::analyseLayers

  void MDTDqaDeadElements::reanalyseMezzanines(MDTChamber *chamber) {
     for(int i=1; i<=chamber->getNmultilayers(); i++) {
       Multilayer *ml=chamber->getMultilayer(i);
       if(ml->getStatus()!=10) continue;
       for(int j=0; j<ml->getNmezzanines(); j++) {
	 Mezzanine *mezz=ml->getMezzanine(j);
	 int ntubes=0;
	 int ndeadtubes=0;
	 for(int k=1; k<=mezz->getNtubes(); k++) {
	   Tube *t=mezz->getTube(k);
	   if(t->getStatus()==0) continue;
	   ntubes++;
	   if(t->getStatus()==DEADTUBE) ndeadtubes++;
	 }          
	 if(ntubes==0) continue;
	 else if((double)((double)ndeadtubes/(double)ntubes)>0.5) {
	   if(m_write_report) m_file_report<<"ML "<<i<<" Mezzanine "<<j<<" appears to be dead mezzanine"<<endl;
	   if(m_write_compact_report) m_comp_report<<chamber->getName()<<" dead_MEZZANINE_rean "<<i<<" "<<j<<endl;
	   mezz->setStatus(DEADMEZZANINE);
	   //m_dead_tubes_per_sector-=ntubes;
	   m_dead_tubes_per_sector-=ndeadtubes;
	   // m_deadtubes-=ntubes;
	   m_deadtubes-=ndeadtubes;
	   m_dead_mezzanines_per_sector++;
	   m_deadmezzanines++;
           
	 }
       }
     }

  }  //end  MDTDqaDeadElements::reanalyseMezzanines

  void MDTDqaDeadElements::analyseMezzanines(MDTChamber *chamber) {
    //This method first of all decides if mezzanines have or not enough statistics to be analysed;
    //then it compares mezzanines with all other mezzanines of the whole chamber in order to find dead or inefficient ones;
    //Because of some chambers that suffers of complex geometry/trigger effects, showing very uneven distributions
    //here we define a flatness value, and use different strategy for "normal" chambers and "strang" chambers;

    //Chamber flatness is a value defined as medianDeviationOfMezzanineMedians/medianOfMezzanineMediansDeviation
    //A chamber is considered flat if this value is <=1;

    const int MIN_MAXMEDIAN=2; //If max median of mezzanines is below this value, the statistics is to low to analyze them

 
//    const int MIN_ENTRIES=1;   //Dead condition: {[mezz.entries<=MINENTRIES]
//    const int SPREADDEAD=5;   //                AND[diffToOthers<-SPREADDEAD]}

//    const int SPREADINEFFICIENT=10;  //Inefficient condition: 
                                     //flat chambers: [diffToOthers<=-SPREADINEFFICIENT];
                                     //unflat chambers: [diffToOthers<=-SPREADINEFFICIENT*chamberflatness];
                                     //chamber with flatness>5 not analyzed

//    const int SPREADHOT=10;          //The noisy mezzanine analysis needs to be tuned

    double maxMedianMezzanines=0;

    double medianOfMedians=0;
    double medianOfMediansDeviation=0;
    double medianDeviationOfMedians=0;

    vector<double> values;  
    vector<double> values_dev;
      
    for(int i=1; i<=chamber->getNmultilayers(); i++) {
      Multilayer *ml=chamber->getMultilayer(i);
      if(ml->getStatus()!=10) continue;
      for(int j=0; j<ml->getNmezzanines(); j++) {
	Mezzanine *mezz=ml->getMezzanine(j);
	mezz->calculateStatistics(); //It is necessary to recalculate the statistics in order to exclude dead layers
	double med=mezz->getMedian();
	values.push_back(med);
	values_dev.push_back(mezz->getMedianDeviation());
	if(med>maxMedianMezzanines) maxMedianMezzanines=med;
      }

    }
    int size=values.size();
    sort(values.begin(), values.end());
    if((size%2)==1) medianOfMedians=values[(size-1)/2];
    else medianOfMedians=(double)(values[size/2]+values[(size/2)-1])/2.;
    
    for(int l=0; l<size; l++) values[l]=std::abs(values[l]-medianOfMedians);
    sort(values.begin(), values.end());
    if((size%2)==1) medianDeviationOfMedians=values[(size-1)/2];
    else medianDeviationOfMedians=(double)(values[size/2]+values[(size/2)-1])/2.;
    
    int size_dev=values_dev.size();
    sort(values_dev.begin(), values_dev.end());
    if((size_dev%2)==1) medianOfMediansDeviation=values_dev[(size_dev-1)/2];
    else medianOfMediansDeviation=(double)(values_dev[size_dev/2]+values_dev[(size_dev/2)-1])/2.;
    
    double chamberFlatness=medianDeviationOfMedians/medianOfMediansDeviation;
    
    if(maxMedianMezzanines<MIN_MAXMEDIAN) {
      if(m_write_report) m_file_report<<"Mezzanines without enough statistics, not analysed"<<endl;
      return;
    }
    if(m_write_report) m_file_report<<"ChamberFlatness="<<chamberFlatness<<endl;	

    for(int i=1; i<=chamber->getNmultilayers(); i++) {
      Multilayer *ml=chamber->getMultilayer(i);
      Multilayer *other_ml=NULL;
      if(chamber->getNmultilayers()>1) {
	if(i==1) other_ml=chamber->getMultilayer(2);
	if(i==2) other_ml=chamber->getMultilayer(1);
      }

      if(ml->getStatus()!=10) continue;
          
      for(int j=0; j<ml->getNmezzanines(); j++) {
	Mezzanine *mezz=ml->getMezzanine(j);
	if(mezz->getNtubes()==0) continue;
	double med=mezz->getMedian();
	double meddev=mezz->getMedianDeviation();
	double medianOfOtherMedians=0;
	double medianOfOtherMediansDevs=0;
	
	vector<double> values;
	vector<double> values_dev;
        
	double minOtherLargerMedians=-1;              
		
	for(int k=1; k<=chamber->getNmultilayers(); k++) {
	  Multilayer *ml2=chamber->getMultilayer(k);
	  if(ml2->getStatus()!=10) continue;

	  for(int t=0; t<ml2->getNmezzanines(); t++) {
                        
	    Mezzanine *mezz2=ml2->getMezzanine(t);
	    if (mezz2==mezz) continue;
	    values.push_back(mezz2->getMedian());
	    values_dev.push_back(mezz2->getMedianDeviation());
	    if((minOtherLargerMedians==-1)||((mezz2->getMedian()>med)&&(mezz2->getMedian()<minOtherLargerMedians))) {
	      minOtherLargerMedians=mezz2->getMedian();
	    }
	  }
	}
		
	int size=values.size();
	sort(values.begin(), values.end());
	if((size%2)==1) medianOfOtherMedians=values[(size-1)/2];
	else medianOfOtherMedians=(double)(values[size/2]+values[(size/2)-1])/2.;

	int size_dev=values_dev.size();
	sort(values_dev.begin(), values_dev.end());
	if((size_dev%2)==1) medianOfOtherMediansDevs=values_dev[(size_dev-1)/2];
	else medianOfOtherMediansDevs=(values_dev[size_dev/2]+values_dev[(size_dev/2)-1])/2.;

	//int contr=0;
	double diff=mezz->getMedian()-medianOfOtherMedians;
	double diffToOthers;
	diffToOthers=diff/(std::max(1.,std::max(mezz->getMedianDeviation(),medianOfOtherMediansDevs)));

	std::vector<Mezzanine*> near_mezz;

	//lowstat case
	bool isLowStat=false;
	if(med>0&&meddev>0&&((med-1.5*meddev)<0)) isLowStat=true;
	if(isLowStat) {
	  mezz->setStatus(LOWSTATMEZZANINE);
	  m_lowstatmezzanines++;
	  continue;
	}

	//if(chamber->getName()!="BIR1A11") continue;
	//cout<<"mezzanine status "<<mezz->getStatus()<<endl;
	
	//cout<<"mezz "<<j<<endl;
	if(j>0) {
	  if(ml->getMezzanine(j-1)!=NULL) {
	    //cout<<" sx"<<endl;
	    near_mezz.push_back(ml->getMezzanine(j-1));
	  }
	}
	if(j<(ml->getNmezzanines()-1)) {
	  if(ml->getMezzanine(j+1)!=NULL) {
	    //cout<<" dx"<<endl;
	    near_mezz.push_back(ml->getMezzanine(j+1));
	  }
	}
	if(other_ml!=NULL) {
	  //		    double value=-1;
	  if(other_ml->getMezzanine(j)!=NULL) {
	    //cout<<" up"<<endl;
	    near_mezz.push_back(other_ml->getMezzanine(j));
	  }
	}

	if(near_mezz.size()>0) {
	  bool isDead=false;
	  bool OneGood=false;
	  double lower_limit=9999999;
	  for(unsigned int k=0; k<near_mezz.size(); k++) {
	    //if(near_mezz[k]->getStatus()!=10) continue;

//			double other_med=near_mezz[k]->getMedian();
//			double other_meddev=near_mezz[k]->getMedianDeviation();
	    double other_min70=near_mezz[k]->get70min();
	    //if((other_med-2*other_meddev)<0) continue;
	    OneGood=true;
	    if(other_min70<lower_limit) lower_limit=other_min70;

	  }

	  double max70= mezz->get70max();
	  if(OneGood&&max70<(lower_limit/5.)&&(lower_limit>5)&&med==0) isDead=true;
	  if(isDead) {
	    //cout<<chamber->getName()<<" ml="<<i<<" mezz="<<j<<" med="<<med<<" meddev="<<meddev<<" lowlim="<<(lower_limit/5.)<<endl;
	    for(unsigned int k=0; k<near_mezz.size(); k++) {
//			    double other_med=near_mezz[k]->getMedian();
//			    double other_meddev=near_mezz[k]->getMedianDeviation();
			    //cout<<"  other md="<<other_med<<"  other md_dev="<<other_meddev<<endl;
	    }
	    if(m_write_report) m_file_report<<chamber->getName()<<" ML "<<i<<" Mezzanine "<<j<<" appears to be dead_mezzanine; low_limit="<<lower_limit<<" median="<<med<<endl;
	    if(m_write_compact_report) m_comp_report<<chamber->getName()<<" dead_MEZZANINE "<<i<<" "<<j<<endl;
	    mezz->setStatus(DEADMEZZANINE);
	    m_deadmezzanines++;
	    m_dead_mezzanines_per_sector++;
	    //			contr=1;
	  }

	}
	if(mezz->getStatus()==10 && med<10) { //med=10 is compatible with 0!
	  mezz->setStatus(LOWSTATMEZZANINE);
	  m_lowstatmezzanines++;
	  continue;
	}
	if(m_write_report) m_file_report<<"ML"<<i<<" mezz"<<j<<": med="<<mezz->getMedian()<<"; "<<" mad="<<mezz->getMedianDeviation();
	if(m_write_report) m_file_report<<"medev"<<medianOfOtherMediansDevs<<"; ";
	if(m_write_report) m_file_report<<"entries="<<mezz->getEntries()<<"; diffToOthers="<<diffToOthers<<"; ";
	if(m_write_report) m_file_report<<"minlargermed="<<minOtherLargerMedians<<endl;         
	if(m_write_report) m_file_report<<"inefficient if med below=";
	if(m_write_report) m_file_report<<(minOtherLargerMedians-5*medianOfOtherMediansDevs)<<endl;
		
      }
    }
     
    return;

  }  //end MDTDqaDeadElements::analyseMezzanines

  void MDTDqaDeadElements::analyseTubes(MDTChamber *chamber) {
    //This method first of all decides if the chamber has enough statistics to analyze single tubes
    //then it search for dead tubes

    //There are chambers which looks completely dead except one mezzanine; finding them using median criteria is impossible
    //they just looks like LOW STATISTICS chambers; so, before the low statistics check, I' ve implemented another one
    //If, after ML, L, Mezz analysis, more than 70% of chambers tubes has 0 entries and there is a mezzanine
    //with a median.=50, than the chamber is signed as DEAD, to show that there is a big problem.

    const int MINCHAMBERMED=20;  //chambers with med<MINCHAMBERMED are considered to have 
                                 //too low statistics to analyze single tubes
 
    const int SPREADDEAD=5;  //Dead condition  {[tube.entries<(chambermed-SPREADDEAD*chambermemddev)]
                             //  	        AND[tube.entries<(chambermed/10)]AND[chambermed>=MINCHAMBERMED]}
    			     //		    OR{tube.entries==0 AND chambermed>=50}

                             //Because of chambers with geometry/trigger effects, some tubes which actually are dead
                             //cannot be found (this is whem chambermeddev is very high)
                             //So, I've added a new check: a tube is considered dead also if it has less than 
                             //10% entries respect to the tube with the minimum value of the chamber calculated excluding
                             //the 10% less popoulate tubes of the chamber;
 
    //The noisy tubes analysis is not used for now

    int ndeadtubes=0;
    int nnoisytubes=0;
    if(m_write_report) m_file_report<<"Starting Tube Analysis"<<endl;

    double median=chamber->getMedian();
    //double median_deviation=chamber->getMedianDeviation();
    double standard_deviation=chamber->getStandardDeviation();

    double maxMezzMed=0;

    for(int i=1; i<=chamber->getNmultilayers(); i++) {
      Multilayer *ml=chamber->getMultilayer(i);
      if(ml->getStatus()!=10) continue;
      for(int j=0; j<ml->getNmezzanines(); j++) {
	Mezzanine *mezz=ml->getMezzanine(j);
	if(mezz->getMedian()>maxMezzMed) maxMezzMed=mezz->getMedian();
      }
      
    }
    if((maxMezzMed>=50)&&(chamber->get70()==0)) {
      chamber->setStatus(DEADCHAMBER);
      if(m_write_report) m_file_report<<chamber->getName()<<" Appears to be a chamber with only few live tubes"<<endl<<endl;
      if(m_write_compact_report) m_comp_report<<chamber->getName()<<" strange_CHAMBER"<<endl;
      m_deadchambers++;
      m_dead_chambers_per_sector++;
      return;
      
    }

    if(median<MINCHAMBERMED) {
      m_lowstatisticsfortubeschambers++;
      m_lowstat_chambers_per_sector++;
      if(m_write_report) m_file_report<<"This chamber has too low statistics to analyze single tubes"<<endl;
      if(m_write_compact_report) m_comp_report<<chamber->getName()<<" LOW STATISTICS FOR TUBES"<<endl;
      for(int i=1; i<=chamber->getNmultilayers(); i++) {
	Multilayer *ml=chamber->getMultilayer(i);
	if(ml->getStatus()!=10) continue;
	for(int j=1; j<=ml->getNlayers(); j++) {
	  Layer *l=ml->getLayer(j);
	  if(l->getStatus()!=10) continue;
	  for(int k=1; k<=l->getNtubes(); k++) {
	    Tube *t=l->getTube(k);
	    if(t->getStatus()!=10) continue;
	    t->setStatus(LOWSTATISTICSCHAMBER);
	  }		 
	}
      }

      return;
    }

    double marco_limit=0.1*(chamber->get90min())-1.;
//    double marco_HOTlimit=10.*(chamber->get90max());

    for(int i=1; i<=chamber->getNmultilayers(); i++) {
      Multilayer *ml=chamber->getMultilayer(i);
      if(ml->getStatus()!=10) continue;
	double medML=ml->getMedian();
	double medDevML=ml->getMedianDeviation();
	double limit=medML-SPREADDEAD*medDevML;

        if(m_write_report) {
	  m_file_report<<"ML"<<i<<" Tube Limit="<<limit<<"; medML="<<medML<<" ;medDevML="<<medDevML<<"; medML/10="<<((double)medML/10.)<<"; tube noisy over="<<(10*standard_deviation+median)<<" marcolimit="<<marco_limit<<endl;
	}

	for(int j=0; j<ml->getNmezzanines(); j++) {
	  Mezzanine *mezz=ml->getMezzanine(j);
	  if(mezz->getStatus()!=10) continue;
//		double medMezz=mezz->getMedian();
//		double medDevMezz=mezz->getMedianDeviation();
	  double marco_limitMezz=0.1*mezz->get70min()-1;
	  //if(chamber->getName()=="BIL6A01") cout<<" Mezzanine "<<i<<" "<<j<<" limit="<<marco_limitMezz<<endl;
	  for(int k=1; k<=mezz->getNtubes(); k++) {

	    Tube *t=mezz->getTube(k);
	    if(t->getStatus()!=10) continue;
	    m_ntubes++;
	    int value=t->getValue();
 
	    //if(chamber->getName()=="BIL6A01") cout<<i<<" "<<j<<" "<<k<<" entries="<<value<<endl;
	    if(value<marco_limitMezz) {
	      if(m_write_compact_report) m_comp_report<<chamber->getName()<<" dead_TUBE "<<t->getML()<<" "<<t->getLayer()<<" "<<t->getPosition()<<endl;
	      //if(chamber->getName()=="BIL6A01")cout<<"DEAD!!!"<<endl;

	      t->setStatus(DEADTUBE);
	      ndeadtubes++;
	      m_deadtubes++;
	      m_dead_tubes_per_sector++;
	    }
	  }
	}
// 	for(int j=1; j<=ml->getNlayers(); j++)
// 	  {
// 	    Layer *l=ml->getLayer(j);
//             if(l->getStatus()!=10) continue;
// 	    if(chamber->getName()=="BIL6A01")cout<<"Layer "<<j<<endl;
// 	    for(int k=1; k<=l->getNtubes(); k++)
// 	      {
// 		Tube *t=l->getTube(k);
               
// 		int value=t->getValue();
// 		if(chamber->getName()=="BIL6A01") cout<<i<<" "<<j<<" "<<k<<" entries="<<value<<endl;
// 	      }
// 	  }

// 	for(int j=1; j<=ml->getNlayers(); j++)
// 	  {
// 	    Layer *l=ml->getLayer(j);
//             if(l->getStatus()!=10) continue;

// 	    for(int k=1; k<=l->getNtubes(); k++)
// 	      {
// 		Tube *t=l->getTube(k);
//                 if(t->getStatus()!=10) continue; 
//                 m_ntubes++;
// 		int value=t->getValue();
//                 double valueSD=(standard_deviation==0)?(0.):((value-median)/standard_deviation);
// 		//double valueSD=(median_deviation==0)?(0.):((value-median)/median_deviation); altra possibilita'

                 
// 		if(((value<limit)&&(value<(medML/10.))&&(medML>=10)&&(medML<=200))||((value==0)&&(medML>=50)))
// 		  {
// 		    //m_file_report<<"ML "<<i<<" Layer "<<k<<" Tubo "<<k<<" seems to be dead"<<endl;
//                     if(m_write_compact_report) m_comp_report<<chamber->getName()<<" dead_TUBE "<<i<<" "<<j<<" "<<k<<endl;
//  		    t->setStatus(DEADTUBE);
//                     ndeadtubes++;
//                     m_deadtubes++;
//                     m_dead_tubes_per_sector++;
// 		  }
//                  else if(value<(marco_limit))
// 		  {
//                     if(m_write_compact_report) m_comp_report<<chamber->getName()<<" dead_TUBE "<<i<<" "<<j<<" "<<k<<endl;
//  		    t->setStatus(DEADTUBE);
//                     ndeadtubes++;
//                     m_deadtubes++;
//                     m_dead_tubes_per_sector++;
// 		  }

//                  else if((valueSD>10)&&(median>=10))
// 		  {
//                     if(!m_do_noisy) continue;
// 		    //m_file_report<<"ML "<<i<<" Layer "<<k<<" Tubo "<<k<<" seems to be noisy"<<endl;
//                     if(m_write_compact_report) m_comp_report<<chamber->getName()<<" noisy_TUBE "<<i<<" "<<j<<" "<<k<<endl;
// 		    t->setStatus(NOISYTUBE);
//                     nnoisytubes++;
// 		    m_noisytubes++;		
 
// 		  }
// 		  else if(value>marco_HOTlimit)
// 		  {
//                     if(!m_do_noisy) continue;
//                     if(m_write_compact_report) m_comp_report<<chamber->getName()<<" noisy_TUBE "<<i<<" "<<j<<" "<<k<<endl;
// 		    t->setStatus(NOISYTUBE);
//                     nnoisytubes++;
//                     m_noisytubes++;
		    
// 		  }		

// 	      }
//	    }
    }
    if(m_write_report) m_file_report<<"End of tube analysis; "<<ndeadtubes<<"dead tubes found"<<nnoisytubes<<"noisy tubes found"<<endl;

  }  //end MDTDqaDeadElements::analyseTubes
  
  void MDTDqaDeadElements::PrintListOfDeadTubes(MDTChamber *chamber) {

    for(int i=1; i<=chamber->getNmultilayers(); i++) {
      Multilayer *ml=chamber->getMultilayer(i);
      for(int j=1; j<=ml->getNlayers(); j++) {
	Layer *l=ml->getLayer(j);
	for(int k=1; k<=l->getNtubes(); k++) {
	  Tube *t=l->getTube(k);
	  if((t->getStatus()==10)||(t->getStatus()==0)) continue;
	  m_filelistofdeads<<"Chamber "<<chamber->getName()<<" ML "<<i<<" L "<<j<<" Tube "<<k<<" DEAD"<<endl;
	}
      }
    }
    return;
  }  //end MDTDqaDeadElements::PrintListOfDeadTubes

}  //namespace MuonCalib

