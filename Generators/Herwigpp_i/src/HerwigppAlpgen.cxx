/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Herwigpp_i/Herwigpp.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////
//Function that reads Alpgen inputs, converting the Alpgen events files into a modern
//LHE file, while simultaneously identifying which commands need to be passed to
//Herwig++ based on the Alpgen parameters file.
//This function is just a slight modification of the AlpGenToLH.cc code from
//Contrib/AlpGen/ directory of the Herwig++ source tree. This function (along with 
//the helper functions below) should be updated
//whenever that code is updated. Contact: m.k.bugge@fys.uio.no
////////////////////////////////////////////////////////////////////////////////////////
StatusCode Herwigpp::readAlpgen() {


  bool usePowhegBoxConventions(true); // Control
  int  debugging(3);                  // To validate

  cout << "\n";
  cout << "------------------------------------------------------------------\n";
  cout << "  AlpGenToLH: Convert Alpgen Les Houches to Herwig++ Les Houches \n";
  cout << "                               v2.0-beta \n";
  cout << "------------------------------------------------------------------\n";
  cout << "\n";
  //char* prefix;

 
  //   if(argv[1]) { prefix = argv[1]; } else { 
  //     cout << "Use: ./AlpGenToLH [input string] [number of events (optional)]\n"; exit(1);
  //     cout << "Note: As of version 2, the .stat and _unw.par files are "
  // 	 << "required to read the generation parameters.\n";
  //   }
  int maxevents(0); int eventcount(0);
  // if(argc>2) { maxevents=(atoi(argv[2])); }

  string prefix = "alpgen_unw_events";
  
  string lheFilename    = prefix + ".lhe";
  string unwFilename    = "alpgen.unw_events";
  string unwparFilename = "inparmAlpGen.dat";
  //string statFilename   = string(prefix) + string(".stat");

  cout << "Opening input files ...\n";
  cout << "-----------------------\n";
  cout << "Unweighted events in "   << unwFilename    << ".\n"
       << "Generation settings in " << unwparFilename /*<< " and " << statFilename*/   << ".\n\n";

  ifstream unwStream;
  ifstream unwparStream;
  //ifstream statStream;
  ofstream lheStream;
  unwStream.open(unwFilename.c_str());
  unwparStream.open(unwparFilename.c_str());
  //statStream.open(statFilename.c_str());
  lheStream.open(lheFilename.c_str());
  if(!unwStream) { 
    ATH_MSG_ERROR("error: Failed to open input file "+unwFilename);
    return StatusCode::FAILURE;
  }
  if(!unwparStream) { 
    ATH_MSG_ERROR("error: Failed to open input file "+unwparFilename);
    return StatusCode::FAILURE;
  }
  //   if(!statStream) {
  //     cerr << "error: Failed to open input file " << statFilename << "\n";
  //     exit(1);
  //   }

  // ******************************************************************** //
  // Dump the AlpGen *_unw.par file into the LH header (it's not so big). //
  // ******************************************************************** //

  string tmpString;
  lheStream << "<LesHouchesEvents version =\"1.0\">\n";
  lheStream << "<!--\n";
  lheStream << "File generated using AlpGen and converted with AlpGenToLH \n";
  while(unwparStream) {
    getline(unwparStream,tmpString);
    lheStream << tmpString << "\n";
  }
  lheStream << "\n\n\n" << "-->\n";
  unwparStream.close();

  // ***************************************** //
  // Read in all relevant info from *_unw.par. //
  // ***************************************** //

  int    ihrd(-999);                       // AlpGen convention hard process code.
  double mc(-999.0),mb(-999.0),mt(-999.0),mw(-999.0),mz(-999.0),mh(-999.0);          // C, B, Top, W, Z & Higgs mass from *_unw.par
  double avgwgt(-999.0),errwgt(-999.0);              // Average weight and its error.
  int    unwev(-999);                      // Number of unweighted events.
  double totlum;                     // Effective luminosity.
  vector<double> parval(200,-999.0); // AlpGen parameters.
  vector<string> parstr(200,"----"); // AlpGen parameter (variable) names.
  vector<double> alpgenParticleMasses;

  unwparStream.open(unwparFilename.c_str());
  while(unwparStream) {
    getline(unwparStream,tmpString);
    if(tmpString.find("hard process code") != string::npos) {
      tmpString=tmpString.substr(0,tmpString.find("!"));      
      tmpString=alpgen_trim(tmpString);
      ihrd=atoi(tmpString.c_str());
    }
    if(tmpString.find("mc,mb,mt,mw,mz,mh") != string::npos) {
      tmpString=alpgen_trim(tmpString.substr(0,tmpString.find("!")));
      mc=atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
      alpgenParticleMasses.push_back(mc);
      tmpString=alpgen_trim(tmpString.substr(tmpString.find_first_of(" ")));
      mb=atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
      alpgenParticleMasses.push_back(mb);
      tmpString=alpgen_trim(tmpString.substr(tmpString.find_first_of(" ")));
      mt=atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
      alpgenParticleMasses.push_back(mt);
      tmpString=alpgen_trim(tmpString.substr(tmpString.find_first_of(" ")));
      mw=atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
      alpgenParticleMasses.push_back(mw);
      tmpString=alpgen_trim(tmpString.substr(tmpString.find_first_of(" ")));
      mz=atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
      alpgenParticleMasses.push_back(mz);
      tmpString=alpgen_trim(tmpString.substr(tmpString.find_first_of(" ")));
      mh=atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
      alpgenParticleMasses.push_back(mh);
    }
    if(tmpString.find("Crosssection +- error (pb)") != string::npos) {
      tmpString=alpgen_trim(tmpString.substr(0,tmpString.find("!")));
      avgwgt=atof(alpgen_trim(tmpString.substr(0,tmpString.find(" "))).c_str());
      errwgt=atof(alpgen_trim(tmpString.substr(tmpString.find(" "))).c_str());
    }
    if(tmpString.find("unwtd events, lum (pb-1)") != string::npos) {
      tmpString=alpgen_trim(tmpString.substr(0,tmpString.find("!")));
      unwev=atoi(alpgen_trim(tmpString.substr(0,tmpString.find(" "))).c_str());
      totlum=atof(alpgen_trim(tmpString.substr(tmpString.find(" "))).c_str());
    }
  }

  if (mc == -999.0 || avgwgt == -999.0 || unwev == -999 || ihrd == -999) {
    ATH_MSG_ERROR("Could not extract all necessary parameters from the file "+unwparFilename);
    return StatusCode::FAILURE;
  }

  if(maxevents > unwev) { 
    cout << "-------------------------------\n";
    cout << "requested " << maxevents << " > " << unwev << " (contained in file), will use all events.\n"; maxevents = 0; }

  if(debugging>=4) {
    cout << "\nDebugging initial reading of *_unw.par:\n";
    cout << "ihrd = " << ihrd << "\n";
    cout << "mc,mb,mt,mw,mz,mh = "
	 <<  mc << "  " <<  mb << "  "
	 <<  mt << "  " <<  mw << "  "
	 <<  mz << "  " <<  mh << "\n";
    cout << "Cross section +/- error = "
	 <<  avgwgt << " +/- " << errwgt << "\n";
    cout << "Number of unweighted events = " <<  unwev << "\n";
    cout << "Effective luminosity = " <<  totlum << "\n";
  }
  unwparStream.close();

  unwparStream.open(unwparFilename.c_str());

  double minETclus(-999.0),minRclus(-999.0),maxETAclus(-999.0),exclusive(-999.0);

  int index;
  while(unwparStream) {
    getline(unwparStream,tmpString);

    if(debugging>=4) cout << "\nDebugging reading paramters in *_unw.par:\n";
    if(debugging>=4) cout << "File says: " << tmpString << "\n";

    if((tmpString.find("!")==string::npos && tmpString.find("|")==string::npos)|| //added the treatment of "|" due to a typo in some input datasets on the grid
       tmpString.find("hard process code")!=string::npos||
       tmpString.find("mc,mb,mt,mw,mz,mh")!=string::npos||
       tmpString.find("Crosssection +- error (pb)")!=string::npos||
       tmpString.find("unwtd events, lum (pb-1)")!=string::npos) continue;

    tmpString=alpgen_trim(tmpString);
    index = atoi((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
    tmpString=alpgen_trim(tmpString.substr(tmpString.find_first_of(" ")));

    if (index == 501) {
      minETclus = atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
      continue;
    }
    if (index == 502) {
      minRclus = atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
      continue;
    }
    if (index == 503) {
      maxETAclus = atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
      continue;
    }
    if (index == 504) {
      exclusive = atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
      continue;
    }
    

    parval[index]=atof((tmpString.substr(0,tmpString.find_first_of(" "))).c_str());
    tmpString=alpgen_trim(tmpString.substr(tmpString.find_first_of("!")+1));
    parstr[index]=tmpString;
    if(debugging>=4) cout << "We say:    "
		          << index << "  "
		          << parval[index] << "  "
		          << parstr[index] << "\n\n\n\n\n";
  }
  unwparStream.close();

  // Variables defined in parval array read from *_unw.par:

  // PDG codes for the beam particles.
  int idbmup[2]={0,0};
  if(alpgen_parstrToparval("ih1",&parstr,&parval)==1)       idbmup[0] =  2212;
  else if(alpgen_parstrToparval("ih1",&parstr,&parval)==-1) idbmup[0] = -2212;
  else idbmup[0] =  2212;
  if(alpgen_parstrToparval("ih2",&parstr,&parval)==1)       idbmup[1] =  2212;
  else if(alpgen_parstrToparval("ih2",&parstr,&parval)==-1) idbmup[1] = -2212;
  else idbmup[1] =  2212;

  // Energies of the beam particles --- implementation implicitly assumes
  // these are equal!
  double ebmup[2]={0,0};
  ebmup[0]=alpgen_parstrToparval("ebeam",&parstr,&parval);
  ebmup[1]=ebmup[0];

  // LH accord pdf info variables for <init> block:
  int pdfgup[2];
  pdfgup[0]=-1;  // Simply set to 1 as in POWHEG-BOX.
  pdfgup[1]=-1;
  int pdfsup[2];
  // LHAPDF index: (note in POWHEG-BOX it is set to just -1).
  int ndns = int(alpgen_parstrToparval("ndns",&parstr,&parval));
  pdfsup[0]=alpgen_ndnsToLHAPDF(ndns); 
  pdfsup[1]=pdfsup[0];
  // LH accord flag defining weight scheme:
  // N.B. AlpGen alpsho.f UPINIT uses idwtup = 3 (this is likely better from the
  // point of view of combining events of diff multiplicity together in real life
  // i.e. in ATLAS - so we should probably use it!).
  int idwtup( 3); // As in POWHEG-BOX withnegweights 0 mode: unit wgts +1 only.
  //  int idwtup(-4); // As in POWHEG-BOX withnegweights 1 mode: +/- |xsecup| wgts only.
  // Number of processes in the file (assume all one process as
  // with alpsho.f UPINIT).
  int nprup(1);
  // Cross section, it's error, the maximum weight in the file.
  double xsecup,xerrup,xmaxup;
  xsecup = avgwgt;
  xerrup = errwgt;
  xmaxup = xsecup;
  // Process id code (to be augmented by jet multiplicity - see just below).
  int lprup(ihrd*100);

  // We augment the process code (for the LH file only) by the number of
  // (light) jets, just in case we end up connecting many different files
  // to the shower MC in parallel (otherwise it likely won't distinguish 
  // between X+0,1,2,3,...,n jet processes).
  int njets(int(alpgen_parstrToparval("njets",&parstr,&parval)));
  lprup+=njets;

  // Write out some bits of info to the screen.
  cout << "No. of jets: "      << njets << "\n";
  cout << "Total xsec in pb (all processes): " << scientific
       << xsecup << " +/- " << xerrup << "\n\n";

  // NOW write out <init> block:
  lheStream << "<init>\n";
  lheStream << setw(9) << idbmup[0];
  lheStream << setw(9) << idbmup[1];
  lheStream << scientific << setprecision(5) << setw(13) << ebmup[0];
  lheStream << scientific << setprecision(5) << setw(13) << ebmup[1];
  lheStream << setw(7) << pdfgup[0];
  lheStream << setw(7) << pdfgup[1];
  if(usePowhegBoxConventions) {
    lheStream << setw(7) << -1;
    lheStream << setw(7) << -1;
  } else {
    lheStream << setw(7) << pdfsup[0];
    lheStream << setw(7) << pdfsup[1];
  }
  lheStream << setw(7) << idwtup;
  lheStream << setw(7) << nprup << "\n"; 

  lheStream << scientific << setprecision(5) << setw(13) << xsecup;
  lheStream << scientific << setprecision(5) << setw(13) << xerrup;
  if(usePowhegBoxConventions)
    lheStream << scientific << setprecision(5) << setw(13) << 1.0;
  else // else put in more info (xmaxup=xsecup).
    lheStream << scientific << setprecision(5) << setw(13) << xmaxup;
  lheStream << setw(7) << lprup << "\n";

  lheStream << "</init>\n";

  // ************************************************* //
  // All done with the init section of the LH file!    //
  // ************************************************* //

  // ***************************************************************** //
  // To write the HW++ .in file we have everything we could possibly   //
  // want except maybe the QCD coupling and no. of loops for running.  //
  // These are the only numbers we get / use from *.stat now.          //
  // ***************************************************************** //

  double aqedup(-999),aqedStat(-999);
  double aqcdup(-999),aqcdStat(-999);
  int nloop(-999);

  //   // Fish around for the QCD and QED alphas in .stat.
  //   while(statStream) {
  //     getline(statStream,tmpString);
  //     if(tmpString.find("as(MZ)") != string::npos) {
  //       aqcdup=atof(alpgen_trim(tmpString.substr(tmpString.find_last_of("=")+1,
  // 					tmpString.length())).c_str());
  //       tmpString=alpgen_trim(tmpString.substr(tmpString.find_first_of("nloop=")));
  //       tmpString=alpgen_trim(tmpString.substr(tmpString.find_first_of(" ")));
  //       tmpString=alpgen_trim(tmpString.substr(0,tmpString.find_first_of("]")));
  //       nloop=atoi(tmpString.c_str());
  //     }
  //     if(tmpString.find("aem(mZ)") != string::npos) {
  //       tmpString=tmpString.substr(tmpString.find("aem(mZ)="));
  //       tmpString=tmpString.substr(tmpString.find_first_of(" "));
  //       tmpString=alpgen_trim(tmpString);
  //       aqedup=atof(tmpString.c_str());
  //       aqedup=1/aqedup;
  //     }
  //   }
  //   statStream.close();


  //To eliminate the need for the .stat file from Alpgen (which is not part of a standard ATLAS Alpgen input event dataset)
  //we set the strong coupling based on the PDF number ndns and hard code the EM coupling        //<------- ask about the EM coupling
  //The EW coupling corresponds to what is given in the Alpgen documentation for iewopt=3, which should be default for all processes
  //It seems that the value set here is not actually used for anything!
  aqedup = 1.0/1.325070e+02;
  if (!alpgen_getAlpha_s(ndns,nloop,aqcdup)) {
    ATH_MSG_ERROR("ERROR: Could not determine alpha_s(MZ) and number of loops for running based on PDF number ndns = "+ndns);
    return StatusCode::FAILURE;
  }

  aqedStat=aqedup;
  aqcdStat=aqcdup;

  // Write out a couple more bits of info to the screen.
  cout << "aqcdup [as(MZ)]: " << aqcdup << "\n";
  cout << "nloop for as   : " << nloop << "\n";
  cout << "aqedup [inverse]: " << 1/aqedup << "\n";
  cout << "\n";

  StatusCode sc = alpgen_writeHWPPinFile(prefix,ihrd,unwev,pdfsup[0], /*idbmup[0], idbmup[1],*/ idwtup, //Commented parameters are not used in the current version
				  aqcdup,nloop,/*&alpgenParticleMasses,*/ &parval,&parstr,            //They were commented to avoid compiler warnings
				  minETclus,minRclus,maxETAclus,exclusive);

  if (sc.isFailure()) return sc;

  

  // *********************************************************** //
  // All done writing the HW++ .in file!                         //
  // *********************************************************** //

  // *********************************************************** //
  // Start reading AlpGen events and writing them as LH events:  //
  // *********************************************************** //

  int nupMax(20);

  // First line of an event contains:
  double nup(0),idprup(0),xwgtup(0),scalup(0);

  // Subsequent lines for particle content contain:
  vector<double> idup,istup;
  vector<double> mothup1,mothup2;  // WARNING: not implemented ... YET!
  vector<double> icolup1,icolup2;
  vector<double> vtimup,spinup;    // WARNING: not implemented (but safe).
  vector<vector<double> > pup;
  pup.resize(5);
  for (int ixx = 0; ixx <= 4; ++ixx)
    pup[ixx].resize(nupMax);

  // Initialise pup matrix (not really necessary):
  for(int jup=0; jup < nupMax; jup++)
    for(int ixx=0; ixx<5; ixx++) pup[ixx][jup]=0;

  // Control reading AlpGen file:
  int iup(0);
  int counter(0);
  bool readInWholeEvent(false),beginNewEvent(true);
  double stringdoub;

  // Needed as input to alpgen_doIndividualHardProcessAssignments only:
  int itopprc(alpgen_nInt(alpgen_parstrToparval("itopprc",&parstr,&parval)));

  while(unwStream && (eventcount < maxevents || maxevents==0)) { // So long as we haven't hit the EOF do ...

    if(beginNewEvent) {
      // Rest / set control variables:
      beginNewEvent=false;
      readInWholeEvent=false;
      counter=0;
      iup=0;
      // Reset variables for first line of LH event:
      nup=0; idprup=0; xwgtup=0; scalup=0; aqedup=0; aqcdup=0;
      // Reset variables for all individual particles in LH event:
      idup.clear()   ; istup.clear();
      mothup1.clear(); mothup2.clear();
      icolup1.clear(); icolup2.clear();
      vtimup.clear() ; spinup.clear();
      for(int jup=0; jup < nupMax; jup++)
	for(int ixx=0; ixx<5; ixx++) pup[ixx][jup]=0;
    }

    // Read in next thing starting from last position in
    // the file (int/real) as a double
    unwStream >> stringdoub;

    // counter counts the number of numbers read-in for a given
    // event. On starting to read a new event counter will be 0.
    counter++;

    switch (counter) {
    case  1: if(int(stringdoub)!=0) {
	// 	        if(int(stringdoub)%100==0)
	// 		  cout << "Processed " << fixed << setprecision(0)
	// 		       << stringdoub/unwev*100
	// 		       << " % of events ..." << "\r" << flush;
      }
      break;
    case  2: idprup = stringdoub;  break;
    case  3: nup    = stringdoub;  break;
    case  4: xwgtup = stringdoub;  break;
    case  5: scalup = stringdoub;  break;
      // N.B. There are no aqedup / aqcdup variables from AlpGen.

      // Initial state particle +z direction:
    case  6: idup.push_back(stringdoub)   ;
      istup.push_back(-1);  break;
    case  7: icolup1.push_back(stringdoub);
      mothup1.push_back(0); break; // ATTENTION: not given by AlpGen
    case  8: icolup2.push_back(stringdoub);
      mothup2.push_back(0); break; // ATTENTION: not given by AlpGen
    case  9: pup[2][iup]=stringdoub       ;
      pup[3][iup]=fabs(stringdoub) ; iup++ ; break;

      // Initial state particle -z direction:
    case 10: idup.push_back(stringdoub)   ;
      istup.push_back(-1);  break;
    case 11: icolup1.push_back(stringdoub);
      mothup1.push_back(0); break; // ATTENTION: not given by AlpGen
    case 12: icolup2.push_back(stringdoub);
      mothup2.push_back(0); break; // ATTENTION: not given by AlpGen
    case 13: pup[2][iup]=stringdoub;
      pup[3][iup]=fabs(stringdoub) ; iup++ ; break;
    }

    if(debugging<5) idprup=lprup;

    if(counter<14) continue;

    // Final state particles:
    if(counter==0+7*iup) idup.push_back(stringdoub);
    // istup gets assigned later on to just -1/+1 (I.S. / F.S.).
    if(counter==1+7*iup) { icolup1.push_back(stringdoub);
      mothup1.push_back(1.); } // ATTENTION: not given by AlpGen
    if(counter==2+7*iup) { icolup2.push_back(stringdoub);
      mothup2.push_back(2.); } // ATTENTION: not given by AlpGen
    if(counter==3+7*iup) pup[0][iup] = stringdoub;
    if(counter==4+7*iup) pup[1][iup] = stringdoub;
    if(counter==5+7*iup) pup[2][iup] = stringdoub;
    if(counter==6+7*iup) pup[4][iup] = stringdoub;
    if(counter==6+7*iup) istup.push_back(1.);
    if(counter==6+7*iup) iup+=1;
    if(counter==7*nup-1) readInWholeEvent = true; 

    for(int jup = 0; jup < nup; jup++) {
      pup[3][jup] = sqrt( alpgen_sqr(pup[4][jup]) + alpgen_sqr(pup[0][jup]) 
			  + alpgen_sqr(pup[1][jup]) + alpgen_sqr(pup[2][jup]) );
      vtimup.push_back(0.); // ATTENTION: not implemented - so taking 
      spinup.push_back(9.); // POWHEG-BOX default values (should be v.safe).
    }

    // ***************************************************************** //
    // Now consider assignments specific to individual hard processes:   //
    // ***************************************************************** //    

    if(readInWholeEvent) {
      StatusCode sc2 = alpgen_doIndividualHardProcessAssignments(ihrd, &nup, &idup, &istup,
							  &mothup1  , &mothup2,
							  &icolup1  , &icolup2,
							  &pup      ,
							  alpgenParticleMasses, itopprc);
      if (sc2.isFailure()) {
	ATH_MSG_ERROR("ERROR in alpgen_doIndividualHardProcessAssignments");
	return sc2;
      }
    }

    if(readInWholeEvent) {
      lheStream << "<event>\n";
      if(debugging>=5) {
	lheStream << nup    << "\t" << idprup << "\t" << xwgtup << "\t"
		  << scalup << "\t" << "0.007297352" << "\t" << "0.118\n";
      } else {
	// Bit about signing of xwgtup here is redundant as 
	// AlpGen only gives +ve weight events ...
	double signOfXwgtup = xwgtup >= 0 ? 1 : -1;
	xwgtup = idwtup==3 ? 1 : xsecup*signOfXwgtup;
	// N.B. There are no aqedup / aqcdup variables from AlpGen
	// events only the stat file has related information.
	aqedup=aqedStat;
	aqcdup=aqcdStat;
	if(usePowhegBoxConventions) aqedup=-1;
	lheStream << setw(7) << int(nup);
	lheStream << setw(7) << int(idprup);
	lheStream << scientific << setprecision(5) << setw(13) << xwgtup;
	lheStream << scientific << setprecision(5) << setw(13) << scalup;
	lheStream << scientific << setprecision(5) << setw(13) << aqedup;
	lheStream << scientific << setprecision(5) << setw(13) << aqcdup;
	lheStream << "\n";
      }
      for(int jup = 0; jup < nup; jup++) {
	if(debugging>=5) {
	  lheStream << idup[jup]     << "\t" << istup[jup]    << "\t"
		    << mothup1[jup]  << "\t" << mothup2[jup]  << "\t" 
		    << icolup1[jup]  << "\t" << icolup2[jup]  << "\t";
	  lheStream << pup[0][jup]   << "\t" << pup[1][jup]   << "\t"
		    << pup[2][jup]   << "\t" << pup[3][jup]   << "\t"
		    << pup[4][jup]   << "\t";
	  lheStream << "0"           << "\t" << "1\n";
	} else {
	  if(icolup1[jup]!=0) icolup1[jup]+=500;
	  if(icolup2[jup]!=0) icolup2[jup]+=500;
	  lheStream << setw(8) << int(idup[jup]);
	  lheStream << setw(6) << int(istup[jup])
		    << setw(6) << int(mothup1[jup])
		    << setw(6) << int(mothup2[jup])
		    << setw(6) << int(icolup1[jup])
		    << setw(6) << int(icolup2[jup]);
	  lheStream << scientific << setprecision(9) << setw(17) << pup[0][jup];
	  lheStream << scientific << setprecision(9) << setw(17) << pup[1][jup];
	  lheStream << scientific << setprecision(9) << setw(17) << pup[2][jup];
	  lheStream << scientific << setprecision(9) << setw(17) << pup[3][jup];
	  lheStream << scientific << setprecision(9) << setw(17) << pup[4][jup];
	  lheStream << scientific << setprecision(5) << setw(13) << vtimup[jup];
	  lheStream << scientific << setprecision(3) << setw(11) << vtimup[jup];
	  lheStream << "\n";
	}
      }
      lheStream << "</event>\n";
      eventcount++;
      if (eventcount % 1000 == 0)
	cout << "Processed " << eventcount << " events ..." << "\n";
      beginNewEvent=true;
    }
  }
  
  cout << "\n\n";

  if( maxevents!=0 ) {
    cout << "All done (" << maxevents << " out of " <<  unwev << " events).\n";
  } else { 
    cout << "All done (" << eventcount << " events).\n";
  }

  cout << "\n\n"
       << "Wrote a LH event file " << lheFilename
       << ".\n\n";
  

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Below follow the helper functions required by the readAlpgen() function.
//They are mostly copied directly or modified slightly from the AlpGenToLH.cc code, with the exception
//of alpgen_getAlpha_s which was added in order to eliminate the need for the .stat file produced by Alpgen
/////////////////////////////////////////////////////////////////////////////////////////////////////

int Herwigpp::alpgen_nInt(double x) {
  int theCeiling=int(ceil(x));
  int theFloor=int(floor(x));
  if((theCeiling-theFloor!=1&&theCeiling!=theFloor)
     ||theCeiling-x>1.0||x-theFloor>1.0||x<theFloor||x>theCeiling) {
    cout << "alpgen_nInt:\n"
	 << "Fatal double to integer conversion error.\n"
	 << "input double    = " << x << "\n"
	 << "integer ceiling = " << theCeiling << "\n"
	 << "integer floor   = " << theFloor << "\n"
	 << "Quitting ...";
    exit(1);
  }
  return (theCeiling-x) < (x-theFloor) ? theCeiling : theFloor;
}

double Herwigpp::alpgen_sqr(double x) { return x*x; }

int Herwigpp::alpgen_ndnsToLHAPDF(int ndns) {
  // The information in this function is based on 
  // subroutine PRNTSF from alplib/alppdf.f, LHAPDF's 
  // PDFsets.index and, finally, the .stat output that
  // results when the relevant ndns value is entered
  // in the input file.
  string Set("no PDF set found");
  //double Lambda_4(0),Lambda_5_2loop(0);
  //string Scheme("no PDF scheme");
  int    LHAPDFindex(-999);
  //string tmpString("");

  if(ndns==1) {
    Set = "CTEQ4M"       ; //Lambda_4 = 0.298 ; Lambda_5_2loop = 0.202 ; Scheme = "MS" ;
    LHAPDFindex =  19150;
  } else if(ndns==2) {
    Set = "CTEQ4L"       ; //Lambda_4 = 0.298 ; Lambda_5_2loop = 0.202 ; Scheme = "MS" ;
    LHAPDFindex =  19170;
  } else if(ndns==3) {
    Set = "CTEQ4HJ"      ; //Lambda_4 = 0.298 ; Lambda_5_2loop = 0.202 ; Scheme = "MS" ;
    LHAPDFindex = -99999;
  } else if(ndns==4) {
    Set = "CTEQ5M"       ; //Lambda_4 = 0.326 ; Lambda_5_2loop = 0.226 ; Scheme = "MS" ;
    LHAPDFindex =  19050;
  } else if(ndns==5) {
    Set = "CTEQ5L"       ; //Lambda_4 = 0.192 ; Lambda_5_2loop = 0.144 ; Scheme = "MS" ;
    LHAPDFindex =  19070;
  } else if(ndns==6) {
    Set = "CTEQ5HJ"      ; //Lambda_4 = 0.326 ; Lambda_5_2loop = 0.226 ; Scheme = "MS" ;
    LHAPDFindex = -99999;
  } else if(ndns==7) {
    Set = "CTEQ6M"       ; //Lambda_4 = 0.326 ; Lambda_5_2loop = 0.226 ; Scheme = "MS" ;
    LHAPDFindex=   10050;
  } else if(ndns==8) {
    Set = "CTEQ6L"       ; //Lambda_4 = 0.326 ; Lambda_5_2loop = 0.226 ; Scheme = "MS" ;
    LHAPDFindex =  10041;
  } else if(ndns==9) {
    Set = "CTEQ6L1"      ; //Lambda_4 = 0.215 ; Lambda_5_2loop = 0.167 ; Scheme = "MS" ;
    LHAPDFindex =  10042;
  } else if(ndns>=10&&ndns<=50) {
    Set = "CTEQ6xx"      ; //Lambda_4 = 0.326 ; Lambda_5_2loop = 0.226 ; Scheme = "MS" ;
    LHAPDFindex =  10150+(ndns-10);
  } else if(ndns==101) {
    Set = "MRST99"       ; //Lambda_4 = 0.321 ; Lambda_5_2loop = 0.220 ; Scheme = "MS" ;
    LHAPDFindex = -99999;
  } else if(ndns==102) {
    Set = "MRST01"       ; //Lambda_4 = 0.342 ; Lambda_5_2loop = 0.239 ; Scheme = "MS" ;
    LHAPDFindex = -99999;
  } else if(ndns==103) {
    Set = "MRST01"       ; //Lambda_4 = 0.310 ; Lambda_5_2loop = 0.214 ; Scheme = "MS" ;
    LHAPDFindex = -99999;
  } else if(ndns==104) {
    Set = "MRST01"       ; //Lambda_4 = 0.378 ; Lambda_5_2loop = 0.267 ; Scheme = "MS" ;
    LHAPDFindex = -99999;
  } else if(ndns==105) {
    Set = "MRST01J"      ; //Lambda_4 = 0.378 ; Lambda_5_2loop = 0.267 ; Scheme = "MS" ;
    LHAPDFindex = -99999;
  } else if(ndns==106) {
    Set = "MRST02LO"     ; //Lambda_4 = 0.215 ; Lambda_5_2loop = 0.167 ; Scheme = "MS" ;
    LHAPDFindex = -99999;
  } else if(ndns==201) {
    Set = "MSTW2008lo"   ; //Lambda_4 = 0.322 ; Lambda_5_2loop = 0.255 ; Scheme = "MS" ;
    LHAPDFindex =  21000;
  } else if(ndns==202) {
    Set = "MSTW2008nlo"  ; //Lambda_4 = 0.365 ; Lambda_5_2loop = 0.255 ; Scheme = "MS" ;
    LHAPDFindex =  21100;
  } else if(ndns>=203&&ndns<=242) {
    Set = "MSTW2008lo68cl"; //Lambda_4 = 0.322 ; Lambda_5_2loop = 0.255 ; Scheme = "MS" ;
    LHAPDFindex =  21000+(ndns-202);
  } else if(ndns==243) {
    Set = "MRST LO*" ; //Lambda_4 = 0.365 ; Lambda_5_2loop = 0.255 ; Scheme = "MS" ;
    LHAPDFindex =  20650;
  } else if(ndns==244) {
    Set = "MRST LO**" ; //Lambda_4 = 0.280 ; Lambda_5_2loop = 0.190 ; Scheme = "MS" ;
    LHAPDFindex =  20651;
  } else if(ndns==301 ) {
    Set = "CTQ6.6" ; //Lambda_4 = 0.326 ; Lambda_5_2loop = 0.226 ; Scheme = "MS" ;
    LHAPDFindex =  10550;
  } else if(ndns>=302&&ndns<=345) {
    Set = "CTQ66" ; //Lambda_4 = 0.326 ; Lambda_5_2loop = 0.226 ; Scheme = "MS" ;
    LHAPDFindex =  10550+(ndns-301);
  } else if(ndns==346) {
    Set = "CT09MC1" ; //Lambda_4 = 0.215 ; Lambda_5_2loop = 0.167 ; Scheme = "MS" ;
    LHAPDFindex =  10771;
  } else if(ndns==347) {
    Set = "CT09MC2" ; //Lambda_4 = 0.326 ; Lambda_5_2loop = 0.226 ; Scheme = "MS" ;
    LHAPDFindex =  10772;
  }

  cout << "-------------------------------\n";
  cout << "ndnsToLHAPDF found: \n";
  cout << "PDF set      = " << Set << "\n";
  cout << "ndns index   = " << ndns << "\n";
  cout << "LHAPDF index = " << LHAPDFindex << "\n";
  cout << "-------------------------------\n\n";
  return LHAPDFindex;

}


double Herwigpp::alpgen_parstrToparval(string varName, vector<string> * parstrPtr, vector<double> * parvalPtr) {
  for(unsigned int index=0; index<parvalPtr->size(); index++)
    if(varName==parstrPtr->at(index))
      return parvalPtr->at(index);

  return -999.0;
}

string Herwigpp::alpgen_trim(string theString) {
  unsigned int endStr    = theString.find_last_not_of(" ");
  unsigned int beginStr  = theString.find_first_not_of(" ");
  if(beginStr==0&&endStr==theString.length()-1) return theString; // No lead / trail spaces.
  theString = theString.substr(beginStr,endStr-beginStr+1);
  return theString;
}

StatusCode Herwigpp::alpgen_writeHWPPinFile(string prefix, int ihrd, int unwev, int lhapdf, /*int idbmup0, int idbmup1,*/ int idwtup,
				     double aqcdup, int nloop, /*vector<double> * massesPtr,*/ vector<double> * parvalPtr, vector<string> * parstrPtr,
				     double minETclus, double minRclus, double maxETAclus, double exclusive) {

  m_herwigCommandVector.push_back(string("\n#The following section was written by the function Herwigpp::readAlpgen() \n"));

  char* buf = new char[100]; //Buffer for number-to-string conversions

  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("# Create a LH event handler (set up & assigned below) ...   #"));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/EventHandlers"));
  m_herwigCommandVector.push_back(string("library LesHouches.so"));
  m_herwigCommandVector.push_back(string("create ThePEG::LesHouchesEventHandler theLesHouchesHandler"));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("# Create a LH reader (set up & assigned below) ...          #"));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/EventHandlers"));
  m_herwigCommandVector.push_back(string("library BasicLesHouchesFileReader.so"));
  m_herwigCommandVector.push_back(string("create Herwig::BasicLesHouchesFileReader theLHReader"));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("# Create an AlpGenHandler (set up & assigned below) ...     #"));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/Shower"));
  m_herwigCommandVector.push_back(string("library AlpGenHandler.so"));
  m_herwigCommandVector.push_back(string("create Herwig::AlpGenHandler AlpGenHandler"));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("# Create an LHAPDF (set up & assigned below) ...            #"));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/Partons"));
  m_herwigCommandVector.push_back(string("create ThePEG::LHAPDF thePDFset ThePEGLHAPDF.so"));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("############################################################"));
  m_herwigCommandVector.push_back(string("# Create a cuts object ...                                 #"));
  m_herwigCommandVector.push_back(string("############################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/EventHandlers"));
  m_herwigCommandVector.push_back(string("create ThePEG::Cuts   /Herwig/Cuts/NoCuts"));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("# Setup the LH event handler ...                            #"));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/EventHandlers"));
  m_herwigCommandVector.push_back(string("insert theLesHouchesHandler:LesHouchesReaders 0 theLHReader"));
  if(idwtup==3) {
    m_herwigCommandVector.push_back(string("set theLesHouchesHandler:WeightOption UnitWeight"));
  } else if(idwtup==-3) {
    m_herwigCommandVector.push_back(string("set theLesHouchesHandler:WeightOption NegUnitWeight"));
  } else if(idwtup==4) {
    m_herwigCommandVector.push_back(string("set theLesHouchesHandler:WeightOption VarWeight"));
  } else {
    m_herwigCommandVector.push_back(string("set theLesHouchesHandler:WeightOption VarNegWeight"));
  }
  m_herwigCommandVector.push_back(string("set theLesHouchesHandler:PartonExtractor /Herwig/Partons/QCDExtractor"));
  m_herwigCommandVector.push_back(string("set theLesHouchesHandler:CascadeHandler /Herwig/Shower/AlpGenHandler"));
  m_herwigCommandVector.push_back(string("set theLesHouchesHandler:HadronizationHandler /Herwig/Hadronization/ClusterHadHandler"));
  m_herwigCommandVector.push_back(string("set theLesHouchesHandler:DecayHandler /Herwig/Decays/DecayHandler"));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("# Set up the Evolver to veto hard emissions > scalup ...    #"));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/Shower"));
  m_herwigCommandVector.push_back(string("# MaxTry 100 sets the maximum number of times to try "));
  m_herwigCommandVector.push_back(string("# showering a given shower tree to 100. "));
  m_herwigCommandVector.push_back(string("# HardVetoMode Yes to veto emissions with pT greater than pT_max."));
  m_herwigCommandVector.push_back(string("# HardVetoScaleSource Read means pT_max comes from hepeup.SCALUP."));
  m_herwigCommandVector.push_back(string("# This is what you need to set _along_with_ HardVetoMode Yes in "));
  m_herwigCommandVector.push_back(string("# the case of Powheg external events _AND_ mc@nlo (we know this "));
  m_herwigCommandVector.push_back(string("# from looking at the *MCinput file that mc@nlo generates). "));
  m_herwigCommandVector.push_back(string("# MeCorrMode No turns off ME corrs."));
  m_herwigCommandVector.push_back(string("set Evolver:MaxTry               100"));
  m_herwigCommandVector.push_back(string("set Evolver:HardVetoMode         Yes"));
  m_herwigCommandVector.push_back(string("set Evolver:HardVetoScaleSource  Read"));
  m_herwigCommandVector.push_back(string("set Evolver:HardVetoReadOption   PrimaryCollision"));
  m_herwigCommandVector.push_back(string("set Evolver:MECorrMode           No"));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("# Set up the AlpGenHandler ...                              #"));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/Shower"));
  m_herwigCommandVector.push_back(string("set AlpGenHandler:MPIHandler  /Herwig/UnderlyingEvent/MPIHandler"));
  m_herwigCommandVector.push_back(string("set AlpGenHandler:RemDecayer  /Herwig/Partons/RemnantDecayer"));
  m_herwigCommandVector.push_back(string("set AlpGenHandler:Evolver     Evolver"));

  if (!m_useTwoLoopAlphas) {
    //Use these lines to get the "proper" alpha_s as governed by the PDF choice
    sprintf(buf,"%f",aqcdup);
    m_herwigCommandVector.push_back(string("set AlphaQCD:AlphaMZ       "+string(buf)));
    sprintf(buf,"%d",nloop);
    m_herwigCommandVector.push_back(string("set AlphaQCD:NumberOfLoops "+string(buf)));
    m_herwigCommandVector.push_back(string("set AlpGenHandler:ShowerAlpha  AlphaQCD"));
  } else {
    //Use these lines to get the two-loop alpha_s used by fHerwig
    m_herwigCommandVector.push_back(string("#Two-loop alpha_s as used in fHerwig"));
    m_herwigCommandVector.push_back(string("set /Herwig/Shower/AlphaQCD:InputOption LambdaQCD"));
    m_herwigCommandVector.push_back(string("set /Herwig/Shower/AlphaQCD:LambdaQCD       180.0*MeV"));
    m_herwigCommandVector.push_back(string("set /Herwig/Shower/AlphaQCD:NumberOfLoops 2"));
    m_herwigCommandVector.push_back(string("set /Herwig/Shower/AlphaQCD:LambdaOption Convert"));
    m_herwigCommandVector.push_back(string("set /Herwig/Shower/AlpGenHandler:ShowerAlpha  /Herwig/Shower/AlphaQCD"));
  }

  m_herwigCommandVector.push_back(string("# Calorimeter granularity settings used by GetJet algorithm")); 
  m_herwigCommandVector.push_back(string("set AlpGenHandler:NoCellsInRapidity 100")); 
  m_herwigCommandVector.push_back(string("set AlpGenHandler:NoCellsInPhi       60"));
  // AlpGen hard process code.
  m_herwigCommandVector.push_back(string("# AlpGen hard process code."));
  sprintf(buf,"%d",ihrd);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:ihrd        "+string(buf)));
  // Number of (light) jets.
  int njets(int(alpgen_parstrToparval("njets",parstrPtr,parvalPtr)));
  m_herwigCommandVector.push_back(string("# No. of light jets in AlpGen process (the \"extra\" ones).")); 
  sprintf(buf,"%d",njets);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:njets       "+string(buf)));
  // Mimimum jet pT use for generation.
  ////  double ptjmin(alpgen_parstrToparval("ptjmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:ptjmin      " << ptjmin << "*GeV"));
  // Mimimum parton-parton R-sep used for generation.
  double drjmin(alpgen_parstrToparval("drjmin",parstrPtr,parvalPtr));
  m_herwigCommandVector.push_back(string("# Mimimum parton-parton R-sep used for generation.")); 
  sprintf(buf,"%f",drjmin);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:drjmin      "+string(buf)));
  // Max |eta| for partons in generation.
  ////  double etajmax(alpgen_parstrToparval("etajmax",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:etajmax     " << etajmax << ""));
  // Also probably want these variables fed to AlpGenHandler too ---
  // they get set in the alpsho.f AHspar routine (note the list below
  // does not include some variables from AHspar because they are already
  // included in the above eg the PDFs are already handled so I removed
  // ndns and also ptjmin drjmin are written out for the AlpGenHandler above).
  ////  int ickkw(int(alpgen_parstrToparval("ickkw",parstrPtr,parvalPtr)));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:ickkw       " << ickkw << ""));
  int ihvy(int(alpgen_parstrToparval("ihvy",parstrPtr,parvalPtr)));
  m_herwigCommandVector.push_back(string("# heavy flavour in WQQ,ZQQ,2Q etc (4=c, 5=b, 6=t):"));
  sprintf(buf,"%d",ihvy);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:ihvy              "+string(buf)));
  ////  int ihvy2(int(alpgen_parstrToparval("ihvy2",parstrPtr,parvalPtr)));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:ihvy2       " << ihvy2 << ""));
  ////int itopprc(alpgen_nInt(alpgen_parstrToparval("itopprc",parstrPtr,parvalPtr)));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:itopprc     " << itopprc << ""));
  ////int nw(int(alpgen_parstrToparval("nw",parstrPtr,parvalPtr)));
  ////if(ihrd==13) {
  ////  nw=1;                // N.B. nw is reassigned in this way 
  ////  if(itopprc>=3) nw=2; //      by UPEVNT (after UPINIT).
  ////}
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:nw          " << nw << ""));
  ////  int nz(int(alpgen_parstrToparval("nz",parstrPtr,parvalPtr)));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:nz          " << nz << ""));
  int nh(int(alpgen_parstrToparval("nh",parstrPtr,parvalPtr)));
  m_herwigCommandVector.push_back(string("# Number of Higgses in the AlpGen process:"));
  sprintf(buf,"%d",nh);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:nh          "+string(buf)));
  int nph(int(alpgen_parstrToparval("nph",parstrPtr,parvalPtr)));
  m_herwigCommandVector.push_back(string("# Number of photons in the AlpGen process:"));
  sprintf(buf,"%d",nph);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:nph         "+string(buf)));
  ////  double ptbmin(alpgen_parstrToparval("ptbmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:ptbmin      " << ptbmin << ""));
  ////  double ptcmin(alpgen_parstrToparval("ptcmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:ptcmin      " << ptcmin << ""));
  ////  double ptlmin(alpgen_parstrToparval("ptlmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:ptlmin      " << ptlmin << ""));
  ////  double metmin(alpgen_parstrToparval("metmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:metmin      " << metmin << ""));
  ////  double ptphmin(alpgen_parstrToparval("ptphmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:ptphmin     " << ptphmin << ""));
  ////  double etabmax(alpgen_parstrToparval("etabmax",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:etabmax     " << etabmax << ""));
  ////  double etacmax(alpgen_parstrToparval("etacmax",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:etacmax     " << etacmax << ""));
  ////  double etalmax(alpgen_parstrToparval("etalmax",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:etalmax     " << etalmax << ""));
  ////  double etaphmax(alpgen_parstrToparval("etaphmax",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:etaphmax    " << etaphmax << ""));
  ////  double drbmin(alpgen_parstrToparval("drbmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:drbmin      " << drbmin << ""));
  ////  double drcmin(alpgen_parstrToparval("drcmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:drcmin      " << drcmin << ""));
  ////  double drlmin(alpgen_parstrToparval("drlmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:drlmin      " << drlmin << ""));
  ////  double drphjmin(alpgen_parstrToparval("drphjmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:drphjmin    " << drphjmin << ""));
  ////  double drphlmin(alpgen_parstrToparval("drphlmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:drphlmin    " << drphlmin << ""));
  ////  double drphmin(alpgen_parstrToparval("drphmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:drphmin     " << drphmin << ""));
  ////  double mllmin(alpgen_parstrToparval("mllmin",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:mllmin      " << mllmin << ""));
  ////  double mllmax(alpgen_parstrToparval("mllmax",parstrPtr,parvalPtr));
  ////  m_herwigCommandVector.push_back(string("set AlpGenHandler:mllmax      " << mllmax << ""));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("# Set up the LH reader ...                                  #"));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/EventHandlers"));
  m_herwigCommandVector.push_back(string("set theLHReader:WeightWarnings    false"));
  m_herwigCommandVector.push_back(string("# Input event file name:"));
  m_herwigCommandVector.push_back(string("set theLHReader:FileName               "+prefix+".lhe"));
  m_herwigCommandVector.push_back(string("set theLHReader:MomentumTreatment      RescaleEnergy"));
  m_herwigCommandVector.push_back(string("# set theLHReader:IgnoreIDPRUP           Yes"));
  m_herwigCommandVector.push_back(string("set theLHReader:Cuts  /Herwig/Cuts/NoCuts"));
  m_herwigCommandVector.push_back(string("set theLHReader:OverSampling ForbidOverSampling"));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("# Set up the LHAPDF ...                                     #"));
  m_herwigCommandVector.push_back(string("#############################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/Partons"));
  m_herwigCommandVector.push_back(string("# Don't try and find PDF index out from the LH file ..."));
  m_herwigCommandVector.push_back(string("set /Herwig/EventHandlers/theLHReader:InitPDFs false"));
  m_herwigCommandVector.push_back(string("# Instead set them explicitly here:"));
  sprintf(buf,"%d",lhapdf);
  m_herwigCommandVector.push_back(string("set thePDFset:PDFNumber       "+string(buf)));
  m_herwigCommandVector.push_back(string("set thePDFset:RemnantHandler  HadronRemnants"));
  m_herwigCommandVector.push_back(string("set /Herwig/EventHandlers/theLHReader:PDFA thePDFset"));
  m_herwigCommandVector.push_back(string("set /Herwig/EventHandlers/theLHReader:PDFB thePDFset"));
  m_herwigCommandVector.push_back(string("set /Herwig/Particles/p+:PDF    thePDFset"));
  m_herwigCommandVector.push_back(string("set /Herwig/Particles/pbar-:PDF thePDFset"));
  m_herwigCommandVector.push_back(string("# The PDF for beam particles A/B - overrides particle's own PDF above"));
  m_herwigCommandVector.push_back(string("set /Herwig/Shower/AlpGenHandler:PDFA    thePDFset"));
  m_herwigCommandVector.push_back(string("set /Herwig/Shower/AlpGenHandler:PDFB    thePDFset"));
  m_herwigCommandVector.push_back(string("set /Herwig/Shower/ShowerHandler:PDFA thePDFset"));
  m_herwigCommandVector.push_back(string("set /Herwig/Shower/ShowerHandler:PDFB thePDFset"));
  m_herwigCommandVector.push_back(string("set /Herwig/Partons/MPIExtractor:FirstPDF  thePDFset"));
  m_herwigCommandVector.push_back(string("set /Herwig/Partons/MPIExtractor:SecondPDF thePDFset"));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("####################################################"));
  m_herwigCommandVector.push_back(string("# Set up the generator ...                         #"));
  m_herwigCommandVector.push_back(string("####################################################"));
  m_herwigCommandVector.push_back(string("cd /Herwig/Generators"));
  m_herwigCommandVector.push_back(string("set LHCGenerator:EventHandler /Herwig/EventHandlers/theLesHouchesHandler"));
  sprintf(buf,"%d", unwev);
  m_herwigCommandVector.push_back(string("set LHCGenerator:NumberOfEvents "+string(buf)));
  m_herwigCommandVector.push_back(string(""));
  //   m_herwigCommandVector.push_back(string("###################################################################"));
  //   m_herwigCommandVector.push_back(string("# ReDefine particle data like it is in the AlpGen parameter file. #"));
  //   m_herwigCommandVector.push_back(string("###################################################################"));
  //   m_herwigCommandVector.push_back(string("\ncd  /Herwig/Particles/ "));
  //   // 'if' statements needed here to protect against mc<ms (i.e. if AlpGen has mc=0
  //   // for example). If this occurs then the QCD coupling can't initialise and the
  //   // HW++ read step fails (due to matching at flavour thresholds etc).
  //   if(massesPtr->at(0)>1.0) {
  //     sprintf(buf,"%f",massesPtr->at(0));
  //     m_herwigCommandVector.push_back(string("set c:NominalMass           "+string(buf)+"*GeV"));
  //     m_herwigCommandVector.push_back(string("set cbar:NominalMass        "+string(buf)+"*GeV"));
  //   }
  //   // Ditto.
  //   if(massesPtr->at(0)>4.0) {
  //     sprintf(buf,"%f",massesPtr->at(1));
  //     m_herwigCommandVector.push_back(string("set b:NominalMass           "+string(buf)+"*GeV"));
  //     m_herwigCommandVector.push_back(string("set bbar:NominalMass        "+string(buf)+"*GeV"));
  //   }
  //   sprintf(buf,"%f",massesPtr->at(2));
  //   m_herwigCommandVector.push_back(string("set t:NominalMass           "+string(buf)+"*GeV"));
  //   m_herwigCommandVector.push_back(string("set tbar:NominalMass        "+string(buf)+"*GeV"));
  //   sprintf(buf,"%f",massesPtr->at(3));
  //   m_herwigCommandVector.push_back(string("set W+:NominalMass          "+string(buf)+"*GeV"));
  //   m_herwigCommandVector.push_back(string("set W-:NominalMass          "+string(buf)+"*GeV"));
  //   sprintf(buf,"%f",massesPtr->at(4));
  //   m_herwigCommandVector.push_back(string("set Z0:NominalMass          "+string(buf)+"*GeV"));
  //   sprintf(buf,"%f",massesPtr->at(5));
  //   m_herwigCommandVector.push_back(string("set h0:NominalMass          "+string(buf)+"*GeV"));
  //   m_herwigCommandVector.push_back(string("\n\n\n\n"));
  m_herwigCommandVector.push_back(string("######################################################### "));
  m_herwigCommandVector.push_back(string("# Option to off shower / hadronization / decays / MPI.  # "));
  m_herwigCommandVector.push_back(string("######################################################### "));
  m_herwigCommandVector.push_back(string("cd /Herwig/EventHandlers "));
  m_herwigCommandVector.push_back(string("# set theLesHouchesHandler:CascadeHandler        NULL "));
  m_herwigCommandVector.push_back(string("#set theLesHouchesHandler:HadronizationHandler  NULL "));
  m_herwigCommandVector.push_back(string("#set theLesHouchesHandler:DecayHandler          NULL "));
  m_herwigCommandVector.push_back(string("# The handler for multiple parton interactions "));
  m_herwigCommandVector.push_back(string("#set /Herwig/Shower/AlpGenHandler:MPIHandler       NULL "));
  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("########################################################### "));
  m_herwigCommandVector.push_back(string("# A couple of commands from lhef_cmds which may be useful # "));
  m_herwigCommandVector.push_back(string("########################################################### "));
  m_herwigCommandVector.push_back(string("## According to H++ authors, without the following line the decay of heavy particles is"));
  m_herwigCommandVector.push_back(string("## delayed until after the showering, which usually goes wrong somewhere."));
  m_herwigCommandVector.push_back(string("insert /Herwig/EventHandlers/theLesHouchesHandler:PreCascadeHandlers 0 /Herwig/NewPhysics/DecayHandler"));
  m_herwigCommandVector.push_back(string("#Include spin effects"));
  m_herwigCommandVector.push_back(string("set /Herwig/EventHandlers/theLHReader:IncludeSpin Yes"));
  m_herwigCommandVector.push_back(string("#Turn on QED radiation"));
  m_herwigCommandVector.push_back(string("insert /Herwig/EventHandlers/theLesHouchesHandler:PostSubProcessHandlers[0] /Herwig/QEDRadiation/QEDRadiationHandler"));

  if (m_useShowerImprovement) {
    m_herwigCommandVector.push_back(string(""));
    m_herwigCommandVector.push_back(string("########################################################## "));
    m_herwigCommandVector.push_back(string("# Shower improvement                                     # "));
    m_herwigCommandVector.push_back(string("########################################################## "));
    m_herwigCommandVector.push_back(string("cd /Herwig/Shower"));
    m_herwigCommandVector.push_back(string("set Evolver:ColourEvolutionMethod 1"));
    m_herwigCommandVector.push_back(string("set PartnerFinder:PartnerMethod 1"));
    m_herwigCommandVector.push_back(string("set GtoGGSplitFn:SplittingColourMethod 1"));
  }

  m_herwigCommandVector.push_back(string(""));
  m_herwigCommandVector.push_back(string("######################################################### "));
  m_herwigCommandVector.push_back(string("# MLM merging parameters below are from the .par file,  # "));
  m_herwigCommandVector.push_back(string("# except RClusFactor which is always 1.5 in Fortran     # "));
  m_herwigCommandVector.push_back(string("# Herwig                                                # "));
  m_herwigCommandVector.push_back(string("######################################################### "));
  m_herwigCommandVector.push_back(string("cd /Herwig/Shower"));
  m_herwigCommandVector.push_back(string("# Is this the highest multiplicity ME in merging? "));
  m_herwigCommandVector.push_back(string("# 0 = no, 1 = yes . "));
  int highestMult;
  if (exclusive == 1.0)
    highestMult = 0;
  else if (exclusive == 0.0)
    highestMult = 1;
  else {
    delete[] buf;
    ATH_MSG_ERROR("The parameter determining whether the matching should be inclusive or exclusive seems not to have been correctly read from the parameters file");
    return StatusCode::FAILURE;
  }
  sprintf(buf,"%d",highestMult);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:highestMultiplicity "+string(buf)+" "));
  m_herwigCommandVector.push_back(string("# Jet ET cut to apply in jet clustering in merging."));
  double etclus = minETclus;//(max(ptjmin+5,1.2*ptjmin));
  sprintf(buf,"%f",etclus);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:ETClus "+string(buf)+"*GeV"));
  m_herwigCommandVector.push_back(string("# Cone size used in clustering in merging."));
  double rclus = minRclus; //(drjmin);
  sprintf(buf,"%f",rclus);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:RClus "+string(buf)));
  m_herwigCommandVector.push_back(string("# Max |eta| for jets in clustering in merging."));
  double etaclmax = maxETAclus; //(etajmax);
  sprintf(buf,"%f",etaclmax);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:EtaClusMax "+string(buf)));
  m_herwigCommandVector.push_back(string("# Default 1.5 factor used to decide if a jet matches a parton"));
  m_herwigCommandVector.push_back(string("# in merging: if DR(parton,jet)<rclusfactor*rclus the parton "));
  m_herwigCommandVector.push_back(string("# and jet are said to have been matched."));
  double rclusfactor(1.5);
  sprintf(buf,"%f",rclusfactor);
  m_herwigCommandVector.push_back(string("set AlpGenHandler:RClusFactor "+string(buf)));
  m_herwigCommandVector.push_back(string("\n#End of the section written by the function Herwigpp::readAlpgen()\n"));

  delete[] buf;

  return StatusCode::SUCCESS;
}

// Now consider assignments specific to individual hard processes:
StatusCode Herwigpp::alpgen_doIndividualHardProcessAssignments(int ihrd, double * nup, vector<double> * idup, vector<double> * istup,
							vector<double> * mothup1, vector<double> * mothup2, vector<double> * icolup1, vector<double> * icolup2,
							vector<vector<double> > * pup, vector<double> masses, int itopprc) {

  int iwch(0);
  
  // W/Z/gamma b bbar + jets ( wcjet*, ihrd=10 / wphjet*, ihrd=14 / wphqq*,
  //  ihrd=15 ), or W/Z + jets ( wjet*, ihrd=3 / zjet*, ihrd=4 ):
  // In this case we add to the list of particles the single intermediate
  // boson (at the end of the list) appropriately, and assign the relevant
  // parent-daughter and colour flow indices.
  if (ihrd<=4||ihrd==10||ihrd==14||ihrd==15) {
    iwch=0; // <--- used to determine type: W/Z/gamma
    for(int iup=int(*nup)-2; iup<int(*nup); iup++) {
      (*mothup1)[iup]=*nup+1; // Assigning, to-be-added boson, as
      (*mothup2)[iup]=0;      // the parent of each decay prod.
      if(ihrd!=2) iwch = iwch - (int((*idup)[iup])%2);
      // electron+nubar -> 11 + (-12) => -(1)+0 = -1  => W-
      // positron+nu    -> -11+ 12    => -(-1)+0 = -1 => W+
      // u dbar -> 2 -1  => 0 -(-1) = 1 => W+
      // c dbar -> 4 -1  => W+
      // etc.
    }
    // Now we start adding the intermediate boson entry:
    int iup=alpgen_nInt(*nup);
    if(iwch>0)      (*idup).push_back( 24);
    else if(iwch<0) (*idup).push_back(-24);
    else            (*idup).push_back( 23);
    (*istup).push_back(2);
    (*mothup1).push_back(1);
    (*mothup2).push_back(2);
    (*pup).push_back(vector<double>(5));
    double tmp = (*pup)[3][iup-2]+(*pup)[3][iup-1]; // Vector boson energy.
    (*pup)[3][iup] = tmp;
    tmp = alpgen_sqr(tmp);
    for(unsigned int ixx=0; ixx<=2; ixx++) {
      (*pup)[ixx][iup] = (*pup)[ixx][iup-2]+(*pup)[ixx][iup-1];
      tmp = tmp-alpgen_sqr((*pup)[ixx][iup]);  // Vector boson mass^2 when loop ends.
    }
    (*pup)[4][iup] = sqrt(tmp);  // Set vector boson mass.
    (*icolup1).push_back(0);   // Set 1st colour line for vector boson.
    (*icolup2).push_back(0);   // Set 2nd colour line for vector boson.
    (*nup) = (*nup)+1;         // Increment number of particles to be read in the event.
  }
  // nW + mZ + kH + jets ( vbjet* / ihrd=5 ):
  else if(ihrd==5) {
    //unsigned int ivstart(0),ivend(0);
    // Identify the range of the Z and W bosons in the event (AlpGen conventions).
    // Note the Z's and W's are the only things that decay - Higgs's and photons 
    // do not.
    vector<unsigned int> bosonIndex;
    for(unsigned int ixx=0; ixx<(*nup); ixx++)
      if(abs((*idup)[ixx])==24||(*idup)[ixx]==23) {
	(*istup)[ixx]=2;             //  Set the W/Z boson status to intermediate.
	bosonIndex.push_back(ixx+1); //  W/Z boson index in LH event record (1->nup).
      }
    unsigned int bosonCounter(alpgen_nInt(*nup)-2*bosonIndex.size());
    for(unsigned int ixx=0; ixx<bosonIndex.size(); ixx++) {
      (*mothup1)[bosonCounter]=bosonIndex[ixx];
      (*mothup2)[bosonCounter]=0;
      bosonCounter++;
      (*mothup1)[bosonCounter]=bosonIndex[ixx];
      (*mothup2)[bosonCounter]=0;
      bosonCounter++;
    }
  }
  // t tbar + jets [ + photons ] ( 2Q*, ihrd=6 [ 2Qph*, ihrd=16 ] ):
  else if ((ihrd==6||ihrd==16)&&abs((*idup)[2])==6) {
    // Redefine the tops as intermediates in the event record.
    (*istup)[2]=2;
    (*istup)[3]=2;
    unsigned int it(3),itb(4); // Index of t & tbar in evt.record (LH index).
    if((*idup)[2]!=6) swap(it,itb);
    // Reconstruct intermediate W's from the decay products.
    for(unsigned int ixx=0; ixx<4;ixx++) {
      (*idup).push_back(0);
      (*istup).push_back(0);
      (*mothup1).push_back(0);
      (*mothup2).push_back(0);
      (*icolup1).push_back(0);
      (*icolup2).push_back(0);
      (*pup).push_back(vector<double>(5));
    }
    for(unsigned int iw=1; iw<=2; iw++) {
      int iwdec(alpgen_nInt(*nup)-5+2*iw);  // First of decay products for this W (iw).
      int iwup(alpgen_nInt(*nup)+iw); // Where the reco. W will go in evt.record (LH index).
      int ibup(iwup+2);    // Where the reco. b will go in evt.record (under the Ws).
      int iwch(0);
      for(int iup=iwdec; iup<=iwdec+1; iup++) {
	(*mothup1)[iup-1]=iwup;
	(*mothup2)[iup-1]=0;
	iwch=iwch-int((*idup)[iup-1])%2; // iwch = charge of W boson.
	// electron+nubar -> 11 + (-12) = -1 => W-
	// d + ubar -> 1 + (-2) = -1 => W-
	// positron+nu    -> -11+ 12    =  1 => W+
	// u + dbar -> 2 + (-1) = 1 => W+
      }
      // Make space for the b and W:
      // Fill in b and W LH record entries:
      if(iwch>0) {
	(*idup)[iwup-1]=24;
	(*idup)[ibup-1]=5;
	(*mothup1)[iwup-1]=it;
	(*mothup2)[iwup-1]=0;
	(*mothup1)[ibup-1]=it;
	(*mothup2)[ibup-1]=0;
      } else if (iwch<0) {
	(*idup)[iwup-1]=-24;
	(*idup)[ibup-1]=-5;
	(*mothup1)[iwup-1]=itb;
	(*mothup2)[iwup-1]=0;
	(*mothup1)[ibup-1]=itb;
	(*mothup2)[ibup-1]=0;
      }
      (*istup)[iwup-1]=2; // The W is an intermediate and the
      (*istup)[ibup-1]=1; // b is a final-state particle.
      // Now reconstruct W boson momentum
      double tmp=(*pup)[3][iwdec-1]+(*pup)[3][iwdec];
      (*pup)[3][iwup-1]=tmp;            // W energy.
      tmp=alpgen_sqr(tmp);
      for(unsigned int ixx=0; ixx<=2; ixx++) {
	(*pup)[ixx][iwup-1] = (*pup)[ixx][iwdec-1]            // W's 3-mom.
	  + (*pup)[ixx][iwdec];
	tmp=tmp-alpgen_sqr((*pup)[ixx][iwup-1]); // Equals m^2 at the end of the loop.
      }
      (*pup)[4][iwup-1]=sqrt(tmp);      // W mass.
      // Reconstruct b momentum
      int itmp(alpgen_nInt((*mothup1)[iwup-1]));
      tmp=(*pup)[3][itmp-1]-(*pup)[3][iwup-1];
      (*pup)[3][ibup-1]=tmp;            // b energy.
      tmp=alpgen_sqr(tmp);
      for(unsigned int ixx=0; ixx<=2; ixx++) {
	(*pup)[ixx][ibup-1] = (*pup)[ixx][alpgen_nInt((*mothup1)[iwup-1])-1] // b's 3mom.
	  - (*pup)[ixx][iwup-1];
	tmp=tmp-alpgen_sqr((*pup)[ixx][ibup-1]); // Equals m^2 at the end of the loop.
      }
      (*pup)[4][ibup-1]=sqrt(tmp);      // b mass.
      (*icolup1)[iwup-1]=0;             // W has no colour
      (*icolup2)[iwup-1]=0;             // lines.
      (*icolup1)[ibup-1]=(*icolup1)[alpgen_nInt((*mothup1)[iwup-1])-1]; // b shares top
      (*icolup2)[ibup-1]=(*icolup2)[alpgen_nInt((*mothup1)[iwup-1])-1]; // colour line.
    }
    (*nup)+=4;
  }
  // H t tbar + jets ( QQh*, ihrd=8 ):
  else if (ihrd==8&&abs((*idup)[3])==6) {
    // Redefine the tops as intermediates in the event record.
    (*istup)[3]=2;
    (*istup)[4]=2;
    unsigned int it(4),itb(5); // Index of t & tbar in evt.record (LH index).
    if((*idup)[3]!=6) swap(it,itb);
    // Reconstruct intermediate W's from the decay products.
    for(unsigned int ixx=0; ixx<4;ixx++) {
      (*idup).push_back(0);
      (*istup).push_back(0);
      (*mothup1).push_back(0);
      (*mothup2).push_back(0);
      (*icolup1).push_back(0);
      (*icolup2).push_back(0);
      (*pup).push_back(vector<double>(5));
    }
    for(unsigned int iw=1; iw<=2; iw++) {
      int iwdec(alpgen_nInt(*nup)-5+2*iw);  // First of decay products for this W (iw).
      int iwup(alpgen_nInt(*nup)+iw); // Where the reco. W will go in evt.record (LH index).
      int ibup(iwup+2);    // Where the reco. b will go in evt.record (under the Ws).
      int iwch(0);
      for(int iup=iwdec; iup<=iwdec+1; iup++) {
	(*mothup1)[iup-1]=iwup;
	(*mothup2)[iup-1]=0;
	iwch=iwch-int((*idup)[iup-1])%2; // iwch = charge of W boson.
	// electron+nubar -> 11 + (-12) = -1 => W-
	// d + ubar -> 1 + (-2) = -1 => W-
	// positron+nu    -> -11+ 12    =  1 => W+
	// u + dbar -> 2 + (-1) = 1 => W+
      }
      if(iwch>0) {
	(*idup)[iwup-1]=24;
	(*idup)[ibup-1]=5;
	(*mothup1)[iwup-1]=it;
	(*mothup2)[iwup-1]=0;
	(*mothup1)[ibup-1]=it;
	(*mothup2)[ibup-1]=0;
      } else if (iwch<0) {
	(*idup)[iwup-1]=-24;
	(*idup)[ibup-1]=-5;
	(*mothup1)[iwup-1]=itb;
	(*mothup2)[iwup-1]=0;
	(*mothup1)[ibup-1]=itb;
	(*mothup2)[ibup-1]=0;
      }
      (*istup)[iwup-1]=2; // The W is an intermediate and the
      (*istup)[ibup-1]=1; // b is a final-state particle.
      // Now reconstruct W boson momentum
      double tmp=(*pup)[3][iwdec-1]+(*pup)[3][iwdec];
      (*pup)[3][iwup-1]=tmp;            // W energy.
      tmp=alpgen_sqr(tmp);
      for(unsigned int ixx=0; ixx<=2; ixx++) {
	(*pup)[ixx][iwup-1] = (*pup)[ixx][iwdec-1]            // W's 3-mom.
	  + (*pup)[ixx][iwdec];
	tmp=tmp-alpgen_sqr((*pup)[ixx][iwup-1]); // Equals m^2 at the end of the loop.
      }
      (*pup)[4][iwup-1]=sqrt(tmp);      // W mass.
      // Reconstruct b momentum
      tmp=(*pup)[3][alpgen_nInt((*mothup1)[iwup-1])-1]-(*pup)[3][iwup-1];
      (*pup)[3][ibup-1]=tmp;            // b energy.
      tmp=alpgen_sqr(tmp);
      for(unsigned int ixx=0; ixx<=2; ixx++) {
	(*pup)[ixx][ibup-1] = (*pup)[ixx][alpgen_nInt((*mothup1)[iwup-1])-1] // b's 3mom.
	  - (*pup)[ixx][iwup-1];
	tmp=tmp-alpgen_sqr((*pup)[ixx][ibup-1]); // Equals m^2 at the end of the loop.
      }
      (*pup)[4][ibup-1]=sqrt(tmp);      // b mass.
      (*icolup1)[iwup-1]=0;             // W has no colour
      (*icolup2)[iwup-1]=0;             // lines.
      (*icolup1)[ibup-1]=(*icolup1)[alpgen_nInt((*mothup1)[iwup-1])-1]; // b shares top
      (*icolup2)[ibup-1]=(*icolup2)[alpgen_nInt((*mothup1)[iwup-1])-1]; // colour line.
    }
    (*nup)+=4;
  }
  // Single top production ( top*, ihrd=13):
  else if (ihrd==13) {
    int nw=1;
    if(itopprc>=3) nw=2;
    // Assign a mass to the incoming bottom quark, if there is one,
    // rescaling the energy to accommodate the mass.
    for(unsigned int ixx=1; ixx<=2; ixx++) {
      if(abs((*idup)[ixx-1])==5) {
	(*pup)[4][ixx-1]=masses[1];
	(*pup)[3][ixx-1]=sqrt(alpgen_sqr((*pup)[2][ixx-1])+alpgen_sqr((*pup)[4][ixx-1]));
      }
    }
    // Set the top status to that of an intermediate.
    (*istup)[2]=2;
    // Get the index of the t / tbar in the evt. record (LH convention: 1->nup).
    unsigned int it=0;
    unsigned itb=0;
    if((*idup)[2]==6)       it=3;
    else if((*idup)[2]==-6) itb=3;
    else {
      ATH_MSG_ERROR("alpgen_doIndividualHardProcessAssignments: wrong assumption about top position.\n");
      return StatusCode::FAILURE;
    }
    // Reconstruct intermediate W's from the decay products.
    // iwdec is the index of the first W decay product 
    unsigned int iwdec(alpgen_nInt(*nup)-1);  // LH conventions: 1->nup.
    if(nw==2) iwdec=alpgen_nInt(*nup)-3;
    // The W and b will go at the end of the record.
    unsigned int iwup(alpgen_nInt(*nup)+1);
    unsigned int ibup(alpgen_nInt(*nup)+2);
    int iwch(0);
    for(unsigned int iup=iwdec; iup<=iwdec+1; iup++) {
      (*mothup1)[iup-1]=iwup;
      (*mothup2)[iup-1]=0;
      iwch=iwch-int((*idup)[iup-1])%2; // iwch = charge of W boson.
      // electron+nubar -> 11 + (-12) = -1 => W-
      // d + ubar -> 1 + (-2) = -1 => W-
      // positron+nu    -> -11+ 12    =  1 => W+
      // u + dbar -> 2 + (-1) = 1 => W+
    }
    for(unsigned int ixx=0; ixx<2;ixx++) {
      (*idup).push_back(0);
      (*istup).push_back(0);
      (*mothup1).push_back(0);
      (*mothup2).push_back(0);
      (*icolup1).push_back(0);
      (*icolup2).push_back(0);
      (*pup).push_back(vector<double>(5));
    }
    if(iwch>0) {
      (*idup)[iwup-1]=24;
      (*idup)[ibup-1]=5;
      (*mothup1)[iwup-1]=it;
      (*mothup2)[iwup-1]=0;
      (*mothup1)[ibup-1]=it;
      (*mothup2)[ibup-1]=0;
    } else if (iwch<0) {
      (*idup)[iwup-1]=-24;
      (*idup)[ibup-1]=-5;
      (*mothup1)[iwup-1]=itb;
      (*mothup2)[iwup-1]=0;
      (*mothup1)[ibup-1]=itb;
      (*mothup2)[ibup-1]=0;
    }
    (*istup)[iwup-1]=2;
    (*istup)[ibup-1]=1;
    // Now reconstruct W boson momentum
    double tmp=(*pup)[3][iwdec-1]+(*pup)[3][iwdec];
    (*pup)[3][iwup-1]=tmp;            // W energy.
    tmp=alpgen_sqr(tmp);
    for(unsigned int ixx=0; ixx<=2; ixx++) {
      (*pup)[ixx][iwup-1] = (*pup)[ixx][iwdec-1]            // W's 3-mom.
	+ (*pup)[ixx][iwdec];
      tmp=tmp-alpgen_sqr((*pup)[ixx][iwup-1]); // Equals m^2 at the end of the loop.
    }
    (*pup)[4][iwup-1]=sqrt(tmp);      // W mass.
    // Reconstruct b momentum
    tmp=(*pup)[3][alpgen_nInt((*mothup1)[iwup-1])-1]-(*pup)[3][iwup-1];
    (*pup)[3][ibup-1]=tmp;            // b energy.
    tmp=alpgen_sqr(tmp);
    for(unsigned int ixx=0; ixx<=2; ixx++) {
      (*pup)[ixx][ibup-1] = (*pup)[ixx][alpgen_nInt((*mothup1)[iwup-1])-1] // b's 3mom.
	- (*pup)[ixx][iwup-1];
      tmp=tmp-alpgen_sqr((*pup)[ixx][ibup-1]); // Equals m^2 at the end of the loop.
    }
    (*pup)[4][ibup-1]=sqrt(tmp);      // b mass.
    (*icolup1)[iwup-1]=0;
    (*icolup2)[iwup-1]=0;
    (*icolup1)[ibup-1]=(*icolup1)[alpgen_nInt((*mothup1)[iwup-1])-1];
    (*icolup2)[ibup-1]=(*icolup2)[alpgen_nInt((*mothup1)[iwup-1])-1];
    (*nup)=(*nup)+2;
    if(nw==2) {
      // W DECAY
      // iwdec is the index of the first W decay product.
      iwdec=alpgen_nInt(*nup)-3;
      // iwup is the index of the W in the event record (LH conventions: 1->nup).
      iwup=alpgen_nInt(*nup)-6;
      iwch=0;
      int iwch(0);
      for(unsigned int iup=iwdec; iup<=iwdec+1; iup++) {
	(*mothup1)[iup-1]=iwup;
	(*mothup2)[iup-1]=0;
	iwch=iwch-int((*idup)[iup-1])%2; // iwch = charge of W boson.
	// electron+nubar -> 11 + (-12) = -1 => W-
	// d + ubar -> 1 + (-2) = -1 => W-
	// positron+nu    -> -11+ 12    =  1 => W+
	// u + dbar -> 2 + (-1) = 1 => W+
      }
      (*istup)[iwup-1]=2;
      (*icolup1)[iwup-1]=0;
      (*icolup2)[iwup-1]=0;
    }
  }

  return StatusCode::SUCCESS;
}


//This function returns the strong coupling constant at the Z mass (alpha_s) and the number of loops (nloop) for the running of alpha_s.
//The values of alpha_s and nloop are determined by the PDF number ndns as given in the Alpgen documentation, section 2.1.4
//The function returns false in case the value of ndns is not recognized
bool Herwigpp::alpgen_getAlpha_s(int ndns, int& nloop, double& alpha_s) {

  if (ndns >= 1 && ndns <= 3) {
    alpha_s = 0.116;
    nloop = 2;
  }
  else if (ndns == 4 || (ndns >= 6 && ndns <= 8) || (ndns >= 10 && ndns <= 50)) {
    alpha_s = 0.118;
    nloop = 2;
  }
  else if (ndns == 5) {
    alpha_s = 0.127;
    nloop = 1;
  }
  else if (ndns == 9 || ndns == 106) {
    alpha_s = 0.130;
    nloop = 1;
  }
  else if (ndns == 101) {
    alpha_s = 0.1175;
    nloop = 2;
  }
  else if (ndns == 102) {
    alpha_s = 0.119;
    nloop = 2;
  }
  else if (ndns == 103) {
    alpha_s = 0.117;
    nloop = 2;
  }
  else if (ndns == 104 || ndns == 105) {
    alpha_s = 0.121;
    nloop = 2;
  }
  else
    return false;

  return true;
}
