/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

#include <cmath>

//FIXME
//are these threadsafe?
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <Eigen/Dense>

#include "LArOFCCondAlg.h"

#include "LArElecCalib/LArConditionsException.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

LArOFCCondAlg::LArOFCCondAlg(const std::string &name,
        ISvcLocator *pSvcLocator)
    : ::AthAlgorithm(name, pSvcLocator),
    m_LArOnOffIdMappingObjKey("LArOnOffIdMap"),
    m_LArShapeObjKey("LArShapeSym"),
    m_LArNoiseObjKey("LArNoiseSym"),
    m_LArPedestalObjKey("LArPedestal"),
    m_LArAutoCorrTotalObjKey("LArAutoCorrTotal"),
    m_LArOFCObjKey("LArOFC"),
    m_condSvc("CondSvc", name), m_Nminbias(0), m_isMC(true),
    m_isSuperCell(false), m_firstSample(0), 
    m_useHighestGainAutoCorr(false), m_Dump(false) {
        declareProperty("LArOnOffIdMappingObjKey", m_LArOnOffIdMappingObjKey,
                "Key to read LArOnOffIdMapping object");
        declareProperty("LArShapeObjKey", m_LArShapeObjKey,
                "Key to read LArShape object");
        declareProperty("LArNoiseObjKey", m_LArNoiseObjKey,
                "Key to read LArNoise object");
        declareProperty("LArPedestalObjKey", m_LArPedestalObjKey,
                "Key to read LArPedestal object");
        declareProperty("LArAutoCorrTotalObjKey", m_LArAutoCorrTotalObjKey,
                "Key to read LArAutoCorrTotal object");
        declareProperty("LArOFCObjKey", m_LArOFCObjKey,
                "Key to write LArOFC object");
        declareProperty("Nminbias", m_Nminbias);
        declareProperty("isMC", m_isMC);
        declareProperty("isSuperCell", m_isSuperCell);
        declareProperty(
                "firstSample", m_firstSample,
                "First sample to use for in-time event on the full pulse shape");
        declareProperty("useHighestGainAutoCorr",m_useHighestGainAutoCorr);
        declareProperty("DumpOFCCondAlg",m_Dump);
    }

LArOFCCondAlg::~LArOFCCondAlg() {}

StatusCode LArOFCCondAlg::initialize() {
    ATH_MSG_DEBUG("initialize " << name());

    // CondSvc
    ATH_CHECK(m_condSvc.retrieve());

    // ReadCondHandle initialization
    ATH_CHECK(m_LArShapeObjKey.initialize());

    ATH_CHECK(m_LArAutoCorrTotalObjKey.initialize());
    ATH_CHECK(m_LArOnOffIdMappingObjKey.initialize());

    //WriteHandle initialization 
    ATH_CHECK(m_LArOFCObjKey.initialize());

    ATH_CHECK(m_LArNoiseObjKey.initialize(m_isMC));
    ATH_CHECK(m_LArPedestalObjKey.initialize(!m_isMC));

    // WriteCondHandle initialization
    if (m_condSvc->regHandle(this, m_LArOFCObjKey).isFailure()) {
        ATH_MSG_ERROR("Unable to register WriteCondHandle "
                << m_LArOFCObjKey.fullKey() << " with CondSvc");
        return StatusCode::FAILURE;
    }

    // Number of gains (does this have to be in initialize now b/c of AthenaMT?)
    // Copied from LArADC2MeVCondAlg.cxx
    if (m_isSuperCell) {
        m_nGains = 1;
    } else {
        m_nGains = 3;
    }

    return StatusCode::SUCCESS;
}

StatusCode LArOFCCondAlg::execute() {

    // WriteHandle setup
    SG::WriteCondHandle<LArOFC> writeHandle(m_LArOFCObjKey);
    // So the following should not be called usually?!
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG(
                "CondHandle "
                << writeHandle.fullKey() << " is already valid.");
        return StatusCode::SUCCESS;
    }

    // Identifier helper
    // Copied from LArADC2MeVCondAlg.cxx
    const LArOnlineID_Base *larOnlineID = nullptr;
    if (m_isSuperCell) {
        const LArOnline_SuperCellID *scidhelper;
        ATH_CHECK(detStore()->retrieve(scidhelper, "LArOnline_SuperCellID"));
        larOnlineID = scidhelper; // cast to base-class
    } else {                    // regular cells
        const LArOnlineID *idhelper;
        ATH_CHECK(detStore()->retrieve(idhelper, "LArOnlineID"));
        larOnlineID = idhelper; // cast to base-class
    }

    // Mapping helper
    const LArOnOffIdMapping *larOnOffIdMapping = nullptr;
    SG::ReadCondHandle<LArOnOffIdMapping> larOnOffIdMappingHdl{
        m_LArOnOffIdMappingObjKey
    };
    larOnOffIdMapping = *larOnOffIdMappingHdl;
    if (larOnOffIdMapping == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve LArOnOffIdMapping object");
        return StatusCode::FAILURE;
    }
    writeHandle.addDependency(larOnOffIdMappingHdl);

    // Get pointers to inputs
    // Retrieve validity ranges and determine their intersection
    SG::ReadCondHandle<ILArShape> ShapeHdl{ m_LArShapeObjKey };
    // FIXME: should check if handle is properly created and/or check if handle is
    // properly retrieved
    // operator star of a ReadCondHandle returns a const pointer to type T
    const ILArShape *larShape{ *ShapeHdl };
    if (larShape == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve LArShape object");
	return StatusCode::FAILURE;
    }
    writeHandle.addDependency(ShapeHdl);

    SG::ReadCondHandle<LArAutoCorrTotal> AutoCorrTotalHdl{ m_LArAutoCorrTotalObjKey };
    const LArAutoCorrTotal *larAutoCorrTotal = nullptr;
    larAutoCorrTotal= *AutoCorrTotalHdl;
    if (larAutoCorrTotal == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve LArADC2MeV object");
	return StatusCode::FAILURE;
    }
    writeHandle.addDependency(AutoCorrTotalHdl);

    // Consider the determinstic objects
    const ILArNoise *larNoise = nullptr;
    const ILArPedestal *larPedestal = nullptr;

    if (m_isMC) {
        SG::ReadCondHandle<ILArNoise> NoiseHdl{ m_LArNoiseObjKey };
        larNoise = *NoiseHdl;
        if (larNoise == nullptr) {
            ATH_MSG_ERROR("Failed to retrieve object LArNoise");
	    return StatusCode::FAILURE;
        }
	writeHandle.addDependency(NoiseHdl);
    } else {
        SG::ReadCondHandle<ILArPedestal> PedestalHdl{ m_LArPedestalObjKey };
        larPedestal = *PedestalHdl;
        if (larPedestal == nullptr) {
            ATH_MSG_ERROR("Failed to retrieve object LArPedestal");
	    return StatusCode::FAILURE;
        }
	writeHandle.addDependency(PedestalHdl);
    }

    ATH_MSG_INFO("IOV found from intersection for LArOFCCondObj object: "
		 << writeHandle.getRange());

    // make output object
    // dimensions: number of gains x number of channel IDs x elements of
    // OFC
    std::unique_ptr<LArOFC> larOFC =
        std::make_unique<LArOFC>(larOnlineID, larOnOffIdMapping, m_nGains);


    ///////////////////////////////////////////////////
    std::vector<HWIdentifier>::const_iterator it = larOnlineID->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_e = larOnlineID->channel_end();
    int count = 0;
    int count2 = 0;

    for (; it != it_e; ++it) {
        count++;
        const HWIdentifier chid = *it;
        const IdentifierHash hid = larOnlineID->channel_Hash(chid);

        //if (!(larOnOffIdMapping->isOnlineConnected(chid))) continue;
        if (larOnOffIdMapping->isOnlineConnected(chid)) {
            count2++;
            for (size_t igain = 0; igain < m_nGains; igain++) {

                std::vector<float> OFCa_tmp, OFCb_tmp;

                //:::::::::::::::::::::::::::::::
                //retrieve the data
                //:::::::::::::::::::::::::::::::
                ILArShape::ShapeRef_t Shape = larShape->Shape(chid,igain);
                unsigned int nsamples_shape = Shape.size();
                ILArShape::ShapeRef_t ShapeDer = larShape->ShapeDer(chid,igain);
                //:::::::::::::::::::::::::::::::

                // get Noise autocorrelation for gain
                int igain_autocorr=igain;
                // to use only Autocorr fro highest gain in optimization: HEC/FCAL=> medium gain    EM=>high gain
                if (m_useHighestGainAutoCorr) {
                    if  (!m_isSuperCell &&(larOnlineID->isHECchannel(chid) || larOnlineID->isFCALchannel(chid)) ) igain_autocorr=1;
                    else igain_autocorr=0;
                }

                const std::vector<double> AutoCorr = 
                    larAutoCorrTotal->autoCorrTotal(chid,igain_autocorr,m_Nminbias);
                //unsigned int nsamples_AC_OFC=AutoCorr.size()+1;
                unsigned int nsamples_AC_OFC = (1+((int)(sqrt(1+8*AutoCorr.size()))))/2;

                const std::vector<double>& rmsSampl =
                    larAutoCorrTotal->samplRMS(chid,igain_autocorr,m_Nminbias);
                unsigned int nsamples2 = rmsSampl.size();
                if (nsamples2 != nsamples_AC_OFC) {
                    ATH_MSG_WARNING( " bad size for rmsSampl "  );
                    //return (m_OFCtmp);  // return empty vector
                }
                //:::::::::::::::::::::::::::::::
                //unsigned int iBeginOfNSamples=findTheNSamples(Shape,
                //						    nsamples_AC_OFC,
                //						    nsamples_shape); 
                unsigned int firstSample = m_firstSample; 
                if(larOnlineID->isHECchannel(chid) && m_firstSample == 0 && nsamples_AC_OFC==4){ 
                    firstSample=1; 
                } 
                unsigned int iBeginOfNSamples = firstSample; 
                if(nsamples_AC_OFC + iBeginOfNSamples > nsamples_shape) 
                    iBeginOfNSamples=0;      
                //:::::::::::::::::::::::::::::::

                if(m_isMC) {
                }
                else          
                {	 
                    float RMSpedestal = larPedestal->pedestalRMS(chid,igain);
                    if(RMSpedestal>= (1.0+LArElecCalib::ERRORCODE))
                        ;
                    else
                    {
                        ATH_MSG_WARNING(" PedestalRMS vector empty for "
                                <<chid<<" at gain "<<igain );
                    }	
                }
                //:::::::::::::::::::::::::::::::
                //protection against missing data
                //:::::::::::::::::::::::::::::::
                if(Shape.size()==0 || ShapeDer.size()==0 || AutoCorr.size()==0)
                {
                    ATH_MSG_WARNING("Some data are missing -> OFC will be empty for "
                            <<chid<<" at gain "<<igain );
                    //return (m_OFCtmp);
                    //returns an empty vector
                }
		if (Shape.size()!=ShapeDer.size()) {
		  ATH_MSG_ERROR("Got invalid shape object: Size=" << Shape.size() << ", DerSize=" << ShapeDer.size());
		  return StatusCode::SUCCESS;
		}
                //:::::::::::::::::::::::::::::::
                unsigned int l,c,i; 
                //:::::::::::::::::::::::::::::::
                //calculations
                //:::::::::::::::::::::::::::::::
                // fill and inverrt AC matrix
                //HepMatrix AC(nsamples_AC_OFC,nsamples_AC_OFC),
                          //ACinv(nsamples_AC_OFC,nsamples_AC_OFC);    
                Eigen::MatrixXf AC = Eigen::MatrixXf::Zero(nsamples_AC_OFC,nsamples_AC_OFC);
                Eigen::MatrixXf ACinv = Eigen::MatrixXf::Zero(nsamples_AC_OFC,nsamples_AC_OFC);
                for(l=0;l<nsamples_AC_OFC;++l) {  //  l=line c=column      	
                    for(c=0;c<nsamples_AC_OFC;++c) {
                        if (l==c) {
                            AC(l,c)=1.;
                        }
                        else {
                            int i1=std::min(l,c);
                            int i2=std::max(l,c);
                            int index = i1*nsamples_AC_OFC - i1*(i1+1)/2 -(i1+1) + i2;
                            AC(l,c)=AutoCorr[index];
                        }
                        AC(l,c) = AC(l,c)*rmsSampl[l]*rmsSampl[c];
                    }
                }
                ACinv=AC.inverse();
                //:::::::::::::::::::::::::::::::           


                float ACinv_PS[32];//ACinv_PS
                float ACinv_PSD[32]; //ACinv_PSD
                //Q1 Q2 Q3 DELTA
                float Q1=0.;
                float Q2=0.;
                float Q3=0.;

                for(l=0;l<nsamples_AC_OFC;++l)
                {
                    ACinv_PS[l]=0.; ACinv_PSD[l]=0.;
                    for(c=0;c<nsamples_AC_OFC;++c){
                        ACinv_PS[l]+=ACinv(l,c)*Shape[c+iBeginOfNSamples];
                        ACinv_PSD[l]+=ACinv(l,c)*ShapeDer[c+iBeginOfNSamples];
                    }
                    Q1+=Shape[l+iBeginOfNSamples]*ACinv_PS[l];
                    Q2+=ShapeDer[l+iBeginOfNSamples]*ACinv_PSD[l];
                    Q3+=ShapeDer[l+iBeginOfNSamples]*ACinv_PS[l];
                } 
                float DELTA=Q1*Q2-Q3*Q3;  
                //:::::::::::::::::::::::::::::::
                //OFCa  
                for(i=0;i<nsamples_AC_OFC;++i) 
                    OFCa_tmp.push_back( (ACinv_PS[i]*Q2-ACinv_PSD[i]*Q3)/DELTA );
                //OFCb  
                for(i=0;i<nsamples_AC_OFC;++i) 
                    OFCb_tmp.push_back( (ACinv_PS[i]*Q3-ACinv_PSD[i]*Q1)/DELTA ); 

                //for debugging only
                if(m_Dump)
                    { 
                        std::cout<<larOnlineID
                            ->show_to_string(larOnOffIdMapping->cnvToIdentifier(chid))
                            <<" gain="<<igain<<" Nminbias="<<m_Nminbias<<std::endl;
                        std::cout<<"Shape: ";
                        for(c=0;c<nsamples_shape;++c)
                            std::cout<<Shape[c]<<" ";
                        std::cout<<std::endl;
                        std::cout<<"ShapeDer: ";
                        for(c=0;c<nsamples_shape;++c)
                            std::cout<<ShapeDer[c]<<" ";
                        std::cout<<std::endl;
                        for(c=0;c<nsamples_AC_OFC;++c)
                            std::cout<<Shape[c+iBeginOfNSamples]<<" ";
                        std::cout<<" <- "<<iBeginOfNSamples<<std::endl;
                        for(i=0;i<nsamples_AC_OFC;++i) std::cout<<ACinv_PS[i]<<" ";
                        std::cout<<std::endl;
                        for(i=0;i<nsamples_AC_OFC;++i) std::cout<<ACinv_PSD[i]<<" ";
                        std::cout<<std::endl;
                        std::cout<<" Q1="<<Q1<<" Q2="<<Q2<<" Q3="<<Q3
                            <<" DELTA="<<DELTA<<std::endl;
                        std::cout << " OFCa: ";
                        for(i=0;i<nsamples_AC_OFC;++i) 
                            std::cout<<(ACinv_PS[i]*Q2-ACinv_PSD[i]*Q3)/DELTA<<" ";
                        std::cout<<std::endl;
                }
                bool stat = larOFC->setOFC(hid, igain, std::make_pair(OFCa_tmp, OFCb_tmp));
            	if (!stat) {
	                msg(MSG::ERROR) << "LArOFC::setOFC fails for gain " << igain << ", hash " << hid << endmsg;
                }
	     } // end loop over gains
        } else { // end loop over connected channels -- now, set empty for disc. chnanels
            for (unsigned igain=0;igain<m_nGains;++igain) {
                std::vector<float> empty;
                bool stat = larOFC->setOFC(hid,igain, std::make_pair(empty, empty));
            	if (!stat) {
	                msg(MSG::ERROR) << "LArOFC::setOFC fails for gain " << igain << ", hash " << hid << endmsg;
                }
            } // end loop over gains of disconnected channels
        } // end loop over disconnected channels

    } // end loop over all channels


    ATH_CHECK(writeHandle.record(std::move(larOFC)));
    ATH_MSG_INFO("Wrote LArOFC obj to CondStore");
    return StatusCode::SUCCESS;
}

unsigned int LArOFCCondAlg::findTheNSamples(ILArShape::ShapeRef_t Shape,
					 unsigned int nsamples_AC_OFC,
					 unsigned int nsamples_shape) const
{
  unsigned int  i_ShapeMax=0;
  double ShapeMax=0;
  for(unsigned int i=0;i<nsamples_shape;++i)
  {
    double value=Shape[i];
    if(value>ShapeMax) { ShapeMax=value; i_ShapeMax=i; }
    else if(value<0 && i>3) break;//after the peak  
  }
  
  unsigned int tmp=int(nsamples_AC_OFC/2.);
  if(tmp>i_ShapeMax) return 0;
  else               return i_ShapeMax-tmp;
}
