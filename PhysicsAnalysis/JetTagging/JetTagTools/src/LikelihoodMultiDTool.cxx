/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////

#include "JetTagTools/LikelihoodMultiDTool.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "JetTagTools/HistoHelperRoot.h"
#include <string>
#include <utility>
#include <vector>
#include <cmath>

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

namespace Analysis
{

  LikelihoodMultiDTool::LikelihoodMultiDTool(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p),
    m_histoSvc(0),
    //m_allLhVariables(std::vector<std::string>()),
    m_useTheseLhVariables(std::vector<std::string>()),
    m_mapOfAllHistos(std::vector<std::map<std::string, TH1*>* >()),
    m_histoLimitsMap1D(std::map< std::string, HistoLimits>() ),
    m_histoLimitsMap2D(std::map< std::string, HistoLimits>() ),
    m_histoLimitsMap3D(std::map< std::string, HistoLimits>() ),
    m_lhVariableValues(std::vector<Slice> ()),
    m_likelihoodVector(std::vector<double>()),
    m_nhis1D(0),
    m_nhis2D(0),
    m_nhis3D(0),
    m_DoInterpol(false),
    m_WriteSmoothedHistos(false),
    m_nbHypotheses(2),
    m_nSmooth1D(1),
    m_normProb(true)
  {
    declareInterface<LikelihoodMultiDTool>(this);
    declareProperty("WriteSmoothedHistos", m_WriteSmoothedHistos);
    declareProperty("NSmooth1D",           m_nSmooth1D);
    declareProperty("NormalizeProb",       m_normProb);
  }

  LikelihoodMultiDTool::~LikelihoodMultiDTool()
  {
    for (std::vector< std::map<std::string, TH1*>* >::iterator itr = m_mapOfAllHistos.begin();
	 itr != m_mapOfAllHistos.end(); ++itr)   delete (*itr);
  }

  StatusCode LikelihoodMultiDTool::initialize()
  {

    StatusCode sc = service("THistSvc", m_histoSvc);
    if (sc.isFailure())
      {
	ATH_MSG_FATAL("THistSvc not found!");
	return StatusCode::FAILURE;
      }
    //
    return StatusCode::SUCCESS;
  }

  StatusCode LikelihoodMultiDTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  void LikelihoodMultiDTool::prepareHistosFromFile(const std::string& refFileName) {
    MsgStream mlog(msgSvc(),name()); 
    ATH_MSG_DEBUG("refFileName = "<<refFileName);
    // the tmpHistoMap belongs to m_mapOfAllHistos and is deleted at the end in the destructor of LikelihoodMultiDTool()
    std::map<std::string, TH1*>* tmpHistoMap = new std::map<std::string, TH1*>();
    // loop on requested histos:
    for (std::vector<std::string>::iterator itr = m_useTheseLhVariables.begin() ;
	 itr != m_useTheseLhVariables.end()  ; ++itr ) {
      std::string histoName = *itr;
      std::vector<std::string> grades;
      // parse histo name to extract words separated by _ sign:
      // first word is the histo base name, following ones are the grades
      // first word is jet category, 2nd is the histo base name, following ones are the grades
      std::vector<std::string> words;

      ATH_MSG_DEBUG("decoding histoName = "<<histoName);

      const std::string delim("_");
      std::string::size_type sPos, sEnd, sLen;
      sPos = histoName.find_first_not_of(delim);
      while ( sPos != std::string::npos ) {
	sEnd = histoName.find_first_of(delim, sPos);
	if(sEnd==std::string::npos) sEnd = histoName.length();
	sLen = sEnd - sPos;
	std::string word = histoName.substr(sPos,sLen);

	ATH_MSG_DEBUG(" -> word = "<<word);

	words.push_back(word);
	sPos = histoName.find_first_not_of(delim, sEnd);
      }
      // first treat cases with a single or no grade:
      int nGrade = words.size() - 2;
      TH1* histoSum(0);
      if(nGrade==0) {
	ATH_MSG_DEBUG("Histo "<<histoName<<" has "<<nGrade<<" grade: direct retrieval");
	histoSum = this->retrieveHistoFromFile(refFileName, words[1], words[0]);
      } else if(nGrade==1) {
	ATH_MSG_DEBUG("Histo "<<histoName<<" has "<<nGrade<<" grade: direct retrieval");
	histoSum = this->retrieveHistoFromFile(refFileName, words[1]+"_"+words[2], words[0]);
      } else {
	// for many grades, get individual histos and sum them up:
	ATH_MSG_DEBUG("Histo "<<histoName<<" has "<<nGrade<<" grades:");
	for(int i=1;i<=nGrade;i++) {
	  ATH_MSG_DEBUG("  -> retrieving histo for grade "<<i<<" : "<<words[1]<<" "<<words[i+1]);
	  TH1* histo = this->retrieveHistoFromFile(refFileName, words[1]+"_"+words[i+1], words[0]);
	  ATH_MSG_DEBUG("   #entries= "<<histo->GetEntries());
	  if(1==i) {
	    histoSum = histo;
	  } else {
	    histoSum->Add(histo,1.);
	  }
	  ATH_MSG_DEBUG("   #entries for Sum= "<<histoSum->GetEntries());
	}
      }
      if(histoSum) {
	// smooth and normalize:
	double norm = histoSum->Integral();
	if(norm) {
	  if(1==histoSum->GetDimension() && m_nSmooth1D) {
	    if(histoName.find("N2T")==std::string::npos){ // do not smooth N2T
	      if(norm>10000)histoSum->Smooth(m_nSmooth1D);
	      else histoSum->Smooth((int)(m_nSmooth1D*100./sqrt(norm)));
	    }
	  }
	  if(2==histoSum->GetDimension()) {
	    int m2d=3;
	    if(refFileName.find("Bkg/")!=std::string::npos) m2d=5;
	    bool debug = mlog.level()<=MSG::DEBUG ? true : false;
	    // do not smooth Sip3D for the time being
	    if(histoName.find("Sip3D")==std::string::npos) HistoHelperRoot::smoothASH2D(dynamic_cast<TH2*>(histoSum), m2d, m2d, debug);
	  }
	  if(3==histoSum->GetDimension()) {
	    int m3d1=3;
	    int m3d3=2;
	    bool debug = mlog.level()<=MSG::DEBUG ? true : false;
	    HistoHelperRoot::smoothASH3D(dynamic_cast<TH3*>(histoSum), m3d1, m3d1, m3d3, debug);
	  }
	  norm = histoSum->Integral();
	  histoSum->Scale(1./norm);
	} else {
	  ATH_MSG_WARNING("Histo "<<histoName<<" is empty!");
	}
	// store: if >= 2 grades in one histo, the histoName is the name of the first Grade
	// Change LV 2005/10/09: if >=2 grades, insert a copy for each grade:
	std::string namemap = histoName;
	if (nGrade == 0) { 
	  namemap = words[0]+"_"+words[1];
	  ATH_MSG_VERBOSE("Inserting as " << namemap);
	  tmpHistoMap->insert(std::pair<std::string, TH1*>(namemap, histoSum));
	}
	for(int i=1;i<=nGrade;i++) {
	  namemap = words[0] + "_" + words[1] + "_" + words[i+1];
	  ATH_MSG_VERBOSE("Inserting as " << namemap);
	  tmpHistoMap->insert(std::pair<std::string, TH1*>(namemap, histoSum));
	}
	// Also for control, store the smoothed histograms in a file
	if (m_WriteSmoothedHistos) {
	  std::string namedir = refFileName; namedir = namedir.substr((namedir.erase(namedir.size()-1,1)).rfind('/')+1)+"/";
	  std::string namehis = histoSum->GetName();
	  ATH_MSG_VERBOSE("Registering smoothed histogram in " << namedir);
	  TH1* histoSumClone = (TH1*)histoSum->Clone();
	  StatusCode sc = m_histoSvc->regHist("/ControlFile/"+namedir+namehis, histoSumClone);
	  if (sc.isFailure()) {
	    ATH_MSG_ERROR("Cannot store the smoothed histogram in the control file !");
	  }
	}
      }
    }
    ATH_MSG_DEBUG("I found "<<m_nhis1D<<" 1D histos, "<<m_nhis2D<<" 2D histos and "<<m_nhis3D<<" 3D histos");
    if (m_nhis1D+m_nhis2D+m_nhis3D) m_mapOfAllHistos.push_back(tmpHistoMap);
  }

  TH1* LikelihoodMultiDTool::retrieveHistoFromFile(const std::string& refFileName, 
						   const std::string& histoName,
						   const std::string& jetPrefix) {
    std::string fullPath(refFileName+histoName);
    ATH_MSG_VERBOSE("Retrieving histo: " << fullPath);
    bool is1D = false, is2D = false, is3D = false;
    TH1* tmphisto(0);
    StatusCode sc = m_histoSvc->regHist(fullPath);
    if(sc.isFailure()) {};
    sc = m_histoSvc->getHist(fullPath,tmphisto);
    int dim = 0; 
    if (sc.isSuccess()) {
      dim = tmphisto->GetDimension();
      if (dim == 1) {
	is1D = true;
	m_nhis1D++;
      } else if (dim == 2) {
	is2D = true;
	m_nhis2D++;
      } else if (dim == 3) {
	is3D = true;
	m_nhis3D++;
      } else {
	ATH_MSG_DEBUG("cannot retrieve histo " << fullPath << " Unexpected dimension "<< dim);
	return tmphisto;
      }
    } else {
      ATH_MSG_DEBUG("I COULD NOT RETRIEVE HISTO: " << fullPath);
      return tmphisto;
    }
    /** Retrieve and set histo limits for this histogram */
    unsigned int bins  = (tmphisto->GetXaxis())->GetNbins();
    double minx        = (tmphisto->GetXaxis())->GetXmin();
    double maxx        = (tmphisto->GetXaxis())->GetXmax();
    if (is1D) {
      m_histoLimitsMap1D[jetPrefix+"_"+histoName] = HistoLimits(bins, minx, maxx);
      ATH_MSG_DEBUG("Histo "<< jetPrefix+"_"+histoName << " properties: ");
      ATH_MSG_DEBUG(bins  <<" bins X between " << minx <<" and " << maxx);
    } else if (is2D) {
      unsigned int binsy = (tmphisto->GetYaxis())->GetNbins();
      double miny        = (tmphisto->GetYaxis())->GetXmin();
      double maxy        = (tmphisto->GetYaxis())->GetXmax();
      m_histoLimitsMap2D[jetPrefix+"_"+histoName] = HistoLimits(bins, minx, maxx, binsy, miny, maxy);
      ATH_MSG_DEBUG("Histo "<< jetPrefix+"_"+histoName << " properties: ");
      ATH_MSG_DEBUG(bins  <<" bins X between " << minx <<" and " << maxx );
      ATH_MSG_DEBUG(binsy <<" bins Y between " << miny <<" and " << maxy);
    } else if (is3D) {
      unsigned int binsy = (tmphisto->GetYaxis())->GetNbins();
      double miny        = (tmphisto->GetYaxis())->GetXmin();
      double maxy        = (tmphisto->GetYaxis())->GetXmax();
      unsigned int binsz = (tmphisto->GetZaxis())->GetNbins();
      double minz        = (tmphisto->GetZaxis())->GetXmin();
      double maxz        = (tmphisto->GetZaxis())->GetXmax();
      m_histoLimitsMap3D[jetPrefix+"_"+histoName] = HistoLimits(bins, minx, maxx, binsy, miny, maxy, binsz, minz, maxz);

      ATH_MSG_DEBUG("Histo "<< jetPrefix+"_"+histoName << " properties: ");
      ATH_MSG_DEBUG(bins  <<" bins X between " << minx <<" and " << maxx);
      ATH_MSG_DEBUG(binsy <<" bins Y between " << miny <<" and " << maxy);
      ATH_MSG_DEBUG(binsz <<" bins Z between " << minz <<" and " << maxz);
    }
    return tmphisto;
  }

  void LikelihoodMultiDTool::setLhVariableValue(std::vector<Slice>& value)
  {
    m_lhVariableValues=value;
  }
  void LikelihoodMultiDTool::setLhVariableValue(Slice& value)
  {
    std::vector<Slice> tmpVector;
    tmpVector.push_back(value);
    setLhVariableValue(tmpVector);
  }


  std::vector<double> LikelihoodMultiDTool::calculateLikelihood() {
    m_likelihoodVector.clear();
    std::vector<double> probDensityPerEventClassAllVariables;
    ATH_MSG_VERBOSE("The sizes of the maps "<<m_mapOfAllHistos.size()<<" "<<m_nbHypotheses);
    probDensityPerEventClassAllVariables.resize(m_nbHypotheses);
    for (unsigned int i = 0 ; i < m_nbHypotheses; ++i) {
      probDensityPerEventClassAllVariables[i]=1.;
    }

    // Loop on Tracks in the Jet (IP) / Vertices in the Jet (SV)
    for (unsigned int iel = 0; iel<m_lhVariableValues.size(); iel++) {
      int ncompo = m_lhVariableValues[iel].composites.size();
      ATH_MSG_VERBOSE("-- element "<<iel<<" "<<m_lhVariableValues[iel].name<<" has "<<ncompo<<" composites.");
      // Loop on variables that make up the Tag, e.g. 
      // one 1D for IP2D, one 2D for IP3D, one 1D and one 2D for SV1, one 3D for SV2
      for (int icompo = 0;icompo<ncompo;icompo++) {
	double sum(0.);
	std::vector<double> tmpVector; 
	std::string histName = m_lhVariableValues[iel].composites[icompo].name;
	int idim = m_lhVariableValues[iel].composites[icompo].atoms.size();
	ATH_MSG_VERBOSE("  -- composite "<<icompo<<" histo= "<<histName<<" dim= "<<idim);
	if (idim == 1) {
	  const HistoLimits& histoLimits = (*(m_histoLimitsMap1D.find(histName))).second;
	  // Loop on Hypotheses : b or light for the time being... 
	  for (std::vector< std::map<std::string, TH1*>* >::iterator
		 itr2 = m_mapOfAllHistos.begin() ;
	       itr2 != m_mapOfAllHistos.end() ; ++itr2 ) {
	    if( (*itr2)->find(histName) != (*itr2)->end() ) {
	      TH1* tmpHisto = (*((*itr2)->find(histName))).second;
	      double value = m_lhVariableValues[iel].composites[icompo].atoms[0].value;
	      int bin((tmpHisto->GetXaxis())->FindBin(value));
	      double tmp(0);
	      if(!m_DoInterpol) {
		if(value>= histoLimits.xmax) bin = histoLimits.bins;
		if(value<= histoLimits.xmin) bin = 1;
		tmp = tmpHisto->GetBinContent(bin);
	      }
	      else {
		tmp = HistoHelperRoot::Interpol1d(value,dynamic_cast<TH1*>(tmpHisto));
	      }
	      ATH_MSG_VERBOSE("    (1D) value= "<<value<<" bin= "<<bin<< " f = "<<tmp);
	      if (!m_normProb) {
		double binw = tmpHisto->GetBinWidth(bin);
		if(binw != 0.)tmp /= binw;
		else ATH_MSG_ERROR("Bin width is 0.");
		ATH_MSG_VERBOSE("    (1D) value= "<<value<<" bin= "<<bin<<" binw= "<<binw<< " f = "<<tmp);
	      }
	      tmpVector.push_back(tmp);
	      sum += tmp;
	    }
	  }
	} else if (idim == 2) {
	  const HistoLimits& histoLimits = (*(m_histoLimitsMap2D.find(histName))).second;
	  for (std::vector< std::map<std::string, TH1*>* >::iterator
		 itr2 = m_mapOfAllHistos.begin() ;
	       itr2 != m_mapOfAllHistos.end() ; ++itr2 ) {
	    if( (*itr2)->find(histName) != (*itr2)->end() ) {
	      TH1* tmpHisto = (*((*itr2)->find(histName))).second;
	      ATH_MSG_VERBOSE("tmpHisto="<<tmpHisto);
	      double valuex = m_lhVariableValues[iel].composites[icompo].atoms[0].value; 
	      double valuey = m_lhVariableValues[iel].composites[icompo].atoms[1].value; 
	      int binx((tmpHisto->GetXaxis())->FindBin(valuex));
	      int biny((tmpHisto->GetYaxis())->FindBin(valuey));
	      if(valuex>= histoLimits.xmax) binx = histoLimits.bins;
	      if(valuex<= histoLimits.xmin) binx = 1;
	      if(valuey>= histoLimits.ymax) biny = histoLimits.binsy;
	      if(valuey<= histoLimits.ymin) biny = 1;
	      double tmp(tmpHisto->GetBinContent(binx,biny));
	      ATH_MSG_VERBOSE("    (2D) value= "<<valuex<<" "<<valuey<<" bin= "<<binx<<" "<<biny<< " f = "<<tmp);
	      if (!m_normProb) {
		double binw  = tmpHisto->GetXaxis()->GetBinWidth(binx) * tmpHisto->GetYaxis()->GetBinWidth(biny);
		if(binw != 0.)tmp /= binw;
		else ATH_MSG_ERROR("Bin area is 0.");
		ATH_MSG_VERBOSE("    (2D) value= "<<valuex<<" "<<valuey<<" bin= "<<binx<<" "<<biny<<" binw= "<<binw<< " f = "<<tmp);
	      }
	      tmpVector.push_back(tmp);
	      sum += tmp;
	    }
	  }
	} else if (idim == 3) {
	  const HistoLimits& histoLimits = (*(m_histoLimitsMap3D.find(histName))).second;
	  for (std::vector< std::map<std::string, TH1*>* >::iterator
		 itr2 = m_mapOfAllHistos.begin() ;
	       itr2 != m_mapOfAllHistos.end() ; ++itr2 ) {
	    if( (*itr2)->find(histName) != (*itr2)->end() ) {
	      TH1* tmpHisto = (*((*itr2)->find(histName))).second;
	      double valuex = m_lhVariableValues[iel].composites[icompo].atoms[0].value; 
	      double valuey = m_lhVariableValues[iel].composites[icompo].atoms[1].value; 
	      double valuez = m_lhVariableValues[iel].composites[icompo].atoms[2].value; 
	      int binx((tmpHisto->GetXaxis())->FindBin(valuex));
	      int biny((tmpHisto->GetYaxis())->FindBin(valuey));
	      int binz((tmpHisto->GetZaxis())->FindBin(valuez));
	      if(valuex>= histoLimits.xmax) binx = histoLimits.bins;
	      if(valuex<= histoLimits.xmin) binx = 1;
	      if(valuey>= histoLimits.ymax) biny = histoLimits.binsy;
	      if(valuey<= histoLimits.ymin) biny = 1;
	      if(valuez>= histoLimits.zmax) binz = histoLimits.binsz;
	      if(valuez<= histoLimits.zmin) binz = 1;
	      double tmpNoInt(tmpHisto->GetBinContent(binx,biny,binz));
	      double tmp(HistoHelperRoot::Interpol3d(valuex,valuey,valuez,dynamic_cast<TH3*>(tmpHisto)));
	      ATH_MSG_VERBOSE("    (3D) value= "<<valuex<<" "<<valuey<<" "<<valuez <<" bin= "<<binx<<" "<<biny<<" "<<binz<<" binContent = "<<tmp<< " binContentNoInt = "<< tmpNoInt);
	      if (!m_normProb) {
		double binw  = 
		  tmpHisto->GetXaxis()->GetBinWidth(binx) * 
		  tmpHisto->GetYaxis()->GetBinWidth(biny) * 
		  tmpHisto->GetZaxis()->GetBinWidth(binz);
		if(binw != 0.) {
		  tmpNoInt /= binw;
		  tmp /= binw;
		}
		else ATH_MSG_ERROR("Bin volume is 0.");	
		ATH_MSG_VERBOSE("    (3D) value= "<<valuex<<" "<<valuey<<" "<<valuez <<" bin= "<<binx<<" "<<biny<<" "<<binz<<" binw= "<<binw<<" binContent = "<<tmp<< " binContentNoInt = "<< tmpNoInt);
	      }
	      if (m_DoInterpol) {
	        tmpVector.push_back(tmp);
	        sum += tmp;
	      }  else {
	        tmpVector.push_back(tmpNoInt);
	        sum += tmpNoInt;  
	      }
	    }
	  }
	} else {
	  ATH_MSG_WARNING("No more that 3D pdf for the time being !");
	}
	unsigned int classCount(0);
	for (std::vector<double>::iterator itr3 = tmpVector.begin();  itr3 != tmpVector.end(); ++itr3) {
	  if (sum != 0.) {
	    ATH_MSG_VERBOSE("pb + pu = "<<sum);
	    double p = (*itr3);
	    if (m_normProb) p /= sum;
	    probDensityPerEventClassAllVariables[classCount] *= p;
	  } else {
	    ATH_MSG_WARNING("Empty bins for all hypothesis... The discriminating variables are not taken into account in this jet");
	    ATH_MSG_WARNING("Check your inputs");
	  }
	  ATH_MSG_VERBOSE("  probDensity= "<<probDensityPerEventClassAllVariables[classCount]<<" ic= "<<classCount);
	  classCount++;
	}
	ATH_MSG_VERBOSE(" Final probDensity= "<<probDensityPerEventClassAllVariables);
      }
    }
    ATH_MSG_VERBOSE(" Ending ...");
    m_likelihoodVector=probDensityPerEventClassAllVariables;
    return m_likelihoodVector;
  }

  std::vector<double> LikelihoodMultiDTool::tagLikelihood()
  {
    return m_likelihoodVector;
  }

  void LikelihoodMultiDTool::printStatus()
  {
    unsigned int num = m_mapOfAllHistos.size();
    ATH_MSG_INFO("Currently I hold histos of " << num << " input files");
  }

  double LikelihoodMultiDTool::getEff(const std::string& refFileName, const std::string& histoName, const std::string& mode)
  {
    //
    StatusCode scs = SUCCESS, scg = SUCCESS;
    std::string fullPathSel(refFileName+histoName+"Eff"+mode);
    std::string fullPathGen(refFileName+histoName+"Norm"+mode);
    ATH_MSG_VERBOSE("Retrieving histo for efficiency computation: " << fullPathSel);
    ATH_MSG_VERBOSE("Retrieving histo for efficiency computation: " << fullPathGen);
    TH1 *sel(0), *gen(0);
    scs = m_histoSvc->regHist(fullPathSel);
    scg = m_histoSvc->regHist(fullPathGen);
    if (scs.isSuccess() && scg.isSuccess()) { 
      scs = m_histoSvc->getHist(fullPathSel,sel);
      scg = m_histoSvc->getHist(fullPathGen,gen);
      if (sel && gen && scs.isSuccess() && scg.isSuccess()) {
	double nsel = sel->GetEntries(); // I need also the over/under flow for the efficiency !!!
	double ngen = gen->GetEntries();
	if (ngen != 0) {
	  ATH_MSG_VERBOSE("N RecSvx = " << nsel << " in a total of "<< ngen << " jets.");
	  return nsel/ngen;
	} else {
	  ATH_MSG_ERROR("Pb in computing Svx efficiency " << nsel << " "<< ngen);
	  return 0.;
	}
      }
    } else {
      // already there...
      scs = m_histoSvc->getHist(fullPathSel,sel);
      scg = m_histoSvc->getHist(fullPathGen,gen);
      if (sel && gen && scs.isSuccess() && scg.isSuccess()) {
	double nsel = sel->GetEntries();
	double ngen = gen->GetEntries();
	if (ngen != 0) {
	  ATH_MSG_VERBOSE("N RecSvx = " << nsel << " in a total of "<< ngen << " jets.");
	  return nsel/ngen;
	} else {
	  ATH_MSG_ERROR("Pb in computing Svx efficiency " << nsel << " "<< ngen);
	  return 0.;
	}
      }
    }
    ATH_MSG_ERROR("Pb in computing Svx efficiency ");
    return 0.;
  }

  void LikelihoodMultiDTool::addLhVariableToUse(const std::string& var) {
    m_useTheseLhVariables.push_back(var);
  }

}
