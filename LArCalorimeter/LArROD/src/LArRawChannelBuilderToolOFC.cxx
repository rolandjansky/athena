/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderToolOFC.h"
#include "LArROD/LArRawChannelBuilderDriver.h"
#include "GaudiKernel/MsgStream.h"

#include "LArRawEvent/LArDigit.h"
#include "LArElecCalib/ILArShape.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include <math.h>

using CLHEP::nanosecond;
using CLHEP::picosecond;

#define MAXINT 2147483000
#define MAXINT2 -2147483000


LArRawChannelBuilderToolOFC::LArRawChannelBuilderToolOFC(const std::string& type,
							 const std::string& name,
							 const IInterface* parent):
  LArRawChannelBuilderToolBase(type,name,parent),
  m_OFCTool("LArOFCTool"),
  m_larShape(NULL),
  m_shapeMode(0),
  m_keyShape("LArShape")
{
  m_helper = new LArRawChannelBuilderStatistics( 3,      // number of possible errors
					       0x05);  // bit pattern special for this tool,
                                                       // to be stored in  "uint16_t provenance"
  m_helper->setErrorString(0, "no errors");
  m_helper->setErrorString(1, "channel saturated");
  m_helper->setErrorString(2, "OFC size zero");
  
  declareProperty("ADCMax",                    m_AdcMax=4095);
  declareProperty("Skip",                      m_skipSaturatedCells=true);
  declareProperty("ECut",                      m_Ecut=50);
  declareProperty("OFCTool",		       m_OFCTool);
  declareProperty("KeyShape",                  m_keyShape);
}

StatusCode LArRawChannelBuilderToolOFC::initTool()
{
  if (detStore()->retrieve(m_larShape,m_keyShape).isFailure()) {
    //MsgStream log(msgSvc(), name());
    msg(MSG::WARNING) << "Can't retrieve LArShape from Conditions Store"
	<< ". Trying later." <<  endmsg;
    m_larShape=NULL;
  }
  if(m_OFCTool.retrieve().isFailure())
    {
      //MsgStream log(msgSvc(), name());
      msg(MSG::ERROR) << "Could not retrieve OFCTool" << endmsg;
      return StatusCode::FAILURE;
    }
  
  return StatusCode::SUCCESS;
}

bool LArRawChannelBuilderToolOFC::buildRawChannel(const LArDigit* digit,
						  float pedestal,
						  const std::vector<float>& ramps,
						  MsgStream* pLog)
{
  HWIdentifier chid=m_parent->curr_chid;
  if(bool(pLog))
    (*pLog) << MSG::DEBUG << "Start " <<MSG::hex<< chid<<MSG::dec<< endmsg;
  CaloGain::CaloGain gain=m_parent->curr_gain;
  
  if ( m_parent->curr_maximum > m_AdcMax )
    {
      if(bool(pLog))
	(*pLog) << MSG::DEBUG << "Saturation on channel 0x"
	     << MSG::hex << chid.get_compact() << MSG::dec << ". ";
      if ( m_skipSaturatedCells )
	{
      if(bool(pLog))
	(*pLog) << "Skipping channel." << endmsg; 
	  m_helper->incrementErrorCount(1);
	  return false;
	}
      if(bool(pLog))
	(*pLog) << endmsg;
    }
  
  // Optimal Filtering Coefficients  
  const std::vector<float> *ofc_a;
  const std::vector<float> *ofc_b;
  ofc_a=&(this->OFC_a());
   
  int OFCTimeBin=0;
  
  //Check if we have OFC for this channel and time bin
  if (ofc_a->size()==0)
    {
      if(bool(pLog))
	(*pLog) << MSG::DEBUG << ". No OFC's found for channel 0x"
	     << MSG::hex << chid.get_compact() << MSG::dec 
	     << " Time bin = " << OFCTimeBin << ", Gain = " << gain
	     << ". Skipping channel." << endmsg;
      m_helper->incrementErrorCount(2);
      return false;
    }
  


  //Now apply Optimal Filtering to get ADC peak
  float ADCPeak=0;
  for (unsigned i=0;i<(ofc_a->size());i++) {
    ADCPeak+=(digit->samples()[i+m_parent->curr_shiftTimeSamples]-
	      pedestal)*ofc_a->at(i);
    if(bool(pLog))
      (*pLog) << MSG::DEBUG << "OFC i, a= " <<i<<" "<< ofc_a->at(i) << endmsg;
    
  }  
  float power=1;
  float energy=0;
  for( unsigned int i=0; i<ramps.size(); i++)
    {
      energy += ramps[i] * power;
      power  *= ADCPeak;
    }
  float time=0;
  float quality=0;
  
  //Check if energy is above threshold for time & quality calculation
  if (energy>m_Ecut)
    {
      ofc_b=&(this->OFC_b());
      if (ofc_b->size() != ofc_a->size())
	{
	  //don't have proper number of coefficients
	  if (ofc_b->size()==0)
	    {
	      if(bool(pLog))
		(*pLog) << MSG::DEBUG << "No time-OFC's found for channel 0x"
		     << MSG::hex << chid.get_compact() << MSG::dec 
		     << " Gain "<< gain << " found. Time not calculated." << endmsg;
	    }else{
	      if(bool(pLog))
		(*pLog) << MSG::DEBUG << "OFC for time size " << ofc_b->size() 
		     << " not equal to OFC for energy size " << ofc_a->size() 
		     << "   Time not calculated " << endmsg;
	    }
	}else{
	  for (unsigned i=0;i<(ofc_b->size());i++) 
	    time+=(digit->samples()[i+m_parent->curr_shiftTimeSamples]-pedestal)
	      *ofc_b->at(i);
	  time/=ADCPeak;
	}
      if(bool(pLog))
	(*pLog) << MSG::VERBOSE << "Time calculated " << time
	     << " TimeBin=" << OFCTimeBin  << endmsg;
      
      //Calculate Quality factor
      if (this->larShape())
	{
	  //Get Shape & Shape Derivative for this channel
          ILArShape::ShapeRef_t shape =
	    larShape()->Shape(chid,gain,OFCTimeBin,m_shapeMode);
          ILArShape::ShapeRef_t shapeDer =
	    larShape()->ShapeDer(chid,gain,OFCTimeBin,m_shapeMode);
	  
	  //Check Shape
	  if (shape.size() < ofc_a->size())
	    {
	      if (shape.size()==0) 
		{
		  if(bool(pLog))
		    (*pLog) << MSG::DEBUG << "No Shape found for channel 0x"
			 << MSG::hex << chid.get_compact() << MSG::dec 
			 << " Gain "<< gain
			 << ". Quality factor not calculated." << endmsg;
		}else{
		  if(bool(pLog))
		    (*pLog) << MSG::DEBUG << "Shape size " << shape.size() 
			 << "smaller than OFC size " << ofc_a->size() 
			 << "for channel 0x" << MSG::hex << chid.get_compact() 
			 << MSG::dec << ". Quality factor not calculated." << endmsg;
		}
	      quality=0;
	    } else {
	      //Shape ok
	      if (time!=0 && shapeDer.size()!=shape.size())
		{
		  if(bool(pLog))
		    (*pLog) << MSG::DEBUG
			 << "Shape-Derivative has different size than "
			 << "Shape for channel 0x"
			 << MSG::hex << chid.get_compact() << MSG::dec 
			 << "."
			 << " Derivative not taken into accout for quality factor."
			 << endmsg;
		} 
	      if (time==0 || shapeDer.size()!=shape.size() )
		{
		  //Calculate Q without time info
		  for (unsigned i=0;i<(ofc_a->size());i++)
		    quality+=((digit->samples()[i+m_parent->curr_shiftTimeSamples]-
			       pedestal)-shape[i]*ADCPeak)*
		      ((digit->samples()[i+m_parent->curr_shiftTimeSamples]-
			pedestal)-shape[i]*ADCPeak);
		} else {
		  //All input data ok, calculate Q with time info
		  for (unsigned i=0;i<(ofc_a->size());i++) 
		    quality+=
		      ((digit->samples()[i+m_parent->curr_shiftTimeSamples]-
			pedestal)-
		       ((shape[i]-shapeDer[i]*time)*ADCPeak))*
		      ((digit->samples()[i+m_parent->curr_shiftTimeSamples]-
			pedestal)-
		       ((shape[i]-shapeDer[i]*time)*ADCPeak));
		}
	    }
	} else {
	  //No Shape found at all 
	  quality=0; //Can't calculate chi^2, assume good hit.
	  // noShape++;
	}
    }
  else 
    quality=-1; //in case E<Ecut
  
  uint16_t iquality=0;
  uint16_t iprovenance=0;
  if (quality>=0) {
     iquality = ((int)(quality)) & 0xFFFF;
     iprovenance = iprovenance | 0x2000;
  }

  iprovenance |= m_parent->qualityBitPattern;
  iprovenance |= m_helper->returnBitPattern();
  iprovenance = iprovenance & 0x3FFF;
  
  time=time*(nanosecond/picosecond); //Convert time to ps

  const float fMAXINT = static_cast<float>(MAXINT);
  const float fMAXINT2 = static_cast<float>(MAXINT2);

  if (time>fMAXINT) time=fMAXINT;
  if (time<fMAXINT2) time=fMAXINT2;

  if (energy>fMAXINT) energy=fMAXINT;
  if (energy<fMAXINT2) energy=fMAXINT2;
  
  (this->*m_buildIt)((int)(floor(energy+0.5)),(int)floor(time+0.5),iquality,iprovenance,digit);
  m_helper->incrementErrorCount(0);
  
  return true;
}

const std::vector<float>&
LArRawChannelBuilderToolOFC::OFC_a()
{
  m_OFCtmp.clear();
  m_OFCtmp = m_OFCTool->OFC_a(m_parent->curr_chid,m_parent->curr_gain).asVector();
  return m_OFCtmp;
}

const std::vector<float>&
LArRawChannelBuilderToolOFC::OFC_b()
{
  m_OFCtmp.clear();
  m_OFCtmp = m_OFCTool->OFC_b(m_parent->curr_chid,m_parent->curr_gain).asVector();
  return m_OFCtmp;
}

const ILArShape*
LArRawChannelBuilderToolOFC::larShape()
{
  if(!bool(m_larShape))
    {
      StatusCode sc=detStore()->retrieve(m_larShape,m_keyShape);
      if (sc.isFailure()) {
	MsgStream log(msgSvc(), name());
	log << MSG::WARNING << "Can't retrieve LArShape from Conditions Store"
	    << std::endl
	    << "Quality factor will not be caluclated." << endmsg;
	m_larShape=NULL;
      }
    }
  return m_larShape;
}
