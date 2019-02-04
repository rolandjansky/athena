/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// PixelDigitization includes
#include "EfieldInterpolator.h"

#include "TROOT.h"
#include "TH1.h"
#include "TF1.h"
#include "TVectorD.h"
#include <TFile.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include <TMath.h>
#include <TGraph.h>
#include <TPRegexp.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TGraph2D.h>
#include <algorithm>

using namespace RadDam;

static const InterfaceID IID_IEfieldInterpolator("EfieldInterpolator", 1, 0);
const InterfaceID& EfieldInterpolator::interfaceID( ){ return IID_IEfieldInterpolator; }

// Constructor with parameters:                                                                                                                                     
EfieldInterpolator::EfieldInterpolator(const std::string& type, const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_initialized   (false  ),  
  m_saveDocu      (true   ),     
  m_useSpline     (true   ),     
  m_sensorDepth   (200    ),          //um - default is IBL layer
  m_efieldOrigin(interspline)
{
  declareInterface< EfieldInterpolator >( this );
  declareProperty( "initialized", m_initialized, "Flag whether already initalized" );
  declareProperty( "saveDocu", m_saveDocu, "Flag whether to save individual Histograms for documentation" ); 
  declareProperty( "useSpline", m_useSpline, "Flag whether to use cubic splines for interpolation" ); 
  declareProperty( "sensorDepth", m_sensorDepth, "Depth of E fields in sensors in um" ); 
}

EfieldInterpolator::~EfieldInterpolator() 
{
}

StatusCode EfieldInterpolator::initialize() {
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  return StatusCode::SUCCESS;
}


StatusCode EfieldInterpolator::finalize() {
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

TVectorD CastStdVec(const std::vector<double> vin ){
	TVectorD tmp(vin.size());
	//for(uint i = 0; i<vin.size(); i++ ){
        uint index = 0;
        for(auto i : vin){
		tmp[index] = i;
                index++;
	}
	return tmp;
}

//Returns index at of std::vectorv where val is contained
int isContainedAt(std::vector<double> v, double val){
	for(uint i=0; i<v.size(); i++){
                //Equality for decimals
		if( v.at(i)-0.00001  < val && val < v.at(i)+0.00001 ) return i;
	}
	return -1;	
}

// Initialize indicating layer due to different geometry
// One instance interpolates only for fixed geometry (=pixel depth)
void EfieldInterpolator::setLayer(int layer)
{
	ATH_MSG_INFO("Create interpolator for layer "<< layer);
        if(layer > 0){
            //it's b layer
            m_sensorDepth = 250;
        }else{
            //IBL
            m_sensorDepth = 200;
        }
        ATH_MSG_INFO("EfieldInterpolator: Default ctor");
}

StatusCode EfieldInterpolator::loadTCADlist(std::string TCADfileListToLoad )
{
        m_efieldOrigin    = interspline;
	ATH_MSG_INFO("Load from list " << TCADfileListToLoad);
        if(!initializeFromFile(TCADfileListToLoad) ){
            ATH_MSG_WARNING("ERROR: Initialize failed looking for file " << TCADfileListToLoad );
            //Check if given path links to directory:
            if(!initializeFromDirectory(TCADfileListToLoad)){;
                return StatusCode::FAILURE;
            }
        }
        ATH_MSG_INFO("Finished loading TCAD list");
        return StatusCode::SUCCESS;
}

bool EfieldInterpolator::initializeFromDirectory(const std::string fpath){
    //similar to function  loadTCADfiles() 
    //instead of reading file list, list files from directory fpath and create file listing information  
    
    //retrieve fluence and bias voltage from file name (including path, directory names fluence value)
    //skip files which do not dollow naming declaration

    //Create textfile for tmp storage
    std::ofstream efieldscalib;
    efieldscalib.open("listOfEfields.txt");
    TPRegexp rvo("\\b-[0-9](\\w+)V\\b");
    TPRegexp rfl("\\bfl([\\w.]+)e[0-9]{1,2}");
    TString ext = ".dat";
    TString sflu, svol, fullpath;
    TSystemDirectory dir( (TString) fpath, (TString) fpath); 
    TList *files = dir.GetListOfFiles(); 
    if (files) { 
        TSystemFile *file; 
        TString fname; 
        TIter next(files); 
        while ((file=(TSystemFile*)next())) { 
            fname = file->GetName(); 
            if(fname.BeginsWith("fl") && file->IsDirectory() ){
                TString deeppath = fpath;
                deeppath += fname;
                deeppath += "/reference/";
                TSystemDirectory deepdir(deeppath, deeppath); 
                TList *deepfiles = deepdir.GetListOfFiles(); 
                if (deepfiles) { 
                    TSystemFile *deepfile; 
                    TString deepfname; 
                    TIter deepnext(deepfiles); 
                    while ((deepfile=(TSystemFile*)deepnext())) { 
                        deepfname = deepfile->GetName(); 
                        if (!deepfile->IsDirectory() && deepfname.EndsWith(ext)) { 
                            svol = deepfname(rvo);    
                            svol.ReplaceAll("-","");
                            svol.ReplaceAll("V","");
                            sflu = deeppath(rfl);    
                            sflu.ReplaceAll("fl","");
                            fullpath = deeppath;
                            fullpath += deepfname;
                            if(!deepfname.Contains("pruned")){
                                if(!sflu.IsFloat()){
                                    ATH_MSG_INFO("EfieldInterpolator load from directory - could not resolve fluence from " << fullpath);
                                    continue;
                                }
                                if(!svol.IsFloat()){
                                    ATH_MSG_INFO("EfieldInterpolator load from directory - could not resolve voltage from " << fullpath);
                                    continue;
                                }
                            }else{
                                ATH_MSG_INFO("Skip pruned files: " << fullpath);
                                continue;
                            }
                            efieldscalib << fullpath << " " << sflu << " " << svol << std::endl; 
                        } 
                    }
                }
            }
        } 
    } 
    efieldscalib.close();
    bool success = initializeFromFile("listOfEfields.txt");
    if(success) ATH_MSG_INFO("Initialized from directory");  
    return success;
}
// Load maps into TTree for faster processing 
bool EfieldInterpolator::initializeFromFile(std::string finpath){
    TString fpath = finpath ;
    m_initialized = false;
    TString fTCAD = "";
    if(fpath.EndsWith(".txt")){
        //File list path, fluence and bias voltage of TCAD simulation as plain text
	fTCAD = loadTCADfiles(fpath.Data());
        ATH_MSG_INFO("Loaded file " << fpath << " - all maps accumulated in " << fTCAD);
        m_fInter = createInterpolationFromTCADtree(fTCAD.Data()); 
        ATH_MSG_INFO("created interpolation tree ");
	m_initialized 	= true;
        ATH_MSG_INFO("Loaded from .txt file");
    }else{
        if(fpath.EndsWith("toTTree.root") ){
            //File list TCAD efield maps as leaves
            m_fInter = createInterpolationFromTCADtree(fpath.Data()); 
	    m_initialized 	= true;
        }else{
            if(fpath.EndsWith(".root")){
                //File list is already transformed to tree
                m_fInter = fpath;
	        m_initialized 	= true;
            }
        }
    } 
    ATH_MSG_INFO("Interpolation has been initialized from file "<< m_fInter <<" - successful "<< m_initialized);
    return m_initialized;
}

// Check if requested values out of range of given TCAD samples
void EfieldInterpolator::reliabilityCheck(double aimFluence, std::vector<double> fluences, double aimVoltage, std::vector<double> voltages){
    bool tooLowVolt     = true;
    bool tooHighVolt    = true;
    for(const auto iv : voltages){
        if( aimVoltage < iv ) tooHighVolt= false;
        if( aimVoltage > iv ) tooLowVolt= false;
    }
    bool tooLowFlu     = true;
    bool tooHighFlu    = true;
    for(uint iv = 0; iv< fluences.size(); iv++){
        if( aimFluence < fluences.at(iv) ) tooHighFlu = false;
        if( aimFluence > fluences.at(iv) ) tooLowFlu = false;
    }
    if(tooLowFlu        ) ATH_MSG_WARNING("WARNING: The fluence value you specified (" << aimFluence <<") is too low to be reliably interpolated!"  );
    if(tooLowVolt       ) ATH_MSG_WARNING("WARNING: The voltage value you specified (" << aimVoltage <<") is too low to be reliably interpolated!"  );
    if(tooHighFlu       ) ATH_MSG_WARNING("WARNING: The fluence value you specified (" << aimFluence <<") is too high to be reliably interpolated!" );
    if(tooHighVolt      ) ATH_MSG_WARNING("WARNING: The voltage value you specified (" << aimVoltage <<") is too high to be reliably interpolated!" );

    // Results from Closure Test
    // TCAD files save 20 for 20e14 neq/cm2
    if( 12.2 < aimFluence || aimFluence < 1.  ) ATH_MSG_WARNING(" WARNING: The fluence value you specified ("<< aimFluence <<") is outside the range within it could be reliably interpolated!" ); //Based on closure test June 2018 - max fluences available: ... 10 12 15 20
    if( 1010. < aimVoltage || aimVoltage < 79.  ) ATH_MSG_WARNING(" WARNING: The voltage value you specified ("<< aimVoltage <<") is outside the range within it could be reliably interpolated!" ); //Based on closure test June 2018 - max volatges available: ... 600 800 1000V
    return;
}

void EfieldInterpolator::scaleIntegralTo(TH1* hin, double aimInt, int first , int last){
    hin->Scale(aimInt/(float) hin->Integral(first,last) );
}

double EfieldInterpolator::relativeDistance(double x1, double x2)
{
    if( x1 == 0. ) return 0.;
    return( (x1-x2)/x1 );
}

//Use as definition for distance in Fluence/Voltage space
double EfieldInterpolator::relativeDistance(double x1, double y1, double x2, double y2)
{
    return( std::sqrt( relativeDistance(x1,x2)*relativeDistance(x1,x2) + relativeDistance(y1,y2)*relativeDistance(y1,y2))) ;
}

double EfieldInterpolator::extrapolateLinear(double x1, double y1, double x2, double y2, double xaim){
	// follow linear extrapolation: y=mx+b
	double delx 	= x2-x1;
	double dely 	= y2-y1;
	double b 	= y1 - (dely/delx)*x1;
	return (xaim*(dely/delx)+b) ;  		
}


// Return E field which is directly read from TCAD simulation
// and fill edges values
TH1D* EfieldInterpolator::loadEfieldFromDat(std::string fname, bool fillEdges ){
    TH1D* hefieldz = new TH1D( "hefieldz", "hefieldz",m_sensorDepth +1,-0.5, m_sensorDepth + 0.5);
    std::ifstream in;
    in.open(fname);
    TString z,e;
    int nlines =0;
    ATH_MSG_INFO("Load E field from " << fname);
    while (1) {
    	in >> z >> e;
    	if (!in.good()) break;
    	if (nlines < 3) printf("e=%s=%f, z=%s=%f  \n",e.Data(), e.Atof(),z.Data(), z.Atof() );
    	nlines++;
            hefieldz->Fill(z.Atof(), e.Atof());
    }
    in.close();
    if(fillEdges) EfieldInterpolator::fillEdgeValues(hefieldz);
    return hefieldz;
}

// original TCAD simulations given as txt (.dat) files (zVal efieldVal)
std::string EfieldInterpolator::loadTCADfiles(std::string targetList)
{
        bool isIBL = true;
	TString tl = targetList;
        TString fName   = targetList;
        fName.ReplaceAll(".txt", "_toTTree.root");
        fName  =fName.Remove(0,fName.Last('/')+1); //Remove prepending path and store in run directory
        TFile* bufferTCADtree = new TFile(fName.Data() ,"RECREATE");

	if(tl.Length() < 1 ){
		tl = "list_TCAD_efield_maps.txt";
		ATH_MSG_WARNING("No List to load! Set default: " << tl.Data());
	}
        std::ifstream in;
  	TTree *tcadtree     = new TTree("tcad","All TCAD E field simulations stored as individual events in tree");
	double voltage = -1.;
	double fluence = -1.;
	std::vector<double> efield;
	std::vector<Int_t> pixeldepth;
	tcadtree->Branch("voltage"	,&voltage, "voltage/D" );
  	tcadtree->Branch("fluence"	,&fluence,"fluence/D");
  	tcadtree->Branch("efield"	,&efield );
  	tcadtree->Branch("pixeldepth"	,&pixeldepth);
	//Get vetcor of {filename, fluence, bias voltage}
	std::vector<std::vector<TString>> infile = list_files(tl); 
	TString z,e;
	TString tmp = "";
	for(uint ifile = 0; ifile<infile.size(); ifile++){
		tmp = infile.at(ifile).at(0);
   		in.open(infile.at(ifile).at(0));
                // Number format eg 10e14 also 1.2e13
		//fluence = (infile.at(ifile).at(1).ReplaceAll("e14","")).Atof();
		fluence = (infile.at(ifile).at(1)).Atof();
                fluence = fluence * 1e-14; //choose fluence e14 as default unit
		voltage = infile.at(ifile).at(2).Atof();
   		Int_t nlines = 0;
		efield.clear();
		pixeldepth.clear();
   		while (1) {
      			in >> z >> e;
      			if (!in.good()){
                            ATH_MSG_DEBUG("Break for file No. " << ifile << ": "<< infile.at(ifile).at(0) <<" . After " << nlines << " steps");
                            break;
                        }
                        ATH_MSG_DEBUG("Reading input line: fluence=" << (infile.at(ifile).at(1)).Data() << fluence << " voltage=" << voltage <<  " e="<< e.Atof() <<"="<< e.Data() << ", z="<< (int) z.Atof() <<"="<< z.Data() <<"  in file = "<< ifile );
      			nlines++;
			efield.push_back(e.Atof());
			pixeldepth.push_back((int) z.Atof());
                        if(z.Atof() > 200 ) isIBL = false; // Pixel depth to huge to be IBL 
  		}
                bufferTCADtree->cd();
		tcadtree->Fill();
		in.close();
	}
        if(!isIBL){
            ATH_MSG_INFO("Pixel depth read from file too big for IBL. Set to B layer, depth = 250um\n");
            m_sensorDepth = 250;
        }
        bufferTCADtree->cd();
        tcadtree->Write();	
        bufferTCADtree->Close();
	return fName.Data();
}

std::vector<std::vector<TString>> EfieldInterpolator::list_files(TString fileList_TCADsamples)
{	
	std::vector<std::vector<TString>> filelist;	
	TString tmpname = "";
	TString tmpfluence = "";
	TString tmpvolt = "";
	std::vector<TString> vstr;
        std::ifstream in;
	ATH_MSG_DEBUG("Try to open: " << fileList_TCADsamples.Data());
	in.open(fileList_TCADsamples);
	int nlines = 0;
	while (1) {
      		in >> tmpname >> tmpfluence >> tmpvolt ;
		nlines++;
      		if (!in.good()) break;
		if (tmpname.BeginsWith('#')) continue;
		if (tmpname.EndsWith(".dat")){
			vstr.push_back(tmpname);
			vstr.push_back(tmpfluence);
			vstr.push_back(tmpvolt);
 			filelist.push_back(vstr);
			ATH_MSG_DEBUG("Found and load:"<< tmpfluence.Atof() <<" neq/cm2 " << tmpvolt.Atof() <<  "V - " << tmpname.Data());
			vstr.clear();
		}else{
			ATH_MSG_WARNING("Wrong extension: "<< tmpname.Data() <<" -- check input " );
		}
	}
	in.close();
	return filelist;
}

// Return path to file containing tree
// Final tree is restructured providing e field value as function of fluence, voltage and pixeldepth
std::string EfieldInterpolator::createInterpolationFromTCADtree(std::string fTCAD){ 
        TString tmpfinter = fTCAD;
        tmpfinter.ReplaceAll("toTTree","toInterpolationTTree");
	TFile* faim = new TFile(tmpfinter, "Recreate");
	TFile* ftreeTCAD = new TFile(fTCAD.c_str());
	TTreeReader myReader("tcad", ftreeTCAD);
	TTreeReaderValue<double> 		involtage(myReader	, "voltage"	);
	TTreeReaderValue<double> 		influence(myReader	, "fluence"	);
	TTreeReaderValue<std::vector<double>> 	inefield(myReader	, "efield"	);
	TTreeReaderValue<std::vector<int>> 		inpixeldepth(myReader   , "pixeldepth");
	// Get Data from TCAD tree
	// Loop tree once to initialize values
	// Finding which values for fluence and bias voltage exist
	// do not hardcode values to maintain compatibility with new simulations available
	std::vector<double> allFluences; // serves as x-axis
	std::vector<double> allVoltages; // serves as y-axis
	std::vector<double> allEfield; 
	int ne = 0;
	double tmpflu, tmpvol;
	while(myReader.Next()){
		tmpflu = *influence;
		tmpvol = *involtage;
                //Check if (double) value of fluence and bias voltage is already saved: if not save
                if( std::find_if(allFluences.begin(), allFluences.end(), [&tmpflu](const double &b) { return (std::abs( tmpflu - b) < 1E-6); } ) == allFluences.end()) allFluences.push_back(tmpflu); 
                if( std::find_if(allVoltages.begin(), allVoltages.end(), [&tmpvol](const double &b) { return (std::abs( tmpvol - b) < 1E-6); } ) == allVoltages.end()) allVoltages.push_back(tmpvol); 
		ne++;
	}
	//put into ascending order
        std::sort(allFluences.begin(), allFluences.end());	
	std::sort(allVoltages.begin(), allVoltages.end());	
	for(uint i=0; i<allFluences.size();i++ ) ATH_MSG_DEBUG("fluences recorded: "<< allFluences.at(i));
	for(uint i=0; i<allVoltages.size();i++ ) ATH_MSG_DEBUG("voltages recorded: "<< allVoltages.at(i));
	std::vector<double> tmpef;	
	myReader.Restart();	//available from ROOT 6.10.
        //Exclude TCAD efield values close to sensor edge
        int leftEdge = 3; //unify maps - different startting points from z=1 to z=2 and z=3. Simulation not reliable at edges, skip first and last
        int rightEdge= m_sensorDepth-2; 
	std::vector<int> tmpz; 
	int nz = rightEdge - leftEdge;
	// Temporary saving to avoid nesting tree loops
	// ie read all z values at once -> add to each branch-param dimension for z
	std::vector<double> 		zpixeldepth(nz, -1);
	std::vector<std::vector<double>> 	zfluence(nz,std::vector<double>( ne, -1));
	std::vector<std::vector<double>> 	zvoltage(nz,std::vector<double>( ne, -1));
	std::vector<std::vector<double>> 	zefield(nz, std::vector<double>( ne, -1));
	std::vector<std::vector<std::vector<double>>> zefieldmap(nz,std::vector<std::vector<double>>(allFluences.size(), std::vector<double>(allVoltages.size(), -1) )); 
	int iev = 0;
        ATH_MSG_INFO("Access TTreeReader second time\n");
               
	while(myReader.Next()){
		
	    tmpz = *inpixeldepth; //Pixeldepth of current TCAD map
	    ATH_MSG_DEBUG("Number of available z values = " << tmpz.size());
            if(tmpz.at(0) > leftEdge ) ATH_MSG_WARNING("Map starting from high pixeldepth = " << tmpz.at(0) << ". Might trouble readout.");
	    for(int iz = leftEdge; iz < rightEdge; iz++){
		int index = 0;
		//Safety check: 
                //files starting from z=1, z=2 or z=3
		//determine correct index to match sensor depth
                ATH_MSG_DEBUG("Access tmpz \n");
                ATH_MSG_DEBUG("Adapt index shift \n");
		while( (tmpz.at(index) != iz) && (index < nz) ){
                        ATH_MSG_DEBUG("Adapt possible index shift for missing edge values: pixeldepth tree = " << nz << " current index = " << index);
 			index++;
		}
		if(iz%2 ==0) ATH_MSG_DEBUG("Index "<< index <<" - iev "<< iev <<" - iz " << iz );
		tmpflu = *influence;
		tmpvol = *involtage;
		tmpef  = *inefield;
        	zfluence.at(iz-leftEdge).at(iev) = tmpflu; // assign value to certain pixeldepth(z)
        	zvoltage.at(iz-leftEdge).at(iev) = tmpvol;
        	zefield .at(iz-leftEdge).at(iev) = tmpef.at(index) ;
		((zefieldmap.at(iz-leftEdge)).at(isContainedAt(allFluences, tmpflu))).at(isContainedAt(allVoltages,tmpvol)) = tmpef.at(index);
                ATH_MSG_DEBUG ("Event #" << iev << "-z="<< iz << ": fluence ="<< tmpflu <<" voltage=" << tmpvol <<", E="<< tmpef.at(index));
	}
        iev++;
    }
    ATH_MSG_DEBUG("# Start filling interpolation tree \n");
    //Filling the interpolation tree
    faim->cd(); 
    TTree *tz_tmp     	= new TTree("tz","All TCAD E field simulations stored splitted by pixel depth");
    double 		pixeldepth = -1.;
    std::vector<double> 	fluence;
    std::vector<double> 	voltage;
    std::vector<double> 	xfluence;
    std::vector<double> 	yvoltage;
    std::vector<double> 	efield; 	
    std::vector<std::vector<double>> efieldfluvol; 	
    
    tz_tmp->Branch("pixeldepth"	,&pixeldepth);
    tz_tmp->Branch("voltage"	,&voltage);
    tz_tmp->Branch("fluence"	,&fluence);
    tz_tmp->Branch("yvoltage"	,&yvoltage);
    tz_tmp->Branch("xfluence"	,&xfluence);
    tz_tmp->Branch("efield"		,&efield );
    tz_tmp->Branch("efieldfluvol"	,&efieldfluvol);
    for(int iz = leftEdge; iz < rightEdge; iz++ ){
    	pixeldepth 	= iz;
    	fluence 	= zfluence.at(iz-leftEdge);
    	voltage		= zvoltage.at(iz-leftEdge);
    	efield		= zefield .at(iz-leftEdge);   
    	efieldfluvol	= zefieldmap.at(iz-leftEdge);
    	xfluence = allFluences;
    	yvoltage = allVoltages;
    	ATH_MSG_DEBUG("Fill tree for z ="<< iz <<" pd=" << pixeldepth);	
        faim->cd();
    	tz_tmp->Fill();
    }
                    
   //Save new Interpolation tree
    faim->cd();
    tz_tmp->Write();
    faim->Close();
    m_fInter =  tmpfinter.Data();
    return m_fInter;
} 

// Retrieve fluence values corresponding to a fixed voltage or viceversa if regular order == false
int EfieldInterpolator::fillXYvectors(std::vector<double> vLoop,int ifix, std::vector<std::vector<double>> v2vsv1, std::vector<double> &xx, std::vector<double> &yy, bool regularOrder ){
	yy.clear();
	xx.clear();
	int nfills = 0;
	if(regularOrder){
		for(uint ie = 0; ie <  v2vsv1.size(); ie++ ){
			double ef = v2vsv1.at(ie).at(ifix); // different fluences for volatge ifix
			if(ef > 0){
				yy.push_back(ef);
				xx.push_back(vLoop.at(ie));
				nfills++;
			}else{
				ATH_MSG_DEBUG("E field value not available for Phi=" << vLoop.at(ie) <<" index Vol= " << ifix);
                                //Values not ordered in a regular fluence-bias voltage grid 
			}
		}
	}else{
		for(uint ie = 0; ie < v2vsv1.at(0).size(); ie++ ){
			double ef = v2vsv1.at(ifix).at(ie); 
			if(ef > 0){
				yy.push_back(ef);
				xx.push_back(vLoop.at(ie));
				nfills++;
			}else{
				ATH_MSG_DEBUG("E field value not available for Phi="<< vLoop.at(ifix) << " U="<<  vLoop.at(ie));
                                //Values not ordered in a regular fluence-bias voltage grid 
			}
		}
	}
	
	return nfills;
}

//Final computation of efield according to method specified
double EfieldInterpolator::estimateEfieldLinear(double aimVoltage){
    return aimVoltage/(float) m_efieldProfile->GetNbinsX() * 10000; //10^4 for unit conversion 
}

//Interpolate following inverse distance weighted Interpolation
double EfieldInterpolator::estimateEfieldInvDistance(std::vector<double> vvol, std::vector<double> vflu, std::vector<std::vector<double>> vfluvvol, double aimFlu, double aimVol, double measure)
{
    ATH_MSG_WARNING("Use interpolation method _Inverse distance weighted_ - guarantees positive E field but no reliable interpolation");
    double weight     = 0.;
    double meanEf     = 0.;
    double distance   = 1.;
    double efEntry    = 0.;
    //Loop available efield values for fluence/voltage - take weighted mean
    for(uint ivol = 0; ivol < vvol.size(); ivol++ ){
        for(uint iflu = 0; iflu < vflu.size(); iflu++ ){
            efEntry = vfluvvol.at(iflu).at(ivol);
            if(efEntry > 0. ){ //Otherwise (-1), TCAD not available
                distance = relativeDistance(aimVol, aimFlu, vvol.at(ivol), vflu.at(iflu) );
                if( distance < 0.00001 ) return efEntry;         //fluence and voltage almost correpsond to available TCAD simulation 
                meanEf += efEntry * TMath::Power( (1./distance), measure);
                weight += TMath::Power( (1./distance),measure);
            }
        }
    }
    return (meanEf/weight);
}

// Interpolate using cubic splines
// E efield values given as function of fluence and bias voltage (vvol, vflu)
// interpolate to value for aimFluence and aimVoltage
double EfieldInterpolator::estimateEfield(std::vector<double> vvol, std::vector<double> vflu, std::vector<std::vector<double>> vfluvvol, double aimFlu, double aimVol, std::string prepend, bool debug){
	ATH_MSG_DEBUG("Estimating efield");	
	std::vector<double> evol;          // e field values for fixed voltages inter- or extrapolated to fluence of interest     
	std::vector<double> vvolWoEp;      // fixed voltages values for which no extrapolation is used to obatin E field in between fluences
	std::vector<double> evolWoEp;
	//Loop the voltages
	for(uint ifix = 0; ifix < vvol.size(); ifix++  ){
		std::vector<double> vx;// = new std::vector<double> ;
		std::vector<double> vy;// = new std::vector<double>;
		double  efflu = -1.;
		int availableTCADpoints = fillXYvectors(vflu, ifix,  vfluvvol,  vx, vy);
                ATH_MSG_DEBUG("Number of available TCAD points for voltage " << vvol.at(ifix) << ": " << availableTCADpoints );
		TString name = "FluenceEfield_";
		name +=  ifix;
		name += "FixVol";
		name += TString::Format("%.0f",vvol.at(ifix));
                name += "-aimFlu";
                name += TString::Format("%.1f",aimFlu);
                name += "-aimVol";
                name += TString::Format("%.0f",aimVol);

		TGraph* tmpgr = new TGraph(CastStdVec(vx),CastStdVec(vy));
		tmpgr->SetTitle(name.Data());
                if(isInterpolation(vx, aimFlu)){
                    name+="_ip";
                }else{
                    name+="_ep";
                }       
		if(m_useSpline ){
		    efflu = tmpgr->Eval(aimFlu,0,"S");
		}else{
		    efflu = tmpgr->Eval(aimFlu); //linear extrapolation
		}
                if(debug){
                    TString aimFile = m_fInter;
                    aimFile.ReplaceAll(".root", "_debug.root");
                    aimFile.ReplaceAll(".root", prepend );
                    aimFile+= name;
                    aimFile+=".root";
                    tmpgr->SaveAs(aimFile);
                }
                if(isInterpolation(vx, aimFlu)){
                    // try without extrapolation: skip extrapolated values
                    vvolWoEp.push_back(vvol.at(ifix)); 
                    evolWoEp.push_back(efflu);
                }
		
                delete tmpgr;
		evol.push_back(efflu); //includes extrapolated values
	}//end loop voltages
        
        //Check for debugging distribution of available E field values in fluence and 
        if(debug){ 
            saveTGraph(vvol, vflu, vfluvvol, aimFlu, aimVol, prepend);
        }
        // if possible to reach voltage of interest without any extrapolation in previous step, prefer this
        if(isInterpolation(vvolWoEp, aimVol) && vvolWoEp.size() > 1 ){
	    vvol = vvolWoEp;
	    evol = evolWoEp;
        }else{
            ATH_MSG_WARNING("E field created on extrapolation. Please check if reasonable!");
        }	
        
        TString name = "VoltageEfield";
        name += "-aimFlu";
        name += TString::Format("%.1f",aimFlu);
        name += "-aimVol";
        name += TString::Format("%.0f",aimVol);
	double aimEf = -1;
	TGraph* tmpgr = new TGraph(CastStdVec(vvol),CastStdVec(evol) );		 
	tmpgr->SetTitle(name.Data());
	if(isInterpolation(vvol, aimVol)){
            name+="_ip";
        }else{
            name+="_ep";
        }
	if(m_useSpline){
		aimEf = tmpgr->Eval(aimVol,0,"S");
	}else{
		aimEf = tmpgr->Eval(aimVol); //linear extrapolation
	}	
        if(debug){
            TString aimFile = m_fInter;
            aimFile.ReplaceAll(".root", "_debug.root");
            aimFile.ReplaceAll(".root", prepend );
            aimFile+= name;
            aimFile+=".root";
            tmpgr->SaveAs(aimFile);
        }
	delete tmpgr;
	return aimEf;
}

//Save all E field values as function of fluence and bias voltage for debugging
void EfieldInterpolator::saveTGraph(std::vector<double> vvol, std::vector<double> vflu, std::vector<std::vector<double>> vfluvvol, double aimFlu, double aimVol, std::string prepend, bool skipNegative){
    TString name = "VoltageEfield";
    name += "-aimFlu";
    name += TString::Format("%.1f",aimFlu);
    name += "-aimVol";
    name += TString::Format("%.0f",aimVol);
    TGraph2D* tmpgr = new TGraph2D();		 
    tmpgr->GetYaxis()->SetTitle("voltage");
    tmpgr->GetXaxis()->SetTitle("fluence");
    tmpgr->SetTitle(name.Data());
    ATH_MSG_DEBUG("E field values: "<< vfluvvol.size() << " x " << vfluvvol.at(0).size() << ", flu(x)"<< vflu.size()<< ", vol(y)" << vvol.size() );
    int npoint = 0;
    for(uint ix = 0; ix < vfluvvol.size(); ix++){
        for(uint iy=0; iy < vfluvvol.at(ix).size(); iy++){
                printf("Set point %i, %f,%f,%f\n",npoint, vflu.at(ix), vvol.at(iy), vfluvvol.at(ix).at(iy) );
                if(vfluvvol.at(ix).at(iy) < 0){
                    if(!skipNegative) tmpgr->SetPoint(npoint, vflu.at(ix), vvol.at(iy), -1);
                }else{
                    tmpgr->SetPoint(npoint, vflu.at(ix), vvol.at(iy), vfluvvol.at(ix).at(iy) );
                }
                npoint++;
        }
    }
    TString aimFile = m_fInter;
    aimFile.ReplaceAll(".root", "_debugAvailableEfieldVals.root");
    aimFile.ReplaceAll(".root", prepend );
    aimFile+= name;
    aimFile+=".root";
    tmpgr->SaveAs(aimFile);
}

TH1D* EfieldInterpolator::createEfieldProfile(double aimFluence, double aimVoltage){
        if(!m_initialized){
            ATH_MSG_WARNING("ERROR: EfieldInterpolator not properly intialized from " << m_fInter);
            return NULL;
        } 
        if(aimFluence > 1e12) aimFluence = aimFluence/1e14; //adapt units - TCAD files save 20 for 20e14 neq/cm2
	TString title = "hefieldz"; 
	TString info  = "#Phi=";
	info += TString::Format("%.2f",aimFluence);
	info += "-U="; 
	info += TString::Format("%.0f",aimVoltage);
	info += ";Pixeldepth z [#mum]";
	info += ";E [V/cm]";
	m_efieldProfile = new TH1D(title, info, m_sensorDepth,-0.5,m_sensorDepth + 0.5); 
	double 			pixeldepth;
	std::vector<double> 		xfluence;
	std::vector<double> 		yvoltage;
	std::vector<std::vector<double>>	efieldfluvol; 	
        TFile* ftreeInterpolation = TFile::Open(m_fInter.c_str());
	TTreeReader myReader("tz", ftreeInterpolation);
	TTreeReaderValue<std::vector<double>> 		involtage(myReader	, "yvoltage"	);
	TTreeReaderValue<std::vector<double>> 		influence(myReader	, "xfluence"	);
	TTreeReaderValue<std::vector<std::vector<double>>> 	inefield(myReader	, "efieldfluvol"	);
	TTreeReaderValue<double> 			inpixeldepth(myReader   , "pixeldepth"	);
	int ientry = 0;
	while(myReader.Next()){
		ATH_MSG_DEBUG("TTree entry: " << ientry);
		pixeldepth 	= *inpixeldepth;		
        	xfluence	= *influence;
        	yvoltage	= *involtage;
		efieldfluvol	= *inefield;	
                // Check if interpolation is reliable based on given TCAD samples
                if(ientry < 2){
                    reliabilityCheck(aimFluence, xfluence, aimVoltage, yvoltage);
                }
		double aimEf =0.;
                switch(m_efieldOrigin)
                {
                    case interspline    : aimEf = estimateEfield(yvoltage, xfluence, efieldfluvol,aimFluence, aimVoltage )              ; break;
                    case interinvdist   : aimEf = estimateEfieldInvDistance(yvoltage, xfluence, efieldfluvol,aimFluence, aimVoltage)    ; break;
                    case linearField    : m_useSpline = false;  
                                          aimEf = estimateEfield(yvoltage, xfluence, efieldfluvol,aimFluence, aimVoltage )              ; break; 
                    case TCAD           : aimEf = estimateEfieldLinear(aimVoltage)                                                      ;
                                          if(aimEf < 0.) ATH_MSG_ERROR("TCAD E field negative at" << pixeldepth <<" !")                 ; break;
                }

                if(aimEf < 0.){ 
                    if(m_useSpline){
                        TString debugName = "negativeSplineZ"; 
                        debugName+= TString::Format("%.0f",pixeldepth);
		        aimEf = estimateEfield(yvoltage, xfluence, efieldfluvol,aimFluence, aimVoltage,debugName.Data(), true );
                        ATH_MSG_INFO("InterpolatorMessage: linearly interpolated e=" << aimEf << ", z=" << pixeldepth <<" Phi=,"<< aimFluence <<" U=" << aimVoltage);
                        m_useSpline = false;
                        m_efieldOrigin = linearField; // not as good as interpolation
                    }else{     
                        TString debugName = "negativeLinearZ"; 
                        debugName+= TString::Format("%.0f",pixeldepth);
		        aimEf = estimateEfield(yvoltage, xfluence, efieldfluvol,aimFluence, aimVoltage,debugName.Data(), true );
                        ATH_MSG_ERROR("InterpolatorMessage: spline and linear interpolation failed => InvDistWeighhted  e=" << aimEf << ", z=" << pixeldepth <<" Phi=,"<< aimFluence <<" U=" << aimVoltage);
                        m_efieldOrigin = interinvdist; // not as good as interpolation (linear or spline) but guaranteed to be positive
                    }

                    myReader.Restart();
                }
		m_efieldProfile->SetBinContent(m_efieldProfile->FindBin(pixeldepth), aimEf ); 
		ientry++;
	}
	ftreeInterpolation->Close(); 
	//Check edge values
        ATH_MSG_DEBUG("Fill edges");
	fillEdgeValues(m_efieldProfile);
        scaleIntegralTo(m_efieldProfile,aimVoltage*10000, 2,m_sensorDepth );  //exclude first and last bin
        TString newtitle =  m_efieldProfile->GetTitle();
        switch(m_efieldOrigin)
        {
            case interspline    :newtitle +=" spline" ; break;
            case interinvdist   :newtitle +=" inverse distance" ; break;
            case linearField    :newtitle +=" linear" ; break;
            case TCAD           :newtitle +=" TCAD" ; break;
        }

        m_efieldProfile->SetTitle(newtitle.Data());
	ATH_MSG_DEBUG("Created Efield");
	m_efieldProfile->SetLineWidth(3) ;
	m_efieldProfile->SetLineStyle(2) ;
	m_efieldProfile->SetLineColor(4) ;
	m_efieldProfile->SetStats(0) ;
	return m_efieldProfile;
}

//First few and last few bins of TCAD maps not filled due to edge effect - fill with extrapolation
void EfieldInterpolator::fillEdgeValues(TH1D* hin){
	int nBins = hin->GetNbinsX();
	//Check first and last bins if filled
	// if not extrapolate linearly from two neighbouring values
        // empty (or zero) e field values cause unphysical behaviour in ATHENA/Allpix
	for(int i = 5; i > 0; i--){
		//first bins
                float curval    = hin->GetBinContent(i);
                float binii     = hin->GetBinContent(i+1);
                float biniii    = hin->GetBinContent(i+2);  
                float bini      = hin->GetBinContent(i);
		if( (bini <0.01 && binii > 0.01 && biniii > 0.01) || ( (biniii-binii)/(float) binii *(binii-bini)/(float) binii <-0.2) ) {//either neighbour filled and bin negative, or edge detected, i.e. slope changes sign and relatively larger than middle entry
                    bini = extrapolateLinear(i+1, binii , i+2, biniii, i);
                    if (bini > 0 ){
                        hin->SetBinContent(i,bini ); 
                    }else{
                        ATH_MSG_WARNING("Could not correct bin "<< i <<" for zero or edge " );
                        if(curval <= 0.) hin->SetBinContent(i, 1.); //avoid negative Efield
                    }
		}else{
			ATH_MSG_INFO("No need to fill edge bin: " << i);
		}
                //Last bins = right hand edge
                curval    = hin->GetBinContent(nBins+1-i);
                binii     = hin->GetBinContent(nBins+1-i-1);
                biniii    = hin->GetBinContent(nBins+1-i-2);  
                bini      = hin->GetBinContent(nBins+1-i);
		if( (bini <0.01 &&  binii > 0.01 &&  biniii > 0.01) ||( (biniii-binii)/(float) binii *(binii-bini)/(float) binii < -0.2) ) {//left neighbour filled and bin below negative or slope changes sign and magnitude 
                        bini = extrapolateLinear(nBins+1-i-2, hin->GetBinContent(nBins+1-i-2) , nBins+1-i-1, hin->GetBinContent(nBins+1-i-1), nBins+1-i); 
			if (bini > 0.){
                            hin->SetBinContent(nBins+1-i, bini);
                        }else{
                            ATH_MSG_WARNING("Could not correct bin"<< nBins+1-i << " for zero or edge " );
                            if(curval <= 0.) hin->SetBinContent(nBins+1-i, 1.); //avoid negative Efield
                        }
		}else{
			ATH_MSG_INFO("No need to fill edge bin: " << (nBins-i) );
		}
	}
}

// Main function to be called to create E field of interest

TH1D* EfieldInterpolator::getEfield(double aimFluence, double aimVoltage){
	if(m_initialized){
		m_efieldProfile =  createEfieldProfile( aimFluence, aimVoltage);
	}else{
		ATH_MSG_WARNING("EfieldInterpolator not initialized! Not able to produce E field.");
	}
	return m_efieldProfile;
}

