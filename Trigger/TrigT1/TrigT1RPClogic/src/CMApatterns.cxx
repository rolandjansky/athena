/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include "TrigT1RPClogic/CMApatterns.h"
#include "TrigT1RPClogic/windows.h"

#include "RPC_CondCabling/CMAprogram.h"

///////////////////////////////////////////////////////////////////////////////

const CMApatterns::TRIGdistance CMApatterns::no_distance =
  TRIGdistance(99999.,99999.);

///////////////////////////////////////////////////////////////////////////////

CMApatterns::CMApatterns(int sector,const CMAparameters* cma,
                         unsigned long int debug) :
    RPCtrigDataObject(0,"CMA patterns"),
    m_debug(debug),
    m_sector(sector),m_cma_parameters(cma),
m_pivot0("pivot0 ",PATTERNidentity(Pivot,0),CMAparameters::pivot_channels),
m_pivot1("pivot1 ",PATTERNidentity(Pivot,1),CMAparameters::pivot_channels),
m_lowPt0("lowPt0 ",PATTERNidentity(LowPt,0),CMAparameters::confirm_channels),
m_lowPt1("lowPt1 ",PATTERNidentity(LowPt,1),CMAparameters::confirm_channels),
m_highPt0("highPt0",PATTERNidentity(HighPt,0),CMAparameters::confirm_channels),
m_highPt1("highPt1",PATTERNidentity(HighPt,1),CMAparameters::confirm_channels),
m_low_pt_matrix(0),m_high_pt_matrix(0)
{}

CMApatterns::CMApatterns(const CMApatterns& patterns) :
    RPCtrigDataObject(0,patterns.name()),
    m_debug(patterns.debug()),
    m_sector(patterns.sector()),
    m_cma_parameters(&patterns.cma_parameters()),
    m_pivot0(patterns.pivot0()),
    m_pivot1(patterns.pivot1()),
    m_lowPt0(patterns.lowPt0()),
    m_lowPt1(patterns.lowPt1()),
    m_highPt0(patterns.highPt0()),
    m_highPt1(patterns.highPt1()),
    m_low_pt_matrix(0),m_high_pt_matrix(0)
{}

CMApatterns::~CMApatterns()
{
    if(m_low_pt_matrix) delete m_low_pt_matrix;
    if(m_high_pt_matrix) delete m_high_pt_matrix;
}

CMApatterns
CMApatterns::operator=(const CMApatterns& patterns)
{
    static_cast<RPCtrigDataObject&>(*this) = 
        static_cast<const RPCtrigDataObject&>(patterns);
    m_debug = patterns.debug();
    m_sector = patterns.sector();
    m_cma_parameters = &patterns.cma_parameters();
    m_pivot0 = patterns.pivot0();
    m_pivot1 = patterns.pivot1();
    m_lowPt0 = patterns.lowPt0();
    m_lowPt1 = patterns.lowPt1();
    m_highPt0 = patterns.highPt0();
    m_highPt1 = patterns.highPt1();
    m_low_pt_matrix  = 0;
    m_high_pt_matrix = 0;  
    return *this;
}

bool
CMApatterns::operator==(const CMApatterns& patterns) const
{
    if(m_sector == patterns.sector())
    {
     if (m_cma_parameters->id()==patterns.cma_parameters().id()) return true;
    }
    return false;
}

bool
CMApatterns::operator!=(const CMApatterns& patterns) const
{
    return !(*this == patterns);
}


bool
CMApatterns::operator<(const CMApatterns& patterns) const
{
    if(m_sector < patterns.sector())
    {
     if (m_cma_parameters->id() < patterns.cma_parameters().id()) return true;
    }
    return false;
}

void
CMApatterns::load_digit(const RPCdigit* digit)
{
    int station = digit->decoding().lvl1_station();
    int cabling = digit->decoding().cabling_code();

    CMAinput IO = NoInput;
    int channel = 0;
    int layer   = 0;

    if(m_cma_parameters->give_connection(station,cabling,IO,layer,channel))
    {
        if(IO == Pivot)
        {
            if(layer) m_pivot1.set(digit,channel);
            else      m_pivot0.set(digit,channel);
        }
        else if (IO == LowPt)
        {
            if(layer) m_lowPt1.set(digit,channel);
            else      m_lowPt0.set(digit,channel);
        }
        else if (IO == HighPt)
        {
            if(layer) m_highPt1.set(digit,channel);
            else      m_highPt0.set(digit,channel);
        }
    }
}

bitPATTERN
CMApatterns::decluster( const bitPATTERN& data) const
{

    bitPATTERN pattern(data);

    for(int i=0;i<pattern.size();++i)
    {
	if(pattern.test(i))
	{
	    int cluster_size = 0;
            do { ++cluster_size; } while( pattern.test(i+cluster_size) );

            int decl_lower = 1;
            int decl_upper = cluster_size;

            switch (cluster_size%2)
	    {
	        case 0:
		for( ;decl_lower+1 < decl_upper && decl_lower<=3; ++decl_lower)
                {
		    pattern.unset(i - 1 + decl_lower);
                    pattern.unset(i - 1 + decl_upper);
                    --decl_upper;
		}
 	        case 1:
                for( ;decl_lower+1 < decl_upper && decl_lower<=3; ++decl_lower)
                { 
                    pattern.unset(i - 1 + decl_lower);
                    pattern.unset(i - 1 + decl_upper);
                    --decl_upper;
		}
	    }
	    i += (cluster_size - 1);
	}
    }
    return pattern;
}


bitPATTERN
CMApatterns::majority(bitPATTERN& layer0,bitPATTERN& layer1) const
{

    int left  = 1;
    int right = 1;

    bitPATTERN result(layer0.name(),layer0.id(),layer0.size());
    bitPATTERN tmp1(layer1);
    bitPATTERN tmp2(layer1);

    result |= layer0 & layer1;
    for(int i=0;i<left;++i)  result |= layer0 & (tmp1<<1);
    for(int i=0;i<right;++i) result |= layer0 & (tmp2>>1);

    for (int i=0;i<result.size();++i)
    {
	if(result.test(i))
	{
	    layer0.unset(i);
	    layer0.unset(i-1);
	    layer0.unset(i+1);
	    layer1.unset(i);
	    layer1.unset(i-1);
	    layer1.unset(i+1);
	}
    }

    return result;
}

void 
CMApatterns::update_distance(const TRIGdistance& NEW,TRIGdistance& OLD) const
{
    if(fabsf(NEW.first) < fabsf(OLD.first)) OLD = NEW;
} 

void 
CMApatterns::coincidence_distance(TRIGdistance& lowTr,TRIGdistance& highTr) 
const
{

    lowTr  = no_distance;
    highTr = no_distance;

    bitPATTERN pivot0  = decluster(m_pivot0);
    bitPATTERN pivot1  = decluster(m_pivot1);
    bitPATTERN lowPt0  = decluster(m_lowPt0);
    bitPATTERN lowPt1  = decluster(m_lowPt1);
    bitPATTERN highPt0 = decluster(m_highPt0);
    bitPATTERN highPt1 = decluster(m_highPt1);

    bitPATTERN pivot2  = majority(pivot0,pivot1);
    bitPATTERN lowPt2  = majority(lowPt0,lowPt1);
    bitPATTERN highPt2 = majority(highPt0,highPt1);

    update_distance( lowest_distance(pivot2,lowPt2) , lowTr );
    update_distance( lowest_distance(pivot2,lowPt0) , lowTr );
    update_distance( lowest_distance(pivot2,lowPt1) , lowTr );
    update_distance( lowest_distance(pivot0,lowPt2) , lowTr );
    update_distance( lowest_distance(pivot1,lowPt2) , lowTr ); 

    update_distance( lowest_distance(pivot2,highPt2) , highTr );
    update_distance( lowest_distance(pivot2,highPt0) , highTr );
    update_distance( lowest_distance(pivot2,highPt1) , highTr );
    update_distance( lowest_distance(pivot0,highPt2) , highTr );
    update_distance( lowest_distance(pivot1,highPt2) , highTr ); 
}

const CMApatterns::TRIGdistance
CMApatterns::lowest_distance(const bitPATTERN& pivot,const bitPATTERN& confirm)
const
{
    if ( !pivot.any() || !confirm.any() ) return no_distance;
 
    TRIGdistance value = no_distance;

    for(int i=0;i<pivot.size();++i)
    {
        if(const RPCdigit* pivot_digit = pivot[i])
	{
	    for(int j=0;j<confirm.size();++j)
	    {
		if(const RPCdigit* confirm_digit = confirm[j])
		{
		    update_distance(distance(pivot_digit,confirm_digit),value);
		}
	    }
	}
    }

    return value;
}

const CMApatterns::TRIGdistance
CMApatterns::distance(const RPCdigit* pivot,const RPCdigit* confirm) const
{


    TRIGdistance value = no_distance;
    float pivrad = 0;
    float conrad = 0;
    if( pivot->station_radius(pivrad) && confirm->station_radius(conrad) )
    {
        if(pivot->decoding().view() == Eta)
        {
	    if(pivot->station_eta(value.second))
	    {
                float Zprime = pivot->z()*(conrad/pivrad);
                value.first  = confirm->z() - Zprime;
	    }
        }
        else if (pivot->decoding().view() == Phi)
        {
            float pivphi = 0;
            float conphi = 0;
            float staphi = 0;
            if( pivot->phi(pivphi)         && 
                confirm->phi(conphi)       && 
                pivot->station_phi(staphi) &&
                pivot->station_eta(value.second) )
            {
                float Sprime = tan(pivphi-staphi)*conrad;
                value.first  = tan(conphi-staphi)*conrad - Sprime;
            } 
        }
    }
    return value;
}

const CMAtrigger
CMApatterns::give_trigger(Lvl1RPCalgo type) const
{



    if(type == HardwareEmulation)
    {
        return CMAtrigger(type,this);
    }
    else
    {
        TrigSigns signatures;
	search_for_lowPt(type,signatures);
	if(!signatures.empty())
	{
	    if(signatures.top().trigger().searchForHigh()) 
	                                   search_for_highPt(type,signatures);
	    return signatures.top();
	}
        else return CMAtrigger(type,this);
    }
}

void
CMApatterns::search_for_lowPt(Lvl1RPCalgo type,TrigSigns& signatures) const
{        


    bitPATTERN pivot0  = decluster(m_pivot0);
    bitPATTERN pivot1  = decluster(m_pivot1);
    bitPATTERN lowPt0  = decluster(m_lowPt0);
    bitPATTERN lowPt1  = decluster(m_lowPt1);
    
    bitPATTERN pivot2  = majority(pivot0,pivot1);
    bitPATTERN lowPt2  = majority(lowPt0,lowPt1);    
    
    search_signatures(type,Low,signatures,pivot2,lowPt2);
    search_signatures(type,Low,signatures,pivot2,lowPt0);
    search_signatures(type,Low,signatures,pivot2,lowPt1);
    search_signatures(type,Low,signatures,pivot0,lowPt2);
    search_signatures(type,Low,signatures,pivot1,lowPt2);
}

void
CMApatterns::search_for_highPt(Lvl1RPCalgo type,TrigSigns& signatures) const
{


    bitPATTERN highPt0 = decluster(m_highPt0);
    bitPATTERN highPt1 = decluster(m_highPt1);
    
    bitPATTERN highPt2 = majority(highPt0,highPt1);   
    
    TrigSigns tmp;
    
    while(!signatures.empty())
    {
        CMAtrigger signature = signatures.top();
	signatures.pop();
	if(signature.trigger().searchForHigh())
	{
	    search_signatures(type,High,signature,highPt2);
	    search_signatures(type,High,signature,highPt0);
	    search_signatures(type,High,signature,highPt1);
	    tmp.push(signature);
	}
    }
    
    signatures = tmp;
}


void
CMApatterns::search_signatures(Lvl1RPCalgo algoType,TrigType trigType,
                               TrigSigns& signatures,
                               const bitPATTERN& pivot,
                               const bitPATTERN& confirm) const
{    

    if ( !pivot.any() || !confirm.any() ) return;
     
    for(int i=0;i<pivot.size();++i)
    {
        if(const RPCdigit* pivot_digit = pivot[i])
	{
	    // Start searching for trigger signature
	    CMAtrigger trigger_result(algoType,this);

	    for(int j=0;j<confirm.size();++j)
	    {
		if(const RPCdigit* confirm_digit = confirm[j])
		{		    
		    TRIGdistance d = distance(pivot_digit,confirm_digit);
		    CMAtrigger signature(algoType,this);
		    
		    if(algoType == GeometricAlgo)
	            {
                        signature = trigger_window(d,trigType);
	            }
	            else if (algoType == LogicEmulation)
	            {
	                signature = trigger_threshold(i,j,trigType);
	            }
		    
		    if(trigType == Low)
		    {
                        signature.set_pivot(pivot_digit);
		        signature.set_lowPt(confirm_digit);
	            }
		    else if(trigType == High)
		    {
		        signature.set_highPt(confirm_digit);
		    }
		    signature.set_width(d.first);
		    signature.set_Kpatt(i);
                    trigger_result += signature;
		}

                //if ( trigger_result.trigger().highest(trigType) ) break;  
	    }
	    if(trigger_result.trigger().type()!=None) 
	                                      signatures.push(trigger_result);
	}
    }
}


void
CMApatterns::search_signatures(Lvl1RPCalgo algoType,TrigType trigType,
                               CMAtrigger& trigger_result,
                               const bitPATTERN& confirm) const
{    

    if ( !confirm.any() ) return;
 

    const RPCdigit* pivot_digit = trigger_result.pivot_hit();
    const int Kpatt = trigger_result.Kpatt();
    
    // Start searching for trigger signature
    CMAtrigger signature(algoType,this);

    for(int j=0;j<confirm.size();++j)
    {
        if(const RPCdigit* confirm_digit = confirm[j])
        {
            TRIGdistance d = distance(pivot_digit,confirm_digit);
	    
	    if(algoType == GeometricAlgo)
	    {
                signature = trigger_window(d,trigType);
	    }
	    else if (algoType == LogicEmulation)
	    {
	        signature = trigger_threshold(Kpatt,j,trigType);
	    }
	    
            if(trigType == Low)
            {
                signature.set_pivot(pivot_digit);
		signature.set_lowPt(confirm_digit);
	    }
            else if(trigType == High)
            {
                signature.set_highPt(confirm_digit);
            }
            signature.set_width(d.first);
	    signature.set_Kpatt(Kpatt);
            trigger_result += signature;
        }

        //if ( trigger_result.trigger().highest(trigType) ) break;  
    }
}


CMAtrigger
CMApatterns::find_trigger(const bitPATTERN& pivot,const bitPATTERN& lowPt,
			  const bitPATTERN& highPt) const
{
 

   CMAtrigger trigger_result(GeometricAlgo,this);

    if ( !pivot.any() || !lowPt.any() ) return trigger_result;
 
    for(int i=0;i<pivot.size();++i)
    {
        if(const RPCdigit* pivot_digit = pivot[i])
	{
	    // Start searching for Low Pt trigger signature
	    for(int j=0;j<lowPt.size();++j)
	    {
		if(const RPCdigit* confirm_digit = lowPt[j])
		{
		    TRIGdistance d = distance(pivot_digit,confirm_digit);
		    CMAtrigger signature = trigger_window(d,Low);
                    signature.set_pivot(pivot_digit);
		    signature.set_lowPt(confirm_digit);
                    trigger_result += signature;
		}

                // if Low Pt Third threshold has fired
                if ( trigger_result.trigger().searchForHigh() )
		{            
                    // Start searching for High Pt trigger signature
                    for(int k=0;k<highPt.size();++k)
	            {
		        if(const RPCdigit* confirm_digit = highPt[k])
		        {
		            TRIGdistance d=distance(pivot_digit,confirm_digit);
		            CMAtrigger signature = trigger_window(d,High);
		            signature.set_highPt(confirm_digit);
                            trigger_result += signature;
		        }
                        if( trigger_result.trigger().highest(High) ) break;
		    }
	        }
                if ( trigger_result.trigger().highest(High) ) break;  
	    }
	}
        if ( trigger_result.trigger().highest(High) ) break;
    }

    return trigger_result;
}


CMAtrigger
CMApatterns::find_trigger(const bitPATTERN& pivot,  const bitPATTERN& lowPt,
		    const bitPATTERN& highPt0,const bitPATTERN& highPt1) const
{

    CMAtrigger trigger_result(GeometricAlgo,this);

    if ( !pivot.any() || !lowPt.any() ) return trigger_result;
 
    for(int i=0;i<pivot.size();++i)
    {
        if(const RPCdigit* pivot_digit = pivot[i])
	{
	    // Start searching for Low Pt trigger signature
	    for(int j=0;j<lowPt.size();++j)
	    {
		if(const RPCdigit* confirm_digit = lowPt[j])
		{
		    TRIGdistance d = distance(pivot_digit,confirm_digit);
		    CMAtrigger signature =  trigger_window(d,Low);
                    signature.set_pivot(pivot_digit);
		    signature.set_lowPt(confirm_digit);
                    trigger_result += signature;
		}

                // if Low Pt Third threshold has fired
                if ( trigger_result.trigger().searchForHigh() )
		{            
                    // Start searching for High Pt trigger signature in plane 0
                    for(int k=0;k<highPt0.size();++k)
	            {
		        if(const RPCdigit* confirm_digit = highPt0[k])
		        {
		            TRIGdistance d=distance(pivot_digit,confirm_digit);
		            CMAtrigger signature =  trigger_window(d,High);
		            signature.set_highPt(confirm_digit);
                            trigger_result += signature;
		        }
                        if( trigger_result.trigger().highest(High) ) break;
		    }
	        }

                // if Low Pt Third threshold has fired
                if ( trigger_result.trigger().searchForHigh() )
		{            
                    // Start searching for High Pt trigger signature in plane 1
                    for(int k=0;k<highPt1.size();++k)
	            {
		        if(const RPCdigit* confirm_digit = highPt1[k])
		        {
		            TRIGdistance d=distance(pivot_digit,confirm_digit);
		            CMAtrigger signature =  trigger_window(d,High);
		            signature.set_highPt(confirm_digit);
                            trigger_result += signature;
		        }
                        if( trigger_result.trigger().highest(High) ) break;
		    }
	        }
                if ( trigger_result.trigger().highest(High) ) break;  
	     }
	}
        if ( trigger_result.trigger().highest(High) ) break;  
    }

    return trigger_result;
}

CMAtrigger
CMApatterns::trigger_window(TRIGdistance dist, TrigType type) const
{

    if (type == None) return CMAtrigger(GeometricAlgo,this);

    if(m_cma_parameters->id().type() == Eta)
    {
        //implement the geometric coincidence for the eta projection
        int upper_threshold = (type == Low)? 2 : 4;
        int lower_threshold = (type == Low)? 0 : 3;

        for(int i=upper_threshold; i >= lower_threshold;--i)
        {
	    if (windows(dist.first,dist.second,i,m_sector%32))
	    {
	        Threshold threshold = FirstTh;
                if      ( (i%3) == 0 ) threshold = FirstTh;
                else if ( (i%3) == 1 ) threshold = SecondTh;
                else if ( (i%3) == 2 ) threshold = ThirdTh;
	        return CMAtrigger(GeometricAlgo,type,threshold,this);
	    }
        }
    }
    else if (m_cma_parameters->id().type() == Phi)
    {
        //implement the geometric coincidence for the phi projection
	if(type == Low)
	{
	    // set the trigger window width equal to 10 centimeters
	    if(fabsf(dist.first)<10.0) 
                return CMAtrigger(GeometricAlgo,type,ThirdTh,this);
	}
	else if (type == High)
	{
	    // set the trigger window width equal to 10 centimeters
	    if(fabsf(dist.first)<10.0) 
                return CMAtrigger(GeometricAlgo,type,SecondTh,this); 
	}
    }
    
    return CMAtrigger(GeometricAlgo,this);
}

CMAtrigger
CMApatterns::trigger_threshold(int pivot,int confirm,TrigType type) const
{

    if (type == None) return CMAtrigger(LogicEmulation,this);
    
    const int pivch = CMAparameters::pivot_channels;
    const int conch = CMAparameters::confirm_channels;
    
    const bool* program = 0;
        
    if(type == Low) 
    {
        if(const CMAprogram* prog = cma_parameters().lowPt_program() )
	{
	    // avoid to use the Phi program for the time being ///////////
	    if(cma_parameters().id().type() == Phi)                     //
	        return CMAtrigger(LogicEmulation,type,ThirdTh,this);    //
	    //////////////////////////////////////////////////////////////       
	    program = prog->registers();
	}
	else return CMAtrigger(LogicEmulation,type,ThirdTh,this);
    }
    else
    {
        if(const CMAprogram* prog = cma_parameters().highPt_program() )
	{
	    // avoid to use the Phi program for the time being ///////////
	    if(cma_parameters().id().type() == Phi)                     //
	        return CMAtrigger(LogicEmulation,type,ThirdTh,this);    //
            //////////////////////////////////////////////////////////////
	    program = prog->registers();
	}
	else return CMAtrigger(LogicEmulation,type,ThirdTh,this);
    }
    
    
    for(int th=2;th>=0;--th)
    {
	int pos = th*pivch*conch + pivot*conch + confirm;
	if(*(program+pos))
	{
	    Threshold threshold = static_cast<Threshold>(th);
	    return CMAtrigger(LogicEmulation,type,threshold,this);
	}
    }
    return CMAtrigger(LogicEmulation,this);
}

CMApatterns&
CMApatterns::operator<<(const RPCdigit* digit)
{
    load_digit(digit);
    return *this;
}

void CMApatterns::Print(std::ostream& stream,bool detail) const
{    
    //unsigned int upper = 19;
    //unsigned int lower = 17;

    unsigned int upper = 95;
    unsigned int lower = 45;
    
    for(int i=0;i<80;++i) stream << (char) upper;
    stream << std::endl;

    stream << name() << " giving input to: logic sector = " << m_sector
	   << ", sector type = " << cma_parameters().sector_type() << std::endl;
    stream << cma_parameters() << std::endl;

    //bitPATTERN layer0 = decluster(pivot0());
    //bitPATTERN layer1 (pivot1()); 

    pivot1().Print(stream,detail);
    pivot0().Print(stream,detail);

    //layer0.Print(stream,detail);       // test for declustering
    //stream << majority(layer0,layer1); // test for majority logic
    //layer1.Print(stream,detail);       // test for majority logic
    //layer0.Print(stream,detail);       // test for majority logic

    lowPt1().Print(stream,detail);
    lowPt0().Print(stream,detail);
    highPt1().Print(stream,detail);
    highPt0().Print(stream,detail);   
    
    for(int i=0;i<80;++i) stream << (char) lower;
    stream << std::endl;
}

void
CMApatterns::create_hardware(TrigType type)
{   

    if(type == None) return;

    Matrix* matrix          = 0; 
    int run                 = 0;
    int event               = 0;
    unsigned int long debug = m_debug;
    int subsystem           = (m_sector<32)? 0 : 1;
    int proj                = cma_parameters().id().type();
    int sector              = m_sector%32;
    int PADadd              = cma_parameters().id().PAD_index();
    int lohi                = (type == Low) ? 0 : 1;
    int address[2]          = { cma_parameters().id().eta_index(),
                                cma_parameters().id().phi_index()  };
    int localAdd            = cma_parameters().id().Ixx_index();
     

    // Instanciate the Matrix

    matrix = new Matrix(run,event,debug,subsystem,proj,sector,PADadd,lohi,
                        address,localAdd);

    ////////////////////////////////////////////////////////////////////////
    // ****************** START PROGRAMMING THE MATRIX ****************** //
    ////////////////////////////////////////////////////////////////////////


    // Loading pivot data into Matrix
    // load_data(0,0,m_pivot0,matrix);
    // load_data(0,1,m_pivot1,matrix);


    // Specify the readout threshold to be transmitted into the readout
    matrix->setKReadOut(0);

    // Specify the AND/OR logic for a matrix input (0=pivot, 1=coincidence)
    matrix->setLocalDirection(0,7);
    matrix->setLocalDirection(1,7);

    // Specify the width of the pulse signal in Matrix clock thick (320 Mhz)
    matrix->setPulseWidth(4);

    if(type == Low) 
    {
      // Loading pivot data into Matrix
      load_data(0,0,m_pivot0,matrix);
      load_data(0,1,m_pivot1,matrix);
      
      // Load the theshold registers into Matrix hardware simulation
      const CMAprogram* program = cma_parameters().lowPt_program();
      const int pivot_ch = CMAparameters::pivot_channels;
      
      CMAword ovl1 = 0x0;
      CMAword ovl2 = 0x0;


      if(program)
	{
	  if(cma_parameters().id().type() == Phi)
	    {
	      const uint32_t* bytes = program->bytes();

	      for(int th=0;th<3;++th)                 // loop over thresholds
		for (int ch=0; ch<pivot_ch; ++ch)   // loop over channels
		  for (int by=0;by<2;++by)        // loop over bytes
		    {
                     // G.Bruni, M.Corradi 1/3/2010 set phi roads for new cabling
                     if (cma_parameters().conf_type() != CMAparameters::Atlas){
                         matrix->setRoad(th,ch,by,0xffffffff); // old cabling, keep the phi roads all open
                     } else {
                         int pos = th*pivot_ch*2 + ch*2 + by;
                         matrix->setRoad(th,ch,by,bytes[pos]);
		     }
		    }
	    }
	  else
	    {

	      const uint32_t* bytes = program->bytes();
	      
	      for(int th=0;th<3;++th)                 // loop over thresholds
		for (int ch=0; ch<pivot_ch; ++ch)   // loop over channels
		  for (int by=0;by<2;++by)        // loop over bytes
		    {
		      int pos = th*pivot_ch*2 + ch*2 + by;
		      matrix->setRoad(th,ch,by,bytes[pos]);
		    }
	    }
	  
	  ovl1 = program->overlap1();
	  ovl2 = program->overlap2();
	  
        }
      else if (!program && cma_parameters().id().type() == Phi)
	{
	  // open only the Phi matrix if no program is found
	  
	  for(int th=0;th<3;++th)                 // loop over thresholds
	    for (int ch=0; ch<pivot_ch; ++ch)   // loop over channels
	      for (int by=0;by<2;++by)        // loop over bytes
		{
		  matrix->setRoad(th,ch,by,0xffffffff);
		}
	}
      
      // Load the Majority logic setup
      matrix->setMajority(0,2);
      matrix->setMajority(1,2);
      matrix->setMajority(2,2);
      
      //matrix->setMask1(0,1);
      
      // Load the Overlap flag
      matrix->setMatOverlap(0, ovl1);
      matrix->setMatOverlap(1, ovl2);
      
      // Load Low Pt plane data into Matrix
      load_data(1,0,m_lowPt0,matrix);
      load_data(1,1,m_lowPt1,matrix);
      
      // Execute the matrix simulation
      matrix->execute(); 
      m_low_pt_matrix = matrix;
    }
    else 
      {
	// Load the theshold registers into Matrix hardware simulation
        const CMAprogram* program = cma_parameters().highPt_program();
        const int pivot_ch = CMAparameters::pivot_channels;
	
        CMAword ovl1 = 0x0;
	CMAword ovl2 = 0x0;

	
        if(program)
	  {

	    if(cma_parameters().id().type() == Phi)
	      {
                 const uint32_t* bytes = program->bytes();

                 for(int th=0;th<3;++th)                 // loop over thresholds
		  for (int ch=0; ch<pivot_ch; ++ch)   // loop over channels
		    for (int by=0;by<2;++by)        // loop over bytes
                    {
                        // G.Bruni, M.Corradi 1/3/2010 set phi roads for new cabling
                       if (cma_parameters().conf_type()!= CMAparameters::Atlas){
                           matrix->setRoad(th,ch,by,0xffffffff); // old cabling, keep the phi roads all open
                       } else {
                           int pos = th*pivot_ch*2 + ch*2 + by;
                           matrix->setRoad(th,ch,by,bytes[pos]);
                       }
		      }
	      }
	    else
	      {
	        const uint32_t* bytes = program->bytes();
                for(int th=0;th<3;++th)                 // loop over thresholds
		  for (int ch=0; ch<pivot_ch; ++ch)   // loop over channels
		    for (int by=0;by<2;++by)        // loop over bytes
		      {
			int pos = th*pivot_ch*2 + ch*2 + by;
			matrix->setRoad(th,ch,by,bytes[pos]);
		      }
	      }
	    
            ovl1 = program->overlap1();
	    ovl2 = program->overlap2();

	  }
        else if (!program && cma_parameters().id().type() == Phi)
	  {

            // open only the Phi matrix if no program is found
	    
            for(int th=0;th<3;++th)                 // loop over thresholds
	      for (int ch=0; ch<pivot_ch; ++ch)   // loop over channels
		for (int by=0;by<2;++by)        // loop over bytes
		  {
		    matrix->setRoad(th,ch,by,0xffffffff);
		  }
	  }
        // Load the Majority logic setup
        matrix->setMajority(0,1);
        matrix->setMajority(1,1);
        matrix->setMajority(2,1);
	
	// Load the Overlap flag
	matrix->setMatOverlap(0, ovl1);
	matrix->setMatOverlap(1, ovl2);

        // Load High Pt plane data into Matrix
        load_data(1,0,m_highPt0,matrix);
        load_data(1,1,m_highPt1,matrix); 

        if(!m_low_pt_matrix) create_hardware(Low);

        // Put the Low Pt patterns into the High Pt Matrix input
        matrix->putPatt(m_low_pt_matrix);

        // Execute the matrix simulation
        matrix->execute();
        m_high_pt_matrix = matrix;
    }
}

void
CMApatterns::load_data(int side,int layer,const bitPATTERN& pattern,
                       Matrix* mat)
{
  
  if(pattern.any())
    {
      for (int i=0;i<pattern.size();++i)
	{
	  if(pattern.test(i)) mat->putData(side,layer,i,pattern[i]->time());
	}
    }
}

Matrix* 
CMApatterns::give_low_pt_matrix(void)
{
  
  if(!m_low_pt_matrix) create_hardware(Low);
  return m_low_pt_matrix;
}


Matrix* 
CMApatterns::give_high_pt_matrix(void)
{
  
  if(!m_high_pt_matrix) create_hardware(High);
  return m_high_pt_matrix;
}
